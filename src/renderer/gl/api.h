#if defined(GLAPI_GET_FUNCTION) && defined(GLAPI_HAS_EXTENSION)
	// Initialization mode.
	#define GLEND }
	#define GLEXTENSION(name) if ((name = GLAPI_HAS_EXTENSION("GL_"#name))) {
	#define GLFLOAT(name) GetFloatv(GL_##name, &name);
	#define GLFLOATV(name, count) do { name.resize(count); GetFloatv(GL_##name, name.data()); } while (false);
	#define GLFUNCTION(name, ret, attr) reinterpret_cast<GlAddress&>(name) = GLAPI_GET_FUNCTION("gl"#name);
	#define GLINTEGER(name) GetIntegerv(GL_##name, &name);
	#define GLINTEGERV(name, count) do { name.resize(count); GetIntegerv(GL_##name, name.data()); } while (false);
	#define GLSTRING(name) name = GetString(GL_##name);
	#define GLSTRINGV(name, count) do { name.reserve(count); for (GLsizei i = 0; i < count; ++i) name.emplace_back(GetStringi(GL_##name, i)); } while (false);
#else
	// Declaration mode.
	#define GLEND
	#define GLEXTENSION(name) bool name = false;
	#define GLFLOAT(name) GLfloat name = 0;
	#define GLFLOATV(name, count) std::vector<GLfloat> name;
	#define GLFUNCTION(name, ret, attr) ret (GLAPI_CALL *name)attr = nullptr;
	#define GLINTEGER(name) GLint name = 0;
	#define GLINTEGERV(name, count) std::vector<GLint> name;
	#define GLSTRING(name) const char* name = nullptr;
	#define GLSTRINGV(name, count) std::vector<std::string> name; // TODO-17: Use std::string_view.
#endif

// OpenGL 1.0

GLFUNCTION(BlendFunc, void, (GLenum, GLenum))
GLFUNCTION(Clear, void, (GLbitfield))
GLFUNCTION(ClearColor, void, (GLclampf, GLclampf, GLclampf, GLclampf))
GLFUNCTION(ClearDepth, void, (GLclampd))
GLFUNCTION(ClearStencil, void, (GLint))
GLFUNCTION(ColorMask, void, (GLboolean, GLboolean, GLboolean, GLboolean))
GLFUNCTION(CullFace, void, (GLenum))
GLFUNCTION(DepthFunc, void, (GLenum))
GLFUNCTION(DepthMask, void, (GLboolean))
GLFUNCTION(DepthRange, void, (GLclampd, GLclampd))
GLFUNCTION(Disable, void, (GLenum))
GLFUNCTION(DrawBuffer, void, (GLenum))
GLFUNCTION(Enable, void, (GLenum))
GLFUNCTION(Finish, void, ())
GLFUNCTION(Flush, void, ())
GLFUNCTION(FrontFace, void, (GLenum))
GLFUNCTION(GetBooleanv, void, (GLenum, GLboolean*))
GLFUNCTION(GetDoublev, void, (GLenum, GLdouble*))
GLFUNCTION(GetError, GLenum, ())
GLFUNCTION(GetFloatv, void, (GLenum, GLfloat*))
GLFUNCTION(GetIntegerv, void, (GLenum, GLint*))
GLFUNCTION(GetString, const char*, (GLenum))
GLFUNCTION(GetTexImage, void, (GLenum, GLint, GLenum, GLenum, void*))
GLFUNCTION(GetTexLevelParameterfv, void, (GLenum, GLint, GLenum, GLfloat*))
GLFUNCTION(GetTexLevelParameteriv, void, (GLenum, GLint, GLenum, GLint*))
GLFUNCTION(GetTexParameterfv, void, (GLenum, GLenum, GLfloat*))
GLFUNCTION(GetTexParameteriv, void, (GLenum, GLenum, GLint*))
GLFUNCTION(Hint, void, (GLenum, GLenum))
GLFUNCTION(IsEnabled, GLboolean, (GLenum))
GLFUNCTION(LineWidth, void, (GLfloat))
GLFUNCTION(LogicOp, void, (GLenum))
GLFUNCTION(PixelStoref, void, (GLenum, GLfloat))
GLFUNCTION(PixelStorei, void, (GLenum, GLint))
GLFUNCTION(PointSize, void, (GLfloat))
GLFUNCTION(PolygonMode, void, (GLenum, GLenum))
GLFUNCTION(ReadBuffer, void, (GLenum))
GLFUNCTION(ReadPixels, void, (GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void*))
GLFUNCTION(Scissor, void, (GLint, GLint, GLsizei, GLsizei))
GLFUNCTION(StencilFunc, void, (GLenum, GLint, GLuint))
GLFUNCTION(StencilMask, void, (GLuint))
GLFUNCTION(StencilOp, void, (GLenum, GLenum, GLenum))
GLFUNCTION(TexImage1D, void, (GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const void*))
GLFUNCTION(TexImage2D, void, (GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*))
GLFUNCTION(TexParameterf, void, (GLenum, GLenum, GLfloat))
GLFUNCTION(TexParameterfv, void, (GLenum, GLenum, const GLfloat*))
GLFUNCTION(TexParameteri, void, (GLenum, GLenum, GLint))
GLFUNCTION(TexParameteriv, void, (GLenum, GLenum, const GLint*))
GLFUNCTION(Viewport, void, (GLint, GLint, GLsizei, GLsizei))

