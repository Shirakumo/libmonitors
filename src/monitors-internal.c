#include <stdlib.h>
#include "monitors-internal.h"

MONITOR *alloc_monitors(int count){
  return calloc(count, sizeof(MONITOR));
}

MODE *alloc_modes(int count){
  return calloc(count, sizeof(MODE));
}

void free_monitor(MONITOR *monitor){
  for(int i=0; i<monitor->mode_count; ++i){
    if(monitor->modes[i])
      free(monitor->modes[i]);
  }
  free(monitor->name);
  free(monitor);
}
