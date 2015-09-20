# TODO
* Library:
	* Overall:
		- Windows port.
	* Base:
		- Convenient string formatting.
	* GUI:
		- Line editor widget.
		- Layouts.
	* Image:
		- Compressed image format support (e.g. compressed textures).
		- Multi-level image support (e.g. textures with mipmaps).
		- Separate image header/data loading support.
		- Image pack support (a list of headers with filenames and raw image data offsets).
	* Internationalization:
		- Unicode font support.
	* Memory:
		- Keep a global track of all ProxyAllocators.
		- Replace allocator objects with allocation tags.
		- Try defaulting pool granularity to memory page size.
	* Renderer:
		- Pre-rendered text (i.e. pre-built geometry).
* Examples:
	* Tetrium:
		- Pack the data at build time.
		- Runtime block texture generation.