GLINTEGER(MAX_TEXTURE_SIZE)
GLINTEGERV(MAX_VIEWPORT_DIMS, 2)
GLSTRING(RENDERER)
GLSTRING(VENDOR)
GLSTRING(VERSION)

// OpenGL 1.1

GLFUNCTION(BindTexture, void, (GLenum, GLuint))
GLFUNCTION(CopyTexImage1D, void, (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint))
GLFUNCTION(CopyTexImage2D, void, (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint))
GLFUNCTION(CopyTexSubImage1D, void, (GLenum, GLint, GLint, GLint, GLint, GLsizei))
GLFUNCTION(CopyTexSubImage2D, void, (GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))
GLFUNCTION(DeleteTextures, void, (GLsizei, const GLuint*))
GLFUNCTION(DrawArrays, void, (GLenum, GLint, GLsizei))
GLFUNCTION(DrawElements, void, (GLenum, GLsizei, GLenum, const void*))
GLFUNCTION(GenTextures, void, (GLsizei, GLuint*))
GLFUNCTION(IsTexture, GLboolean, (GLuint))
GLFUNCTION(PolygonOffset, void, (GLfloat, GLfloat))
GLFUNCTION(TexSubImage1D, void, (GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const void*))
GLFUNCTION(TexSubImage2D, void, (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void*))

// OpenGL 1.2

GLFUNCTION(CopyTexSubImage3D, void, (GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))
GLFUNCTION(DrawRangeElements, void, (GLenum, GLuint, GLuint, GLsizei, GLenum, const void*))
GLFUNCTION(TexImage3D, void, (GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*))
GLFUNCTION(TexSubImage3D, void, (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void*))

GLINTEGER(MAX_3D_TEXTURE_SIZE)
GLINTEGER(MAX_ELEMENTS_INDICES)
GLINTEGER(MAX_ELEMENTS_VERTICES)

// OpenGL 1.3

GLFUNCTION(ActiveTexture, void, (GLenum))
GLFUNCTION(GetCompressedTexImage, void, (GLenum, GLint, void*))
GLFUNCTION(CompressedTexImage1D, void, (GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const void*))
GLFUNCTION(CompressedTexImage2D, void, (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void*))
GLFUNCTION(CompressedTexImage3D, void, (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void*))
GLFUNCTION(CompressedTexSubImage1D, void, (GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const void*))
GLFUNCTION(CompressedTexSubImage2D, void, (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void*))
GLFUNCTION(CompressedTexSubImage3D, void, (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void*))
GLFUNCTION(SampleCoverage, void, (GLfloat, GLboolean))

GLINTEGER(MAX_COMBINED_TEXTURE_IMAGE_UNITS)
GLINTEGER(NUM_COMPRESSED_TEXTURE_FORMATS)
GLINTEGERV(COMPRESSED_TEXTURE_FORMATS, NUM_COMPRESSED_TEXTURE_FORMATS)

// OpenGL 1.5

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

