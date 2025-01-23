#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec2 tex_coords;
void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	tex_coords = texture_coordinates;
}