#include <stdlib.h>
#include "monitors-internal.h"

MONITOR *alloc_monitors(int count){
  return calloc(count, sizeof(MONITOR));
}

MODE *alloc_modes(int count){
  return calloc(count, sizeof(MODE));
}

void libmonitors_free_modes(int count, MODE *modes){
  for(int i=0; i<count; ++i){
    free(modes[i]._data);
  }
  free(modes);
}

void libmonitors_free_monitors(int count, MONITOR *monitors){
  for(int i=0; i<count; ++i){
    free(monitors[i].name);
    free(monitors[i]._data);
    libmonitors_free_modes(monitors[i].mode_count, monitors[i].modes);
  }
  free(monitors);
}
