#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	rotax = 0.0f;
	rotay = 0.0f;
	rotaz = 0.0f;

	articulacion1 = 0.0f;
	articulacion2 = 0.0f;
	articulacion3 = 0.0f;
	articulacion4 = 0.0f;
	articulacion5 = 0.0f;
	articulacion6 = 0.0f;

	llanta1 = 0.0f;
	llanta2 = 0.0f;
	llanta3 = 0.0f;
	llanta4 = 0.0f;
	llanta5 = 0.0f;
	llanta6 = 0.0f;

	mouseFirstMoved = true;
	lastX = 0.0f;
	lastY = 0.0f;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

int Window::Initialise()
{
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Practica 04: Modelado Jerarquico", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow);
	createCallbacks();
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, bufferWidth, bufferHeight);
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}

GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_E) { theWindow->rotax += 10.0; }
	if (key == GLFW_KEY_T) { theWindow->rotaz += 10.0; }

	// Controles del Brazo Robótico
	if (key == GLFW_KEY_R) { theWindow->articulacion1 += 5.0; } // Esfera roja (Adelante/Atrás en X)
	if (key == GLFW_KEY_F) { theWindow->articulacion2 += 5.0; } // Esfera amarilla (Doble en Z)
	if (key == GLFW_KEY_G) { theWindow->articulacion3 += 5.0; } // Muñeca azul (Doble en Z)
	if (key == GLFW_KEY_H) { theWindow->articulacion4 += 5.0; } // Muñeca azul (Torsión lateral en X)
	if (key == GLFW_KEY_J) { theWindow->articulacion5 += 5.0; } // Punta Gris (Giro sobre su propio eje en Y)
	if (key == GLFW_KEY_K) { theWindow->articulacion6 += 5.0; } // Pinza negra (Abre/Cierra)

	// Llantas Individuales
	if (key == GLFW_KEY_1) { theWindow->llanta1 -= 15.0; } // Frontal Izq
	if (key == GLFW_KEY_2) { theWindow->llanta2 -= 15.0; } // Medio Izq
	if (key == GLFW_KEY_3) { theWindow->llanta3 -= 15.0; } // Trasera Izq
	if (key == GLFW_KEY_4) { theWindow->llanta4 -= 15.0; } // Frontal Der
	if (key == GLFW_KEY_5) { theWindow->llanta5 -= 15.0; } // Medio Der
	if (key == GLFW_KEY_6) { theWindow->llanta6 -= 15.0; } // Trasera Der

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}