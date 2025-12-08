#version 430 core

#define TAP_DIM 4
#define WORKGROUP_SIZE 32

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1) in;
layout(binding=0, RGBA8) uniform image2D s_workImage;

shared vec4 sharedData[WORKGROUP_SIZE + (TAP_DIM * 2)];

void main()
{
	int absPos = int(gl_LocalInvocationID.x);
	int localPos = absPos + TAP_DIM;

	if(absPos < TAP_DIM)
	{
		sharedData[absPos] = imageLoad(s_workImage, ivec2(gl_GlobalInvocationID.x - TAP_DIM, gl_GlobalInvocationID.y));
	}
	if(absPos >= (WORKGROUP_SIZE - TAP_DIM))
	{
		sharedData[absPos + (2 * TAP_DIM)] = imageLoad(s_workImage, ivec2(gl_GlobalInvocationID.x + TAP_DIM, gl_GlobalInvocationID.y));
	}
	sharedData[localPos] = imageLoad(s_workImage, ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y));

	barrier();

	int tapCount = (TAP_DIM * 2) + 1;
	vec4 totalColor = vec4(0);
	for(int tapPos = -TAP_DIM; tapPos <= TAP_DIM; tapPos++)
	{
		int sampleLoc = localPos + tapPos;
		vec4 inputColor = sharedData[sampleLoc];
		totalColor += inputColor;
	}
	totalColor /= vec4(tapCount);
	imageStore(s_workImage, ivec2(gl_GlobalInvocationID.xy), totalColor);
}
