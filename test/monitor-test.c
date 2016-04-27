#include <stdio.h>
#include "../src/monitors.h"

void print_mode(struct libmonitors_mode *mode){
  printf("%ix%i @ %i Hz", mode->width, mode->height, mode->refresh);
  if(mode == mode->monitor->current_mode)
    printf(" CURRENT");
  printf("\n");
}

void print_monitor(struct libmonitors_monitor *monitor){
  printf("Monitor %s (%ix%i)", monitor->name, monitor->width, monitor->height);
  if(monitor->primary) printf(" PRIMARY");

  printf("\n");
  for(int i=0; i<monitor->mode_count; ++i){
    printf("  %i: ", i);
    print_mode(&monitor->modes[i]);
  }
}

int main(){
  if(!libmonitors_init())
    return 1;

  int count;
  struct libmonitors_monitor *monitors;
  if(!libmonitors_detect(&count, &monitors))
    return 2;

  printf("Detected %i monitors:\n", count);
  for(int i=0; i<count; ++i){
    print_monitor(&monitors[i]);
  }

  libmonitors_free_monitors(count, monitors);
  libmonitors_deinit();
  return 0;
}
