#version 330 core

const int MAX_POINT_LIGHTS = 6;
const int MAX_SPOT_LIGHTS = 6;

in vec2 tex_coords;
in vec3 normal_;
in vec3 local_position_;
flat in ivec4 boneIDs_;
in vec4 weights_;

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

struct SpotLight
{
	PointLight PointLightBase;
	vec3 Direction;
	float Cutoff;
};

struct Material
{
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
};


// Lighntining and material stuff
uniform Material material;

uniform DirectionalLight directional_light;

uniform int point_lights_size;
uniform PointLight point_lights[MAX_POINT_LIGHTS];

uniform int spot_lights_size;
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];

uniform vec3 camera_local_position;

uniform sampler2D texture_sampler;
uniform sampler2D texture_sampler_specular;

// animation
uniform int display_bone_index;


// lightning functions
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

vec4 calculate_point_light(PointLight point_light, vec3 normal)
{
	vec3 light_direction = local_position_ - point_light.local_pos;
	float distance = length(light_direction);
	light_direction = normalize(light_direction);

	vec4 color = calculate_light_internal(point_light.base, light_direction, normal);
	float attenuation = point_light.attenuation.Constant +
		point_light.attenuation.Linear * distance +
		point_light.attenuation.Exp * distance * distance;

	return color / attenuation;

}

vec4 calculate_spot_lights(SpotLight spot_light, vec3 normal)
{
	vec3 light_to_pixel = normalize(local_position_ - spot_light.PointLightBase.local_pos);
	float spot_factor = dot(light_to_pixel, spot_light.Direction);

	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	if (spot_factor > spot_light.Cutoff)
	{
		color = calculate_point_light(spot_light.PointLightBase, normal);
		float spot_light_intensity = (1.0 - (1.0 - spot_factor) / (1.0 - spot_light.Cutoff));
		return color * spot_light_intensity;
	}
	else {
		return vec4(0.0, 0.0, 0.0, 0.0);
	}

}

void main()
{
	vec3 normal = normalize(normal_);
	vec4 total_light = calculate_directional_light(normal);

	for (int i = 0; i < point_lights_size; i++)
	{
		total_light += calculate_point_light(point_lights[i], normal);
	}

	for (int i = 0; i < spot_lights_size; i++)
	{
		total_light += calculate_spot_lights(spot_lights[i], normal);
	}

	bool found = false;

	for (int i = 0; i < 4; i++)
	{
		if (boneIDs_[i] == display_bone_index)
		{
			if(weights_[i] >= 0.7)
			{
				frag_color = vec4(1.0, 0.0, 0.0, 0.0) * weights_[i];
			}
			else if (weights_[i] >= 0.4 && weights_[i] <= 0.6)
			{
				frag_color = vec4(0.0, 1.0, 0.0, 0.0) * weights_[i];
			}
			else if (weights_[i] >= 0.1)
			{
				frag_color = vec4(1.0, 1.0, 0.0, 0.0) * weights_[i];
			}
			found = true;
			break;
		}
	}

	if(!found)
	{
		frag_color = texture2D(texture_sampler, tex_coords) * total_light * vec4(0.0001) + vec4(0.0, 0.0, 1.0, 0.0);
	}
}