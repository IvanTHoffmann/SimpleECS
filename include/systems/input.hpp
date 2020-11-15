#pragma once

#include "SimpleECS/simpleECS.hpp"
#include "appData.hpp"

// void lockCursor(GLFWwindow *window, InputInfo *inputInfo);
// void unlockCursor(GLFWwindow *window, InputInfo *inputInfo);

#define setMouseLock(s) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (GLFW_CURSOR_DISABLED - GLFW_CURSOR_NORMAL) * ((s)!=0))

void updateJoysticks();

// status = GLFW_CONNECTED / GLFW_DISCONNECTED

void onKey(GLFWwindow *window, int key, int scancode, int action, int mods);

void onMouseButton(GLFWwindow *window, int button, int action, int mods);
void onCursorEnter(GLFWwindow *window, int entered);
void onCursorPos(GLFWwindow *window, double x, double y);
void onScroll(GLFWwindow *window, double x, double y);

void onJoystick(GLFWwindow *window, int id, int status);

void onDrop(GLFWwindow *window, const char **filenames);
void onMonitor(GLFWwindow *window, int id, int status);
void onWindowClose(GLFWwindow *window);
void onWindowContentScale(GLFWwindow *window, float x, float y);
void onWindowFocus(GLFWwindow *window, int focused);
void onWindowIconify(GLFWwindow *window, int iconified);
void onWindowMaximize(GLFWwindow *window, int maximized);
void onWindowPos(GLFWwindow *window, int x, int y);
void onWindowRefresh(GLFWwindow *window);
void onWindowSize(GLFWwindow *window, int w, int h);

void onFramebufferSize(GLFWwindow *window, int w, int h);

void setCallbacks(GLFWwindow *window, InputInfo *inputInfo);

void inputInit(InputInfo *inputInfo);
void refreshInput(InputInfo *inputInfo);

void initInputSystem(CB_PARAMS);
void updateInputSystem(CB_PARAMS);