#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>

int main(int argc, const char **argv) {
  if (!glfwInit()) {
    printf("Failed to initialize glfw.\r\n");
    return -1;
  }

  //Initialiser GLFW et créer un contexte OpenGL. Nous utiliserons un “Core Profile” pour éviter de toucher aux fonctionnalités “legacy” de l’API.
  // OpenGL 4.6 core profile (major version 4, minor version 6)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


  GLFWwindow *window = glfwCreateWindow(640, 480, "PGJ1303", NULL, NULL);
  if (!window) {
    printf("Failed to create window.\r\n");
    return -1;
  }

  // Définir le contexte courant pour la “thread” active. Tous les appels OpenGL subséquents se feront sur ce contexte.
  glfwMakeContextCurrent(window);


  // Charger les adresses des fonctions OpenGL en utilisant la librairie glad
  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    printf("Failed to initialize OpenGL context\n");
    return -1;
  }

  // Main render loop - Run tant que la fenêtre est ouverte
  while (!glfwWindowShouldClose(window)) {
    int width, height;

    // Get the size du frame buffer and stocke les valeurs dans les variables width et height
    glfwGetFramebufferSize(window, &width, &height);

    // Viewport aussi grand que le frame buffer
    glViewport(0, 0, width, height);

    glClearColor(1.0f, 0.0f, 0.0f, 0.0f); // Set a color
    glClear(GL_COLOR_BUFFER_BIT);         // Apply the color
 
    glfwSwapBuffers(window);  // Faire le rendu à l'écran
    glfwPollEvents();         // Lecture d'event (keyboard, mouse, window close)
  }

  // Destroy window et shut down GLFW
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
