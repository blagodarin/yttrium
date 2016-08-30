# TODO
* Library:
	* Overall:
		- Windows port.
		- Lua scripting.
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
		- Keep a global track of all NamedAllocators for realtime memory activity tracking.
	* Renderer:
		- OpenGL 3.3 compliance.
* Examples:
	* Tetrium:
		- Pack the data at build time.
		- Runtime block texture generation.
