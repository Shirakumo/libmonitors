#include <stdio.h>
#include <stdlib.h>
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

int main(int argc, char *argv[]){
  if(!libmonitors_init()){
    fputs("Failed to connect to X server\n", stderr);
    return 1;
  }

  int count;
  struct libmonitors_monitor *monitors;
  if(!libmonitors_detect(&count, &monitors)){
    fputs("Failed to detect monitors\n", stderr);
    return 2;
  }

  if(argc == 3){
    int monitor = atoi(argv[1]);
    int mode = atoi(argv[2]);
    if(count <= monitor
       || monitors[monitor].mode_count <= mode){
      fputs("Invalid monitor/mode spec\n", stderr);
      return 3;
    }
    
    if(libmonitors_make_mode_current(&monitors[monitor].modes[mode])){
      printf("Switched mode.\n");
    }else{
      fputs("Failed to switch mode.\n", stderr);
      return 4;
    }
  }

  printf("Detected %i monitors:\n", count);
  for(int i=0; i<count; ++i){
    print_monitor(&monitors[i]);
  }

  libmonitors_free_monitors(count, monitors);
  libmonitors_deinit();
  return 0;
}
