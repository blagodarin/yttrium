# Roadmap

1. **Text input**. Current status: in progress.
   - Replace the current workaround key-to-text mapping with platform-supplied
     Unicode text input.
   - It may be better to finally drop X11 port (in favor of XCB port) than to
     implement text input for X11.
1. **Vulkan support**. Current status: in progress.
   - Vulkan support may require to drop OpenGL support due to significant
     efforts needed to keep both backends under single interface. This is not
     a preferable outcome, but still a possible one.
   - The goal is running examples with Vulkan instead of OpenGL without any
     noticeable visual difference.
1. **Android port**. Requires **Vulkan support**. Current status: not started.
   - Requires research on Android development in general. Also OpenAL may be
     unavailable on Android, which in turn will require a new audio backend.
   - It is possible to port to Android without using Vulkan (i. e. using
     OpenGL), but it doesn't look like a good idea because of uncertain future
     of the OpenGL renderer itself.
   - The goal is running Tetrium on Android. Android devices don't usually have
     keyboard or mouse, requiring an overhaul of user interaction mechanisms.
