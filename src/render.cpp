//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <string>
#include <filesystem>

#include "glad.h"
#include "stb_image.h"

#include "render.hpp"
#include "core.hpp"
#include "gui.hpp"
#include "input.hpp"

using std::cout;
using std::string;
using std::filesystem::current_path;
using std::filesystem::path;

using Core::Hub;
using Graphics::GUI::GUI_Hub;
using Core::Input;

namespace Graphics
{
	void Render::Initialize()
	{
		cout << "Initializing GLFW...\n";

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		cout << "Initializing window...\n";

		string windowName = "Elypso hub " + Hub::version;
		if (currentWidth == 0) currentWidth = 1024;
		if (currentHeight == 0) currentHeight = 768;
		window = glfwCreateWindow(
			currentWidth,
			currentHeight,
			windowName.c_str(),
			NULL,
			NULL);

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
		glfwSetWindowSizeLimits(window, 480, 270, 7680, 4320);
		glfwSwapInterval(1);

		Hub::defaultPath = current_path().generic_string();

		int width, height, channels;
		string iconpath = (path(Hub::defaultPath) / "files" / "icon.png").string();
		cout << "icon path: " << iconpath << "\n";
		unsigned char* iconData = stbi_load(iconpath.c_str(), &width, &height, &channels, 4);

		GLFWimage icon{};
		icon.width = width;
		icon.height = height;
		icon.pixels = iconData;

		glfwSetWindowIcon(window, 1, &icon);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		glfwSetKeyCallback(window, Input::KeyCallback);
		glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
		glfwSetScrollCallback(window, Input::ScrollCallback);
		glfwSetCursorPosCallback(window, Input::MouseMovementCallback);

		cout << "Initializing GLAD...\n";

		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		UpdateAfterRescale(window, currentWidth, currentHeight);

		GUI_Hub::Initialize();
		GUI_Hub::UpdateFileList();
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
		//Calculate the new aspect ratio
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		//Set the viewport based on the aspect ratio
		glViewport(0, 0, width, height);

		currentWidth = width;
		currentHeight = height;
	}

	void Render::Run()
	{
		glClearColor(
			backgroundColor.x,
			backgroundColor.y,
			backgroundColor.z,
			1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GUI_Hub::Render();

		//swap the front and back buffers
		glfwSwapBuffers(window);
		if (!Hub::IsUserIdle())
		{
			glfwPollEvents();
		}
		else glfwWaitEvents();
	}
}
