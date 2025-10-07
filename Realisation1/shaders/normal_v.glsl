#version 330 core

in vec3 a_position;
in vec3 a_normal;
out vec3 v_normal;

layout(std140) uniform Matrices
{
    mat4 worldViewProjMatrix;
};

void main()
{
    gl_Position = worldViewProjMatrix * vec4(a_position, 1);
    v_normal = a_normal;
}
