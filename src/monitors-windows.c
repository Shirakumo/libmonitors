#include <stdlib.h>
#include <windows.h>
#include "monitors.h"
#include "monitors-internal.h"

#pragma comment(lib, "user32.lib")

MODE_DATA{
};

MONITOR_DATA{
  WCHAR adapter_name[32];
  WCHAR display_name[32];
  bool modes_pruned;
};

bool get_device(DISPLAY_DEVICEW *device, WCHAR *parent, int index){
  ZeroMemory(device, sizeof(DISPLAY_DEVICEW));
  device->cb = sizeof(DISPLAY_DEVICEW);
  return EnumDisplayDevicesW(parent, index, device, 0);
}

bool get_settings(DEVMODEW *settings, WCHAR *parent, int index){
  ZeroMemory(settings, sizeof(DEVMODEW));
  settings->dmSize = sizeof(DEVMODEW);
  return EnumDisplaySettingsW(parent, index, settings);
}

bool is_acceptable_mode(MONITOR *monitor, DEVMODEW *settings){
  if(15 < settings->dmBitsPerPel)
    return false;

  if(monitor->_data->modes_pruned){
    if(ChangeDisplaySettingsExW(monitor->_data->adapter_name, settings, NULL, CDS_TEST, NULL)
       != DISP_CHANGE_SUCCESSFUL){
      return false;
    }
  }
  return true;
}

bool is_comparable_setting(DEVMODEW a, DEVMODEW b){
  return (a.dmPelsWidth == b.dmPelsWidth
          && a.dmPelsHeight == b.dmPelsHeight
          && a.dmDisplayFrequency == b.dmDisplayFrequency);
}

void detect_modes(MONITOR *monitor){
  int count = 0;
  MODE *modes = NULL;
  DEVMODEW settings;
  DEVMODEW current;

  get_settings(&current, monitor->_data->adapter_name, ENUM_CURRENT_SETTINGS);
  
  // Count modes
  for(int i=0; get_settings(&settings, monitor->_data->adapter_name, i); ++i){
    if(is_acceptable_mode(monitor, &settings)){
      ++count;
    }
  }

  // Initialize modes
  modes = alloc_modes(count);
  int mode = 0;
  for(int i=0; get_settings(&settings, monitor->_data->adapter_name, i); ++i){
    if(is_acceptable_mode(monitor, &settings)){
      modes[mode].monitor = monitor;
      modes[mode].width = settings.dmPelsWidth;
      modes[mode].height = settings.dmPelsHeight;
      modes[mode].refresh = settings.dmDisplayFrequency;
      if(is_duplicate_mode(&modes[mode], mode, modes)){
        --count;
      }else{
        if(is_comparable_setting(settings, current)){
          monitor->current_mode = &modes[mode];
        }
        ++mode;
      }
    }
  }

  monitor->mode_count = count;
  monitor->modes = modes;
}

MONITOR *process_monitor(DISPLAY_DEVICEW *adapter, DISPLAY_DEVICEW *display){
  MONITOR *monitor = alloc_monitor(sizeof(MONITOR_DATA));

  monitor->name = calloc(128, sizeof(char));
  WCHAR *name = display? display->DeviceString : adapter->DeviceString;
  for(int i=0; i<128; ++i)monitor->name[i] = (name[i]<128)? name[i]: '?';
  
  if (adapter->StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
    monitor->primary = true;

  HDC device_context = CreateDCW(L"DISPLAY", adapter->DeviceName, NULL, NULL);
  monitor->width = GetDeviceCaps(device_context, HORZSIZE);
  monitor->height = GetDeviceCaps(device_context, VERTSIZE);
  DeleteDC(device_context);

  wcscpy_s(monitor->_data->adapter_name, 32, adapter->DeviceName);
  if(display)
    wcscpy_s(monitor->_data->display_name, 32, display->DeviceName);
  if(adapter->StateFlags & DISPLAY_DEVICE_MODESPRUNED)
    monitor->_data->modes_pruned = true;

  detect_modes(monitor);

  return monitor;
}

MONITORS_EXPORT bool libmonitors_detect(int *ext_count, MONITOR ***ext_monitors){
  int count = 0;
  MONITOR **monitors = NULL;
  DISPLAY_DEVICEW adapter, display;
  bool hasDisplays = false;

  // Detect if any displays at all
  for(int i=0; get_device(&adapter, NULL, i); i++){
    if(adapter.StateFlags & DISPLAY_DEVICE_ACTIVE){
      if(get_device(&display, adapter.DeviceName, 0)){
        hasDisplays = true;
        break;
      }
    }
  }

  // Count monitors
  for(int i=0; get_device(&adapter, NULL, i); i++){
    if((adapter.StateFlags & DISPLAY_DEVICE_ACTIVE)){
      if(hasDisplays){
        for(int j=0; get_device(&display, adapter.DeviceName, j); j++){
          ++count;
        }
      }else{
        ++count;
      }
    }
  }
  
  // Initialize monitors
  monitors = alloc_monitors(count);
  int monitor = 0;
  for(int i=0; get_device(&adapter, NULL, i); i++){
    if((adapter.StateFlags & DISPLAY_DEVICE_ACTIVE)){
      
      if(hasDisplays){
        for(int j=0; get_device(&display, adapter.DeviceName, j); j++){
          monitors[monitor] = process_monitor(&adapter, &display);
          monitor++;
        }
      }else{
        monitors[monitor] = process_monitor(&adapter, NULL);
        monitor++;
      }
    }
  }

  *ext_count = count;
  *ext_monitors = monitors;
  return true;
}

MONITORS_EXPORT bool libmonitors_make_mode_current(MODE *mode){
  if(mode->monitor->current_mode != mode){
    DEVMODEW settings;
    ZeroMemory(&settings, sizeof(DEVMODEW));
    settings.dmSize = sizeof(DEVMODEW);
    settings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
    settings.dmPelsWidth = mode->width;
    settings.dmPelsHeight = mode->height;
    settings.dmDisplayFrequency = mode->refresh;
    
    if(ChangeDisplaySettingsExW(mode->monitor->_data->adapter_name,
                                &settings,
                                NULL,
                                CDS_FULLSCREEN,
                                NULL) == DISP_CHANGE_SUCCESSFUL){
      mode->monitor->current_mode = mode;
    }else{
      return false;
    }
  }
  return true;
}

MONITORS_EXPORT bool libmonitors_init(){
  return true;
}

MONITORS_EXPORT void libmonitors_deinit(){
  
}
