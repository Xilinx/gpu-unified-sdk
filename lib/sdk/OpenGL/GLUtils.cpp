/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#include "GLUtils.hpp"

#include <fstream>
#include <functional>
#include <memory>
#include <vector>

#include "FileUtils.hpp"
#ifdef SDK_USE_SHADER_PROGRAM_FOLDERS
#include "Folders.hpp"
#endif
#include "Log.hpp"




ShaderProgram ShaderProgram::createFromFile(const std::filesystem::path & vertex,
		const std::filesystem::path & fragment, const Params & params)
{
	const std::string vertexString = readTextFile(vertex);
	const std::string fragmentString = readTextFile(fragment);

	const std::string_view vs = vertexString;
	const std::string_view fs = fragmentString;

	return _create(&vs, 1, &fs, 1, params);
}


#ifdef SDK_USE_SHADER_PROGRAM_FOLDERS
ShaderProgram ShaderProgram::createFromName(const std::string_view & name, const Params & params)
{
	const std::filesystem::path vertexPath =
			Folders::get().shadersDir / (std::string(name) + ".vert.glsl");
	const std::filesystem::path fragmentPath =
			Folders::get().shadersDir / (std::string(name) + ".frag.glsl");
	return createFromFile(vertexPath, fragmentPath, params);
}
#endif


ShaderProgram ShaderProgram::_create(const std::string_view * const vertex, const int vertexSize,
		const std::string_view * const fragment, const int fragmentSize, const Params & params)
{
	const auto glue = [&params] (const std::string_view * const code, const int size)
			-> std::string {
		std::string_view versionLine;
		std::string_view code0 = code[0];
		if (code0.starts_with("#version")) {
			const auto end = code0.find('\n');
			CHECK(end != std::string_view::npos)
			versionLine = code0.substr(0, end + 1);
			code0 = code0.substr(end + 1);
		}

		std::ostringstream s;

		s << versionLine;

		int linesAdded = 0;

		if (!params.defines.empty()) {
			for (const auto & d : params.defines) {
				const std::string & name = d.first;
				const std::string & value = d.second;
				s << "#define " << name;
				if (!value.empty()) {
					s << " " << value;
				}
				s << "\n";
			}
			linesAdded += params.defines.size();
		}

		if (linesAdded != 0) {
			s << "#line 1\n";
		}

		s << code0;
		for (int i = 1; i < size; ++i) {
			s << code[i];
		}

		return s.str();
	};

	struct shader_t {
		static shader_t * fromId(GLuint id) noexcept { return reinterpret_cast<shader_t*>(intptr_t(id)); }
		GLuint id() const noexcept { return intptr_t(this); }
	};

	struct shader_deleter_t {
		void operator()(shader_t * s) noexcept { glDeleteShader(s->id()); }
	};

	using Shader = std::unique_ptr<shader_t, shader_deleter_t>;

	const auto compile = [] (const GLenum type, const std::string_view & source) -> Shader {
		CHECK_GL_ERROR()
		const GLuint shader = glCreateShader(type);
		CHECK_GL_ERROR()
		const char * const sourcePointer = source.data();
		glShaderSource(shader, 1, &sourcePointer, nullptr);
		glCompileShader(shader);

		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) [[unlikely]] {
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength + 1);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
			THROW("shader compile error:", errorLog.data(), "type:", type == GL_VERTEX_SHADER ? "vertex" : "fragment")
		}

		return Shader(shader_t::fromId(shader), shader_deleter_t());
	};

	CHECK_GL_ERROR()
	Shader vs = compile(GL_VERTEX_SHADER, glue(vertex, vertexSize));
	CHECK_GL_ERROR()
	Shader fs = compile(GL_FRAGMENT_SHADER, glue(fragment, fragmentSize));

	CHECK_GL_ERROR()
	const GLuint programId = glCreateProgram();
	CHECK_GL_ERROR()
	CHECK(programId != 0)

	ShaderProgram program(programId);

	glAttachShader(programId, vs->id());
	glAttachShader(programId, fs->id());
	glLinkProgram(programId);

	GLint linked;
	glGetProgramiv(programId, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint maxLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(programId, maxLength, &maxLength, &errorLog[0]);
		THROW("shader link error:", errorLog.data())
	}

	glDetachShader(programId, vs->id());
	glDetachShader(programId, fs->id());

	return program;
}


ShaderProgram::~ShaderProgram() noexcept
{
	if (id_ != 0) {
		glDeleteProgram(id_);
	}
}




struct ThreadLocal {
	int count = 0;
};

thread_local ThreadLocal gl_error_tl;


void glErrorReset() noexcept
{
	gl_error_tl.count = 0;
}


int glErrorCount() noexcept
{
	return gl_error_tl.count;
}


void checkGLError(const char * const file, const int line, const char * const func)
{
	gl_error_tl.count++;
	const GLenum err = glGetError();
	if (err != GL_NO_ERROR) [[unlikely]] {
		throw (ExceptionMaker(Log::context(Log::Level::Error, file, line, func,
				LOG_MAKE_STACKTRACE)),
				"GL error:", Log::hex(err, 2)).make();
	}
}
