# TODO
* Library:
	* Overall:
		- Windows port.
		- Lua scripting.
		- Get rid of exception-unsafe Y_NEW.
		- Introduce generic resource concept (real file, packed file, named buffer, etc.).
		- Introduce advanced resource naming (string-based, hash-based).
	* GUI:
		- Line editor widget.
		- Move Gui object out of Window object.
	* Image:
		- Compressed image format support (e.g. compressed textures).
		- Multi-level image support (e.g. textures with mipmaps).
		- Separate image header/data loading support.
		- Image pack support (a list of headers with filenames and raw image data offsets).
	* Internationalization:
		- Unicode font support.
	* ION:
		- Location information support.
	* Memory:
		- Keep a global track of all ProxyAllocators for realtime memory activity tracking.
		- Don't use ProxyAllocators inside the library except for the default allocator.
	* Renderer:
		- Pre-rendered text (i.e. pre-built geometry).
		- OpenGL 3.3 compliance.
* Examples:
	* Tetrium:
		- Pack the data at build time.
		- Runtime block texture generation.
