#ifndef _src_renderer_gl_api_h_
#define _src_renderer_gl_api_h_

namespace Yttrium
{
	namespace Gl
	{
		bool check_extension(const char* list, const char* name);
	}
}

#endif

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
#		define GLINTEGER(name) GLint name = 0;
#		define GLINTEGERV(name, count) GLint name[count];
#		define GLFLOAT(name) GLfloat name = 0;
#		define GLFLOATV(name, count) GLfloat name[count];
#		define GLSTRING(name) const GLubyte* name = nullptr;
#	else
#		define GLINTEGER(name)
#		define GLINTEGERV(name, count)
#		define GLFLOAT(name)
#		define GLFLOATV(name, count)
#		define GLSTRING(name)
#	endif
#	define GLEXTENSION(name) bool name = false;
#	define GLEND
#	define GLEXTFUNC(name, ret, attr) ret (__Y_GLAPI *name)attr;
#
#endif

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
#		define GLFUNCTION(name, ret, attr) *(Gl::Address*)(&GLAPI_CONTEXT name) = GLAPI_FUNCTION("gl"#name);
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
#	define GLEXTENSION(name) if ((GLAPI_CONTEXT name = GLAPI_CHECK_EXTENSION((const char*) GLAPI_CONTEXT EXTENSIONS, "GL_"#name))) {
#	define GLEND }
#	define GLEXTFUNC(name, ret, attr) *(Gl::Address*)(&GLAPI_CONTEXT name) = GLAPI_EXTFUNC("gl"#name);
#
#endif

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
#endif

// Now, after all the preparations, we are able to include
// the requested GLAPI "script".

#ifdef __GLAPI_API

// Non-deprecated OpenGL 1.0 API.

GLFUNCTION(BlendFunc, GLvoid, (GLenum, GLenum))
GLFUNCTION(Clear, GLvoid, (GLbitfield))
GLFUNCTION(ClearColor, GLvoid, (GLclampf, GLclampf, GLclampf, GLclampf))
GLFUNCTION(ClearDepth, GLvoid, (GLclampd))
GLFUNCTION(ClearStencil, GLvoid, (GLint))
GLFUNCTION(ColorMask, GLvoid, (GLboolean, GLboolean, GLboolean, GLboolean))
GLFUNCTION(CullFace, GLvoid, (GLenum))
GLFUNCTION(DepthFunc, GLvoid, (GLenum))
GLFUNCTION(DepthMask, GLvoid, (GLboolean))
GLFUNCTION(DepthRange, GLvoid, (GLclampd, GLclampd))
GLFUNCTION(Disable, GLvoid, (GLenum))
GLFUNCTION(DrawBuffer, GLvoid, (GLenum))
GLFUNCTION(Enable, GLvoid, (GLenum))
GLFUNCTION(Finish, GLvoid, ())
GLFUNCTION(Flush, GLvoid, ())
GLFUNCTION(FrontFace, GLvoid, (GLenum))
GLFUNCTION(GetBooleanv, GLvoid, (GLenum, GLboolean*))
GLFUNCTION(GetDoublev, GLvoid, (GLenum, GLdouble*))
GLFUNCTION(GetError, GLenum, ())
GLFUNCTION(GetFloatv, GLvoid, (GLenum, GLfloat*))
GLFUNCTION(GetIntegerv, GLvoid, (GLenum, GLint*))
GLFUNCTION(GetString, const GLubyte*, (GLenum))
GLFUNCTION(GetTexImage, GLvoid, (GLenum, GLint, GLenum, GLenum, GLvoid*))
GLFUNCTION(GetTexLevelParameterfv, GLvoid, (GLenum, GLint, GLenum, GLfloat*))
GLFUNCTION(GetTexLevelParameteriv, GLvoid, (GLenum, GLint, GLenum, GLint*))
GLFUNCTION(GetTexParameterfv, GLvoid, (GLenum, GLenum, GLfloat*))
GLFUNCTION(GetTexParameteriv, GLvoid, (GLenum, GLenum, GLint*))
GLFUNCTION(Hint, GLvoid, (GLenum, GLenum))
GLFUNCTION(IsEnabled, GLboolean, (GLenum))
GLFUNCTION(LineWidth, GLvoid, (GLfloat))
GLFUNCTION(LogicOp, GLvoid, (GLenum))
GLFUNCTION(PixelStoref, GLvoid, (GLenum, GLfloat))
GLFUNCTION(PixelStorei, GLvoid, (GLenum, GLint))
GLFUNCTION(PointSize, GLvoid, (GLfloat))
GLFUNCTION(PolygonMode, GLvoid, (GLenum, GLenum))
GLFUNCTION(ReadBuffer, GLvoid, (GLenum))
GLFUNCTION(ReadPixels, GLvoid, (GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*))
GLFUNCTION(Scissor, GLvoid, (GLint, GLint, GLsizei, GLsizei))
GLFUNCTION(StencilFunc, GLvoid, (GLenum, GLint, GLuint))
GLFUNCTION(StencilMask, GLvoid, (GLuint))
GLFUNCTION(StencilOp, GLvoid, (GLenum, GLenum, GLenum))
GLFUNCTION(TexImage1D, GLvoid, (GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid*))
GLFUNCTION(TexImage2D, GLvoid, (GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*))
GLFUNCTION(TexParameterf, GLvoid, (GLenum, GLenum, GLfloat))
GLFUNCTION(TexParameterfv, GLvoid, (GLenum, GLenum, const GLfloat*))
GLFUNCTION(TexParameteri, GLvoid, (GLenum, GLenum, GLint))
GLFUNCTION(TexParameteriv, GLvoid, (GLenum, GLenum, const GLint*))
GLFUNCTION(Viewport, GLvoid, (GLint, GLint, GLsizei, GLsizei))
// EXT_direct_state_access
GLFUNCTION(TextureImage2DEXT, void, (GLuint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*))
GLFUNCTION(TextureParameterfEXT, void, (GLuint, GLenum, GLenum, GLfloat))
GLFUNCTION(TextureParameteriEXT, void, (GLuint, GLenum, GLenum, GLint))

