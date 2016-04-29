#ifndef __LIBMONITORS_H__
#define __LIBMONITORS_H__
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MSC_VER
#  if _MSC_VER <= 1600
#    define bool int
#    define true 1
#    define false 0
#  else
#    include <stdbool.h>
#  endif
#else
#  include <stdbool.h>
#endif
  
#include "export.h"
  struct libmonitors_monitor_data;
  struct libmonitors_mode_data;
  struct libmonitors_monitor;

  MONITORS_EXPORT struct libmonitors_mode{
    struct libmonitors_monitor *monitor;
    int width;
    int height;
    int refresh;
    struct libmonitors_mode_data *_data;
  };
  
  MONITORS_EXPORT struct libmonitors_monitor{
    char *name;
    bool primary;
    int width;
    int height;
    int mode_count;
    struct libmonitors_mode *current_mode;
    struct libmonitors_mode *modes;
    struct libmonitors_monitor_data *_data;
  };

  MONITORS_EXPORT bool libmonitors_init();
  MONITORS_EXPORT void libmonitors_deinit();
  MONITORS_EXPORT bool libmonitors_detect(int *count, struct libmonitors_monitor ***monitors);
  MONITORS_EXPORT bool libmonitors_make_mode_current(struct libmonitors_mode *mode);
  MONITORS_EXPORT void libmonitors_free_monitor(struct libmonitors_monitor *monitor);
  MONITORS_EXPORT void libmonitors_free_monitors(int count, struct libmonitors_monitor **monitors);

#ifdef __cplusplus
}
#endif
#endif
