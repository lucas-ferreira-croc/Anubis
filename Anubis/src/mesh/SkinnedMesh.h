#pragma once

#include <vector>
#include <filesystem>
#include <string>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>


#include "Transform.h"
#include "Material.h"

#define ARRAY_SIZE_IN_ELEMENTS(arr) (sizeof(arr) / sizeof((arr)[0]))


class SkinnedMesh
{
public:
	SkinnedMesh();
	~SkinnedMesh();

	bool load(const std::string& filename);
	void render();

	unsigned int num_bones() const
	{
		return (unsigned int )m_BoneNameToIndexMap.size();
	}

	Transform& get_transform() { return m_Transform; }
	
	const Material& get_material()
	{
		for(Material& m : m_Materials)
		{
			if(m.m_AmbientColor != glm::vec3(0.0f))
			{
				return m;
			}
		}
	}

	void get_bone_transformations(float time_in_seconds, std::vector<glm::mat4>& transforms);

private:
	#define MAX_NUM_BONES_PER_VERTEX 4

	void clear();

	bool init_from_scene(const aiScene* scene, const std::string& filename);
	void count_vertices_and_indices(const aiScene* scene, unsigned int& num_vertices, unsigned int& num_indices);
	void reserve_space(unsigned int num_vertices, unsigned int num_indices);

	void init_all_meshes(const aiScene* scene);
	void init_single_mesh(unsigned int mesh_index, const aiMesh* mesh);

	bool init_materials(const aiScene* scene, const std::string& filename);
	void load_textures(const std::filesystem::path& directory, const aiMaterial* material, int index);
	void load_specular_texture(const std::filesystem::path& directory, const aiMaterial* material, int index);
	void load_diffuse_texture(const std::filesystem::path& directory, const aiMaterial* material, int index);
	void load_colors(const aiMaterial* material, int index);

	void populate_buffers();

	struct VertexBoneData
	{
		unsigned int bone_ids[MAX_NUM_BONES_PER_VERTEX] = { 0 };
		float weights[MAX_NUM_BONES_PER_VERTEX] = { 0 };


		VertexBoneData()
		{
		}

		void add_bone_data(unsigned int bone_id, float weight)
		{
			for(unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(bone_ids); i++)
			{
				if(weights[i] == 0.0)
				{	
					bone_ids[i] = bone_id;
;					weights[i] = weight;

					return;
				}
			}
			assert(0);
		}
	};

	void load_mesh_bones(unsigned int mesh_index, const aiMesh* mesh);
	void load_single_bone(unsigned int mesh_index, const aiBone* bone);
	int get_bone_id(const aiBone* bone);
	void read_node_heirarchy(float animation_time_ticks, const aiNode* node, const glm::mat4& parent_transform);
	
	const aiNodeAnim* find_node_anim(const aiAnimation* animation, const std::string node_name);
	void calculate_interpolated_scaling(aiVector3D& scaling, float animation_time_ticks, const aiNodeAnim* node_anim);
	void calculate_interpolated_rotation(aiQuaternion& rotation, float animation_time_ticks, const aiNodeAnim* node_anim);
	void calculate_interpolated_position(aiVector3D& position, float animation_time_ticks, const aiNodeAnim* node_anim);

	unsigned int find_scaling(float animation_time_ticks, const aiNodeAnim* node_anim);
	unsigned int find_rotation(float animation_time_ticks, const aiNodeAnim* node_anim);
	unsigned int find_translation(float animation_time_ticks, const aiNodeAnim* node_anim);
	

#define INVALID_MATERIAL 0xFFFFFFFF

	enum BUFFER_TYPE
	{
		INDEX = 0,
		POSITION = 1,
		TEXCOORD = 2,
		NORMAL = 3,
		BONE = 4,
		NUM_BUFFERS = 5
	};

	Transform m_Transform;
	unsigned int m_VAO;
	GLuint m_Buffers[NUM_BUFFERS] = { 0 };

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


	Assimp::Importer importer;
	const aiScene* scene = NULL;
	glm::mat4 m_GlobalInverseTransform;

	std::vector<MeshEntry> m_Meshes;
	std::vector<Material> m_Materials;


	std::vector<glm::vec3> m_Positions;
	std::vector<glm::vec3> m_Normals;
	std::vector<glm::vec2> m_TexCoords;
	std::vector<unsigned int> m_Indices;
	std::vector<VertexBoneData> m_Bones;

	std::map<std::string, unsigned int> m_BoneNameToIndexMap;

	struct BoneInfo
	{
		glm::mat4 offset_matrix;
		glm::mat4 final_transformation;

		BoneInfo(const glm::mat4& offset)
		{
			offset_matrix = offset;
			final_transformation = glm::mat4(0.0f);
		}
	};
	
	std::vector<BoneInfo> m_BoneInfo;
};

