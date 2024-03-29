// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "gl.h"

namespace seir
{
	class Mat4;
}

namespace Yt
{
	class GlBufferHandle
	{
	public:
		GlBufferHandle(const GlApi&, GLenum target);
		GlBufferHandle(GlBufferHandle&&) noexcept;
		~GlBufferHandle() noexcept;

		void bind() const noexcept;
		GLuint get() const noexcept { return _handle; }
		void initialize(GLenum usage, size_t size, const void* data) noexcept;
		GLuint size() const noexcept { return _size; }
		void unbind() const noexcept;
		void write(size_t offset, size_t size, const void* data) const noexcept;

		GlBufferHandle(const GlBufferHandle&) = delete;
		GlBufferHandle& operator=(const GlBufferHandle&) = delete;
		GlBufferHandle& operator=(GlBufferHandle&&) = delete;

	private:
		const GlApi& _gl;
		const GLenum _target;
		GLuint _handle = 0;
		GLuint _size = 0;
	};

	class GlProgramHandle
	{
	public:
		explicit GlProgramHandle(const GlApi&);
		GlProgramHandle(GlProgramHandle&&) noexcept;
		~GlProgramHandle();

		void attach(GLuint) const;
		GLuint get() const { return _handle; }
		std::string info_log() const;
		bool link() const;
		void set_uniform(const char*, const seir::Mat4&) const;

		GlProgramHandle(const GlProgramHandle&) = delete;
		GlProgramHandle& operator=(const GlProgramHandle&) = delete;
		GlProgramHandle& operator=(GlProgramHandle&&) = delete;

	private:
		const GlApi& _gl;
		GLuint _handle = 0;
	};

	class GlShaderHandle
	{
	public:
		GlShaderHandle(const GlApi&, GLenum type);
		GlShaderHandle(GlShaderHandle&&) noexcept;
		~GlShaderHandle();

		bool compile(std::string_view) const;
		GLuint get() const { return _handle; }
		std::string info_log() const;

		GlShaderHandle(const GlShaderHandle&) = delete;
		GlShaderHandle& operator=(const GlShaderHandle&) = delete;
		GlShaderHandle& operator=(GlShaderHandle&&) = delete;

	private:
		const GlApi& _gl;
		const GLenum _type;
		GLuint _handle = 0;
	};

	class GlTextureHandle
	{
	public:
		GlTextureHandle(const GlApi&, GLenum target);
		GlTextureHandle(GlTextureHandle&&) noexcept;
		~GlTextureHandle();

		void bind() const;
		void generate_mipmaps() const;
		void set_anisotropy_enabled(bool) const;
		void set_data(GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) const;
		void set_parameter(GLenum, GLint) const;

		GlTextureHandle(const GlTextureHandle&) = delete;
		GlTextureHandle& operator=(const GlTextureHandle&) = delete;
		GlTextureHandle& operator=(GlTextureHandle&&) = delete;

	private:
		const GlApi& _gl;
		const GLenum _target;
		GLuint _handle = 0;
	};

	class GlVertexArrayHandle
	{
	public:
		explicit GlVertexArrayHandle(const GlApi&);
		GlVertexArrayHandle(GlVertexArrayHandle&&) noexcept;
		~GlVertexArrayHandle() noexcept;

		void bind() const noexcept;
		void bind_vertex_buffer(GLuint binding, GLuint buffer, size_t offset, size_t stride) noexcept;
		void unbind() const noexcept;
		void vertex_attrib_binding(GLuint attrib, GLuint binding) noexcept;
		void vertex_attrib_format(GLuint attrib, GLint size, GLenum type, GLboolean normalized, size_t offset) noexcept;

		GlVertexArrayHandle(const GlVertexArrayHandle&) = delete;
		GlVertexArrayHandle& operator=(const GlVertexArrayHandle&) = delete;
		GlVertexArrayHandle& operator=(GlVertexArrayHandle&&) = delete;

	private:
		const GlApi& _gl;
		GLuint _handle = 0;
		uint32_t _attributes = 0;
	};
}
