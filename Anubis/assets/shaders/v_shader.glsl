#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec2 tex_coords;
out vec3 normal_;
out vec3 local_position_;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	tex_coords = texture_coordinates;
	normal_ = normal;
	local_position_ = position;
}