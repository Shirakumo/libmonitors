#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xinerama.h>
#include "monitors.h"
#include "monitors-internal.h"

static Display *display = NULL;
static Window root = 0;
static int screen = 0;

MODE_DATA{
  RRMode rrmode;
};

MONITOR_DATA{
  RROutput rroutput;
  RRCrtc rrcrtc;
};

char *copy_str(char *string){
  char *copy = NULL;
  int count = 0;
  while(string[count] != 0) ++count;
  
  copy = calloc(count, sizeof(char));
  for(; count>=0; --count) copy[count]=string[count];
  return copy;
}


bool test_xrandr(){
  int eventBase, errorBase, major, minor;
  if(XRRQueryExtension(display, &eventBase, &errorBase)
     && XRRQueryVersion(display, &major, &minor)){
    return (major > 1 || minor >= 3);
  }
  return false;
}

bool process_mode(MODE *mode, XRRScreenResources *screen_resources,
                  XRRCrtcInfo *crtc_info, RRMode output_mode){
  XRRModeInfo mode_info;

  if(mode->_data == NULL)
    mode->_data = calloc(1, sizeof(MODE_DATA));
  mode->_data->rrmode = output_mode;

  for(int j=0; j<screen_resources->nmode; ++j){
    mode_info = screen_resources->modes[j];
    if(mode_info.id == output_mode)
      break;
  }
  
  if((mode_info.modeFlags & RR_Interlace) == 0){        
    if(crtc_info->rotation != RR_Rotate_90 && crtc_info->rotation != RR_Rotate_270){
      mode->width = mode_info.width;
      mode->height = mode_info.height;
    }else{
      mode->width = mode_info.height;
      mode->height = mode_info.width;
    }

    if(mode_info.hTotal && mode_info.vTotal){
      mode->refresh = (int)((double)mode_info.dotClock /
                            ((double)mode_info.hTotal *
                             (double)mode_info.vTotal));
    }else{
      mode->refresh = -1;
    }
    return true;
  }else{
    return false;
  }
}

bool process_mode_default(MODE *mode){
  mode->width = DisplayWidth(display, screen);
  mode->height = DisplayWidth(display, screen);
  mode->refresh = -1;
  return true;
}

void detect_modes(MONITOR *monitor, RRCrtc crtc, RROutput output){
  MODE *modes = NULL;
  int count = 0;

  if(test_xrandr()){
    XRRScreenResources *screen_resources = XRRGetScreenResourcesCurrent(display, root);
    XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, screen_resources, crtc);
    XRROutputInfo *output_info = XRRGetOutputInfo(display, screen_resources, output);

    modes = alloc_modes(output_info->nmode);

    for(int i=0; i<output_info->nmode; ++i){
      if(process_mode(&modes[count], screen_resources, crtc_info, output_info->modes[i])){
        if(!is_duplicate_mode(&modes[count], count, modes)){
          modes[count].monitor = monitor;
          if(crtc_info->mode == output_info->modes[i])
            monitor->current_mode = &modes[count];
          ++count;
        }
      }
    }

    XRRFreeOutputInfo(output_info);
    XRRFreeCrtcInfo(crtc_info);
    XRRFreeScreenResources(screen_resources);
  }else{
    count = 1;
    modes = alloc_modes(count);
    process_mode_default(&modes[0]);
  }

  monitor->modes = modes;
  monitor->mode_count = count;
}

MONITOR *process_monitor(XRRScreenResources *screen_resources, RRCrtc crtc, RROutput output){
  XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, screen_resources, crtc);
  XRROutputInfo *output_info = XRRGetOutputInfo(display, screen_resources, output);
  MONITOR *monitor = NULL;

  if(output_info->connection == RR_Connected){
    monitor = alloc_monitor(sizeof(MONITOR_DATA));
    monitor->name = copy_str(output_info->name);
    monitor->_data->rroutput = output;
    monitor->_data->rrcrtc = crtc;
          
    if(crtc_info->rotation != RR_Rotate_90 && crtc_info->rotation != RR_Rotate_270){
      monitor->width = output_info->mm_width;
      monitor->height = output_info->mm_height;
    }else{
      monitor->width = output_info->mm_height;
      monitor->height = output_info->mm_width;
    }

    detect_modes(monitor, output_info->crtc, output);
  }
  
  XRRFreeOutputInfo(output_info);
  XRRFreeCrtcInfo(crtc_info);
  return monitor;
}

bool process_monitor_default(MONITOR *monitor){
  monitor->name = copy_str("Display");
  monitor->width = DisplayWidthMM(display, screen);
  monitor->height = DisplayWidthMM(display, screen);
  return true;
}

MONITORS_EXPORT bool libmonitors_detect(int *ext_count, MONITOR ***ext_monitors){
  if(!display) return false;
  
  MONITOR **monitors = NULL;
  int count = 0;
  
  if(test_xrandr()){
    XRRScreenResources *screen_resources = XRRGetScreenResources(display, root);
    RROutput primary_output = XRRGetOutputPrimary(display, root);
    
    monitors = alloc_monitors(screen_resources->noutput);
    for(int i=0; i<screen_resources->ncrtc; ++i){
      RRCrtc crtc = screen_resources->crtcs[i];
      XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, screen_resources, crtc);

      for(int j=0; j<crtc_info->noutput; ++j){
        RROutput output = crtc_info->outputs[j];
        MONITOR *monitor = process_monitor(screen_resources, crtc, output);
        if(monitor != NULL){
          if(output == primary_output){
            monitor->primary = true;
          }
          monitors[count] = monitor;
          ++count;
        }
      }

      XRRFreeCrtcInfo(crtc_info);
    }
    XRRFreeScreenResources(screen_resources);
  }else{
    count = 1;
    monitors = alloc_monitors(count);
    monitors[0]->primary = true;
    process_monitor_default(monitors[0]);
  }
  
  *ext_monitors = monitors;
  *ext_count = count;
  return true;
}

MONITORS_EXPORT bool libmonitors_make_mode_current(MODE *mode){
  if(!display || !test_xrandr()) return false;

  if(mode->monitor->current_mode != mode){
    int success = false;
    XRRScreenResources *screen_resources = XRRGetScreenResources(display, root);
    XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display,
                                            screen_resources,
                                            mode->monitor->_data->rrcrtc);

    if(!XRRSetCrtcConfig(display,
                         screen_resources,
                         mode->monitor->_data->rrcrtc,
                         CurrentTime,
                         crtc_info->x,
                         crtc_info->y,
                         mode->_data->rrmode,
                         crtc_info->rotation,
                         crtc_info->outputs,
                         crtc_info->noutput)){
      success = true;
      mode->monitor->current_mode = mode;
    }

    XRRFreeCrtcInfo(crtc_info);
    XRRFreeScreenResources(screen_resources);
    return success;
  }
  
  return true;
}

MONITORS_EXPORT bool libmonitors_init(){
  if(!display){
    display = XOpenDisplay(NULL);
    if(!display) return false;
    
    root = XRootWindow(display, screen);
    screen = XDefaultScreen(display);
  }
  return true;
}

MONITORS_EXPORT void libmonitors_deinit(){
  if(display){
    XCloseDisplay(display);
    display = NULL;
    root = 0;
    screen = 0;
  }
}
