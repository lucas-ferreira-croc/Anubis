#include "Mesh.h"
#include <iostream>


void Mesh::clear()
{
    for (unsigned int i = 0; i < m_Textures.size(); i++)
    {
        delete(m_Textures[i]);
    }

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



Mesh::~Mesh()
{
    clear();
}

bool Mesh::load(const std::string& filename)
{
    // release previous mesh
    clear();

    // create mesh vao
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // create the buffers for vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    bool ret = false;
    Assimp::Importer importer;
    
    const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals  | aiProcess_JoinIdenticalVertices);

    if (scene)
    {
        ret = init_from_scene(scene, filename);
    }
    else
    {
        std::cout << "error parsing " << filename << " " << importer.GetErrorString() << "\n";
    }

    glBindVertexArray(0);
    return ret;
}

void Mesh::render()
{
    glBindVertexArray(m_VAO);

    for(unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        unsigned int material_index = m_Meshes[i].material_index;
        assert(material_index < m_Materials.size());

        if(m_Materials[material_index].m_SpecularExponent)
        {
            m_Materials[material_index].m_SpecularExponent->use(GL_TEXTURE6);
        }

       if(m_Materials[material_index].m_DiffuseTexture != nullptr)
       {
           m_Materials[material_index].m_DiffuseTexture->use(GL_TEXTURE0);
       }

        glDrawElementsBaseVertex(GL_TRIANGLES, m_Meshes[i].num_indices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_Meshes[i].base_index), m_Meshes[i].base_vertex);
    }
}

bool Mesh::init_from_scene(const aiScene* scene, const std::string& filename)
{
    m_Meshes.resize(scene->mNumMeshes);
    m_Textures.resize(scene->mNumMaterials);
    m_Materials.resize(scene->mNumMaterials);

    unsigned int num_vertices = 0;
    unsigned int num_indices = 0;

    count_vertices_and_indices(scene, num_vertices, num_indices);
    reserve_space(num_vertices, num_indices);
    init_all_meshes(scene);

    if(!init_materials(scene, filename))
    {
        return false;
    }

    populate_buffers();
    return true;
}

void Mesh::count_vertices_and_indices(const aiScene* scene, unsigned int& num_vertices, unsigned int& num_indices)
{
    for(int i = 0; i < m_Meshes.size(); i++)
    {
        m_Meshes[i].material_index = scene->mMeshes[i]->mMaterialIndex;
        m_Meshes[i].num_indices = scene->mMeshes[i]->mNumFaces * 3;
        m_Meshes[i].base_vertex = num_vertices;
        m_Meshes[i].base_index = num_indices;

        num_vertices += scene->mMeshes[i]->mNumVertices;
        num_indices += m_Meshes[i].num_indices;
    }
}

void Mesh::reserve_space(unsigned int num_vertices, unsigned int num_indices)
{
    m_Positions.reserve(num_vertices);
    m_Normals.reserve(num_vertices);
    m_TexCoords.reserve(num_vertices);
    m_Indices.reserve(num_indices);
}

void Mesh::init_all_meshes(const aiScene* scene)
{
    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        init_single_mesh(mesh);
    }
}

void Mesh::init_single_mesh(const aiMesh* mesh)
{
    const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& normal = mesh->mNormals[i];
        const aiVector3D& texcoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero3D;

        m_Positions.push_back(glm::vec3(position.x, position.y, position.z));
        m_Normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
        m_TexCoords.push_back(glm::vec2(texcoord.x, texcoord.y));
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        m_Indices.push_back(face.mIndices[0]);
        m_Indices.push_back(face.mIndices[1]);
        m_Indices.push_back(face.mIndices[2]);
    }
}

bool Mesh::init_materials(const aiScene* scene, const std::string& filename)
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

void Mesh::load_textures(const std::filesystem::path& directory, const aiMaterial* material, int index)
{
    load_diffuse_texture(directory, material, index);
    load_specular_texture(directory, material, index);

}

void Mesh::load_diffuse_texture(const std::filesystem::path& directory, const aiMaterial* material, int index)
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

            std::cout << "vai carregar em";
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

void Mesh::load_specular_texture(const std::filesystem::path& directory, const aiMaterial* material, int index)
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

void Mesh::load_colors(const aiMaterial* material, int index)
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


void Mesh::populate_buffers()
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

}
