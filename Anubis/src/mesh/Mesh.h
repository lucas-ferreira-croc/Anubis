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
#include "../Physics/BoxCollision.h"

#define ARRAY_SIZE_IN_ELEMENTS(arr) (sizeof(arr) / sizeof((arr)[0]))

class Mesh
{
public:
	Mesh() {};
	~Mesh();
	
	bool load(const std::string& filename);
	
	void render();
	void render(unsigned int draw_index, unsigned int primID);
	
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

	std::vector<glm::vec3> get_positions() { return m_Positions; }

	void get_min_max()
	{
	
		glm::vec3 min, max;

		min = max = m_Positions[0];

		// Encontra os extremos do modelo
		for (const glm::vec3& v : m_Positions) {
			min.x = std::min(min.x, v.x);
			min.y = std::min(min.y, v.y);
			min.z = std::min(min.z, v.z);

			max.x = std::max(max.x, v.x);
			max.y = std::max(max.y, v.y);
			max.z = std::max(max.z, v.z);
		}

		m_BoxCollision->m_Vertices.min = min;
		m_BoxCollision->m_Vertices.max = max;
	}

	void get_bounding_box(CollisionVertices& vertices)
	{
		glm::vec3 min, max;

		min = max = m_Positions[0];

		// Encontra os extremos do modelo
		for (const glm::vec3& v : m_Positions) {
			min.x = std::min(min.x, v.x);
			min.y = std::min(min.y, v.y);
			min.z = std::min(min.z, v.z);

			max.x = std::max(max.x, v.x);
			max.y = std::max(max.y, v.y);
			max.z = std::max(max.z, v.z);
		}

		vertices.m_PositionsBuffer.reserve(9);

		glm::vec3 A;
		A.x = min.x;
		A.y = max.y;
		A.z = max.z;

		A = glm::vec4(A, 1.0f) * m_Transform.get_matrix();
		vertices.m_PositionsBuffer.push_back(A);

		glm::vec3 B;
		B.x = max.x;
		B.y = min.y;
		B.z = max.z;
		B = glm::vec4(B, 1.0f) * m_Transform.get_matrix();
		vertices.m_PositionsBuffer.push_back(B);

		glm::vec3 C;
		C.x = min.x;
		C.y = min.y;
		C.z = max.z;
		C = glm::vec4(C, 1.0f) * m_Transform.get_matrix();
		vertices.m_PositionsBuffer.push_back(C);

		glm::vec3 D;
		D.x = max.x;
		D.y = max.y;
		D.z = max.z;
		D = glm::vec4(D, 1.0f) * m_Transform.get_matrix();
		vertices.m_PositionsBuffer.push_back(D);

		glm::vec3 E;
		E.x = max.x;
		E.y = min.y;
		E.z = min.z;
		E = glm::vec4(E, 1.0f) * m_Transform.get_matrix();
		vertices.m_PositionsBuffer.push_back(E);

		glm::vec3 F;
		F.x = max.x;
		F.y = max.y;
		F.z = min.z;
		F = glm::vec4(F, 1.0f) * m_Transform.get_matrix();
		vertices.m_PositionsBuffer.push_back(F);

		glm::vec3 G;
		G.x = min.x;
		G.y = min.y;
		G.z = min.z;
		G = glm::vec4(G, 1.0f) * m_Transform.get_matrix();
		vertices.m_PositionsBuffer.push_back(G);

		glm::vec3 H;
		H.x = max.x;
		H.y = min.y;
		H.z = min.z;
		H = glm::vec4(H, 1.0f) * m_Transform.get_matrix();
		vertices.m_PositionsBuffer.push_back(H);

		glm::vec3 I;
		I.x = min.x;
		I.y = max.y;
		I.z = min.z;
		I = glm::vec4(I, 1.0f) * m_Transform.get_matrix();
		vertices.m_PositionsBuffer.push_back(I);

		vertices.min = glm::vec4(min, 1.0f) * m_Transform.get_matrix();
		vertices.max = glm::vec4(max, 1.0f) * m_Transform.get_matrix();
	}

	BoxCollision* m_BoxCollision;
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
	std::vector<Material> m_Materials;

	std::vector<Texture*> m_Textures;

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

