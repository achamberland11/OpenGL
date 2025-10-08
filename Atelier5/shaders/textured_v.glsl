#version 330 core

in vec3 a_position;
in vec2 a_texCoord;

out vec2 v_texCoord;

layout(std140) uniform Matrices
{
    mat4 worldViewProjMatrix;
};

void main()
{
    gl_Position = worldViewProjMatrix * vec4(a_position, 1);
    v_texCoord = a_texCoord;
}
