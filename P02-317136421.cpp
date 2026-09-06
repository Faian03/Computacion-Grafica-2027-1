//Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>

//glm
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//clases para dar orden y limpieza al código
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

const float toRadians = 3.14159265f / 180.0;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader> shaderList;

// Shaders originales 
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";

static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";

// Rutas de los nuevos shaders de color
static const char* vShaderRojo = "shaders/shaderrojo.vert";
static const char* vShaderVerde = "shaders/shaderverde.vert";
static const char* vShaderAzul = "shaders/shaderazul.vert";
static const char* vShaderCafe = "shaders/shadercafe.vert";
static const char* vShaderMagenta = "shaders/shadermagenta.vert";
static const char* vShaderAmarillo = "shaders/shaderamarillo.vert";
static const char* vShaderNegro = "shaders/shadernegro.vert";

float angulo = 0.0f;

// ----------------- OBJETOS 3D (PIRÁMIDES Y CUBOS) -----------------
void CreaPiramide()
{
	unsigned int indices[] = { 0,1,2, 1,3,2, 3,0,2, 1,0,3 };
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,	 //0
		0.5f, -0.5f, 0.0f,	 //1
		0.0f, 0.5f, -0.25f,	 //2
		0.0f, -0.5f, -0.5f,	 //3
	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1); // meshList[0] = Pirámide
}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		0, 1, 2,  2, 3, 0,  1, 5, 6,  6, 2, 1,
		7, 6, 5,  5, 4, 7,  4, 0, 3,  3, 7, 4,
		4, 5, 1,  1, 0, 4,  3, 2, 6,  6, 7, 3
	};
	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo); // meshList[1] = Cubo
}

// ----------------- OBJETOS 2D (LAS INICIALES) -----------------
void CrearInicialesColoridas()
{
	GLfloat vertices_iniciales[] = {
		// ================= LETRA F (ROJO) =================
		-0.9f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   -0.75f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		-0.9f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   -0.75f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		-0.75f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   -0.9f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		-0.75f, 0.35f, 0.0f,  1.0f, 0.0f, 0.0f,   -0.4f, 0.35f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.75f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   -0.4f, 0.35f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.4f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   -0.75f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.75f, -0.075f, 0.0f,1.0f, 0.0f, 0.0f,   -0.5f, -0.075f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.75f, 0.075f, 0.0f, 1.0f, 0.0f, 0.0f,   -0.5f, -0.075f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.075f, 0.0f,  1.0f, 0.0f, 0.0f,   -0.75f, 0.075f, 0.0f, 1.0f, 0.0f, 0.0f,

		// ================= LETRA G (VERDE) =================
		-0.2f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   -0.05f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.2f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   -0.05f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.05f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   -0.2f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		-0.05f, 0.35f, 0.0f,  0.0f, 1.0f, 0.0f,    0.3f, 0.35f, 0.0f,   0.0f, 1.0f, 0.0f,
		-0.05f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,    0.3f, 0.35f, 0.0f,   0.0f, 1.0f, 0.0f,
		 0.3f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   -0.05f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		-0.05f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,    0.3f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		-0.05f, -0.35f, 0.0f, 0.0f, 1.0f, 0.0f,    0.3f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		 0.3f, -0.35f, 0.0f,  0.0f, 1.0f, 0.0f,   -0.05f, -0.35f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.15f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,    0.3f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		 0.15f, 0.075f, 0.0f, 0.0f, 1.0f, 0.0f,    0.3f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		 0.3f, 0.075f, 0.0f,  0.0f, 1.0f, 0.0f,    0.15f, 0.075f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f, -0.075f, 0.0f, 0.0f, 1.0f, 0.0f,    0.15f, -0.075f, 0.0f,0.0f, 1.0f, 0.0f,
		 0.0f, 0.075f, 0.0f,  0.0f, 1.0f, 0.0f,    0.15f, -0.075f, 0.0f,0.0f, 1.0f, 0.0f,
		 0.15f, 0.075f, 0.0f, 0.0f, 1.0f, 0.0f,    0.0f, 0.075f, 0.0f,  0.0f, 1.0f, 0.0f,

		 // ================= LETRA C (AZUL) =================
		  0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,    0.65f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
		  0.5f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    0.65f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
		  0.65f, 0.5f, 0.0f,   0.0f, 0.0f, 1.0f,    0.5f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		  0.65f, 0.35f, 0.0f,  0.0f, 0.0f, 1.0f,    1.0f, 0.35f, 0.0f,   0.0f, 0.0f, 1.0f,
		  0.65f, 0.5f, 0.0f,   0.0f, 0.0f, 1.0f,    1.0f, 0.35f, 0.0f,   0.0f, 0.0f, 1.0f,
		  1.0f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    0.65f, 0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		  0.65f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    1.0f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		  0.65f, -0.35f, 0.0f, 0.0f, 0.0f, 1.0f,    1.0f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		  1.0f, -0.35f, 0.0f,  0.0f, 0.0f, 1.0f,    0.65f, -0.35f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	MeshColor* misIniciales = new MeshColor();
	misIniciales->CreateMeshColor(vertices_iniciales, 396);
	meshColorList.push_back(misIniciales); // Índice 0
}

void CreateShaders()
{
	// [0] Shader original
	Shader* shader0 = new Shader();
	shader0->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader0);

	// [1] Shader para las iniciales
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader1);

	// -------- LOS NUEVOS SHADERS DE COLOR ESTÁTICO --------
	Shader* shaderRojo = new Shader();
	shaderRojo->CreateFromFiles(vShaderRojo, fShader);
	shaderList.push_back(*shaderRojo); // [2]

	Shader* shaderVerde = new Shader();
	shaderVerde->CreateFromFiles(vShaderVerde, fShader);
	shaderList.push_back(*shaderVerde); // [3]

	Shader* shaderAzul = new Shader();
	shaderAzul->CreateFromFiles(vShaderAzul, fShader);
	shaderList.push_back(*shaderAzul); // [4]

	Shader* shaderCafe = new Shader();
	shaderCafe->CreateFromFiles(vShaderCafe, fShader);
	shaderList.push_back(*shaderCafe); // [5]

	Shader* shaderMagenta = new Shader();
	shaderMagenta->CreateFromFiles(vShaderMagenta, fShader);
	shaderList.push_back(*shaderMagenta); // [6]

	Shader* shaderAmarillo = new Shader();
	shaderAmarillo->CreateFromFiles(vShaderAmarillo, fShader);
	shaderList.push_back(*shaderAmarillo); // [7]

	Shader* shaderNegro = new Shader();
	shaderNegro->CreateFromFiles(vShaderNegro, fShader);
	shaderList.push_back(*shaderNegro); // [8]
}

