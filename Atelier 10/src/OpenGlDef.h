#pragma once

#ifdef __APPLE__

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#else

#include <glad/gl.h>

#endif

#define CHECKGLERROR()                                       \
	{                                                        \
		auto errorCode = glGetError();                       \
		if(errorCode != GL_NO_ERROR)                         \
		{                                                    \
			printf("GL error %d triggered.\r\n", errorCode); \
			assert(false);                                   \
		}                                                    \
	}
