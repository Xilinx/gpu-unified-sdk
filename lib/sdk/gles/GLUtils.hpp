
#pragma once

#include <filesystem>
#include <string_view>
#include <unordered_map>
#include <utility>

#ifdef SDK_USE_EPOXY
#include <epoxy/gl.h>
#else
#include <GLES2/gl2.h>
#endif




enum class GLVariant {
	Desktop,
	ES2,
#ifdef SDK_USE_GLES3
	ES3,
#endif
};




class TextureId {
public:
	static TextureId null() noexcept { return TextureId(0); }

	static TextureId create() noexcept
	{
		GLuint id;
		glGenTextures(1, &id);
		return TextureId(id);
	}

	~TextureId() noexcept { if (id_ != 0) glDeleteTextures(1, &id_); }

	TextureId(TextureId && other) noexcept : id_(std::exchange(other.id_, 0)) {}
	TextureId & operator=(TextureId && other) noexcept { std::swap(id_, other.id_); return *this; }

	TextureId(const TextureId &) = delete;
	void operator=(const TextureId &) = delete;

	bool isNull() const noexcept { return id_ == 0; }
	GLuint id() const noexcept { return id_; }
	operator GLuint() const noexcept { return id_; }

private:
	TextureId(GLuint id) noexcept : id_(id) {}

	GLuint id_;
};




class RenderbufferId {
public:
	static RenderbufferId null() noexcept { return RenderbufferId(0); }

	static RenderbufferId create() noexcept
	{
		GLuint id;
		glGenRenderbuffers(1, &id);
		return RenderbufferId(id);
	}

	~RenderbufferId() noexcept { if (id_ != 0) glDeleteRenderbuffers(1, &id_); }

	RenderbufferId(RenderbufferId && other) noexcept : id_(std::exchange(other.id_, 0)) {}
	RenderbufferId & operator=(RenderbufferId && other) noexcept { std::swap(id_, other.id_); return *this; }

	RenderbufferId(const RenderbufferId &) = delete;
	void operator=(const RenderbufferId &) = delete;

	bool isNull() const noexcept { return id_ == 0; }
	GLuint id() const noexcept { return id_; }
	operator GLuint() const noexcept { return id_; }

private:
	RenderbufferId(GLuint id) noexcept : id_(id) {}

	GLuint id_;
};




class FboId {
public:
	static FboId null() noexcept { return FboId(0); }

	static FboId create() noexcept
	{
		GLuint id;
		glGenFramebuffers(1, &id);
		return FboId(id);
	}

	~FboId() noexcept { if (id_ != 0) glDeleteFramebuffers(1, &id_); }

	FboId(FboId && other) noexcept : id_(std::exchange(other.id_, 0)) {}
	FboId & operator=(FboId && other) noexcept { std::swap(id_, other.id_); return *this; }

	FboId(const FboId &) = delete;
	void operator=(const FboId &) = delete;

	GLuint id() const noexcept { return id_; }
	operator GLuint() const noexcept { return id_; }

private:
	FboId(GLuint id) noexcept : id_(id) {}

	GLuint id_;
};




class BufferObject {
public:
	static BufferObject null() noexcept { return BufferObject(0); }

	static BufferObject create() noexcept
	{
		GLuint id;
		glGenBuffers(1, &id);
		return BufferObject(id);
	}

	~BufferObject() noexcept
	{
		glDeleteBuffers(1, &id_);
	}

	BufferObject(BufferObject && other) noexcept : id_(std::exchange(other.id_, 0)) {}
	BufferObject & operator=(BufferObject && other) noexcept { std::swap(id_, other.id_); return *this; }

	BufferObject(const BufferObject &) = delete;
	void operator=(const BufferObject &) = delete;

	GLuint id() const noexcept { return id_; }
	operator GLuint() const noexcept { return id_; }

private:
	BufferObject(GLuint id) noexcept : id_(id) {}

	GLuint id_;
};




class ShaderProgram {
public:
	struct Params {
		std::unordered_map<std::string, std::string> defines;
	};

	static ShaderProgram null() noexcept { return ShaderProgram(0); }

	static ShaderProgram createFromString(const std::string_view & vertex,
			const std::string_view & fragment, const Params & params)
	{ return _create(&vertex, 1, &fragment, 1, params); }

	template <size_t VS, size_t FS>
	static ShaderProgram createFromArray(const std::string_view (& vertex)[VS],
			const std::string_view (&fragment)[FS], const Params & params)
	{ return _create(vertex, VS, fragment, FS, params); }

	static ShaderProgram createFromFile(const std::filesystem::path & vertex,
			const std::filesystem::path & fragment, const Params & params);

#ifdef SDK_USE_SHADER_PROGRAM_FOLDERS
	static ShaderProgram createFromName(const std::string_view & name, const Params & params);
#endif

	static ShaderProgram adapt(GLuint id) noexcept { return ShaderProgram(id); }

	~ShaderProgram() noexcept;

	ShaderProgram(ShaderProgram && other) noexcept : id_(std::exchange(other.id_, 0)) {}
	ShaderProgram & operator=(ShaderProgram && other) noexcept { std::swap(id_, other.id_); return *this; }

	GLuint id() const noexcept { return id_; }
	operator GLuint() const noexcept { return id_; }

	ShaderProgram(const ShaderProgram &) = delete;
	void operator=(const ShaderProgram &) = delete;

private:
	ShaderProgram(GLuint id) noexcept : id_(id) {}

	static ShaderProgram _create(const std::string_view * vertex, int vertexSize,
			const std::string_view * fragment, int fragmentSize, const Params & params);

	GLuint id_;
};




extern void glErrorReset() noexcept;
extern int glErrorCount() noexcept;
extern void checkGLError(const char * file, int line, const char * func);

#define CHECK_GL_ERROR() \
	checkGLError(__FILE__, __LINE__, __FUNCTION__);
