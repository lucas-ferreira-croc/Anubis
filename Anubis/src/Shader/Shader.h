#pragma once


#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Light/DirectionalLight.h"
#include "../Mesh/Material.h"

class Shader
{
public:
	Shader();
	~Shader();

	void create_from_string(const char* vertex_code, const char* fragment_code);
	void create_from_file(const std::string& vertex_code_path, const std::string& fragment_code_path);

	void bind();
	void unbind();

	void clear();

	void set_int(const std::string& name, int value);
	void set_float(const std::string& name, float value);
	void set_float3(const std::string& name, const glm::vec3& value);
	void set_float4(const std::string& name, float r, float g, float b, float a);
	void set_float4(const std::string& name, const glm::vec4& value);
	void set_mat4(const std::string& name, const glm::mat4& value);
	void set_mat4(const std::string& name, float* value);
	void set_vec3_array(const std::string& name, std::vector<glm::vec3> values);
	
	void set_directional_light(const DirectionalLight& light);
	void set_material(const Material& material);
	
	int get_uniform_location(std::string name);

	bool reload_shader();
private:
	unsigned int id;

	void compile_shader(const char* vertex_code, const char* fragment_code);
	void add_shader(const char* shader_code, GLenum shader_type);

	std::string read_file(const std::string& filepath);

	std::unordered_map<std::string, int> uniform_cache;

	std::string vertex_path;
	std::string fragment_path;
};

