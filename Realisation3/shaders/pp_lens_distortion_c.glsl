#version 430 core

layout(local_size_x = 16, local_size_y = 16) in;
layout(binding = 0, RGBA8) uniform image2D s_workImage;

layout(std140) uniform Params
{
	float u_curvature;
};

void main()
{
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 imageSize = imageSize(s_workImage);
	
	if(pixelCoord.x >= imageSize.x || pixelCoord.y >= imageSize.y)
		return;
	
	vec2 uv = vec2(pixelCoord) / vec2(imageSize);
	vec2 center = uv - vec2(0.5, 0.5);
	
	float r2 = dot(center, center);
	float distortion = 1.0 + u_curvature * r2;
	vec2 distortedUV = center * distortion + vec2(0.5, 0.5);
	
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
