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
#define MODE struct libmonitors_mode

  MONITORS_EXPORT MODE{
    int width;
    int height;
    int refresh;
  };
  
  MONITORS_EXPORT MONITOR{
    char *name;
    bool primary;
    int width;
    int height;
    int mode_count;
    MODE current_mode;
    MODE** modes;
  };

  MONITORS_EXPORT bool libmonitors_init();
  MONITORS_EXPORT void libmonitors_deinit();
  MONITORS_EXPORT bool libmonitors_detect(int *count, MONITOR **monitors);
  
#ifdef __cplusplus
}
#endif
#endif
