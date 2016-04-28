#include <stdlib.h>
#include "monitors.h"
#include "monitors-internal.h"

MONITOR **alloc_monitors(int count){
  return calloc(count, sizeof(MONITOR*));
}

MONITOR *alloc_monitor(int data){
  MONITOR *monitor = calloc(1, sizeof(MONITOR));
  monitor->_data = calloc(1, data);
  return monitor;
}

MODE *alloc_modes(int count){
  return calloc(count, sizeof(MODE));
}

void free_modes(int count, MODE *modes){
  for(int i=0; i<count; ++i){
    if(modes[i]._data != NULL)
      free(modes[i]._data);
  }
  free(modes);
}

MONITORS_EXPORT void libmonitors_free_monitor(MONITOR *monitor){
  if(monitor->name != NULL)
    free(monitor->name);
  if(monitor->_data != NULL)
    free(monitor->_data);
  if(monitor->modes != NULL)
    free_modes(monitor->mode_count, monitor->modes);
  free(monitor);
}

MONITORS_EXPORT void libmonitors_free_monitors(int count, MONITOR **monitors){
  for(int i=0; i<count; ++i){
    libmonitors_free_monitor(monitors[i]);
  }
  free(monitors);
}

bool is_duplicate_mode(MODE *mode, int count, MODE *modes){
  for(int i=0; i<count; ++i){
    if(&modes[i] != mode
       && modes[i].width == mode->width
       && modes[i].height == mode->height
       && modes[i].refresh == mode->refresh)
      return true;
  }
  return false;
}
