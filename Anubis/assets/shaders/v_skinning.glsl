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
out vec4 light_space_pos; //shadow mapping

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 light_wvp; //shadow mapping

const int MAX_BONES = 100;
uniform mat4 bones[MAX_BONES];

void main()
{
	mat4 bone_transform = bones[boneIDs[0]] * Weights[0];
	bone_transform     += bones[boneIDs[1]] * Weights[1];
	bone_transform     += bones[boneIDs[2]] * Weights[2];
	bone_transform     += bones[boneIDs[3]] * Weights[3];

	vec4 pos_local = bone_transform * vec4(position, 1.0);

	//gl_Position = projection * view * model * vec4(position, 1.0);
	gl_Position = projection * view * model * pos_local;
	tex_coords = texture_coordinates;
	normal_ = normal;
	local_position_ = position;
	boneIDs_ = boneIDs;
	weights_ = Weights;

	light_space_pos = light_wvp * vec4(Position, 1.0); //shadow mapping
}