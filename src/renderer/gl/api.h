#ifndef __RENDERER_GL_API_H
#define __RENDERER_GL_API_H

namespace Yttrium
{

namespace Gl
{

bool check_extension(const char *list, const char *name);

} // namespace Gl

} // namespace Yttrium

#endif // __RENDERER_GL_API_H

#if defined(GLAPI_DECLARE) || defined(GLAPI_INIT) || defined(GLAPI_DROP)
#	define __GLAPI_API
#endif

// Mode: GLAPI_DECLARE.
// Declares the [extension] functions and persistent state data.
// Input:
// GLAPI_DYNAMIC_GL [optional]
// GLAPI_NO_DATA [optional]

#ifdef GLAPI_DECLARE
#
#	define GLFUNCTION(name, ret, attr) ret (__Y_GLAPI *name)attr;
#	ifndef GLAPI_NO_DATA
#		define GLINTEGER(name) GLint name;
#		define GLINTEGERV(name, count) GLint name[count];
#		define GLFLOAT(name) GLfloat name;
#		define GLFLOATV(name, count) GLfloat name[count];
#		define GLSTRING(name) const GLubyte *name;
#	else
#		define GLINTEGER(name)
#		define GLINTEGERV(name, count)
#		define GLFLOAT(name)
#		define GLFLOATV(name, count)
#		define GLSTRING(name)
#	endif
#	define GLEXTENSION(name) bool name;
#	define GLEND
#	define GLEXTFUNC(name, ret, attr) ret (__Y_GLAPI *name)attr;
#
#endif // GLA_DECLARE

// Mode: GLAPI_INIT.
// Initializes the [extension] functions and persistent state data.
// Input:
// GLAPI_CONTEXT [optional]
// GLAPI_CHECK_EXTENSION(list, name) [optional]
// GLAPI_EXTFUNC(name) [optional]
// GLAPI_FUNCTION(name) [optional]
// GLAPI_DYNAMIC_GL [optional]
// GLAPI_NO_DATA [optional]

#ifdef GLAPI_INIT
#
#	ifndef GLAPI_CHECK_EXTENSION
#		define GLAPI_CHECK_EXTENSION(list, name) Gl::check_extension(list, name)
#	endif
#
#	ifndef GLAPI_CONTEXT
#		define GLAPI_CONTEXT
#	endif
#
#	ifndef GLAPI_FUNCTION
#		define GLAPI_FUNCTION(name) Gl::address(GLAPI_INIT, name)
#	endif
#
#	ifndef GLAPI_EXTFUNC
#		define GLAPI_EXTFUNC(name) Gl::address(GLAPI_INIT, name)
#	endif
#
#	ifndef GLAPI_DYNAMIC_GL
#		define GLFUNCTION(name, ret, attr) GLAPI_CONTEXT name = gl##name;
#	else
#		define GLFUNCTION(name, ret, attr) *(Gl::Address *)(&GLAPI_CONTEXT name) = GLAPI_FUNCTION("gl"#name);
#	endif
#	ifndef GLAPI_NO_DATA
#		ifndef GLAPI_DYNAMIC_GL
#			define GLFLOAT(name) glGetFloatv(GL_##name, &GLAPI_CONTEXT name);
#			define GLFLOATV(name, count) glGetFloatv(GL_##name, GLAPI_CONTEXT name);
#			define GLINTEGER(name) glGetIntegerv(GL_##name, &GLAPI_CONTEXT name);
#			define GLINTEGERV(name, count) glGetIntegerv(GL_##name, GLAPI_CONTEXT name);
#			define GLSTRING(name) GLAPI_CONTEXT name = glGetString(GL_##name);
#		else
#			define GLFLOAT(name) GLAPI_CONTEXT GetFloatv(GL_##name, &GLAPI_CONTEXT name);
#			define GLFLOATV(name, count) GLAPI_CONTEXT GetFloatv(GL_##name, GLAPI_CONTEXT name);
#			define GLINTEGER(name) GLAPI_CONTEXT GetIntegerv(GL_##name, &GLAPI_CONTEXT name);
#			define GLINTEGERV(name, count) GLAPI_CONTEXT GetIntegerv(GL_##name, GLAPI_CONTEXT name);
#			define GLSTRING(name) GLAPI_CONTEXT name = GLAPI_CONTEXT GetString(GL_##name);
#		endif
#	endif
#	define GLEXTENSION(name) if ((GLAPI_CONTEXT name = GLAPI_CHECK_EXTENSION((const char *) GLAPI_CONTEXT EXTENSIONS, "GL_"#name))) {
#	define GLEND }
#	define GLEXTFUNC(name, ret, attr) *(Gl::Address *)(&GLAPI_CONTEXT name) = GLAPI_EXTFUNC("gl"#name);
#
#endif // GLAPI_INIT

