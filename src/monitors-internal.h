#ifndef __LIBMONITORS_INTERNAL_H__
#define __LIBMONITORS_INTERNAL_H__
#include "monitors.h"

MONITOR *alloc_monitors(int count);
MODE *alloc_modes(int count);
void free_monitor(MONITOR *monitor);

#endif
