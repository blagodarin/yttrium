#include "gl.h"

#include "../../renderer/gl/version.h"
#include "../gl.h"

#include <cstring>
#include <stdexcept>

namespace Yttrium
{
	namespace
	{
		// X error handling (see below).
		bool error_occurred = false;
		int error_handler(::Display*, ::XErrorEvent*)
		{
			error_occurred = true;
			return 0;
		}

		bool check_extension(const char* list, const char* name)
		{
			const size_t name_size = ::strlen(name);
			while ((list = ::strstr(list, name)))
			{
				list += name_size;
				if (*list == ' ' || *list == 0)
					return true;
			}
			return false;
		}

		template <typename T>
		void get_proc_address(T& address, const char* name)
		{
			address = reinterpret_cast<T>(::glXGetProcAddress(reinterpret_cast<const GLubyte*>(name)));
		}
	}

	GlContext::GlContext(::Display* display, int screen)
		: _display(display)
		, _screen(screen)
	{
		int major = 0;
		int minor = 0;
		if (!::glXQueryVersion(display, &major, &minor) || major != 1 || minor < 4)
			throw std::runtime_error("GLX 1.4 is unavailable");

		const int attributes[] =
		{
			GLX_CONFIG_CAVEAT, GLX_NONE,
			GLX_X_RENDERABLE,  True,
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, // Default.
			GLX_RENDER_TYPE,   GLX_RGBA_BIT,   // Default.
			GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
			GLX_RED_SIZE,      8,
			GLX_GREEN_SIZE,    8,
			GLX_BLUE_SIZE,     8,
			GLX_ALPHA_SIZE,    8,
			GLX_DEPTH_SIZE,    24,
			GLX_STENCIL_SIZE,  8,
			GLX_DOUBLEBUFFER,  True,
			None
		};

		::GLXFBConfig best_fbc = {};
		{
			int fbc_count = 0;
			const Y_UNIQUE_PTR(::GLXFBConfig[], ::XFree) fbcs(::glXChooseFBConfig(display, screen, attributes, &fbc_count));
			if (!fbcs)
				throw std::runtime_error("Failed to obtain GLXFBConfigs");
			for (int i = 0; i < fbc_count; ++i)
			{
				// TODO: Try sorting by GLX_SAMPLE_BUFFERS and GLX_SAMPLES as suggested in the official OpenGL example.
				decltype(_visual_info) vi(::glXGetVisualFromFBConfig(display, fbcs[i]));
				if (vi->depth == 24) // A depth of 32 will give us an ugly result.
				{
					_visual_info = std::move(vi);
					best_fbc = fbcs[i];
					break;
				}
			}
		}
		if (!_visual_info)
			throw std::runtime_error("Failed to find suitable GLXFBConfig");

		const auto extensions = ::glXQueryExtensionsString(_display, _screen);
		if (!check_extension(extensions, "GLX_ARB_create_context"))
			throw std::runtime_error("GLX_ARB_create_context is unavailable");

		::GLXContext (*glXCreateContextAttribsARB)(::Display*, ::GLXFBConfig, ::GLXContext, Bool, const int*) = nullptr;
		get_proc_address(glXCreateContextAttribsARB, "glXCreateContextAttribsARB");
		if (!glXCreateContextAttribsARB) // The official OpenGL context creation example suggests checking this too.
			throw std::runtime_error("glXCreateContextAttribsARB is unavailable");

		const int context_attributes[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, Gl::required_major,
			GLX_CONTEXT_MINOR_VERSION_ARB, Gl::required_minor,
		#if Y_IS_DEBUG
			GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
		#endif
			None
		};

		// The actual context creation is wrapped in error handling code as advised
		// by the official OpenGL context creation tutorial. The tutorial also warns
		// that X error handling is global and not thread-safe.
		error_occurred = false;
		const auto old_error_handler = ::XSetErrorHandler(error_handler);
		const auto context = glXCreateContextAttribsARB(_display, best_fbc, nullptr, True, context_attributes);
		if (!context)
			throw std::runtime_error("Failed to create a GLX context");
		::XSync(_display, False); // To ensure any errors generated are processed.
		::XSetErrorHandler(old_error_handler);
		if (error_occurred)
		{
			::glXDestroyContext(_display, context);
			throw std::runtime_error("Failed to create a GLX context");
		}

		if (!::glXIsDirect(_display, context))
		{
			::glXDestroyContext(_display, context);
			throw std::runtime_error("Failed to create a direct GLX context");
		}

		_context = context;

		_extensions.EXT_swap_control = check_extension(extensions, "GLX_EXT_swap_control");
		if (_extensions.EXT_swap_control)
			get_proc_address(_extensions.SwapIntervalEXT, "glXSwapIntervalEXT");

		_extensions.EXT_swap_control_tear = check_extension(extensions, "GLX_EXT_swap_control_tear");
	}

	GlContext::GlContext(GlContext&& context)
		: _display(context._display)
		, _screen(context._screen)
		, _context(context._context)
		, _extensions(context._extensions)
	{
		context._context = nullptr;
	}

	GlContext& GlContext::operator=(GlContext&& context)
	{
		if (_context)
			::glXDestroyContext(_display, _context);
		_display = context._display;
		_screen = context._screen;
		_context = context._context;
		_extensions = context._extensions;
		context._context = nullptr;
		return *this;
	}

	GlContext::~GlContext()
	{
		if (_context)
			::glXDestroyContext(_display, _context);
	}

	void GlContext::bind(::Window window) const
	{
		::glXMakeCurrent(_display, window, _context);
	}

	void GlContext::swap_buffers(::Window window) const
	{
		::glXSwapBuffers(_display, window);
	}

	void GlContext::unbind() const
	{
		::glXMakeCurrent(_display, None, nullptr);
	}

	GlAddress gl_address(const char* name)
	{
		return ::glXGetProcAddress(reinterpret_cast<const GLubyte*>(name));
	}
}
