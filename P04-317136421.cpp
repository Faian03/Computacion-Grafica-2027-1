/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la R a la K para rotaciones de articulaciones del brazo
Teclas del 1 al 6 para rotación de cada llanta
*/
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtc\random.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"

using std::vector;

const float toRadians = 3.14159265f / 180.0;
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20);

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1,
		7, 6, 5, 5, 4, 7, 4, 0, 3, 3, 7, 4,
		4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2, 1,3,2, 3,0,2, 1,0,3
	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.25f, 0.0f, -0.5f, -0.5f,
	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);
}

void CrearCilindro(int res, float R) {
	int n, i; GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	vector<GLfloat> vertices; vector<unsigned int> indices;

	for (n = 0; n <= (res); n++) {
		if (n != res) { x = R * cos((n)*dt); z = R * sin((n)*dt); }
		else { x = R * cos((0) * dt); z = R * sin((0) * dt); }
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break; case 1: vertices.push_back(y); break; case 2: vertices.push_back(z); break;
			case 3: vertices.push_back(x); break; case 4: vertices.push_back(0.5); break; case 5: vertices.push_back(z); break;
			}
		}
	}
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break; case 1: vertices.push_back(-0.5f); break; case 2: vertices.push_back(z); break;
			}
		}
	}
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break; case 1: vertices.push_back(0.5); break; case 2: vertices.push_back(z); break;
			}
		}
	}
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

