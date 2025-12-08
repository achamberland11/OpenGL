#version 330 core

in vec2 v_texCoord;
out vec4 fragColor;

uniform sampler2D s_inputTexture;

void main()
{
	vec4 inputColor = texture(s_inputTexture, v_texCoord);
	float avg = (inputColor.x + inputColor.y + inputColor.z) / 3;
	fragColor = vec4(avg, avg, avg, 1);
}
