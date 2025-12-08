#version 330 core

in vec3 a_position;
in vec2 a_texCoord;

out vec2 v_texCoord;

layout(std140) uniform Matrices
{
	mat4 worldMatrix;
	mat4 viewProjMatrix;
};

void main()
{
	vec4 worldPos = worldMatrix * vec4(a_position, 1);
	gl_Position = viewProjMatrix * worldPos;
    v_texCoord = a_texCoord;
}
