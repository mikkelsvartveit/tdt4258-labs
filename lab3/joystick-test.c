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
    if (strcmp(device_name, "Raspberry Pi Sense HAT Joystick") != 0) {
      continue;
    }

    js_found = true;
    break;
  }

  if (!js_found) {
    printf("Sensehat joystick not found\n");
    return 0;
  }

  // Check for joystick events
  struct pollfd evpoll = {.events = POLLIN, .fd = jsfd};
  while (1) {
    while (poll(&evpoll, 1, 0) > 0) {
      struct input_event ev;
      read(jsfd, &ev, sizeof(struct input_event));
      if (ev.type == EV_KEY && ev.value == 1) {
        printf("Key: %d\n", ev.code);
      }
    }
  }
}
