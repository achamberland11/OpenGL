#version 330 core

out vec2 v_texCoord;

void main()
{
	vec2 uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	v_texCoord = vec2(uv.x, 1 - uv.y);
	gl_Position = vec4(uv * vec2(2, -2) + vec2(-1, 1), 0, 1);
}
