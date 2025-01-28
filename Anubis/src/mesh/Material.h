#pragma once

#include <glm/glm.hpp>
#include "../Texture/Texture.h"

class Material
{
public:
	glm::vec3 m_AmbientColor = glm::vec3(0.0f);
	glm::vec3 m_DiffuseColor = glm::vec3(0.0f);
	glm::vec3 m_SpecularColor = glm::vec3(0.0f);

	Texture* m_DiffuseTexture = nullptr;
	Texture* m_SpecularExponent = nullptr;
};