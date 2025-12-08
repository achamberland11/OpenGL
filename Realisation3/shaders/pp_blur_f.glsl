#version 330 core

in vec2 v_texCoord;
out vec4 fragColor;

uniform sampler2D s_inputTexture;

layout(std140) uniform Params
{
	int tapDim;
};

void main()
{
	int tapCount = (tapDim * 2) * (tapDim * 2);
	vec4 totalColor = vec4(0);
	for(int y = -tapDim; y < tapDim; y++)
	{
		for(int x = -tapDim; x < tapDim; x++)
		{
			vec4 inputColor = texelFetch(s_inputTexture, ivec2(gl_FragCoord.xy) + ivec2(x, y), 0);
			totalColor += inputColor;
		}
	}
	fragColor = totalColor / vec4(tapCount);
}
