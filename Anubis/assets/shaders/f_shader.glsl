#version 330 core

in vec2 tex_coords;
in vec3 normal_;
in vec3 local_position_;

out vec4 frag_color;

struct BaseLight
{
	vec3 color;
	float ambient_intensity;
	float diffuse_intensity;
};


struct DirectionalLight
{
	BaseLight base;
	vec3 direction;
};

struct Attenuation
{
	float Constant;
	float Linear;
	float Exp;
};

struct PointLight
{
	BaseLight base;
	vec3 local_pos;
	Attenuation attenuation;
};

struct Material
{
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
};

uniform Material material;
uniform DirectionalLight directional_light;
uniform int point_lights_size;
const int MAX_POINT_LIGHTS = 6;
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform sampler2D texture_sampler;
uniform sampler2D texture_sampler_specular;
uniform vec3 camera_local_position;

vec4 calculate_light_internal(BaseLight base, vec3 light_direction, vec3 normal)
{
	vec4 ambient_color = vec4(base.color, 1.0) *
						 base.ambient_intensity *
						 vec4(material.ambient_color, 1.0);

	float diffuse_factor = dot(normal, -light_direction);

	vec4 diffuse_color = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 specular_color = vec4(0.0, 0.0, 0.0, 0.0);

	if (diffuse_factor > 0)
	{
		diffuse_color = vec4(base.color, 1.0) *
						base.diffuse_intensity *
						vec4(material.diffuse_color, 1.0) *
						diffuse_factor;

		vec3 pixel_to_camera = normalize(camera_local_position - local_position_);
		vec3 light_reflect = normalize(reflect(light_direction, normal));
		float specular_factor = dot(pixel_to_camera, light_reflect);
		if (specular_factor > 0)
		{
			float specular_exponent = texture2D(texture_sampler_specular, tex_coords).r * 255.0f;
			specular_factor = pow(specular_factor, specular_exponent);

			specular_color = vec4(base.color, 1.0) *
							 vec4(material.specular_color, 1.0) *
							 specular_factor;
		}
	}

	return clamp((ambient_color + diffuse_color + specular_color), 0, 1);
}

vec4 calculate_directional_light(vec3 normal)
{
	return calculate_light_internal(directional_light.base, directional_light.direction, normal);
}

vec4 calculate_point_lights(int index , vec3 normal)
{
	vec3 light_direction = local_position_ - point_lights[index].local_pos;
	float distance = length(light_direction);
	light_direction = normalize(light_direction);

	vec4 color = calculate_light_internal(point_lights[index].base, light_direction, normal);
	float attenuation = point_lights[index].attenuation.Constant +
		point_lights[index].attenuation.Linear * distance +
		point_lights[index].attenuation.Exp * distance * distance;

	return color / attenuation;

}

void main()
{
	vec3 normal = normalize(normal_);
	vec4 total_light = calculate_directional_light(normal);

	for(int i = 0; i < point_lights_size; i++)
	{
		total_light += calculate_point_lights(i, normal);
	}

	//frag_color = texture2D(texture_sampler, tex_coords) * clamp((ambient_color + diffuse_color + specular_color), 0, 1);
	frag_color = texture2D(texture_sampler, tex_coords) * total_light;
}