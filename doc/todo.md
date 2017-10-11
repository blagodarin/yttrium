# TODO
* Overall:
	- Building with clang.
* Library:
	- Multiple windows.
	- Windowed mode.
	* GUI:
		- Line editor widget.
	* Image:
		- Compressed images (e.g. compressed textures).
		- Multi-level images (e.g. textures with mipmaps).
		- Separate image header/data loading.
		- Image packs (lists of image headers with filenames and raw image data offsets).
		- Loading image data into the specified buffer (e.g. into mapped texture memory).
	* Internationalization:
		- Unicode fonts.
	* Renderer:
		- Loading textures without intermediate Images.
		- Don't count debug output in rendering statistics.
	* Storage:
		- Hash-based packed file access.
* New tools:
	- Resource compiler (like Qt's RCC).
	- Shader compiler (GLSL to SPIR-V).
	- Textured font builder.
