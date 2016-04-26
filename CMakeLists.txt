cmake_minimum_required(VERSION 3.1)
project(libmonitors)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_BUILD_TYPE Debug)

include_directories(source)
add_library(monitors SHARED "src/monitors-internal.c")

if(UNIX AND NOT APPLE)
  find_package(Xrandr REQUIRED)
  target_sources(monitors PRIVATE "src/monitors-linux.c")
  target_link_libraries(monitors X11)
  target_link_libraries(monitors ${XRANDR_LIBRARIES})
endif()

if(APPLE)
  find_library(CORE_FOUNDATION CoreFoundation)
  target_link_libraries(monitors ${CORE_FOUNDATION})
  target_sources(monitors PUBLIC "src/monitors-darwin.c")
endif()

if(WIN32)
  find_package(WindowsSDK REQUIRED)
  include_directories(${WINSDK_INCLUDE_DIRS})
  target_compile_options(monitors PRIVATE -D_MSC_VER=1500)
  target_link_libraries(monitors ${WINSDK_LIBRARY_DIRS})
  target_sources(monitors PUBLIC "src/monitors-windows.c")
endif()

add_executable(monitor_test "test/monitor-test.c")
target_link_libraries(monitor_test monitors)

install(TARGETS monitors DESTINATION /usr/lib)
