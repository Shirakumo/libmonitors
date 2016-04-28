## About libmonitors
Get and set monitor information on all platforms that actually matter (Linux, OS X, Windows). Debate about the last joke is not part of this README.

## Building
Use cmake to build. You will need a compiler that supports C99, Clang being the preferred choice.

### Linux
You'll need X11 and Xrandr to build against, and cmake, GNU make, and clang to build with.

From the project root directory, run:

    mkdir build
    cd build
    CC=clang cmake ..
    make

To test if it's working, run the `monitors_test` program.

### Windows
You'll need the Windows SDK to build against, and cmake, a build tool (NMake, MinGW Make), and a suitable compiler (MSVC2015+, Clang) to build with.

Here's the steps for Clang, using the MSVC toolchain. Open the MSVC tools console and run from the project root directory:

    mkdir build
    cd build
    set CC=clang-cl
    cmake .. -G "NMake Makefiles"
    nmake

To test if it's working, run the `monitors_test.exe` program.

### OS X
To be done.

## Using
Compile against `src/monitors.h`.

Before doing anything else with this library, you will need to call `libmonitors_init`. On some platforms, this sets up required connections to the display system. If it returns `false`, the setup failed and other function calls are sure to fail too. Similarly, once you're done call `libmonitors_deinit` to clean things up again.

If setup was successful, you can get an array of monitors with `libmonitors_detect` like so:

    int count = 0;
    struct libmonitors_monitor *monitors[];
    if(!libmonitors_detect(&count, &monitors)){
        // Error handling
    }

The detect call too returns a boolean indicating success or failure. If successful, `count` will contain how many monitors were detected, and `monitors` will be an array of pointers to monitor structs.

Each monitor struct has `name`, `primary`, `width`, `height`, `mode_count`, `current_mode`, and a `modes` field. The width and height being the physical size of the monitor in millimetres. Using the count and modes, you can traverse the available modes for a monitor like so:

    struct libmonitors_monitor *monitor = monitors[0];
    for(int i=0; i<monitor->mode_count; ++i){
        struct libmonitors_mode *mode = &monitor->modes[i];
        // Process the mode
    }

Each mode has `monitor`, `width`, `height`, and `refresh` fields. The width and height being the pixel sizes, and the refresh rate being in Herz. The monitor field is a back-link to the monitor the mode originated from.

You can then switch the monitor's mode by using `libmonitors_make_mode_current` on the desired mode. If the switch was successful, true is returned and the monitor's `current_mode` will have changed to reflect the newly chosen mode. On some platforms the mode will be reset to its original state automatically once the application exits, but to be safe you should always keep a reference to the original mode before changing it and reset things back to it once you are done.

Once you're done with the monitors, you can free the array with `libmonitors_free_monitors`. Alternatively if you, say, only need a specific monitor, you can simply `free` the array you got from the `detect` call and manually free each unused monitor object with `libmonitors_free_monitor`.

For a simple sample program that uses all the features of this library, see `src/monitors_test.c`.