GLSTRING(EXTENSIONS)
GLINTEGER(MAX_TEXTURE_SIZE)
GLINTEGERV(MAX_VIEWPORT_DIMS, 2)
GLSTRING(RENDERER)
GLSTRING(VENDOR)
GLSTRING(VERSION)

// Non-deprecated OpenGL 1.1 API.

GLFUNCTION(BindTexture, GLvoid, (GLenum, GLuint))
GLFUNCTION(CopyTexImage1D, GLvoid, (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint))
GLFUNCTION(CopyTexImage2D, GLvoid, (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint))
GLFUNCTION(CopyTexSubImage1D, GLvoid, (GLenum, GLint, GLint, GLint, GLint, GLsizei))
GLFUNCTION(CopyTexSubImage2D, GLvoid, (GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))
GLFUNCTION(DeleteTextures, GLvoid, (GLsizei, const GLuint*))
GLFUNCTION(DrawArrays, GLvoid, (GLenum, GLint, GLsizei))
GLFUNCTION(DrawElements, GLvoid, (GLenum, GLsizei, GLenum, const GLvoid*))
GLFUNCTION(GenTextures, GLvoid, (GLsizei, GLuint*))
GLFUNCTION(IsTexture, GLboolean, (GLuint))
GLFUNCTION(PolygonOffset, GLvoid, (GLfloat, GLfloat))
GLFUNCTION(TexSubImage1D, GLvoid, (GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid*))
GLFUNCTION(TexSubImage2D, GLvoid, (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))

// Non-deprecated OpenGL 1.2 API.

GLFUNCTION(CopyTexSubImage3D, GLvoid, (GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))
GLFUNCTION(DrawRangeElements, GLvoid, (GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid*))
GLFUNCTION(TexImage3D, GLvoid, (GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*))
GLFUNCTION(TexSubImage3D, GLvoid, (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))

//GLINTEGER(MAX_3D_TEXTURE_SIZE)
//GLINTEGER(MAX_ELEMENTS_INDICES)
//GLINTEGER(MAX_ELEMENTS_VERTICES)

// Non-deprecated OpenGL 1.3 API.

