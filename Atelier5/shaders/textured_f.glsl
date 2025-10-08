#version 330 core

in vec2 v_texCoord;

out vec4 fragColor;

uniform sampler2D s_texture1;
uniform sampler2D s_texture2;
uniform sampler2D s_textureMask;

void main()
{ 
	vec4 color1 = texture(s_texture1, v_texCoord);
	vec4 color2 = texture(s_texture2, v_texCoord);
	vec4 mask = texture(s_textureMask, v_texCoord);
	fragColor = mix(color1, color2, mask.a);
}
