#ifndef __LIBMONITORS_INTERNAL_H__
#define __LIBMONITORS_INTERNAL_H__
#include "monitors.h"

#define MONITOR struct libmonitors_monitor
#define MONITOR_DATA struct libmonitors_monitor_data
#define MODE struct libmonitors_mode
#define MODE_DATA struct libmonitors_mode_data

MONITOR **alloc_monitors(int count);
MONITOR *alloc_monitor(int data);
MODE *alloc_modes(int count);
bool is_duplicate_mode(MODE *mode, int count, MODE *modes);

#endif