// Mode: GLAPI_DROP.
// Invalidates all the extensions.
// Input:
// GLAPI_CONTEXT [optional]

#ifdef GLAPI_DROP
#
#	ifndef GLAPI_CONTEXT
#		define GLAPI_CONTEXT
#	endif
#
#	define GLFUNCTION(name, ret, attr)
#	define GLINTEGER(name)
#	define GLINTEGERV(name, count)
#	define GLFLOAT(name)
#	define GLFLOATV(name, count)
#	define GLSTRING(name)
#	define GLEXTENSION(name) GLAPI_CONTEXT name = false;
#	define GLEND
#	define GLEXTFUNC(name, ret, param)
#
#endif // GLAPI_DROP

// Now, after all the preparations, we are able to include
// the requested GLAPI "script".

#ifdef __GLAPI_API

GLFUNCTION(BindTexture, GLvoid, (GLenum, GLuint))
GLFUNCTION(BlendFunc, GLvoid, (GLenum, GLenum))
GLFUNCTION(Clear, GLvoid, (GLbitfield))
GLFUNCTION(ClearColor, GLvoid, (GLclampf, GLclampf, GLclampf, GLclampf))
GLFUNCTION(ClearDepth, GLvoid, (GLclampd))
GLFUNCTION(ClearStencil, GLvoid, (GLint))
GLFUNCTION(ColorMask, GLvoid, (GLboolean, GLboolean, GLboolean, GLboolean))
GLFUNCTION(ColorPointer, GLvoid, (GLint, GLenum, GLsizei, const GLvoid *)) // TODO: Remove.
GLFUNCTION(CopyTexImage1D, GLvoid, (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint))
GLFUNCTION(CopyTexImage2D, GLvoid, (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint))
GLFUNCTION(CopyTexSubImage1D, GLvoid, (GLenum, GLint, GLint, GLint, GLint, GLsizei))
GLFUNCTION(CopyTexSubImage2D, GLvoid, (GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))
GLFUNCTION(CullFace, GLvoid, (GLenum))
GLFUNCTION(DeleteTextures, GLvoid, (GLsizei, const GLuint *))
GLFUNCTION(DepthFunc, GLvoid, (GLenum))
GLFUNCTION(DepthMask, GLvoid, (GLboolean))
GLFUNCTION(DepthRange, GLvoid, (GLclampd, GLclampd))
GLFUNCTION(Disable, GLvoid, (GLenum))
GLFUNCTION(DisableClientState, GLvoid, (GLenum)) // TODO: Remove.
GLFUNCTION(DrawArrays, GLvoid, (GLenum, GLint, GLsizei))
GLFUNCTION(DrawBuffer, GLvoid, (GLenum))
GLFUNCTION(DrawElements, GLvoid, (GLenum, GLsizei, GLenum, const GLvoid *))
GLFUNCTION(Enable, GLvoid, (GLenum))
GLFUNCTION(EnableClientState, GLvoid, (GLenum)) // TODO: Remove.
GLFUNCTION(Finish, GLvoid, ())
GLFUNCTION(Flush, GLvoid, ())
GLFUNCTION(FrontFace, GLvoid, (GLenum))
GLFUNCTION(GenTextures, GLvoid, (GLsizei, GLuint *))
GLFUNCTION(GetBooleanv, GLvoid, (GLenum, GLboolean *))
GLFUNCTION(GetDoublev, GLvoid, (GLenum, GLdouble *))
GLFUNCTION(GetError, GLenum, ())
GLFUNCTION(GetFloatv, GLvoid, (GLenum, GLfloat *))
GLFUNCTION(GetIntegerv, GLvoid, (GLenum, GLint *))
GLFUNCTION(GetLightfv, GLvoid, (GLenum, GLenum, GLfloat *))
GLFUNCTION(GetLightiv, GLvoid, (GLenum, GLenum, GLint *))
GLFUNCTION(GetString, const GLubyte *, (GLenum))
GLFUNCTION(GetTexImage, GLvoid, (GLenum, GLint, GLenum, GLenum, GLvoid *))
GLFUNCTION(GetTexLevelParameterfv, GLvoid, (GLenum, GLint, GLenum, GLfloat *))
GLFUNCTION(GetTexLevelParameteriv, GLvoid, (GLenum, GLint, GLenum, GLint *))
GLFUNCTION(GetTexParameterfv, GLvoid, (GLenum, GLenum, GLfloat *))
GLFUNCTION(GetTexParameteriv, GLvoid, (GLenum, GLenum, GLint *))
GLFUNCTION(Hint, GLvoid, (GLenum, GLenum))
GLFUNCTION(IsEnabled, GLboolean, (GLenum))
GLFUNCTION(IsTexture, GLboolean, (GLuint))
GLFUNCTION(LineWidth, GLvoid, (GLfloat))
GLFUNCTION(LoadMatrixf, GLvoid, (const GLfloat *)) // TODO: Remove.
GLFUNCTION(LogicOp, GLvoid, (GLenum))
GLFUNCTION(MatrixMode, GLvoid, (GLenum)) // TODO: Remove.
GLFUNCTION(PixelStoref, GLvoid, (GLenum, GLfloat))
GLFUNCTION(PixelStorei, GLvoid, (GLenum, GLint))
GLFUNCTION(PointSize, GLvoid, (GLfloat))
GLFUNCTION(PolygonMode, GLvoid, (GLenum, GLenum))
GLFUNCTION(PolygonOffset, GLvoid, (GLfloat, GLfloat))
GLFUNCTION(PopAttrib, GLvoid, ()) // TODO: Remove.
GLFUNCTION(PopClientAttrib, GLvoid, ()) // TODO: Remove.
GLFUNCTION(PushAttrib, GLvoid, (GLbitfield)) // TODO: Remove.
GLFUNCTION(PushClientAttrib, GLvoid, (GLbitfield)) // TODO: Remove.
GLFUNCTION(ReadBuffer, GLvoid, (GLenum))
GLFUNCTION(ReadPixels, GLvoid, (GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *))
GLFUNCTION(Scissor, GLvoid, (GLint, GLint, GLsizei, GLsizei))
GLFUNCTION(StencilFunc, GLvoid, (GLenum, GLint, GLuint))
GLFUNCTION(StencilMask, GLvoid, (GLuint))
GLFUNCTION(StencilOp, GLvoid, (GLenum, GLenum, GLenum))
GLFUNCTION(TexCoordPointer, GLvoid, (GLint, GLenum, GLsizei, const GLvoid *)) // TODO: Remove.
GLFUNCTION(TexImage1D, GLvoid, (GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid *))
GLFUNCTION(TexImage2D, GLvoid, (GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *))
GLFUNCTION(TexParameterf, GLvoid, (GLenum, GLenum, GLfloat))
GLFUNCTION(TexParameterfv, GLvoid, (GLenum, GLenum, const GLfloat *))
GLFUNCTION(TexParameteri, GLvoid, (GLenum, GLenum, GLint))
GLFUNCTION(TexParameteriv, GLvoid, (GLenum, GLenum, const GLint *))
GLFUNCTION(TexSubImage1D, GLvoid, (GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *))
GLFUNCTION(TexSubImage2D, GLvoid, (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))
GLFUNCTION(VertexPointer, GLvoid, (GLint, GLenum, GLsizei, const GLvoid *)) // TODO: Remove.
GLFUNCTION(Viewport, GLvoid, (GLint, GLint, GLsizei, GLsizei))

