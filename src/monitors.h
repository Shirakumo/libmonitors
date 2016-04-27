#ifndef __LIBMONITORS_H__
#define __LIBMONITORS_H__
#ifdef __cplusplus
extern "C" {
#endif

#if _MSC_VER <= 1600
#define bool int
#define true 1
#define false 0
#else
#include <stdbool.h>
#endif
#include "export.h"
#define MONITOR struct libmonitors_monitor
#define MONITOR_DATA struct libmonitors_monitor_data
#define MODE struct libmonitors_mode
#define MODE_DATA struct libmonitors_mode_data

  MONITOR_DATA;
  MODE_DATA;
  MONITORS_EXPORT MONITOR;

  MONITORS_EXPORT MODE{
    MONITOR *monitor;
    int width;
    int height;
    int refresh;
    MODE_DATA *_data;
  };
  
  MONITORS_EXPORT MONITOR{
    char *name;
    bool primary;
    int width;
    int height;
    int mode_count;
    MODE *current_mode;
    MODE *modes;
    MONITOR_DATA *_data;
  };

  MONITORS_EXPORT bool libmonitors_init();
  MONITORS_EXPORT void libmonitors_deinit();
  MONITORS_EXPORT bool libmonitors_detect(int *count, MONITOR **monitors);
  MONITORS_EXPORT bool libmonitors_make_mode_current(MODE *mode);
  MONITORS_EXPORT void libmonitors_free_monitors(int count, MONITOR *monitors);

#ifdef __cplusplus
}
#endif
#endif