void CrearCono(int res, float R) {
	int n, i; GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	vector<GLfloat> vertices; vector<unsigned int> indices;

	vertices.push_back(0.0); vertices.push_back(0.5); vertices.push_back(0.0);
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break; case 1: vertices.push_back(y); break; case 2: vertices.push_back(z); break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt); vertices.push_back(-0.5); vertices.push_back(R * sin(0) * dt);
	for (i = 0; i < res + 2; i++) indices.push_back(i);
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4, 3,2,4, 2,1,4, 1,0,4, 0,1,2, 0,2,4
	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f, 0.5f,-0.5f,-0.5f, -0.5f,-0.5f,-0.5f, -0.5f,-0.5f,0.5f, 0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CrearCubo();
	CrearPiramideTriangular();
	CrearCilindro(18, 1.0f);
	CrearCono(25, 2.0f);
	CrearPiramideCuadrangular();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	sp.init();
	sp.load();

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelaux2(1.0);

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();


		// ============================================
		// CABINA Y BASE
		// ============================================
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, -4.0f));
		modelaux = model;

		// Cabina Magenta
		model = glm::translate(model, glm::vec3(1.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(1.0f, 0.0f, 1.0f); // Magenta
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Base Verde
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(10.0f, 1.5f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f); // Verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		modelaux2 = model; // MATRIZ PRINCIPAL PARA CONECTAR SUSPENSIÓN A LA BASE


		// ============================================
		// BRAZO ROBÓTICO ORIGINAL (6 ARTICULACIONES)
		// ============================================

		// BASE PARA EL BRAZO
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-3.75f, 1.25f, 3.0f));
		modelaux = model;
		color = glm::vec3(0.0f, 0.0f, 1.0f); // Azul
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Articulación 1 (Esfera roja - Rotación eje X con tecla R)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.75f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f); // Rojo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// Brazo parte 1
		model = modelaux;
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.25f, 1.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 3.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f); // Blanco
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		// Articulación 2 (Esfera Amarilla - Rotación con tecla F)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0, 1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f); // Amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// Brazo parte 2
		model = modelaux;
		model = glm::rotate(model, glm::radians(-75.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 3.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.8f, 0.8f); // Gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		// Articulación 3 y 4 (Muñeca Azul - Rotación con teclas G y H)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0, 1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f)); // Doblez principal (G)
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Inclinación fija hacia adentro
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f)); // Torsión lateral (H)
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 1.0f); // Azul
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// Punta Parte 1 (Base Gris Claro - Tecla J)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación sobre su propio eje (J)
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.7f, 0.7f, 0.7f); // Gris claro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Punta Parte 2 (Conector Gris Oscuro)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.35f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.3f, 0.2f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f); // Gris oscuro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Punta Parte 3 (Naranja)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.25f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.3f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.6f, 0.0f); // Naranja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Punta Parte 4 (Pinza Izquierda animada con Tecla K)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.1f, 0.25f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f)); // Abre/Cierra (K)
		model = glm::scale(model, glm::vec3(0.05f, 0.2f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.1f, 0.1f, 0.1f); // Negro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Punta Parte 4 (Pinza Derecha animada con Tecla K)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.1f, 0.25f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f)); // Cierra/Abre inverso (K)
		model = glm::scale(model, glm::vec3(0.05f, 0.2f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.1f, 0.1f, 0.1f); // Negro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();


		// =========================================================
		// --- SISTEMA DE 6 LLANTAS CON SOPORTE HACIA ADENTRO ---
		// =========================================================

		// Función para llantas del LADO IZQUIERDO (Soporte L apuntando hacia adentro: -Z)
		auto DibujarLlantaIzq = [&](glm::mat4 base, float posX, GLfloat rotLlanta) {
			glm::mat4 m = base;
			m = glm::translate(m, glm::vec3(posX, 0.0f, 4.5f));
			m = glm::rotate(m, glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 anclaje = m;

			// Brazo de suspensión
			m = glm::translate(m, glm::vec3(-1.5f, 0.0f, 0.0f));
			m = glm::scale(m, glm::vec3(3.0f, 0.4f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
			glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.7f, 0.7f, 0.7f)));
			meshList[0]->RenderMesh();

			// Articulación azul
			m = anclaje;
			m = glm::translate(m, glm::vec3(-3.0f, 0.0f, 0.0f));
			glm::mat4 artLlanta = m;
			m = glm::scale(m, glm::vec3(0.6f, 0.6f, 0.6f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
			glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.3f, 1.0f)));
			sp.render();

			// Soporte "L" Horizontal (-Z)
			m = artLlanta;
			m = glm::translate(m, glm::vec3(0.0f, -0.2f, -0.4f));
			m = glm::rotate(m, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			m = glm::scale(m, glm::vec3(0.2f, 0.8f, 0.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
			glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.3f, 0.3f, 0.3f)));
			meshList[2]->RenderMeshGeometry();

			// Soporte "L" Vertical
			m = artLlanta;
			m = glm::translate(m, glm::vec3(0.0f, -0.6f, -0.8f));
			glm::mat4 ejeRueda = glm::translate(artLlanta, glm::vec3(0.0f, -1.0f, -0.8f));
			m = glm::scale(m, glm::vec3(0.2f, 0.8f, 0.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
			glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.3f, 0.3f, 0.3f)));
			meshList[2]->RenderMeshGeometry();

			// Llanta interactiva (Rota con el teclado)
			m = ejeRueda;
			m = glm::rotate(m, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			m = glm::rotate(m, glm::radians(rotLlanta), glm::vec3(0.0f, 1.0f, 0.0f));
			m = glm::scale(m, glm::vec3(1.5f, 0.5f, 1.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
			glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.15f, 0.15f, 0.15f)));
			meshList[2]->RenderMeshGeometry();
			};

		// Función para llantas del LADO DERECHO (Soporte L apuntando hacia adentro: +Z)
		auto DibujarLlantaDer = [&](glm::mat4 base, float posX, GLfloat rotLlanta) {
			glm::mat4 m = base;
			m = glm::translate(m, glm::vec3(posX, 0.0f, -4.5f));
			m = glm::rotate(m, glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 anclaje = m;

			// Brazo de suspensión
			m = glm::translate(m, glm::vec3(-1.5f, 0.0f, 0.0f));
			m = glm::scale(m, glm::vec3(3.0f, 0.4f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
			glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.7f, 0.7f, 0.7f)));
			meshList[0]->RenderMesh();

			// Articulación azul
			m = anclaje;
			m = glm::translate(m, glm::vec3(-3.0f, 0.0f, 0.0f));
			glm::mat4 artLlanta = m;
			m = glm::scale(m, glm::vec3(0.6f, 0.6f, 0.6f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
			glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.3f, 1.0f)));
			sp.render();

			// Soporte "L" Horizontal (+Z)
			m = artLlanta;
			m = glm::translate(m, glm::vec3(0.0f, -0.2f, 0.4f));
			m = glm::rotate(m, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			m = glm::scale(m, glm::vec3(0.2f, 0.8f, 0.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
			glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.3f, 0.3f, 0.3f)));
			meshList[2]->RenderMeshGeometry();

			// Soporte "L" Vertical
			m = artLlanta;
			m = glm::translate(m, glm::vec3(0.0f, -0.6f, 0.8f));
			glm::mat4 ejeRueda = glm::translate(artLlanta, glm::vec3(0.0f, -1.0f, 0.8f));
			m = glm::scale(m, glm::vec3(0.2f, 0.8f, 0.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
			glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.3f, 0.3f, 0.3f)));
			meshList[2]->RenderMeshGeometry();

			// Llanta interactiva (Rota con el teclado)
			m = ejeRueda;
			m = glm::rotate(m, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			m = glm::rotate(m, glm::radians(rotLlanta), glm::vec3(0.0f, 1.0f, 0.0f));
			m = glm::scale(m, glm::vec3(1.5f, 0.5f, 1.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
			glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.15f, 0.15f, 0.15f)));
			meshList[2]->RenderMeshGeometry();
			};

		// Dibuja las 6 llantas dinámicamente con posiciones X (-4.0f Frontal, 0.0f Central, 4.0f Trasera)
		DibujarLlantaIzq(modelaux2, -4.0f, mainWindow.getLlanta1()); // Tecla 1
		DibujarLlantaIzq(modelaux2, 0.0f, mainWindow.getLlanta2()); // Tecla 2
		DibujarLlantaIzq(modelaux2, 4.0f, mainWindow.getLlanta3()); // Tecla 3

		DibujarLlantaDer(modelaux2, -4.0f, mainWindow.getLlanta4()); // Tecla 4
		DibujarLlantaDer(modelaux2, 0.0f, mainWindow.getLlanta5()); // Tecla 5
		DibujarLlantaDer(modelaux2, 4.0f, mainWindow.getLlanta6()); // Tecla 6


		// =========================================================
		// --- EJERCICIO 2: SONDA ESPACIAL (SATÉLITE) ---
		// =========================================================

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 8.0f, 0.0f));
		model = glm::rotate(model, (GLfloat)now * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 centroSonda = model;

		// Núcleo de la Sonda 
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Propulsor Inferior
		model = centroSonda;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.0f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry();

		// Antena Superior
		model = centroSonda;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
		glm::mat4 baseAntena = model;

		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 2.0f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		model = baseAntena;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// Panel Solar Izquierdo
		model = centroSonda;
		model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 0.0f));
		glm::mat4 basePanelIzq = model;

		model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 1.0f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		model = basePanelIzq;
		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 0.1f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.2f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Panel Solar Derecho
		model = centroSonda;
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
		glm::mat4 basePanelDer = model;

		model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 1.0f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		model = basePanelDer;
		model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 0.1f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.2f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();


		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}