// OpenGL 2.0

GLFUNCTION(AttachShader, void, (GLuint, GLuint))
GLFUNCTION(BindVertexArray, void, (GLuint))
GLFUNCTION(CompileShader, void, (GLuint))
GLFUNCTION(CreateProgram, GLuint, ())
GLFUNCTION(CreateShader, GLuint, (GLenum))
GLFUNCTION(DeleteProgram, void, (GLuint))
GLFUNCTION(DeleteShader, void, (GLuint))
GLFUNCTION(DeleteVertexArrays, void, (GLsizei, const GLuint*))
GLFUNCTION(DisableVertexAttribArray, void, (GLuint))
GLFUNCTION(EnableVertexAttribArray, void, (GLuint))
GLFUNCTION(GenVertexArrays, void, (GLsizei, GLuint*))
GLFUNCTION(GetProgramiv, void, (GLuint, GLenum, GLint*))
GLFUNCTION(GetProgramInfoLog, void, (GLuint, GLsizei, GLsizei*, GLchar*))
GLFUNCTION(GetShaderiv, void, (GLuint, GLenum, GLint*))
GLFUNCTION(GetShaderInfoLog, void, (GLuint, GLsizei, GLsizei*, GLchar*))
GLFUNCTION(GetUniformLocation, GLint, (GLuint, const GLchar*))
GLFUNCTION(LinkProgram, void, (GLuint))
GLFUNCTION(ShaderSource, void, (GLuint, GLsizei, const GLchar**, const GLint*))
GLFUNCTION(UseProgram, void, (GLuint))
GLFUNCTION(VertexAttribPointer, void, (GLuint, GLint, GLenum, GLboolean, GLsizei, const void*))

// OpenGL 3.0

GLFUNCTION(BindFramebuffer, void, (GLenum, GLuint))
GLFUNCTION(BindRenderbuffer, void, (GLenum, GLuint))
GLFUNCTION(BlitFramebuffer, void, (GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum))
GLFUNCTION(CheckFramebufferStatus, GLenum, (GLenum))
GLFUNCTION(DeleteFramebuffers, void, (GLsizei, const GLuint*))
GLFUNCTION(DeleteRenderbuffers, void, (GLsizei, const GLuint*))
GLFUNCTION(FramebufferRenderbuffer, void, (GLenum, GLenum, GLenum, GLuint))
GLFUNCTION(FramebufferTexture, void, (GLenum, GLenum, GLuint, GLint))
GLFUNCTION(FramebufferTexture1D, void, (GLenum, GLenum, GLenum, GLuint, GLint))
GLFUNCTION(FramebufferTexture2D, void, (GLenum, GLenum, GLenum, GLuint, GLint))
GLFUNCTION(FramebufferTexture3D, void, (GLenum, GLenum, GLenum, GLuint, GLint, GLint))
GLFUNCTION(FramebufferTextureLayer, void, (GLenum, GLenum, GLuint, GLint, GLint))
GLFUNCTION(GenFramebuffers, void, (GLsizei, GLuint*))
GLFUNCTION(GenRenderbuffers, void, (GLsizei, GLuint*))
GLFUNCTION(GetFramebufferAttachmentParameteriv, void, (GLenum, GLenum, GLenum, GLint*))
GLFUNCTION(GetRenderbufferParameteriv, void, (GLenum, GLenum, GLint*))
GLFUNCTION(GetStringi, const char*, (GLenum, GLuint))
GLFUNCTION(IsFramebuffer, GLboolean, (GLuint))
GLFUNCTION(IsRenderbuffer, GLboolean, (GLuint))
GLFUNCTION(RenderbufferStorage, void, (GLenum, GLenum, GLsizei, GLsizei))
GLFUNCTION(RenderbufferStorageMultisample, void, (GLenum, GLsizei, GLenum, GLsizei, GLsizei))

GLINTEGER(MAJOR_VERSION)
GLINTEGER(MINOR_VERSION)
GLINTEGER(NUM_EXTENSIONS)
GLSTRINGV(EXTENSIONS, NUM_EXTENSIONS)

