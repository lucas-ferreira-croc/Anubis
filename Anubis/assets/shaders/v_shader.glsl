#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform mat4 wvp;
uniform mat4 light_wvp; //shadow mapping

out vec2 tex_coords;
out vec3 normal_;
out vec3 local_position_;
out vec4 light_space_pos; //shadow mapping

void main()
{
	//gl_Position = projection * view * model * vec4(position, 1.0);
	gl_Position = wvp * vec4(position, 1.0);
	tex_coords = texture_coordinates;
	normal_ = normal;
	local_position_ = position;

	light_space_pos = light_wvp * vec4(position, 1.0); //shadow mapping
}
