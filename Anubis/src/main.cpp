#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <sysinfoapi.h>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh/Transform.h"
#include "camera/Camera.h"
#include "texture/Texture.h"
#include "Texture/PickingTexture.h"
#include "Mesh/Mesh.h"
#include "Mesh/SkinnedMesh.h"
#include "Shader/Shader.h"
#include "Shader/Light/DirectionalLight.h"
#include "Shader/Light/PointLight.h"
#include "Display/Display.h"

long long startTimeMilis = 0;


std::function<void(int, int, int)> mouseButtonLambda;
std::function<void(double, double)> onMouseMoveLambda;

struct
{
	bool isPressed = false;
	int x;
	int y;
} m_LeftMouseButton;


void mouse_callback(int button, int action, int x, int y)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		m_LeftMouseButton.isPressed = (action == GLFW_PRESS);
		m_LeftMouseButton.x = x;
		m_LeftMouseButton.y = y;
	}
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	mouse_callback(button, action, (int)x, (int)y);
	mouseButtonLambda(button, action, mode);
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	onMouseMoveLambda(xpos, ypos);
}


int main() 
{

	int width = 1600;
	int height = 1200;
	Display display(width, height);
	display.initialize_window();

	float scale = 0.0f;
	float delta = 0.0065f;
	float angle = 0.0f;
	float angle_delta = 1.0f;

	glm::mat4 identity(1.0f);

	//glEnable(GL_CULL_FACE);

	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, .1f, 100.0f);

	Transform cube_transform;
	glm::vec3 camera_pos = glm::vec3(0.0f, 0.0, 0.0f);
	glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);

	Camera camera(camera_pos, target);
	
	//glfwSetInputMode(display.get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(display.get_window(), GLFW_STICKY_KEYS, GLFW_TRUE);

	glfwSetWindowUserPointer(display.get_window(), &camera);
	
	glfwSetKeyCallback(display.get_window(), &Camera::key_callback);
	glfwSetCursorPosCallback(display.get_window(), &Camera::cursor_position_callback);
	glfwSetCursorPosCallback(display.get_window(), cursor_pos_callback);
	glfwSetMouseButtonCallback(display.get_window(), mouse_button_callback);

	double previous_seconds = glfwGetTime();

	Mesh* mesh = new Mesh();
	if(!mesh->load("C:\\croc\\Anubis\\Anubis\\assets\\Content\\dragon.obj"))
	{
		return 0;
	}

	//SkinnedMesh* mesh = new SkinnedMesh();
	//if (!mesh->load("C:\\croc\\Anubis\\Anubis\\assets\\Content\\boblampclean.md5mesh"))
	//{
	//	return 0;
	//}


	const char* vs_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\v_shader.glsl";
	const char* fs_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\f_shader.glsl";
	
	const char* vs_skinning_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\v_skinning.glsl";
	const char* fs_skinning_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\f_skinning.glsl";

	const char* vs_picking_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\v_picking.glsl";
	const char* fs_picking_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\f_picking.glsl";

	const char* vs_simple_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\v_simple.glsl";
	const char* fs_simple_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\f_simple.glsl";


	Shader shader;
	Shader picking_shader;
	Shader simple_color_shader;

	shader.create_from_file(vs_filename, fs_filename);
	picking_shader.create_from_file(vs_picking_filename, fs_picking_filename);
	simple_color_shader.create_from_file(vs_simple_filename, fs_simple_filename);

	//shader.create_from_file(vs_skinning_filename, fs_skinning_filename);
	DirectionalLight light;
	light.m_Color = glm::vec3(1.0f);
	light.m_ambientIntensity = .1f;
	light.m_DiffuseIntensity = 1.0f;
	light.m_WorldDirection = glm::vec3(3.0f, 0.0f, -1.0f);

	std::vector<PointLight> point_lights;

	PointLight point_light0;
	point_light0.m_DiffuseIntensity = 1.0f;
	point_light0.m_Color = glm::vec3(1.0f, 0.0f, .0f);
	point_light0.attenuation.Linear = 0.2f;
	point_light0.attenuation.Exp = 0.0f;
	point_lights.push_back(point_light0);
	point_lights[0].world_position.x = 3.0f;
	point_lights[0].world_position.y = 0;
	point_lights[0].world_position.z = 1.0f;

	PointLight point_light1;
	point_light1.m_DiffuseIntensity = 1.0f;
	point_light1.m_Color = glm::vec3(0.0f, 1.0f, 0.0f);
	point_light1.attenuation.Linear = 0.0f;
	point_light1.attenuation.Exp = 0.2f;
	point_lights.push_back(point_light1);
	point_lights[1].world_position.x = -1.0f;
	point_lights[1].world_position.y = 0.0f;
	point_lights[1].world_position.z = 1.0f;

	std::vector<SpotLight> spot_lights;
	SpotLight spot_light0;
	spot_light0.m_DiffuseIntensity = 10000.0f;
	spot_light0.m_Color = glm::vec3(1.0f, 0.0f, 1.0);
	spot_light0.attenuation.Linear = 0.1f;
	spot_light0.cutoff = glm::cos(30.0f);
	spot_light0.world_position = glm::vec3(0.0f, 2.0f, 0.0f);
	spot_light0.world_direction = glm::vec3(0.0f, -1.0f, 0.0f);
	spot_lights.push_back(spot_light0);


	std::stringstream ss;

	startTimeMilis = GetTickCount();

	mesh->get_transform().set_rotation(0, 90, 0);
	
	PickingTexture picking_texture;
	picking_texture.init(display.get_buffer_width(), display.get_buffer_height());

	bool clicked = false;

	bool isDragging = false;
	glm::vec3 dragOffset;

	mouseButtonLambda = [&](int button, int action, int mods) {
		if(m_LeftMouseButton.isPressed)
		{
			glm::mat4 inverseProjection = glm::inverse(projection);

			float mouse_x = m_LeftMouseButton.x;
			float mouse_y = m_LeftMouseButton.y;

			float ndc_x = (2.0f * mouse_x) / display.get_buffer_width() - 1.0f;
			float ndc_y = 1.0f - (2.0f * mouse_y) / display.get_buffer_height();

			glm::vec4 ndc_4d(ndc_x, ndc_y, 1.0f, 1.0f);
			glm::vec4 ray_view_4d = inverseProjection * ndc_4d;
			ray_view_4d.z = -1.0f;
			ray_view_4d.w = 0.0f;

			glm::vec3 ray_direction = glm::normalize(glm::vec3(glm::inverse(camera.get_look_at()) * ray_view_4d));
			glm::vec3 ray_origin = camera.get_position();

			float epsilon = 0.05f;

			for (const glm::vec3& p : mesh->get_positions()) {
				glm::vec3 world_p = mesh->get_transform().get_matrix() * glm::vec4(p, 1.0f);
				glm::vec3 diff = ray_origin - world_p;
				float distance = glm::length(glm::cross(diff, ray_direction)) / glm::length(ray_direction);

				if (distance < epsilon) {
					isDragging = true;
					dragOffset = mesh->get_transform().get_position() - world_p;
					break;
				}
			}
		}
		else {
			isDragging = false;
		}
	};

	glm::vec3 target_pos = mesh->get_transform().get_position();

	onMouseMoveLambda = [&](double x_pos, double y_pos) {
		std::cout << "entrou em";
		if (!isDragging) return;

		std::cout << "ta dragando em\n";

		float mouse_x = (float)x_pos;
		float mouse_y = (float)y_pos;

		float ndc_x = (2.0f * mouse_x) / display.get_buffer_width() - 1.0f;
		float ndc_y = 1.0f - (2.0f * mouse_y) / display.get_buffer_height();

		glm::vec4 ndc_4d(ndc_x, ndc_y, -1.0f, 1.0f);
		glm::vec4 ray_view_4d = glm::inverse(projection) * ndc_4d;
		ray_view_4d.w = 0.0f;
		glm::vec3 ray_direction = glm::normalize(glm::vec3(ray_view_4d));

		glm::vec3 ray_origin = camera.get_position();
		glm::mat4 invView = glm::inverse(camera.get_look_at());
		ray_direction = glm::normalize(glm::vec3(invView * glm::vec4(ray_direction, 0.0f)));

		float meshZ = mesh->get_transform().get_position().z;
		float t = (meshZ - ray_origin.z) / ray_direction.z;
		target_pos = ray_origin + t * ray_direction;
		target_pos = glm::vec3(target_pos.x, target_pos.y, meshZ) + dragOffset;

		mesh->get_transform().set_position(target_pos);
	};


	while (!display.should_close())
	{
		mesh->get_transform().set_position(mesh->get_transform().get_position().x, mesh->get_transform().get_position().y, -1.0f);

		double current_seconds = glfwGetTime();
		double delta_time = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		if (GLFW_PRESS == glfwGetKey(display.get_window(), GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(display.get_window(), 1);
		}


		display.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		display.clear();
		scale += delta;
		if(std::abs(scale) >= 1.0f)
		{
			//delta *= -1.0f;
		}


		angle += angle_delta;
		if(std::abs(glm::radians(angle)) >= glm::radians(90.0f))
		{
			//angle_delta *= -1.0f;
		}

		//mesh->get_transform().set_rotation(0.0f, angle, 0.0f);

		
		//std::vector<glm::mat4> bones_transformations;
		//mesh->get_bone_transformations(animationTimeSec, bones_transformations);
		////mesh->get_bone_transformations(bones_transformations);
		//
		//for(int i = 0; i < bones_transformations.size(); i++)
		//{
		//	ss << "bones[" << i << "]";
		//	std::string name = ss.str();
		//	shader.set_mat4(name, bones_transformations[i]);
		//	ss.str("");
		//}
		
		//mesh->get_transform().set_scale(0.03f);
		camera.update(delta_time);

		shader.bind();
		//vs uniforms
		//shader.set_mat4("model", mesh->get_transform().get_matrix());
		shader.set_mat4("view", camera.get_look_at());
		shader.set_mat4("projection", projection);

		light.calculate_local_direction(mesh->get_transform());

		// fs uniforms
		shader.set_int("texture_sampler", 0);
		shader.set_int("texture_sampler_specular", 6);

		camera.calculate_local_position(mesh->get_transform());
		shader.set_float3("camera_local_position", camera.get_local_position());

		shader.set_directional_light(light);


		point_lights[0].calculate_local_position(mesh->get_transform());
		point_lights[1].calculate_local_position(mesh->get_transform());

		shader.set_point_lights(point_lights);

		spot_lights[0].calculate_local_direction_and_position(mesh->get_transform());
		shader.set_spot_lights(spot_lights);

		shader.set_material(mesh->get_material());


		long long currentTimeMilis = GetTickCount();
		float animationTimeSec = ((float)(currentTimeMilis - startTimeMilis)) / 1000.0f;

	
		//glm::vec3 rayDir(1.0f);
		//if (m_LeftMouseButton.isPressed)
		//{
		//	glm::mat4 inverseProjection = glm::inverse(projection);

		//	float mouse_x = (float)m_LeftMouseButton.x;
		//	float mouse_y = (float)m_LeftMouseButton.y;

		//	float ndc_x = (2.0f * mouse_x) / display.get_buffer_width() - 1.0f;
		//	float ndc_y = 1.0f - (2.0f * mouse_y) / display.get_buffer_height();

		//	glm::vec4 ndc_4d(ndc_x, ndc_y, 1.0f, 1.0f);
		//	glm::vec4 ray_view_4d = glm::inverse(projection) * ndc_4d;
		//	ray_view_4d.z = -1.0f; // Direção para frente na câmera
		//	ray_view_4d.w = 0.0f;  // Direção, não posição


		//	glm::vec4 objectViewSpacePos = camera.get_look_at() * glm::vec4(mesh->get_transform().get_position(), 1.0f);

		//	glm::vec4 view_space_intersect = glm::vec4(ray_view_4d * objectViewSpacePos.z);

		//	glm::vec3 ray_direction = glm::normalize(glm::vec3(glm::inverse(camera.get_look_at()) * ray_view_4d));
		//	glm::vec3 ray_origin = camera.get_position(); 

		//	float epsilon = 0.05f; // Ajuste dependendo da precisão necessária

		//	for (const glm::vec3& p : mesh->get_positions()) // Pegando os vértices do mesh
		//	{
		//		glm::vec3 world_p = mesh->get_transform().get_matrix() * glm::vec4(p, 1.0f); // Convertendo vértice para world space
		//		glm::vec3 diff = ray_origin - world_p;
		//		float distance = glm::length(glm::cross(diff, ray_direction)) / glm::length(ray_direction);

		//		if (distance < epsilon)
		//		{
		//			isDragging = true;
		//			dragOffset = mesh->get_transform().get_position() - world_p;
		//			std::cout << "Hit no ponto: " << world_p.x << ", " << world_p.y << ", " << world_p.z << std::endl;
		//			std::cout << "ray_direction: " << ray_direction.x << ", " << ray_direction.y << ", " << ray_direction.z << std::endl;
		//			break;
		//		}
		//	}


		//}

		//glm::vec3 targetPos = mesh->get_transform().get_position();
		//if (isDragging)
		//{
		//	float mouse_x = (float)m_LeftMouseButton.x;
		//	float mouse_y = (float)m_LeftMouseButton.y;

		//	float ndc_x = (2.0f * mouse_x) / display.get_buffer_width() - 1.0f;
		//	float ndc_y = 1.0f - (2.0f * mouse_y) / display.get_buffer_height();

		//	glm::vec4 ndc_4d(ndc_x, ndc_y, -1.0f, 1.0f);
		//	glm::vec4 ray_view_4d = glm::inverse(projection) * ndc_4d;
		//	ray_view_4d.w = 0.0f;
		//	glm::vec3 ray_direction = glm::normalize(glm::vec3(ray_view_4d));

		//	glm::vec3 ray_origin = camera.get_position();
		//	glm::mat4 invView = glm::inverse(camera.get_look_at());
		//	ray_direction = glm::normalize(glm::vec3(invView * glm::vec4(ray_direction, 0.0f)));

		//	// **Aqui está a diferença**: Mantemos o Z fixo do objeto original
		//	float meshZ = mesh->get_transform().get_position().z;
		//	float t = (meshZ - ray_origin.z) / ray_direction.z;
		//	targetPos = ray_origin + t * ray_direction;
		//	targetPos = glm::vec3(targetPos.x, targetPos.y, meshZ) + dragOffset;
		//	
		//	// **Mantemos Z original e aplicamos o offset**
		//}
		//mesh->get_transform().set_position(targetPos);

		//// 3. Parar o dragging quando o botão do mouse for solto
		//if (!m_LeftMouseButton.isPressed)
		//{
		//	isDragging = false;
		//}

		shader.set_mat4("model", mesh->get_transform().get_matrix());

	/*	std::cout << "positions x = " << mesh->get_transform().get_position().x
			<< " y = " << mesh->get_transform().get_position().y
			<< " z =" << mesh->get_transform().get_position().z << "\n";*/

		mesh->render();
		
		display.swap_buffers();

		
	}

	return 0;
}