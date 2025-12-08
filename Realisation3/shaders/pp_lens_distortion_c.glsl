#version 430 core

layout(local_size_x = 16, local_size_y = 16) in;
layout(binding = 0, RGBA8) uniform image2D s_workImage;

layout(std140) uniform Params
{
	float k1;
	float k2;
	float p1;
	float p2;
};

vec2 applyDistortion(vec2 uv)
{
	vec2 normalized = (uv - vec2(0.5)) * 2.0;
	
	float x = normalized.x;
	float y = normalized.y;
	
	float r2 = x * x + y * y;
	float r4 = r2 * r2;
	
	float radialDistortion = 1.0 + k1 * r2 + k2 * r4;
	
	vec2 distorted;
	distorted.x = x * radialDistortion;
	distorted.y = y * radialDistortion;
	
	distorted.x += 2.0 * p1 * x * y + p2 * (r2 + 2.0 * x * x);
	distorted.y += p1 * (r2 + 2.0 * y * y) + 2.0 * p2 * x * y;
	
	return (distorted / 2.0) + vec2(0.5);
}

void main()
{
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 imageSize = imageSize(s_workImage);
	
	if(pixelCoord.x >= imageSize.x || pixelCoord.y >= imageSize.y)
		return;
	
	vec2 uv = vec2(pixelCoord) / vec2(imageSize);
	
	vec2 distortedUV = applyDistortion(uv);
	
	vec4 color;
	if(distortedUV.x < 0.0 || distortedUV.x > 1.0 || distortedUV.y < 0.0 || distortedUV.y > 1.0)
	{
		color = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else
	{
		ivec2 sampleCoord = ivec2(distortedUV * vec2(imageSize));
		sampleCoord = clamp(sampleCoord, ivec2(0), imageSize - ivec2(1));
		color = imageLoad(s_workImage, sampleCoord);
	}
	
	imageStore(s_workImage, pixelCoord, color);
}
