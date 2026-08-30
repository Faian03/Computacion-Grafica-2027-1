#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Para srand() y rand()
#include <time.h>   // Para time()
#include <glew.h>
#include <glfw3.h>

// Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

// Shaders con formato Raw String (para no tener que poner \n\ en cada renglón)
static const char* vShader = R"(
#version 330
layout (location = 0) in vec3 pos;
void main()
{
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f);
}
)";

// Fragment Shader: Dejamos el color de salida fijo en blanco para todas las letras
static const char* fShader = R"(
#version 330
out vec4 color;
void main()
{
	color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
)";

void CrearLetras()
{
	// Armando las iniciales F, G, C a puro triángulo
	// Cada rectángulo lleva 2 triángulos (6 vértices)
	GLfloat vertices[] = {
		// ================= LETRA F =================
		// Poste vertical de la F
		-0.9f, -0.5f, 0.0f,   -0.75f, -0.5f, 0.0f,  -0.9f, 0.5f, 0.0f,
		-0.75f, -0.5f, 0.0f,  -0.75f, 0.5f, 0.0f,   -0.9f, 0.5f, 0.0f,
		// Barra superior de la F
		-0.75f, 0.35f, 0.0f,  -0.4f, 0.35f, 0.0f,   -0.75f, 0.5f, 0.0f,
		-0.4f, 0.35f, 0.0f,   -0.4f, 0.5f, 0.0f,    -0.75f, 0.5f, 0.0f,
		// Barra de en medio de la F
		-0.75f, -0.075f, 0.0f,-0.5f, -0.075f, 0.0f, -0.75f, 0.075f, 0.0f,
		-0.5f, -0.075f, 0.0f, -0.5f, 0.075f, 0.0f,  -0.75f, 0.075f, 0.0f,

		// ================= LETRA G =================
		// Poste izquierdo de la G
		-0.2f, -0.5f, 0.0f,   -0.05f, -0.5f, 0.0f,  -0.2f, 0.5f, 0.0f,
		-0.05f, -0.5f, 0.0f,  -0.05f, 0.5f, 0.0f,   -0.2f, 0.5f, 0.0f,
		// Barra superior de la G
		-0.05f, 0.35f, 0.0f,   0.3f, 0.35f, 0.0f,   -0.05f, 0.5f, 0.0f,
		 0.3f, 0.35f, 0.0f,    0.3f, 0.5f, 0.0f,    -0.05f, 0.5f, 0.0f,
		 // Barra inferior de la G
		 -0.05f, -0.5f, 0.0f,   0.3f, -0.5f, 0.0f,   -0.05f, -0.35f, 0.0f,
		  0.3f, -0.5f, 0.0f,    0.3f, -0.35f, 0.0f,  -0.05f, -0.35f, 0.0f,
		  // Pedacito derecho abajo de la G
		   0.15f, -0.5f, 0.0f,   0.3f, -0.5f, 0.0f,    0.15f, 0.075f, 0.0f,
		   0.3f, -0.5f, 0.0f,    0.3f, 0.075f, 0.0f,   0.15f, 0.075f, 0.0f,
		   // Barra de en medio de la G
			0.0f, -0.075f, 0.0f,  0.15f, -0.075f, 0.0f, 0.0f, 0.075f, 0.0f,
			0.15f, -0.075f, 0.0f, 0.15f, 0.075f, 0.0f,  0.0f, 0.075f, 0.0f,

			// ================= LETRA C =================
			// Poste izquierdo de la C
			 0.5f, -0.5f, 0.0f,    0.65f, -0.5f, 0.0f,   0.5f, 0.5f, 0.0f,
			 0.65f, -0.5f, 0.0f,   0.65f, 0.5f, 0.0f,    0.5f, 0.5f, 0.0f,
			 // Barra superior de la C
			  0.65f, 0.35f, 0.0f,   1.0f, 0.35f, 0.0f,    0.65f, 0.5f, 0.0f,
			  1.0f, 0.35f, 0.0f,    1.0f, 0.5f, 0.0f,     0.65f, 0.5f, 0.0f,
			  // Barra inferior de la C
			   0.65f, -0.5f, 0.0f,   1.0f, -0.5f, 0.0f,    0.65f, -0.35f, 0.0f,
			   1.0f, -0.5f, 0.0f,    1.0f, -0.35f, 0.0f,   0.65f, -0.35f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1];
	theCode[0] = shaderCode;
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);
	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error al compilar el shader %d: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);
}

void CompileShaders() {
	shader = glCreateProgram();
	if (!shader)
	{
		printf("Error creando el shader program");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);

	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error al linkear: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error al validar: %s \n", eLog);
		return;
	}
}

int main()
{
	if (!glfwInit())
	{
		printf("Fallo al inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Practica Iniciales y Random RGB", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo al crear la ventana");
		glfwTerminate();
		return 1;
	}

	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);
	glfwMakeContextCurrent(mainWindow);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glViewport(0, 0, BufferWidth, BufferHeight);

	CrearLetras();
	CompileShaders();

	// REQUISITO 1: Semilla para los colores aleatorios para que cambie en cada ejecución
	srand((unsigned int)time(NULL));

	// Variables para controlar el tiempo del cambio de color
	double ultimoTiempo = glfwGetTime();
	float randR = 0.0f, randG = 0.0f, randB = 0.0f;

	// Loop principal
	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		double tiempoActual = glfwGetTime();

		// Revisar si ya pasaron 2 segundos
		if (tiempoActual - ultimoTiempo >= 2.0) {
			// Generar RGB aleatorio de 0.0 a 1.0
			randR = (float)rand() / (float)RAND_MAX;
			randG = (float)rand() / (float)RAND_MAX;
			randB = (float)rand() / (float)RAND_MAX;

			// Actualizar el temporizador
			ultimoTiempo = tiempoActual;
		}

		// Asignar el color de fondo generado
		glClearColor(randR, randG, randB, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glBindVertexArray(VAO);

		// Dibujamos las letras. En total sumé 66 vértices entre todos los rectángulos que armé
		glDrawArrays(GL_TRIANGLES, 0, 66);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}