GLFUNCTION(ActiveTexture, GLvoid, (GLenum))
GLFUNCTION(GetCompressedTexImage, GLvoid, (GLenum, GLint, GLvoid*))
GLFUNCTION(CompressedTexImage1D, GLvoid, (GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid*))
GLFUNCTION(CompressedTexImage2D, GLvoid, (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid*))
GLFUNCTION(CompressedTexImage3D, GLvoid, (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid*))
GLFUNCTION(CompressedTexSubImage1D, GLvoid, (GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid*))
GLFUNCTION(CompressedTexSubImage2D, GLvoid, (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid*))
GLFUNCTION(CompressedTexSubImage3D, GLvoid, (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid*))
GLFUNCTION(SampleCoverage, GLvoid, (GLfloat, GLboolean))

GLINTEGER(MAX_COMBINED_TEXTURE_IMAGE_UNITS)
GLINTEGER(NUM_COMPRESSED_TEXTURE_FORMATS)
//GLINTEGERV(COMPRESSED_TEXTURE_FORMATS, NUM_COMPRESSED_TEXTURE_FORMATS)

// Non-deprecated OpenGL 1.5 API (partial).

GLFUNCTION(BindBuffer, void, (GLenum, GLuint))
GLFUNCTION(BufferData, void, (GLenum, GLsizeiptr, const void*, GLenum))
GLFUNCTION(BufferSubData, void, (GLenum, GLintptr, GLsizeiptr, const void*))
GLFUNCTION(DeleteBuffers, void, (GLsizei, const GLuint*))
GLFUNCTION(GenBuffers, void, (GLsizei, GLuint*))
GLFUNCTION(GetBufferParameteriv, void, (GLenum, GLenum, GLint*))
GLFUNCTION(GetBufferPointerv, void, (GLenum, GLenum, void**))
GLFUNCTION(GetBufferSubData, void, (GLenum, GLintptr, GLsizeiptr, void*))
GLFUNCTION(IsBuffer, GLboolean, (GLuint))
GLFUNCTION(MapBuffer, void*, (GLenum, GLenum))
GLFUNCTION(UnmapBuffer, GLboolean, (GLenum))
// EXT_direct_state_access
GLFUNCTION(NamedBufferDataEXT, void, (GLuint, GLsizeiptr, const void*, GLenum))
GLFUNCTION(NamedBufferSubDataEXT, void, (GLuint, GLintptr, GLsizeiptr, const void*))

// Non-deprecated OpenGL 2.0 API (partial).

GLFUNCTION(AttachShader, void, (GLuint, GLuint))
GLFUNCTION(CompileShader, void, (GLuint))
GLFUNCTION(CreateProgram, GLuint, ())
GLFUNCTION(CreateShader, GLuint, (GLenum))
GLFUNCTION(DeleteProgram, void, (GLuint))
GLFUNCTION(DeleteShader, void, (GLuint))
GLFUNCTION(DisableVertexAttribArray, void, (GLuint))
GLFUNCTION(EnableVertexAttribArray, void, (GLuint))
GLFUNCTION(GetProgramiv, void, (GLuint, GLenum, GLint*))
GLFUNCTION(GetProgramInfoLog, void, (GLuint, GLsizei, GLsizei*, GLchar*))
GLFUNCTION(GetShaderiv, void, (GLuint, GLenum, GLint*))
GLFUNCTION(GetShaderInfoLog, void, (GLuint, GLsizei, GLsizei*, GLchar*))
GLFUNCTION(GetUniformLocation, GLint, (GLuint, const GLchar*))
GLFUNCTION(LinkProgram, void, (GLuint))
GLFUNCTION(ShaderSource, void, (GLuint, GLsizei, const GLchar**, const GLint*))
GLFUNCTION(UseProgram, void, (GLuint))
GLFUNCTION(VertexAttribPointer, void, (GLuint, GLint, GLenum, GLboolean, GLsizei, const void*))
// EXT_direct_state_access
GLFUNCTION(ProgramUniform1fEXT, void, (GLuint, GLint, GLfloat))
GLFUNCTION(ProgramUniform2fEXT, void, (GLuint, GLint, GLfloat, GLfloat))
GLFUNCTION(ProgramUniform3fEXT, void, (GLuint, GLint, GLfloat, GLfloat, GLfloat))
GLFUNCTION(ProgramUniform4fEXT, void, (GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat))
GLFUNCTION(ProgramUniform1iEXT, void, (GLuint, GLint, GLint))
GLFUNCTION(ProgramUniform2iEXT, void, (GLuint, GLint, GLint, GLint))
GLFUNCTION(ProgramUniform3iEXT, void, (GLuint, GLint, GLint, GLint, GLint))
GLFUNCTION(ProgramUniform4iEXT, void, (GLuint, GLint, GLint, GLint, GLint, GLint))
GLFUNCTION(ProgramUniform1fvEXT, void, (GLuint, GLint, GLsizei, const GLfloat*))
GLFUNCTION(ProgramUniform2fvEXT, void, (GLuint, GLint, GLsizei, const GLfloat*))
GLFUNCTION(ProgramUniform3fvEXT, void, (GLuint, GLint, GLsizei, const GLfloat*))
GLFUNCTION(ProgramUniform4fvEXT, void, (GLuint, GLint, GLsizei, const GLfloat*))
GLFUNCTION(ProgramUniform1ivEXT, void, (GLuint, GLint, GLsizei, const GLint*))
GLFUNCTION(ProgramUniform2ivEXT, void, (GLuint, GLint, GLsizei, const GLint*))
GLFUNCTION(ProgramUniform3ivEXT, void, (GLuint, GLint, GLsizei, const GLint*))
GLFUNCTION(ProgramUniform4ivEXT, void, (GLuint, GLint, GLsizei, const GLint*))
GLFUNCTION(ProgramUniformMatrix2fvEXT, void, (GLuint, GLint, GLsizei, GLboolean, const GLfloat*))
GLFUNCTION(ProgramUniformMatrix3fvEXT, void, (GLuint, GLint, GLsizei, GLboolean, const GLfloat*))
GLFUNCTION(ProgramUniformMatrix4fvEXT, void, (GLuint, GLint, GLsizei, GLboolean, const GLfloat*))

