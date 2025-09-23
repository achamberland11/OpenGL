#version 330 core

in vec3 a_position;
in vec4 a_color;
out vec4 v_color;

layout(std140) uniform Matrices
{
    mat4 worldViewProjMatrix;
};

void main()
{
    gl_Position = worldViewProjMatrix * vec4(a_position, 1);
    v_color = a_color;
}
