#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include <windows.h>
#include "monitors.h"
#include "monitors-internal.h"

MODE_DATA{
};

MONITOR_DATA{
  WCHAR adapter_name[32];
  WCHAR display_name[32];
};

WCHAR *copy_str(WCHAR *string){
  WCHAR *copy = NULL;
  int count = 0;
  while(string[count] != 0) ++count;
  
  copy = calloc(count, sizeof(char));
  for(; count>=0; --count) copy[count]=string[count];
  return copy;
}


void process_monitor(MONITOR *monitor, DISPLAY_DEVICEW *adapter, DISPLAY_DEVICEW *display){
  HDC device_context = CreateDCW(L"DISPLAY", adapter->DeviceName, NULL, NULL);

  if(monitor->_data == NULL)
    monitor->_data = calloc(1, sizeof(MONITOR_DATA));
  wcscpy_s(monitor->_data->adapter_name, 32, adapter->DeviceName);
  if(display) wcscpy_s(monitor->_data->display_name, 32, display->DeviceName);

  monitor->name = copy_str(display? display->DeviceString : adapter->DeviceString);
  monitor->width = GetDeviceCaps(device_context, HORZSIZE);
  monitor->height = GetDeviceCaps(device_context, VERTSIZE);
  
  DeleteDC(device_context);
}

MONITORS_EXPORT bool libmonitors_detect(int *ext_count, MONITOR **ext_monitors){
  int count = 0;
  MONITOR *monitors = NULL;

  DISPLAY_DEVICEW adapter;
  for(int i=0; EnumDisplayDevicesW(NULL, i, &adapter, 0); ++i){
    if(adapter.StateFlags & DISPLAY_DEVICE_ACTIVE){
      DISPLAY_DEVICEW display;
      // Count at least one per adapter
      ++count;
      for(int j=1; EnumDisplayDevicesW(adapter.DeviceName, j, &display, 0); ++j){
        ++count;
      }
    }
  }

  monitors = alloc_monitors(count);
  for(int i=0; EnumDisplayDevicesW(NULL, i, &adapter, 0); ++i){
    if(adapter.StateFlags & DISPLAY_DEVICE_ACTIVE){
      DISPLAY_DEVICEW display;
      if(EnumDisplayDevicesW(adapter.DeviceName, 0, &display, 0)){
        for(int j=0; EnumDisplayDevicesW(adapter.DeviceName, j, &display, 0); ++j){
          process_monitor(&monitors[count], &adapter, &display);
        }
      }else{
        --count;
        process_monitor(&monitors[count], &adapter, NULL);
      }
    }
  }
  
  *ext_count = count;
  *ext_monitors = monitors;
  return true;
}

MONITORS_EXPORT bool libmonitors_make_mode_current(MODE *mode){
  return true;
}

MONITORS_EXPORT bool libmonitors_init(){
  return true;
}

MONITORS_EXPORT void libmonitors_deinit(){
  
}