GLSTRING(EXTENSIONS)
GLINTEGER(MAX_ATTRIB_STACK_DEPTH)
GLINTEGER(MAX_CLIENT_ATTRIB_STACK_DEPTH)
GLINTEGER(MAX_CLIP_PLANES)
GLINTEGER(MAX_EVAL_ORDER)
GLINTEGER(MAX_LIGHTS)
GLINTEGER(MAX_MODELVIEW_STACK_DEPTH)
GLINTEGER(MAX_NAME_STACK_DEPTH)
GLINTEGER(MAX_PIXEL_MAP_TABLE)
GLINTEGER(MAX_PROJECTION_STACK_DEPTH)
GLINTEGER(MAX_TEXTURE_SIZE)
GLINTEGER(MAX_TEXTURE_STACK_DEPTH)
GLINTEGERV(MAX_VIEWPORT_DIMS, 2)
GLSTRING(RENDERER)
GLSTRING(VENDOR)
GLSTRING(VERSION)

GLEXTENSION(ARB_texture_non_power_of_two) // NOTE: Core OpenGL 2.0 and higher.
	GLEND

GLEXTENSION(ARB_texture_rectangle) // NOTE: Core OpenGL 3.1 and higher.
	GLINTEGER(MAX_RECTANGLE_TEXTURE_SIZE_ARB)
	GLEND

