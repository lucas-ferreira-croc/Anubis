#include "Shader.h"

#include <fstream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader() : id(0)
{
}

Shader::~Shader()
{
	clear();
}

void Shader::create_from_string(const char* vertex_code, const char* fragment_code)
{
	compile_shader(vertex_code, fragment_code);
	unbind();
}

void Shader::create_from_file(const std::string& vertex_code_path, const std::string& fragment_code_path)
{
	vertex_path = vertex_code_path;
	fragment_path = fragment_code_path;

	compile_shader(read_file(vertex_code_path).c_str(), read_file(fragment_code_path).c_str());
	unbind();
}

void Shader::bind()
{
	glUseProgram(id);
}

void Shader::unbind()
{
	glUseProgram(0);
}

void Shader::clear()	
{
	if (id != 0)
	{
		glDeleteProgram(id);
		id = 0;
	}
}

void Shader::set_int(const std::string& name, int value)
{
	glUniform1i(get_uniform_location(name), value);
}

void Shader::set_float(const std::string& name, float value)
{
	glUniform1f(get_uniform_location(name), value);
}

void Shader::set_float4(const std::string& name, float r, float g, float b, float a)
{
	glUniform4f(get_uniform_location(name), r, g, b, a);
}

void Shader::set_mat4(const std::string& name, float* value)
{
	glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, value);
}

void Shader::set_float3(const std::string& name, const glm::vec3& value)
{
	glUniform3f(get_uniform_location(name), value.x, value.y, value.z);
}

void Shader::set_float4(const std::string& name, const glm::vec4& value)
{
	glUniform4f(get_uniform_location(name), value.x, value.y, value.z, value.w);
}

void Shader::set_mat4(const std::string& name, const glm::mat4& value)
{
	glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_vec3_array(const std::string& name, std::vector<glm::vec3> values)
{
	std::stringstream ss;

	for (int i = 0; i < values.size(); i++)
	{
		ss << name << "[" << i << "]";
		std::string uniform = ss.str();
		set_float3(uniform, values[i]);
		ss.str("");
	}
}

void Shader::set_directional_light(const DirectionalLight& light)
{
	set_float3("directional_light.base.color", light.m_Color);
	set_float("directional_light.base.ambient_intensity", light.m_ambientIntensity);
	
	set_float3("directional_light.direction", light.get_local_direction());
	set_float("directional_light.diffuse_intensity", light.m_DiffuseIntensity);
}

void Shader::set_material(const Material& material)
{
	set_float3("material.ambient_color", material.m_AmbientColor);
	set_float3("material.diffuse_color", material.m_DiffuseCOlor);
}

int Shader::get_uniform_location(std::string name)
{
	int location;
	if (uniform_cache.find(name) != uniform_cache.end())
	{
		location = uniform_cache.at(name);
		return location;
	}
	location = glGetUniformLocation(id, name.c_str());
	uniform_cache.insert(std::make_pair(name, location));
	return location;
}

bool Shader::reload_shader()
{
	if(vertex_path.empty() || fragment_path.empty())
		return false;

	clear();
	create_from_file(vertex_path, fragment_path);
	bind();
}

void Shader::compile_shader(const char* vertex_code, const char* fragment_code)
{
	id = glCreateProgram();

	if (!id) {
		std::cout << "ERROR CREATING SHADER PROGRAM!" << std::endl;
		return;
	}

	add_shader(vertex_code, GL_VERTEX_SHADER);
	add_shader(fragment_code, GL_FRAGMENT_SHADER);

	int result = 0;
	char error_log[1024] = { 0 };

	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(id, sizeof(error_log), NULL, error_log);
		std::cout << "ERRROR LINKING PROGRAM: " << error_log << std::endl;
		return;
	}

	glValidateProgram(id);
	glGetProgramiv(id, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(id, sizeof(error_log), NULL, error_log);
		std::cout << "ERRROR VALIDATING PROGRAM: " << error_log << std::endl;
		return;
	}
}

void Shader::add_shader(const char* shader_code, GLenum shader_type)
{
	unsigned int shader = glCreateShader(shader_type);
	const char* code[1];
	code[0] = shader_code;

	int code_length[1];
	code_length[0] = strlen(shader_code);

	glShaderSource(shader, 1, code, code_length);
	glCompileShader(shader);

	int result = 0;
	char error_log[1024] = { 0 };

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		std::string shader_type_name = shader_type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";

		glGetShaderInfoLog(shader, sizeof(error_log), NULL, error_log);
		std::cout << "ERRROR COMPILING THE " << shader_type << " SHADER: " << error_log << std::endl;
		return;
	}

	glAttachShader(id, shader);

}

std::string Shader::read_file(const std::string& filepath)
{
	std::fstream stream(filepath);
	std::stringstream buffer;

	buffer << stream.rdbuf();
	stream.close();

	std::string source_code = buffer.str();
	return source_code;
}
