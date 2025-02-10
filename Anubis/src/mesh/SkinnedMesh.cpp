#include "SkinnedMesh.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


glm::mat4 assimpToGLM(const aiMatrix4x4& aiMat)
{
    glm::mat4 glmMat;

    glmMat[0][0] = aiMat.a1; glmMat[0][1] = aiMat.b1; glmMat[0][2] = aiMat.c1; glmMat[0][3] = aiMat.d1;
    glmMat[1][0] = aiMat.a2; glmMat[1][1] = aiMat.b2; glmMat[1][2] = aiMat.c2; glmMat[1][3] = aiMat.d2;
    glmMat[2][0] = aiMat.a3; glmMat[2][1] = aiMat.b3; glmMat[2][2] = aiMat.c3; glmMat[2][3] = aiMat.d3;
    glmMat[3][0] = aiMat.a4; glmMat[3][1] = aiMat.b4; glmMat[3][2] = aiMat.c4; glmMat[3][3] = aiMat.d4;

    return glmMat;
}

SkinnedMesh::SkinnedMesh()
{
}

SkinnedMesh::~SkinnedMesh()
{
	clear();
}

bool SkinnedMesh::load(const std::string& filename)
{
    // release previous mesh
    clear();

    // create mesh vao
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // create the buffers for vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    bool ret = false;

     scene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

    if (scene)
    {
        m_GlobalInverseTransform = glm::inverse(assimpToGLM(scene->mRootNode->mTransformation));

        ret = init_from_scene(scene, filename);
    }
    else
    {
        std::cout << "error parsing " << filename << " " << importer.GetErrorString() << "\n";
    }

    glBindVertexArray(0);
    return ret;
}

void SkinnedMesh::render()
{
    glBindVertexArray(m_VAO);

    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        unsigned int material_index = m_Meshes[i].material_index;
        assert(material_index < m_Materials.size());

        if (m_Materials[material_index].m_SpecularExponent)
        {
            m_Materials[material_index].m_SpecularExponent->use(GL_TEXTURE6);
        }

        if (m_Materials[material_index].m_DiffuseTexture != nullptr)
        {
            m_Materials[material_index].m_DiffuseTexture->use(GL_TEXTURE0);
        }

        glDrawElementsBaseVertex(GL_TRIANGLES,
            m_Meshes[i].num_indices,
            GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned int) * m_Meshes[i].base_index),
            m_Meshes[i].base_vertex);
    }

    glBindVertexArray(0);
}

void SkinnedMesh::get_bone_transformations(float time_in_seconds, std::vector<glm::mat4>& transforms)
{

    glm::mat4 identity(1.0f);

    float ticks_per_second = (float)(scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float time_in_ticks = time_in_seconds * ticks_per_second;
    float animation_time_ticks = fmod(time_in_ticks, (float)scene->mAnimations[0]->mDuration);

    read_node_heirarchy(animation_time_ticks, scene->mRootNode, identity);
    transforms.resize(m_BoneInfo.size());

    for(unsigned int i = 0; i < m_BoneInfo.size(); i++)
    {
        transforms[i] = m_BoneInfo[i].final_transformation;
    }
}

void SkinnedMesh::clear()
{
    if (m_Buffers[0] != 0)
    {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

bool SkinnedMesh::init_from_scene(const aiScene* scene, const std::string& filename)
{
    m_Meshes.resize(scene->mNumMeshes);
    m_Materials.resize(scene->mNumMaterials);

    unsigned int num_vertices = 0;
    unsigned int num_indices = 0;

    count_vertices_and_indices(scene, num_vertices, num_indices);
    reserve_space(num_vertices, num_indices);
    init_all_meshes(scene);

    if (!init_materials(scene, filename))
    {
        return false;
    }

    populate_buffers();
    return true;
}

void SkinnedMesh::count_vertices_and_indices(const aiScene* scene, unsigned int& num_vertices, unsigned int& num_indices)
{
    for (int i = 0; i < m_Meshes.size(); i++)
    {
        m_Meshes[i].material_index = scene->mMeshes[i]->mMaterialIndex;
        m_Meshes[i].num_indices = scene->mMeshes[i]->mNumFaces * 3;
        m_Meshes[i].base_vertex = num_vertices;
        m_Meshes[i].base_index = num_indices;

        num_vertices += scene->mMeshes[i]->mNumVertices;
        num_indices += m_Meshes[i].num_indices;
    }
}

void SkinnedMesh::reserve_space(unsigned int num_vertices, unsigned int num_indices)
{
    m_Positions.reserve(num_vertices);
    m_Normals.reserve(num_vertices);
    m_TexCoords.reserve(num_vertices);
    m_Indices.reserve(num_indices);
    m_Bones.resize(num_vertices);
}

void SkinnedMesh::init_all_meshes(const aiScene* scene)
{
    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        init_single_mesh(i, mesh);
    }
}

void SkinnedMesh::init_single_mesh(unsigned int mesh_index, const aiMesh* mesh)
{
    const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& normal = mesh->mNormals[i];
        const aiVector3D& texcoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero3D;

        m_Positions.push_back(glm::vec3(position.x, position.y, position.z));
        m_Normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
        m_TexCoords.push_back(glm::vec2(texcoord.x, texcoord.y));
    }

    load_mesh_bones(mesh_index, mesh);

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        m_Indices.push_back(face.mIndices[0]);
        m_Indices.push_back(face.mIndices[1]);
        m_Indices.push_back(face.mIndices[2]);
    }
}