GLEXTENSION(EXT_direct_state_access)
	// OpenGL 1.0
	GLFUNCTION(TextureImage2DEXT, void, (GLuint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*))
	GLFUNCTION(TextureParameterfEXT, void, (GLuint, GLenum, GLenum, GLfloat))
	GLFUNCTION(TextureParameteriEXT, void, (GLuint, GLenum, GLenum, GLint))
	// OpenGL 1.5
	GLFUNCTION(NamedBufferDataEXT, void, (GLuint, GLsizeiptr, const void*, GLenum))
	GLFUNCTION(NamedBufferSubDataEXT, void, (GLuint, GLintptr, GLsizeiptr, const void*))
	// OpenGL 2.0
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
	// OpenGL 3.0
	GLFUNCTION(GenerateTextureMipmapEXT, void, (GLuint, GLenum))
	GLEND

GLEXTENSION(ARB_vertex_attrib_binding) // Core OpenGL 4.3 and higher.
	GLFUNCTION(BindVertexBuffer, void, (GLuint, GLuint, GLintptr, GLsizei))
	GLFUNCTION(VertexAttribFormat, void, (GLuint, GLint, GLenum, GLboolean, GLuint))
	GLFUNCTION(VertexAttribIFormat, void, (GLuint, GLint, GLenum, GLuint))
	GLFUNCTION(VertexAttribLFormat, void, (GLuint, GLint, GLenum, GLuint))
	GLFUNCTION(VertexAttribBinding, void, (GLuint, GLuint))
	GLFUNCTION(VertexBindingDivisor, void, (GLuint, GLuint))
	// EXT_direct_state_access
	GLFUNCTION(VertexArrayBindVertexBufferEXT, void, (GLuint, GLuint, GLuint, GLintptr, GLsizei))
	GLFUNCTION(VertexArrayVertexAttribFormatEXT, void, (GLuint, GLuint, GLint, GLenum, GLboolean, GLuint))
	GLFUNCTION(VertexArrayVertexAttribIFormatEXT, void, (GLuint, GLuint, GLint, GLenum, GLuint))
	GLFUNCTION(VertexArrayVertexAttribLFormatEXT, void, (GLuint, GLuint, GLint, GLenum, GLuint))
	GLFUNCTION(VertexArrayVertexAttribBindingEXT, void, (GLuint, GLuint, GLuint))
	GLFUNCTION(VertexArrayVertexBindingDivisorEXT, void, (GLuint, GLuint, GLuint))
	GLEND

GLEXTENSION(EXT_texture_filter_anisotropic)
	GLFLOAT(MAX_TEXTURE_MAX_ANISOTROPY_EXT)
	GLEND

GLEXTENSION(KHR_debug)
	GLFUNCTION(DebugMessageCallback, void, (GLDEBUGPROC, const void*))
	GLFUNCTION(DebugMessageControl, void, (GLenum, GLenum, GLenum, GLsizei, const GLuint*, GLboolean))
	GLFUNCTION(DebugMessageInsert, void, (GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*))
	GLFUNCTION(GetDebugMessageLog, GLuint, (GLuint, GLsizei, GLenum*, GLenum*, GLuint*, GLenum*, GLsizei*, GLchar*))
	GLFUNCTION(GetObjectLabel, void, (GLenum, GLuint, GLsizei, GLsizei*, GLchar*))
	GLFUNCTION(GetObjectPtrLabel, void, (void*, GLsizei, GLsizei*, GLchar*))
	GLFUNCTION(GetPointerv, void, (GLenum, void**))
	GLFUNCTION(ObjectLabel, void, (GLenum, GLuint, GLsizei, const GLchar*))
	GLFUNCTION(ObjectPtrLabel, void, (void*, GLsizei, const GLchar*))
	GLFUNCTION(PopDebugGroup, void, ())
	GLFUNCTION(PushDebugGroup, void, (GLenum, GLuint, GLsizei, const GLchar*))
	GLEND

#undef GLEND
#undef GLEXTENSION
#undef GLFLOAT
#undef GLFLOATV
#undef GLFUNCTION
#undef GLINTEGER
#undef GLINTEGERV
#undef GLSTRING
#undef GLSTRINGV

#undef GLAPI_GET_FUNCTION
#undef GLAPI_HAS_EXTENSION
