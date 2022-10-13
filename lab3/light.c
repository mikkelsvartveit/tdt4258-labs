#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/types.h>

struct Row {
  u_int16_t pixel[8];
};

struct Screen {
  struct Row row[8];
};

int get_pixel_value(int x, int y) {
  return (y * 8 + x);
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

    fbdata[get_pixel_value(4, 4)] = 0b0000011111111111;
  }
}
