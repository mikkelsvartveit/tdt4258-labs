// Learning resources:
// - https://kevinboone.me/linuxfbc.html
// - Documentation mentioned in handout PDF

#include <fcntl.h>
#include <linux/fb.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

void set_pixel(u_int16_t fbdata[], u_int8_t x_pos, u_int8_t y_pos, u_int8_t red,
               u_int8_t green, u_int8_t blue) {
  int index = y_pos * 8 + x_pos;

  u_int16_t pixel_value = ((red & (1 << 5) - 1) << 11) +
                          ((green & (1 << 6) - 1) << 5) + (blue & (1 << 5) - 1);

  fbdata[index] = pixel_value;
}

int main() {
  int fd = -1;

  bool found = false;

  for (int i = 0; i < 32; i++) {
    char device_string[12];
    sprintf(device_string, "/dev/fb%d", i);
    fd = open(device_string, O_RDWR);

    if (fd < 0) {
      continue;
    }

    struct fb_fix_screeninfo finfo;
    ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
    if (strcmp(finfo.id, "RPi-Sense FB") != 0) {
      continue;
    }

    printf("Found SenseHat framebuffer on /dev/fb%d\n", i);
    found = true;
    break;
  }

  if (!found) {
    printf("Sensehat framebuffer not found\n");
    return 0;
  }

  struct fb_var_screeninfo vinfo;
  ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);

  int fb_width = vinfo.xres;
  int fb_height = vinfo.yres;
  int fb_bpp = vinfo.bits_per_pixel;
  int fb_bytes = fb_bpp / 8;
  int fb_data_size = fb_width * fb_height * fb_bytes;

  u_int16_t *fbdata =
      mmap(0, fb_data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);

  memset(fbdata, 0, fb_data_size);

  for (int i = 0; i < 8; i++) {
    set_pixel(fbdata, i, 0, 0x10, 0, 0);
    set_pixel(fbdata, i, 1, 0x10, 0x10, 0);
    set_pixel(fbdata, i, 2, 0x10, 0x20, 0);
    set_pixel(fbdata, i, 3, 0x00, 0x20, 0);
    set_pixel(fbdata, i, 4, 0, 0, 0x10);
    set_pixel(fbdata, i, 5, 0x08, 0, 0x10);
    set_pixel(fbdata, i, 6, 0x10, 0, 0x10);
    set_pixel(fbdata, i, 7, 0x10, 0, 0);
  }

  set_pixel(fbdata, 3, 3, 0xff, 0xff, 0xff);

  munmap(fbdata, fb_data_size);
  close(fd);
}
