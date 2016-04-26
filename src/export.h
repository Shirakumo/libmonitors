#ifndef MONITORS_EXPORT_H
#define MONITORS_EXPORT_H

#ifdef _MSC_VER
#  ifdef MONITORS_STATIC_DEFINE
#    define MONITORS_EXPORT
#    define MONITORS_NO_EXPORT
#  else
#    ifndef MONITORS_EXPORT
#      ifdef stem_gamepad_EXPORTS
#        define MONITORS_EXPORT __declspec(dllexport)
#      else
#        define MONITORS_EXPORT __declspec(dllimport)
#      endif
#    endif
#  endif
#else
#  define MONITORS_EXPORT
#  define MONITORS_NO_EXPORT
#endif

#endif