void DibujarObjeto(int indiceShader, int indiceMesh, glm::mat4 model, glm::mat4 projection) {
	shaderList[indiceShader].useShader();
	GLuint uniformModel = shaderList[indiceShader].getModelLocation();
	GLuint uniformProjection = shaderList[indiceShader].getProjectLocation();
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	meshList[indiceMesh]->RenderMesh();
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CreaPiramide(); // meshList[0]
	CrearCubo();    // meshList[1]
	CrearInicialesColoridas(); // meshColorList[0]
	CreateShaders();

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;

	glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -3.0f, 3.0f, 0.1f, 100.0f);
	glm::mat4 model(1.0);

	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// =================== INICIALES COLORIDAS ===================
		shaderList[1].useShader();
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, -4.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshColorList[0]->RenderMeshColor();


		// =================== EL PISO NEGRO ===================
		// El cubo 3D y el cuadrado 2D tienen el MISMO tamaño (1x1). 
		// No se ocupa re-escalar nada, se queda con la original.
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, -5.2f));
		model = glm::scale(model, glm::vec3(10.0f, 0.2f, 1.0f));
		DibujarObjeto(8, 1, model, projection); // Cubo


		// =================== FIGURA IZQUIERDA (TORRE) ===================
		// Postes (Cubos)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.5f, -0.2f, -5.1f));
		model = glm::scale(model, glm::vec3(0.2f, 3.0f, 1.0f));
		DibujarObjeto(5, 1, model, projection);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.5f, -0.2f, -5.1f));
		model = glm::scale(model, glm::vec3(0.2f, 3.0f, 1.0f));
		DibujarObjeto(5, 1, model, projection);

		// Triángulos (Pirámides) 
		// IMPORTANTE: La pirámide 3D mide la mitad del triángulo 2D. 
		// Por lo tanto le ponemos un scale(2.0f) al mero final de la matriz para que coincida.
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 0.8f, -5.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.48f, 0.5f, 1.0f)); // Escala de embonado
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f)); // Compensador de Pirámide
		DibujarObjeto(7, 0, model, projection);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, -0.2f, -5.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.48f, 0.5f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		DibujarObjeto(2, 0, model, projection);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, -1.2f, -5.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.48f, 0.5f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		DibujarObjeto(3, 0, model, projection);


		// =================== FIGURA CENTRAL (CUADRADO Y ROMBOS) ===================
		// Esquinas (Pirámides) - Ocupan el compensador x2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -5.1f));
		model = glm::translate(model, glm::vec3(-0.7f, 0.7f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.5f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f)); // Compensador
		DibujarObjeto(7, 0, model, projection);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -5.1f));
		model = glm::translate(model, glm::vec3(0.7f, 0.7f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.5f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		DibujarObjeto(2, 0, model, projection);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -5.1f));
		model = glm::translate(model, glm::vec3(-0.7f, -0.7f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.5f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		DibujarObjeto(6, 0, model, projection);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -5.1f));
		model = glm::translate(model, glm::vec3(0.7f, -0.7f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
		model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.5f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		DibujarObjeto(3, 0, model, projection);

		// Rombos del Centro (Cubos) - Escalas Originales
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -5.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		DibujarObjeto(4, 1, model, projection);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -4.9f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
		DibujarObjeto(5, 1, model, projection);


		// =================== FIGURA DERECHA (LA TRIFUERZA) ===================
		// Pirámides - Llevan el compensador x2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.0f, -0.2f, -5.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		DibujarObjeto(6, 0, model, projection);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.5f, -1.2f, -5.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		DibujarObjeto(3, 0, model, projection);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.5f, -1.2f, -5.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		DibujarObjeto(2, 0, model, projection);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.0f, -1.2f, -5.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		DibujarObjeto(7, 0, model, projection);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}