#ifndef __LIBMONITORS_INTERNAL_H__
#define __LIBMONITORS_INTERNAL_H__
#include "monitors.h"

MONITOR **alloc_monitors(int count);
MONITOR *alloc_monitor(int data);
MODE *alloc_modes(int count);
bool is_duplicate_mode(MODE *mode, int count, MODE *modes);

#endif
