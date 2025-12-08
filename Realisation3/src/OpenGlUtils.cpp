#include "OpenGlUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

OpenGl::CTexture OpenGl::LoadTextureFromFile(const char* path)
{
	int width = 0, height = 0, components = 0;
	stbi_uc* image = stbi_load(path, &width, &height, &components, 4);
	if(!image)
	{
		printf("Failed to load image: %s.\r\n", stbi_failure_reason());
		assert(false);
	}

	auto texture = OpenGl::CTexture::Create();

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(image);

	return texture;
}
