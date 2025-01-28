#pragma once

#include <vector>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Transform.h"
#include "Material.h"
#include "../Texture/Texture.h"

#define ARRAY_SIZE_IN_ELEMENTS(arr) (sizeof(arr) / sizeof((arr)[0]))

class Mesh
{
public:
	Mesh() {};
	~Mesh();
	
	bool load(const std::string& filename);
	
	void render();
	void render(unsigned int num_instances, const glm::mat4& wvp_matrix, const glm::mat4& world_matrix);
	Transform& get_transform() { return m_Transform; }

	const Material& get_material()
	{
		for(Material m : m_Materials)
		{
			if(m.m_AmbientColor !=  glm::vec3(0.0f, 0.0f, 0.0f))
			{
				return m;
			}
		}
	}

private:
	void clear();
	
	bool init_from_scene(const aiScene* scene, const std::string& filename);
	void count_vertices_and_indices(const aiScene* scene, unsigned int& num_vertices, unsigned int& num_indices);
	void reserve_space(unsigned int num_vertices, unsigned int num_indices);
	
	void init_all_meshes(const aiScene* scene);
	void init_single_mesh(const aiMesh* mesh);
	
	bool init_materials(const aiScene* scene, const std::string& filename);
	void load_textures(const std::filesystem::path& directory, const aiMaterial* material, int index);
	void load_specular_texture(const std::filesystem::path& directory, const aiMaterial* material, int index);
	void load_diffuse_texture(const std::filesystem::path& directory, const aiMaterial* material, int index);
	void load_colors(const aiMaterial* material, int index);

	void populate_buffers();

	Transform m_Transform;

	struct MeshEntry
	{
		MeshEntry() 
			: num_indices(0), base_vertex(0), base_index(0), material_index(0)
		{
		}

		unsigned int num_indices;
		unsigned int base_vertex;
		unsigned int base_index;
		unsigned int material_index;
	};

	std::vector<MeshEntry> m_Meshes;
	std::vector<Texture*> m_Textures;
	std::vector<Material> m_Materials;

	std::vector<glm::vec3> m_Positions;
	std::vector<glm::vec3> m_Normals;
	std::vector<glm::vec2> m_TexCoords;
	std::vector<unsigned int> m_Indices;

	struct Vertex
	{
		Vertex() = default;
		Vertex(glm::vec3 position, glm::vec2 uv) : pos(position), uv(uv)
		{
		}

		glm::vec3 pos;
		glm::vec2 uv;
	};

	//std::vector<Material> m_materials>
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> indices;

	enum BUFFER_TYPE
	{
		INDEX       = 0,
		POSITION    = 1,
		TEXCOORD    = 2,
		NORMAL      = 3,
		NUM_BUFFERS = 4
	};

	unsigned int m_VAO;
	GLuint m_Buffers[NUM_BUFFERS] = { 0 };

};