// Non-deprecated OpenGL 3.0 API (partial).

// EXT_direct_state_access
GLFUNCTION(GenerateTextureMipmapEXT, void, (GLuint, GLenum))

GLEXTENSION(ARB_texture_rectangle) // Core OpenGL 3.1 and higher.
	GLINTEGER(MAX_RECTANGLE_TEXTURE_SIZE_ARB)
	GLEND

GLEXTENSION(ARB_explicit_attrib_location) // Core OpenGL 3.3 and higher.
	GLEND

GLEXTENSION(ARB_vertex_array_object) // Core OpenGL 2.0 and higher.
	GLEXTFUNC(BindVertexArray, GLvoid, (GLuint))
	GLEXTFUNC(DeleteVertexArrays, GLvoid, (GLsizei, const GLuint*))
	GLEXTFUNC(GenVertexArrays, GLvoid, (GLsizei, GLuint*))
	GLEXTFUNC(IsVertexArray, GLboolean, (GLuint))
	GLEND

GLEXTENSION(ARB_vertex_attrib_binding) // Core OpenGL 4.3 and higher.
	GLEXTFUNC(BindVertexBuffer, void, (GLuint, GLuint, GLintptr, GLsizei))
	GLEXTFUNC(VertexAttribFormat, void, (GLuint, GLint, GLenum, GLboolean, GLuint))
	GLEXTFUNC(VertexAttribIFormat, void, (GLuint, GLint, GLenum, GLuint))
	GLEXTFUNC(VertexAttribLFormat, void, (GLuint, GLint, GLenum, GLuint))
	GLEXTFUNC(VertexAttribBinding, void, (GLuint, GLuint))
	GLEXTFUNC(VertexBindingDivisor, void, (GLuint, GLuint))
	// EXT_direct_state_access
	GLEXTFUNC(VertexArrayBindVertexBufferEXT, void, (GLuint, GLuint, GLuint, GLintptr, GLsizei))
	GLEXTFUNC(VertexArrayVertexAttribFormatEXT, void, (GLuint, GLuint, GLint, GLenum, GLboolean, GLuint))
	GLEXTFUNC(VertexArrayVertexAttribIFormatEXT, void, (GLuint, GLuint, GLint, GLenum, GLuint))
	GLEXTFUNC(VertexArrayVertexAttribLFormatEXT, void, (GLuint, GLuint, GLint, GLenum, GLuint))
	GLEXTFUNC(VertexArrayVertexAttribBindingEXT, void, (GLuint, GLuint, GLuint))
	GLEXTFUNC(VertexArrayVertexBindingDivisorEXT, void, (GLuint, GLuint, GLuint))
	GLEND

