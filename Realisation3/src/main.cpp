#include "OpenGlDef.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include "GlfwInputProvider.h"
#include "scenes/MeshScene.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void GlMessageCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
#ifndef __APPLE__
	if((severity == GL_DEBUG_SEVERITY_NOTIFICATION) || (severity == GL_DEBUG_SEVERITY_LOW))
	{
		return;
	}
	printf("API: %s\r\n", message);
	if(severity == GL_DEBUG_SEVERITY_HIGH)
	{
		assert(false);
	}
#endif
}

int main(int argc, const char** argv)
{
	if(!glfwInit())
	{
		printf("Failed to initialize glfw.\r\n");
		return -1;
	}

#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "PGJ1303", NULL, NULL);
	if(!window)
	{
		printf("Failed to create window.\r\n");
		return -1;
	}

	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	int version = gladLoadGL(glfwGetProcAddress);
	if(version == 0)
	{
		printf("Failed to initialize OpenGL context\n");
		return -1;
	}

	int flags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if(flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GlMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif

	if(glfwRawMouseMotionSupported())
	{
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	CGlfwInputProvider inputProvider(window);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	CMeshScene scene;
	scene.SetInputProvider(&inputProvider);

	double lastTime = glfwGetTime();
	while(!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		scene.SetWindowSize(width, height);

		double xpos = 0, ypos = 0;
		glfwGetCursorPos(window, &xpos, &ypos);

		double currentTime = glfwGetTime();

		scene.Update(currentTime - lastTime);
		scene.Draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();

		lastTime = currentTime;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
