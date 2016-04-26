#include <stdio.h>
#include "../src/monitors.h"

void print_mode(struct libmonitors_mode *mode){
  printf("%ix%i (%i Hz)", mode->width, mode->height, mode->refresh);
}

void print_monitor(struct libmonitors_monitor *monitor){
  printf("Monitor %s (%ix%i)", monitor->name, monitor->width, monitor->height);
  if(monitor->primary) printf(" PRIMARY");

  for(int i=0; i<monitor->mode_count; ++i){
    printf("\n  %i: ");
    print_mode(monitor->modes[i]);
  }
}

int main(){
  if(!libmonitors_init())
    return 1;

  int count;
  struct libmonitors_monitor *monitors;
  libmonitors_detect(&count, &monitors);

  for(int i=0; i<count; ++i){
    print_monitor(&monitors[i]);
  }

  libmonitors_deinit();
  return 0;
}
