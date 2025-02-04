#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;
layout(location = 2) in vec3 normal;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 Weights;

out vec2 tex_coords;
out vec3 normal_;
out vec3 local_position_;
flat out ivec4 boneIDs_;
out vec4 weights_;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	tex_coords = texture_coordinates;
	normal_ = normal;
	local_position_ = position;
	boneIDs_ = boneIDs;
	weights_ = Weights;
}