GLEXTENSION(ARB_framebuffer_object) // Core OpenGL 3.0 and higher.
	GLEXTFUNC(BindFramebuffer, GLvoid, (GLenum, GLuint))
	GLEXTFUNC(BindRenderbuffer, GLvoid, (GLenum, GLuint))
	GLEXTFUNC(BlitFramebuffer, GLvoid, (GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum))
	GLEXTFUNC(CheckFramebufferStatus, GLenum, (GLenum))
	GLEXTFUNC(DeleteFramebuffers, GLvoid, (GLsizei, const GLuint*))
	GLEXTFUNC(DeleteRenderbuffers, GLvoid, (GLsizei, const GLuint*))
	GLEXTFUNC(FramebufferRenderbuffer, GLvoid, (GLenum, GLenum, GLenum, GLuint))
	GLEXTFUNC(FramebufferTexture1D, GLvoid, (GLenum, GLenum, GLenum, GLuint, GLint))
	GLEXTFUNC(FramebufferTexture2D, GLvoid, (GLenum, GLenum, GLenum, GLuint, GLint))
	GLEXTFUNC(FramebufferTexture3D, GLvoid, (GLenum, GLenum, GLenum, GLuint, GLint, GLint))
	GLEXTFUNC(FramebufferTextureLayer, GLvoid, (GLenum, GLenum, GLuint, GLint, GLint))
	GLEXTFUNC(GenFramebuffers, GLvoid, (GLsizei, GLuint*))
	GLEXTFUNC(GenRenderbuffers, GLvoid, (GLsizei, GLuint*))
	GLEXTFUNC(GenerateMipmap, GLvoid, (GLenum))
	GLEXTFUNC(GetFramebufferAttachmentParameteriv, GLvoid, (GLenum, GLenum, GLenum, GLint*))
	GLEXTFUNC(GetRenderbufferParameteriv, GLvoid, (GLenum, GLenum, GLint*))
	GLEXTFUNC(IsFramebuffer, GLboolean, (GLuint))
	GLEXTFUNC(IsRenderbuffer, GLboolean, (GLuint))
	GLEXTFUNC(RenderbufferStorage, GLvoid, (GLenum, GLenum, GLsizei, GLsizei))
	GLEXTFUNC(RenderbufferStorageMultisample, GLvoid, (GLenum, GLsizei, GLenum, GLsizei, GLsizei))
	GLEND

GLEXTENSION(EXT_direct_state_access)
	// ...
	GLEND

GLEXTENSION(EXT_texture_filter_anisotropic)
	GLFLOAT(MAX_TEXTURE_MAX_ANISOTROPY_EXT)
	GLEND

GLEXTENSION(KHR_debug)
	GLEXTFUNC(DebugMessageCallback, GLvoid, (GLDEBUGPROC, const GLvoid*))
	GLEXTFUNC(DebugMessageControl, GLvoid, (GLenum, GLenum, GLenum, GLsizei, const GLuint*, GLboolean))
	GLEXTFUNC(DebugMessageInsert, GLvoid, (GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*))
	GLEXTFUNC(GetDebugMessageLog, GLuint, (GLuint, GLsizei, GLenum*, GLenum*, GLuint*, GLenum*, GLsizei*, GLchar*))
	GLEXTFUNC(GetObjectLabel, GLvoid, (GLenum, GLuint, GLsizei, GLsizei*, GLchar*))
	GLEXTFUNC(GetObjectPtrLabel, GLvoid, (GLvoid*, GLsizei, GLsizei*, GLchar*))
	GLEXTFUNC(GetPointerv, GLvoid, (GLenum, GLvoid**))
	GLEXTFUNC(ObjectLabel, GLvoid, (GLenum, GLuint, GLsizei, const GLchar*))
	GLEXTFUNC(ObjectPtrLabel, GLvoid, (GLvoid*, GLsizei, const GLchar*))
	GLEXTFUNC(PopDebugGroup, GLvoid, ())
	GLEXTFUNC(PushDebugGroup, GLvoid, (GLenum, GLuint, GLsizei, const GLchar*))
	GLEND

#endif

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
#endif
