#ifndef BIRDY3D_ENGINE_HPP
#define BIRDY3D_ENGINE_HPP

#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EventHandler.hpp"

struct framebufferSizeArg {
    int width;
    int height;
};

class Engine {
public:
    static bool init(const char *windowName, int width, int height);
    static GLFWwindow *getWindow();
    static EventHandler<framebufferSizeArg> *getFramebufferSizeEventHandler();

private:
    static GLFWwindow *window;
    static EventHandler<framebufferSizeArg> *framebufferSizeEventHandler;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void window_focus_callback(GLFWwindow *window, int focused);
};

#endif
