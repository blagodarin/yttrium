# TODO
* Library:
	* Overall:
		- Windows port.
	* GUI:
		- Line editor widget.
		- Layouts.
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
		- Try defaulting pool granularity to memory page size.
		- Don't use ProxyAllocators inside the library except for the default allocator.
	* Renderer:
		- Pre-rendered text (i.e. pre-built geometry).
		- Vertex attribute arrays.
* Examples:
	* Tetrium:
		- Pack the data at build time.
		- Runtime block texture generation.
