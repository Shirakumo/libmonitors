#ifndef __MONITORS_EXPORT_H__
#define __MONITORS_EXPORT_H__

#ifdef _MSC_VER
#  ifdef MONITORS_STATIC_DEFINE
#    define MONITORS_EXPORT
#  else
#    ifndef MONITORS_EXPORT
#      define MONITORS_EXPORT __declspec(dllexport)
#    endif
#  endif
#else
#  define MONITORS_EXPORT
#endif

#endif
