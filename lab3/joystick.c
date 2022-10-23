#include <fcntl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

int main() {
  int jsfd = -1;
  bool js_found = false;

  for (int i = 0; i < 32; i++) {
    char device_string[12];
    sprintf(device_string, "/dev/input/event%d", i);
    jsfd = open(device_string, O_RDWR);

    if (jsfd < 0) {
      continue;
    }

    char device_name[128] = {'\0'};
    ioctl(jsfd, EVIOCGNAME(128), &device_name);
    printf("%s\n", device_name);
    if (strcmp(device_name, "RPi-Sense FB") != 0) {
      continue;
    }
  }
}
