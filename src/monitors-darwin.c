#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <Carbon/Carbon.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <CoreVideo/CVBase.h>
#include <CoreVideo/CVDisplayLink.h>
#include "monitors.h"
#include "monitors-internal.h"

MODE_DATA{
};

MONITOR_DATA{
  CGDirectDisplayID display_id;
  uint32_t unit_number;
};

char *copy_str(char *string){
  char *copy = NULL;
  int count = 0;
  while(string[count] != 0) ++count;
  
  copy = calloc(count, sizeof(char));
  for(; count>=0; --count) copy[count]=string[count];
  return copy;
}

MONITORS_EXPORT bool libmonitors_detect(int *ext_count, MONITOR ***ext_monitors){  
  MONITOR **monitors = NULL;
  int count = 0;
  
  
  *ext_monitors = monitors;
  *ext_count = count;
  return true;
}

MONITORS_EXPORT bool libmonitors_make_mode_current(MODE *mode){
  if(mode->monitor->current_mode != mode){
    int success = false;

    return success;
  }
  
  return true;
}

MONITORS_EXPORT bool libmonitors_init(){
  return true;
}

MONITORS_EXPORT void libmonitors_deinit(){
}
