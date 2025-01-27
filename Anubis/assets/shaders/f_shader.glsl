#version 330 core

in vec2 tex_coords;
in vec3 normal_;

out vec4 frag_color;

struct BaseLight
{
	vec3 color;
	float ambient_intensity;
};


struct DirectionalLight
{
	BaseLight base;
	float diffuse_intensity;
	vec3 direction;
};


struct Material
{
	vec3 ambient_color;
	vec3 diffuse_color;
};

uniform Material material;
uniform DirectionalLight directional_light;
uniform sampler2D texture_sampler;

void main()
{
	vec4 ambient_color = vec4(directional_light.base.color, 1.0) *
						 directional_light.base.ambient_intensity *
						 vec4(material.ambient_color, 1.0);

	float diffuse_factor = dot(normalize(normal_), -directional_light.direction);
	vec4 diffuse_color = vec4(0.0, 0.0, 0.0, 0.0);

	if(diffuse_factor > 0)
	{
		diffuse_color = vec4(directional_light.base.color, 1.0) *
						directional_light.diffuse_intensity *
						vec4(material.diffuse_color, 1.0) *
						diffuse_factor;
	}

	frag_color = texture(texture_sampler, tex_coords) * (ambient_color + diffuse_color);
}