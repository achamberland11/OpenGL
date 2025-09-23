#pragma once

#include "OpenGlDef.h"
#include <memory>
#include <cassert>
#include <vector>

namespace OpenGl
{
	class CProgram
	{
	public:
		CProgram() = default;
		CProgram(const CProgram&) = delete;

		CProgram(CProgram&& rhs)
		{
			MoveFrom(std::move(rhs));
		}

		~CProgram()
		{
			Reset();
		}

		CProgram& operator=(const CProgram&) = delete;

		CProgram& operator=(CProgram&& rhs)
		{
			Reset();
			MoveFrom(std::move(rhs));
			return (*this);
		}

		static CProgram Create()
		{
			CProgram program;
			program.m_handle = glCreateProgram();
			return program;
		}

		void Reset()
		{
			if(m_handle != 0)
			{
				glDeleteProgram(m_handle);
			}
			m_handle = 0;
		}

		operator GLuint() const
		{
			return m_handle;
		}

		void AttachShader(GLuint shaderHandle)
		{
			glAttachShader(m_handle, shaderHandle);
		}

		bool Link()
		{
			GLint status = GL_FALSE;

			glLinkProgram(m_handle);
			glGetProgramiv(m_handle, GL_LINK_STATUS, &status);

			if(status == GL_FALSE)
			{
				GLint length = 0;
				glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &length);
				std::vector<char> errorString(length + 1);
				glGetProgramInfoLog(m_handle, length + 1, &length, errorString.data());
				errorString[length] = 0;
				printf("%s\r\n", errorString.data());
				assert(0);
			}

			return (status == GL_TRUE);
		}

		bool Validate()
		{
			GLint status = GL_FALSE;

			glValidateProgram(m_handle);
			glGetProgramiv(m_handle, GL_VALIDATE_STATUS, &status);

			if(status == GL_FALSE)
			{
				GLint length = 0;
				glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &length);
				char* error = reinterpret_cast<char*>(alloca(length + 1));
				glGetProgramInfoLog(m_handle, length + 1, &length, error);
				error[length] = 0;
				assert(0);
			}

			return (status == GL_TRUE);
		}

	private:
		void MoveFrom(CProgram&& rhs)
		{
			assert(m_handle == 0);
			std::swap(m_handle, rhs.m_handle);
		}

		GLuint m_handle = 0;
	};
}
