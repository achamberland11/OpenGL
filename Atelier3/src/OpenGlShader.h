#pragma once

#include <cstddef>
#include <algorithm>
#include <string>
#include <cassert>
#include <filesystem>
#include "OpenGlDef.h"

namespace OpenGl
{
	class CShader
	{
	public:
		CShader() = default;
		CShader(const CShader&) = delete;

		CShader(CShader&& src)
		{
			std::swap(src.m_handle, m_handle);
		}

		~CShader()
		{
			if(m_handle != 0)
			{
				glDeleteShader(m_handle);
			}
		}

		CShader& operator=(const CShader&) = delete;

		CShader& operator=(CShader&&);

		static CShader CreateFromFile(GLenum type, const std::filesystem::path& sourcePath)
		{
			CShader shader;
			shader.m_handle = glCreateShader(type);
			shader.SetSourceFromFile(sourcePath);
			return shader;
		}

		operator GLuint() const
		{
			return m_handle;
		}

		void SetSource(const std::string& source)
		{
			GLint size = static_cast<GLint>(source.size());
			const char* sourcePtr = source.c_str();
			glShaderSource(m_handle, 1, &sourcePtr, &size);
		}

		void SetSourceFromFile(const std::filesystem::path& shaderPath)
		{
			std::string result;

			{
				char buffer[256];
				FILE* stream = fopen(shaderPath.string().c_str(), "rb");
				assert(stream);
				while(1)
				{
					size_t amountRead = fread(buffer, 1, sizeof(buffer), stream);
					result += std::string(buffer, buffer + amountRead);
					if(feof(stream))
					{
						break;
					}
				}

				fclose(stream);
			}

			SetSource(result);
		}

		bool Compile()
		{
			GLint status = GL_FALSE;

			glCompileShader(m_handle);
			glGetShaderiv(m_handle, GL_COMPILE_STATUS, &status);

			if(status == GL_FALSE)
			{
				GLint length = 0;
				glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &length);
				std::vector<char> errorString(length + 1);
				glGetShaderInfoLog(m_handle, length + 1, &length, errorString.data());
				errorString[length] = 0;
				printf("%s\r\n", errorString.data());
				assert(0);
			}

			return (status == GL_TRUE);
		}

	private:
		GLuint m_handle = 0;
	};
}
