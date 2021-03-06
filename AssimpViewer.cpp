#include "stdafx.h"

#include "Import.h"
#include "AnimatedModel.h"
#include "Shader.h"
#include "Renderer.h"

#include <string>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>


glm::vec3 camera_vel;
float camera_rot_vel = 0;

int window_width = 640, window_height = 480;

void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	window_width = width;
	window_height = height;
	glViewport(0, 0, width, height);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UP) {
		if (action == GLFW_PRESS) {
			camera_vel.y = -1;
		}
		if (action == GLFW_RELEASE) {
			camera_vel.y = 0;
		}
	}

	if (key == GLFW_KEY_RIGHT) {
		if (action == GLFW_PRESS) {
			camera_vel.x = -1;
		}
		if (action == GLFW_RELEASE) {
			camera_vel.x = 0;
		}
	}

	if (key == GLFW_KEY_DOWN) {
		if (action == GLFW_PRESS) {
			camera_vel.y = 1;
		}
		if (action == GLFW_RELEASE) {
			camera_vel.y = 0;
		}
	}

	if (key == GLFW_KEY_LEFT) {
		if (action == GLFW_PRESS) {
			camera_vel.x = 1;
		}
		if (action == GLFW_RELEASE) {
			camera_vel.x = 0;
		}
	}

	if (key == GLFW_KEY_PERIOD) {
		if (action == GLFW_PRESS) {
			camera_rot_vel = 1;
		}
		if (action == GLFW_RELEASE) {
			camera_rot_vel = 0;
		}
	}

	if (key == GLFW_KEY_COMMA) {
		if (action == GLFW_PRESS) {
			camera_rot_vel = -1;
		}
		if (action == GLFW_RELEASE) {
			camera_rot_vel = 0;
		}
	}
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera_vel.z = static_cast<float>(yoffset * 100);
}

GLFWwindow* Initialize() {
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "AssimpViewer", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << glewGetErrorString(err) << std::endl;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	return window;
}

int main(int argc, const char * argv[])
{
	if (argc == 1) {
		std::cout << "Usage: AssimpViewer <filename> " << std::endl;
		return -1;
	}

	GLFWwindow* window = Initialize();

	std::string filename = argv[1];
	std::cout << "Loading " << filename << std::endl;

	Shader* shader = new Shader();
	shader->AddComponent("shaders/fs.glsl", GL_FRAGMENT_SHADER);
	shader->AddComponent("shaders/skeletal_vs.glsl", GL_VERTEX_SHADER);
	shader->Link();

	AnimatedModel* m = Import::LoadAnimatedFile(filename);
	m->SetAnimation("Take 001");
	m->shader = shader;

	glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
	float camera_rotation = 0.0f;

	double last_frame_time = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera_position += (camera_vel * 0.005f);
		camera_rotation += (camera_rot_vel * 0.005f);
		glm::mat4 view_matrix = glm::translate(glm::mat4(1), camera_position);
		view_matrix = glm::rotate(view_matrix, camera_rotation, glm::vec3(0, 1, 0));
		glm::mat4 projection_matrix = glm::perspective(glm::radians(90.0f), (float)window_width / window_height, 0.1f, 100.0f);

		shader->SetMat4("model_matrix", glm::mat4(1));
		shader->SetMat4("view_matrix", view_matrix);
		shader->SetMat4("projection_matrix", projection_matrix);

		double current_time = glfwGetTime();
		m->Update(current_time - last_frame_time);
		Renderer::Render(*m);

		camera_vel.z *= 0.5;

		last_frame_time = current_time;

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