bool SkinnedMesh::init_materials(const aiScene* scene, const std::string& filename)
{
    std::filesystem::path model_path(filename);
    std::filesystem::path directory = model_path.parent_path();

    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        const aiMaterial* material = scene->mMaterials[i];

        load_textures(directory.string(), material, i);
        load_colors(material, i);
    }

    return true;
}

void SkinnedMesh::load_textures(const std::filesystem::path& directory, const aiMaterial* material, int index)
{
    load_diffuse_texture(directory, material, index);
    load_specular_texture(directory, material, index);
}

void SkinnedMesh::load_specular_texture(const std::filesystem::path& directory, const aiMaterial* material, int index)
{
    m_Materials[index].m_SpecularExponent = nullptr;
    if (material->GetTextureCount(aiTextureType_SHININESS) > 0) {
        aiString path;

        if (material->GetTexture(aiTextureType_SHININESS, 0, &path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {

            std::string texture_path_str(path.C_Str());


            if (texture_path_str._Starts_with("./") || texture_path_str._Starts_with(".\\")) {
                texture_path_str = texture_path_str.substr(2);
            }

            std::filesystem::path texture_path = directory / texture_path_str;

            if (!std::filesystem::exists(texture_path)) {
                std::cout << "Texture file not found: " << texture_path.string() << "\n";
                return;
            }

            m_Materials[index].m_SpecularExponent = new Texture(GL_TEXTURE_2D, texture_path.string().c_str());
            if (!m_Materials[index].m_SpecularExponent->load_texture_grayscale()) {
                std::cout << "Error loading texture \"" << texture_path.string() << "\"\n";
                delete m_Materials[index].m_SpecularExponent;
                m_Materials[index].m_SpecularExponent = nullptr;
                return;
            }
            else {
                std::cout << "Loaded texture \"" << texture_path.string() << "\"\n";
            }
        }
        else {
            std::cout << "No valid diffuse texture found for material " << index << "\n";
        }
    }
}

void SkinnedMesh::load_diffuse_texture(const std::filesystem::path& directory, const aiMaterial* material, int index)
{
    m_Materials[index].m_DiffuseTexture = nullptr;
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
        {

            std::string texture_path_str(path.C_Str());


            if (texture_path_str._Starts_with("./") || texture_path_str._Starts_with(".\\"))
            {
                texture_path_str = texture_path_str.substr(2);
            }

            std::filesystem::path texture_path = directory / texture_path_str;

            if (!std::filesystem::exists(texture_path))
            {
                std::cout << "Texture file not found: " << texture_path.string() << "\n";
                return;
            }

            m_Materials[index].m_DiffuseTexture = new Texture(GL_TEXTURE_2D, texture_path.string().c_str());
            if (!m_Materials[index].m_DiffuseTexture->load_textureA())
            {
                std::cout << "Error loading texture \"" << texture_path.string() << "\"\n";
                delete m_Materials[index].m_DiffuseTexture;
                m_Materials[index].m_DiffuseTexture = nullptr;
                return;
            }
            else
            {
                std::cout << "Loaded texture \"" << texture_path.string() << "\"\n";
            }
        }
        else
        {
            std::cout << "No valid diffuse texture found for material " << index << "\n";
        }
    }
}

void SkinnedMesh::load_colors(const aiMaterial* material, int index)
{
    aiColor3D ambient_color(0.0f, 0.0f, 0.0f);
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambient_color) == AI_SUCCESS)
    {
        std::cout << "loaaded material color " << ambient_color.r << " " << ambient_color.g << " " << ambient_color.b << "\n";
        m_Materials[index].m_AmbientColor.r = ambient_color.r;
        m_Materials[index].m_AmbientColor.g = ambient_color.g;
        m_Materials[index].m_AmbientColor.b = ambient_color.b;
    }

    aiColor3D diffuse_color(0.0f, 0.0f, 0.0f);
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color) == AI_SUCCESS)
    {
        std::cout << "lodaded diffuse color " << diffuse_color.r << " " << diffuse_color.g << " " << diffuse_color.b << "\n";
        m_Materials[index].m_DiffuseColor.r = diffuse_color.r;
        m_Materials[index].m_DiffuseColor.g = diffuse_color.g;
        m_Materials[index].m_DiffuseColor.b = diffuse_color.b;
    }

    aiColor3D specular_color(0.0f, 0.0f, 0.0f);
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, specular_color) == AI_SUCCESS)
    {
        std::cout << "lodaded specular color " << specular_color.r << " " << specular_color.g << " " << specular_color.b << "\n";
        m_Materials[index].m_SpecularColor.r = specular_color.r;
        m_Materials[index].m_SpecularColor.g = specular_color.g;
        m_Materials[index].m_SpecularColor.b = specular_color.b;
    }
}

