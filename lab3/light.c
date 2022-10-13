// Learning resources:
// - https://kevinboone.me/linuxfbc.html
// - Documentation mentioned in handout PDF

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/types.h>

void set_pixel(u_int16_t fbdata[], u_int8_t x_pos, u_int8_t y_pos, u_int8_t red, u_int8_t green, u_int8_t blue) {
  int index = y_pos * 8 + x_pos;

  u_int16_t pixel_value = ((red & (1 << 5) - 1) << 11) + ((green & (1 << 6) - 1) << 5) + (blue & (1 << 5) - 1);

  fbdata[index] = pixel_value;
}

int main() {
  int fd = open("/dev/fb0", O_RDWR);
  
  if (fd >= 0) {
    struct fb_var_screeninfo vinfo;

    ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);

    int fb_width = vinfo.xres;
    int fb_height = vinfo.yres;
    int fb_bpp = vinfo.bits_per_pixel;
    int fb_bytes = fb_bpp / 8;
    int fb_data_size = fb_width * fb_height * fb_bytes;

    u_int16_t *fbdata = mmap(0, fb_data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t) 0);

    memset(fbdata, 0, fb_data_size);

    set_pixel(fbdata, 3, 3, 0xff, 0, 0xff);

    munmap(fbdata, fb_data_size);
    close(fd);
  }
}

