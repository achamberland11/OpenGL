#version 330 core

in vec2 v_texCoord;

out vec4 fragColor;

uniform sampler2D s_texture;

void main()
{
	fragColor = texture(s_texture, v_texCoord);
}
