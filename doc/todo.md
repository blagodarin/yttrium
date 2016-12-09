# TODO
* Library:
	* Overall:
		- Windows port.
		- Lua scripting.
		- Hash-based packed file access.
		- Named realtime counters.
		- Split the library into core library and toolkit (utility, support) library.
		- Consistent exceptions.
	* Audio:
		- Consider loading the entire Music data into memory at once, it shouldn't be big enough to make streaming the only option.
	* GUI:
		- Line editor widget.
	* Image:
		- Compressed image format support (e.g. compressed textures).
		- Multi-level image support (e.g. textures with mipmaps).
		- Separate image header/data loading support.
		- Image pack support (a list of headers with filenames and raw image data offsets).
	* Internationalization:
		- Unicode font support.
	* Renderer:
		- Smooth 3D rotation.
* Tools:
	- Create textured font builder.
