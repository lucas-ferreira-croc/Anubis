#version 330 core

in vec2 tex_coords;

out vec4 frag_color;

struct BaseLight
{
	vec3 color;
	float ambient_intensity;
};

struct Material
{
	vec3 ambient_color;
};

uniform Material material;
uniform BaseLight base_light;
uniform sampler2D texture_sampler;

void main()
{
	frag_color = texture(texture_sampler, tex_coords) *
						vec4(material.ambient_color, 1.0) *
						vec4(base_light.color, 1.0) *
						base_light.ambient_intensity;
}