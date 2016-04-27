#include <stdlib.h>
#include "monitors-internal.h"

MONITOR *alloc_monitors(int count){
  return calloc(count, sizeof(MONITOR));
}

MODE *alloc_modes(int count){
  return calloc(count, sizeof(MODE));
}

void free_monitor(MONITOR *monitor){
  free(monitor->modes);
  free(monitor->name);
  free(monitor);
}
