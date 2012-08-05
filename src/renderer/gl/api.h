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
#		define GLAPI_FUNCTION(name) addr(GLAPI_INIT, name)
#	endif
#
#	ifndef GLAPI_EXTFUNC
#		define GLAPI_EXTFUNC(name) addr(GLAPI_INIT, name)
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
#	define GLEXTENSION(name) if ((GLAPI_CONTEXT name = GLAPI_CHECK_EXTENSION((const char *) GLAPI_CONTEXT EXTENSIONS, "GL_" #name))) {
#	define GLEND }
#	define GLEXTFUNC(name, ret, attr) *(Gl::Address *)(&GLAPI_CONTEXT name) = GLAPI_EXTFUNC(GLAPI_PREFIX_LOWER #name);
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
// the requested GLA "script".

#ifdef __GLAPI_API

GLFUNCTION(Accum, GLvoid, (GLenum, GLfloat))
GLFUNCTION(AlphaFunc, GLvoid, (GLenum, GLclampf))
GLFUNCTION(AreTexturesResident, GLboolean, (GLsizei, const GLuint *, GLboolean *))
GLFUNCTION(ArrayElement, GLvoid, (GLint))
GLFUNCTION(Begin, GLvoid, (GLenum))
GLFUNCTION(BindTexture, GLvoid, (GLenum, GLuint))
GLFUNCTION(Bitmap, GLvoid, (GLsizei, GLsizei, GLfloat, GLfloat, GLfloat, GLfloat, const GLubyte *))
GLFUNCTION(BlendFunc, GLvoid, (GLenum, GLenum))
GLFUNCTION(CallList, GLvoid, (GLuint))
GLFUNCTION(CallLists, GLvoid, (GLsizei, GLenum, const GLvoid *))
GLFUNCTION(Clear, GLvoid, (GLbitfield))
GLFUNCTION(ClearAccum, GLvoid, (GLfloat, GLfloat, GLfloat, GLfloat))
GLFUNCTION(ClearColor, GLvoid, (GLclampf, GLclampf, GLclampf, GLclampf))
GLFUNCTION(ClearDepth, GLvoid, (GLclampd))
GLFUNCTION(ClearIndex, GLvoid, (GLfloat))
GLFUNCTION(ClearStencil, GLvoid, (GLint))
GLFUNCTION(ClipPlane, GLvoid, (GLenum, const GLdouble *))
GLFUNCTION(Color3b, GLvoid, (GLbyte, GLbyte, GLbyte))
GLFUNCTION(Color3bv, GLvoid, (const GLbyte *))
GLFUNCTION(Color3d, GLvoid, (GLdouble, GLdouble, GLdouble))
GLFUNCTION(Color3dv, GLvoid, (const GLdouble *))
GLFUNCTION(Color3f, GLvoid, (GLfloat, GLfloat, GLfloat))
GLFUNCTION(Color3fv, GLvoid, (const GLfloat *))
GLFUNCTION(Color3i, GLvoid, (GLint, GLint, GLint))
GLFUNCTION(Color3iv, GLvoid, (const GLint *))
GLFUNCTION(Color3s, GLvoid, (GLshort, GLshort, GLshort))
GLFUNCTION(Color3sv, GLvoid, (const GLshort *))
GLFUNCTION(Color3ub, GLvoid, (GLubyte, GLubyte, GLubyte))
GLFUNCTION(Color3ubv, GLvoid, (const GLubyte *))
GLFUNCTION(Color3ui, GLvoid, (GLuint, GLuint, GLuint))
GLFUNCTION(Color3uiv, GLvoid, (const GLuint *))
GLFUNCTION(Color3us, GLvoid, (GLushort, GLushort, GLushort))
GLFUNCTION(Color3usv, GLvoid, (const GLushort *))
GLFUNCTION(Color4b, GLvoid, (GLbyte, GLbyte, GLbyte, GLbyte))
GLFUNCTION(Color4bv, GLvoid, (const GLbyte *))
GLFUNCTION(Color4d, GLvoid, (GLdouble, GLdouble, GLdouble, GLdouble))
GLFUNCTION(Color4dv, GLvoid, (const GLdouble *))
GLFUNCTION(Color4f, GLvoid, (GLfloat, GLfloat, GLfloat, GLfloat))
GLFUNCTION(Color4fv, GLvoid, (const GLfloat *))
GLFUNCTION(Color4i, GLvoid, (GLint, GLint, GLint, GLint))
GLFUNCTION(Color4iv, GLvoid, (const GLint *))
GLFUNCTION(Color4s, GLvoid, (GLshort, GLshort, GLshort, GLshort))
GLFUNCTION(Color4sv, GLvoid, (const GLshort *))
GLFUNCTION(Color4ub, GLvoid, (GLubyte, GLubyte, GLubyte, GLubyte))
GLFUNCTION(Color4ubv, GLvoid, (const GLubyte *))
GLFUNCTION(Color4ui, GLvoid, (GLuint, GLuint, GLuint, GLuint))
GLFUNCTION(Color4uiv, GLvoid, (const GLuint *))
GLFUNCTION(Color4us, GLvoid, (GLushort, GLushort, GLushort, GLushort))
GLFUNCTION(Color4usv, GLvoid, (const GLushort *))
GLFUNCTION(ColorMask, GLvoid, (GLboolean, GLboolean, GLboolean, GLboolean))
GLFUNCTION(ColorMaterial, GLvoid, (GLenum, GLenum))
GLFUNCTION(ColorPointer, GLvoid, (GLint, GLenum, GLsizei, const GLvoid *))
GLFUNCTION(CopyPixels, GLvoid, (GLint, GLint, GLsizei, GLsizei, GLenum))
GLFUNCTION(CopyTexImage1D, GLvoid, (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint))
GLFUNCTION(CopyTexImage2D, GLvoid, (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint))
GLFUNCTION(CopyTexSubImage1D, GLvoid, (GLenum, GLint, GLint, GLint, GLint, GLsizei))
GLFUNCTION(CopyTexSubImage2D, GLvoid, (GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))
GLFUNCTION(CullFace, GLvoid, (GLenum))
GLFUNCTION(DeleteLists, GLvoid, (GLuint, GLsizei))
GLFUNCTION(DeleteTextures, GLvoid, (GLsizei, const GLuint *))
GLFUNCTION(DepthFunc, GLvoid, (GLenum))
GLFUNCTION(DepthMask, GLvoid, (GLboolean))
GLFUNCTION(DepthRange, GLvoid, (GLclampd, GLclampd))
GLFUNCTION(Disable, GLvoid, (GLenum))
GLFUNCTION(DisableClientState, GLvoid, (GLenum))
GLFUNCTION(DrawArrays, GLvoid, (GLenum, GLint, GLsizei))
GLFUNCTION(DrawBuffer, GLvoid, (GLenum))
GLFUNCTION(DrawElements, GLvoid, (GLenum, GLsizei, GLenum, const GLvoid *))
GLFUNCTION(DrawPixels, GLvoid, (GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))
GLFUNCTION(EdgeFlag, GLvoid, (GLboolean))
GLFUNCTION(EdgeFlagPointer, GLvoid, (GLsizei, const GLvoid *))
GLFUNCTION(EdgeFlagv, GLvoid, (const GLboolean *))
GLFUNCTION(Enable, GLvoid, (GLenum))
GLFUNCTION(EnableClientState, GLvoid, (GLenum))
GLFUNCTION(End, GLvoid, ())
GLFUNCTION(EndList, GLvoid, ())
GLFUNCTION(EvalCoord1d, GLvoid, (GLdouble))
GLFUNCTION(EvalCoord1dv, GLvoid, (const GLdouble *))
GLFUNCTION(EvalCoord1f, GLvoid, (GLfloat))
GLFUNCTION(EvalCoord1fv, GLvoid, (const GLfloat *))
GLFUNCTION(EvalCoord2d, GLvoid, (GLdouble, GLdouble))
GLFUNCTION(EvalCoord2dv, GLvoid, (const GLdouble *))
GLFUNCTION(EvalCoord2f, GLvoid, (GLfloat, GLfloat))
GLFUNCTION(EvalCoord2fv, GLvoid, (const GLfloat *))
GLFUNCTION(EvalMesh1, GLvoid, (GLenum, GLint, GLint))
GLFUNCTION(EvalMesh2, GLvoid, (GLenum, GLint, GLint, GLint, GLint))
GLFUNCTION(EvalPoint1, GLvoid, (GLint))
GLFUNCTION(EvalPoint2, GLvoid, (GLint, GLint))
GLFUNCTION(FeedbackBuffer, GLvoid, (GLsizei, GLenum, GLfloat *))
GLFUNCTION(Finish, GLvoid, ())
GLFUNCTION(Flush, GLvoid, ())
GLFUNCTION(Fogf, GLvoid, (GLenum, GLfloat))
GLFUNCTION(Fogfv, GLvoid, (GLenum, const GLfloat *))
GLFUNCTION(Fogi, GLvoid, (GLenum, GLint))
GLFUNCTION(Fogiv, GLvoid, (GLenum, const GLint *))
GLFUNCTION(FrontFace, GLvoid, (GLenum))
GLFUNCTION(Frustum, GLvoid, (GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble))
GLFUNCTION(GenLists, GLuint, (GLsizei))
GLFUNCTION(GenTextures, GLvoid, (GLsizei, GLuint *))
GLFUNCTION(GetBooleanv, GLvoid, (GLenum, GLboolean *))
GLFUNCTION(GetClipPlane, GLvoid, (GLenum, GLdouble *))
GLFUNCTION(GetDoublev, GLvoid, (GLenum, GLdouble *))
GLFUNCTION(GetError, GLenum, ())
GLFUNCTION(GetFloatv, GLvoid, (GLenum, GLfloat *))
GLFUNCTION(GetIntegerv, GLvoid, (GLenum, GLint *))
GLFUNCTION(GetLightfv, GLvoid, (GLenum, GLenum, GLfloat *))
GLFUNCTION(GetLightiv, GLvoid, (GLenum, GLenum, GLint *))
GLFUNCTION(GetMapdv, GLvoid, (GLenum, GLenum, GLdouble *))
GLFUNCTION(GetMapfv, GLvoid, (GLenum, GLenum, GLfloat *))
GLFUNCTION(GetMapiv, GLvoid, (GLenum, GLenum, GLint *))
GLFUNCTION(GetMaterialfv, GLvoid, (GLenum, GLenum, GLfloat *))
GLFUNCTION(GetMaterialiv, GLvoid, (GLenum, GLenum, GLint *))
GLFUNCTION(GetPixelMapfv, GLvoid, (GLenum, GLfloat *))
GLFUNCTION(GetPixelMapuiv, GLvoid, (GLenum, GLuint *))
GLFUNCTION(GetPixelMapusv, GLvoid, (GLenum, GLushort *))
GLFUNCTION(GetPointerv, GLvoid, (GLenum, GLvoid **))
GLFUNCTION(GetPolygonStipple, GLvoid, (GLubyte *))
GLFUNCTION(GetString, const GLubyte *, (GLenum))
GLFUNCTION(GetTexEnvfv, GLvoid, (GLenum, GLenum, GLfloat *))
GLFUNCTION(GetTexEnviv, GLvoid, (GLenum, GLenum, GLint *))
GLFUNCTION(GetTexGendv, GLvoid, (GLenum, GLenum, GLdouble *))
GLFUNCTION(GetTexGenfv, GLvoid, (GLenum, GLenum, GLfloat *))
GLFUNCTION(GetTexGeniv, GLvoid, (GLenum, GLenum, GLint *))
GLFUNCTION(GetTexImage, GLvoid, (GLenum, GLint, GLenum, GLenum, GLvoid *))
GLFUNCTION(GetTexLevelParameterfv, GLvoid, (GLenum, GLint, GLenum, GLfloat *))
GLFUNCTION(GetTexLevelParameteriv, GLvoid, (GLenum, GLint, GLenum, GLint *))
GLFUNCTION(GetTexParameterfv, GLvoid, (GLenum, GLenum, GLfloat *))
GLFUNCTION(GetTexParameteriv, GLvoid, (GLenum, GLenum, GLint *))
GLFUNCTION(Hint, GLvoid, (GLenum, GLenum))
GLFUNCTION(IndexMask, GLvoid, (GLuint))
GLFUNCTION(Indexd, GLvoid, (GLdouble))
GLFUNCTION(Indexdv, GLvoid, (const GLdouble *))
GLFUNCTION(Indexf, GLvoid, (GLfloat))
GLFUNCTION(Indexfv, GLvoid, (const GLfloat *))
GLFUNCTION(Indexi, GLvoid, (GLint))
GLFUNCTION(Indexiv, GLvoid, (const GLint *))
GLFUNCTION(Indexs, GLvoid, (GLshort))
GLFUNCTION(Indexsv, GLvoid, (const GLshort *))
GLFUNCTION(Indexub, GLvoid, (GLubyte))
GLFUNCTION(Indexubv, GLvoid, (const GLubyte *))
GLFUNCTION(IndexPointer, GLvoid, (GLenum, GLsizei, const GLvoid *))
GLFUNCTION(InitNames, GLvoid, ())
GLFUNCTION(InterleavedArrays, GLvoid, (GLenum, GLsizei, const GLvoid *))
GLFUNCTION(IsEnabled, GLboolean, (GLenum))
GLFUNCTION(IsList, GLboolean, (GLuint))
GLFUNCTION(IsTexture, GLboolean, (GLuint))
GLFUNCTION(LightModelf, GLvoid, (GLenum, GLfloat))
GLFUNCTION(LightModelfv, GLvoid, (GLenum, const GLfloat *))
GLFUNCTION(LightModeli, GLvoid, (GLenum, GLint))
GLFUNCTION(LightModeliv, GLvoid, (GLenum, const GLint *))
GLFUNCTION(Lightf, GLvoid, (GLenum, GLenum, GLfloat))
GLFUNCTION(Lightfv, GLvoid, (GLenum, GLenum, const GLfloat *))
GLFUNCTION(Lighti, GLvoid, (GLenum, GLenum, GLint))
GLFUNCTION(Lightiv, GLvoid, (GLenum, GLenum, const GLint *))
GLFUNCTION(LineStipple, GLvoid, (GLint, GLushort))
GLFUNCTION(LineWidth, GLvoid, (GLfloat))
GLFUNCTION(ListBase, GLvoid, (GLuint))
GLFUNCTION(LoadIdentity, GLvoid, ())
GLFUNCTION(LoadMatrixd, GLvoid, (const GLdouble *))
GLFUNCTION(LoadMatrixf, GLvoid, (const GLfloat *))
GLFUNCTION(LoadName, GLvoid, (GLuint))
GLFUNCTION(LogicOp, GLvoid, (GLenum))
GLFUNCTION(Map1d, GLvoid, (GLenum, GLdouble, GLdouble, GLint, GLint, const GLdouble *))
GLFUNCTION(Map1f, GLvoid, (GLenum, GLfloat, GLfloat, GLint, GLint, const GLfloat *))
GLFUNCTION(Map2d, GLvoid, (GLenum, GLdouble, GLdouble, GLint, GLint, GLdouble, GLdouble, GLint, GLint, const GLdouble *))
GLFUNCTION(Map2f, GLvoid, (GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, const GLfloat *))
GLFUNCTION(MapGrid1d, GLvoid, (GLint, GLdouble, GLdouble))
GLFUNCTION(MapGrid1f, GLvoid, (GLint, GLfloat, GLfloat))
GLFUNCTION(MapGrid2d, GLvoid, (GLint, GLdouble, GLdouble, GLint, GLdouble, GLdouble))
GLFUNCTION(MapGrid2f, GLvoid, (GLint, GLfloat, GLfloat, GLint, GLfloat, GLfloat))
GLFUNCTION(Materialf, GLvoid, (GLenum, GLenum, GLfloat))
GLFUNCTION(Materialfv, GLvoid, (GLenum, GLenum, const GLfloat *))
GLFUNCTION(Materiali, GLvoid, (GLenum, GLenum, GLint))
GLFUNCTION(Materialiv, GLvoid, (GLenum, GLenum, const GLint *))
GLFUNCTION(MatrixMode, GLvoid, (GLenum))
GLFUNCTION(MultMatrixd, GLvoid, (const GLdouble *))
GLFUNCTION(MultMatrixf, GLvoid, (const GLfloat *))
GLFUNCTION(NewList, GLvoid, (GLuint, GLenum))
GLFUNCTION(Normal3b, GLvoid, (GLbyte, GLbyte, GLbyte))
GLFUNCTION(Normal3bv, GLvoid, (const GLbyte *))
GLFUNCTION(Normal3d, GLvoid, (GLdouble, GLdouble, GLdouble))
GLFUNCTION(Normal3dv, GLvoid, (const GLdouble *))
GLFUNCTION(Normal3f, GLvoid, (GLfloat, GLfloat, GLfloat))
GLFUNCTION(Normal3fv, GLvoid, (const GLfloat *))
GLFUNCTION(Normal3i, GLvoid, (GLint, GLint, GLint))
GLFUNCTION(Normal3iv, GLvoid, (const GLint *))
GLFUNCTION(Normal3s, GLvoid, (GLshort, GLshort, GLshort))
GLFUNCTION(Normal3sv, GLvoid, (const GLshort *))
GLFUNCTION(NormalPointer, GLvoid, (GLenum, GLsizei, const GLvoid *))
GLFUNCTION(Ortho, GLvoid, (GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble))
GLFUNCTION(PassThrough, GLvoid, (GLfloat))
GLFUNCTION(PixelMapfv, GLvoid, (GLenum, GLsizei, const GLfloat *))
GLFUNCTION(PixelMapuiv, GLvoid, (GLenum, GLsizei, const GLuint *))
GLFUNCTION(PixelMapusv, GLvoid, (GLenum, GLsizei, const GLushort *))
GLFUNCTION(PixelStoref, GLvoid, (GLenum, GLfloat))
GLFUNCTION(PixelStorei, GLvoid, (GLenum, GLint))
GLFUNCTION(PixelTransferf, GLvoid, (GLenum, GLfloat))
GLFUNCTION(PixelTransferi, GLvoid, (GLenum, GLint))
GLFUNCTION(PixelZoom, GLvoid, (GLfloat, GLfloat))
GLFUNCTION(PointSize, GLvoid, (GLfloat))
GLFUNCTION(PolygonMode, GLvoid, (GLenum, GLenum))
GLFUNCTION(PolygonOffset, GLvoid, (GLfloat, GLfloat))
GLFUNCTION(PolygonStipple, GLvoid, (const GLubyte *))
GLFUNCTION(PopAttrib, GLvoid, ())
GLFUNCTION(PopClientAttrib, GLvoid, ())
GLFUNCTION(PopMatrix, GLvoid, ())
GLFUNCTION(PopName, GLvoid, ())
GLFUNCTION(PrioritizeTextures, GLvoid, (GLsizei, const GLuint *, const GLclampf *))
GLFUNCTION(PushAttrib, GLvoid, (GLbitfield))
GLFUNCTION(PushClientAttrib, GLvoid, (GLbitfield))
GLFUNCTION(PushMatrix, GLvoid, ())
GLFUNCTION(PushName, GLvoid, (GLuint))
GLFUNCTION(RasterPos2d, GLvoid, (GLdouble, GLdouble))
GLFUNCTION(RasterPos2dv, GLvoid, (const GLdouble *))
GLFUNCTION(RasterPos2f, GLvoid, (GLfloat, GLfloat))
GLFUNCTION(RasterPos2fv, GLvoid, (const GLfloat *))
GLFUNCTION(RasterPos2i, GLvoid, (GLint, GLint))
GLFUNCTION(RasterPos2iv, GLvoid, (const GLint *))
GLFUNCTION(RasterPos2s, GLvoid, (GLshort, GLshort))
GLFUNCTION(RasterPos2sv, GLvoid, (const GLshort *))
GLFUNCTION(RasterPos3d, GLvoid, (GLdouble, GLdouble, GLdouble))
GLFUNCTION(RasterPos3dv, GLvoid, (const GLdouble *))
GLFUNCTION(RasterPos3f, GLvoid, (GLfloat, GLfloat, GLfloat))
GLFUNCTION(RasterPos3fv, GLvoid, (const GLfloat *))
GLFUNCTION(RasterPos3i, GLvoid, (GLint, GLint, GLint))
GLFUNCTION(RasterPos3iv, GLvoid, (const GLint *))
GLFUNCTION(RasterPos3s, GLvoid, (GLshort, GLshort, GLshort))
GLFUNCTION(RasterPos3sv, GLvoid, (const GLshort *))
GLFUNCTION(RasterPos4d, GLvoid, (GLdouble, GLdouble, GLdouble, GLdouble))
GLFUNCTION(RasterPos4dv, GLvoid, (const GLdouble *))
GLFUNCTION(RasterPos4f, GLvoid, (GLfloat, GLfloat, GLfloat, GLfloat))
GLFUNCTION(RasterPos4fv, GLvoid, (const GLfloat *))
GLFUNCTION(RasterPos4i, GLvoid, (GLint, GLint, GLint, GLint))
GLFUNCTION(RasterPos4iv, GLvoid, (const GLint *))
GLFUNCTION(RasterPos4s, GLvoid, (GLshort, GLshort, GLshort, GLshort))
GLFUNCTION(RasterPos4sv, GLvoid, (const GLshort *))
GLFUNCTION(ReadBuffer, GLvoid, (GLenum))
GLFUNCTION(ReadPixels, GLvoid, (GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *))
GLFUNCTION(Rectd, GLvoid, (GLdouble, GLdouble, GLdouble, GLdouble))
GLFUNCTION(Rectdv, GLvoid, (const GLdouble *, const GLdouble *))
GLFUNCTION(Rectf, GLvoid, (GLfloat, GLfloat, GLfloat, GLfloat))
GLFUNCTION(Rectfv, GLvoid, (const GLfloat *, const GLfloat *))
GLFUNCTION(Recti, GLvoid, (GLint, GLint, GLint, GLint))
GLFUNCTION(Rectiv, GLvoid, (const GLint *, const GLint *))
GLFUNCTION(Rects, GLvoid, (GLshort, GLshort, GLshort, GLshort))
GLFUNCTION(Rectsv, GLvoid, (const GLshort *, const GLshort *))
GLFUNCTION(RenderMode, GLint, (GLenum))
GLFUNCTION(Rotated, GLvoid, (GLdouble, GLdouble, GLdouble, GLdouble))
GLFUNCTION(Rotatef, GLvoid, (GLfloat, GLfloat, GLfloat, GLfloat))
GLFUNCTION(Scaled, GLvoid, (GLdouble, GLdouble, GLdouble))
GLFUNCTION(Scalef, GLvoid, (GLfloat, GLfloat, GLfloat))
GLFUNCTION(Scissor, GLvoid, (GLint, GLint, GLsizei, GLsizei))
GLFUNCTION(SelectBuffer, GLvoid, (GLsizei, GLuint *))
GLFUNCTION(ShadeModel, GLvoid, (GLenum))
GLFUNCTION(StencilFunc, GLvoid, (GLenum, GLint, GLuint))
GLFUNCTION(StencilMask, GLvoid, (GLuint))
GLFUNCTION(StencilOp, GLvoid, (GLenum, GLenum, GLenum))
GLFUNCTION(TexCoord1d, GLvoid, (GLdouble))
GLFUNCTION(TexCoord1dv, GLvoid, (const GLdouble *))
GLFUNCTION(TexCoord1f, GLvoid, (GLfloat))
GLFUNCTION(TexCoord1fv, GLvoid, (const GLfloat *))
GLFUNCTION(TexCoord1i, GLvoid, (GLint))
GLFUNCTION(TexCoord1iv, GLvoid, (const GLint *))
GLFUNCTION(TexCoord1s, GLvoid, (GLshort))
GLFUNCTION(TexCoord1sv, GLvoid, (const GLshort *))
GLFUNCTION(TexCoord2d, GLvoid, (GLdouble, GLdouble))
GLFUNCTION(TexCoord2dv, GLvoid, (const GLdouble *))
GLFUNCTION(TexCoord2f, GLvoid, (GLfloat, GLfloat))
GLFUNCTION(TexCoord2fv, GLvoid, (const GLfloat *))
GLFUNCTION(TexCoord2i, GLvoid, (GLint, GLint))
GLFUNCTION(TexCoord2iv, GLvoid, (const GLint *))
GLFUNCTION(TexCoord2s, GLvoid, (GLshort, GLshort))
GLFUNCTION(TexCoord2sv, GLvoid, (const GLshort *))
GLFUNCTION(TexCoord3d, GLvoid, (GLdouble, GLdouble, GLdouble))
GLFUNCTION(TexCoord3dv, GLvoid, (const GLdouble *))
GLFUNCTION(TexCoord3f, GLvoid, (GLfloat, GLfloat, GLfloat))
GLFUNCTION(TexCoord3fv, GLvoid, (const GLfloat *))
GLFUNCTION(TexCoord3i, GLvoid, (GLint, GLint, GLint))
GLFUNCTION(TexCoord3iv, GLvoid, (const GLint *))
GLFUNCTION(TexCoord3s, GLvoid, (GLshort, GLshort, GLshort))
GLFUNCTION(TexCoord3sv, GLvoid, (const GLshort *))
GLFUNCTION(TexCoord4d, GLvoid, (GLdouble, GLdouble, GLdouble, GLdouble))
GLFUNCTION(TexCoord4dv, GLvoid, (const GLdouble *))
GLFUNCTION(TexCoord4f, GLvoid, (GLfloat, GLfloat, GLfloat, GLfloat))
GLFUNCTION(TexCoord4fv, GLvoid, (const GLfloat *))
GLFUNCTION(TexCoord4i, GLvoid, (GLint, GLint, GLint, GLint))
GLFUNCTION(TexCoord4iv, GLvoid, (const GLint *))
GLFUNCTION(TexCoord4s, GLvoid, (GLshort, GLshort, GLshort, GLshort))
GLFUNCTION(TexCoord4sv, GLvoid, (const GLshort *))
GLFUNCTION(TexCoordPointer, GLvoid, (GLint, GLenum, GLsizei, const GLvoid *))
GLFUNCTION(TexEnvf, GLvoid, (GLenum, GLenum, GLfloat))
GLFUNCTION(TexEnvfv, GLvoid, (GLenum, GLenum, const GLfloat *))
GLFUNCTION(TexEnvi, GLvoid, (GLenum, GLenum, GLint))
GLFUNCTION(TexEnviv, GLvoid, (GLenum, GLenum, const GLint *))
GLFUNCTION(TexGend, GLvoid, (GLenum, GLenum, GLdouble))
GLFUNCTION(TexGendv, GLvoid, (GLenum, GLenum, const GLdouble *))
GLFUNCTION(TexGenf, GLvoid, (GLenum, GLenum, GLfloat))
GLFUNCTION(TexGenfv, GLvoid, (GLenum, GLenum, const GLfloat *))
GLFUNCTION(TexGeni, GLvoid, (GLenum, GLenum, GLint))
GLFUNCTION(TexGeniv, GLvoid, (GLenum, GLenum, const GLint *))
GLFUNCTION(TexImage1D, GLvoid, (GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid *))
GLFUNCTION(TexImage2D, GLvoid, (GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *))
GLFUNCTION(TexParameterf, GLvoid, (GLenum, GLenum, GLfloat))
GLFUNCTION(TexParameterfv, GLvoid, (GLenum, GLenum, const GLfloat *))
GLFUNCTION(TexParameteri, GLvoid, (GLenum, GLenum, GLint))
GLFUNCTION(TexParameteriv, GLvoid, (GLenum, GLenum, const GLint *))
GLFUNCTION(TexSubImage1D, GLvoid, (GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *))
GLFUNCTION(TexSubImage2D, GLvoid, (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))
GLFUNCTION(Translated, GLvoid, (GLdouble, GLdouble, GLdouble))
GLFUNCTION(Translatef, GLvoid, (GLfloat, GLfloat, GLfloat))
GLFUNCTION(Vertex2d, GLvoid, (GLdouble, GLdouble))
GLFUNCTION(Vertex2dv, GLvoid, (const GLdouble *))
GLFUNCTION(Vertex2f, GLvoid, (GLfloat, GLfloat))
GLFUNCTION(Vertex2fv, GLvoid, (const GLfloat *))
GLFUNCTION(Vertex2i, GLvoid, (GLint, GLint))
GLFUNCTION(Vertex2iv, GLvoid, (const GLint *))
GLFUNCTION(Vertex2s, GLvoid, (GLshort, GLshort))
GLFUNCTION(Vertex2sv, GLvoid, (const GLshort *))
GLFUNCTION(Vertex3d, GLvoid, (GLdouble, GLdouble, GLdouble))
GLFUNCTION(Vertex3dv, GLvoid, (const GLdouble *))
GLFUNCTION(Vertex3f, GLvoid, (GLfloat, GLfloat, GLfloat))
GLFUNCTION(Vertex3fv, GLvoid, (const GLfloat *))
GLFUNCTION(Vertex3i, GLvoid, (GLint, GLint, GLint))
GLFUNCTION(Vertex3iv, GLvoid, (const GLint *))
GLFUNCTION(Vertex3s, GLvoid, (GLshort, GLshort, GLshort))
GLFUNCTION(Vertex3sv, GLvoid, (const GLshort *))
GLFUNCTION(Vertex4d, GLvoid, (GLdouble, GLdouble, GLdouble, GLdouble))
GLFUNCTION(Vertex4dv, GLvoid, (const GLdouble *))
GLFUNCTION(Vertex4f, GLvoid, (GLfloat, GLfloat, GLfloat, GLfloat))
GLFUNCTION(Vertex4fv, GLvoid, (const GLfloat *v))
GLFUNCTION(Vertex4i, GLvoid, (GLint, GLint, GLint, GLint))
GLFUNCTION(Vertex4iv, GLvoid, (const GLint *))
GLFUNCTION(Vertex4s, GLvoid, (GLshort, GLshort, GLshort, GLshort))
GLFUNCTION(Vertex4sv, GLvoid, (const GLshort *))
GLFUNCTION(VertexPointer, GLvoid, (GLint, GLenum, GLsizei, const GLvoid *))
GLFUNCTION(Viewport, GLvoid, (GLint, GLint, GLsizei, GLsizei))

GLSTRING(EXTENSIONS)
GLINTEGER(MAX_ATTRIB_STACK_DEPTH)
GLINTEGER(MAX_CLIENT_ATTRIB_STACK_DEPTH)
GLINTEGER(MAX_CLIP_PLANES)
GLINTEGER(MAX_EVAL_ORDER)
GLINTEGER(MAX_LIGHTS)
GLINTEGER(MAX_LIST_NESTING)
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

GLEXTENSION(ARB_texture_non_power_of_two)
	GLEND

GLEXTENSION(ARB_texture_rectangle)
	GLINTEGER(MAX_RECTANGLE_TEXTURE_SIZE_ARB)
	GLEND

GLEXTENSION(EXT_texture_filter_anisotropic)
	GLFLOAT(MAX_TEXTURE_MAX_ANISOTROPY_EXT)
	GLEND

GLEXTENSION(SGIS_generate_mipmap)
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