void SkinnedMesh::populate_buffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POSITION]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), &m_Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), &m_Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Bones[0]) * m_Bones.size(), &m_Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const void*)0);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, 
        sizeof(VertexBoneData), (const void*)(MAX_NUM_BONES_PER_VERTEX * sizeof(int32_t)));
       

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);
}

void SkinnedMesh::load_mesh_bones(unsigned int mesh_index, const aiMesh* mesh)
{
    for(unsigned int i = 0; i < mesh->mNumBones; i++)
    {
        load_single_bone(mesh_index, mesh->mBones[i]);
    }
}

void SkinnedMesh::load_single_bone(unsigned int mesh_index, const aiBone* bone)
{
    int bone_id = get_bone_id(bone);
    
    if(bone_id == m_BoneInfo.size())
    {
        BoneInfo bi(assimpToGLM(bone->mOffsetMatrix));
        m_BoneInfo.push_back(bi);
    }
    
    for (unsigned int i = 0; i < bone->mNumWeights; i++)
    {
        const aiVertexWeight& vw = bone->mWeights[i];
        unsigned int global_vertex_id = m_Meshes[mesh_index].base_vertex + bone->mWeights[i].mVertexId;
        m_Bones[global_vertex_id].add_bone_data(bone_id, vw.mWeight);
    }
}

int SkinnedMesh::get_bone_id(const aiBone* bone)
{
    int bone_index = 0;
    std::string bone_name(bone->mName.C_Str());

    if(m_BoneNameToIndexMap.find(bone_name) == m_BoneNameToIndexMap.end())
    {
        bone_index = (int)m_BoneNameToIndexMap.size();
        m_BoneNameToIndexMap[bone_name] = bone_index;
    }
    else
    {
        bone_index = m_BoneNameToIndexMap[bone_name];
    }

    return bone_index;
}

void SkinnedMesh::read_node_heirarchy(float animation_time_ticks, const aiNode* node, const glm::mat4& parent_transform)
{
    std::string node_name(node->mName.data);
    glm::mat4 node_transformation(assimpToGLM(node->mTransformation));
    
    const aiAnimation* animation = scene->mAnimations[0];

    std::cout << node_name << "\n";

    const aiNodeAnim* node_anim = find_node_anim(animation, node_name);

   if(node_anim)
    {
        aiVector3D scaling;
        calculate_interpolated_scaling(scaling, animation_time_ticks, node_anim);
        glm::mat4 scaling_matrix(1.0f);
        scaling_matrix = glm::scale(scaling_matrix, glm::vec3(scaling.x, scaling.y, scaling.z));

        aiQuaternion rotation_q;
        calculate_interpolated_rotation(rotation_q, animation_time_ticks, node_anim);
        glm::quat rotation = glm::quat(rotation_q.w, rotation_q.x, rotation_q.y, rotation_q.z);
        glm::mat4 rotation_matrix = glm::toMat4(rotation);

        aiVector3D translation;
        calculate_interpolated_position(translation, animation_time_ticks, node_anim);
        glm::mat4 transtlation_matrix = glm::mat4(1.0f);
        transtlation_matrix = glm::translate(transtlation_matrix, glm::vec3(translation.x, translation.y, translation.z));

        node_transformation = transtlation_matrix * rotation_matrix * scaling_matrix;
    }


    glm::mat4 global_transformation = parent_transform * node_transformation;
    if(m_BoneNameToIndexMap.find(node_name) != m_BoneNameToIndexMap.end())
    {
        unsigned int bone_index = m_BoneNameToIndexMap[node_name];
        m_BoneInfo[bone_index].final_transformation = global_transformation * m_BoneInfo[bone_index].offset_matrix;
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        read_node_heirarchy(animation_time_ticks, node->mChildren[i], global_transformation);
    }
}

