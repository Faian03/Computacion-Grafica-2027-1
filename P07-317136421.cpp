/*
Práctica 7: Iluminación 1 
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
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dadoTexture;
Texture holocronTexture;
// Añadir Texturas del dado y del holocron


Model Cuerpo_M;
Model BrazoBase_M, BrazoSuperior_M, Antebrazo_M, Pinza_M;
GLfloat giroBaseBrazo = 0.0f, giroHombro = 0.0f, giroCodo = 0.0f, giroPinza = 0.0f;
// Centro del bloque principal del chasis, medido antes de recentrar el OBJ.
const glm::vec3 centroCuerpo(-1.366451000f, 3.685409000f, 0.000000000f);
Model PataDD_M, PataDI_M, PataMD_M, PataMI_M, PataTD_M, PataTI_M;			//Delantera/Media/Trasera - Derecha/Izquierda
Model RuedaDD_M, RuedaDI_M, RuedaMD_M, RuedaMI_M, RuedaTD_M, RuedaTI_M;
const float escalaRover = 0.3f;
//Colores solidos (RGB de 0 a 1) de cada parte del Rover
const glm::vec3 colorCuerpo = glm::vec3(0.90f, 0.90f, 0.85f);	//blanco
const glm::vec3 colorBrazo = glm::vec3(0.55f, 0.57f, 0.60f);	//gris metalico
const glm::vec3 colorPata = glm::vec3(0.95f, 0.45f, 0.10f);	//naranja
const glm::vec3 colorRueda = glm::vec3(0.12f, 0.12f, 0.12f);	//negro
GLfloat tiempoAnim = 0.0f;		//tiempo acumulado de la animacion de avance
GLfloat tiempoPrevio = 0.0f;

GLfloat angulosPatas[6] = {};
GLfloat giroRover = 0.0f;
bool animarRover = false;
bool teclaPAnterior = false;
//Añadir modelo del avión.
Model Avion_M;
glm::vec3 posicionRover(0.0f);
glm::vec3 posicionAvion(0.0f, 5.0f, 6.0f);
GLfloat giroAvion = 90.0f;
GLfloat inclinacionAvion = 15.9454f; // La nariz apunta al centro del holocron.

// Las mismas matrices padre se usan para dibujar y colocar los faros.
glm::mat4 MatrizRover()
{
    glm::mat4 raiz = glm::translate(glm::mat4(1.0f),
        glm::vec3(1.0f, -1.0f, 6.0f) + escalaRover * centroCuerpo + posicionRover);
    raiz = glm::rotate(raiz, giroRover * toRadians, glm::vec3(0, 1, 0));
    return glm::scale(raiz, glm::vec3(escalaRover));
}

glm::mat4 MatrizAvion()
{
    glm::mat4 raiz = glm::translate(glm::mat4(1.0f), posicionAvion);
    raiz = glm::rotate(raiz, giroAvion * toRadians, glm::vec3(0, 1, 0));
    raiz = glm::rotate(raiz, inclinacionAvion * toRadians, glm::vec3(1, 0, 0));
    return glm::scale(raiz, glm::vec3(0.7f));
}

void ColocarFaro(SpotLight& faro, const glm::mat4& padre,
    const glm::vec3& posicionLocal, const glm::vec3& direccionLocal)
{
    // w=1 hereda la traslacion; w=0 transforma solo la direccion.
    glm::vec3 posicion = glm::vec3(padre * glm::vec4(posicionLocal, 1.0f));
    glm::vec3 direccion = glm::normalize(glm::vec3(padre * glm::vec4(direccionLocal, 0.0f)));
    faro.SetFlash(posicion, direccion);
}

void ActualizarAvance(bool* keys, GLfloat segundos)
{
    float pasoRover = float(keys[GLFW_KEY_UP]) - float(keys[GLFW_KEY_DOWN]);
    glm::vec3 frenteRover = glm::normalize(glm::vec3(MatrizRover() * glm::vec4(1, 0, 0, 0)));
    posicionRover += frenteRover * (pasoRover * 2.0f * segundos);
    if (keys[GLFW_KEY_C]) giroAvion += 45.0f * segundos;
    if (keys[GLFW_KEY_V]) giroAvion -= 45.0f * segundos;
    float pasoAvion = float(keys[GLFW_KEY_Z]) - float(keys[GLFW_KEY_X]);
    glm::vec3 frenteAvion = glm::normalize(glm::vec3(MatrizAvion() * glm::vec4(0, 0, -1, 0)));
    posicionAvion += frenteAvion * (pasoAvion * 3.0f * segundos);
    float subida = float(keys[GLFW_KEY_T]) - float(keys[GLFW_KEY_B]);
    posicionAvion.y = glm::max(1.0f, posicionAvion.y + subida * 2.0f * segundos);
}


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


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
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, 1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, 1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);

	MeshModel *obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshListModel.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	// Un logo en cada cara.
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.50000000f, -0.50000000f, 0.50000000f, 0.00520833f, 0.50781250f, 0.00000000f, 0.00000000f, 1.00000000f,	//0
		0.50000000f, -0.50000000f, 0.50000000f, 0.32812500f, 0.50781250f, 0.00000000f, 0.00000000f, 1.00000000f,	//1
		0.50000000f, 0.50000000f, 0.50000000f, 0.32812500f, 0.99218750f, 0.00000000f, 0.00000000f, 1.00000000f,	//2
		-0.50000000f, 0.50000000f, 0.50000000f, 0.00520833f, 0.99218750f, 0.00000000f, 0.00000000f, 1.00000000f,	//3
		// right
		//x		y		z		S		T
		0.50000000f, -0.50000000f, 0.50000000f, 0.33854167f, 0.50781250f, 1.00000000f, 0.00000000f, 0.00000000f,
		0.50000000f, -0.50000000f, -0.50000000f, 0.66145833f, 0.50781250f, 1.00000000f, 0.00000000f, 0.00000000f,
		0.50000000f, 0.50000000f, -0.50000000f, 0.66145833f, 0.99218750f, 1.00000000f, 0.00000000f, 0.00000000f,
		0.50000000f, 0.50000000f, 0.50000000f, 0.33854167f, 0.99218750f, 1.00000000f, 0.00000000f, 0.00000000f,
		// back
		0.50000000f, -0.50000000f, -0.50000000f, 0.67187500f, 0.50781250f, 0.00000000f, 0.00000000f, -1.00000000f,
		-0.50000000f, -0.50000000f, -0.50000000f, 0.99479167f, 0.50781250f, 0.00000000f, 0.00000000f, -1.00000000f,
		-0.50000000f, 0.50000000f, -0.50000000f, 0.99479167f, 0.99218750f, 0.00000000f, 0.00000000f, -1.00000000f,
		0.50000000f, 0.50000000f, -0.50000000f, 0.67187500f, 0.99218750f, 0.00000000f, 0.00000000f, -1.00000000f,

		// left
		//x		y		z		S		T
		-0.50000000f, -0.50000000f, -0.50000000f, 0.00520833f, 0.00781250f, -1.00000000f, 0.00000000f, 0.00000000f,
		-0.50000000f, -0.50000000f, 0.50000000f, 0.32812500f, 0.00781250f, -1.00000000f, 0.00000000f, 0.00000000f,
		-0.50000000f, 0.50000000f, 0.50000000f, 0.32812500f, 0.49218750f, -1.00000000f, 0.00000000f, 0.00000000f,
		-0.50000000f, 0.50000000f, -0.50000000f, 0.00520833f, 0.49218750f, -1.00000000f, 0.00000000f, 0.00000000f,

		// bottom
		//x		y		z		S		T
		-0.50000000f, -0.50000000f, -0.50000000f, 0.33854167f, 0.00781250f, 0.00000000f, -1.00000000f, 0.00000000f,
		0.50000000f, -0.50000000f, -0.50000000f, 0.66145833f, 0.00781250f, 0.00000000f, -1.00000000f, 0.00000000f,
		 0.50000000f, -0.50000000f, 0.50000000f, 0.66145833f, 0.49218750f, 0.00000000f, -1.00000000f, 0.00000000f,
		-0.50000000f, -0.50000000f, 0.50000000f, 0.33854167f, 0.49218750f, 0.00000000f, -1.00000000f, 0.00000000f,

		//UP
		 //x		y		z		S		T
		 -0.50000000f, 0.50000000f, 0.50000000f, 0.67187500f, 0.00781250f, 0.00000000f, 1.00000000f, 0.00000000f,
		 0.50000000f, 0.50000000f, 0.50000000f, 0.99479167f, 0.00781250f, 0.00000000f, 1.00000000f, 0.00000000f,
		  0.50000000f, 0.50000000f, -0.50000000f, 0.99479167f, 0.49218750f, 0.00000000f, 1.00000000f, 0.00000000f,
		 -0.50000000f, 0.50000000f, -0.50000000f, 0.67187500f, 0.49218750f, 0.00000000f, 1.00000000f, 0.00000000f,

	};

	MeshModel* dado = new MeshModel();
	dado->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(dado);

}


void CrearHolocron() 	// reemplazar con su propio holocron texturizado
{
	unsigned int holocron_indices[] = {

	12, 1, 0,
	 12, 4, 15,
	 14, 9, 2,
	 2, 12, 15,
	 8,  3,  4,
	 15,  4,  3,
	 3, 6, 14,
	 4, 5,  8,
	 6, 7, 13,
	 9, 10, 11,
	 11, 2, 9,
	 10, 7, 5,
	 13, 14, 6,
	 2, 3, 14,
	 2, 15, 3,
	 11, 12, 2,
	 13, 9, 14,
	 8, 6, 3,
	 12, 0, 4,
	 12, 11, 1,
	 4, 0, 5,
	 6, 8, 7,
	 9, 13, 10,
	 5, 0, 1,
	 1, 11, 10,
	 10, 13, 7,
	 7, 8, 5,
	 5, 1, 10

	};

	GLfloat holocron_vertices[] = {
		// front
		//x					y			z			S		T		NX		NY		NZ
		 -2.517274,		-2.579095,		0.002115,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//0
		-1.185055,		-2.530499,		-1.198261,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//1
		0.049649,		2.440128,		-2.417217,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//2
		0.051152,		2.399781,		2.545043,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//3
		-2.646786,		-0.139047,		2.493757,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//4
		-1.328995,		-2.578386,		1.297838,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//5
		2.593003,		-0.085222,		2.505755,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//6
		1.385028,		-2.549476,		1.308677,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//7
		-0.140696,		-2.601934,		2.558561,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//8
		2.534944,		-0.061146,		-2.580280,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//9
		1.344182,		-2.542571,		-1.294387,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//10
		0.081432,		-2.513204,		-2.426005,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//11
		-2.453980,		 -0.063388,		-2.477451,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//12
		2.632201,		-2.553892,		0.007226,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//13
		2.521206,		2.448241,		0.063388,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//14
		-2.451372,		2.421759,		0.077538,	0.0f, 0.0f,		0.0f,	0.0f,	0.0f,	//15
	};

	// Un logo distinto en cada cara principal.
	GLfloat coordenadasUV[] = {
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.67708333f, 0.75134761f, 0.98958333f, 0.74425582f, 0.83744874f, 0.98437500f,
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.15988804f, 0.51562500f, 0.17132777f, 0.98437500f, 0.01041667f, 0.74645505f,
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.49757136f, 0.51562500f, 0.49957291f, 0.98437500f, 0.34375000f, 0.74765471f,
		0.75524344f, 0.37793644f, 0.75276642f, 0.13319918f, 0.91747294f, 0.13419260f,
		0.16388670f, 0.01562500f, 0.16051256f, 0.48437500f, 0.01041667f, 0.24698925f,
		0.50092752f, 0.48437500f, 0.50100843f, 0.01562500f, 0.65625000f, 0.25006118f,
		0.50092752f, 0.48437500f, 0.34375000f, 0.24872468f, 0.50100843f, 0.01562500f,
		0.49757136f, 0.51562500f, 0.65625000f, 0.74744224f, 0.49957291f, 0.98437500f,
		0.16388670f, 0.01562500f, 0.32291667f, 0.24924688f, 0.16051256f, 0.48437500f,
		0.15988804f, 0.51562500f, 0.32291667f, 0.75149961f, 0.17132777f, 0.98437500f,
		0.67708333f, 0.75134761f, 0.83287417f, 0.51562500f, 0.98958333f, 0.74425582f,
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f, 0.00520833f, 0.99218750f,
		0.91747294f, 0.13419260f, 0.98958333f, 0.25601594f, 0.90872617f, 0.36893122f,
		0.90872617f, 0.36893122f, 0.83186565f, 0.48437500f, 0.75524344f, 0.37793644f,
		0.75524344f, 0.37793644f, 0.67708333f, 0.25554151f, 0.75276642f, 0.13319918f,
		0.75276642f, 0.13319918f, 0.84536732f, 0.01562500f, 0.91747294f, 0.13419260f,
		0.91747294f, 0.13419260f, 0.90872617f, 0.36893122f, 0.75524344f, 0.37793644f,
	};

	std::vector<GLfloat> verticesTexturizados;
	std::vector<unsigned int> indicesTexturizados;
	// Se separan los vertices para ajustar las UV.
	for (unsigned int i = 0; i < 84; i += 3)
	{
		glm::vec3 puntos[3];
		for (unsigned int j = 0; j < 3; ++j)
		{
			unsigned int indice = holocron_indices[i + j] * 8;
			puntos[j] = glm::vec3(holocron_vertices[indice], holocron_vertices[indice + 1], holocron_vertices[indice + 2]);
		}
		glm::vec3 normal = glm::normalize(glm::cross(puntos[1] - puntos[0], puntos[2] - puntos[0]));
        // Los triangulos sin logo son los biseles de las esquinas del holocron.
        bool esquinaGris = coordenadasUV[i * 2] == coordenadasUV[(i + 1) * 2]
            && coordenadasUV[i * 2 + 1] == coordenadasUV[(i + 1) * 2 + 1]
            && coordenadasUV[i * 2] == coordenadasUV[(i + 2) * 2]
            && coordenadasUV[i * 2 + 1] == coordenadasUV[(i + 2) * 2 + 1];
		for (unsigned int j = 0; j < 3; ++j)
		{
			verticesTexturizados.insert(verticesTexturizados.end(), {
				puntos[j].x, puntos[j].y, puntos[j].z,
				esquinaGris ? -1.0f : coordenadasUV[(i + j) * 2], esquinaGris ? -1.0f : coordenadasUV[(i + j) * 2 + 1],
				normal.x, normal.y, normal.z });
			indicesTexturizados.push_back(i + j);
		}
	}

	MeshModel* holocron = new MeshModel();
	holocron->CreateMeshModel(verticesTexturizados.data(), indicesTexturizados.data(), static_cast<unsigned int>(verticesTexturizados.size()), static_cast<unsigned int>(indicesTexturizados.size()));
	meshListModel.push_back(holocron);

}




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
void ActualizarRover(bool* keys, GLfloat tiempoCuadro)
{
    GLfloat sentido = (keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) ? -1.0f : 1.0f;
    GLfloat paso = 45.0f * tiempoCuadro;
    const int teclasPatas[] = { GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L };
    for (int i = 0; i < 6; ++i)
        if (keys[teclasPatas[i]]) angulosPatas[i] = glm::clamp(angulosPatas[i] + sentido * paso, -45.0f, 45.0f);
    if (keys[GLFW_KEY_R]) giroRover += sentido * 60.0f * tiempoCuadro;
    if (keys[GLFW_KEY_P] && !teclaPAnterior) animarRover = !animarRover;
    teclaPAnterior = keys[GLFW_KEY_P];
    if (keys[GLFW_KEY_O]) for (int i = 0; i < 6; ++i) angulosPatas[i] = 0.0f;
    if (animarRover) tiempoAnim += tiempoCuadro;
	if (keys[GLFW_KEY_Q]) giroBaseBrazo += sentido * paso;
	if (keys[GLFW_KEY_Y]) giroHombro += sentido * paso;
	if (keys[GLFW_KEY_U]) giroCodo += sentido * paso;
	if (keys[GLFW_KEY_I]) giroPinza += sentido * paso;
	giroBaseBrazo = glm::clamp(giroBaseBrazo, -180.0f, 180.0f);
	giroHombro = glm::clamp(giroHombro, -45.0f, 45.0f);
	giroCodo = glm::clamp(giroCodo, -70.0f, 70.0f);
	giroPinza = glm::clamp(giroPinza, -60.0f, 60.0f);
	if (keys[GLFW_KEY_9]) giroBaseBrazo = giroHombro = giroCodo = giroPinza = 0.0f;
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
	CrearHolocron();
	CreateShaders();

	camera = Camera(glm::vec3(14.0f, 10.0f, 18.0f), glm::vec3(0.0f, 1.0f, 0.0f), -127.87f, -17.07f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
    dadoTexture = Texture("Textures/star_wars_optimizada.png");
    dadoTexture.LoadTextureA();
    holocronTexture = Texture("Textures/holocron_star_wars.png");
    holocronTexture.LoadTextureA();

	
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
	
	//Añadir modelo del avión
	Avion_M.LoadModel("Models/avion_spartan117.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.04f, 0.01f,
		20.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;
	
	// Faros hijos del Rover y del avion. La linterna y la luz fija se conservan.
    spotLights[2] = SpotLight(1.0f, 0.95f, 0.8f, 0.0f, 3.0f,
        0, 0, 0, 1, -0.15f, 0, 1.0f, 0.05f, 0.01f, 25.0f);
    spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f, 0.0f, 4.0f,
        0, 0, 0, 0, -1, 0, 1.0f, 0.04f, 0.008f, 25.0f);
    spotLights[4] = SpotLight(1.0f, 0.95f, 0.8f, 0.0f, 4.0f,
        0, 0, 0, 0, 0, -1, 1.0f, 0.04f, 0.008f, 20.0f);
    spotLightCount += 3; 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = static_cast<GLfloat>(glfwGetTime());
        GLfloat segundos = glm::clamp(now - lastTime, 0.0f, 0.1f);

		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
        ActualizarRover(mainWindow.getsKeys(), segundos);
        ActualizarAvance(mainWindow.getsKeys(), segundos);
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		// La linterna nace en la camara y apunta al centro de la vista.
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
        const glm::mat4 padreRover = MatrizRover();
        const glm::mat4 padreAvion = MatrizAvion();
        ColocarFaro(spotLights[2], padreRover, glm::vec3(5.2f, 0.0f, 0.0f), glm::vec3(1, -0.15f, 0));
        ColocarFaro(spotLights[3], padreAvion, glm::vec3(0, -1.3f, 0), glm::vec3(0, -1, 0));
        ColocarFaro(spotLights[4], padreAvion, glm::vec3(0, 0, -2.8f), glm::vec3(0, 0, -1));


		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[2]->RenderMeshModel();
	
        plainTexture.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		GLfloat pataDD = angulosPatas[0], pataDI = angulosPatas[1];
		GLfloat pataMD = angulosPatas[2], pataMI = angulosPatas[3];
		GLfloat pataTD = angulosPatas[4], pataTI = angulosPatas[5];
		if (animarRover)
		{
			// El tiempo de animacion se actualiza en ActualizarRover.
			//Marcha en tripode: DI, MD y TI van juntas; DD, MI y TD en sentido contrario
			GLfloat onda = 45.0f * sin(tiempoAnim * 3.0f);
			pataDI = pataMD = pataTI = onda;
			pataDD = pataMI = pataTD = -onda;
		}
		tiempoPrevio = now;
		GLfloat giroRueda = -tiempoAnim * 180.0f; //las ruedas giran mientras avanza

		//Cuerpo (raiz de la jerarquia)
		model = padreRover;
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

        glUniform3fv(uniformColor, 1, glm::value_ptr(color));



		//Dado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 6.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[4]->RenderMeshModel();

		//Holocron
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-7.0f, 7.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocronTexture.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		shaderList[0].SetGreyHolocronCorners(true);
        meshListModel[5]->RenderMeshModel();
        shaderList[0].SetGreyHolocronCorners(false);


		// Avion: los dos faros heredan esta misma raiz.
        model = padreAvion;
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        Avion_M.RenderModel();
        


		//Agave ¿qué sucede si lo renderizan antes del helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[3]->RenderMeshModel();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
