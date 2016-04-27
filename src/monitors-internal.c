#include <stdlib.h>
#include "monitors.h"
#include "monitors-internal.h"

MONITOR *alloc_monitors(int count){
  return calloc(count, sizeof(MONITOR));
}

MODE *alloc_modes(int count){
  return calloc(count, sizeof(MODE));
}

void libmonitors_free_modes(int count, MODE *modes){
  for(int i=0; i<count; ++i){
    if(modes[i]._data != NULL)
      free(modes[i]._data);
  }
  free(modes);
}

MONITORS_EXPORT void libmonitors_free_monitors(int count, MONITOR *monitors){
  for(int i=0; i<count; ++i){
    if(monitors[i].name != NULL)
      free(monitors[i].name);
    if(monitors[i]._data != NULL)
      free(monitors[i]._data);
    if(monitors[i].modes != NULL)
      libmonitors_free_modes(monitors[i].mode_count, monitors[i].modes);
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
