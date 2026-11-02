/*
Animación:
Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
Compleja: Por medio de funciones y algoritmos.
Textura Animada
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
#include "Cohete.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación


float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;



Window mainWindow;
std::vector<MeshModel*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;
Texture holocronTexture;
MeshModel* holocronMesh = nullptr;
MeshModel esquinasHolocron[8];
glm::vec3 centrosEsquinas[8];
glm::vec3 direccionesEsquinas[8];
float tiempoHolocron = 0.0f;



Model PiezasRover[17];
Model Avion_M;



Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;


// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";



//cálculo del promedio de las normales para sombreado de Phong
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
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
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
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	MeshModel* obj5 = new MeshModel();
	obj5->CreateMeshModel(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	MeshModel* obj6 = new MeshModel();
	obj6->CreateMeshModel(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	MeshModel* obj7 = new MeshModel();
	obj7->CreateMeshModel(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

    // Una bolita sencilla de ocho caras para el humo.
    GLfloat humoVertices[] = {
         1,0,0, 0,0,  1,0,0,  -1,0,0, 0,0, -1,0,0,
         0,1,0, 0,0,  0,1,0,   0,-1,0,0,0,  0,-1,0,
         0,0,1, 0,0,  0,0,1,   0,0,-1,0,0,  0,0,-1
    };
    unsigned int humoIndices[] = {
        2,4,0, 2,1,4, 2,5,1, 2,0,5,
        3,0,4, 3,4,1, 3,1,5, 3,5,0
    };
    MeshModel* humo = new MeshModel();
    humo->CreateMeshModel(humoVertices, humoIndices, 48, 24);
    meshList.push_back(humo);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




// Actividades 1 y 2. Las velocidades se expresan por segundo.
const float mitadPiso = 300.0f;
const float escalaRover = 0.3f;

const glm::vec3 centroCuerpo(-1.366451f, 3.685409f, 0.0f);
// Extremos del Rover ensamblado, incluyendo el radio de sus llantas.
const float extremoIzquierdo = -mitadPiso + 5.023f * escalaRover;
const float extremoDerecho = mitadPiso - 8.320f * escalaRover;
const float alturaCuerpo = -2.0f + (3.685409f - 0.145f) * escalaRover;

// El Rover conserva su animacion basica.
// Se actualiza 60 veces por segundo para conservar la velocidad a distintos FPS.

struct EstadoAnimacion {
    float x = 0.0f;
    bool avanza = true;
    float giroLlantas = 0.0f; // Grados; se incrementa sin calcular distancia / radio.
    float anguloAvion = 0.0f;
    float tiempoPendiente = 0.0f;
};
EstadoAnimacion animacion;
void ActualizarAnimacion(EstadoAnimacion& estado, float segundos)
{
    estado.anguloAvion += 45.0f * segundos;
    if (estado.anguloAvion >= 360.0f) estado.anguloAvion -= 360.0f;
    const float pasoTiempo = 1.0f / 60.0f;
    estado.tiempoPendiente += segundos;
    while (estado.tiempoPendiente + 0.000001f >= pasoTiempo) {
        estado.tiempoPendiente -= pasoTiempo;
        // Actividad 1: traslacion del cuerpo y giro de las seis llantas.
        if (estado.avanza) {
            estado.x += 0.12f;
            estado.giroLlantas -= 11.0f;
            if (estado.x >= extremoDerecho) {
                estado.x = extremoDerecho;
                estado.avanza = false;
            }
        } else {
            estado.x -= 0.12f;
            estado.giroLlantas += 11.0f;
            if (estado.x <= extremoIzquierdo) {
                estado.x = extremoIzquierdo;
                estado.avanza = true;
            }
        }
        if (estado.giroLlantas >= 360.0f) estado.giroLlantas -= 360.0f;
        if (estado.giroLlantas <= -360.0f) estado.giroLlantas += 360.0f;

    }
}

// El avion hace un circulo vertical usando seno y coseno.

glm::mat4 MatrizAvion(const EstadoAnimacion& estado)
{
    float angulo = estado.anguloAvion * toRadians;
    float y = 18.0f + 10.0f * (1.0f - cos(angulo));
    float z = -10.0f * sin(angulo);

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(12.0f, y, z));
    model = glm::rotate(model, angulo, glm::vec3(1, 0, 0));
    model = glm::scale(model, glm::vec3(0.7f));
    return model;
}
// Cada particula conserva el lugar donde aparecio y dura dos segundos.
struct ParticulaHumo {
    glm::vec3 posicion;
    double inicio;
};
std::vector<ParticulaHumo> humo;
double ultimoHumo = 0.0;

void ActualizarHumo(const glm::mat4& avion, double tiempo)
{
    for (int i = static_cast<int>(humo.size()) - 1; i >= 0; --i) {
        if (tiempo - humo[i].inicio >= 2.0)
            humo.erase(humo.begin() + i);
    }
    // Mantener H detiene la emision, pero el humo anterior sigue desapareciendo.
    if (mainWindow.getHumoEncendido() && tiempo - ultimoHumo >= 0.15) {
        // Dos salidas a los lados del motor delantero, detras de la helice.
        humo.push_back({glm::vec3(avion * glm::vec4(-0.8f, 0.3f, -1.8f, 1)), tiempo});
        humo.push_back({glm::vec3(avion * glm::vec4( 0.8f, 0.3f, -1.8f, 1)), tiempo});
        ultimoHumo = tiempo;
    }
}

void DibujarHumo(GLuint uniformModel, GLuint uniformColor)
{
    plainTexture.UseTexture();
    glUniform3f(uniformColor, 0.8f, 0.8f, 0.8f);
    for (const ParticulaHumo& particula : humo) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), particula.posicion);
        float vida = glm::max(0.0f, 1.0f - static_cast<float>((glfwGetTime() - particula.inicio) / 2.0));
        model = glm::scale(model, glm::vec3(0.45f * vida));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[7]->RenderMeshModel();
    }
    glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
}
const char* archivosRover[] = {
    "Cuerpo", "Brazo_Base", "Brazo_Superior", "Brazo_Antebrazo", "Brazo_Pinza",
    "Pata_DelanteraDer", "Pata_DelanteraIzq", "Pata_MediaDer", "Pata_MediaIzq",
    "Pata_TraseraDer", "Pata_TraseraIzq", "Rueda_DelanteraDer", "Rueda_DelanteraIzq",
    "Rueda_MediaDer", "Rueda_MediaIzq", "Rueda_TraseraDer", "Rueda_TraseraIzq"
};
// Pivotes de las piezas locales; el brazo conserva su jerarquia ensamblada.
const glm::vec3 pivotesRover[] = {
    centroCuerpo,
    {1.700f,5.180f,-1.100f}, {1.701f,5.982f,-1.100f},
    {4.236f,8.538f,-1.100f}, {1.588f,11.487f,-0.913f},
    {2.100f,4.208f,3.054f}, {2.100f,4.208f,-3.066f},
    {-3.040f,3.290f,3.000f}, {-3.040f,3.290f,-3.000f},
    {-3.040f,3.290f,3.000f}, {-3.040f,3.290f,-3.000f},
    {5.712f,1.705f,3.070f}, {5.710f,1.705f,-3.070f},
    {-0.945f,1.412f,4.159f}, {-0.947f,1.412f,-4.159f},
    {-5.131f,1.412f,4.159f}, {-5.113f,1.386f,-4.159f}
};

void DibujarRover(GLuint uniformModel, GLuint uniformColor)
{
    glm::mat4 raiz = glm::translate(glm::mat4(1.0f),
        glm::vec3(animacion.x, alturaCuerpo, 6.0f));
    raiz = glm::scale(raiz, glm::vec3(escalaRover));
    plainTexture.UseTexture();
    for (int i = 0; i < 17; ++i) {
        glm::mat4 pieza = glm::translate(raiz, pivotesRover[i] - centroCuerpo);
        if (i >= 11)
            pieza = glm::rotate(pieza, animacion.giroLlantas * toRadians, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(pieza));
        // Los colores de los materiales permiten distinguir el giro de las llantas.
        PiezasRover[i].RenderModel(uniformColor);
    }
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

	// Region superior derecha del atlas: Republica Galactica.
	const GLfloat uvEsquinas[] = {
		0.67708333f, 0.51562500f,
		0.98958333f, 0.51562500f,
		0.83333333f, 0.98437500f
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
        // Los biseles usan el emblema de la Republica Galactica sobre fondo negro.
        bool esquinaTexturizada = coordenadasUV[i * 2] == coordenadasUV[(i + 1) * 2]
            && coordenadasUV[i * 2 + 1] == coordenadasUV[(i + 1) * 2 + 1]
            && coordenadasUV[i * 2] == coordenadasUV[(i + 2) * 2]
            && coordenadasUV[i * 2 + 1] == coordenadasUV[(i + 2) * 2 + 1];
		for (unsigned int j = 0; j < 3; ++j)
		{
			verticesTexturizados.insert(verticesTexturizados.end(), {
				puntos[j].x, puntos[j].y, puntos[j].z,
				esquinaTexturizada ? uvEsquinas[j * 2] : coordenadasUV[(i + j) * 2], esquinaTexturizada ? uvEsquinas[j * 2 + 1] : coordenadasUV[(i + j) * 2 + 1],
				normal.x, normal.y, normal.z });
			indicesTexturizados.push_back(i + j);
		}
	}

	MeshModel* holocron = new MeshModel();
	holocron->CreateMeshModel(verticesTexturizados.data(), indicesTexturizados.data(), static_cast<unsigned int>(verticesTexturizados.size()), static_cast<unsigned int>(indicesTexturizados.size()));
	holocronMesh = holocron;

    // Cada fila contiene una piramide: x, y, z, u, v, nx, ny, nz.
    // Las primeras cuatro tienen 4 vertices; las inferiores tienen 5.
    GLfloat verticesEsquinas[8][40] = {
        { // Esquina 1
            0.049649f, 2.440128f, -2.417217f, 0.677083f, 0.515625f, 0.0f, 0.0f, 0.0f,
            -2.453980f, -0.063388f, -2.477451f, 0.989583f, 0.515625f, 0.0f, 0.0f, 0.0f,
            -2.451372f, 2.421759f, 0.077538f, 0.989583f, 0.984375f, 0.0f, 0.0f, 0.0f,
            -2.550000f, 2.550000f, -2.550000f, 0.677083f, 0.984375f, 0.0f, 0.0f, 0.0f,
        },
        { // Esquina 2
            -2.451372f, 2.421759f, 0.077538f, 0.677083f, 0.515625f, 0.0f, 0.0f, 0.0f,
            -2.646786f, -0.139047f, 2.493757f, 0.989583f, 0.515625f, 0.0f, 0.0f, 0.0f,
            0.051152f, 2.399781f, 2.545043f, 0.989583f, 0.984375f, 0.0f, 0.0f, 0.0f,
            -2.550000f, 2.550000f, 2.550000f, 0.677083f, 0.984375f, 0.0f, 0.0f, 0.0f,
        },
        { // Esquina 3
            0.051152f, 2.399781f, 2.545043f, 0.677083f, 0.515625f, 0.0f, 0.0f, 0.0f,
            2.593003f, -0.085222f, 2.505755f, 0.989583f, 0.515625f, 0.0f, 0.0f, 0.0f,
            2.521206f, 2.448241f, 0.063388f, 0.989583f, 0.984375f, 0.0f, 0.0f, 0.0f,
            2.550000f, 2.550000f, 2.550000f, 0.677083f, 0.984375f, 0.0f, 0.0f, 0.0f,
        },
        { // Esquina 4
            2.521206f, 2.448241f, 0.063388f, 0.677083f, 0.515625f, 0.0f, 0.0f, 0.0f,
            2.534944f, -0.061146f, -2.580280f, 0.989583f, 0.515625f, 0.0f, 0.0f, 0.0f,
            0.049649f, 2.440128f, -2.417217f, 0.989583f, 0.984375f, 0.0f, 0.0f, 0.0f,
            2.550000f, 2.550000f, -2.550000f, 0.677083f, 0.984375f, 0.0f, 0.0f, 0.0f,
        },
        { // Esquina 5
            -2.453980f, -0.063388f, -2.477451f, 0.677083f, 0.515625f, 0.0f, 0.0f, 0.0f,
            0.081432f, -2.513204f, -2.426005f, 0.989583f, 0.515625f, 0.0f, 0.0f, 0.0f,
            -1.185055f, -2.530499f, -1.198261f, 0.989583f, 0.984375f, 0.0f, 0.0f, 0.0f,
            -2.517274f, -2.579095f, 0.002115f, 0.677083f, 0.984375f, 0.0f, 0.0f, 0.0f,
            -2.550000f, -2.550000f, -2.550000f, 0.833333f, 0.750000f, 0.0f, 0.0f, 0.0f,
        },
        { // Esquina 6
            -2.646786f, -0.139047f, 2.493757f, 0.677083f, 0.515625f, 0.0f, 0.0f, 0.0f,
            -2.517274f, -2.579095f, 0.002115f, 0.989583f, 0.515625f, 0.0f, 0.0f, 0.0f,
            -1.328995f, -2.578386f, 1.297838f, 0.989583f, 0.984375f, 0.0f, 0.0f, 0.0f,
            -0.140696f, -2.601934f, 2.558561f, 0.677083f, 0.984375f, 0.0f, 0.0f, 0.0f,
            -2.550000f, -2.550000f, 2.550000f, 0.833333f, 0.750000f, 0.0f, 0.0f, 0.0f,
        },
        { // Esquina 7
            2.593003f, -0.085222f, 2.505755f, 0.677083f, 0.515625f, 0.0f, 0.0f, 0.0f,
            -0.140696f, -2.601934f, 2.558561f, 0.989583f, 0.515625f, 0.0f, 0.0f, 0.0f,
            1.385028f, -2.549476f, 1.308677f, 0.989583f, 0.984375f, 0.0f, 0.0f, 0.0f,
            2.632201f, -2.553892f, 0.007226f, 0.677083f, 0.984375f, 0.0f, 0.0f, 0.0f,
            2.550000f, -2.550000f, 2.550000f, 0.833333f, 0.750000f, 0.0f, 0.0f, 0.0f,
        },
        { // Esquina 8
            2.534944f, -0.061146f, -2.580280f, 0.677083f, 0.515625f, 0.0f, 0.0f, 0.0f,
            2.632201f, -2.553892f, 0.007226f, 0.989583f, 0.515625f, 0.0f, 0.0f, 0.0f,
            1.344182f, -2.542571f, -1.294387f, 0.989583f, 0.984375f, 0.0f, 0.0f, 0.0f,
            0.081432f, -2.513204f, -2.426005f, 0.677083f, 0.984375f, 0.0f, 0.0f, 0.0f,
            2.550000f, -2.550000f, -2.550000f, 0.833333f, 0.750000f, 0.0f, 0.0f, 0.0f,
        },
    };

    // Cada grupo de tres indices forma un triangulo.
    unsigned int indicesEsquinas[8][18] = {
        {0, 1, 3, 1, 2, 3, 2, 0, 3, 0, 2, 1},
        {0, 1, 3, 1, 2, 3, 2, 0, 3, 0, 2, 1},
        {0, 1, 3, 1, 2, 3, 2, 0, 3, 0, 2, 1},
        {0, 1, 3, 1, 2, 3, 2, 0, 3, 0, 2, 1},
        {0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4, 0, 2, 1, 0, 3, 2},
        {0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4, 0, 2, 1, 0, 3, 2},
        {0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4, 0, 2, 1, 0, 3, 2},
        {0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4, 0, 2, 1, 0, 3, 2},
    };

    // Cada vertice tiene 8 datos: posicion, textura y normal.
    unsigned int cantidadDatos[8] = {32, 32, 32, 32, 40, 40, 40, 40};
    unsigned int cantidadIndices[8] = {12, 12, 12, 12, 18, 18, 18, 18};

    // Centros de giro y direcciones diagonales de las ocho esquinas.
    glm::vec3 centros[8] = {
        {-1.851426f, 1.837125f, -1.841782f},
        {-1.899252f, 1.808123f, 1.916584f},
        {1.928840f, 1.828200f, 1.916046f},
        {1.913950f, 1.844306f, -1.871027f},
        {-1.724975f, -2.047237f, -1.729920f},
        {-1.836750f, -2.089692f, 1.780454f},
        {1.803907f, -2.068105f, 1.786044f},
        {1.828552f, -2.044163f, -1.768689f},
    };
    glm::vec3 direcciones[8] = {
        {-1.0f, 1.0f, -1.0f},
        {-1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, -1.0f},
    };

    for (int i = 0; i < 8; i++) {
        centrosEsquinas[i] = centros[i];
        direccionesEsquinas[i] = glm::normalize(direcciones[i]);
        // Usar la misma funcion de normales de las figuras anteriores.
        calcAverageNormals(indicesEsquinas[i], cantidadIndices[i],
            verticesEsquinas[i], cantidadDatos[i], 8, 5);
        esquinasHolocron[i].CreateMeshModel(verticesEsquinas[i], indicesEsquinas[i],
            cantidadDatos[i], cantidadIndices[i]);
    }
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	if (mainWindow.Initialise() != 0) return 1;
    glfwSetWindowTitle(glfwGetCurrentContext(), "P09-2-317136421 | Animacion compleja | Holocron y Saturn V");

	CreateObjects();
    CrearHolocron();
	CreateShaders();

	camera = Camera(glm::vec3(16.0f, 12.0f, 36.0f), glm::vec3(0, 1, 0), -125.0f, -12.0f, 18.0f, 0.15f);

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
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();
    holocronTexture = Texture("Textures/holocron_star_wars.png");
    holocronTexture.LoadTextureA();

    for (int i = 0; i < 17; ++i)
        PiezasRover[i].LoadModel(std::string("Models/Rover/") + archivosRover[i] + ".obj");
    Avion_M.LoadModel("Models/avion_spartan117.obj");
    CargarCohete();
	

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
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f * toRadians, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	




	glm::vec3 lowerLight(0.0f,0.0f,0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	lastTime = static_cast<GLfloat>(glfwGetTime());
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
        GLfloat now = static_cast<GLfloat>(glfwGetTime());
        deltaTime = glm::clamp(now - lastTime, 0.0f, 0.1f);
        lastTime = now;


        glfwPollEvents();
        ActualizarAnimacion(animacion, deltaTime);
        tiempoHolocron += deltaTime;
        ActualizarCohete(deltaTime);
        ActualizarHumo(MatrizAvion(animacion), glfwGetTime());
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
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Reinicializando variables cada ciclo de reloj
		model= glm::mat4(1.0);
		modelaux= glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(mitadPiso / 10.0f, 1.0f, mitadPiso / 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMeshModel();



        // Modelos recuperados de la practica 8, listos para la actividad del reporte.
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 14.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
        holocronTexture.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        holocronMesh->RenderMeshModel();
        // El coseno produce el ciclo cerrado -> abierto -> cerrado.
        // apertura vale 0 al cerrar y 1 al abrir; el ciclo dura 6 segundos.
        float apertura = (1.0f - cos(tiempoHolocron * 2.0f * 3.14159265f / 6.0f)) / 2.0f;
        glm::mat4 cuerpoHolocron = model;
        for (int i = 0; i < 8; i++) {
            model = glm::translate(cuerpoHolocron, direccionesEsquinas[i] * (2.0f * apertura));
            // Girar cada piramide alrededor de su propio centro.
            model = glm::translate(model, centrosEsquinas[i]);
            model = glm::rotate(model, 120.0f * apertura * toRadians, direccionesEsquinas[i]);
            model = glm::translate(model, -centrosEsquinas[i]);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            esquinasHolocron[i].RenderMeshModel();
        }

        DibujarCohete(uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);

        // El cuerpo se traslada y todas las piezas heredan su matriz.
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        DibujarRover(uniformModel, uniformColor);

        model = MatrizAvion(animacion);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
        Avion_M.RenderModel();
        DibujarHumo(uniformModel, uniformColor);
        glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);


		//Agave ¿qué sucede si lo renderizan antes del Rover?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMeshModel();
		
		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001;
		toffsetflechav = 0.000;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMeshModel();

		//plano con todos los números
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMeshModel();

		//número 1
		//toffsetnumerou = 0.0;
		//toffsetnumerov = 0.0;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		//glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMeshModel();

		for (int i = 1; i<4; i++)
		{
			//números 2-4
			toffsetnumerou += 0.25;
			toffsetnumerov = 0.0;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-10.0f - (i * 3.0), 2.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMeshModel();

		 }

		for (int j = 1; j < 5; j++)
		{
			//números 5-8
			toffsetnumerou += 0.25;
			toffsetnumerov = -0.33;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-7.0f - (j * 3.0), 5.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMeshModel();
		}
 

		//número cambiante 
		/*
		¿Cómo hacer para que sea a una velocidad visible?
		*/
		toffsetnumerocambiau += 0.25; 
		if (toffsetnumerocambiau > 1.0)
			toffsetnumerocambiau = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMeshModel();

		//cambiar automáticamente entre textura número 1 y número 2
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Numero1Texture.UseTexture();
		//if
		//Numero1Texture.UseTexture();
		//Numero2Texture.UseTexture();
		
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMeshModel();

		glDisable(GL_BLEND);
		
		



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
