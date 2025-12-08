#version 430 core

layout(location = 0) out vec4 o_color;

uniform sampler2D s_inputTexture;

layout(std140) uniform Params
{
	float u_intensity;
};

void main()
{
	vec2 fragCoord = gl_FragCoord.xy;
	vec2 resolution = vec2(textureSize(s_inputTexture, 0));
	
	vec2 uv = fragCoord / resolution;
	vec4 color = texture(s_inputTexture, uv);
	
	// Calculate distance from center (0.0 at center, ~0.7 at corners)
	vec2 centerOffset = uv - vec2(0.5, 0.5);
	float dist = length(centerOffset);
	
	// Vignette that grows from edges to center based on intensity
	// At intensity 0: no darkening
	// At intensity 1: entire screen is dark
	float vignette = 1.0 - smoothstep(1.0 - u_intensity, 1.0, dist / 0.7);
	
	o_color = vec4(color.rgb * vignette, color.a);
}