GLEXTENSION(ARB_vertex_array_object)
	GLEXTFUNC(BindVertexArray, GLvoid, (GLuint))
	GLEXTFUNC(DeleteVertexArrays, GLvoid, (GLsizei, const GLuint *))
	GLEXTFUNC(GenVertexArrays, GLvoid, (GLsizei, GLuint *))
	GLEXTFUNC(IsVertexArray, GLboolean, (GLuint))
	GLEND

GLEXTENSION(ARB_vertex_buffer_object) // NOTE: Core OpenGL 1.5 and higher.
	GLEXTFUNC(BindBufferARB, GLvoid, (GLenum, GLuint))
	GLEXTFUNC(DeleteBuffersARB, GLvoid, (GLsizei, const GLuint *))
	GLEXTFUNC(GenBuffersARB, GLvoid, (GLsizei, GLuint *))
	GLEXTFUNC(IsBufferARB, GLboolean, (GLuint))
	GLEXTFUNC(BufferDataARB, GLvoid, (GLenum, GLsizeiptrARB, const GLvoid *, GLenum))
	GLEXTFUNC(BufferSubDataARB, GLvoid, (GLenum, GLintptrARB, GLsizeiptrARB, const GLvoid *))
	GLEXTFUNC(GetBufferSubDataARB, GLvoid, (GLenum, GLintptrARB, GLsizeiptrARB, GLvoid *))
	GLEXTFUNC(MapBufferARB, GLvoid *, (GLenum, GLenum))
	GLEXTFUNC(UnmapBufferARB, GLboolean, (GLenum))
	GLEXTFUNC(GetBufferParameterivARB, GLvoid, (GLenum, GLenum, GLint *))
	GLEXTFUNC(GetBufferPointervARB, GLvoid, (GLenum, GLenum, GLvoid **))
	GLEND

GLEXTENSION(EXT_texture_filter_anisotropic)
	GLFLOAT(MAX_TEXTURE_MAX_ANISOTROPY_EXT)
	GLEND

#endif // __GLAPI_API

// After all, clear all the definitions (inluding the input ones).

#ifdef __GLAPI_API
#
#	undef GLEXTFUNC
#	undef GLEND
#	undef GLEXTENSION
#	undef GLFLOAT
#	undef GLFLOATV
#	undef GLFUNCTION
#	undef GLINTEGER
#	undef GLINTEGERV
#	undef GLSTRING
#
#	undef GLAPI_CHECK_EXTENSION
#	undef GLAPI_CONTEXT
#	undef GLAPI_EXTFUNC
#	undef GLAPI_FUNCTION
#	undef GLAPI_NO_DATA
#	undef GLAPI_DYNAMIC_GL
#
#	undef __GLAPI_API
#
#	undef GLAPI_DROP
#	undef GLAPI_INIT
#	undef GLAPI_DECLARE
#
#endif // __GLAPI_API
