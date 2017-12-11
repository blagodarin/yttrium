# Roadmap

This is an ordered list of high-priority features to be developed, describing
where the project is currently heading to.

1. **Vulkan support**. Current status: in progress.
   - Vulkan support may require to drop OpenGL support due to significant
     efforts needed to keep both backends under single interface. This is not
     a preferable outcome, but still a possible one.
   - The goal is running examples with Vulkan instead of OpenGL without any
     noticeable visual difference.
1. **Text input**. Current status: in progress.
   - Replaces the current workaround key-to-text mapping with platform-supplied
     Unicode text input, which may be the first step to Unicode support.
   - It may be better to finally drop X11 port (in favor of XCB port) than to
     implement text input for X11.
1. **Android port**. Requires **Vulkan support**. Current status: not started.
   - Requires research on Android development in general. Also OpenAL may be
     unavailable on Android, which in turn will require a new audio backend.
   - It is possible to port to Android without using Vulkan (i. e. using
     OpenGL), but it doesn't look like a good idea because of uncertain future
     of the OpenGL renderer itself.
   - The goal is running Tetrium on Android. Android devices don't usually have
     keyboard or mouse, requiring an overhaul of user interaction mechanisms.

# Backlog

This is a list of low-priority features that would be nice to have once. There
are also "TODO" comments in source files marking places for some internal
improvements and other code-related issues.

* Library:
	- Multiple windows.
	- Windowed mode.
	- Proper benchmarks (e. g. using some benchmarking library).
	- Debug output, e. g. in a ring buffer that can be dumped on exception.
	* Audio:
		- Windows Multimedia audio backend.
	* GUI:
		- Line editor widget.
	* Image:
		- Compressed images (e. g. compressed textures).
		- Multi-level images (e. g. textures with mipmaps).
		- Separate image header/data loading.
		- Image packs (lists of image headers with filenames and raw image data offsets).
		- Loading image data into the specified buffer (e. g. into mapped texture memory).
	* Internationalization:
		- Unicode fonts.
	* Platforms:
		- Fully replace X11 port with XCB port.
		- Port to MacOS X.
	* Renderer:
		- Loading textures without intermediate Images.
	* Storage:
		- Hash-based packed file access.
* New tools:
	- Textured font builder.
