#include <stdlib.h>
#include "monitors-internal.h"

MONITOR *alloc_monitors(int count){
  return calloc(count, sizeof(MONITOR));
}

MODE *alloc_modes(int count){
  return calloc(count, sizeof(MODE));
}

void libmonitors_free_monitors(int count, MONITOR *monitors){
  for(int i=0; i<count; ++i){
    free(monitors[i].modes);
    free(monitors[i].name);
  }
  free(monitors);
}
