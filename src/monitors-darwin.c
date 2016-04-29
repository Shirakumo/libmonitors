#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <Carbon/Carbon.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <CoreVideo/CVBase.h>
#include <CoreVideo/CVDisplayLink.h>
#include "monitors.h"
#include "monitors-internal.h"

MODE_DATA{
};

MONITOR_DATA{
  CGDirectDisplayID display_id;
  uint32_t unit_number;
};

char *copy_str(char *string){
  char *copy = NULL;
  int count = 0;
  while(string[count] != 0) ++count;
  
  copy = calloc(count, sizeof(char));
  for(; count>=0; --count) copy[count]=string[count];
  return copy;
}

bool process_mode(MODE *mode, CGDisplayModeRef display_mode, CVDisplayLinkRef display_link){
  bool result = false;
  
  uint32_t mode_flags = CGDisplayModeFlags(display_mode);
  if((flags & kDisplayModeValidFlag)
     && (flags & kDisplayModeSafeFlag)
     && !(flags & kDisplayModeInterlacedFlag)
     && !(flags & kDisplayModeStretchedFlag)){

    CFStringRef format = CGDisplayModeCopyPixelEncoding(display_mode);
    if(!CFStringCompare(format, CFSTR(IO16BitDirectPixels), 0)
       || !CFStringCompare(format, CFSTR(IO32BitDirectPixels), 0)){

      mode->width = (int)CGDisplayModeGetWidth(display_mode);
      mode->height = (int)CGDisplayModeGetHeight(display_mode);
      mode->refresh = (int)CGDisplayModeGetRefreshRate(display_mode);

      // Attempt to recover by calculation if possible
      if(mode->refresh == 0){
        const CVTime time = CVDisplayLinkGetNominalOutputVideoRefreshPeriod(display_link);
        if(!(time.flags & kCVTimeIsIndefinite)){
          mode->refresh = (int)(time.timeScale / (double)time.timeValue);
        }
      }
      result = true;
    }
    CFRelease(format);
  }
  
  return result;
}

void detect_modes(MONITOR *monitor){
  int count = 0;
  MODE *modes = NULL;

  CVDisplayLinkRef display_link;
  CVDisplayLinkCreateWithCGDisplay(monitor->_data->display_id, &display_link);
  CFArrayRef display_modes = CGDisplayCopyAllDisplayModes(monitor->_data->display_id, NULL);
  int mode_count = CFArrayGetCount(display_modes);
  
  modes = alloc_modes(mode_count);
  for(int i=0; i<mode_count; ++i){
    CGDisplayModeRef display_mode = (CGDisplayModeRef)CFArrayGetValueAtIndex(display_modes, i);
    modes[count].monitor = monitor;
    if(process_mode(&modes[count], display_mode, display_link)){
      ++count;
    }
  }

  CFRelease(display_modes);
  CVDisplayLinkRelease(display_link);

  monitor->mode_count = count;
  monitor->modes = modes;
}

MONITOR *process_monitor(CGDirectDisplayID display){
  if(!CGDisplayIsAsleep(display)){
    MONITOR *monitor = alloc_monitor(sizeof(MONITOR_DATA));

    CFDictionaryRef info = IODisplayCreateInfoDictionary(CGDisplayIOServicePort(display),
                                                         kIODisplayOnlyPreferredName);
    CFDictionaryRef names = CFDictionaryGetValue(info, CFSTR(kDisplayProductName));
    CFStringRef value;
    if(names == NULL || !CFDictionaryGetValueIfPresent(names, CFSTR("en_US"), (const void**) &value)){
      monitor->name = copy_str("Unknown");
    }else{
      CFIndex size = CFStringGetMaximumSizeForEncoding(CFStringGetLength(value),
                                                       kCFStringEncodingASCII);
      char *name = calloc(size+1, sizeof(char));
      CFStringGetCString(value, name, size, kCFStringEncodingASCII);
      monitor->name = name;
    }
    CFRelease(info);

    monitor->primary = CGDisplayIsMain(display);
    
    const CGSize size = CGDisplayScreenSize(display);
    monitor->width = size.width;
    monitor->height = size.height;
    
    monitor->_data->display_id = display;
    monitor->_data->unit_number = CGDisplayUnitNumber(display);

    detect_modes(monitor);

    return monitor;
  }
  return NULL;
}

MONITORS_EXPORT bool libmonitors_detect(int *ext_count, MONITOR ***ext_monitors){  
  MONITOR **monitors = NULL;
  int count = 0;

  int display_count = 0;
  CGGetOnlineDisplayList(0, NULL, &display_count);
  CGDirectDisplayID *display_ids = calloc(display_count, sizeof(CGDirectDisplayID));
  CGGetOnlineDisplayList(display_count, display_ids, &display_count);
  
  monitors = alloc_monitors(display_count);
  for(int i=0; i<display_count; ++i){
    MONITOR *monitor = process_monitor(display_ids[i]);
    if(monitor != NULL){
      monitors[count] = monitor;
      ++count;
    }
  }

  free(display_ids);
  
  *ext_monitors = monitors;
  *ext_count = count;
  return true;
}

MONITORS_EXPORT bool libmonitors_make_mode_current(MODE *mode){
  if(mode->monitor->current_mode != mode){
    int success = false;

    return success;
  }
  
  return true;
}

MONITORS_EXPORT bool libmonitors_init(){
  return true;
}

MONITORS_EXPORT void libmonitors_deinit(){
}
