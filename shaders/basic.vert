/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

#version 330 core
layout(location = 0) in vec3 Vertex_Position;
layout(location = 1) in vec4 Vertex_Color;

out vec4 Fragment_Color;

void main(void)
{
   Fragment_Color = Vertex_Color;
   gl_Position = vec4(Vertex_Position.x, Vertex_Position.y, Vertex_Position.z, 1.0f);
};
