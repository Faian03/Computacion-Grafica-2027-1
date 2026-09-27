/*
Práctica 5: Optimización y Carga de Modelos
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_m.h"
#include "Camera.h"
//#include "Sphere.h" //no se usa en esta practica
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList; //solo recibe xyz
std::vector<MeshColor*> meshListColor; // recibe xyz rgb
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;
//Lista de Modelos a importar (Rover separado en partes, cada una con su origen en su punto de giro)
Model Cuerpo_M;
Model BrazoBase_M, BrazoSuperior_M, Antebrazo_M, Pinza_M;
GLfloat giroBaseBrazo = 0.0f, giroHombro = 0.0f, giroCodo = 0.0f, giroPinza = 0.0f;
// Centro del bloque principal del chasis, medido antes de recentrar el OBJ.
const glm::vec3 centroCuerpo(-1.366451000f, 3.685409000f, 0.000000000f);
Model PataDD_M, PataDI_M, PataMD_M, PataMI_M, PataTD_M, PataTI_M;			//Delantera/Media/Trasera - Derecha/Izquierda
Model RuedaDD_M, RuedaDI_M, RuedaMD_M, RuedaMI_M, RuedaTD_M, RuedaTI_M;
// Holocron: todas las piezas usan el centro del conjunto como origen.
Model CentroHolocron_M, EsquinaHolocron_M[8];
GLfloat giroEsquina[8] = { 0.0f };
const float escalaHolocron = 0.24f;

// Satelite: los OBJ moviles tienen el origen en su union.
Model CuerpoSatelite_M, PanelPosZ_M, PanelNegZ_M, Antena_M;
glm::vec3 posicionSatelite(5.2f, 0.0f, -2.0f);
const float escalaSatelite = 2.2f;
const glm::vec3 pivotePanelPosZ(0.0f, -0.213f, 0.15f);
const glm::vec3 pivotePanelNegZ(0.0f, -0.213f, -0.15f);
const glm::vec3 pivoteAntena(-0.459f, -0.09f, 0.0f);
GLfloat giroPanelPosZ = 0.0f, giroPanelNegZ = 0.0f, giroAntena = 0.0f;

const float escalaRover = 0.3f;
//Colores solidos (RGB de 0 a 1) de cada parte del Rover
const glm::vec3 colorCuerpo = glm::vec3(0.90f, 0.90f, 0.85f);	//blanco
const glm::vec3 colorBrazo = glm::vec3(0.55f, 0.57f, 0.60f);	//gris metalico
const glm::vec3 colorPata = glm::vec3(0.95f, 0.45f, 0.10f);	//naranja
const glm::vec3 colorRueda = glm::vec3(0.12f, 0.12f, 0.12f);	//negro
GLfloat tiempoAnim = 0.0f;		//tiempo acumulado de la animacion de avance
GLfloat tiempoPrevio = 0.0f;

//Lista de Skybox a crear
Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_m.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_m.frag";


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};


	MeshModel* obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel* obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel* obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);


}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



//Dibuja una pata (hija del cuerpo) y su llanta (hija de la pata)
//pivotePata: posicion de la articulacion respecto al cuerpo; posRueda: centro de la llanta respecto a la articulacion
void DibujarPataRueda(glm::mat4 modelCuerpo, Model& pata, Model& rueda, glm::vec3 pivotePata, glm::vec3 posRueda,
	GLfloat anguloPata, GLfloat giroRueda, GLuint uniformModel, GLuint uniformColor)
{
	glm::mat4 model = modelCuerpo;
	model = glm::translate(model, pivotePata - centroCuerpo);
	model = glm::rotate(model, anguloPata * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); //eje Z = eje lateral del rover
	glm::mat4 modelaux = model; //la llanta hereda la rotacion de la pata
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(colorPata));
	pata.RenderModel();

	model = modelaux;
	model = glm::translate(model, posRueda);
	model = glm::rotate(model, giroRueda * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(colorRueda));
	rueda.RenderModel();
}

// Movimiento continuo de los modelos nuevos, en segundos reales.
void ActualizarModelos(bool* keys, GLfloat tiempoCuadro)
{
	GLfloat sentido = (keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) ? -1.0f : 1.0f;
	for (int i = 0; i < 8; i++)
	{
		if (keys[GLFW_KEY_1 + i])
			giroEsquina[i] = fmod(giroEsquina[i] + sentido * 45.0f * tiempoCuadro, 360.0f);
	}

	GLfloat paso = 2.0f * tiempoCuadro;
	if (keys[GLFW_KEY_LEFT]) posicionSatelite.x -= paso;
	if (keys[GLFW_KEY_RIGHT]) posicionSatelite.x += paso;
	if (keys[GLFW_KEY_PAGE_DOWN]) posicionSatelite.y -= paso;
	if (keys[GLFW_KEY_PAGE_UP]) posicionSatelite.y += paso;
	if (keys[GLFW_KEY_UP]) posicionSatelite.z -= paso;
	if (keys[GLFW_KEY_DOWN]) posicionSatelite.z += paso;

	paso = 35.0f * tiempoCuadro;
	if (keys[GLFW_KEY_Z]) giroPanelPosZ += paso;
	if (keys[GLFW_KEY_X]) giroPanelPosZ -= paso;
	if (keys[GLFW_KEY_C]) giroPanelNegZ += paso;
	if (keys[GLFW_KEY_V]) giroPanelNegZ -= paso;
	if (keys[GLFW_KEY_B]) giroAntena += paso;
	if (keys[GLFW_KEY_N]) giroAntena -= paso;
	giroPanelPosZ = glm::clamp(giroPanelPosZ, -60.0f, 60.0f);
	giroPanelNegZ = glm::clamp(giroPanelNegZ, -60.0f, 60.0f);
	giroAntena = glm::clamp(giroAntena, -35.0f, 35.0f);

	// Brazo: cada articulacion transmite su movimiento a las siguientes.
	if (keys[GLFW_KEY_Q]) giroBaseBrazo += sentido * paso;
	if (keys[GLFW_KEY_Y]) giroHombro += sentido * paso;
	if (keys[GLFW_KEY_U]) giroCodo += sentido * paso;
	if (keys[GLFW_KEY_I]) giroPinza += sentido * paso;
	giroBaseBrazo = glm::clamp(giroBaseBrazo, -180.0f, 180.0f);
	giroHombro = glm::clamp(giroHombro, -45.0f, 45.0f);
	giroCodo = glm::clamp(giroCodo, -70.0f, 70.0f);
	giroPinza = glm::clamp(giroPinza, -60.0f, 60.0f);
	if (keys[GLFW_KEY_9]) giroBaseBrazo = giroHombro = giroCodo = giroPinza = 0.0f;

	// 0 restablece solamente el Holocron y el satelite.
	if (keys[GLFW_KEY_0])
	{
		for (int i = 0; i < 8; i++) giroEsquina[i] = 0.0f;
		posicionSatelite = glm::vec3(5.2f, 0.0f, -2.0f);
		giroPanelPosZ = giroPanelNegZ = giroAntena = 0.0f;
	}
}

// Igual que las patas del Rover: padre, traslado a la union y giro local.
void DibujarParteSatelite(glm::mat4 modelSatelite, Model& pieza, glm::vec3 pivote,
	glm::vec3 eje, GLfloat angulo, GLuint uniformModel, GLuint uniformColor)
{
	glm::mat4 model = glm::translate(modelSatelite, pivote);
	model = glm::rotate(model, angulo * toRadians, eje);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	pieza.RenderModel(uniformColor);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 7.0f, 18.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -22.0f, 0.3f, 0.3f);
	//Cargar modelos: cada parte del Rover por separado
	Cuerpo_M.LoadModel("Models/Rover/Cuerpo.obj");
	BrazoBase_M.LoadModel("Models/Rover/Brazo_Base.obj");
	BrazoSuperior_M.LoadModel("Models/Rover/Brazo_Superior.obj");
	Antebrazo_M.LoadModel("Models/Rover/Brazo_Antebrazo.obj");
	Pinza_M.LoadModel("Models/Rover/Brazo_Pinza.obj");
	PataDD_M.LoadModel("Models/Rover/Pata_DelanteraDer.obj");
	PataDI_M.LoadModel("Models/Rover/Pata_DelanteraIzq.obj");
	PataMD_M.LoadModel("Models/Rover/Pata_MediaDer.obj");
	PataMI_M.LoadModel("Models/Rover/Pata_MediaIzq.obj");
	PataTD_M.LoadModel("Models/Rover/Pata_TraseraDer.obj");
	PataTI_M.LoadModel("Models/Rover/Pata_TraseraIzq.obj");
	RuedaDD_M.LoadModel("Models/Rover/Rueda_DelanteraDer.obj");
	RuedaDI_M.LoadModel("Models/Rover/Rueda_DelanteraIzq.obj");
	RuedaMD_M.LoadModel("Models/Rover/Rueda_MediaDer.obj");
	RuedaMI_M.LoadModel("Models/Rover/Rueda_MediaIzq.obj");
	RuedaTD_M.LoadModel("Models/Rover/Rueda_TraseraDer.obj");
	RuedaTI_M.LoadModel("Models/Rover/Rueda_TraseraIzq.obj");


	// Cargar una vez las piezas de los otros dos modelos.
	CentroHolocron_M.LoadModel("Models/Holocron/Centro.obj");
	for (int i = 0; i < 8; i++)
		EsquinaHolocron_M[i].LoadModel("Models/Holocron/Esquina_" + std::to_string(i + 1) + ".obj");
	CuerpoSatelite_M.LoadModel("Models/Satelite/Cuerpo.obj");
	PanelPosZ_M.LoadModel("Models/Satelite/PanelPosZ.obj");
	PanelNegZ_M.LoadModel("Models/Satelite/PanelNegZ.obj");
	Antena_M.LoadModel("Models/Satelite/Antena.obj");
	printf("\nRover: R, F/G/H/J/K/L (+ Shift), P animar, O reiniciar patas.\n");
	printf("Brazo: Q base, Y hombro, U codo, I pinza; Shift invierte; 9 reinicia.\n");
	printf("Holocron: mantener 1 a 8; Shift invierte el giro.\n");
	printf("Satelite: flechas X/Z, Page Up/Down Y; Z/X y C/V paneles; B/N antena.\n");
	printf("0 reinicia los modelos nuevos. Camara: W/A/S/D y raton.\n\n");

	//Crear Skybox con sus 6 texturas
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f * toRadians, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	lastTime = tiempoPrevio = static_cast<GLfloat>(glfwGetTime());

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		GLfloat tiempoCuadro = now - lastTime;
		deltaTime = tiempoCuadro;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		ActualizarModelos(mainWindow.getsKeys(), tiempoCuadro);
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Se dibuja el Skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// INICIA DIBUJO DEL PISO
		color = glm::vec3(0.5f, 0.5f, 0.5f); //piso de color gris
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[2]->RenderMeshModel();

		//------------*INICIA DIBUJO DEL ROVER (JERARQUIA)-------------------*
		//Angulos de las patas: por teclado (F,G,H,J,K,L y Shift) o automaticos con la tecla P
		GLfloat pataDD = mainWindow.getarticulacion1(), pataDI = mainWindow.getarticulacion2();
		GLfloat pataMD = mainWindow.getarticulacion3(), pataMI = mainWindow.getarticulacion4();
		GLfloat pataTD = mainWindow.getarticulacion5(), pataTI = mainWindow.getarticulacion6();
		if (mainWindow.getAnimar())
		{
			tiempoAnim += now - tiempoPrevio;
			//Marcha en tripode: DI, MD y TI van juntas; DD, MI y TD en sentido contrario
			GLfloat onda = 45.0f * sin(tiempoAnim * 3.0f);
			pataDI = pataMD = pataTI = onda;
			pataDD = pataMI = pataTD = -onda;
		}
		tiempoPrevio = now;
		GLfloat giroRueda = -tiempoAnim * 180.0f; //las ruedas giran mientras avanza

		//Cuerpo (raiz de la jerarquia)
		model = glm::mat4(1.0);
		// Compensacion fija para conservar la posicion inicial al centrar el cuerpo.
		model = glm::translate(model, glm::vec3(-5.2f, -2.0f, -1.5f) + escalaRover * centroCuerpo);
		model = glm::rotate(model, mainWindow.getrotay() * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaRover));
		modelaux = model; //se guarda la matriz del cuerpo para que la hereden sus hijos
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorCuerpo));
		Cuerpo_M.RenderModel();

		// Brazo: base -> segmento superior -> antebrazo -> pinza.
		model = glm::translate(modelaux, glm::vec3(1.700f, 5.180f, -1.100f) - centroCuerpo);
		model = glm::rotate(model, giroBaseBrazo * toRadians, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoBase_M.RenderModel(uniformColor);
		model = glm::translate(model, glm::vec3(0.001f, 0.802f, 0.0f));
		model = glm::rotate(model, giroHombro * toRadians, glm::vec3(0, 0, 1));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoSuperior_M.RenderModel(uniformColor);
		model = glm::translate(model, glm::vec3(2.535f, 2.556f, 0.0f));
		model = glm::rotate(model, giroCodo * toRadians, glm::vec3(0, 0, 1));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Antebrazo_M.RenderModel(uniformColor);
		model = glm::translate(model, glm::vec3(-2.648f, 2.949f, 0.187f));
		model = glm::rotate(model, giroPinza * toRadians, glm::vec3(0, 0, 1));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pinza_M.RenderModel(uniformColor);

		//Patas (hijas del cuerpo) con su llanta (hija de la pata)
		//	Llanta delantera derecha
		DibujarPataRueda(modelaux, PataDD_M, RuedaDD_M, glm::vec3(2.100f, 4.208f, 3.054f), glm::vec3(3.612f, -2.503f, 0.016f), pataDD, giroRueda, uniformModel, uniformColor);
		//	Llanta delantera izquierda
		DibujarPataRueda(modelaux, PataDI_M, RuedaDI_M, glm::vec3(2.100f, 4.208f, -3.066f), glm::vec3(3.610f, -2.503f, -0.004f), pataDI, giroRueda, uniformModel, uniformColor);
		//	Llanta media derecha
		DibujarPataRueda(modelaux, PataMD_M, RuedaMD_M, glm::vec3(-3.040f, 3.290f, 3.000f), glm::vec3(2.095f, -1.878f, 1.159f), pataMD, giroRueda, uniformModel, uniformColor);
		//	Llanta media izquierda
		DibujarPataRueda(modelaux, PataMI_M, RuedaMI_M, glm::vec3(-3.040f, 3.290f, -3.000f), glm::vec3(2.093f, -1.878f, -1.159f), pataMI, giroRueda, uniformModel, uniformColor);
		//	Llanta trasera derecha
		DibujarPataRueda(modelaux, PataTD_M, RuedaTD_M, glm::vec3(-3.040f, 3.290f, 3.000f), glm::vec3(-2.091f, -1.878f, 1.159f), pataTD, giroRueda, uniformModel, uniformColor);
		//	Llanta trasera izquierda
		DibujarPataRueda(modelaux, PataTI_M, RuedaTI_M, glm::vec3(-3.040f, 3.290f, -3.000f), glm::vec3(-2.073f, -1.904f, -1.159f), pataTI, giroRueda, uniformModel, uniformColor);


		//------------ HOLocron: centro y ocho esquinas independientes ------------
		glm::mat4 modelHolocron(1.0f);
		modelHolocron = glm::translate(modelHolocron, glm::vec3(0.0f, 0.0f, -2.0f));
		modelHolocron = glm::rotate(modelHolocron, 25.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelHolocron = glm::scale(modelHolocron, glm::vec3(escalaHolocron));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelHolocron));
		CentroHolocron_M.RenderModel(uniformColor);
		for (int i = 0; i < 8; i++)
		{
			// Cada esquina parte del mismo padre; su geometria ya esta desplazada del centro.
			model = glm::rotate(modelHolocron, giroEsquina[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			EsquinaHolocron_M[i].RenderModel(uniformColor);
		}

		//------------ Satelite: cuerpo y tres piezas articuladas ------------
		glm::mat4 modelSatelite(1.0f);
		modelSatelite = glm::translate(modelSatelite, posicionSatelite);
		modelSatelite = glm::rotate(modelSatelite, 65.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelSatelite = glm::scale(modelSatelite, glm::vec3(escalaSatelite));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelSatelite));
		CuerpoSatelite_M.RenderModel(uniformColor);
		DibujarParteSatelite(modelSatelite, PanelPosZ_M, pivotePanelPosZ, glm::vec3(1.0f, 0.0f, 0.0f), giroPanelPosZ, uniformModel, uniformColor);
		DibujarParteSatelite(modelSatelite, PanelNegZ_M, pivotePanelNegZ, glm::vec3(1.0f, 0.0f, 0.0f), giroPanelNegZ, uniformModel, uniformColor);
		DibujarParteSatelite(modelSatelite, Antena_M, pivoteAntena, glm::vec3(0.0f, 0.0f, 1.0f), giroAntena, uniformModel, uniformColor);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