const aiNodeAnim* SkinnedMesh::find_node_anim(const aiAnimation* animation, const std::string node_name)
{
    for(unsigned int i = 0; i < animation->mNumChannels; i++)
    {
        const aiNodeAnim* node_anim = animation->mChannels[i];

        if (std::string(node_anim->mNodeName.data) == node_name) 
        {
            return node_anim;
        }
    }
    return NULL;
}

void SkinnedMesh::calculate_interpolated_scaling(aiVector3D& scaling, float animation_time_ticks, const aiNodeAnim* node_anim)
{
    if(node_anim->mNumScalingKeys == 1)
    {
        scaling = node_anim->mScalingKeys[0].mValue;
        return;
    }

    unsigned int scaling_index = find_scaling(animation_time_ticks, node_anim);
    unsigned int next_scaling_index = scaling_index + 1;
    assert(next_scaling_index < node_anim->mNumScalingKeys);
    float t1 = (float)node_anim->mScalingKeys[scaling_index].mTime;
    float t2 = (float)node_anim->mScalingKeys[next_scaling_index].mTime;
    float delta_time = t2 - t1;
    float factor = (animation_time_ticks - (float)t1) / delta_time;
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiVector3D& start = node_anim->mScalingKeys[scaling_index].mValue;
    const aiVector3D& end = node_anim->mScalingKeys[next_scaling_index].mValue;
    aiVector3D delta = end - start;
    scaling = start + factor * delta;
}

void SkinnedMesh::calculate_interpolated_rotation(aiQuaternion& rotation, float animation_time_ticks, const aiNodeAnim* node_anim)
{
    if(node_anim->mNumRotationKeys == 1)
    {
        rotation = node_anim->mRotationKeys[0].mValue;
        return;
    }

    unsigned int rotation_index = find_rotation(animation_time_ticks, node_anim);
    unsigned int next_rotation_index = rotation_index + 1;
    assert(next_rotation_index < node_anim->mNumRotationKeys);
    float t1 = (float)node_anim->mRotationKeys[rotation_index].mTime;
    float t2 = (float)node_anim->mRotationKeys[next_rotation_index].mTime;
    float delta_time = t2 - t1;
    float factor = (animation_time_ticks - (float)t1) / delta_time;
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiQuaternion& start_rotation = node_anim->mRotationKeys[rotation_index].mValue;
    const aiQuaternion& end_rotation = node_anim->mRotationKeys[next_rotation_index].mValue;
    aiQuaternion::Interpolate(rotation, start_rotation, end_rotation, factor);
    rotation = start_rotation;
    rotation.Normalize();
}

void SkinnedMesh::calculate_interpolated_position(aiVector3D& position, float animation_time_ticks, const aiNodeAnim* node_anim)
{
    if(node_anim->mNumPositionKeys == 1)
    {
        position = node_anim->mPositionKeys[0].mValue;
        return;
    }

    unsigned int position_index = find_translation(animation_time_ticks, node_anim);
    unsigned int next_position_index = position_index + 1;
    assert(next_position_index < node_anim->mNumPositionKeys);
    float t1 = (float)node_anim->mPositionKeys[position_index].mTime;
    float t2 = (float)node_anim->mPositionKeys[next_position_index].mTime;
    float delta_time = t2 - t1;
    float factor = (animation_time_ticks - (float)t1 / delta_time);
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiVector3D& start = node_anim->mPositionKeys[position_index].mValue;
    const aiVector3D& end = node_anim->mPositionKeys[next_position_index].mValue;
    aiVector3D delta = end - start;
    position = start + factor * delta;
}

unsigned int SkinnedMesh::find_scaling(float animation_time_ticks, const aiNodeAnim* node_anim)
{
    assert(node_anim->mNumScalingKeys > 0);

    for(unsigned int i = 0; i < node_anim->mNumScalingKeys -1; i++)
    {
        float t = (float)node_anim->mScalingKeys[i + 1].mTime;
        if(t > animation_time_ticks)
        {
            return i;
        }
    }

    return 0;
}

unsigned int SkinnedMesh::find_rotation(float animation_time_ticks, const aiNodeAnim* node_anim)
{
    assert(node_anim->mNumRotationKeys > 0);

    for (unsigned int i = 0; i < node_anim->mNumRotationKeys - 1; i++)
    {
        float t = (float)node_anim->mRotationKeys[i + 1].mTime;
        if (t > animation_time_ticks)
        {
            return i;
        }
    }

    return 0;
}

unsigned int SkinnedMesh::find_translation(float animation_time_ticks, const aiNodeAnim* node_anim)
{
    assert(node_anim->mNumPositionKeys> 0);

    for (unsigned int i = 0; i < node_anim->mNumPositionKeys - 1; i++)
    {
        float t = (float)node_anim->mPositionKeys[i + 1].mTime;
        if (t > animation_time_ticks)
        {
            return i;
        }
    }

    return 0;
}
