#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

int main()
{
    glfwSetErrorCallback([] (int error, const char * description) {
       std::cout << "Error: " << description << "\n";
    });

    if(!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(1024, 768, "First example", nullptr, nullptr);
    if(!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, [] (GLFWwindow * window, int key, int scancode, int action, int mods) {
       if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
    });

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
}
