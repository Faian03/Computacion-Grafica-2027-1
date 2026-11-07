/*
Proyecto final CGeIHC 2027-1 · Sistema Tres Mundos
Secciones del main (cada quien trabaja en la suya y en su rama):
  1. Geometria hecha en codigo ................ comun
  2. Iluminacion: sol, luminarias, spotlights  comun
  3. Camaras C1 a C6 .......................... comun (C5 y C6 siguen el cohete de Reach)
  4. Escenario comun: planetas, pista, estacion, plataforma, cuerpos celestes, nave viajera
  5. P1 Mundo Forja (Ratchet & Clank) ......... 419049898
  6. P2 Reach (Halo) .......................... 317136421
  7. P3 Venom (Star Fox 64) ................... 318337872
  8. Avatar (alien insecto) ................. comun

Teclas que ya funcionan: 1-6 camaras, WASD + mouse mueven al avatar en la camara 1, C sale del avatar y deja la camara libre
(otra vez C regresa al avatar), T viajar al siguiente planeta,
R cohete (AB1), G torreta (AB2), H Ratchet vuela con el Heli-Pack (KF1), L reflector S1, K luces de la estacion S2,
P despega el Pelican de Reach (AB3), O patrulla del Warthog, J pluma del puesto de control, X acelera el ciclo de dia (solo pruebas),
B cohete por keyframes (KF-P10: despega, se separa en fases y la capsula orbita la estacion),
V guarda esos keyframes en keyframescohete.txt,
coma (,) pilotar el Arwing de Venom con WASD, F cohete de Venom por keyframes (despega de la cima del domo; otra vez F lo regresa), F7 guarda sus keyframes en keyframes_cohete_venom.txt y F8 los lee de nuevo,
espacio inicia o pausa el cohete de Reach (keyframes), retroceso lo reinicia, F5 guarda sus keyframes en keyframes_cohete_reach.txt y F6 los lee de nuevo
(todas las teclas se guardan en Window y el main solo pregunta)
*/
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <stdlib.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;
const float PI = 3.14159265f;

Window mainWindow;
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;

// texturas (terreno y skybox)
Texture plainTexture;
Texture forjaTexture;
Texture reachTexture;
Texture venomTexture;
Texture rocaTexture;
Texture metalTexture;
Texture pistaTexture;
Texture neonTexture;	// franjas de luz de las orillas de la pista
Texture menuTexture[4];	// teclas de cada planeta y del espacio (menu que se abre con M)
Texture pestanaTexture;	// pestaña del menu cerrado
Texture solarTexture;		// paneles solares de la estacion y los satelites
Texture ventanasTexture;	// ventanas prendidas de la estacion
Texture planetaTexture[4];	// cuerpos celestes: gaseoso, hielo, lava, alien
Texture anilloTexture;
//textura cohete de venom
Texture plasmaTexture;	// plasma azul del cohete de Venom



// modelos reunidos de Kenney (Space Kit, licencia CC0), en Models/kenney
Model naveSabre;		// N2: el Sabre de Halo Reach (N1 es el hoverbike de Ratchet & Clank)
Model naveSpeederB;		// N3
Model antenaSatelite;	// satelites del espacio
Model meteoro;			// asteroides
Model chimenea;			// MF-01
Model generador;		// junto a las refinerias de Mundo Forja
// modelos de Ratchet & Clank (The Models Resource, de los juegos), en Models/rc
Model ratchetModelo;	// MF-11 figura y KF1 vuelo con el Heli-Pack
Model vendedorGadgetron;	// MF-06
Model clankModelo;		// Clank en la espalda de Ratchet (MF-11 y KF1)
Model hoverbike;		// N1 de la pista (Going Commando)
Model cajaTornillos, cajaNanotech, cajaMetal;	// MF-05
Model tornilloModelo;	// MF-04
Model tornilloOro;		// tornillo de oro flotando en el centro de Mundo Forja
Model naveRatchet;		// la Aphelion, estacionada junto al planeta
Model alienCuerpo, alienPataIzq, alienPataDer, alienPataFrente, alienBrazoIzq, alienBrazoDer, alienCola;	// avatar: cada pieza esta centrada en su articulacion
Model Etapa1_M, Etapa2_M, Etapa3_M;	// cohete Saturn V en tres etapas; juntas forman el cohete completo (AB1)
Model reachEtapas[4];	// Saturn V de Reach (Practica 10 de 317136421): etapa 1, etapa 2, etapa 3 y capsula, ya centradas (Models/SaturnV)
// modelos de Halo para Reach (The Models Resource, ver Models/halo/FUENTES.txt), en Models/halo
Model arwingModel, wolfenModel;	// ARWING/WOLFEN: naves de Venom (Models/Arwing.obj y Models/WolfenII.obj, texturas en Textures/)
Model lunaMajoras;		// LUNA: luna de Majora's Mask (Models/venom/luna_majoras)
Model cuerpoSuperiorV, cuerpoInferiorV, propulsoresV;
Model catspawModel;

Model pelican;			// HR-06 y AB3 en la zona de aterrizaje, y la nave viajera NV
Model warthog;			// HR-05 chasis (padre de las ruedas y la torreta)
Model ruedasWarthog[4];
Model torretaWarthog;
Model marine, inquisidor, grunt, jefe;	// HR-11 a HR-14
// texturas de Reach (Textures/halo)
Texture concretoReach, cajaReach, letreroReach, pistaReach, escudoReach;
// humo y llama del cohete (AB1)
Texture humoTexture, llamaTexture;

// un skybox por zona: espacio y uno por planeta
Skybox skyEspacio;
Skybox skyForja;
Skybox skyReach;
Skybox skyVenom;

// materiales (tabla 1.7.5 de la propuesta)
Material metalPulido;
Material metalPintado;
Material metalNave;
Material concreto;
Material roca;
Material materialAlien;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

// indices de las mallas en meshListModel
#define CUBO 0
#define TORNILLO 1
#define PISTA 2
#define ESFERA 3
#define HEMISFERIO 4
#define CILINDRO 5
#define CONO 6
#define DISCO 7
#define MURO_PISTA 8
#define CUADRO 9
#define ANILLO 10
#define BARRICADA 11	// HR-02
#define ESCUDO 12		// HR-08 / AC4
#define ATERRIZAJE 13	// HR-09 marca H
#define HUMO 14			// AB1 bocanada de humo del cohete
#define LLAMA 15		// AB1 llama del cohete
#define PLASMA 16		// llama del cohete venom

// la pista: elipse de semiejes 84 y 42 con centro en (0, 16, 31); la altura es 16 + 3.5 sen(2t), asi sube y baja y se ve mas futurista
const float PISTA_A = 84.0f, PISTA_B = 42.0f, PISTA_Z = 31.0f, PISTA_ANCHO = 5.0f;

// centro y radio de cada planeta (si se vuelven a agrandar los mundos, se cambia aqui)
const glm::vec3 C_FORJA = glm::vec3(-126.0f, 0.0f, -50.0f), C_REACH = glm::vec3(126.0f, 0.0f, -50.0f), C_VENOM = glm::vec3(0.0f, 0.0f, 120.0f);
const float R_FORJA = 80.0f, R_REACH = 80.0f, R_VENOM = 76.0f;
const glm::vec3 C_ESTACION = glm::vec3(0.0f, 84.0f, -81.0f);

// LUNA: luna de Majora's Mask (easter egg de Venom)
const glm::vec3 LUNA_POS = C_VENOM + glm::vec3(0.0f, 100.0f, 110.0f);	// quieta, detras del planeta Venom
const float LUNA_RADIO = 30.0f;			// el modelo mide ~2100 de radio, se escala a este valor
// ARWING/WOLFEN: naves que vuelan sobre Venom (los dos modelos tienen la nariz en +z)
const float ARWING_ESCALA = 0.0028f;		// el modelo mide ~2200 de ala (unidades de N64), queda de ~6
const float WOLFEN_ESCALA = 0.9f;
const float ARWING_VEL = 22.0f;				// velocidad de crucero (unidades por segundo)
const float ARWING_GIRO = 1.6f;				// rapidez de giro con A y D (rad/s)
const float ARWING_R_PATRULLA = 60.0f, ARWING_ALT_PATRULLA = 25.0f;	// circulo que recorre solo cuando no lo controlas
const float ARWING_R_MAX = 150.0f;			// limite horizontal desde el centro de Venom (la luna queda dentro)
const float ARWING_ALT_MIN = 3.0f, ARWING_ALT_MAX = 135.0f;
// VENOM 2 (Star Fox 64): domo central, pilares con la cara de Andross y mini estaciones de donde salen naves
const float DOMO_R = 14.0f, DOMO_ALTO = 10.0f, DOMO_BASE = 2.2f;	// radio, alto del cascaron y altura a la que empieza
const int N_PILARES = 8;
const float R_PILARES = 38.0f;
const int N_ESTACIONES = 3;
const float R_ESTACION = 58.0f, ALT_ESTACION = 40.0f;
const float CICLO_ESTACION = 36.0f, T_VUELO = 14.0f;	// cada 36 s una nave sale (14 s de vuelo) y otra regresa






const glm::vec3 PLATAFORMA = C_FORJA + glm::vec3(0.0f, 0.0f, -34.0f);	// plataforma de lanzamiento, en el norte de Mundo Forja

// KF-P10 (practica 10): animacion del cohete por keyframes. Cada cuadro guarda, para las tres
// etapas del cohete, su posicion (desde la plataforma), su giro en Z y cuanto esta encendida su
// llama; ademas guarda hacia donde mira la camara y a que distancia. Los 19 valores de cada fila:
// x1 y1 z1  x2 y2 z2  x3 y3 z3  giro1 giro2 giro3  llama1 llama2 llama3  camX camY camZ camDist
// (etapa 1 abajo, etapa 2 en medio y etapa 3 = capsula de la punta)
// del cuadro 0 al 5 sube vertical, en el 6 se suelta la etapa 1, en el 8 la etapa 2, la capsula
// llega a la orbita en el 11 y del 12 al 24 es una vuelta de orbita
#define KC_NUM 25
#define KC_CAMPOS 19
// el cohete de esta animacion se dibuja mas grande que el de la plataforma para que se vea de lejos
const float kcEscala = 2.6f;
// pasos entre un cuadro y el siguiente (a 60 por segundo son 1.5 s)
const int kc_max_steps = 90;
// numero de cuadros y cuadro donde empieza la vuelta de orbita (al llegar al ultimo se regresa a ese)
int kcN = KC_NUM, kcOrbIni = 12;
float kcDatos[KC_NUM][KC_CAMPOS] = {
	/* 0*/ { 0.00f, 4.16f, 0.00f, 0.00f, 11.96f, 0.00f, 0.00f, 19.24f, 0.00f,
	        0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 12.00f, 0.00f, 62.00f },
	/* 1*/ { 0.00f, 4.16f, 0.00f, 0.00f, 11.96f, 0.00f, 0.00f, 19.24f, 0.00f,
	        0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 12.00f, 0.00f, 62.00f },
	/* 2*/ { 0.00f, 7.16f, 0.00f, 0.00f, 14.96f, 0.00f, 0.00f, 22.24f, 0.00f,
	        0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 16.00f, 0.00f, 62.00f },
	/* 3*/ { 0.00f, 20.16f, 0.00f, 0.00f, 27.96f, 0.00f, 0.00f, 35.24f, 0.00f,
	        0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 30.00f, 0.00f, 66.00f },
	/* 4*/ { 0.00f, 42.16f, 0.00f, 0.00f, 49.96f, 0.00f, 0.00f, 57.24f, 0.00f,
	        0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 50.00f, 0.00f, 70.00f },
	/* 5*/ { 0.00f, 66.16f, 0.00f, 0.00f, 73.96f, 0.00f, 0.00f, 81.24f, 0.00f,
	        0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 72.00f, 0.00f, 78.00f },
	/* 6*/ { 0.00f, 86.00f, 0.00f, 5.08f, 99.78f, 0.00f, 6.34f, 106.95f, 0.00f,
	        -10.00f, -10.00f, -10.00f, 0.00f, 1.00f, 0.00f, 2.00f, 76.00f, 0.00f, 90.00f },
	/* 7*/ { 10.00f, 66.00f, 2.00f, 19.98f, 120.36f, 0.00f, 23.62f, 126.66f, 0.00f,
	        -40.00f, -30.00f, -30.00f, 0.00f, 1.00f, 0.00f, 12.00f, 62.00f, 0.00f, 105.00f },
	/* 8*/ { 18.00f, 32.00f, 5.00f, 33.54f, 130.99f, 0.00f, 42.30f, 138.87f, 0.00f,
	        -70.00f, -48.00f, -48.00f, 0.00f, 0.00f, 0.00f, 22.00f, 80.00f, 0.00f, 135.00f },
	/* 9*/ { 25.00f, 1.81f, 8.00f, 34.00f, 118.00f, 0.00f, 64.00f, 126.00f, 0.00f,
	        -90.00f, -90.00f, -125.00f, 0.00f, 0.00f, 1.00f, 30.00f, 64.00f, 4.00f, 150.00f },
	/*10*/ { 25.00f, 1.81f, 8.00f, 36.00f, 92.00f, 0.50f, 86.00f, 100.00f, 1.00f,
	        -90.00f, -150.00f, -153.00f, 0.00f, 0.00f, 1.00f, 40.00f, 50.00f, 0.00f, 140.00f },
	/*11*/ { 25.00f, 1.81f, 8.00f, 37.00f, 52.00f, 1.00f, 104.79f, 62.79f, 3.00f,
	        -90.00f, -220.00f, -135.00f, 0.00f, 0.00f, 0.00f, 60.00f, 40.00f, 0.00f, 140.00f },
	/*12*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 104.79f, 62.79f, 3.00f,
	        -90.00f, -270.00f, -135.00f, 0.00f, 0.00f, 0.00f, 70.00f, 32.00f, 0.00f, 150.00f },
	/*13*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 118.24f, 55.02f, 3.00f,
	        -90.00f, -270.00f, -105.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f },
	/*14*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 133.76f, 55.02f, 3.00f,
	        -90.00f, -270.00f, -75.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f },
	/*15*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 147.21f, 62.79f, 3.00f,
	        -90.00f, -270.00f, -45.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f },
	/*16*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 154.98f, 76.24f, 3.00f,
	        -90.00f, -270.00f, -15.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f },
	/*17*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 154.98f, 91.76f, 3.00f,
	        -90.00f, -270.00f, 15.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f },
	/*18*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 147.21f, 105.21f, 3.00f,
	        -90.00f, -270.00f, 45.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f },
	/*19*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 133.76f, 112.98f, 3.00f,
	        -90.00f, -270.00f, 75.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f },
	/*20*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 118.24f, 112.98f, 3.00f,
	        -90.00f, -270.00f, 105.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f },
	/*21*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 104.79f, 105.21f, 3.00f,
	        -90.00f, -270.00f, 135.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f },
	/*22*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 97.02f, 91.76f, 3.00f,
	        -90.00f, -270.00f, 165.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f },
	/*23*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 97.02f, 76.24f, 3.00f,
	        -90.00f, -270.00f, 195.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f },
	/*24*/ { 25.00f, 1.81f, 8.00f, 37.00f, 1.81f, 1.00f, 104.79f, 62.79f, 3.00f,
	        -90.00f, -270.00f, 225.00f, 0.00f, 0.00f, 0.00f, 126.00f, 84.00f, 0.00f, 90.00f }
};
const char kcArchivo[] = "keyframescohete.txt";

// imprime en la consola los nombres de las columnas
void kcEncabezado() {
	printf("cu |      x1      y1      z1      x2      y2      z2      x3      y3      z3\n");
	printf("   |   giro1   giro2   giro3  llama1  llama2  llama3    camX    camY    camZ camDist\n");
	printf("---+--------------------------------------------------------------------------------\n");
}
// imprime un cuadro en dos renglones
void kcImprimir(int k) {
	printf("%2d |", k);
	for (int c = 0; c < 9; c++) printf(" %7.2f", kcDatos[k][c]);
	printf("\n   |");
	for (int c = 9; c < KC_CAMPOS; c++) printf(" %7.2f", kcDatos[k][c]);
	printf("\n");
}
// escribe los keyframes en el txt (se sobreescribe) y los imprime
void kcGuardar() {
	FILE* f = fopen(kcArchivo, "w");
	if (f == NULL) { printf("No se pudo escribir %s\n", kcArchivo); return; }
	fprintf(f, "%d %d\n", kcN, kcOrbIni);
	for (int k = 0; k < kcN; k++) {
		for (int c = 0; c < KC_CAMPOS; c++) fprintf(f, "%.2f ", kcDatos[k][c]);
		fprintf(f, "\n");
	}
	fclose(f);
	printf("Se guardaron %d keyframes en %s\n", kcN, kcArchivo);
	kcEncabezado();
	for (int k = 0; k < kcN; k++) kcImprimir(k);
}
// lee los keyframes del txt; si no existe o esta incompleto se quedan los del codigo
bool kcLeer() {
	FILE* f = fopen(kcArchivo, "r");
	if (f == NULL) return false;
	int n = 0, o = 0;
	float tmp[KC_NUM][KC_CAMPOS];
	bool bien = fscanf(f, "%d %d", &n, &o) == 2 && n >= 2 && n <= KC_NUM && o >= 0 && o <= n - 2;
	for (int k = 0; bien && k < n; k++)
		for (int c = 0; bien && c < KC_CAMPOS; c++) bien = fscanf(f, "%f", &tmp[k][c]) == 1;
	fclose(f);
	if (!bien) return false;
	kcN = n; kcOrbIni = o;
	for (int k = 0; k < n; k++) for (int c = 0; c < KC_CAMPOS; c++) kcDatos[k][c] = tmp[k][c];
	return true;
}



// 1. GEOMETRIA HECHA EN CODIGO
// ojo: este shader pide las normales al reves (hacia adentro), por eso todas van con signo menos


// cubo de lado 1 centrado en el origen, cada cara con sus uv completas
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		0, 1, 2,	2, 3, 0,
		4, 5, 6,	6, 7, 4,
		8, 9, 10,	10, 11, 8,
		12, 13, 14,	14, 15, 12,
		16, 17, 18,	18, 19, 16,
		20, 21, 22,	22, 23, 20,
	};
	GLfloat cubo_vertices[] = {
		//x		y		z		S		T		NX		NY		NZ
		// frente (+z)
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		0.0f,	0.0f,	-1.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f,	0.0f,	-1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		0.0f,	0.0f,	-1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		0.0f,	0.0f,	-1.0f,
		// derecha (+x)
		 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		-1.0f,	0.0f,	0.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		-1.0f,	0.0f,	0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		-1.0f,	0.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		-1.0f,	0.0f,	0.0f,
		 // atras (-z)
		  0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f,	0.0f,	1.0f,
		 -0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		0.0f,	0.0f,	1.0f,
		 -0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		0.0f,	0.0f,	1.0f,
		  0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f,	0.0f,	1.0f,
		  // izquierda (-x)
		  -0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		1.0f,	0.0f,	0.0f,
		  -0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		1.0f,	0.0f,	0.0f,
		  -0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		1.0f,	0.0f,	0.0f,
		  -0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		1.0f,	0.0f,	0.0f,
		  // abajo (-y)
		  -0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f,	1.0f,	0.0f,
		   0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		0.0f,	1.0f,	0.0f,
		   0.5f, -0.5f,  0.5f,	1.0f, 1.0f,		0.0f,	1.0f,	0.0f,
		  -0.5f, -0.5f,  0.5f,	0.0f, 1.0f,		0.0f,	1.0f,	0.0f,
		  // arriba (+y)
		  -0.5f,  0.5f,  0.5f,	0.0f, 0.0f,		0.0f,	-1.0f,	0.0f,
		   0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		0.0f,	-1.0f,	0.0f,
		   0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		0.0f,	-1.0f,	0.0f,
		  -0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f,	-1.0f,	0.0f,
	};
	MeshModel* cubo = new MeshModel();
	cubo->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(cubo);
}

// tornillo (bolt): prisma hexagonal de radio 0.5 y alto 0.3, acostado en el plano XY para que se vea de frente
void CrearTornillo()
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	// tapas: centro + 6 vertices en z = +0.15 y z = -0.15
	for (int tapa = 0; tapa < 2; tapa++) {
		float z = tapa == 0 ? 0.15f : -0.15f;
		float nz = tapa == 0 ? -1.0f : 1.0f;
		unsigned int base = v.size() / 8;
		v.insert(v.end(), { 0.0f, 0.0f, z,	0.5f, 0.5f,	0.0f, 0.0f, nz });
		for (int i = 0; i < 6; i++) {
			float a = i * PI / 3.0f;
			v.insert(v.end(), { 0.5f * cos(a), 0.5f * sin(a), z,	0.5f + 0.5f * cos(a), 0.5f + 0.5f * sin(a),	0.0f, 0.0f, nz });
		}
		for (int i = 0; i < 6; i++) {
			ind.push_back(base); ind.push_back(base + 1 + i); ind.push_back(base + 1 + (i + 1) % 6);
		}
	}
	// los 6 lados, cada uno con su propia normal
	for (int i = 0; i < 6; i++) {
		float a1 = i * PI / 3.0f, a2 = (i + 1) * PI / 3.0f, am = (a1 + a2) / 2.0f;
		unsigned int base = v.size() / 8;
		v.insert(v.end(), { 0.5f * cos(a1), 0.5f * sin(a1),  0.15f,	0.0f, 0.0f,	-cos(am), -sin(am), 0.0f });
		v.insert(v.end(), { 0.5f * cos(a2), 0.5f * sin(a2),  0.15f,	1.0f, 0.0f,	-cos(am), -sin(am), 0.0f });
		v.insert(v.end(), { 0.5f * cos(a2), 0.5f * sin(a2), -0.15f,	1.0f, 1.0f,	-cos(am), -sin(am), 0.0f });
		v.insert(v.end(), { 0.5f * cos(a1), 0.5f * sin(a1), -0.15f,	0.0f, 1.0f,	-cos(am), -sin(am), 0.0f });
		ind.insert(ind.end(), { base, base + 1, base + 2, base + 2, base + 3, base });
	}
	MeshModel* tornillo = new MeshModel();
	tornillo->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(tornillo);
}

// pista E2: tira de triangulos entre dos elipses con PISTA_ANCHO de cada lado; la altura sale de 16 + 3.5 sen(2t)
void CrearPista()
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	int n = 320;
	for (int i = 0; i <= n; i++) {
		float t = 2.0f * PI * i / n;
		float vt = i * 0.5f;   // la textura avanza a lo largo de la pista y se repite
		float y = (16.0f + 3.5f * sin(2.0f * t));
		v.insert(v.end(), { (PISTA_A - PISTA_ANCHO) * cos(t), y, (PISTA_B - PISTA_ANCHO) * sin(t),	0.0f, vt,	0.0f, -1.0f, 0.0f });
		v.insert(v.end(), { (PISTA_A + PISTA_ANCHO) * cos(t), y, (PISTA_B + PISTA_ANCHO) * sin(t),	1.0f, vt,	0.0f, -1.0f, 0.0f });
	}
	for (int i = 0; i < n; i++) {
		unsigned int a = 2 * i;
		ind.insert(ind.end(), { a, a + 1, a + 3, a + 3, a + 2, a });
	}
	MeshModel* pista = new MeshModel();
	pista->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(pista);
}

// muros bajos de las dos orillas de la pista (la franja de luz va en la textura neon)
void CrearMuroPista()
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	int n = 320;
	for (int lado = 0; lado < 2; lado++) {
		float d = lado == 0 ? -PISTA_ANCHO : PISTA_ANCHO;
		unsigned int base = v.size() / 8;
		for (int i = 0; i <= n; i++) {
			float t = 2.0f * PI * i / n;
			float y = (16.0f + 3.5f * sin(2.0f * t));
			float nx = (lado == 0 ? 1.0f : -1.0f) * cos(t), nz = (lado == 0 ? 1.0f : -1.0f) * sin(t);	// normal al reves, como pide el shader
			v.insert(v.end(), { (PISTA_A + d) * cos(t), y - 0.3f, (PISTA_B + d) * sin(t),	i * 0.25f, 0.0f,	nx, 0.0f, nz });
			v.insert(v.end(), { (PISTA_A + d) * cos(t), y + 0.9f, (PISTA_B + d) * sin(t),	i * 0.25f, 1.0f,	nx, 0.0f, nz });
		}
		for (int i = 0; i < n; i++) {
			unsigned int a = base + 2 * i;
			ind.insert(ind.end(), { a, a + 1, a + 3, a + 3, a + 2, a });
		}
	}
	MeshModel* muro = new MeshModel();
	muro->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(muro);
}

// esfera de radio 1; con lat0 y lat1 se puede sacar solo un pedazo (el hemisferio de abajo de los planetas)
void CrearEsfera(float lat0, float lat1)
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	int pisos = 16, gajos = 32;
	for (int i = 0; i <= pisos; i++) {
		float lat = lat0 + (lat1 - lat0) * i / pisos;   // 0 = polo de arriba, PI = polo de abajo
		for (int j = 0; j <= gajos; j++) {
			float lon = 2.0f * PI * j / gajos;
			float x = sin(lat) * cos(lon), y = cos(lat), z = sin(lat) * sin(lon);
			v.insert(v.end(), { x, y, z,	(float)j / gajos * 2.0f, (float)i / pisos,	-x, -y, -z });
		}
	}
	for (int i = 0; i < pisos; i++)
		for (int j = 0; j < gajos; j++) {
			unsigned int a = i * (gajos + 1) + j, b = a + gajos + 1;
			ind.insert(ind.end(), { a, b, a + 1, a + 1, b, b + 1 });
		}
	MeshModel* esfera = new MeshModel();
	esfera->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(esfera);
}

// cilindro de y = 0 a y = 1 con radio de abajo 0.5; con rArriba = 0 sale un cono
void CrearCilindro(float rArriba)
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	int n = 24;
	for (int i = 0; i <= n; i++) {
		float a = 2.0f * PI * i / n;
		v.insert(v.end(), { 0.5f * cos(a), 0.0f, 0.5f * sin(a),		(float)i / n, 0.0f,	-cos(a), 0.0f, -sin(a) });
		v.insert(v.end(), { rArriba * cos(a), 1.0f, rArriba * sin(a),	(float)i / n, 1.0f,	-cos(a), 0.0f, -sin(a) });
	}
	for (int i = 0; i < n; i++) {
		unsigned int a = 2 * i;
		ind.insert(ind.end(), { a, a + 2, a + 1, a + 1, a + 2, a + 3 });
	}
	// tapas
	for (int tapa = 0; tapa < 2; tapa++) {
		float y = tapa == 0 ? 0.0f : 1.0f, r = tapa == 0 ? 0.5f : rArriba, ny = tapa == 0 ? 1.0f : -1.0f;
		unsigned int c = v.size() / 8;
		v.insert(v.end(), { 0.0f, y, 0.0f,	0.5f, 0.5f,	0.0f, ny, 0.0f });
		for (int i = 0; i <= n; i++) {
			float a = 2.0f * PI * i / n;
			v.insert(v.end(), { r * cos(a), y, r * sin(a),	0.5f + 0.5f * cos(a), 0.5f + 0.5f * sin(a),	0.0f, ny, 0.0f });
		}
		for (int i = 0; i < n; i++) { ind.push_back(c); ind.push_back(c + 1 + i); ind.push_back(c + 2 + i); }
	}
	MeshModel* cil = new MeshModel();
	cil->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(cil);
}

// disco de radio 1 en y = 0 (piso de los planetas y de la plataforma); la textura se repite 24 veces (los planetas son cuatro veces mas grandes que al principio)
void CrearDisco()
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	int n = 96;
	v.insert(v.end(), { 0.0f, 0.0f, 0.0f,	0.0f, 0.0f,	0.0f, -1.0f, 0.0f });
	for (int i = 0; i <= n; i++) {
		float a = 2.0f * PI * i / n;
		v.insert(v.end(), { cos(a), 0.0f, sin(a),	24.0f * cos(a), 24.0f * sin(a),	0.0f, -1.0f, 0.0f });
	}
	for (int i = 0; i < n; i++) { ind.push_back(0); ind.push_back(1 + i); ind.push_back(2 + i); }
	MeshModel* disco = new MeshModel();
	disco->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(disco);
}

// anillo plano (corona entre radio 1.4 y 2.4) para el planeta gaseoso; u va de adentro hacia afuera
void CrearAnillo()
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	int n = 96;
	for (int i = 0; i <= n; i++) {
		float a = 2.0f * PI * i / n;
		v.insert(v.end(), { 1.4f * cos(a), 0.0f, 1.4f * sin(a),	0.0f, i * 8.0f / n,	0.0f, -1.0f, 0.0f });
		v.insert(v.end(), { 2.4f * cos(a), 0.0f, 2.4f * sin(a),	1.0f, i * 8.0f / n,	0.0f, -1.0f, 0.0f });
	}
	for (int i = 0; i < n; i++) {
		unsigned int a = 2 * i;
		ind.insert(ind.end(), { a, a + 1, a + 3, a + 3, a + 2, a });
	}
	MeshModel* anillo = new MeshModel();
	anillo->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(anillo);
}

// cuadro de 1 x 1 en el plano XY (de 0 a 1), para el menu de teclas que se dibuja encima de la escena
void CrearCuadro()
{
	unsigned int ind[] = { 0, 1, 2, 2, 3, 0 };
	GLfloat v[] = {
		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,	1.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		1.0f, 1.0f, 0.0f,	1.0f, 1.0f,	0.0f, 0.0f, -1.0f,
		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,	0.0f, 0.0f, -1.0f,
	};
	MeshModel* cuadro = new MeshModel();
	cuadro->CreateMeshModel(v, ind, 32, 6);
	meshListModel.push_back(cuadro);
}

void CreateShaders()
{
	// ojo: el vector guarda copias y si crece las vuelve a copiar; el destructor de Shader borra el programa de OpenGL,
	// asi que reservo lugar para todos desde antes (si no, el shader de luces quedaba borrado y la escena salia negra)
	shaderList.reserve(3);
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
	Shader* shader2 = new Shader();	// [1] menu de teclas (solo textura, sin luces)
	shader2->CreateFromFiles("shaders/shader_hud.vert", "shaders/shader_hud.frag");
	shaderList.push_back(*shader2);
	Shader* shader3 = new Shader();	// [2] efectos transparentes sin luces: escudos de Reach, humo y llama del cohete
	shader3->CreateFromFiles("shaders/shader_efectos.vert", "shaders/shader_efectos.frag");
	shaderList.push_back(*shader3);
}


int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();
	CrearCubo();			// 0
	CrearTornillo();		// 1
	CrearPista();			// 2
	CrearEsfera(0.0f, PI);	// 3 esfera completa
	CrearEsfera(PI / 2, PI);	// 4 hemisferio de abajo
	CrearCilindro(0.5f);	// 5
	CrearCilindro(0.0f);	// 6 cono
	CrearDisco();			// 7
	CrearMuroPista();		// 8
	CrearCuadro();			// 9
	CrearAnillo();			// 10
	// 11 barricada de Reach: el mismo cubo de lado 1, con la cara de arriba angosta en z (de 1 a 0.4) para que los lados queden inclinados
	unsigned int barricada_indices[] = {
		0, 1, 2,	2, 3, 0,
		4, 5, 6,	6, 7, 4,
		8, 9, 10,	10, 11, 8,
		12, 13, 14,	14, 15, 12,
		16, 17, 18,	18, 19, 16,
		20, 21, 22,	22, 23, 20,
	};
	GLfloat barricada_vertices[] = {
		//x		y		z		S		T		NX		NY		NZ
		// frente (+z), inclinado
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		0.0f,	-0.287f,	-1.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f,	-0.287f,	-1.0f,
		 0.5f,  0.5f,  0.2f,	1.0f, 1.0f,		0.0f,	-0.287f,	-1.0f,
		-0.5f,  0.5f,  0.2f,	0.0f, 1.0f,		0.0f,	-0.287f,	-1.0f,
		// derecha (+x)
		 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		-1.0f,	0.0f,	0.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		-1.0f,	0.0f,	0.0f,
		 0.5f,  0.5f, -0.2f,	1.0f, 1.0f,		-1.0f,	0.0f,	0.0f,
		 0.5f,  0.5f,  0.2f,	0.0f, 1.0f,		-1.0f,	0.0f,	0.0f,
		 // atras (-z), inclinado
		  0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f,	-0.287f,	1.0f,
		 -0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		0.0f,	-0.287f,	1.0f,
		 -0.5f,  0.5f, -0.2f,	1.0f, 1.0f,		0.0f,	-0.287f,	1.0f,
		  0.5f,  0.5f, -0.2f,	0.0f, 1.0f,		0.0f,	-0.287f,	1.0f,
		  // izquierda (-x)
		  -0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		1.0f,	0.0f,	0.0f,
		  -0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		1.0f,	0.0f,	0.0f,
		  -0.5f,  0.5f,  0.2f,	1.0f, 1.0f,		1.0f,	0.0f,	0.0f,
		  -0.5f,  0.5f, -0.2f,	0.0f, 1.0f,		1.0f,	0.0f,	0.0f,
		  // abajo (-y)
		  -0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f,	1.0f,	0.0f,
		   0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		0.0f,	1.0f,	0.0f,
		   0.5f, -0.5f,  0.5f,	1.0f, 1.0f,		0.0f,	1.0f,	0.0f,
		  -0.5f, -0.5f,  0.5f,	0.0f, 1.0f,		0.0f,	1.0f,	0.0f,
		  // arriba (+y), angosta
		  -0.5f,  0.5f,  0.2f,	0.0f, 0.0f,		0.0f,	-1.0f,	0.0f,
		   0.5f,  0.5f,  0.2f,	1.0f, 0.0f,		0.0f,	-1.0f,	0.0f,
		   0.5f,  0.5f, -0.2f,	1.0f, 1.0f,		0.0f,	-1.0f,	0.0f,
		  -0.5f,  0.5f, -0.2f,	0.0f, 1.0f,		0.0f,	-1.0f,	0.0f,
	};
	MeshModel* barricada = new MeshModel();
	barricada->CreateMeshModel(barricada_vertices, barricada_indices, 192, 36);
	meshListModel.push_back(barricada);
	// 12 escudo de Reach: plano vertical de 1 x 1 con la base en y = 0
	GLfloat escudo_vertices[] = {
		-0.5f, 0.0f, 0.0f,	0.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		 0.5f, 0.0f, 0.0f,	1.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		 0.5f, 1.0f, 0.0f,	1.0f, 1.0f,	0.0f, 0.0f, -1.0f,
		-0.5f, 1.0f, 0.0f,	0.0f, 1.0f,	0.0f, 0.0f, -1.0f,
	};
	unsigned int escudo_indices[] = { 0, 1, 2,	2, 3, 0 };
	MeshModel* escudo = new MeshModel();
	escudo->CreateMeshModel(escudo_vertices, escudo_indices, 32, 6);
	meshListModel.push_back(escudo);
	// 13 marca de aterrizaje de Reach: disco de radio 1 con la textura una sola vez (u, v de 0 a 1)
	std::vector<GLfloat> marca_vertices;
	std::vector<unsigned int> marca_indices;
	marca_vertices.insert(marca_vertices.end(), { 0.0f, 0.0f, 0.0f,	0.5f, 0.5f,	0.0f, -1.0f, 0.0f });
	for (int i = 0; i <= 64; i++) {
		float a = 2.0f * PI * i / 64;
		marca_vertices.insert(marca_vertices.end(), { cos(a), 0.0f, sin(a),	0.5f + 0.5f * cos(a), 0.5f + 0.5f * sin(a),	0.0f, -1.0f, 0.0f });
	}
	for (int i = 0; i < 64; i++) { marca_indices.push_back(0); marca_indices.push_back(1 + i); marca_indices.push_back(2 + i); }
	MeshModel* marca = new MeshModel();
	marca->CreateMeshModel(marca_vertices.data(), marca_indices.data(), marca_vertices.size(), marca_indices.size());
	meshListModel.push_back(marca);
	// 14 humo del cohete: plano vertical de 1 x 1 que usa 1/8 de la textura; los otros cuadros se recorren con el desplazamiento
	unsigned int humo_indices[] = { 0, 1, 2,	0, 2, 3 };
	GLfloat humo_vertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,		0.125f, 0.0f,	0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,		0.125f, 1.0f,	0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	MeshModel* humo = new MeshModel();
	humo->CreateMeshModel(humo_vertices, humo_indices, 32, 6);
	meshListModel.push_back(humo);
	// 15 llama del cohete: dos planos cruzados que cuelgan del origen hacia abajo, cada uno con 1/8 de la textura
	unsigned int llama_indices[] = { 0, 1, 2,	0, 2, 3,	4, 5, 6,	4, 6, 7 };
	GLfloat llama_vertices[] = {
		-0.5f, -1.0f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.5f, -1.0f, 0.0f,		0.125f, 0.0f,	0.0f, 0.0f, 0.0f,
		 0.5f,  0.0f, 0.0f,		0.125f, 1.0f,	0.0f, 0.0f, 0.0f,
		-0.5f,  0.0f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		 0.0f, -1.0f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f, -1.0f,  0.5f,	0.125f, 0.0f,	0.0f, 0.0f, 0.0f,
		 0.0f,  0.0f,  0.5f,	0.125f, 1.0f,	0.0f, 0.0f, 0.0f,
		 0.0f,  0.0f, -0.5f,	0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	MeshModel* llama = new MeshModel();
	llama->CreateMeshModel(llama_vertices, llama_indices, 64, 12);
	meshListModel.push_back(llama);
	// 16 plasma del cohete de Venom: dos planos cruzados de 1 x 1 con la textura completa (la parte ancha arriba)
	unsigned int plasma_indices[] = { 0, 1, 2,	0, 2, 3,	4, 5, 6,	4, 6, 7 };
	GLfloat plasma_vertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		 0.0f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f,  0.5f,  0.5f,	1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		 0.0f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	MeshModel* plasma = new MeshModel();
	plasma->CreateMeshModel(plasma_vertices, plasma_indices, 64, 12);
	meshListModel.push_back(plasma);
	CreateShaders();

	// camara libre: arranca viendo Mundo Forja desde arriba
	camera = Camera(C_FORJA + glm::vec3(0.0f, 50.0f, 80.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -30.0f, 0.8f, 0.5f);
	// si existe la variable PF_CAM ("x,y,z,yaw,pitch") la camara libre arranca ahi; sirve para tomar capturas de un punto exacto
	char textoCam[128]; size_t largoCam = 0;
	if (getenv_s(&largoCam, textoCam, sizeof(textoCam), "PF_CAM") == 0 && largoCam > 0) {
		float cx, cy, cz, cyaw, cpitch;
		if (sscanf_s(textoCam, "%f,%f,%f,%f,%f", &cx, &cy, &cz, &cyaw, &cpitch) == 5)
			camera = Camera(glm::vec3(cx, cy, cz), glm::vec3(0.0f, 1.0f, 0.0f), cyaw, cpitch, 0.8f, 0.5f);
	}

	plainTexture = Texture("Textures/plain.png");			plainTexture.LoadTextureA();
	forjaTexture = Texture("Textures/terreno_forja.png");	forjaTexture.LoadTextureA();
	reachTexture = Texture("Textures/terreno_reach.png");	reachTexture.LoadTextureA();
	venomTexture = Texture("Textures/terreno_venom.png");	venomTexture.LoadTextureA();
	rocaTexture = Texture("Textures/roca.png");				rocaTexture.LoadTextureA();
	metalTexture = Texture("Textures/metal.png");			metalTexture.LoadTextureA();
	pistaTexture = Texture("Textures/pista_espacial.png");	pistaTexture.LoadTextureA();
	neonTexture = Texture("Textures/neon_pista.png");		neonTexture.LoadTextureA();
	// paneles del menu de teclas : 0 forja, 1 reach, 2 venom, 3 espacio
	// (la clase Texture guarda el apuntador al nombre, no una copia, asi que los nombres tienen que seguir existiendo)
	static const char* archivosMenu[4] = { "Textures/menu_forja.png", "Textures/menu_reach.png", "Textures/menu_venom.png", "Textures/menu_espacio.png" };
	for (int i = 0; i < 4; i++) { menuTexture[i] = Texture(archivosMenu[i]); menuTexture[i].LoadTextureA(); }
	pestanaTexture = Texture("Textures/menu_pestana.png");	pestanaTexture.LoadTextureA();
	solarTexture = Texture("Textures/solar.png");			solarTexture.LoadTextureA();
	ventanasTexture = Texture("Textures/ventanas.png");		ventanasTexture.LoadTextureA();
	anilloTexture = Texture("Textures/anillo.png");			anilloTexture.LoadTextureA();
	static const char* archivosPlaneta[4] = { "Textures/planeta_gas.png", "Textures/planeta_hielo.png", "Textures/planeta_lava.png", "Textures/planeta_alien.png" };
	for (int i = 0; i < 4; i++) { planetaTexture[i] = Texture(archivosPlaneta[i]); planetaTexture[i].LoadTextureA(); }

	naveSabre = Model();		naveSabre.LoadModel("Models/halo/sabre.obj");	// N2 de la pista (texturas en Textures/halo)
	naveSpeederB = Model();		naveSpeederB.LoadModel("Models/kenney/craft_speederB.obj");
	antenaSatelite = Model();	antenaSatelite.LoadModel("Models/kenney/satelliteDish_large.obj");
	meteoro = Model();			meteoro.LoadModel("Models/kenney/meteor_detailed.obj");
	chimenea = Model();			chimenea.LoadModel("Models/kenney/chimney_detailed.obj");
	generador = Model();		generador.LoadModel("Models/kenney/machine_generatorLarge.obj");
	ratchetModelo = Model();	ratchetModelo.LoadModel("Models/rc/ratchet.obj");
	vendedorGadgetron = Model();	vendedorGadgetron.LoadModel("Models/rc/vendedor.obj");
	clankModelo = Model();		clankModelo.LoadModel("Models/rc/clank.obj");
	hoverbike = Model();		hoverbike.LoadModel("Models/rc/hoverbike.obj");
	cajaTornillos = Model();	cajaTornillos.LoadModel("Models/rc/caja_tornillos.obj");
	cajaNanotech = Model();		cajaNanotech.LoadModel("Models/rc/caja_nanotech.obj");
	cajaMetal = Model();		cajaMetal.LoadModel("Models/rc/caja_metal.obj");
	tornilloModelo = Model();	tornilloModelo.LoadModel("Models/rc/tornillo.obj");
	tornilloOro = Model();		tornilloOro.LoadModel("Models/rc/tornillo_oro.obj");
	naveRatchet = Model();		naveRatchet.LoadModel("Models/rc/nave_ratchet.obj");
	Etapa1_M = Model();		Etapa1_M.LoadModel("Models/Cohete/Etapa1.obj");	// sus texturas las lee de saturn_v.mtl
	Etapa2_M = Model();		Etapa2_M.LoadModel("Models/Cohete/Etapa2.obj");
	Etapa3_M = Model();		Etapa3_M.LoadModel("Models/Cohete/Etapa3.obj");
	reachEtapas[0] = Model();	reachEtapas[0].LoadModel("Models/SaturnV/etapa1.obj");	// sus texturas las lee de Models/SaturnV/saturn_v.mtl (en Textures/SaturnV)
	reachEtapas[1] = Model();	reachEtapas[1].LoadModel("Models/SaturnV/etapa2.obj");
	reachEtapas[2] = Model();	reachEtapas[2].LoadModel("Models/SaturnV/etapa3.obj");
	reachEtapas[3] = Model();	reachEtapas[3].LoadModel("Models/SaturnV/capsula.obj");
	alienCuerpo = Model();		alienCuerpo.LoadModel("Models/alien/Alien_Cuerpo.obj");	// sus texturas las lee de alien.mtl
	alienPataIzq = Model();		alienPataIzq.LoadModel("Models/alien/Alien_PataIzq.obj");
	alienPataDer = Model();		alienPataDer.LoadModel("Models/alien/Alien_PataDer.obj");
	alienPataFrente = Model();	alienPataFrente.LoadModel("Models/alien/Alien_PataFrente.obj");
	alienBrazoIzq = Model();	alienBrazoIzq.LoadModel("Models/alien/Alien_BrazoIzq.obj");
	alienBrazoDer = Model();	alienBrazoDer.LoadModel("Models/alien/Alien_BrazoDer.obj");
	alienCola = Model();		alienCola.LoadModel("Models/alien/Alien_Cola.obj");
	// Reach: los OBJ estan centrados en el origen; sus texturas las leen de Textures/halo por el .mtl
	//obj de venom
	arwingModel = Model();		arwingModel.LoadModel("Models/venom/naves/Arwing.obj");
	wolfenModel = Model();		wolfenModel.LoadModel("Models/venom/naves/WolfenII.obj");
	lunaMajoras = Model();		lunaMajoras.LoadModel("Models/venom/luna_majoras/moon.obj");
	catspawModel = Model();		catspawModel.LoadModel("Models/venom/naves/Catzpaw.obj");
	//cohete de venom
	cuerpoSuperiorV = Model();	cuerpoSuperiorV.LoadModel("Models/venom/cohete/cuerpo_superior.obj");	// sin texturas: se pintan con color al dibujarlos
	cuerpoInferiorV = Model();	cuerpoInferiorV.LoadModel("Models/venom/cohete/cuerpo_inferior.obj");
	propulsoresV = Model();		propulsoresV.LoadModel("Models/venom/cohete/cohetes_propulsores.obj");

	// la textura moon_00_0.png va en Textures/
	pelican = Model();			pelican.LoadModel("Models/halo/pelican.obj");
	warthog = Model();			warthog.LoadModel("Models/halo/warthog_chasis.obj");
	ruedasWarthog[0] = Model();	ruedasWarthog[0].LoadModel("Models/halo/warthog_rueda_0.obj");
	ruedasWarthog[1] = Model();	ruedasWarthog[1].LoadModel("Models/halo/warthog_rueda_1.obj");
	ruedasWarthog[2] = Model();	ruedasWarthog[2].LoadModel("Models/halo/warthog_rueda_2.obj");
	ruedasWarthog[3] = Model();	ruedasWarthog[3].LoadModel("Models/halo/warthog_rueda_3.obj");
	torretaWarthog = Model();	torretaWarthog.LoadModel("Models/halo/warthog_torreta.obj");
	marine = Model();			marine.LoadModel("Models/halo/marine.obj");
	inquisidor = Model();		inquisidor.LoadModel("Models/halo/inquisidor.obj");
	grunt = Model();			grunt.LoadModel("Models/halo/grunt.obj");
	jefe = Model();				jefe.LoadModel("Models/halo/jefe.obj");
	concretoReach = Texture("Textures/halo/concreto.png");		concretoReach.LoadTextureA();
	cajaReach = Texture("Textures/halo/caja_unsc.png");			cajaReach.LoadTextureA();
	letreroReach = Texture("Textures/halo/letrero_unsc.png");	letreroReach.LoadTextureA();
	pistaReach = Texture("Textures/halo/aterrizaje.png");		pistaReach.LoadTextureA();
	escudoReach = Texture("Textures/halo/escudo.png");			escudoReach.LoadTextureA();
	humoTexture = Texture("Textures/humo.png");					humoTexture.LoadTextureA();	// 8 cuadros en fila, de humo denso a casi transparente
	llamaTexture = Texture("Textures/llama_atlas.png");			llamaTexture.LoadTextureA();	// 8 cuadros de la llama
	plasmaTexture = Texture("Textures/plasma_azul.png");		plasmaTexture.LoadTextureA();	// plasma azul del cohete de Venom (practica 10)
	// skyboxes: orden rt, lf, up, dn, bk, ft
	std::vector<std::string> caras;
	const char* nombresSky[4] = { "espacio", "forja", "reach", "venom" };
	Skybox* skies[4] = { &skyEspacio, &skyForja, &skyReach, &skyVenom };
	for (int s = 0; s < 4; s++) {
		caras.clear();
		const char* lado[6] = { "rt", "lf", "up", "dn", "bk", "ft" };
		for (int k = 0; k < 6; k++) caras.push_back(std::string("Textures/Skybox/") + nombresSky[s] + "_" + lado[k] + ".png");
		*skies[s] = Skybox(caras);
	}

	metalPulido = Material(1.0f, 128);
	metalPintado = Material(0.5f, 32);
	metalNave = Material(0.8f, 64);
	concreto = Material(0.2f, 8);
	roca = Material(0.1f, 4);
	materialAlien = Material(0.6f, 40);


	//=========================================================================================
	// 2. ILUMINACION (las luces fijas se crean aqui, el sol y los apagados se calculan en el ciclo)
	//=========================================================================================
	// luminarias: 4 postes por planeta, solo de noche (L1 a L12; antes eran 6, dos por planeta, como en la tabla 1.13.1)
	glm::vec3 posLuminaria[12] = {
		C_FORJA + glm::vec3(-20, 0, -20), C_FORJA + glm::vec3(20, 0, -17), C_FORJA + glm::vec3(-36, 0, 10), C_FORJA + glm::vec3(18, 0, 26),
		C_REACH + glm::vec3(-31, 0, -34), C_REACH + glm::vec3(22, 0, -25), C_REACH + glm::vec3(-39, 0, 3), C_REACH + glm::vec3(22, 0, 28),	// entre los objetos de Reach ya separados
		C_VENOM + glm::vec3(-20, 0, -14), C_VENOM + glm::vec3(20, 0, -14), C_VENOM + glm::vec3(-24, 0, 16), C_VENOM + glm::vec3(24, 0, 18) };
	glm::vec3 colorLuminaria[3] = { glm::vec3(1.0f, 0.8f, 0.4f), glm::vec3(0.8f, 0.9f, 1.0f), glm::vec3(0.3f, 1.0f, 0.3f) };	// forja amarillo, reach blanco azulado, venom verde
	PointLight luminarias[12];
	for (int i = 0; i < 12; i++) {
		glm::vec3 c = colorLuminaria[i / 4], q = posLuminaria[i];
		luminarias[i] = PointLight(c.x, c.y, c.z, 0.0f, 1.0f, q.x, 5.0f, q.z, 1.0f, 0.14f, 0.012f);
	}
	// luces de ambiente que siempre estan prendidas: le dan color a cada planeta y a la pista
	const int N_AMBIENTE = 10;
	glm::vec3 posAmbiente[N_AMBIENTE] = {
		C_FORJA + glm::vec3(24, 6, 1),		// horno de la refineria
		C_FORJA + glm::vec3(-6, 4, 3),		// vendedor Gadgetron
		C_REACH + glm::vec3(-14, 6, -16),	// frente del cuartel
		C_REACH + glm::vec3(34, 3, 14),		// zona de aterrizaje
		C_VENOM + glm::vec3(0, 7, 17),		// base de Andross
		C_VENOM + glm::vec3(-14, 4, -2),	// torres
		glm::vec3(PISTA_A, 20.0f, PISTA_Z), glm::vec3(-PISTA_A, 20.0f, PISTA_Z), glm::vec3(0.0f, 20.0f, PISTA_Z + PISTA_B), glm::vec3(0.0f, 20.0f, PISTA_Z - PISTA_B) };	// 4 de la pista
	glm::vec3 colorAmbiente[N_AMBIENTE] = { glm::vec3(1.0f, 0.45f, 0.1f), glm::vec3(0.2f, 0.5f, 1.0f), glm::vec3(0.9f, 0.95f, 1.0f), glm::vec3(0.3f, 1.0f, 0.4f),
		glm::vec3(1.0f, 0.2f, 0.1f), glm::vec3(0.7f, 0.3f, 1.0f), glm::vec3(0.2f, 0.9f, 1.0f), glm::vec3(1.0f, 0.3f, 0.8f), glm::vec3(0.2f, 0.9f, 1.0f), glm::vec3(1.0f, 0.3f, 0.8f) };
	PointLight lucesAmbiente[N_AMBIENTE];
	for (int i = 0; i < N_AMBIENTE; i++) {
		glm::vec3 c = colorAmbiente[i], q = posAmbiente[i];
		lucesAmbiente[i] = PointLight(c.x, c.y, c.z, 0.0f, 1.6f, q.x, q.y, q.z, 1.0f, 0.09f, 0.006f);
	}

	// S1 reflector de la plataforma (tecla L) y S2 luces de la estacion (tecla K)
	SpotLight reflectorS1 = SpotLight(1.0f, 1.0f, 0.8f, 0.0f, 3.0f, PLATAFORMA.x, 18.0f, PLATAFORMA.z - 7.0f, 0.0f, -1.0f, 0.35f, 1.0f, 0.02f, 0.001f, 25.0f);
	SpotLight estacionS2 = SpotLight(0.4f, 0.8f, 1.0f, 0.0f, 3.6f, C_ESTACION.x, C_ESTACION.y - 8.4f, C_ESTACION.z + 16.8f, 0.0f, 0.3f, -1.0f, 1.0f, 0.03f, 0.002f, 20.0f);
	bool s1Prendido = true, s2Prendido = true;

	// ciclo de dia y noche: 120 s; con X se acelera para probarlo (la N es para KF2)
	float tiempoDia = 30.0f;	// arranca a media manana
	{
		char textoHora[32]; size_t largoHora = 0; float horaIni;	// PF_HORA (segundos del ciclo) sirve para ver el escenario de dia o de noche sin esperar
		if (getenv_s(&largoHora, textoHora, sizeof(textoHora), "PF_HORA") == 0 && largoHora > 0 && sscanf_s(textoHora, "%f", &horaIni) == 1) tiempoDia = horaIni;
	}
	float velocidadDia = 1.0f;


	//=========================================================================================
	// 3. CAMARAS (1 avatar en tercera persona, 2 nave N1, 3 nave viajera, 4 cinematica)
	//=========================================================================================
	int modoCamara = 1;
	glm::vec3 camPos = camera.getCameraPosition(), camMira = camPos + camera.getCameraDirection();
	glm::vec3 camPosAntes = camPos, camMiraAntes = camMira;
	float tTransicion = 1.0f;	// 1 = ya llego; cuando cambia de camara se reinicia y se interpola 0.5 s
	float anguloC4 = 0.0f;
	// puntos de llegada para la tecla T (centro de cada planeta, en el orden P1 -> P2 -> P3)
	glm::vec3 llegadas[3] = { C_FORJA + glm::vec3(-30.0f, 8.0f, 24.0f), C_REACH + glm::vec3(-25.0f, 8.0f, 28.0f), C_VENOM + glm::vec3(0.0f, 8.0f, -33.0f) };	// dentro de la zona del skybox de cada planeta
	int planetaActual = 0;
	// avatar: camina sobre el plano XZ del planeta y la camara 1 lo sigue desde atras
	glm::vec3 avatarPos = glm::vec3(llegadas[0].x, 0.0f, llegadas[0].z);
	float avatarGiro = 0.0f, avatarFase = 0.0f, avatarAmp = 0.0f;	// giro en Y (radianes), fase de la caminata y cuanto se mueven las extremidades (0 parado, 1 caminando)
	{
		glm::vec3 d = C_FORJA - avatarPos;
		float yawIni = glm::degrees(atan2(d.z, d.x)), pitchIni = -12.0f;
		char textoAv[128]; size_t largoAv = 0;
		if (getenv_s(&largoAv, textoAv, sizeof(textoAv), "PF_CAM") == 0 && largoAv > 0) {	// para capturas: x,y,z,yaw,pitch; el avatar se pone en x,z
			float ax, ay, az, ayaw, apitch;
			if (sscanf_s(textoAv, "%f,%f,%f,%f,%f", &ax, &ay, &az, &ayaw, &apitch) == 5) { avatarPos = glm::vec3(ax, 0.0f, az); yawIni = ayaw; pitchIni = apitch; }
		}
		avatarGiro = glm::radians(90.0f - yawIni);
		camera = Camera(avatarPos, glm::vec3(0.0f, 1.0f, 0.0f), yawIni, pitchIni, 0.8f, 0.5f);
	}
	// ARWING: estado de la nave (autopiloto o controlada con la coma)
	// Wolfen: radio, altura, velocidad angular y fase de cada uno de los tres que patrullan Venom
	const float radioWolfen[3] = { 58.0f, 40.0f, 68.0f }, altWolfen[3] = { 26.0f, 24.0f, 29.0f };
	const float wWolfen[3] = { 0.30f, -0.36f, 0.24f }, faseWolfen[3] = { 0.0f, 2.1f, 4.2f };
	bool pilotando = false;
	// camara libre (tecla C en Window): sale del avatar y la camara se mueve con WASD y el mouse
	bool camaraLibreAntes = false;
	float yawLibre = 0.0f, pitchLibre = 0.0f;
	glm::vec3 dirLibre = glm::vec3(0.0f);
	glm::vec3 posArwing = C_VENOM + glm::vec3(ARWING_R_PATRULLA, ARWING_ALT_PATRULLA, 0.0f), camChase = glm::vec3(0.0f);
	float yawArwing = 0.0f, pitchArwing = 0.0f, rollArwing = 0.0f;	// yaw 0 = nariz hacia +z


	//=========================================================================================
	// variables de las animaciones
	//=========================================================================================
	// AC2 naves de la pista: angulo de cada una y velocidad base distinta para que se rebasen
	float angNave[3] = { 0.0f, 2.1f, 4.2f };
	float w0Nave[3] = { 0.30f, 0.28f, 0.26f };
	glm::vec3 colorNave[3] = { glm::vec3(0.9f, 0.45f, 0.15f), glm::vec3(0.65f, 0.7f, 0.75f), glm::vec3(0.95f, 0.55f, 0.65f) };	// N1 hoverbike, N2 sabre, N3 catspaw
	// nave viajera NV (Pelican): avanza sobre una curva Catmull-Rom P1 -> P2 -> P3 -> P1
	glm::vec3 rutaNV[3] = { C_FORJA + glm::vec3(0.0f, 42.0f, 0.0f), C_REACH + glm::vec3(0.0f, 42.0f, 0.0f), C_VENOM + glm::vec3(0.0f, 42.0f, 0.0f) };
	float uNV = 0.5f;	// arranca a medio camino entre Mundo Forja y Reach, a la vista de la camara 3
	// anillo de la estacion
	float giroAnillo = 0.0f;
	// AB1 cohete: 0 en la plataforma, 1 cuenta regresiva, 2 sube, 3 va a la estacion, 4 acoplado dentro de la bahia, 5 sale de la estacion y regresa, 6 aterriza
	int faseCohete = 0; float tCohete = 0.0f; glm::vec3 posCohete = PLATAFORMA; float brazoServicio = 0.0f; float inclinaCohete = 0.0f;
	// el cohete se fragmenta como en la practica 9-2: la etapa 1 se suelta al terminar la subida vertical y la etapa 2 al llegar a la estacion
	const float escalaCohete = 1.6f;					// el modelo mide 9 de alto (de -4.5 a 4.5), queda de 14.4
	glm::mat4 cohM(1.0f);								// centro del cohete ya colocado e inclinado (sin escalar)
	glm::vec3 posCoheteAntes = PLATAFORMA, velCohete(0.0f);	// para sacar su velocidad cuadro a cuadro
	float temblorCohete = 0.0f;
	int etapaEncendida = 0;							// 0 primera, 1 segunda, 2 tercera
	bool sepHecha[2] = { false, false };
	float sepPivote[2] = { -2.9f, 0.1f };				// centro de cada etapa en el modelo, para girarla
	glm::vec3 sepPos[2], sepVel[2];
	float sepInc[2] = { 0.0f, 0.0f }, sepVGiro[2] = { 40.0f, 55.0f };	// inclinacion y grados por segundo de la etapa suelta
	const float gravedadEtapa = 8.0f;					// la etapa suelta es un tiro libre: solo la gravedad cambia su velocidad
	// llama: textura animada de 8 cuadros; su base es la tobera de la etapa encendida
	float toberaEtapa[3] = { -4.5f, -1.0f, 1.25f };		// tobera de cada etapa en el modelo
	float llamaAncho[3] = { 3.6f, 2.3f, 1.6f }, llamaLargo[3] = { 8.5f, 5.3f, 3.4f };
	float llamaIgn = 0.0f, llamaLong = 0.0f;			// llamaIgn: 0 apagada, 1 encendida del todo
	int llamaCuadro = 0;
	// humo: cada bocanada vive humoVida segundos y cambia de cuadro con su edad
	const int MAX_HUMO = 300;
	glm::vec3 humoPos[MAX_HUMO], humoVel[MAX_HUMO];
	float humoEdad[MAX_HUMO];
	for (int i = 0; i < MAX_HUMO; i++) humoEdad[i] = 4.0f;	// al empezar no hay humo: todas ya cumplieron su vida
	float humoVida = 4.0f, humoCada = 0.03f, humoAcum = 0.0f, humoDisp = 0.0f, humoTam = 0.0f;
	int humoSig = 0, humoCuadro = 0;
	glm::vec3 tobera(0.0f);

	// cohete de Venom (practica 10, tecla F en Window): por keyframes, empieza en la rampa del domo, abre la escotilla, sube, va a la estacion y orbita
	// se fragmenta como en la practica: primero se sueltan los propulsores, despues el cuerpo inferior, y la capsula sigue sola
	const glm::vec3 RAMPA_V = C_VENOM + glm::vec3(0.0f, DOMO_BASE + DOMO_ALTO + 0.55f, 0.0f);	// cima del domo, sobre la escotilla
	const float escalaCoheteV = 1.3f;					// el modelo mide 7.6 de alto (de y = 0.19 a 7.82), queda de ~10
	const float baseCoheteV = RAMPA_V.y - 0.19f * escalaCoheteV;	// altura del origen del modelo cuando esta apoyado en la rampa
	glm::vec3 posCoheteV = glm::vec3(RAMPA_V.x, baseCoheteV, RAMPA_V.z);
	glm::mat4 cohMV(1.0f);								// origen del cohete ya colocado e inclinado (sin escalar)
	float inclinaV = 0.0f, rumboV = 0.0f;				// inclinacion respecto a la vertical y hacia donde apunta (radianes)
	float temblorV = 0.0f, aperturaDomoV = 0.0f;
	bool sepHechaV[2] = { false, false };				// 0 propulsores, 1 cuerpo inferior
	float sepPivoteV[2] = { 1.42f, 1.685f };			// centro de cada pieza en el modelo, para girarla
	glm::vec3 sepPosV[2];
	float sepIncV[2] = { 0.0f, 0.0f };
	// keyframes del cohete de Venom (Practica 10 de 318337872): cada cuadro del txt trae 19 valores: 0 tiempo, 1 a 3 posicion del cohete, 4 inclinacion, 5 rumbo (grados),
	// 6 apertura del domo, 7 temblor, 8 plasma encendido y, de los propulsores (9 a 13) y del cuerpo inferior (14 a 18): suelto, X, Y, Z y giro en Z
	const char vkArchivo[] = "keyframes_cohete_venom.txt";
	float vkD[200][19] = { { 0.0f } }, vkNuevo[200][19] = { { 0.0f } }, vkAct[19] = { 0.0f }, vkReloj = 0.0f, vkU = 0.0f, vkVal = 0.0f;
	int vkN = 0, vkOrbita = 0, vkCuadro = 0, vkCuantos = 0, vkOrb = 0, vkFilas = 0, vkCol = 0, vkDesp = 0, vkTotal = 0, vkImp = -1, vkImpFin = -1;
	bool vkPlay = false, vkBien = false, vkLeer = true, vkTabla = false;
	char vkLinea[512] = "";
	const char* vkMsg = "";
	FILE* vkFile = nullptr;
	// plasma azul: sale de las toberas de las piezas que siguen pegadas (posiciones en el modelo, medidas en los .obj)
	const int N_TOBERAS_V = 8;							// 0 a 5 propulsores, 6 cuerpo inferior, 7 capsula
	glm::vec3 toberaV[N_TOBERAS_V] = { glm::vec3(0.392f, 0.189f, 0.023f), glm::vec3(0.196f, 0.189f, -0.316f), glm::vec3(-0.196f, 0.189f, -0.316f), glm::vec3(-0.392f, 0.189f, 0.023f),
		glm::vec3(-0.196f, 0.189f, 0.363f), glm::vec3(0.196f, 0.189f, 0.363f), glm::vec3(0.0f, 0.349f, 0.023f), glm::vec3(0.0f, 3.01f, 0.0f) };
	float toberaRadioV[N_TOBERAS_V] = { 0.112f, 0.112f, 0.112f, 0.112f, 0.112f, 0.112f, 0.168f, 0.17f };
	const int MAX_PLASMA = 400;
	glm::vec3 plasmaPos[MAX_PLASMA];
	float plasmaEdad[MAX_PLASMA], plasmaInc[MAX_PLASMA], plasmaRumbo[MAX_PLASMA], plasmaTam[MAX_PLASMA];
	for (int i = 0; i < MAX_PLASMA; i++) plasmaEdad[i] = 1.0f;	// al empezar no hay plasma
	float plasmaVida = 0.8f, plasmaCada = 0.02f, plasmaAcum = 0.0f, plasmaIgn = 0.0f;
	int plasmaSig = 0;
	float giroBaseCatspaw = 0.0f;	// giro de la Catspaw para que su nariz quede sobre la tangente de la pista
	// la Catspaw en la pista de carreras: angulo sobre la elipse y giro para que su nariz quede sobre la tangente

	// KF1 Ratchet vuela con el Heli-Pack alrededor de la refineria (tecla H): cuadros clave guardados e interpolacion lineal
	// cada cuadro es la posicion local en Mundo Forja y el giro en Y; entre cuadro y cuadro hay i_max_steps pasos
	// (las posiciones son desde el pedestal de la figura MF-11, asi el vuelo empieza y termina ahi)
	glm::vec3 kfPos[9] = { glm::vec3(0, 0, 0), glm::vec3(5.6f, 5, -5.6f), glm::vec3(14, 10, -19.6f), glm::vec3(22.4f, 14, -33.6f), glm::vec3(36.4f, 13, -25.2f),
		glm::vec3(36.4f, 14, -8.4f), glm::vec3(22.4f, 10, 2.8f), glm::vec3(8.4f, 5, 2.8f), glm::vec3(0, 0, 0) };
	float kfGiro[9] = { 0.0f, 140.0f, 149.0f, 100.0f, 30.0f, -25.0f, -70.0f, -100.0f, 0.0f };
	// ajustes de los modelos del juego: hacia donde miran y donde va Clank en la espalda de Ratchet (en unidades del modelo)
	float giroBaseRatchet = -90.0f, giroVendedor = 0.0f, giroBaseHoverbike = -90.0f;
	glm::vec3 clankEspalda = glm::vec3(-0.32f, 0.72f, 0.0f);	// desde los pies de Clank
	const int i_max_steps = 90;
	int kfCuadro = 0, i_curr_steps = 0; bool kfReproduciendo = false;
	glm::vec3 kfActual = kfPos[0]; float kfGiroActual = kfGiro[0]; float kfAcum = 0.0f;
	// KF-P10 cohete por keyframes (tecla B): cuadro actual, paso dentro del cuadro y valores
	// ya interpolados de cada etapa
	bool kcActivo = false;
	int kcCuadro = 0, kcPasos = 0; float kcAcum = 0.0f, kcHumoAcum = 0.0f;
	glm::vec3 kcPos[3]; float kcGiro[3] = { 0, 0, 0 }, kcLlama[3] = { 0, 0, 0 };
	// hacia donde mira la camara del cohete y a que distancia (tambien son keyframes)
	glm::vec3 kcCamFoco = glm::vec3(0.0f); float kcCamDist = 45.0f;
	const float kcPivote[3] = { -2.9f, 0.1f, 2.9f };	// centro de cada etapa en el modelo
	Model* etapasKC[3] = { &Etapa1_M, &Etapa2_M, &Etapa3_M };
	if (kcLeer()) printf("Se leyeron %d keyframes de %s\n", kcN, kcArchivo);
	else printf("No hay %s: se usan los del codigo (V los guarda)\n", kcArchivo);
	float giroPedestal = 0.0f;	// mientras esta en su pedestal, la figura gira despacio para que se vea completa
	
	// AB2 torreta: 0 quieta, 1 gira hacia la camara, 2 dispara
	int faseTorreta = 0; float giroTorre = 0.0f, angCanon = 0.0f, retroceso = 0.0f, giroObjetivo = 0.0f; glm::vec3 posBala; glm::vec3 dirBala; float tBala = 0.0f;
	bool menuAbierto = true;	// el menu de teclas arranca abierto; M lo cierra y lo abre
	int camPedida = 0;	// camara pedida con las teclas 1 a 6 (Window la guarda)
	// COHETE DE REACH (Practica 10 de 317136421): el Saturn V despega de Reach, suelta sus tres etapas y la capsula orbita la estacion
	// el txt trae por cuadro el tiempo y, de la etapa 1, la 2, la 3 y la capsula, la posicion X Y Z y el giro en Z; entre dos cuadros se interpola linealmente
	const char reachArchivo[] = "keyframes_cohete_reach.txt";
	float reachT[200] = { 0.0f }, reachGiro[200][4] = { { 0.0f } }, reachNuevoT[200] = { 0.0f }, reachNuevoGiro[200][4] = { { 0.0f } }, reachGiroAct[4] = { 0.0f };
	glm::vec3 reachPos[200][4] = {}, reachNuevoPos[200][4] = {}, reachPosAct[4] = {};
	float reachReloj = 0.0f, reachU = 0.0f, reachX = 0.0f, reachY = 0.0f, reachZ = 0.0f, reachG = 0.0f;
	int reachN = 0, reachOrbita = 0, reachCuadro = 0, reachCuantos = 0, reachOrb = 0, reachFilas = 0, reachPieza = 0, reachDesp = 0, reachTotal = 0, reachImp = -1, reachImpFin = -1;
	bool reachPlay = false, reachBien = false, reachLeer = true, reachTabla = false;
	char reachLinea[512] = "";
	const char* reachMsg = "";
	FILE* reachFile = nullptr;
	// (por ahora se usa la camara libre en lugar del avatar; cuando este el avatar se cambia camPos por su posicion)
	float tornilloOculto[12] = { 0 };	// segundos que le faltan al tornillo para reaparecer
	// Reach: el plano del integrante se hizo en unidades de 1; aqui se multiplica por escalaReach para llenar el planeta
	const float escalaReach = 2.8f;
	glm::mat4 mundoReach(1.0f), antenaHR(1.0f), chasisHR(1.0f), macHR(1.0f);
	float anguloMAC = 0.0f;
	// AB3 Pelican: P inicia el ciclo (sube 3 s, espera 2 s, baja 3 s); mientras dura se ignoran nuevas pulsaciones
	bool pelicanActivo = false;
	float tiempoPelican = 0.0f, alturaPelican = 0.0f;
	// posiciones del plano ya separadas (unas 1.7 veces mas lejos del centro que en el plano original) para llenar el planeta
	glm::vec2 barricadasHR[4] = { glm::vec2(-13.6f, -5.1f), glm::vec2(-13.6f, 5.1f), glm::vec2(0.0f, 5.1f), glm::vec2(5.1f, -3.4f) };
	glm::vec2 pinosHR[14] = { glm::vec2(-15.3f, -15.3f), glm::vec2(-8.5f, -18.7f), glm::vec2(8.5f, -17.0f), glm::vec2(17.0f, -11.9f), glm::vec2(-17.0f, 11.9f), glm::vec2(11.9f, 17.0f),
		glm::vec2(21, -2), glm::vec2(-21, -2), glm::vec2(4, 23), glm::vec2(-8, 21), glm::vec2(19, 13), glm::vec2(14, -21), glm::vec2(-19, -11), glm::vec2(0, -23) };
	glm::vec2 rocasHR[8] = { glm::vec2(-20.4f, -6.8f), glm::vec2(-20.4f, 1.7f), glm::vec2(18.7f, 6.8f), glm::vec2(1.7f, 20.4f),
		glm::vec2(10, -24), glm::vec2(-12, -22), glm::vec2(24, 4), glm::vec2(-24, 10) };
	glm::vec2 escudosHR[2] = { glm::vec2(3.4f, -8.5f), glm::vec2(5.1f, 11.9f) };
	// tecla O: el Warthog patrulla en un circulo; al soltarse queda donde iba. Las ruedas giran lo que avanza entre su radio
	glm::vec2 centroPatrulla = glm::vec2(-9.0f, 10.0f);
	const float radioPatrulla = 4.0f, radioRueda = 0.42f;
	float anguloWarthog = 0.0f, giroRuedas = 0.0f, giroTorretaW = 0.0f;
	// tecla J: pluma del puesto de control; sube hasta 80 grados y baja a 0, poco a poco
	float anguloPluma = 0.0f;
	// pivotes de las ruedas y de la torreta medidos desde el centro del chasis (el OBJ del chasis esta centrado; su centro original estaba en y = 1.06914)
	glm::vec3 pivoteRueda[4] = { glm::vec3(-0.81005f, 0.41767f - 1.06914f, -1.37820f), glm::vec3(-0.81005f, 0.41767f - 1.06914f, 1.33663f),
		glm::vec3(0.81005f, 0.41767f - 1.06914f, -1.37820f), glm::vec3(0.81005f, 0.41767f - 1.06914f, 1.33663f) };
	glm::vec3 pivoteTorreta = glm::vec3(0.0f, 1.52451f - 1.06914f, 0.03261f);
	Model* npcHR[4] = { &marine, &inquisidor, &grunt, &jefe };
	glm::vec3 posNpcHR[4] = { glm::vec3(-10, 0, -6), glm::vec3(-1.5f, 0, 13), glm::vec3(-1.7f, 0, 17.5f), glm::vec3(-10, 0, -3.5f) };
	float mitadNpcHR[4] = { 0.9f, 1.2f, 0.625f, 1.075f };	// mitad del alto de cada OBJ (estan centrados, se suben para que pisen el suelo)
	// efectos (escudos, humo y llama): shader propio con transparencia; sus uniform extra se piden una sola vez
	GLint programaEfectos = 0;
	shaderList[2].UseShader();
	glGetIntegerv(GL_CURRENT_PROGRAM, &programaEfectos);
	GLint uniformAlphaEfecto = glGetUniformLocation(programaEfectos, "alphaEfecto");
	GLint uniformColorEfecto = glGetUniformLocation(programaEfectos, "colorEfecto");
	GLint uniformOffsetEfecto = glGetUniformLocation(programaEfectos, "offsetEfecto");
	glm::vec2 offsetEfecto(0.0f);
	glUseProgram(0);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// variables de Venom (y el centro de Reach) que se usan dentro del while: se declaran aqui y adentro solo se les asigna
	glm::vec3 cSF = C_VENOM;
	const glm::vec3 dorado = glm::vec3(0.55f, 0.43f, 0.16f), oscuro = glm::vec3(0.17f, 0.15f, 0.11f), acero = glm::vec3(0.58f, 0.6f, 0.65f);
	glm::mat4 mDomo = glm::translate(glm::mat4(1.0f), cSF);
	glm::vec3 cHR = C_REACH;
	float giroV = 0.0f, pitchObj = 0.0f, aAct = 0.0f, dif = 0.0f, sV = 0.0f, uV = 0.0f, mezclaV = 0.0f, aper = 0.0f, alV = 0.0f, bV = 0.0f, aE = 0.0f, eY = 0.0f, cE = 0.0f, puertaE = 0.0f, parp = 0.0f, aW = 0.0f, bobW = 0.0f, vxW = 0.0f, vzW = 0.0f, vyW = 0.0f, yW = 0.0f, pitW = 0.0f, rolW = 0.0f, cN = 0.0f, sN = 0.0f, sentidoN = 0.0f, aN = 0.0f, eYawN = 0.0f, s2N = 0.0f, yN = 0.0f, pitN = 0.0f, rolN = 0.0f;
	glm::vec3 rel = glm::vec3(0.0f), objetivo = glm::vec3(0.0f), dV = glm::vec3(0.0f), P0 = glm::vec3(0.0f), P2V = glm::vec3(0.0f), P1V = glm::vec3(0.0f), dirV = glm::vec3(0.0f), fA = glm::vec3(0.0f), objCam = glm::vec3(0.0f), eP = glm::vec3(0.0f), pW = glm::vec3(0.0f), ePosN = glm::vec3(0.0f), FN = glm::vec3(0.0f), pN = glm::vec3(0.0f), dirN = glm::vec3(0.0f);
	glm::vec2 hV = glm::vec2(0.0f);
	bool echaV = false;
	glm::mat4 mc = glm::mat4(1.0f), mp = glm::mat4(1.0f), me = glm::mat4(1.0f), mr = glm::mat4(1.0f);

	// variables del ciclo principal: se declaran aqui y adentro solo se les asigna (regla de las practicas)
	const float ESC_AV = 4.5f;	// el modelo mide 1 de alto
	GLfloat now = 0.0f;
	float dt = 0.0f, yaw = 0.0f, dx = 0.0f, dy = 0.0f, difGiro = 0.0f, radioAv = 0.0f, f2 = 0.0f, f3 = 0.0f, alto = 0.0f, u = 0.0f, dif2 = 0.0f, theta = 0.0f, s2 = 0.0f, difusa = 0.0f, ambiental = 0.0f, distancia[12+N_AMBIENTE], radioPlaneta[3], t4 = 0.0f, t5 = 0.0f, giro = 0.0f, fi = 0.0f, t6 = 0.0f, carril = 0.0f, pendiente = 0.0f, tiempo = 0.0f, t7 = 0.0f, radioCuerpo[5], radioOrbita[3], alturaOrbita[3], a4 = 0.0f, t1 = 0.0f, t2 = 0.0f, t8 = 0.0f, y2 = 0.0f, giroBlarg[4], golpe = 0.0f, tam = 0.0f, brinco = 0.0f, a6 = 0.0f, anguloAv[6], vidaP = 0.0f, tamP = 0.0f, crece = 0.0f, largo = 0.0f, margen = 0.0f, ancho = 0.0f, altoMenu[4];
	bool *teclas = nullptr;
	glm::vec3 t3 = glm::vec3(0.0f), centro = glm::vec3(0.0f), frenteCam = glm::vec3(0.0f), adelanteAv = glm::vec3(0.0f), derechaAv = glm::vec3(0.0f), dirAv = glm::vec3(0.0f), centroAv = glm::vec3(0.0f), desdeCentroAv = glm::vec3(0.0f), pa = glm::vec3(0.0f), pb = glm::vec3(0.0f), fa = glm::vec3(0.0f), fb = glm::vec3(0.0f), tb = glm::vec3(0.0f), posN1 = glm::vec3(0.0f), tanN1 = glm::vec3(0.0f), p0 = glm::vec3(0.0f), p1 = glm::vec3(0.0f), p2 = glm::vec3(0.0f), p3 = glm::vec3(0.0f), posNV = glm::vec3(0.0f), tanNV = glm::vec3(0.0f), destPos = glm::vec3(0.0f), destMira = glm::vec3(0.0f), focoAv = glm::vec3(0.0f), lado2 = glm::vec3(0.0f), dirSol = glm::vec3(0.0f), colSol = glm::vec3(0.0f), color = glm::vec3(0.0f), centroPlaneta[3], c3 = glm::vec3(0.0f), c4 = glm::vec3(0.0f), p = glm::vec3(0.0f), posCuerpo[5], posMeteoro[6], cMF = glm::vec3(0.0f), refineria[3], a5 = glm::vec3(0.0f), b3 = glm::vec3(0.0f), d2 = glm::vec3(0.0f), tanque[2], rielA[2], rielC[2], rielB[2], q1 = glm::vec3(0.0f), q2 = glm::vec3(0.0f), d3 = glm::vec3(0.0f), tornillo2[12], caja[8], torreta[2], blarg[4], protopet[3], pedestal = glm::vec3(0.0f), pivoteAv[6], ejeAv[6];
	int modoNuevo = 0, sig = 0, tramo = 0, nCandidatas = 0, mejor = 0, anchoVentana = 0, altoVentana = 0, zona = 0;
	bool caminaAv = false, navesEnMovimiento = false, esDeNoche = false, mutado = false, volando = false;
	float *a3 = nullptr, *b2 = nullptr;
	glm::mat4 mk = glm::mat4(1.0f), vista = glm::mat4(1.0f), model = glm::mat4(1.0f), modelaux = glm::mat4(1.0f), modelaux2 = glm::mat4(1.0f), mastil = glm::mat4(1.0f), viga = glm::mat4(1.0f), bahia = glm::mat4(1.0f), cabeza = glm::mat4(1.0f), brazo = glm::mat4(1.0f), clank = glm::mat4(1.0f), baseAv = glm::mat4(1.0f), orto = glm::mat4(1.0f);
	unsigned int pointLightCount = 0, spotLightCount = 0;
	PointLight candidatas[12+N_AMBIENTE];
	Skybox *sky = nullptr;
	glm::vec2 c2 = glm::vec2(0.0f);
	Texture *texPlaneta[3], *texCuerpo[5];
	Model *piezaAv[6];
	glm::vec3 escalaAv[6];	// escala de cada pieza del avatar (las patas se alargan para tocar el piso)

	// Venomianos (NPCs de Venom): lugar de cada uno (posicion local de Venom), escala y color de piel; los demas valores se calculan en el ciclo
	glm::vec3 baseVen[3] = { glm::vec3(-54.0f, 0.0f, 13.0f), glm::vec3(-56.0f, 0.0f, 0.0f), glm::vec3(-54.0f, 0.0f, -14.0f) };
	float escVen[3] = { 0.9f, 1.4f, 2.0f };
	glm::vec3 colVen[3] = { glm::vec3(0.55f, 0.95f, 0.55f), glm::vec3(0.35f, 0.8f, 0.9f), glm::vec3(0.65f, 0.3f, 0.85f) };
	float radioVen = 0.0f, sgnVen = 1.0f, angVen = 0.0f, rumboVen = 0.0f, saltoVen = 0.0f, cuerpoYVen = 0.0f, faseVen = 0.0f;
	glm::vec3 pVen = glm::vec3(0.0f);
	glm::mat4 mVen = glm::mat4(1.0f), hVen = glm::mat4(1.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		dt = deltaTime / (1.0f + 1.0f / (float)limitFPS);	// segundos reales del cuadro (el deltaTime de la base viene multiplicado por 61)
		if (dt > 0.1f) dt = 0.1f;

		glfwPollEvents();
		teclas = mainWindow.getsKeys();

		// AB3: P (se guarda en Window) arranca el despegue del Pelican; sube, espera arriba y aterriza
		if (mainWindow.getPelicanPresionado() && !pelicanActivo) { pelicanActivo = true; tiempoPelican = 0.0f; }
		if (pelicanActivo) {
			tiempoPelican += dt;
			if (tiempoPelican < 3.0f) alturaPelican = 2.0f * tiempoPelican;
			else if (tiempoPelican < 5.0f) alturaPelican = 6.0f;
			else if (tiempoPelican < 8.0f) alturaPelican = 6.0f - 2.0f * (tiempoPelican - 5.0f);
			else { alturaPelican = 0.0f; pelicanActivo = false; }
		}
		// O (se guarda en Window): mientras patrulla, el Warthog avanza 0.5 rad/s sobre su circulo y la torreta vigila de lado a lado
		if (mainWindow.getwarthogAndando()) {
			anguloWarthog += 0.5f * dt;
			giroRuedas += glm::degrees(radioPatrulla * 0.5f * dt / radioRueda);	// lo que avanza entre el radio de la rueda
			giroTorretaW = 35.0f * sin(1.5f * now);
		}
		// J (se guarda en Window): la pluma sube o baja 60 grados por segundo hasta su tope
		if (mainWindow.getplumaArriba()) anguloPluma = glm::min(anguloPluma + 60.0f * dt, 80.0f);
		else anguloPluma = glm::max(anguloPluma - 60.0f * dt, 0.0f);

		// ---------------- teclas de un solo golpe (se guardan en Window; el main solo pregunta) ----------------
		if (mainWindow.getTeclaGolpe(GLFW_KEY_L)) s1Prendido = !s1Prendido;
		if (mainWindow.getTeclaGolpe(GLFW_KEY_K)) s2Prendido = !s2Prendido;
		if (mainWindow.getTeclaGolpe(GLFW_KEY_X)) velocidadDia = velocidadDia > 1.0f ? 1.0f : 10.0f;
		if (mainWindow.getTeclaGolpe(GLFW_KEY_R) && faseCohete == 0 && !kcActivo) { faseCohete = 1; tCohete = 0.0f; }
		if (mainWindow.getTeclaGolpe(GLFW_KEY_G) && faseTorreta == 0) {
			faseTorreta = 1;
			t3 = C_FORJA + glm::vec3(-11.2f, 0.0f, -28.0f);	// torreta de la izquierda de la plataforma
			giroObjetivo = glm::degrees(atan2(camPos.x - t3.x, camPos.z - t3.z));
		}
		if (mainWindow.getTeclaGolpe(GLFW_KEY_H)) {	// H arranca o detiene el vuelo de KF1
			kfReproduciendo = !kfReproduciendo;
			giroPedestal = 0.0f;	// despega mirando al frente
			if (kfReproduciendo && kfCuadro >= 8) { kfCuadro = 0; i_curr_steps = 0; }
		}
		// B arranca la animacion del cohete por keyframes; otra vez B lo regresa a la plataforma
		if (mainWindow.getTeclaGolpe(GLFW_KEY_B) && (kcActivo || faseCohete == 0)) {
			kcActivo = !kcActivo; kcCuadro = 0; kcPasos = 0; kcAcum = 0.0f;
			if (kcActivo) {
				printf("--- cohete por keyframes: inicia ---\n");
				kcEncabezado(); kcImprimir(0);
				// la camara pasa a la C4, que con el cohete activo mira donde dicen los keyframes
				camPosAntes = camPos; camMiraAntes = camMira; tTransicion = 0.0f; modoCamara = 4;
			}
			else printf("--- cohete de vuelta en la plataforma ---\n");
		}
		// V guarda los keyframes en el txt y los imprime
		if (mainWindow.getTeclaGolpe(GLFW_KEY_V)) kcGuardar();
		if (mainWindow.getTeclaGolpe(GLFW_KEY_M)) menuAbierto = !menuAbierto;
		modoNuevo = modoCamara;
		camPedida = mainWindow.getCamaraPedida();	// teclas 1 a 6: la 5 y la 6 son las camaras del cohete de Reach
		if (camPedida > 0) modoNuevo = camPedida;
		if (modoNuevo != modoCamara) { camPosAntes = camPos; camMiraAntes = camMira; tTransicion = 0.0f; modoCamara = modoNuevo; }
		if (mainWindow.getTeclaGolpe(GLFW_KEY_T) && modoCamara == 1 && !pilotando) {
			// T: el avatar viaja al siguiente planeta
			planetaActual = (planetaActual + 1) % 3;
			centro = planetaActual == 0 ? C_FORJA : planetaActual == 1 ? C_REACH : C_VENOM;
			avatarPos = glm::vec3(llegadas[planetaActual].x, 0.0f, llegadas[planetaActual].z);
			yaw = glm::degrees(atan2(centro.z - avatarPos.z, centro.x - avatarPos.x));
			camera = Camera(camaraLibreAntes ? llegadas[planetaActual] : avatarPos, glm::vec3(0.0f, 1.0f, 0.0f), yaw, -12.0f, 0.8f, 0.5f);	// con la camara libre llega a la altura del punto de llegada
			avatarGiro = glm::radians(90.0f - yaw);
		}

		// C (se guarda en Window): sale del avatar y deja la camara libre donde esta, o regresa al avatar
		if (mainWindow.getcamaraLibre() != camaraLibreAntes) {
			camaraLibreAntes = mainWindow.getcamaraLibre();
			camPosAntes = camPos; camMiraAntes = camMira; tTransicion = 0.0f;	// la camara se desliza al nuevo punto de vista
			if (camaraLibreAntes) {	// la camara libre arranca donde estaba la camara del avatar, mirando hacia donde miraba
				dirLibre = glm::normalize(camMira - camPos);
				yawLibre = glm::degrees(atan2(dirLibre.z, dirLibre.x));
				pitchLibre = glm::degrees(asin(glm::clamp(dirLibre.y, -1.0f, 1.0f)));
				camera = Camera(camPos, glm::vec3(0.0f, 1.0f, 0.0f), yawLibre, pitchLibre, 0.8f, 0.5f);
			}
		}

		// ---- ARWING: coma activa o suelta el control; W/S suben y bajan la nariz, A/D giran ----
		if (mainWindow.getComaPresionada() && modoCamara == 1) {	// la coma (se guarda en Window) toma o suelta el control del Arwing
			pilotando = !pilotando;
			if (pilotando) camChase = camPos;	// la camara se desliza desde donde estaba; al soltarlo vuelve el avatar
		}
		if (modoCamara != 1) pilotando = false;	// al cambiar a otra camara se suelta la nave

		{	// fisica del Arwing: la nave siempre avanza; con la coma la manejas, si no, patrulla un circulo alrededor de Venom
			giroV = 0.0f; pitchObj = 0.0f;	// giro positivo = izquierda
			if (pilotando) {
				giroV = mainWindow.getArwingGiro();	// A y D salen de Window
				pitchObj = 0.55f * mainWindow.getArwingPitch();	// W y S salen de Window
			}
			else {	// autopiloto: persigue un punto adelantado sobre el circulo de patrulla
				rel = posArwing - C_VENOM;
				aAct = atan2(rel.z, rel.x);
				objetivo = C_VENOM + glm::vec3(ARWING_R_PATRULLA * cos(aAct + 0.6f), ARWING_ALT_PATRULLA, ARWING_R_PATRULLA * sin(aAct + 0.6f));
				dV = objetivo - posArwing;
				dif = atan2(dV.x, dV.z) - yawArwing;
				while (dif > PI) dif -= 2.0f * PI;
				while (dif < -PI) dif += 2.0f * PI;
				giroV = glm::clamp(dif * 2.0f, -1.0f, 1.0f);
				pitchObj = glm::clamp((float)atan2(dV.y, sqrt(dV.x * dV.x + dV.z * dV.z)), -0.4f, 0.4f);
			}
			yawArwing += giroV * ARWING_GIRO * dt;
			pitchArwing += (pitchObj - pitchArwing) * (1.0f - exp(-4.0f * dt));
			rollArwing += (-0.7f * giroV - rollArwing) * (1.0f - exp(-5.0f * dt));	// se inclina hacia donde gira
			posArwing += glm::vec3(sin(yawArwing) * cos(pitchArwing), sin(pitchArwing), cos(yawArwing) * cos(pitchArwing)) * ARWING_VEL * dt;
			hV = glm::vec2(posArwing.x - C_VENOM.x, posArwing.z - C_VENOM.z);
			if (glm::length(hV) > ARWING_R_MAX) { hV *= ARWING_R_MAX / glm::length(hV); posArwing.x = C_VENOM.x + hV.x; posArwing.z = C_VENOM.z + hV.y; }
			posArwing.y = glm::clamp(posArwing.y, ARWING_ALT_MIN, ARWING_ALT_MAX);
		}

		if (modoCamara == 1 && !pilotando && camaraLibreAntes) {	// camara libre: WASD y el mouse mueven la camara, el avatar se queda parado donde estaba
			camera.keyControl(teclas, deltaTime);
			dx = mainWindow.getXChange();
			dy = mainWindow.getYChange();
			if (fabs(dx) < 200.0f && fabs(dy) < 200.0f) camera.mouseControl(dx, dy);
			avatarAmp += (0.0f - avatarAmp) * glm::min(1.0f, 8.0f * dt);	// las extremidades se quedan quietas
		}
		else if (modoCamara == 1 && !pilotando) {
			// el mouse gira la camara alrededor del avatar (el primer movimiento a veces trae un salto enorme y deja la camara en NaN; esos se ignoran)
			dx = mainWindow.getXChange();
			dy = mainWindow.getYChange();
			if (fabs(dx) < 200.0f && fabs(dy) < 200.0f) camera.mouseControl(dx, dy);
			// WASD caminan en el plano XZ respecto a hacia donde mira la camara
			frenteCam = camera.getCameraDirection();
			adelanteAv = glm::normalize(glm::vec3(frenteCam.x, 0.0f, frenteCam.z));
			derechaAv = glm::vec3(-adelanteAv.z, 0.0f, adelanteAv.x);
			dirAv = glm::vec3(0.0f);
			dirAv += adelanteAv * mainWindow.getAvatarAdelante();	// W camina hacia donde mira la camara y S hacia atras (Window lo guarda)
			dirAv += derechaAv * mainWindow.getAvatarLado();	// D camina a la derecha y A a la izquierda
			caminaAv = glm::length(dirAv) > 0.01f;
			if (caminaAv) {
				dirAv = glm::normalize(dirAv);
				avatarPos += dirAv * 14.0f * dt;
				difGiro = atan2(dirAv.x, dirAv.z) - avatarGiro;	// gira hacia donde camina por el camino mas corto
				while (difGiro > PI) difGiro -= 2.0f * PI;
				while (difGiro < -PI) difGiro += 2.0f * PI;
				avatarGiro += glm::clamp(difGiro, -9.0f * dt, 9.0f * dt);
			}
			// no se sale de la orilla del planeta donde esta
			centroAv = planetaActual == 0 ? C_FORJA : planetaActual == 1 ? C_REACH : C_VENOM;
			radioAv = (planetaActual == 0 ? R_FORJA : planetaActual == 1 ? R_REACH : R_VENOM) - 4.0f;
			desdeCentroAv = avatarPos - centroAv; desdeCentroAv.y = 0.0f;
			if (glm::length(desdeCentroAv) > radioAv) avatarPos = centroAv + glm::normalize(desdeCentroAv) * radioAv;
			avatarAmp += ((caminaAv ? 1.0f : 0.0f) - avatarAmp) * glm::min(1.0f, 8.0f * dt);
			avatarFase += 7.0f * avatarAmp * dt;
		}
		else { mainWindow.getXChange(); mainWindow.getYChange(); }

		//=====================================================================================
		// ANIMACIONES (se calculan antes de dibujar)
		//=====================================================================================
		// AC2: x = a cos(t), z = b sin(t), con w(t) = w0 + 0.2 sin(t + i) en proporcion para que se rebasen
		// (estuvieron quietas para la revision del 16-oct; ya vimos animacion y se volvieron a activar)
		navesEnMovimiento = true;
		if (navesEnMovimiento) for (int i = 0; i < 3; i++) angNave[i] += (w0Nave[i] + 0.06f * sin(now + i)) * dt;
		// NV: avanza un tramo de la ruta cada 16 s (la ruta es el doble de larga que antes)
		if (navesEnMovimiento) { uNV += dt / 20.0f; if (uNV >= 3.0f) uNV -= 3.0f; }
		giroAnillo += 10.0f * dt;
		// KF1: avanza un paso por cuadro (60 cuadros por segundo) e interpola entre el cuadro actual y el siguiente
		if (kfReproduciendo) {
			kfAcum += dt * 60.0f;	// el ciclo no esta limitado a 60 fps, asi que cuento los pasos con el tiempo real
			while (kfAcum >= 1.0f) { kfAcum -= 1.0f; i_curr_steps++; }
			if (i_curr_steps >= i_max_steps) { i_curr_steps = 0; kfCuadro++; }
			if (kfCuadro >= 8) { kfCuadro = 8; i_curr_steps = 0; kfReproduciendo = false; }	// termina en el ultimo cuadro (que es igual al primero)
		}
		{
			sig = kfCuadro < 8 ? kfCuadro + 1 : 8;
			f2 = (float)i_curr_steps / i_max_steps;
			kfActual = kfPos[kfCuadro] + (kfPos[sig] - kfPos[kfCuadro]) * f2;
			kfGiroActual = kfGiro[kfCuadro] + (kfGiro[sig] - kfGiro[kfCuadro]) * f2;
		}
		// ---- COHETE DE REACH (Practica 10 de 317136421): espacio inicia o pausa, retroceso reinicia, F5 guarda el txt y F6 lo lee de nuevo (las teclas se guardan en Window) ----
		reachTabla = mainWindow.getTeclaGolpe(GLFW_KEY_F6);
		if (reachLeer || reachTabla) {	// al arrancar y con F6 se leen los cuadros del txt; si algo no cuadra se conservan los que ya estaban
			reachFile = fopen(reachArchivo, "r");
			reachBien = reachFile != NULL;
			reachCuantos = 0; reachOrb = 0; reachFilas = 0;
			while (reachBien && fgets(reachLinea, 512, reachFile) != NULL) {
				if (reachLinea[0] == '#' || reachLinea[0] == '\n' || reachLinea[0] == '\r') continue;
				if (reachCuantos == 0) {	// primera linea: cuantos cuadros hay y en cual empieza la orbita
					reachBien = sscanf(reachLinea, "%d %d", &reachCuantos, &reachOrb) == 2 && reachCuantos >= 2 && reachCuantos <= 200 && reachOrb >= 0 && reachOrb < reachCuantos - 1;
					continue;
				}
				// una fila por cuadro: el tiempo y, de cada pieza, X Y Z y el giro en Z
				reachBien = reachFilas < reachCuantos && sscanf(reachLinea, "%f%n", &reachNuevoT[reachFilas], &reachTotal) == 1;
				for (reachPieza = 0; reachBien && reachPieza < 4; reachPieza++) {
					reachBien = sscanf(reachLinea + reachTotal, "%f %f %f %f%n", &reachX, &reachY, &reachZ, &reachG, &reachDesp) == 4;
					reachTotal += reachDesp;
					reachNuevoPos[reachFilas][reachPieza] = glm::vec3(reachX, reachY, reachZ);
					reachNuevoGiro[reachFilas][reachPieza] = reachG;
				}
				if (reachBien) reachBien = reachFilas == 0 ? reachNuevoT[0] == 0.0f : reachNuevoT[reachFilas] > reachNuevoT[reachFilas - 1];	// los tiempos empiezan en 0 y crecen
				reachFilas++;
			}
			if (reachFile != NULL) fclose(reachFile);
			reachBien = reachBien && reachCuantos >= 2 && reachFilas == reachCuantos;
			// el ultimo cuadro debe ser igual al que abre la orbita, para que la capsula no pegue un salto al repetirse
			for (reachPieza = 0; reachBien && reachPieza < 4; reachPieza++)
				reachBien = glm::distance(reachNuevoPos[reachCuantos - 1][reachPieza], reachNuevoPos[reachOrb][reachPieza]) < 0.01f && fabs(reachNuevoGiro[reachCuantos - 1][reachPieza] - reachNuevoGiro[reachOrb][reachPieza]) < 0.01f;
			if (reachBien) {
				reachN = reachCuantos; reachOrbita = reachOrb; reachCuadro = 0; reachReloj = 0.0f; reachPlay = false;
				for (reachFilas = 0; reachFilas < reachN; reachFilas++) {
					reachT[reachFilas] = reachNuevoT[reachFilas];
					for (reachPieza = 0; reachPieza < 4; reachPieza++) {
						reachPos[reachFilas][reachPieza] = reachNuevoPos[reachFilas][reachPieza];
						reachGiro[reachFilas][reachPieza] = reachNuevoGiro[reachFilas][reachPieza];
					}
				}
				for (reachPieza = 0; reachPieza < 4; reachPieza++) { reachPosAct[reachPieza] = reachPos[0][reachPieza]; reachGiroAct[reachPieza] = reachGiro[0][reachPieza]; }
				printf("REACH: se leyeron %d keyframes de %s (la orbita repite desde el cuadro %d)\n", reachN, reachArchivo, reachOrbita + 1);
				if (reachTabla) { reachMsg = "REACH TXT CARGADO"; reachImp = 0; reachImpFin = reachN - 1; }
			}
			else printf("REACH: falta %s o tiene cuadros invalidos; se conserva el cohete anterior\n", reachArchivo);
			reachLeer = false;
		}
		if (mainWindow.getTeclaGolpe(GLFW_KEY_SPACE) && reachN > 0) {	// espacio inicia, pausa o continua
			reachPlay = !reachPlay;
			reachMsg = reachPlay ? "REACH: REPRODUCIENDO KEYFRAMES" : "REACH: EN PAUSA"; reachImp = reachCuadro; reachImpFin = reachCuadro;
		}
		if (mainWindow.getTeclaGolpe(GLFW_KEY_BACKSPACE) && reachN > 0) {	// retroceso regresa al primer cuadro y queda en pausa
			reachReloj = 0.0f; reachCuadro = 0; reachPlay = false;
			for (reachPieza = 0; reachPieza < 4; reachPieza++) { reachPosAct[reachPieza] = reachPos[0][reachPieza]; reachGiroAct[reachPieza] = reachGiro[0][reachPieza]; }
			printf("REACH: cohete reiniciado\n");
		}
		if (mainWindow.getTeclaGolpe(GLFW_KEY_F5) && reachN > 0) {	// F5 guarda los cuadros en el txt
			reachFile = fopen(reachArchivo, "w");
			if (reachFile == NULL) printf("REACH: no se pudo escribir %s\n", reachArchivo);
			else {
				fprintf(reachFile, "# PRACTICA 10 - 317136421 - Saturn V integrado en Reach\n# Cantidad de cuadros e indice de inicio de orbita (desde cero).\n%d %d\n", reachN, reachOrbita);
				fprintf(reachFile, "# tiempo | etapa1 X Y Z giroZ | etapa2 X Y Z giroZ | etapa3 X Y Z giroZ | capsula X Y Z giroZ\n# Segundos, unidades del proyecto y grados. Interpolacion lineal.\n");
				for (reachFilas = 0; reachFilas < reachN; reachFilas++) {
					fprintf(reachFile, "%.3f", reachT[reachFilas]);
					for (reachPieza = 0; reachPieza < 4; reachPieza++)
						fprintf(reachFile, " %.3f %.3f %.3f %.3f", reachPos[reachFilas][reachPieza].x, reachPos[reachFilas][reachPieza].y, reachPos[reachFilas][reachPieza].z, reachGiro[reachFilas][reachPieza]);
					fprintf(reachFile, "\n");
				}
				fclose(reachFile);
				printf("REACH F5: se guardaron %d keyframes en %s\n", reachN, reachArchivo);
			}
		}
		if (reachN > 0 && reachPlay) {	// el reloj avanza en segundos reales y cada pieza se interpola entre el cuadro actual y el siguiente
			reachReloj += dt;
			if (reachReloj >= reachT[reachN - 1]) {	// llego al final: solo la capsula repite su orbita, las etapas se quedan en el piso
				reachReloj = reachT[reachOrbita] + fmod(reachReloj - reachT[reachN - 1], reachT[reachN - 1] - reachT[reachOrbita]);
				reachCuadro = reachOrbita;
				reachMsg = "REACH ORBITA: se repite solo la capsula"; reachImp = reachCuadro; reachImpFin = reachCuadro;
			}
			while (reachCuadro + 1 < reachN && reachReloj >= reachT[reachCuadro + 1]) {
				reachCuadro++;
				reachMsg = "REACH KEYFRAME REPRODUCIDO"; reachImp = reachCuadro; reachImpFin = reachCuadro;
			}
			reachU = (reachReloj - reachT[reachCuadro]) / (reachT[reachCuadro + 1] - reachT[reachCuadro]);
			for (reachPieza = 0; reachPieza < 4; reachPieza++) {
				reachPosAct[reachPieza] = glm::mix(reachPos[reachCuadro][reachPieza], reachPos[reachCuadro + 1][reachPieza], reachU);
				reachGiroAct[reachPieza] = reachGiro[reachCuadro][reachPieza] + reachU * (reachGiro[reachCuadro + 1][reachPieza] - reachGiro[reachCuadro][reachPieza]);
			}
		}
		if (reachImp >= 0) {	// imprime en la consola los cuadros pedidos (del reachImp al reachImpFin)
			for (reachFilas = reachImp; reachFilas <= reachImpFin; reachFilas++) {
				printf("%s | KEYFRAME %d/%d | t = %.2f s\n", reachMsg, reachFilas + 1, reachN, reachT[reachFilas]);
				for (reachPieza = 0; reachPieza < 4; reachPieza++)
					printf("  %s %d: X=%.3f Y=%.3f Z=%.3f giroZ=%.3f\n", reachPieza == 3 ? "Capsula" : "Etapa", reachPieza + 1,
						reachPos[reachFilas][reachPieza].x, reachPos[reachFilas][reachPieza].y, reachPos[reachFilas][reachPieza].z, reachGiro[reachFilas][reachPieza]);
			}
			reachImp = -1;
		}
		// KF-P10: avanza un paso por cuadro (60 por segundo) e interpola linealmente cada etapa
		// entre el cuadro actual y el siguiente
		if (kcActivo) {
			kcAcum += dt * 60.0f;
			while (kcAcum >= 1.0f) { kcAcum -= 1.0f; kcPasos++; }
			if (kcPasos >= kc_max_steps) {
				kcPasos = 0; kcCuadro++;
				// el ultimo cuadro es igual al de la entrada a la orbita: la capsula sigue orbitando
				if (kcCuadro >= kcN - 1) kcCuadro = kcOrbIni;
				kcImprimir(kcCuadro);
			}
			a3 = kcDatos[kcCuadro]; b2 = kcDatos[kcCuadro + 1];
			f3 = (float)kcPasos / kc_max_steps;
			for (int k = 0; k < 3; k++) {
				pa = glm::vec3(a3[3 * k], a3[3 * k + 1], a3[3 * k + 2]);
				pb = glm::vec3(b2[3 * k], b2[3 * k + 1], b2[3 * k + 2]);
				kcPos[k] = PLATAFORMA + pa + (pb - pa) * f3;
				kcGiro[k] = a3[9 + k] + (b2[9 + k] - a3[9 + k]) * f3;
				kcLlama[k] = a3[12 + k] + (b2[12 + k] - a3[12 + k]) * f3;
			}
			fa = glm::vec3(a3[15], a3[16], a3[17]);
			fb = glm::vec3(b2[15], b2[16], b2[17]);
			kcCamFoco = PLATAFORMA + fa + (fb - fa) * f3;
			kcCamDist = a3[18] + (b2[18] - a3[18]) * f3;
			// humo: sale de la tobera de cada etapa encendida
			kcHumoAcum += dt;
			while (kcHumoAcum >= humoCada) {
				kcHumoAcum -= humoCada;
				for (int k = 0; k < 3; k++) {
					if (kcLlama[k] < 0.3f) continue;
					mk = glm::translate(glm::mat4(1.0f), kcPos[k]);
					mk = glm::rotate(mk, glm::radians(kcGiro[k]), glm::vec3(0.0f, 0.0f, 1.0f));
					alto = (toberaEtapa[k] - kcPivote[k]) * kcEscala;
					tb = glm::vec3(mk * glm::vec4(0.0f, alto, 0.0f, 1.0f));
					if (tb.y < PLATAFORMA.y + 0.5f) tb.y = PLATAFORMA.y + 0.5f;
					humoDisp = tb.y < 10.0f ? 6.4f : 1.1f;
					humoPos[humoSig] = tb;
					humoVel[humoSig] = glm::vec3(((rand() % 1000) / 1000.0f - 0.5f) * humoDisp,
						0.4f + (rand() % 1000) / 1000.0f,
						((rand() % 1000) / 1000.0f - 0.5f) * humoDisp);
					humoEdad[humoSig] = 0.0f;
					humoSig = (humoSig + 1) % MAX_HUMO;
				}
			}
		}
		// AB1 cohete
		if (faseCohete == 1) { tCohete += dt; brazoServicio = glm::min(tCohete / 3.0f, 1.0f) * 70.0f; if (tCohete >= 3.0f) { faseCohete = 2; tCohete = 0.0f; } }
		else if (faseCohete == 2) {	// sube con aceleracion constante: y = 1/2 a t^2
			tCohete += dt; posCohete.y = 0.5f * 2.5f * tCohete * tCohete;
			if (posCohete.y >= 60.0f) { posCohete.y = 60.0f; faseCohete = 3; tCohete = 0.0f; }
		}
		else if (faseCohete == 3) {	// se inclina y va hacia la bahia de la estacion
			tCohete += dt; u = glm::min(tCohete / 8.0f, 1.0f);
			posCohete = glm::mix(PLATAFORMA + glm::vec3(0.0f, 60.0f, 0.0f), C_ESTACION + glm::vec3(-8.8f, -5.5f, 8.8f), u);	// a la bahia de la estacion
			inclinaCohete = -60.0f * sin(u * PI);
			if (u >= 1.0f) { faseCohete = 4; tCohete = 0.0f; }
		}
		else if (faseCohete == 4) {	// acoplado: la tercera etapa esta dentro de la bahia; a los 3 s sale de la estacion
			tCohete += dt;
			if (tCohete > 3.0f) { faseCohete = 5; tCohete = 0.0f; }
		}
		else if (faseCohete == 5) {	// sale de la bahia y regresa hacia la plataforma, inclinada hacia el lado contrario que a la ida
			tCohete += dt; u = glm::min(tCohete / 8.0f, 1.0f);
			posCohete = glm::mix(C_ESTACION + glm::vec3(-8.8f, -5.5f, 8.8f), PLATAFORMA + glm::vec3(0.0f, 60.0f, 0.0f), u);
			inclinaCohete = 60.0f * sin(u * PI);
			if (u >= 1.0f) { faseCohete = 6; tCohete = 0.0f; }
		}
		else if (faseCohete == 6) {	// baja frenando sobre la plataforma: y = 60 (1 - u)^2 llega con velocidad cero; ya parada espera 2 s y todo vuelve a empezar
			tCohete += dt; u = glm::min(tCohete / 6.0f, 1.0f);
			posCohete = PLATAFORMA + glm::vec3(0.0f, 60.0f * (1.0f - u) * (1.0f - u), 0.0f);
			inclinaCohete = 0.0f;
			if (tCohete > 8.0f) {
				faseCohete = 0; posCohete = PLATAFORMA; brazoServicio = 0.0f; inclinaCohete = 0.0f;
				sepHecha[0] = false; sepHecha[1] = false;
			}
		}
		// velocidad del cohete: cuanto avanzo en este cuadro; las etapas sueltas salen con ella
		if (dt > 0.0f) velCohete = (posCohete - posCoheteAntes) / dt;
		posCoheteAntes = posCohete;
		// antes de despegar el motor lo hace temblar
		temblorCohete = 0.0f;
		if (faseCohete == 1 && tCohete > 1.0f) temblorCohete = 0.06f * sin(90.0f * now) * (tCohete - 1.0f) / 2.0f;
		cohM = glm::mat4(1.0);
		cohM = glm::translate(cohM, posCohete + glm::vec3(temblorCohete, 0.0f, 0.0f));
		cohM = glm::rotate(cohM, glm::radians(inclinaCohete), glm::vec3(0.0f, 0.0f, 1.0f));
		cohM = glm::translate(cohM, glm::vec3(0.0f, 4.5f * escalaCohete, 0.0f));	// el modelo esta centrado en su mitad, lo subo para que la base quede en el piso
		// separacion de etapas: la 1 al terminar la subida vertical, la 2 al llegar a la estacion
		for (int k = 0; k < 2; k++) {
			if (!sepHecha[k] && ((k == 0 && faseCohete >= 3) || (k == 1 && faseCohete >= 4))) {
				sepHecha[k] = true;
				sepPos[k] = glm::vec3(cohM * glm::vec4(0.0f, sepPivote[k] * escalaCohete, 0.0f, 1.0f));	// centro de la etapa en el mundo
				sepVel[k] = velCohete - 3.0f * glm::vec3(cohM * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));		// sale con la velocidad del cohete y un empujon hacia atras
				sepInc[k] = inclinaCohete;
			}
			if (sepHecha[k]) {	// ya suelta: tiro libre y da vueltas sobre su centro
				sepVel[k].y -= gravedadEtapa * dt;
				sepPos[k] += sepVel[k] * dt;
				sepInc[k] += sepVGiro[k] * dt;
			}
		}
		etapaEncendida = 0;
		if (sepHecha[0]) etapaEncendida = 1;
		if (sepHecha[1]) etapaEncendida = 2;
		// llama: se enciende en los ultimos 2 s de la cuenta regresiva y se apaga al acoplarse; parpadea un poco
		llamaIgn = 0.0f;
		if (faseCohete == 1) llamaIgn = glm::clamp((tCohete - 1.0f) / 1.5f, 0.0f, 1.0f);
		if (faseCohete == 2 || faseCohete == 3 || faseCohete == 5 || (faseCohete == 6 && tCohete < 6.0f)) llamaIgn = 1.0f;
		llamaLong = llamaLargo[etapaEncendida] * llamaIgn * (1.0f + 0.1f * sin(50.0f * now));
		llamaCuadro = (int)(now * 24.0f) % 8;
		// humo: cada humoCada segundos sale una bocanada de la tobera; cerca del suelo se abre hacia los lados
		tobera = glm::vec3(cohM * glm::vec4(0.0f, toberaEtapa[etapaEncendida] * escalaCohete, 0.0f, 1.0f));
		if (tobera.y < PLATAFORMA.y + 0.5f) tobera.y = PLATAFORMA.y + 0.5f;
		if (llamaIgn > 0.0f) humoAcum += dt;
		while (humoAcum >= humoCada) {
			humoAcum -= humoCada;
			humoDisp = posCohete.y < 10.0f ? 6.4f : 1.1f;
			humoPos[humoSig] = tobera;
			humoVel[humoSig] = glm::vec3(((rand() % 1000) / 1000.0f - 0.5f) * humoDisp, 0.4f + (rand() % 1000) / 1000.0f, ((rand() % 1000) / 1000.0f - 0.5f) * humoDisp);
			humoEdad[humoSig] = 0.0f;
			humoSig = (humoSig + 1) % MAX_HUMO;
		}
		// cada bocanada envejece, avanza y se frena un poco; al llegar a humoVida ya no se dibuja
		for (int i = 0; i < MAX_HUMO; i++) {
			if (humoEdad[i] < humoVida) {
				humoEdad[i] += dt;
				humoPos[i] += humoVel[i] * dt;
				humoVel[i] *= (1.0f - 0.8f * dt);
			}
		}
		// ---- COHETE DE VENOM (Practica 10 de 318337872): F inicia o regresa el cohete a la cima del domo, F7 guarda el txt y F8 lo lee de nuevo (las teclas se guardan en Window) ----
		vkTabla = mainWindow.getTeclaGolpe(GLFW_KEY_F8);
		if (vkLeer || vkTabla) {	// al arrancar y con F8 se leen los cuadros del txt; si algo no cuadra se conservan los que ya estaban
			vkFile = fopen(vkArchivo, "r");
			vkBien = vkFile != NULL;
			vkCuantos = 0; vkOrb = 0; vkFilas = 0;
			while (vkBien && fgets(vkLinea, 512, vkFile) != NULL) {
				if (vkLinea[0] == '#' || vkLinea[0] == '\n' || vkLinea[0] == '\r') continue;
				if (vkCuantos == 0) {	// primera linea: cuantos cuadros hay y en cual empieza la orbita
					vkBien = sscanf(vkLinea, "%d %d", &vkCuantos, &vkOrb) == 2 && vkCuantos >= 2 && vkCuantos <= 200 && vkOrb >= 0 && vkOrb < vkCuantos - 1;
					continue;
				}
				vkBien = vkFilas < vkCuantos;
				vkTotal = 0;
				for (vkCol = 0; vkBien && vkCol < 19; vkCol++) {	// una fila por cuadro: sus 19 valores
					vkBien = sscanf(vkLinea + vkTotal, "%f%n", &vkVal, &vkDesp) == 1;
					vkTotal += vkDesp;
					vkNuevo[vkFilas][vkCol] = vkVal;
				}
				if (vkBien) vkBien = vkFilas == 0 ? vkNuevo[0][0] == 0.0f : vkNuevo[vkFilas][0] > vkNuevo[vkFilas - 1][0];	// los tiempos empiezan en 0 y crecen
				vkFilas++;
			}
			if (vkFile != NULL) fclose(vkFile);
			vkBien = vkBien && vkCuantos >= 2 && vkFilas == vkCuantos;
			// el ultimo cuadro debe ser igual al que abre la orbita (el rumbo puede diferir una vuelta completa) para que el cohete no pegue un salto al repetirse
			for (vkCol = 1; vkBien && vkCol < 5; vkCol++) vkBien = fabs(vkNuevo[vkCuantos - 1][vkCol] - vkNuevo[vkOrb][vkCol]) < 0.01f;
			if (vkBien) vkBien = fabs(fmod(vkNuevo[vkCuantos - 1][5] - vkNuevo[vkOrb][5], 360.0f)) < 0.01f || fabs(fmod(vkNuevo[vkCuantos - 1][5] - vkNuevo[vkOrb][5], 360.0f)) > 359.99f;
			if (vkBien) {
				vkN = vkCuantos; vkOrbita = vkOrb; vkCuadro = 0; vkReloj = 0.0f; vkPlay = false;
				for (vkFilas = 0; vkFilas < vkN; vkFilas++) for (vkCol = 0; vkCol < 19; vkCol++) vkD[vkFilas][vkCol] = vkNuevo[vkFilas][vkCol];
				for (vkCol = 0; vkCol < 19; vkCol++) vkAct[vkCol] = vkD[0][vkCol];
				printf("VENOM: se leyeron %d keyframes de %s (la orbita repite desde el cuadro %d)\n", vkN, vkArchivo, vkOrbita + 1);
				if (vkTabla) { vkMsg = "VENOM TXT CARGADO"; vkImp = 0; vkImpFin = vkN - 1; }
			}
			else printf("VENOM: falta %s o tiene cuadros invalidos; se conserva el cohete anterior\n", vkArchivo);
			vkLeer = false;
		}
		if (mainWindow.getCoheteVenomPresionado() && vkN > 0) {	// F: despega; si ya esta en marcha, regresa a la cima del domo y espera
			if (!vkPlay) { vkPlay = true; vkMsg = "VENOM: REPRODUCIENDO KEYFRAMES"; vkImp = vkCuadro; vkImpFin = vkCuadro; }
			else {
				vkReloj = 0.0f; vkCuadro = 0; vkPlay = false;
				for (vkCol = 0; vkCol < 19; vkCol++) vkAct[vkCol] = vkD[0][vkCol];
				for (vkFilas = 0; vkFilas < MAX_PLASMA; vkFilas++) plasmaEdad[vkFilas] = 1.0f;
				printf("VENOM: cohete de vuelta en la cima del domo\n");
			}
		}
		if (mainWindow.getTeclaGolpe(GLFW_KEY_F7) && vkN > 0) {	// F7 guarda los cuadros en el txt
			vkFile = fopen(vkArchivo, "w");
			if (vkFile == NULL) printf("VENOM: no se pudo escribir %s\n", vkArchivo);
			else {
				fprintf(vkFile, "# PRACTICA 10 - 318337872 - cohete de Venom por keyframes\n# Cantidad de cuadros e indice de inicio de orbita (desde cero).\n%d %d\n", vkN, vkOrbita);
				fprintf(vkFile, "# tiempo | cuerpo: X Y Z inclinacion rumbo apertura-domo temblor plasma | propulsores: suelto X Y Z giro | cuerpo inferior: suelto X Y Z giro\n");
				fprintf(vkFile, "# Segundos, unidades del proyecto y grados. Interpolacion lineal; suelto y plasma valen 0 o 1 y no se interpolan.\n");
				for (vkFilas = 0; vkFilas < vkN; vkFilas++) {
					for (vkCol = 0; vkCol < 19; vkCol++) fprintf(vkFile, vkCol == 0 ? "%.3f" : " %.3f", vkD[vkFilas][vkCol]);
					fprintf(vkFile, "\n");
				}
				fclose(vkFile);
				printf("VENOM F7: se guardaron %d keyframes en %s\n", vkN, vkArchivo);
			}
		}
		if (vkN > 0 && vkPlay) {	// el reloj avanza en segundos reales y cada valor se interpola entre el cuadro actual y el siguiente
			vkReloj += dt;
			if (vkReloj >= vkD[vkN - 1][0]) {	// llego al final: solo se repite la orbita de la capsula, lo demas ya quedo en el piso
				vkReloj = vkD[vkOrbita][0] + fmod(vkReloj - vkD[vkN - 1][0], vkD[vkN - 1][0] - vkD[vkOrbita][0]);
				vkCuadro = vkOrbita;
				vkMsg = "VENOM ORBITA: se repite solo la capsula"; vkImp = vkCuadro; vkImpFin = vkCuadro;
			}
			while (vkCuadro + 1 < vkN && vkReloj >= vkD[vkCuadro + 1][0]) {
				vkCuadro++;
				vkMsg = "VENOM KEYFRAME REPRODUCIDO"; vkImp = vkCuadro; vkImpFin = vkCuadro;
			}
			vkU = (vkReloj - vkD[vkCuadro][0]) / (vkD[vkCuadro + 1][0] - vkD[vkCuadro][0]);
			for (vkCol = 0; vkCol < 19; vkCol++) vkAct[vkCol] = vkD[vkCuadro][vkCol] + vkU * (vkD[vkCuadro + 1][vkCol] - vkD[vkCuadro][vkCol]);
		}
		if (vkImp >= 0) {	// imprime en la consola los cuadros pedidos (del vkImp al vkImpFin)
			for (vkFilas = vkImp; vkFilas <= vkImpFin; vkFilas++) {
				printf("%s | KEYFRAME %d/%d | t = %.2f s\n", vkMsg, vkFilas + 1, vkN, vkD[vkFilas][0]);
				printf("  Cohete: X=%.3f Y=%.3f Z=%.3f inclinacion=%.2f rumbo=%.2f domo=%.2f\n", vkD[vkFilas][1], vkD[vkFilas][2], vkD[vkFilas][3], vkD[vkFilas][4], vkD[vkFilas][5], vkD[vkFilas][6]);
				for (vkCol = 0; vkCol < 2; vkCol++)
					printf("  %s: suelto=%.0f X=%.3f Y=%.3f Z=%.3f giroZ=%.3f\n", vkCol == 0 ? "Propulsores" : "Cuerpo inferior", vkD[vkFilas][9 + 5 * vkCol], vkD[vkFilas][10 + 5 * vkCol],
						vkD[vkFilas][11 + 5 * vkCol], vkD[vkFilas][12 + 5 * vkCol], vkD[vkFilas][13 + 5 * vkCol]);
			}
			vkImp = -1;
		}
		// lo que dicen los keyframes pasa a las variables que usan el domo, el plasma y el dibujo del cohete
		if (vkN > 0) {
			posCoheteV = glm::vec3(vkAct[1], vkAct[2], vkAct[3]);
			inclinaV = glm::radians(vkAct[4]);
			rumboV = glm::radians(vkAct[5]);
			aperturaDomoV = vkAct[6];
			temblorV = vkAct[7] * 0.06f * sin(90.0f * now);	// antes de despegar el motor lo hace temblar
			for (vkCol = 0; vkCol < 2; vkCol++) {
				sepHechaV[vkCol] = vkD[vkCuadro][9 + 5 * vkCol] > 0.5f;
				sepPosV[vkCol] = glm::vec3(vkAct[10 + 5 * vkCol], vkAct[11 + 5 * vkCol], vkAct[12 + 5 * vkCol]);
				sepIncV[vkCol] = vkAct[13 + 5 * vkCol];
			}
		}
		cohMV = glm::mat4(1.0);
		cohMV = glm::translate(cohMV, posCoheteV + glm::vec3(temblorV, 0.0f, 0.0f));
		cohMV = glm::rotate(cohMV, rumboV, glm::vec3(0.0f, 1.0f, 0.0f));
		cohMV = glm::rotate(cohMV, inclinaV, glm::vec3(1.0f, 0.0f, 0.0f));

		// plasma azul: se enciende 1 s antes del despegue (lo dice el keyframe); cada plasmaCada segundos sale una particula de cada tobera que sigue activa
		plasmaIgn = (vkN > 0 && vkD[vkCuadro][8] > 0.5f) ? 1.0f : 0.0f;
		if (plasmaIgn > 0.0f) plasmaAcum += dt;
		while (plasmaAcum >= plasmaCada) {
			plasmaAcum -= plasmaCada;
			for (int n = 0; n < N_TOBERAS_V; n++) {
				// los propulsores y el cuerpo inferior echan plasma mientras siguen pegados; la capsula, cuando ya va sola
				echaV = (n < 6 && !sepHechaV[0]) || (n == 6 && !sepHechaV[1]) || (n == 7 && sepHechaV[1]);
				if (!echaV) continue;
				plasmaPos[plasmaSig] = glm::vec3(cohMV * glm::vec4(toberaV[n] * escalaCoheteV, 1.0f));
				plasmaInc[plasmaSig] = inclinaV;
				plasmaRumbo[plasmaSig] = rumboV;
				plasmaTam[plasmaSig] = 2.0f * toberaRadioV[n] * 1.2f * escalaCoheteV;
				plasmaEdad[plasmaSig] = 0.0f;
				plasmaSig = (plasmaSig + 1) % MAX_PLASMA;
			}
		}
		for (int i = 0; i < MAX_PLASMA; i++) if (plasmaEdad[i] < plasmaVida) plasmaEdad[i] += dt;	// cada particula envejece; al llegar a plasmaVida ya no se dibuja


		// AB2 torreta
		if (faseTorreta == 1) {
			dif2 = giroObjetivo - giroTorre;
			giroTorre += glm::clamp(dif2, -90.0f * dt, 90.0f * dt);
			angCanon = glm::min(angCanon + 20.0f * dt, 15.0f);
			if (fabs(dif2) < 1.0f && angCanon >= 15.0f) {
				faseTorreta = 2; tBala = 0.0f; retroceso = 0.3f;
				posBala = C_FORJA + glm::vec3(-11.2f, 2.2f, -28.0f);
				dirBala = glm::vec3(sin(glm::radians(giroTorre)), -sin(glm::radians(angCanon)) * 0.3f, cos(glm::radians(giroTorre)));
			}
		}
		else if (faseTorreta == 2) {
			tBala += dt; posBala += dirBala * 25.0f * dt; retroceso = glm::max(retroceso - 0.6f * dt, 0.0f);
			if (tBala > 1.5f) { faseTorreta = 3; }
		}
		else if (faseTorreta == 3) {	// regresa a su posicion
			angCanon = glm::max(angCanon - 20.0f * dt, 0.0f);
			if (angCanon <= 0.0f) faseTorreta = 0;
		}

		// ---------------- posicion de las naves para las camaras ----------------
		posN1 = glm::vec3(PISTA_A * cos(angNave[0]), (16.0f + 3.5f * sin(2.0f * angNave[0])) + 0.9f, PISTA_Z + PISTA_B * sin(angNave[0]));
		tanN1 = glm::normalize(glm::vec3(-PISTA_A * sin(angNave[0]), 7.0f * cos(2.0f * angNave[0]), PISTA_B * cos(angNave[0])));	// derivada de la curva (tambien en y porque la pista sube y baja)
		tramo = (int)uNV; u = uNV - tramo;
		// Catmull-Rom con los 3 puntos del circuito: p0 anterior, p1 y p2 del tramo, p3 siguiente (con 3 puntos p0 y p3 son el mismo)
		p0 = rutaNV[(tramo + 2) % 3];
		p1 = rutaNV[tramo];
		p2 = rutaNV[(tramo + 1) % 3];
		p3 = rutaNV[(tramo + 2) % 3];
		posNV = 0.5f * ((2.0f * p1) + (-p0 + p2) * u + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * u * u + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * u * u * u);
		tanNV = glm::normalize(0.5f * ((-p0 + p2) + 2.0f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * u + 3.0f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * u * u));

		// ---------------- camara activa ----------------
		if (pilotando) {	// camara de persecucion: detras y arriba del Arwing, suavizada
			fA = glm::vec3(sin(yawArwing) * cos(pitchArwing), sin(pitchArwing), cos(yawArwing) * cos(pitchArwing));
			objCam = posArwing - fA * 10.0f + glm::vec3(0.0f, 3.5f, 0.0f);
			if (glm::distance(camChase, objCam) > 60.0f) camChase = objCam;
			camChase += (objCam - camChase) * (1.0f - exp(-6.0f * dt));
			if (camChase.y < 1.5f) camChase.y = 1.5f;
			destPos = camChase; destMira = posArwing + fA * 8.0f;
		}
		else if (modoCamara == 1 && camaraLibreAntes) { destPos = camera.getCameraPosition(); destMira = destPos + camera.getCameraDirection(); }	// camara libre
		else if (modoCamara == 1) {	// tercera persona: detras del avatar, mirando a su cabeza
			focoAv = avatarPos + glm::vec3(0.0f, 3.0f, 0.0f);
			destPos = focoAv - camera.getCameraDirection() * 14.0f; destMira = focoAv;
			if (destPos.y < 1.0f) destPos.y = 1.0f;
		}
		else if (modoCamara == 2) { destPos = posN1 - 7.0f * tanN1 + glm::vec3(0.0f, 2.5f, 0.0f); destMira = posN1 + tanN1; }
		else if (modoCamara == 3) { destPos = posNV - 10.0f * tanNV + glm::vec3(0.0f, 2.5f, 0.0f); destMira = posNV + tanNV; }
		else if (modoCamara == 5) {	// C5: sigue al cohete de Reach; primero el Saturn V entero y luego la capsula cuando queda orbitando
			destMira = reachPosAct[3] - glm::vec3(0.0f, 3.0f, 0.0f);
			destPos = destMira + glm::vec3(22.0f, 12.0f, 30.0f);
		}
		else if (modoCamara == 6) {	// C6: vista fija de Reach para ver como caen las etapas
			destMira = glm::vec3(144.0f, 3.0f, -68.0f);
			destPos = glm::vec3(178.0f, 16.0f, -40.0f);
		}
		else if (kcActivo) {	// C4 con el cohete por keyframes: mira al foco del cuadro
			lado2 = glm::normalize(glm::vec3(0.3f, 0.25f, 1.0f));	// desde el frente y un poco arriba
			destMira = kcCamFoco; destPos = kcCamFoco + lado2 * kcCamDist;
		}
		else {	// C4: orbita el punto entre la plataforma y la estacion
			anguloC4 += 0.06f * dt;
			centro = glm::vec3(-63.0f, 39.0f, -81.0f);
			destPos = centro + glm::vec3(100.0f * cos(anguloC4), 20.0f, 100.0f * sin(anguloC4)); destMira = centro;
		}
		if (tTransicion < 1.0f) {	// cambio suave de camara en 0.5 s
			tTransicion = glm::min(tTransicion + dt / 0.5f, 1.0f);
			camPos = glm::mix(camPosAntes, destPos, tTransicion); camMira = glm::mix(camMiraAntes, destMira, tTransicion);
		}
		else { camPos = destPos; camMira = destMira; }
		vista = glm::lookAt(camPos, camMira, glm::vec3(0.0f, 1.0f, 0.0f));

		// ---------------- sol: ciclo de 120 s ----------------
		tiempoDia += dt * velocidadDia;
		theta = 2.0f * PI * fmod(tiempoDia, 120.0f) / 120.0f;
		s2 = sin(theta);
		dirSol = glm::normalize(glm::vec3(-cos(theta), -s2, -0.3f));
		if (s2 > 0.0f) colSol = glm::mix(glm::vec3(1.0f, 0.6f, 0.3f), glm::vec3(1.0f, 1.0f, 0.95f), glm::min(s2 * 2.0f, 1.0f));	// amanecer -> mediodia
		else colSol = glm::vec3(0.45f, 0.55f, 0.9f);	// noche: luz de luna azulada
		difusa = glm::max(0.0f, s2) * 0.8f;
		if (s2 < 0.0f) difusa = 0.6f;	// de noche queda la luna para que se siga viendo el escenario
		ambiental = 0.3f + 0.05f * glm::max(0.0f, s2);
		if (s2 < 0.0f) dirSol = glm::vec3(0.0f, -1.0f, 0.0f);	// de noche la luna cae desde arriba
		mainLight = DirectionalLight(colSol.x, colSol.y, colSol.z, ambiental, difusa, dirSol.x, dirSol.y, dirSol.z);
		esDeNoche = s2 < 0.0f;

		// luminarias: todas a la vez segun el ciclo
		// el shader solo recibe 8 luces puntuales, asi que de todas las candidatas mando las 8 mas cercanas a la camara
		pointLightCount = 0;
		  nCandidatas = 0;
		for (int i = 0; i < N_AMBIENTE; i++) { candidatas[nCandidatas] = lucesAmbiente[i]; distancia[nCandidatas++] = glm::distance(camPos, posAmbiente[i]); }
		if (esDeNoche) for (int i = 0; i < 12; i++) { candidatas[nCandidatas] = luminarias[i]; distancia[nCandidatas++] = glm::distance(camPos, posLuminaria[i]); }
		for (int k = 0; k < nCandidatas && pointLightCount < MAX_POINT_LIGHTS; k++) {	// seleccion: cada vuelta toma la mas cercana que falte
			mejor = -1;
			for (int i = 0; i < nCandidatas; i++) if (distancia[i] >= 0.0f && (mejor < 0 || distancia[i] < distancia[mejor])) mejor = i;
			pointLights[pointLightCount++] = candidatas[mejor]; distancia[mejor] = -1.0f;
		}
		// spotlights: solo se mandan los que estan prendidos
		spotLightCount = 0;
		if (s1Prendido) { reflectorS1.SetPos(glm::vec3(PLATAFORMA.x, 18.0f, PLATAFORMA.z - 7.0f)); spotLights[spotLightCount++] = reflectorS1; }
		if (s2Prendido) spotLights[spotLightCount++] = estacionS2;

		// ---------------- skybox segun la zona ----------------
		sky = &skyEspacio;
		if (modoCamara == 1 || modoCamara >= 5) {	// la camara libre y las del cohete de Reach cambian de skybox segun el planeta donde estan
			c2 = glm::vec2(camPos.x, camPos.z);
			if (glm::distance(c2, glm::vec2(C_FORJA.x, C_FORJA.z)) < R_FORJA && camPos.y < 30.0f) sky = &skyForja;
			else if (glm::distance(c2, glm::vec2(C_REACH.x, C_REACH.z)) < R_REACH && camPos.y < 30.0f) sky = &skyReach;
			else if (glm::distance(c2, glm::vec2(C_VENOM.x, C_VENOM.z)) < R_VENOM && camPos.y < 30.0f) sky = &skyVenom;
		}

		// si se maximiza la ventana el dibujo se quedaba en una esquina: ajusto el viewport y la proyeccion al tamaño real
		glfwGetFramebufferSize(glfwGetCurrentContext(), &anchoVentana, &altoVentana);
		if (anchoVentana > 0 && altoVentana > 0) {
			glViewport(0, 0, anchoVentana, altoVentana);
			projection = glm::perspective(45.0f, (GLfloat)anchoVentana / altoVentana, 0.1f, 1000.0f);
		}
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sky->DrawSkybox(vista, projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(vista));
		glUniform3f(uniformEyePosition, camPos.x, camPos.y, camPos.z);

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		modelaux2 = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);


		//=====================================================================================
		// 4. ESCENARIO COMUN
		//=====================================================================================
		// ---- planetas: disco caminable en y = 0 y hemisferio de roca abajo ----
		centroPlaneta[0] = C_FORJA;
		centroPlaneta[1] = C_REACH;
		centroPlaneta[2] = C_VENOM;
		radioPlaneta[0] = R_FORJA;
		radioPlaneta[1] = R_REACH;
		radioPlaneta[2] = R_VENOM;
		texPlaneta[0] = &forjaTexture;
		texPlaneta[1] = &reachTexture;
		texPlaneta[2] = &venomTexture;
		for (int i = 0; i < 3; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, centroPlaneta[i]);
			model = glm::scale(model, glm::vec3(radioPlaneta[i], 1.0f, radioPlaneta[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			texPlaneta[i]->UseTexture();
			roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[DISCO]->RenderMeshModel();

			model = glm::mat4(1.0);
			model = glm::translate(model, centroPlaneta[i] - glm::vec3(0.0f, 0.02f, 0.0f));
			model = glm::scale(model, glm::vec3(radioPlaneta[i], radioPlaneta[i] * 0.7f, radioPlaneta[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = i == 0 ? glm::vec3(0.55f, 0.5f, 0.45f) : i == 1 ? glm::vec3(0.6f, 0.6f, 0.6f) : glm::vec3(0.6f, 0.4f, 0.3f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			rocaTexture.UseTexture();
			meshListModel[HEMISFERIO]->RenderMeshModel();
		}

		// ---- E2 pista de carreras: flota entre los planetas, sube y baja, con muros de luz y anillos de energia ----
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, PISTA_Z));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pistaTexture.UseTexture();
		metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[PISTA]->RenderMeshModel();
		// muros de las orillas: la franja de luz late despacio (color arriba de 1 para que brille aunque sea de noche)
		color = glm::vec3(1.0f, 1.0f, 1.0f) * (1.3f + 0.3f * sin(3.0f * now));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		neonTexture.UseTexture();
		meshListModel[MURO_PISTA]->RenderMeshModel();
		// soportes: 20 columnas delgadas debajo de la pista con un emisor de energia en la punta
		for (int i = 0; i < 20; i++) {
			t4 = i * 2.0f * PI / 20.0f;
			c3 = glm::vec3(PISTA_A * cos(t4), (16.0f + 3.5f * sin(2.0f * t4)), PISTA_Z + PISTA_B * sin(t4));
			model = glm::mat4(1.0);
			model = glm::translate(model, c3 - glm::vec3(0.0f, 7.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.6f, 6.7f, 0.6f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.35f, 0.38f, 0.45f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
			model = glm::mat4(1.0);
			model = glm::translate(model, c3 - glm::vec3(0.0f, 7.3f, 0.0f));
			model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f) * (1.0f + 0.15f * sin(4.0f * now + i)));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = i % 2 ? glm::vec3(0.4f, 2.2f, 2.6f) : glm::vec3(2.4f, 0.5f, 1.8f);	// cian y magenta como la nebulosa
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
		}
		// 4 anillos de control: 20 segmentos alrededor de la pista que giran sobre la tangente
		for (int i = 0; i < 4; i++) {
			t5 = i * PI / 2.0f + 0.4f;
			c4 = glm::vec3(PISTA_A * cos(t5), (16.0f + 3.5f * sin(2.0f * t5)) + 1.0f, PISTA_Z + PISTA_B * sin(t5));
			giro = glm::degrees(atan2(-PISTA_A * sin(t5), PISTA_B * cos(t5)));	// el eje Z local queda sobre la tangente
			for (int k = 0; k < 20; k++) {
				fi = k * 360.0f / 20.0f + now * 25.0f * (i % 2 ? 1.0f : -1.0f);
				model = glm::mat4(1.0);
				model = glm::translate(model, c4);
				model = glm::rotate(model, glm::radians(giro), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, glm::radians(fi), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::translate(model, glm::vec3(0.0f, 8.0f, 0.0f));
				model = glm::scale(model, glm::vec3(2.6f, 0.5f, 0.8f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = k % 2 ? glm::vec3(0.3f, 0.32f, 0.38f) : (i % 2 ? glm::vec3(0.4f, 2.0f, 2.4f) : glm::vec3(2.2f, 0.45f, 1.6f));
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				plainTexture.UseTexture();
				metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshListModel[CUBO]->RenderMeshModel();
			}
		}
		// ---- AC2 naves N1, N2, N3 en la pista ----
		for (int i = 0; i < 3; i++) {
			t6 = angNave[i];
			carril = (i - 1) * 3.0f;	// cada una en su carril
			p = glm::vec3((PISTA_A + carril) * cos(t6), (16.0f + 3.5f * sin(2.0f * t6)) + 0.9f + 0.15f * sin(6.0f * now + i), PISTA_Z + (PISTA_B + carril) * sin(t6));	// flotan un poco
			giro = glm::degrees(atan2(-PISTA_A * sin(t6), PISTA_B * cos(t6)));	// orientada con la tangente
			pendiente = glm::degrees(atan2(7.0f * cos(2.0f * t6), glm::length(glm::vec2(PISTA_A * sin(t6), PISTA_B * cos(t6)))));	// sube y baja con la pista
			model = glm::mat4(1.0);
			model = glm::translate(model, p);
			model = glm::rotate(model, glm::radians(giro), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-pendiente), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-12.0f), glm::vec3(0.0f, 0.0f, 1.0f));	// se inclina hacia adentro de la curva
			// N1 es el hoverbike de Going Commando (mide 6.2 de largo sobre X), N2 el Sabre de Halo Reach y N3 una nave de Kenney (Space Kit, CC0)
			if (i == 0) {
				model = glm::rotate(model, glm::radians(giroBaseHoverbike), glm::vec3(0.0f, 1.0f, 0.0f));	// su largo va en X, lo paso a Z
				model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));
				model = glm::translate(model, glm::vec3(0.1924f, 0.5249f, 0.0f));	// el OBJ esta centrado en el origen: se regresa a su base
			}
			else if (i == 2) {
				model = glm::rotate(model, glm::radians(giroBaseCatspaw), glm::vec3(0.0f, 1.0f, 0.0f));	// por si la nariz no queda sobre la tangente
				model = glm::scale(model, glm::vec3(2.6f, 2.6f, 2.6f));	// la Catspaw mide ~1.6 de ancho, se agranda para ir a escala con las otras
				model = glm::translate(model, glm::vec3(0.0f, 0.3f, 0.0f));	// el OBJ de esta nave esta centrado en el origen: se regresa a su base
			}
			else if (i == 1) {
				model = glm::translate(model, glm::vec3(0.0f, 1.344f, 0.0f));	// el OBJ esta centrado: se sube su mitad de alto (0.56 x 2.4) para que vaya sobre la pista como antes
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// el Sabre tiene la punta en +X y la pista usa +Z como frente
				model = glm::scale(model, glm::vec3(2.4f, 2.4f, 2.4f));
			}
			else model = glm::scale(model, glm::vec3(2.4f, 2.4f, 2.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = i <= 1 ? glm::vec3(1.0f, 1.0f, 1.0f) : colorNave[i];	// el hoverbike y el Sabre traen su textura
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
			if (i == 0) hoverbike.RenderModel();
			else if (i == 1) naveSabre.RenderModel();
			else naveSpeederB.RenderModel();
		}
		// ---- la Catspaw corre en la pista de carreras (carril de afuera, nariz sobre la tangente, sube y baja con la pista) ----
		{
			tiempo = (float)glfwGetTime();
			t7 = 4.2f + 0.26f * tiempo - 0.06f * cos(tiempo);
			carril = 3.0f;	// angulo sobre la elipse: avanza 0.26 rad/s con una pequena variacion
			p = glm::vec3((PISTA_A + carril) * cos(t7), (16.0f + 3.5f * sin(2.0f * t7)) + 0.9f + 0.15f * sin(6.0f * tiempo), PISTA_Z + (PISTA_B + carril) * sin(t7));
			giro = glm::degrees(atan2(-PISTA_A * sin(t7), PISTA_B * cos(t7)));
			pendiente = glm::degrees(atan2(7.0f * cos(2.0f * t7), glm::length(glm::vec2(PISTA_A * sin(t7), PISTA_B * cos(t7)))));
			model = glm::mat4(1.0);
			model = glm::translate(model, p);
			model = glm::rotate(model, glm::radians(giro), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-pendiente), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-12.0f), glm::vec3(0.0f, 0.0f, 1.0f));	// se inclina hacia adentro de la curva
			model = glm::rotate(model, glm::radians(giroBaseCatspaw), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(2.6f, 2.6f, 2.6f));	// la Catspaw mide ~1.6 de ancho
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);	// trae su textura
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
			catspawModel.RenderModel();
		}

		// ---- E1 estacion espacial (jerarquia: nucleo -> anillo -> brazos; nucleo -> mastil -> alas solares; nucleo -> bahias) ----
		model = glm::mat4(1.0);
		model = glm::translate(model, C_ESTACION);
		model = glm::scale(model, glm::vec3(2.2f, 2.2f, 2.2f));	// crecio junto con los planetas
		modelaux = model;
		model = glm::scale(model, glm::vec3(2.2f, 2.2f, 2.2f));	// nucleo
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.82f, 0.88f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalTexture.UseTexture();
		metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[ESFERA]->RenderMeshModel();
		model = modelaux;	// franja de ventanas alrededor del nucleo
		model = glm::translate(model, glm::vec3(0.0f, -0.35f, 0.0f));
		model = glm::scale(model, glm::vec3(4.7f, 0.7f, 4.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.3f, 1.3f, 1.3f);	// las ventanas se ven prendidas
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		ventanasTexture.UseTexture();
		meshListModel[CILINDRO]->RenderMeshModel();
		model = modelaux;	// cono de abajo
		model = glm::translate(model, glm::vec3(0.0f, -1.6f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.6f, 2.4f, 2.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.6f, 0.62f, 0.68f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalTexture.UseTexture();
		meshListModel[CONO]->RenderMeshModel();
		// mastil de arriba con la antena y su luz roja que parpadea
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		mastil = model;
		model = glm::scale(model, glm::vec3(0.35f, 4.5f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshListModel[CILINDRO]->RenderMeshModel();
		model = mastil;
		model = glm::translate(model, glm::vec3(0.0f, 4.6f, 0.0f));
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = fmod(now, 1.2f) < 0.4f ? glm::vec3(3.0f, 0.3f, 0.2f) : glm::vec3(0.4f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		meshListModel[ESFERA]->RenderMeshModel();
		// alas solares: viga a lo largo de X sobre el anillo con 3 paneles de cada lado
		model = mastil;
		model = glm::translate(model, glm::vec3(0.0f, 2.8f, 0.0f));
		viga = model;
		model = glm::scale(model, glm::vec3(30.0f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.55f, 0.58f, 0.62f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalTexture.UseTexture();
		meshListModel[CUBO]->RenderMeshModel();
		for (int lado = -1; lado <= 1; lado += 2)
			for (int k = 0; k < 3; k++) {
				model = viga;
				model = glm::translate(model, glm::vec3(lado * (6.0f + k * 3.3f), 0.0f, 0.0f));
				model = glm::rotate(model, glm::radians(20.0f * sin(0.2f * now)), glm::vec3(1.0f, 0.0f, 0.0f));	// siguen al sol despacio
				model = glm::scale(model, glm::vec3(3.0f, 0.08f, 4.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(1.0f, 1.0f, 1.0f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				solarTexture.UseTexture();
				metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshListModel[CUBO]->RenderMeshModel();
			}
		metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
		model = modelaux;
		model = glm::rotate(model, glm::radians(giroAnillo), glm::vec3(0.0f, 1.0f, 0.0f));	// el anillo gira con todo y brazos
		modelaux2 = model;
		for (int i = 0; i < 24; i++) {	// anillo hecho de 24 bloques con ventanas
			model = modelaux2;
			model = glm::rotate(model, glm::radians(i * 15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.9f, 0.8f, 1.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = i % 2 ? glm::vec3(0.8f, 0.82f, 0.88f) : glm::vec3(1.2f, 1.2f, 1.2f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			if (i % 2) metalTexture.UseTexture(); else ventanasTexture.UseTexture();
			meshListModel[CUBO]->RenderMeshModel();
		}
		for (int i = 0; i < 4; i++) {	// cuatro brazos
			model = modelaux2;
			model = glm::rotate(model, glm::radians(i * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(3.5f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(4.0f, 0.4f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.55f, 0.58f, 0.62f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalTexture.UseTexture();
			meshListModel[CUBO]->RenderMeshModel();
		}
		for (int i = 0; i < 2; i++) {	// dos bahias de acoplamiento, fijas debajo del nucleo, con una franja de luz que late
			model = modelaux;
			model = glm::translate(model, glm::vec3(i == 0 ? -4.0f : 4.0f, -2.5f, 4.0f));
			bahia = model;
			model = glm::scale(model, glm::vec3(2.5f, 1.5f, 3.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.35f, 0.6f, 0.85f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalTexture.UseTexture();
			meshListModel[CUBO]->RenderMeshModel();
			model = bahia;
			model = glm::translate(model, glm::vec3(0.0f, -0.8f, 1.52f));
			model = glm::scale(model, glm::vec3(2.3f, 0.15f, 0.05f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.3f, 1.5f, 2.2f) * (0.7f + 0.3f * sin(4.0f * now + i));
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			meshListModel[CUBO]->RenderMeshModel();
		}
		// ---- E3 plataforma de lanzamiento y cohete (AB1) ----
		model = glm::mat4(1.0);
		model = glm::translate(model, PLATAFORMA + glm::vec3(0.0f, 0.03f, 0.0f));
		model = glm::scale(model, glm::vec3(6.0f, 1.0f, 6.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.72f, 0.66f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[DISCO]->RenderMeshModel();
		// torre de servicio con su brazo
		model = glm::mat4(1.0);
		model = glm::translate(model, PLATAFORMA + glm::vec3(5.0f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 6.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 13.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.48f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[CUBO]->RenderMeshModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 8.5f, 0.0f));
		model = glm::rotate(model, glm::radians(brazoServicio), glm::vec3(0.0f, 1.0f, 0.0f));	// se retira antes del despegue
		model = glm::translate(model, glm::vec3(-1.9f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.8f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshListModel[CUBO]->RenderMeshModel();
		// cohete Saturn V: la etapa de arriba siempre va; las de abajo solo mientras no se han soltado
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
		if (kcActivo) {	// KF-P10: cada etapa con su posicion y giro interpolados
			for (int k = 0; k < 3; k++) {
				model = glm::mat4(1.0);
				model = glm::translate(model, kcPos[k]);
				model = glm::rotate(model, glm::radians(kcGiro[k]), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::scale(model, glm::vec3(kcEscala, kcEscala, kcEscala));
				model = glm::translate(model, glm::vec3(0.0f, -kcPivote[k], 0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				etapasKC[k]->RenderModel();
			}
		}
		// en la fase 4 la tercera etapa ya entro a la bahia de la estacion
		if (faseCohete != 4 && !kcActivo) {
			model = cohM;
			model = glm::scale(model, glm::vec3(escalaCohete, escalaCohete, escalaCohete));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Etapa3_M.RenderModel();
			if (!sepHecha[1]) Etapa2_M.RenderModel();
			if (!sepHecha[0]) Etapa1_M.RenderModel();
		}
		// etapas sueltas: caen con su posicion y giran sobre su centro hasta llegar al piso
		if (sepHecha[0] && sepPos[0].y > 1.0f) {
			model = glm::mat4(1.0);
			model = glm::translate(model, sepPos[0]);
			model = glm::rotate(model, glm::radians(sepInc[0]), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(escalaCohete, escalaCohete, escalaCohete));
			model = glm::translate(model, glm::vec3(0.0f, -sepPivote[0], 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Etapa1_M.RenderModel();
		}
		if (sepHecha[1] && sepPos[1].y > 1.0f) {
			model = glm::mat4(1.0);
			model = glm::translate(model, sepPos[1]);
			model = glm::rotate(model, glm::radians(sepInc[1]), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(escalaCohete, escalaCohete, escalaCohete));
			model = glm::translate(model, glm::vec3(0.0f, -sepPivote[1], 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Etapa2_M.RenderModel();
		}
		// ---- COHETE DE REACH (Practica 10 de 317136421): cuatro piezas, cada una con su posicion y giro interpolados; los OBJ ya vienen centrados ----
		if (reachN > 0) {
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
			for (reachPieza = 0; reachPieza < 4; reachPieza++) {
				model = glm::mat4(1.0);
				model = glm::translate(model, reachPosAct[reachPieza]);
				model = glm::rotate(model, glm::radians(reachGiroAct[reachPieza]), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::scale(model, glm::vec3(1.4f, 1.4f, 1.4f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				reachEtapas[reachPieza].RenderModel();
			}
		}

		// ---- CE1 a CE5 cuerpos celestes: planeta gaseoso con anillo, luna de hielo, planeta de lava, planeta alien y una luna de roca ----
		// lejos de los planetas para que no se encimen: el gaseoso al fondo, detras de la estacion
		posCuerpo[0] = glm::vec3(-40, 62, -175);
		posCuerpo[1] = glm::vec3(60, 37, 15);
		posCuerpo[2] = glm::vec3(-75, 48, 150);
		posCuerpo[3] = glm::vec3(95, 42, 100);
		posCuerpo[4] = glm::vec3(0, 40, -30);
		radioCuerpo[0] = 18.0f;
		radioCuerpo[1] = 4.4f;
		radioCuerpo[2] = 4.5f;
		radioCuerpo[3] = 6.0f;
		radioCuerpo[4] = 3.0f;
		texCuerpo[0] = &planetaTexture[0];
		texCuerpo[1] = &planetaTexture[1];
		texCuerpo[2] = &planetaTexture[2];
		texCuerpo[3] = &planetaTexture[3];
		texCuerpo[4] = &rocaTexture;
		for (int i = 0; i < 5; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, posCuerpo[i] + glm::vec3(0.0f, 8.0f, 0.0f));
			model = glm::rotate(model, glm::radians(i == 0 ? 25.0f : 0.0f), glm::vec3(1.0f, 0.0f, 0.3f));	// el gaseoso va inclinado con todo y anillo
			modelaux = model;
			model = glm::rotate(model, glm::radians(now * (6.0f + 3.0f * i)), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(radioCuerpo[i], radioCuerpo[i], radioCuerpo[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = i == 2 ? glm::vec3(1.4f, 1.3f, 1.2f) : glm::vec3(1.0f, 1.0f, 1.0f);	// la lava brilla un poco
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			texCuerpo[i]->UseTexture();
			roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[ESFERA]->RenderMeshModel();
			if (i == 0) {	// anillo del gaseoso, gira despacio en sentido contrario
				model = modelaux;
				model = glm::rotate(model, glm::radians(-now * 4.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(radioCuerpo[i], 1.0f, radioCuerpo[i]));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				anilloTexture.UseTexture();
				meshListModel[ANILLO]->RenderMeshModel();
			}
		}
		// satelites: cuerpo dorado, dos paneles solares, antena de Kenney arriba y luz que parpadea; orbitan la estacion
		radioOrbita[0] = 40.0f;
		radioOrbita[1] = 48.0f;
		radioOrbita[2] = 56.0f;
		alturaOrbita[0] = -6.0f;
		alturaOrbita[1] = 8.0f;
		alturaOrbita[2] = 2.0f;
		for (int i = 0; i < 3; i++) {
			a4 = now * (0.10f - 0.02f * i) + i * 2.1f;
			model = glm::mat4(1.0);
			model = glm::translate(model, C_ESTACION + glm::vec3(radioOrbita[i] * cos(a4), alturaOrbita[i], radioOrbita[i] * sin(a4)));
			model = glm::rotate(model, -a4, glm::vec3(0.0f, 1.0f, 0.0f));	// de frente a la estacion mientras da la vuelta
			model = glm::scale(model, glm::vec3(1.6f, 1.6f, 1.6f));
			modelaux = model;
			model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.6f));	// cuerpo
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.9f, 0.7f, 0.25f);	// forro dorado
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CUBO]->RenderMeshModel();
			for (int lado = -1; lado <= 1; lado += 2) {	// paneles
				model = modelaux;
				model = glm::translate(model, glm::vec3(lado * 2.4f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(3.4f, 0.06f, 1.3f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(1.0f, 1.0f, 1.0f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				solarTexture.UseTexture();
				meshListModel[CUBO]->RenderMeshModel();
			}
			model = modelaux;	// antena
			model = glm::translate(model, glm::vec3(0.0f, 0.6f, 0.0f));
			model = glm::scale(model, glm::vec3(1.3f, 1.3f, 1.3f));
			model = glm::translate(model, glm::vec3(0.0f, 0.4071f, 0.0992f));	// el OBJ esta centrado en el origen: se regresa a su base para que quede donde estaba
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.9f, 0.92f, 0.95f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
			antenaSatelite.RenderModel();
			model = modelaux;	// luz
			model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.8f));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = fmod(now + i * 0.4f, 1.5f) < 0.3f ? glm::vec3(0.4f, 3.0f, 0.6f) : glm::vec3(0.1f, 0.3f, 0.1f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			meshListModel[ESFERA]->RenderMeshModel();
		}
		// asteroides (meteoros de Kenney) entre los planetas
		posMeteoro[0] = glm::vec3(-28, 36, -16.8f);
		posMeteoro[1] = glm::vec3(39.2f, 26, 5.6f);
		posMeteoro[2] = glm::vec3(16.8f, 46, -50.4f);
		posMeteoro[3] = glm::vec3(-11.2f, 30, 84);
		posMeteoro[4] = glm::vec3(-84, 40, 28);
		posMeteoro[5] = glm::vec3(86.8f, 44, 33.6f);
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, posMeteoro[i] + glm::vec3(0.0f, 10.0f + 1.5f * sin(0.5f * now + i), 0.0f));	// flotan despacio
			model = glm::rotate(model, glm::radians(now * (9.0f + 4.0f * i)), glm::vec3(0.3f, 1.0f, 0.2f));
			model = glm::scale(model, glm::vec3(4.5f + i * 0.6f, 4.5f + i * 0.6f, 4.5f + i * 0.6f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.55f, 0.5f, 0.45f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meteoro.RenderModel();
		}

		// ---- NV nave viajera (Pelican de Halo) ----
		model = glm::mat4(1.0);
		model = glm::translate(model, posNV);
		model = glm::rotate(model, atan2(tanNV.x, tanNV.z), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
		pelican.RenderModel();


		//=====================================================================================
		// 5. P1 MUNDO FORJA (Ratchet & Clank) · 419049898
		// centro C_FORJA, radio R_FORJA; coordenadas locales: norte = plataforma, este = refineria,
		// oeste = corral de Protopets, centro = vendedor y cajas, sur = garage, figura y plataformas flotantes
		//=====================================================================================
		cMF = C_FORJA;
		// MF-01 torres de refineria x3: chimenea de Kenney (Space Kit, CC0) con un generador al lado
		refineria[0] = glm::vec3(22.4f, 0, -8.4f);
		refineria[1] = glm::vec3(29.4f, 0, 1.4f);
		refineria[2] = glm::vec3(21, 0, 11.2f);
		for (int i = 0; i < 3; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + refineria[i]);
			modelaux = model;
			model = glm::scale(model, glm::vec3(6.5f + 0.7f * i, 6.5f + 0.7f * i, 6.5f + 0.7f * i));
			model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));	// el OBJ esta centrado en el origen: se regresa a su base para que quede donde estaba
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.62f, 0.6f, 0.5f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			chimenea.RenderModel();
			model = modelaux;	// generador junto a la chimenea
			model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
			model = glm::translate(model, glm::vec3(0.0f, 0.3415f, 0.0f));	// el OBJ esta centrado en el origen: se regresa a su base para que quede donde estaba
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.85f, 0.5f, 0.2f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			generador.RenderModel();
		}
		// tuberias entre las torres (cilindro acostado de una torre a la siguiente) y dos tanques
		for (int i = 0; i < 2; i++) {
			a5 = cMF + refineria[i] + glm::vec3(0.0f, 3.5f + i, 0.0f);
			b3 = cMF + refineria[i + 1] + glm::vec3(0.0f, 3.5f + i, 0.0f);
			d2 = b3 - a5;
			model = glm::mat4(1.0);
			model = glm::translate(model, a5);
			model = glm::rotate(model, atan2(d2.x, d2.z), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.9f, glm::length(d2), 0.9f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.55f, 0.52f, 0.46f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalTexture.UseTexture();
			meshListModel[CILINDRO]->RenderMeshModel();
		}
		tanque[0] = glm::vec3(35, 0, -16.8f);
		tanque[1] = glm::vec3(35, 0, 16.8f);
		for (int i = 0; i < 2; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + tanque[i]);
			modelaux = model;
			model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.7f, 0.68f, 0.6f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CILINDRO]->RenderMeshModel();
			model = modelaux;	// tapa redonda
			model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
			model = glm::scale(model, glm::vec3(2.5f, 1.2f, 2.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshListModel[ESFERA]->RenderMeshModel();
			model = modelaux;	// franja naranja de peligro
			model = glm::translate(model, glm::vec3(0.0f, 2.2f, 0.0f));
			model = glm::scale(model, glm::vec3(5.1f, 0.6f, 5.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.95f, 0.5f, 0.1f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CILINDRO]->RenderMeshModel();
		}

		// MF-03 grind rails x2: curvas cuadraticas (A -> control C -> B) hechas con tramos de cilindro
		rielA[0] = glm::vec3(-33.6f, 1.6f, -11.2f);
		rielA[1] = glm::vec3(5.6f, 1.6f, 30.8f);
		rielC[0] = glm::vec3(-25.2f, 1.6f, -36.4f);
		rielC[1] = glm::vec3(25.2f, 1.6f, 39.2f);
		rielB[0] = glm::vec3(-8.4f, 1.6f, -22.4f);
		rielB[1] = glm::vec3(33.6f, 1.6f, 18.2f);
		for (int r = 0; r < 2; r++) {
			for (int k = 0; k < 30; k++) {
				t1 = k / 30.0f;
				t2 = (k + 1) / 30.0f;
				q1 = (1 - t1) * (1 - t1) * rielA[r] + 2 * (1 - t1) * t1 * rielC[r] + t1 * t1 * rielB[r];
				q2 = (1 - t2) * (1 - t2) * rielA[r] + 2 * (1 - t2) * t2 * rielC[r] + t2 * t2 * rielB[r];
				d3 = q2 - q1;
				model = glm::mat4(1.0);
				model = glm::translate(model, cMF + q1);
				model = glm::rotate(model, atan2(d3.x, d3.z), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// el cilindro nace parado, lo acuesto
				model = glm::scale(model, glm::vec3(0.3f, glm::length(d3), 0.3f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.85f, 0.7f, 0.2f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				plainTexture.UseTexture();
				metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshListModel[CILINDRO]->RenderMeshModel();
				if (k % 5 == 0) {	// poste cada pocos tramos
					model = glm::mat4(1.0);
					model = glm::translate(model, cMF + glm::vec3(q1.x, 0.0f, q1.z));
					model = glm::scale(model, glm::vec3(0.2f, 1.6f, 0.2f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = glm::vec3(0.35f, 0.3f, 0.2f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CILINDRO]->RenderMeshModel();
				}
			}
		}
		// MF-04 tornillos x12 con AC1: 8 sobre los rieles y 4 en el centro; giran en Y a 90 grados/s y flotan con un seno desfasado
		for (int r = 0; r < 2; r++)
			for (int k = 0; k < 4; k++) {
				t8 = 0.15f + 0.23f * k;
				tornillo2[r * 4 + k] = (1 - t8) * (1 - t8) * rielA[r] + 2 * (1 - t8) * t8 * rielC[r] + t8 * t8 * rielB[r] + glm::vec3(0.0f, -0.4f, 0.0f);
			}
		tornillo2[8] = glm::vec3(2.8f, 1.2f, -5.6f); tornillo2[9] = glm::vec3(7, 1.2f, -1.4f); tornillo2[10] = glm::vec3(-1.4f, 1.2f, -9.8f); tornillo2[11] = glm::vec3(11.2f, 1.2f, -7);
		for (int i = 0; i < 12; i++) {
			// si el avatar pasa junto al tornillo, se recoge y desaparece 8 s
			if (tornilloOculto[i] > 0.0f) { tornilloOculto[i] -= dt; continue; }
			if (modoCamara == 1 && glm::distance(avatarPos + glm::vec3(0.0f, 1.2f, 0.0f), cMF + tornillo2[i]) < 3.5f) { tornilloOculto[i] = 8.0f; continue; }
			y2 = tornillo2[i].y + 0.25f * sin(2.0f * PI * now / 2.0f + i * 0.7f);
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + glm::vec3(tornillo2[i].x, y2 - 0.5f, tornillo2[i].z));	// el modelo nace en su base, lo bajo medio tornillo
			model = glm::rotate(model, glm::radians(90.0f * now), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.085f, 0.085f, 0.085f));	// mide 11.6 de alto, queda de ~1
			model = glm::translate(model, glm::vec3(0.0f, 5.8047f, 0.0f));	// el OBJ esta centrado en el origen: se regresa a su base para que quede donde estaba
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
			tornilloModelo.RenderModel();	// modelo del juego (antes era el prisma hexagonal TORNILLO)
		}
		// MF-05 cajas x8 junto al vendedor: de tornillos, de nanotech y de metal (modelos del juego, miden 24, quedan de 1.1)
		caja[0] = glm::vec3(-12.6f, 0, 0);
		caja[1] = glm::vec3(-12.6f, 0, 2.8f);
		caja[2] = glm::vec3(-12.6f, 1.1f, 1.4f);
		caja[3] = glm::vec3(-14.7f, 0, 5.6f);
		caja[4] = glm::vec3(-10.5f, 0, 7);
		caja[5] = glm::vec3(1, 0, 3);
		caja[6] = glm::vec3(2.3f, 0, 4.6f);
		caja[7] = glm::vec3(1.4f, 1.1f, 3.6f);
		for (int i = 0; i < 8; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + caja[i]);	// el modelo nace en su base
			model = glm::rotate(model, glm::radians(i * 23.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.046f, 0.046f, 0.046f));
			model = glm::translate(model, glm::vec3(0.0f, 12.0f, 0.0f));	// el OBJ esta centrado en el origen: se regresa a su base para que quede donde estaba
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			if (i % 3 == 0) cajaTornillos.RenderModel();
			else if (i % 3 == 1) cajaNanotech.RenderModel();
			else cajaMetal.RenderModel();
		}
		// MF-06 vendedor Gadgetron (modelo del juego, mide 66 de alto, lo dejo en ~6)
		model = glm::mat4(1.0);
		model = glm::translate(model, cMF + glm::vec3(-5.6f, 0.0f, 2.8f));
		model = glm::rotate(model, glm::radians(giroVendedor), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.09f, 0.09f, 0.09f));
		model = glm::translate(model, glm::vec3(0.0f, 33.2567f, 0.0f));	// el OBJ esta centrado en el origen: se regresa a su base para que quede donde estaba
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
		vendedorGadgetron.RenderModel();

		// MF-07 torretas x2 junto a la plataforma (jerarquia: base -> torre en Y -> cañon en X); la de la izquierda hace AB2
		torreta[0] = cMF + glm::vec3(-11.2f, 0.0f, -28.0f);
		torreta[1] = cMF + glm::vec3(11.2f, 0.0f, -28.0f);
		for (int i = 0; i < 2; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, torreta[i]);
			modelaux = model;
			model = glm::scale(model, glm::vec3(1.8f, 1.0f, 1.8f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.42f, 0.42f, 0.42f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 1.6f, 0.0f));
			model = glm::rotate(model, glm::radians(i == 0 ? giroTorre : 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			modelaux = model;
			model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.6f, 0.18f, 0.18f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.5f - (i == 0 ? retroceso : 0.0f)));
			model = glm::rotate(model, glm::radians(90.0f - (i == 0 ? angCanon : 0.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.35f, 1.8f, 0.35f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.25f, 0.25f, 0.27f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CILINDRO]->RenderMeshModel();
		}
		if (faseTorreta == 2) {	// proyectil de AB2
			model = glm::mat4(1.0);
			model = glm::translate(model, posBala);
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 0.85f, 0.3f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
		}

		// MF-08 plataformas flotantes x3 afuera de la orilla sur: suben y bajan con un seno y giran despacio
		for (int i = 0; i < 3; i++) {
			p = cMF + glm::vec3(-14.0f + 14.0f * i, 2.5f + 0.9f * sin(0.8f * now + i * 2.0f), 46.0f + (i == 1 ? 5.0f : 0.0f));
			model = glm::mat4(1.0);
			model = glm::translate(model, p);
			model = glm::rotate(model, glm::radians(10.0f * now + i * 40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			modelaux = model;
			model = glm::scale(model, glm::vec3(7.0f, 0.6f, 7.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.5f, 0.52f, 0.55f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalTexture.UseTexture();
			metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
			for (int k = 0; k < 8; k++) {	// orilla de franjas amarillas y negras
				model = modelaux;
				model = glm::rotate(model, glm::radians(k * 45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::translate(model, glm::vec3(3.4f, 0.6f, 0.0f));
				model = glm::scale(model, glm::vec3(0.3f, 0.25f, 2.6f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = k % 2 ? glm::vec3(0.12f, 0.12f, 0.12f) : glm::vec3(0.95f, 0.8f, 0.1f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				plainTexture.UseTexture();
				meshListModel[CUBO]->RenderMeshModel();
			}
			model = modelaux;	// propulsor que brilla abajo
			model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
			model = glm::scale(model, glm::vec3(1.2f, 0.5f, 1.2f) * (1.0f + 0.1f * sin(8.0f * now + i)));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.6f, 2.0f, 2.4f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
		}

		// MF-09 Blargs x4 trabajando en la refineria (jerarquia: cadera -> torso -> cabeza con pico; brazos que martillan; piernas)
		blarg[0] = glm::vec3(14, 0, -14);
		blarg[1] = glm::vec3(15.4f, 0, 4.2f);
		blarg[2] = glm::vec3(28, 0, 18.2f);
		blarg[3] = glm::vec3(33.6f, 0, -5.6f);
		giroBlarg[0] = 60.0f;
		giroBlarg[1] = 90.0f;
		giroBlarg[2] = 160.0f;
		giroBlarg[3] = -90.0f;
		for (int i = 0; i < 4; i++) {
			golpe = 45.0f * sin(5.0f * now + i * 1.3f);	// el brazo derecho sube y baja con la llave
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + blarg[i] + glm::vec3(0.0f, 1.1f, 0.0f));
			model = glm::rotate(model, glm::radians(giroBlarg[i]), glm::vec3(0.0f, 1.0f, 0.0f));
			modelaux = model;	// cadera
			for (int lado = -1; lado <= 1; lado += 2) {	// piernas
				model = modelaux;
				model = glm::translate(model, glm::vec3(lado * 0.3f, -1.1f, 0.0f));
				model = glm::scale(model, glm::vec3(0.35f, 1.1f, 0.35f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.25f, 0.28f, 0.36f);	// overol
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				plainTexture.UseTexture();
				concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshListModel[CILINDRO]->RenderMeshModel();
			}
			model = modelaux;	// torso
			model = glm::translate(model, glm::vec3(0.0f, 0.6f, 0.0f));
			modelaux2 = model;
			model = glm::scale(model, glm::vec3(0.8f, 1.0f, 0.6f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.3f, 0.34f, 0.44f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
			model = modelaux2;	// cabeza
			model = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.1f));
			cabeza = model;
			model = glm::scale(model, glm::vec3(0.4f, 0.45f, 0.45f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.45f, 0.62f, 0.32f);	// piel verde
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
			model = cabeza;	// pico largo hacia el frente
			model = glm::translate(model, glm::vec3(0.0f, -0.05f, 0.3f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.35f, 0.8f, 0.25f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshListModel[CONO]->RenderMeshModel();
			for (int lado = -1; lado <= 1; lado += 2) {	// ojos
				model = cabeza;
				model = glm::translate(model, glm::vec3(lado * 0.2f, 0.15f, 0.3f));
				model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(1.0f, 0.85f, 0.2f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[ESFERA]->RenderMeshModel();
			}
			for (int lado = -1; lado <= 1; lado += 2) {	// brazos: el derecho martilla, el izquierdo queda quieto
				model = modelaux2;
				model = glm::translate(model, glm::vec3(lado * 0.85f, 0.7f, 0.0f));
				model = glm::rotate(model, glm::radians(lado > 0 ? -60.0f + golpe : 10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				brazo = model;
				model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
				model = glm::scale(model, glm::vec3(0.25f, 0.9f, 0.25f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.45f, 0.62f, 0.32f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CILINDRO]->RenderMeshModel();
				if (lado > 0) {	// llave en la mano
					model = brazo;
					model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.3f));
					model = glm::scale(model, glm::vec3(0.12f, 0.12f, 0.8f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = glm::vec3(0.6f, 0.6f, 0.62f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
					meshListModel[CUBO]->RenderMeshModel();
					concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
				}
			}
		}

		// MF-10 Protopets x3 en el oeste: dos normales y uno mutado (mas grande); brincan con |sin| y se aplastan al caer
		protopet[0] = glm::vec3(-28, 0, -1.4f);
		protopet[1] = glm::vec3(-32.2f, 0, 5.6f);
		protopet[2] = glm::vec3(-25.2f, 0, 9.8f);
		for (int i = 0; i < 3; i++) {
			mutado = i == 2;
			tam = mutado ? 1.8f : 0.8f;
			brinco = fabs(sin(3.5f * now + i * 1.7f));
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + protopet[i] + glm::vec3(0.0f, tam * (1.0f + 0.9f * brinco), 0.0f));
			model = glm::rotate(model, glm::radians(30.0f * sin(0.7f * now + i)), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(tam, tam * (0.85f + 0.15f * brinco), tam));
			modelaux = model;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = mutado ? glm::vec3(0.6f, 0.2f, 0.45f) : glm::vec3(0.98f, 0.66f, 0.2f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[ESFERA]->RenderMeshModel();
			for (int lado = -1; lado <= 1; lado += 2) {
				model = modelaux;	// ojo grande
				model = glm::translate(model, glm::vec3(lado * 0.35f, 0.3f, 0.8f));
				model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = mutado ? glm::vec3(1.0f, 0.3f, 0.2f) : glm::vec3(1.0f, 1.0f, 1.0f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[ESFERA]->RenderMeshModel();
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.7f));	// pupila
				model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.05f, 0.05f, 0.05f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[ESFERA]->RenderMeshModel();
				model = modelaux;	// cuernito
				model = glm::translate(model, glm::vec3(lado * 0.45f, 0.75f, 0.0f));
				model = glm::rotate(model, glm::radians(-lado * 25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::scale(model, glm::vec3(0.25f, 0.5f, 0.25f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = mutado ? glm::vec3(0.9f, 0.9f, 0.8f) : glm::vec3(0.75f, 0.4f, 0.12f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CONO]->RenderMeshModel();
			}
			if (mutado)	// el mutado tiene dientes
				for (int k = -2; k <= 2; k++) {
					model = modelaux;
					model = glm::translate(model, glm::vec3(k * 0.15f, -0.25f, 0.9f));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.1f, 0.2f, 0.1f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = glm::vec3(1.0f, 1.0f, 0.95f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CONO]->RenderMeshModel();
				}
		}
		// cerca del corral de los Protopets: postes en circulo
		for (int k = 0; k < 22; k++) {
			a6 = k * 2.0f * PI / 22.0f;
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + glm::vec3(-28.7f + 8.5f * cos(a6), 0.0f, 4.2f + 8.5f * sin(a6)));
			model = glm::scale(model, glm::vec3(0.2f, 1.2f, 0.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.5f, 0.45f, 0.35f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
		}

		// MF-12 garage de Ratchet: caja con techo de medio cilindro, porton abierto y franjas naranjas
		model = glm::mat4(1.0);
		model = glm::translate(model, cMF + glm::vec3(-16.8f, 0.0f, 23.8f));
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 4.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.78f, 0.74f, 0.66f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalTexture.UseTexture();
		metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[CUBO]->RenderMeshModel();
		model = modelaux;	// techo: cilindro acostado de lado a lado, medio enterrado en la caja
		model = glm::translate(model, glm::vec3(-4.0f, 4.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(7.0f, 8.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.6f, 0.62f, 0.66f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[CILINDRO]->RenderMeshModel();
		model = modelaux;	// porton (hueco oscuro en el frente)
		model = glm::translate(model, glm::vec3(0.0f, 1.6f, 3.52f));
		model = glm::scale(model, glm::vec3(5.0f, 3.2f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.08f, 0.08f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		meshListModel[CUBO]->RenderMeshModel();
		for (int k = 0; k < 2; k++) {	// franjas naranjas a los lados del porton
			model = modelaux;
			model = glm::translate(model, glm::vec3(k == 0 ? -3.2f : 3.2f, 2.0f, 3.53f));
			model = glm::scale(model, glm::vec3(0.6f, 4.0f, 0.05f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.95f, 0.5f, 0.12f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CUBO]->RenderMeshModel();
		}

		// MF-11 figura de Ratchet y Clank en su pedestal; con H despega y vuela con el Heli-Pack (KF1) y regresa al pedestal
		pedestal = glm::vec3(2.8f, 0.0f, 19.6f);
		model = glm::mat4(1.0);
		model = glm::translate(model, cMF + pedestal);
		model = glm::scale(model, glm::vec3(2.6f, 0.6f, 2.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.95f, 0.75f, 0.15f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[CILINDRO]->RenderMeshModel();
		volando = kfReproduciendo || (kfCuadro > 0 && kfCuadro < 8);
		model = glm::mat4(1.0);
		model = glm::translate(model, cMF + pedestal + kfActual + glm::vec3(0.0f, 0.6f, 0.0f));
		if (!volando) giroPedestal += 25.0f * dt;
		model = glm::rotate(model, glm::radians(kfGiroActual + giroBaseRatchet + giroPedestal), glm::vec3(0.0f, 1.0f, 0.0f));
		if (volando) model = glm::rotate(model, glm::radians(8.0f * sin(3.0f * now)), glm::vec3(1.0f, 0.0f, 0.0f));	// se balancea en el aire
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		modelaux = model;
		model = glm::translate(model, glm::vec3(-0.2397f, 0.7088f, 0.0f));	// el OBJ esta centrado en el origen: se regresa a su base para que quede donde estaba
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
		ratchetModelo.RenderModel();
		// Clank en la espalda (hijo de Ratchet): modelo del juego, mide 16.4 de alto; de espaldas a Ratchet, como mochila
		model = modelaux;
		model = glm::translate(model, clankEspalda);
		clank = model;
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.034f, 0.034f, 0.034f));
		model = glm::translate(model, glm::vec3(0.0025f, 8.1836f, 0.4003f));	// el OBJ esta centrado en el origen: se regresa a su base para que quede donde estaba
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
		clankModelo.RenderModel();
		if (volando)
			for (int k = 0; k < 2; k++) {	// aspas del Heli-Pack: giran rapido en Y sobre la cabeza de Clank
				model = clank;
				model = glm::translate(model, glm::vec3(0.0f, 0.6f, 0.0f));
				model = glm::rotate(model, glm::radians(now * 1080.0f + k * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(1.1f, 0.02f, 0.09f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.95f, 0.8f, 0.2f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				plainTexture.UseTexture();
				meshListModel[CUBO]->RenderMeshModel();
			}
		// tornillo de oro (modelo del juego): flota y gira sobre el centro, con su luz dorada como pista de que es especial
		model = glm::mat4(1.0);
		model = glm::translate(model, cMF + glm::vec3(-2.0f, 2.5f + 0.4f * sin(1.5f * now), -12.0f));
		model = glm::rotate(model, glm::radians(60.0f * now), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));	// mide 26.5 de alto, queda de ~2
		model = glm::translate(model, glm::vec3(0.0f, 13.408f, 0.0f));	// el OBJ esta centrado en el origen: se regresa a su base para que quede donde estaba
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.3f, 1.2f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
		tornilloOro.RenderModel();
		// la Aphelion (nave de Ratchet, modelo del juego) estacionada en el aire junto a la orilla suroeste; sube y baja despacio
		model = glm::mat4(1.0);
		model = glm::translate(model, cMF + glm::vec3(-27.0f, 5.0f + 0.6f * sin(0.7f * now), 36.0f));
		model = glm::rotate(model, glm::radians(40.0f + 4.0f * sin(0.5f * now)), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.11f, 0.11f, 0.11f));	// mide 89 de ancho, queda de ~10
		model = glm::translate(model, glm::vec3(0.0f, 30.5494f, -2.3243f));	// el OBJ esta centrado en el origen: se regresa a su base para que quede donde estaba
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
		naveRatchet.RenderModel();

		//=====================================================================================
		// 6. P2 REACH (Halo) · 317136421
		// apartado de este integrante: aqui solo se integran sus modelos (HR-01 a HR-14, AB3 Pelican, AC3 antena, AC4 escudos, AC5 plataformas MAC)
		// las posiciones van locales, sumadas a cHR (centro del planeta); radio R_REACH
		// pasos: 1) declarar el Model junto a los demas  2) cargarlo con LoadModel en main  3) dibujarlo aqui
		//=====================================================================================
		// todo Reach cuelga de mundoReach: centro del planeta y escala del plano (las posiciones de abajo son las del plano)
		mundoReach = glm::mat4(1.0);
		mundoReach = glm::translate(mundoReach, cHR);
		mundoReach = glm::scale(mundoReach, glm::vec3(escalaReach, escalaReach, escalaReach));
		concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
		// HR-01 base de 6 x 3 x 4 con techo, puerta, letrero UNSC y dos paneles al frente
		model = mundoReach;
		model = glm::translate(model, glm::vec3(-5.0f, 1.5f, -8.0f));
		model = glm::scale(model, glm::vec3(6.0f, 3.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.85f, 0.85f, 0.85f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		concretoReach.UseTexture();
		meshListModel[CUBO]->RenderMeshModel();
		model = mundoReach;
		model = glm::translate(model, glm::vec3(-5.0f, 3.1f, -8.0f));
		model = glm::scale(model, glm::vec3(6.3f, 0.2f, 4.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.45f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalTexture.UseTexture();
		meshListModel[CUBO]->RenderMeshModel();
		model = mundoReach;
		model = glm::translate(model, glm::vec3(-5.0f, 1.1f, -5.98f));
		model = glm::scale(model, glm::vec3(1.8f, 2.2f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.2f, 0.25f, 0.22f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[CUBO]->RenderMeshModel();
		model = mundoReach;
		model = glm::translate(model, glm::vec3(-5.0f, 2.65f, -5.95f));
		model = glm::scale(model, glm::vec3(2.5f, 0.45f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		letreroReach.UseTexture();
		meshListModel[CUBO]->RenderMeshModel();
		for (int i = 0; i < 2; i++) {
			model = mundoReach;
			model = glm::translate(model, glm::vec3(-7.0f + i * 4.0f, 1.8f, -5.95f));
			model = glm::scale(model, glm::vec3(1.0f, 0.55f, 0.05f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.2f, 0.45f, 0.55f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			meshListModel[CUBO]->RenderMeshModel();
		}
		// HR-02 cuatro barricadas de perfil inclinado
		for (int i = 0; i < 4; i++) {
			model = mundoReach;
			model = glm::translate(model, glm::vec3(barricadasHR[i].x, 0.45f, barricadasHR[i].y));
			model = glm::scale(model, glm::vec3(2.0f, 0.9f, 0.65f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.8f, 0.8f, 0.8f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			concretoReach.UseTexture();
			meshListModel[BARRICADA]->RenderMeshModel();
		}
		// HR-03 torre de comunicaciones y AC3 antena hija que gira sobre Y
		model = mundoReach;
		model = glm::translate(model, glm::vec3(-3.0f, 2.5f, -17.0f));
		model = glm::scale(model, glm::vec3(1.4f, 5.0f, 1.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.7f, 0.7f, 0.7f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		concretoReach.UseTexture();
		meshListModel[CUBO]->RenderMeshModel();
		antenaHR = mundoReach;
		antenaHR = glm::translate(antenaHR, glm::vec3(-3.0f, 5.0f, -17.0f));
		antenaHR = glm::rotate(antenaHR, glm::radians(now * 20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = antenaHR;
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, 0.0f));
		model = glm::scale(model, glm::vec3(0.12f, 1.6f, 0.12f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalTexture.UseTexture();
		meshListModel[CUBO]->RenderMeshModel();
		model = antenaHR;
		model = glm::translate(model, glm::vec3(0.0f, 1.3f, 0.0f));
		model = glm::scale(model, glm::vec3(2.3f, 0.12f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshListModel[CUBO]->RenderMeshModel();
		// HR-04 catorce pinos (tronco cilindro + copa cono) y ocho rocas achatadas, repartidos hasta la orilla
		roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
		for (int i = 0; i < 14; i++) {
			model = mundoReach;
			model = glm::translate(model, glm::vec3(pinosHR[i].x, 0.0f, pinosHR[i].y));
			model = glm::scale(model, glm::vec3(0.4f, 1.5f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.4f, 0.28f, 0.16f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			rocaTexture.UseTexture();	// tronco con textura de corteza
			meshListModel[CILINDRO]->RenderMeshModel();
			model = mundoReach;
			model = glm::translate(model, glm::vec3(pinosHR[i].x, 1.2f, pinosHR[i].y));
			model = glm::scale(model, glm::vec3(2.4f, 4.5f, 2.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.6f, 0.85f, 0.6f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			reachTexture.UseTexture();	// copa con la textura de pasto del planeta
			meshListModel[CONO]->RenderMeshModel();
		}
		for (int i = 0; i < 8; i++) {
			model = mundoReach;
			model = glm::translate(model, glm::vec3(rocasHR[i].x, 0.35f, rocasHR[i].y));
			model = glm::scale(model, glm::vec3(1.3f, 0.8f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.65f, 0.65f, 0.65f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			rocaTexture.UseTexture();
			meshListModel[ESFERA]->RenderMeshModel();
		}
		// HR-05 Warthog: el chasis es padre de las cuatro ruedas y de la torreta; con O patrulla sobre un circulo
		chasisHR = mundoReach;
		chasisHR = glm::translate(chasisHR, glm::vec3(centroPatrulla.x + radioPatrulla * cos(anguloWarthog), 1.06914f, centroPatrulla.y + radioPatrulla * sin(anguloWarthog)));	// se sube la mitad del chasis para que las llantas pisen el suelo
		chasisHR = glm::rotate(chasisHR, -anguloWarthog, glm::vec3(0.0f, 1.0f, 0.0f));	// su frente (+z) va en la direccion del circulo
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(chasisHR));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
		warthog.RenderModel();
		for (int i = 0; i < 4; i++) {
			model = chasisHR;
			model = glm::translate(model, pivoteRueda[i]);
			model = glm::rotate(model, glm::radians(giroRuedas), glm::vec3(1.0f, 0.0f, 0.0f));	// la rueda gira sobre su eje (x)
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			ruedasWarthog[i].RenderModel();
		}
		model = chasisHR;
		model = glm::translate(model, pivoteTorreta);
		model = glm::rotate(model, glm::radians(giroTorretaW), glm::vec3(0.0f, 1.0f, 0.0f));	// vigila de lado a lado mientras patrulla
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		torretaWarthog.RenderModel();
		// HR-09 marca H de la zona de aterrizaje (disco de radio 3)
		concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
		model = mundoReach;
		model = glm::translate(model, glm::vec3(12.0f, 0.03f, 5.0f));
		model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pistaReach.UseTexture();
		meshListModel[ATERRIZAJE]->RenderMeshModel();
		// HR-06 / AB3 Pelican sobre la marca H; con P sube, espera y aterriza
		model = mundoReach;
		model = glm::translate(model, glm::vec3(12.0f, 0.08f + 1.01193f + alturaPelican / escalaReach, 5.0f));	// 1.01193 = mitad del alto del Pelican
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
		pelican.RenderModel();
		// HR-07 seis cajas UNSC en dos filas frente al cuartel
		concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
		for (int i = 0; i < 6; i++) {
			model = mundoReach;
			model = glm::translate(model, glm::vec3(-7.0f + (i % 3) * 2.0f, 0.5f, i < 3 ? -4.2f : -2.6f));
			model = glm::scale(model, glm::vec3(0.95f, 1.0f, 0.85f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			cajaReach.UseTexture();
			meshListModel[CUBO]->RenderMeshModel();
		}
		// HR-10 / AC5 dos plataformas MAC en orbita, una vuelta cada 120 segundos
		metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
		metalTexture.UseTexture();
		for (int i = 0; i < 2; i++) {
			anguloMAC = now * 2.0f * PI / 120.0f + i * PI;
			macHR = mundoReach;
			macHR = glm::translate(macHR, glm::vec3(19.0f * cos(anguloMAC), 18.0f, 19.0f * sin(anguloMAC)));
			macHR = glm::rotate(macHR, -anguloMAC, glm::vec3(0.0f, 1.0f, 0.0f));
			model = macHR;
			model = glm::scale(model, glm::vec3(1.2f, 0.8f, 4.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.45f, 0.45f, 0.45f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CUBO]->RenderMeshModel();
			model = macHR;
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));	// canon
			model = glm::scale(model, glm::vec3(0.4f, 0.4f, 3.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.7f, 0.7f, 0.7f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CUBO]->RenderMeshModel();
			model = macHR;
			model = glm::translate(model, glm::vec3(-1.1f, 0.0f, -1.0f));	// ala izquierda
			model = glm::scale(model, glm::vec3(1.0f, 0.3f, 1.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.3f, 0.3f, 0.3f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CUBO]->RenderMeshModel();
			model = macHR;
			model = glm::translate(model, glm::vec3(1.1f, 0.0f, -1.0f));	// ala derecha
			model = glm::scale(model, glm::vec3(1.0f, 0.3f, 1.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshListModel[CUBO]->RenderMeshModel();
		}
		// HR-11 a HR-14 marine, Inquisidor, grunt y Jefe Maestro (figuras quietas)
		metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
		for (int i = 0; i < 4; i++) {
			model = mundoReach;
			model = glm::translate(model, posNpcHR[i] + glm::vec3(0.0f, mitadNpcHR[i], 0.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			npcHR[i]->RenderModel();
		}
		// puesto de control: poste con la pluma (rayas rojas y blancas) que sube y baja con J; el apoyo recibe la punta
		concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
		model = mundoReach;
		model = glm::translate(model, glm::vec3(2.0f, 0.6f, -1.0f));
		model = glm::scale(model, glm::vec3(0.35f, 1.2f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.7f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		meshListModel[CUBO]->RenderMeshModel();
		model = mundoReach;
		model = glm::translate(model, glm::vec3(-2.3f, 0.45f, -1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.9f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshListModel[CUBO]->RenderMeshModel();
		for (int i = 0; i < 4; i++) {
			model = mundoReach;
			model = glm::translate(model, glm::vec3(2.0f, 1.1f, -1.0f));	// bisagra arriba del poste
			model = glm::rotate(model, glm::radians(-anguloPluma), glm::vec3(0.0f, 0.0f, 1.0f));	// negativo: la punta (-x) sube
			model = glm::translate(model, glm::vec3(-0.5f - i * 1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 0.15f, 0.15f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = i % 2 == 0 ? glm::vec3(0.85f, 0.1f, 0.1f) : glm::vec3(0.95f, 0.95f, 0.95f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CUBO]->RenderMeshModel();
		}
		// HR-08 soportes de los escudos (el plano transparente de AC4 se dibuja al final del cuadro)
		metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
		for (int i = 0; i < 2; i++) {
			model = mundoReach;
			model = glm::translate(model, glm::vec3(escudosHR[i].x, 0.1f, escudosHR[i].y));
			model = glm::scale(model, glm::vec3(2.4f, 0.2f, 0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.35f, 0.35f, 0.35f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalTexture.UseTexture();
			meshListModel[CUBO]->RenderMeshModel();
		}


		//=====================================================================================
		// 7. P3 VENOM (Star Fox 64) · 318337872
		// apartado de este integrante: aqui solo se integran sus modelos (SF-01 a SF-11, AB4 Arwing, KF2 acoplamiento en la estacion)
		// las posiciones van locales, sumadas a cSF (centro del planeta); radio R_VENOM
		// pasos: 1) declarar el Model junto a los demas  2) cargarlo con LoadModel en main  3) dibujarlo aqui
		//=====================================================================================

		// ---- ESTRUCTURAS DE VENOM 2: domo central, pilares y mini estaciones espaciales ----
		{
			metalTexture.UseTexture();
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);

			// ---- DOMO central: base escalonada, cascaron dorado con costillas y escotilla por donde despega la nave ----
			{
				model = glm::translate(mDomo, glm::vec3(0.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(36.0f, 1.2f, 36.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = oscuro;
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CILINDRO]->RenderMeshModel();
			}
			{
				model = glm::translate(mDomo, glm::vec3(0.0f, 1.2f, 0.0f));
				model = glm::scale(model, glm::vec3(31.0f, 1.0f, 31.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = dorado * 0.7f;
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CILINDRO]->RenderMeshModel();
			}
			{
				model = glm::translate(mDomo, glm::vec3(0.0f, DOMO_BASE, 0.0f));
				model = glm::scale(model, glm::vec3(DOMO_R, DOMO_ALTO, DOMO_R));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = dorado;
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[ESFERA]->RenderMeshModel();
			}
			for (int i = 0; i < 8; i++) {	// cada elipsoide delgado forma dos costillas (una a cada lado), 16 en total
				mc = glm::rotate(mDomo, glm::radians(22.5f * i), glm::vec3(0.0f, 1.0f, 0.0f));
				{
					model = glm::translate(mc, glm::vec3(0.0f, DOMO_BASE, 0.0f));
					model = glm::scale(model, glm::vec3(DOMO_R + 0.5f, DOMO_ALTO + 0.4f, 0.55f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = oscuro * 1.6f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[ESFERA]->RenderMeshModel();
				}
			}
			{	// anillos del cascaron
				model = glm::translate(mDomo, glm::vec3(0.0f, DOMO_BASE + 2.5f, 0.0f));
				model = glm::scale(model, glm::vec3(27.8f, 0.5f, 27.8f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = oscuro;
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CILINDRO]->RenderMeshModel();
			}
			{
				model = glm::translate(mDomo, glm::vec3(0.0f, DOMO_BASE + 6.0f, 0.0f));
				model = glm::scale(model, glm::vec3(22.9f, 0.5f, 22.9f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = oscuro;
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CILINDRO]->RenderMeshModel();
			}
			aper = aperturaDomoV;
			{	// interior que brilla al abrir
				model = glm::translate(mDomo, glm::vec3(0.0f, 11.9f, 0.0f));
				model = glm::scale(model, glm::vec3(8.0f, 0.3f, 8.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				plainTexture.UseTexture();	// pieza que brilla: color liso para que no se apague
				color = glm::mix(glm::vec3(0.05f, 0.04f, 0.03f), glm::vec3(3.0f, 1.3f, 0.4f), aper);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CILINDRO]->RenderMeshModel();
				metalTexture.UseTexture();
			}
			for (int lado = -1; lado <= 1; lado += 2)	// las dos tapas de la escotilla se deslizan hacia los lados
				{
					model = glm::translate(mDomo, glm::vec3(lado * (2.1f + 5.6f * aper), 11.7f - 1.5f * aper, 0.0f));
					model = glm::scale(model, glm::vec3(4.2f, 1.2f, 8.4f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = dorado * 0.6f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
			{
				model = glm::translate(mDomo, glm::vec3(0.0f, DOMO_BASE + DOMO_ALTO + 0.05f, 0.0f));
				model = glm::scale(model, glm::vec3(2.6f, 0.5f, 2.6f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = oscuro;
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CILINDRO]->RenderMeshModel();
			}
			for (int i = 0; i < 4; i++) {	// luces de aviso alrededor de la escotilla
				alV = glm::radians(45.0f + 90.0f * i);
				bV = 0.5f + 0.5f * sinf(6.0f * now + i);
				{
					model = glm::translate(mDomo, glm::vec3(5.2f * cosf(alV), 11.0f, 5.2f * sinf(alV)));
					model = glm::scale(model, glm::vec3(0.4f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					plainTexture.UseTexture();	// pieza que brilla: color liso para que no se apague
					color = glm::vec3(2.5f, 0.3f, 0.2f) * (0.4f + 0.6f * bV);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[ESFERA]->RenderMeshModel();
					metalTexture.UseTexture();
				}
			}

			// ---- PILARES con la cara de Andross, en circulo alrededor del domo (la cara mira al domo) ----
			for (int i = 0; i < N_PILARES; i++) {
				alV = glm::radians(22.5f + 45.0f * i);
				mp = glm::translate(glm::mat4(1.0f), cSF + glm::vec3(R_PILARES * cosf(alV), 0.0f, R_PILARES * sinf(alV)));
				mp = glm::rotate(mp, atan2f(-cosf(alV), -sinf(alV)), glm::vec3(0.0f, 1.0f, 0.0f));
				{	// base escalonada
					model = glm::translate(mp, glm::vec3(0.0f, 0.7f, 0.0f));
					model = glm::scale(model, glm::vec3(6.2f, 1.4f, 6.2f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = oscuro;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
				{
					model = glm::translate(mp, glm::vec3(0.0f, 1.9f, 0.0f));
					model = glm::scale(model, glm::vec3(4.8f, 1.0f, 4.8f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = dorado * 0.6f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
				{	// fuste
					model = glm::translate(mp, glm::vec3(0.0f, 2.4f, 0.0f));
					model = glm::scale(model, glm::vec3(2.8f, 11.0f, 2.8f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = dorado * 0.75f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CILINDRO]->RenderMeshModel();
				}
				for (int b = 0; b < 4; b++) {	// bandas
					model = glm::translate(mp, glm::vec3(0.0f, 4.0f + 2.6f * b, 0.0f));
					model = glm::scale(model, glm::vec3(3.4f, 0.45f, 3.4f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = oscuro;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CILINDRO]->RenderMeshModel();
				}
				{	// franja de luz del frente
					model = glm::translate(mp, glm::vec3(0.0f, 8.0f, 1.45f));
					model = glm::scale(model, glm::vec3(0.35f, 8.0f, 0.12f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					plainTexture.UseTexture();	// pieza que brilla: color liso para que no se apague
					color = glm::vec3(1.4f, 1.0f, 0.25f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
					metalTexture.UseTexture();
				}
				{	// cabeza
					model = glm::translate(mp, glm::vec3(0.0f, 16.2f, 0.0f));
					model = glm::scale(model, glm::vec3(5.2f, 5.6f, 4.6f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = dorado;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
				{	// ceja
					model = glm::translate(mp, glm::vec3(0.0f, 17.9f, 2.45f));
					model = glm::scale(model, glm::vec3(4.6f, 0.6f, 0.7f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = oscuro;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
				for (int lado = -1; lado <= 1; lado += 2)	// ojos que brillan
					{
						model = glm::translate(mp, glm::vec3(lado * 1.15f, 17.0f, 2.35f));
						model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.35f));
						glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
						plainTexture.UseTexture();	// pieza que brilla: color liso para que no se apague
						color = glm::vec3(3.2f, 0.6f, 0.15f);
						glUniform3fv(uniformColor, 1, glm::value_ptr(color));
						meshListModel[ESFERA]->RenderMeshModel();
						metalTexture.UseTexture();
					}
				{	// nariz
					model = glm::translate(mp, glm::vec3(0.0f, 16.0f, 2.45f));
					model = glm::scale(model, glm::vec3(0.7f, 1.6f, 0.6f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = dorado * 0.5f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
				{	// boca
					model = glm::translate(mp, glm::vec3(0.0f, 14.6f, 2.4f));
					model = glm::scale(model, glm::vec3(2.8f, 0.4f, 0.4f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = oscuro * 0.4f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
				for (int d = 0; d < 4; d++) {	// dientes
					model = glm::translate(mp, glm::vec3(-1.0f + 0.67f * d, 14.35f, 2.4f));
					model = glm::scale(model, glm::vec3(0.25f, 0.5f, 0.2f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = glm::vec3(0.9f, 0.85f, 0.6f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
				{	// corona
					model = glm::translate(mp, glm::vec3(0.0f, 19.0f, 0.0f));
					model = glm::scale(model, glm::vec3(3.6f, 2.4f, 3.6f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = oscuro;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CONO]->RenderMeshModel();
				}
			}

			// ---- MINI ESTACIONES: flotan sobre el borde del planeta, la puerta del hangar sube para dejar salir a la nave ----
			for (int k = 0; k < N_ESTACIONES; k++) {
				aE = PI / 2.0f + k * 2.0f * PI / 3.0f;	// las tres estaciones se reparten cada 120 grados
				eP = C_VENOM + glm::vec3(R_ESTACION * cosf(aE), ALT_ESTACION + 0.6f * sinf(0.8f * now + 2.0f * k), R_ESTACION * sinf(aE));	// flota subiendo y bajando
				eY = atan2f(-cosf(aE), -sinf(aE));	// la boca del hangar (+z local) mira al centro de Venom
				cE = fmodf(now + 9.0f * k, CICLO_ESTACION); puertaE = 0.0f;	// puerta del hangar: 0 cerrada, 1 abierta (abre antes de que salga la nave y cuando va a regresar)
				if (cE < 3.0f) puertaE = cE < 2.0f ? 1.0f : 3.0f - cE;
				else if (cE >= 34.0f) puertaE = (cE - 34.0f) / 2.0f;
				else if (cE >= 28.0f) puertaE = cE < 29.0f ? cE - 28.0f : (cE < 32.0f ? 1.0f : 1.0f - (cE - 32.0f) / 2.0f);
				me = glm::rotate(glm::translate(glm::mat4(1.0f), eP), eY, glm::vec3(0.0f, 1.0f, 0.0f));
				parp = 0.5f + 0.5f * sinf(4.0f * now + 2.0f * k);	// parpadeo de las luces
				{	// casco
					model = glm::translate(me, glm::vec3(0.0f, 0.0f, -5.0f));
					model = glm::scale(model, glm::vec3(12.0f, 6.0f, 10.0f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = acero;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
				{
					model = glm::translate(me, glm::vec3(0.0f, 0.5f, -12.0f));
					model = glm::scale(model, glm::vec3(8.0f, 4.5f, 6.0f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = acero * 0.85f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
				{	// cabina
					model = glm::translate(me, glm::vec3(0.0f, 3.0f, -4.0f));
					model = glm::scale(model, glm::vec3(4.5f, 2.6f, 4.5f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = glm::vec3(0.7f, 0.78f, 0.85f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[ESFERA]->RenderMeshModel();
				}
				{	// antena
					model = glm::translate(me, glm::vec3(2.5f, 3.0f, -7.0f));
					model = glm::scale(model, glm::vec3(0.35f, 5.0f, 0.35f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = acero;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CILINDRO]->RenderMeshModel();
				}
				{
					model = glm::translate(me, glm::vec3(2.5f, 8.2f, -7.0f));
					model = glm::scale(model, glm::vec3(0.5f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					plainTexture.UseTexture();	// pieza que brilla: color liso para que no se apague
					color = glm::vec3(2.5f, 0.2f, 0.2f) * (0.3f + 0.7f * parp);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[ESFERA]->RenderMeshModel();
					metalTexture.UseTexture();
				}
				for (int lado = -1; lado <= 1; lado += 2) {	// brazos y paneles solares
					{
						model = glm::translate(me, glm::vec3(lado * 7.5f, 0.0f, -5.0f));
						model = glm::scale(model, glm::vec3(3.0f, 0.5f, 0.5f));
						glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
						color = acero * 0.8f;
						glUniform3fv(uniformColor, 1, glm::value_ptr(color));
						meshListModel[CUBO]->RenderMeshModel();
					}
					solarTexture.UseTexture();
					{
						model = glm::translate(me, glm::vec3(lado * 13.0f, 0.0f, -5.0f));
						model = glm::scale(model, glm::vec3(8.0f, 0.15f, 4.5f));
						glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
						color = glm::vec3(1.0f);
						glUniform3fv(uniformColor, 1, glm::value_ptr(color));
						meshListModel[CUBO]->RenderMeshModel();
					}
					metalTexture.UseTexture();
				}
				mr = glm::rotate(glm::translate(me, glm::vec3(0.0f, 0.0f, -14.0f)), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				ventanasTexture.UseTexture();	// rueda habitable de atras, con ventanas prendidas
				{
					model = glm::translate(mr, glm::vec3(0.0f, -0.6f, 0.0f));
					model = glm::scale(model, glm::vec3(18.0f, 1.2f, 18.0f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = glm::vec3(1.0f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CILINDRO]->RenderMeshModel();
				}
				metalTexture.UseTexture();
				for (int g = 0; g < 3; g++)	// propulsores de abajo (mantienen la estacion flotando)
					{
						model = glm::translate(me, glm::vec3(g == 0 ? 0.0f : (g == 1 ? 3.5f : -3.5f), -3.4f, g == 0 ? -5.0f : -7.0f));
						model = glm::scale(model, glm::vec3(0.9f));
						glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
						plainTexture.UseTexture();	// pieza que brilla: color liso para que no se apague
						color = glm::vec3(0.5f, 1.2f, 3.0f) * (0.8f + 0.2f * parp);
						glUniform3fv(uniformColor, 1, glm::value_ptr(color));
						meshListModel[ESFERA]->RenderMeshModel();
						metalTexture.UseTexture();
					}
				// hangar: piso, techo y paredes con el interior oscuro; boca hacia el centro de Venom
				{
					model = glm::translate(me, glm::vec3(0.0f, -2.65f, 5.0f));
					model = glm::scale(model, glm::vec3(8.4f, 0.7f, 10.0f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = oscuro * 1.4f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
				{
					model = glm::translate(me, glm::vec3(0.0f, 2.65f, 5.0f));
					model = glm::scale(model, glm::vec3(8.4f, 0.7f, 10.0f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = oscuro * 1.4f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
				for (int lado = -1; lado <= 1; lado += 2) {
					{
						model = glm::translate(me, glm::vec3(lado * 3.85f, 0.0f, 5.0f));
						model = glm::scale(model, glm::vec3(0.7f, 6.0f, 10.0f));
						glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
						color = oscuro * 1.4f;
						glUniform3fv(uniformColor, 1, glm::value_ptr(color));
						meshListModel[CUBO]->RenderMeshModel();
					}
					{	// guias de luz del piso
						model = glm::translate(me, glm::vec3(lado * 2.8f, -2.28f, 5.0f));
						model = glm::scale(model, glm::vec3(0.2f, 0.05f, 8.0f));
						glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
						plainTexture.UseTexture();	// pieza que brilla: color liso para que no se apague
						color = glm::vec3(0.3f, 2.0f, 0.4f);
						glUniform3fv(uniformColor, 1, glm::value_ptr(color));
						meshListModel[CUBO]->RenderMeshModel();
						metalTexture.UseTexture();
					}
					{	// luces de la boca
						model = glm::translate(me, glm::vec3(lado * 4.0f, 2.0f, 10.1f));
						model = glm::scale(model, glm::vec3(0.35f));
						glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
						plainTexture.UseTexture();	// pieza que brilla: color liso para que no se apague
						color = glm::vec3(0.4f, 2.5f, 0.4f) * (0.3f + 0.7f * parp);
						glUniform3fv(uniformColor, 1, glm::value_ptr(color));
						meshListModel[ESFERA]->RenderMeshModel();
						metalTexture.UseTexture();
					}
				}
				{	// puerta que sube
					model = glm::translate(me, glm::vec3(0.0f, 5.0f * puertaE, 10.2f));
					model = glm::scale(model, glm::vec3(7.0f, 4.6f, 0.4f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = acero * 0.7f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CUBO]->RenderMeshModel();
				}
			}
		}


		// ---- VENOMIANOS (NPCs de Venom): linea evolutiva de una especie de Venom, larva -> joven -> adulto ----
		// cada uno es una jerarquia hecha con primitivas: el cuerpo es el padre y de el cuelgan ojo, antenas, patas, brazos con pinza, cuernos y cola;
		// la larva salta en su lugar, el joven y el adulto caminan en circulos (el adulto es el mas grande y camina en sentido contrario)
		for (int i = 0; i < 3; i++) {
			radioVen = i == 0 ? 0.0f : (i == 1 ? 3.0f : 6.0f);
			sgnVen = i == 1 ? 1.0f : -1.0f;
			angVen = sgnVen * (i == 1 ? 0.5f : 0.3f) * now + i * 2.0f;
			pVen = C_VENOM + baseVen[i] + glm::vec3(radioVen * cos(angVen), 0.0f, radioVen * sin(angVen));
			rumboVen = i == 0 ? 0.6f * sin(0.8f * now) : atan2(-sin(angVen) * sgnVen, cos(angVen) * sgnVen);	// mira hacia donde camina
			saltoVen = i == 0 ? fabs(sin(3.0f * now)) : 0.0f;
			cuerpoYVen = i == 0 ? 1.0f : 1.9f;	// la larva va pegada al piso, los otros se levantan sobre sus patas
			faseVen = 4.0f * now;
			mVen = glm::translate(glm::mat4(1.0f), pVen);
			mVen = glm::rotate(mVen, rumboVen, glm::vec3(0.0f, 1.0f, 0.0f));
			mVen = glm::scale(mVen, glm::vec3(escVen[i], escVen[i], escVen[i]));
			rocaTexture.UseTexture();	// la piel lleva textura de roca con tinte
			roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
			// cuerpo (padre de todo lo demas)
			model = glm::translate(mVen, glm::vec3(0.0f, cuerpoYVen + 0.9f * saltoVen, 0.0f));
			model = glm::scale(model, glm::vec3(1.1f, 1.0f, 1.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = colVen[i];
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
			if (i >= 1) {	// las patas se balancean en diagonal: cada una gira sobre la articulacion de la panza
				for (int k = 0; k < 4; k++) {
					model = glm::translate(mVen, glm::vec3((k % 2 == 0 ? -0.75f : 0.75f), 1.3f, (k < 2 ? 0.6f : -0.6f)));
					model = glm::rotate(model, glm::radians(28.0f * sin(faseVen + ((k == 0 || k == 3) ? 0.0f : PI))), glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::translate(model, glm::vec3(0.0f, -1.3f, 0.0f));
					model = glm::scale(model, glm::vec3(0.25f, 1.3f, 0.25f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = colVen[i] * 0.8f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CILINDRO]->RenderMeshModel();
				}
			}
			if (i < 2) {	// la larva y el joven tienen dos antenas que se mecen
				for (int lado = -1; lado <= 1; lado += 2) {
					model = glm::translate(mVen, glm::vec3(lado * 0.4f, cuerpoYVen + 0.85f + 0.9f * saltoVen, 0.2f));
					model = glm::rotate(model, glm::radians(-lado * (22.0f + 12.0f * sin(3.0f * now + i))), glm::vec3(0.0f, 0.0f, 1.0f));
					model = glm::scale(model, glm::vec3(0.12f, 0.7f, 0.12f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = colVen[i] * 1.2f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CONO]->RenderMeshModel();
				}
			}
			if (i == 2) {	// el adulto tiene ademas dos brazos con pinza, dos cuernos y una cola
				for (int lado = -1; lado <= 1; lado += 2) {
					hVen = glm::translate(mVen, glm::vec3(lado * 1.15f, cuerpoYVen + 0.3f, 0.4f));	// hombro
					hVen = glm::rotate(hVen, glm::radians(-lado * 40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					hVen = glm::rotate(hVen, glm::radians(35.0f * sin(faseVen + (lado > 0 ? 0.0f : PI))), glm::vec3(1.0f, 0.0f, 0.0f));
					hVen = glm::translate(hVen, glm::vec3(0.0f, -1.0f, 0.0f));
					model = glm::scale(hVen, glm::vec3(0.22f, 1.0f, 0.22f));	// brazo
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = colVen[i] * 0.8f;
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CILINDRO]->RenderMeshModel();
					model = glm::translate(hVen, glm::vec3(0.0f, 0.0f, 0.0f));	// pinza: cono que apunta hacia abajo, abre y cierra
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.3f + 0.1f * sin(5.0f * now + lado), 0.6f, 0.3f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = glm::vec3(0.9f, 0.85f, 0.7f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CONO]->RenderMeshModel();
					// cuerno
					model = glm::translate(mVen, glm::vec3(lado * 0.55f, cuerpoYVen + 0.85f, 0.1f));
					model = glm::rotate(model, glm::radians(-lado * 30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					model = glm::scale(model, glm::vec3(0.22f, 0.9f, 0.22f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = glm::vec3(0.9f, 0.85f, 0.7f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CONO]->RenderMeshModel();
				}
				hVen = glm::translate(mVen, glm::vec3(0.0f, cuerpoYVen - 0.2f, -1.2f));	// cola: cono que apunta hacia atras y se mueve de lado a lado
				hVen = glm::rotate(hVen, glm::radians(25.0f * sin(2.0f * now)), glm::vec3(0.0f, 1.0f, 0.0f));
				hVen = glm::rotate(hVen, glm::radians(-120.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(hVen, glm::vec3(0.45f, 1.8f, 0.45f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = colVen[i] * 0.8f;
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CONO]->RenderMeshModel();
			}
			plainTexture.UseTexture();	// ojos con color liso
			// un solo ojo (el adulto lo tiene ambar); la pupila se mueve de lado a lado
			model = glm::translate(mVen, glm::vec3(0.0f, cuerpoYVen + 0.25f + 0.9f * saltoVen, 1.2f));
			model = glm::scale(model, glm::vec3(0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = i == 2 ? glm::vec3(1.0f, 0.7f, 0.1f) : glm::vec3(1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
			modelaux = model;
			model = glm::translate(modelaux, glm::vec3(0.18f * sin(1.7f * now + i), 0.0f, 0.65f));
			model = glm::scale(model, glm::vec3(0.55f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.05f, 0.05f, 0.05f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
		}

		// ---- LUNA de Majora's Mask (easter egg) ----
		// el modelo esta centrado en el origen, mide ~2100 de radio y tiene la cara hacia -X: se escala y se gira para que la cara mire a Venom
		model = glm::mat4(1.0);
		model = glm::translate(model, LUNA_POS);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(LUNA_RADIO / 2100.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.15f, 1.15f, 1.1f);	// un poco de brillo para que se vea de noche
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
		lunaMajoras.RenderModel();

		// ---- ARWING y WOLFEN volando sobre Venom ----
		color = glm::vec3(1.0f, 1.0f, 1.0f);	// blanco para que no tina la textura del modelo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
		model = glm::mat4(1.0);
		model = glm::translate(model, posArwing);
		model = glm::rotate(model, yawArwing, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -pitchArwing, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rollArwing, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(ARWING_ESCALA));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		arwingModel.RenderModel();
		for (int i = 0; i < 3; i++) {
			aW = wWolfen[i] * now + faseWolfen[i];
			bobW = 0.7f * now + 1.7f * i;	// sube y baja un poco
			pW = C_VENOM + glm::vec3(radioWolfen[i] * cosf(aW), altWolfen[i] + 2.0f * sinf(bobW), 0.8f * radioWolfen[i] * sinf(aW));
			vxW = -radioWolfen[i] * sinf(aW) * wWolfen[i]; vzW = 0.8f * radioWolfen[i] * cosf(aW) * wWolfen[i]; vyW = 1.4f * cosf(bobW);
			yW = atan2f(vxW, vzW);	// mira hacia donde va
			pitW = atan2f(vyW, sqrtf(vxW * vxW + vzW * vzW));
			rolW = wWolfen[i] > 0.0f ? 0.5f : -0.5f;	// se inclina hacia donde da la vuelta
			model = glm::mat4(1.0);
			model = glm::translate(model, pW);
			model = glm::rotate(model, yW, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, -pitW, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, rolW, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(WOLFEN_ESCALA));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			wolfenModel.RenderModel();
		}
		// naves que salen de las mini estaciones y la que despega del domo (la misma figura de Wolfen)
		for (int k = 0; k < N_ESTACIONES; k++) {
			cN = fmodf(now + 9.0f * k, CICLO_ESTACION);	// ciclo de 36 s: sale 14 s, afuera 4 s, regresa 14 s y se queda guardada 4 s
			if (cN < T_VUELO) { sN = cN / T_VUELO; sentidoN = 1.0f; }
			else if (cN >= 18.0f && cN < 18.0f + T_VUELO) { sN = 1.0f - (cN - 18.0f) / T_VUELO; sentidoN = -1.0f; }
			else continue;	// la nave esta guardada en el hangar
			aN = PI / 2.0f + k * 2.0f * PI / 3.0f;
			ePosN = C_VENOM + glm::vec3(R_ESTACION * cosf(aN), ALT_ESTACION + 0.6f * sinf(0.8f * now + 2.0f * k), R_ESTACION * sinf(aN));
			eYawN = atan2f(-cosf(aN), -sinf(aN));
			FN = glm::vec3(sinf(eYawN), 0.0f, cosf(eYawN));	// hacia donde mira la estacion
			s2N = glm::max(sN, 0.02f);	// para que la direccion nunca sea cero
			pN = ePosN + FN * (3.5f + 280.0f * powf(sN, 1.7f)) + glm::vec3(0.0f, 120.0f * powf(sN, 2.2f), 0.0f);
			dirN = glm::normalize(FN * (280.0f * 1.7f * powf(s2N, 0.7f)) + glm::vec3(0.0f, 120.0f * 2.2f * powf(s2N, 1.2f), 0.0f)) * sentidoN;
			yN = atan2f(dirN.x, dirN.z);
			pitN = asinf(glm::clamp(dirN.y, -1.0f, 1.0f));
			rolN = 0.0f;
			model = glm::mat4(1.0);
			model = glm::translate(model, pN);
			model = glm::rotate(model, yN, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, -pitN, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, rolN, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(WOLFEN_ESCALA));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			wolfenModel.RenderModel();
		}
		// ---- cohete de Venom (practica 10): la capsula siempre va; el cuerpo inferior y los propulsores solo mientras no se han soltado ----
		plainTexture.UseTexture();
		metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
		model = cohMV;
		model = glm::scale(model, glm::vec3(escalaCoheteV, escalaCoheteV, escalaCoheteV));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.95f, 0.95f, 1.0f);	// los modelos no traen textura: cada pieza se pinta con un color
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		cuerpoSuperiorV.RenderModel();
		if (!sepHechaV[1]) {
			color = glm::vec3(0.9f, 0.45f, 0.15f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			cuerpoInferiorV.RenderModel();
		}
		if (!sepHechaV[0]) {
			color = glm::vec3(0.75f, 0.75f, 0.78f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			propulsoresV.RenderModel();
		}
		// piezas sueltas: caen con su posicion y giran sobre su centro hasta quedar acostadas en el piso
		for (int k = 0; k < 2; k++) {
			if (!sepHechaV[k] || sepPosV[k].y < -50.0f) continue;
			model = glm::mat4(1.0);
			model = glm::translate(model, sepPosV[k]);
			model = glm::rotate(model, glm::radians(sepIncV[k]), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(escalaCoheteV, escalaCoheteV, escalaCoheteV));
			model = glm::translate(model, glm::vec3(0.0f, -sepPivoteV[k], 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = k == 0 ? glm::vec3(0.75f, 0.75f, 0.78f) : glm::vec3(0.9f, 0.45f, 0.15f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			if (k == 0) propulsoresV.RenderModel(); else cuerpoInferiorV.RenderModel();
		}





		// ---- postes de las luminarias L1 a L12 (el foco brilla de noche) ----
		for (int i = 0; i < 12; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, posLuminaria[i]);
			model = glm::scale(model, glm::vec3(0.2f, 4.0f, 0.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.25f, 0.25f, 0.25f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
			model = glm::mat4(1.0);
			model = glm::translate(model, posLuminaria[i] + glm::vec3(0.0f, 4.1f, 0.0f));
			model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			// de noche el foco se ve encendido (color mas fuerte para que resalte con la luz ambiental baja)
			color = esDeNoche ? glm::vec3(4.0f, 4.0f, 3.0f) : glm::vec3(0.8f, 0.8f, 0.7f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
		}


		//=====================================================================================
		// 8. AVATAR: alien insecto de un ojo, no humanoide; cuerpo con 3 patas, 2 brazos con pinza y una cola
		// jerarquia: el cuerpo es el padre y cada extremidad cuelga de su articulacion (pivote en el centro de su pieza)
		// caminata: A sin(wt + fase) en las patas (desfasadas un tercio de vuelta), brazos contrarios a las patas y cola que se mueve de lado
		//=====================================================================================
		{
			// pivotes de las piezas respecto al centro del cuerpo (en unidades del modelo): pata izq, pata der, pata frente, brazo izq, brazo der, cola
			// las uniones de las tres patas quedan dentro del cuerpo (la panza esta en y = -0.15 a los lados y en -0.225 al frente), asi no se ve el extremo abierto de cada pata
			pivoteAv[0] = glm::vec3(-0.1717f, -0.14f, -0.0474f);
			pivoteAv[1] = glm::vec3(0.162f, -0.14f, -0.0493f);
			pivoteAv[2] = glm::vec3(-0.0234f, -0.158f, 0.1128f);
			pivoteAv[3] = glm::vec3(-0.222f, 0.0551f, 0.0194f);
			pivoteAv[4] = glm::vec3(0.2192f, 0.0564f, 0.0231f);
			pivoteAv[5] = glm::vec3(-0.1159f, 0.1715f, 0.0788f);
			piezaAv[0] = &alienPataIzq;
			piezaAv[1] = &alienPataDer;
			piezaAv[2] = &alienPataFrente;
			piezaAv[3] = &alienBrazoIzq;
			piezaAv[4] = &alienBrazoDer;
			piezaAv[5] = &alienCola;
			ejeAv[0] = glm::vec3(1.0f, 0.0f, 0.0f);
			ejeAv[1] = glm::vec3(1.0f, 0.0f, 0.0f);
			ejeAv[2] = glm::vec3(1.0f, 0.0f, 0.0f);
			ejeAv[3] = glm::vec3(1.0f, 0.0f, 0.0f);
			ejeAv[4] = glm::vec3(1.0f, 0.0f, 0.0f);
			ejeAv[5] = glm::vec3(0.0f, 1.0f, 0.0f);
			anguloAv[0] = 24.0f * avatarAmp * sin(avatarFase);
			anguloAv[1] = 24.0f * avatarAmp * sin(avatarFase + 2.0f * PI / 3.0f);
			anguloAv[2] = 24.0f * avatarAmp * sin(avatarFase + 4.0f * PI / 3.0f);
			anguloAv[3] = 12.0f * avatarAmp * sin(avatarFase + PI) + 3.0f * sin(1.5f * now);
			anguloAv[4] = 12.0f * avatarAmp * sin(avatarFase) + 3.0f * sin(1.5f * now + 1.0f);
			anguloAv[5] = (10.0f + 12.0f * avatarAmp) * sin(3.0f * now);
			// escala de las piezas: cada pata se alarga en y hasta que su punta llegue a y = -0.7188 (el piso): (0.7188 - union) / largo del modelo
			// = 1.234 y 1.245 las de los lados y 1.119 la de enfrente, y un poco mas ancha para no verse delgada; al balancearse, cada pata se alarga
			// ademas 1/cos(angulo) para que su punta no se levante del piso
			escalaAv[0] = glm::vec3(1.07f, 1.234f / cos(glm::radians(anguloAv[0])), 1.07f);
			escalaAv[1] = glm::vec3(1.07f, 1.245f / cos(glm::radians(anguloAv[1])), 1.07f);
			escalaAv[2] = glm::vec3(1.04f, 1.119f / cos(glm::radians(anguloAv[2])), 1.04f);
			escalaAv[3] = glm::vec3(1.0f);
			escalaAv[4] = glm::vec3(1.0f);
			escalaAv[5] = glm::vec3(1.0f);
			baseAv = glm::translate(glm::mat4(1.0), avatarPos + glm::vec3(0.0f, 0.7188f * ESC_AV, 0.0f));	// con las patas paradas, las puntas tocan el piso
			baseAv = glm::rotate(baseAv, avatarGiro, glm::vec3(0.0f, 1.0f, 0.0f));	// el ojo del modelo mira a +Z
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			materialAlien.UseMaterial(uniformSpecularIntensity, uniformShininess);
			model = baseAv;
			model = glm::scale(model, glm::vec3(ESC_AV, ESC_AV, ESC_AV));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			alienCuerpo.RenderModel();
			for (int i = 0; i < 6; i++) {
				model = baseAv;
				model = glm::translate(model, pivoteAv[i] * ESC_AV);
				model = glm::rotate(model, glm::radians(anguloAv[i]), ejeAv[i]);
				model = glm::scale(model, glm::vec3(ESC_AV) * escalaAv[i]);
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				piezaAv[i]->RenderModel();
			}
		}


		//=====================================================================================
		// EFECTOS TRANSPARENTES (escudos de Reach, humo y llama del cohete): van al final y no escriben profundidad,
		// asi se ve lo de atras y no tapan a lo que se dibuje despues
		//=====================================================================================
		shaderList[2].UseShader();
		glUniformMatrix4fv(shaderList[2].GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(shaderList[2].GetViewLocation(), 1, GL_FALSE, glm::value_ptr(vista));
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);

		// ---- plasma azul del cohete de Venom: dos planos cruzados con la textura completa y blending aditivo; la parte ancha queda en la tobera ----
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glUniform1f(uniformAlphaEfecto, 1.0f);
		offsetEfecto = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformOffsetEfecto, 1, glm::value_ptr(offsetEfecto));
		plasmaTexture.UseTexture();
		for (int i = 0; i < MAX_PLASMA; i++) {
			if (plasmaEdad[i] < plasmaVida) {
				vidaP = plasmaEdad[i] / plasmaVida;	// 0 nace, 1 muere
				tamP = plasmaTam[i] * (1.0f + 1.5f * vidaP);	// se expande
				color = glm::vec3(1.2f, 1.2f, 1.2f) * (1.0f - vidaP);	// y se desvanece
				glUniform3fv(uniformColorEfecto, 1, glm::value_ptr(color));
				model = glm::mat4(1.0);
				model = glm::translate(model, plasmaPos[i]);
				model = glm::rotate(model, plasmaRumbo[i], glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, plasmaInc[i], glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, glm::vec3(0.0f, -0.5f * tamP, 0.0f));
				model = glm::scale(model, glm::vec3(0.5f * tamP, tamP, 0.5f * tamP));
				glUniformMatrix4fv(shaderList[2].GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
				meshListModel[PLASMA]->RenderMeshModel();
			}
		}



		offsetEfecto = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformOffsetEfecto, 1, glm::value_ptr(offsetEfecto));
		// ---- AC4 escudos de energia de Reach: pulsan cada 2 s ----
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1f(uniformAlphaEfecto, 0.4f + 0.15f * sin(now * PI));
		color = glm::vec3(0.35f, 0.75f, 1.0f);
		glUniform3fv(uniformColorEfecto, 1, glm::value_ptr(color));
		escudoReach.UseTexture();
		for (int i = 0; i < 2; i++) {
			model = mundoReach;
			model = glm::translate(model, glm::vec3(escudosHR[i].x, 0.2f, escudosHR[i].y));
			model = glm::scale(model, glm::vec3(2.1f, 2.3f, 1.0f));
			glUniformMatrix4fv(shaderList[2].GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
			meshListModel[ESCUDO]->RenderMeshModel();
		}
		// ---- AB1 humo del cohete: cada bocanada es un plano que mira a la camara; su cuadro depende de su edad ----
		glUniform1f(uniformAlphaEfecto, 1.0f);
		color = esDeNoche ? glm::vec3(0.45f, 0.45f, 0.5f) : glm::vec3(1.0f, 1.0f, 1.0f);	// de noche el humo se ve oscuro
		glUniform3fv(uniformColorEfecto, 1, glm::value_ptr(color));
		humoTexture.UseTexture();
		for (int i = 0; i < MAX_HUMO; i++) {
			if (humoEdad[i] < humoVida) {
				humoCuadro = (int)(humoEdad[i] / humoVida * 8.0f);
				if (humoCuadro > 7) humoCuadro = 7;
				offsetEfecto = glm::vec2(humoCuadro / 8.0f, 0.0f);
				glUniform2fv(uniformOffsetEfecto, 1, glm::value_ptr(offsetEfecto));
				humoTam = 1.4f + 4.3f * humoEdad[i] / humoVida;	// crece mientras se dispersa
				model = glm::mat4(1.0);
				model = glm::translate(model, humoPos[i]);
				model = glm::rotate(model, atan2(camPos.x - humoPos[i].x, camPos.z - humoPos[i].z), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(humoTam, humoTam, humoTam));
				glUniformMatrix4fv(shaderList[2].GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
				meshListModel[HUMO]->RenderMeshModel();
			}
		}
		// ---- AB1 llama del cohete: textura animada de 8 cuadros que se suma a lo de atras (blending aditivo) ----
		if (llamaIgn > 0.0f && faseCohete != 4) {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			offsetEfecto = glm::vec2(llamaCuadro / 8.0f, 0.0f);
			glUniform2fv(uniformOffsetEfecto, 1, glm::value_ptr(offsetEfecto));
			color = glm::vec3(1.6f, 1.6f, 1.6f);
			glUniform3fv(uniformColorEfecto, 1, glm::value_ptr(color));
			model = cohM;
			model = glm::translate(model, glm::vec3(0.0f, toberaEtapa[etapaEncendida] * escalaCohete, 0.0f));
			model = glm::scale(model, glm::vec3(llamaAncho[etapaEncendida], llamaLong, llamaAncho[etapaEncendida]));
			glUniformMatrix4fv(shaderList[2].GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
			llamaTexture.UseTexture();
			meshListModel[LLAMA]->RenderMeshModel();
		}
		// KF-P10: la llama de cada etapa, con el largo segun cuanto este encendida
		if (kcActivo) for (int k = 0; k < 3; k++) {
			if (kcLlama[k] < 0.01f) continue;
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			offsetEfecto = glm::vec2(llamaCuadro / 8.0f, 0.0f);
			glUniform2fv(uniformOffsetEfecto, 1, glm::value_ptr(offsetEfecto));
			color = glm::vec3(1.6f, 1.6f, 1.6f);
			glUniform3fv(uniformColorEfecto, 1, glm::value_ptr(color));
			alto = (toberaEtapa[k] - kcPivote[k]) * kcEscala;	// donde esta la tobera de esa etapa
			crece = kcEscala / escalaCohete;	// la llama crece igual que el cohete
			largo = llamaLargo[k] * crece * kcLlama[k] * (1.0f + 0.1f * sin(50.0f * now));
			model = glm::translate(glm::mat4(1.0), kcPos[k]);
			model = glm::rotate(model, glm::radians(kcGiro[k]), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::translate(model, glm::vec3(0.0f, alto, 0.0f));
			model = glm::scale(model, glm::vec3(llamaAncho[k] * crece, largo, llamaAncho[k] * crece));
			glUniformMatrix4fv(shaderList[2].GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
			llamaTexture.UseTexture();
			meshListModel[LLAMA]->RenderMeshModel();
		}
		offsetEfecto = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformOffsetEfecto, 1, glm::value_ptr(offsetEfecto));
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);


		//=====================================================================================
		// MENU DE TECLAS (se dibuja al final, encima de todo; M lo abre y lo cierra)
		// muestra solo las teclas del planeta donde esta la camara libre; en el espacio y en las otras camaras, las generales
		//=====================================================================================
		{
			zona = 3;
			if (modoCamara == 1)
				for (int i = 0; i < 3; i++)
					if (glm::distance(glm::vec2(camPos.x, camPos.z), glm::vec2(centroPlaneta[i].x, centroPlaneta[i].z)) < radioPlaneta[i] + 15.0f) zona = i;
			shaderList[1].UseShader();
			orto = glm::ortho(0.0f, (float)anchoVentana, 0.0f, (float)altoVentana);	// coordenadas en pixeles, el (0, 0) abajo a la izquierda
			glUniformMatrix4fv(shaderList[1].GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(orto));
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			margen = 0.015f * anchoVentana;
			ancho = menuAbierto ? 0.3f * anchoVentana : 0.17f * anchoVentana;
			altoMenu[0] = 609.0f;
			altoMenu[1] = 729.0f;
			altoMenu[2] = 549.0f;
			altoMenu[3] = 789.0f;	// alto en pixeles de cada panel (todos miden 1024 de ancho)
			alto = menuAbierto ? ancho * altoMenu[zona] / 1024.0f : ancho * 96.0f / 512.0f;
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(margen, altoVentana - margen - alto, 0.0f));
			model = glm::scale(model, glm::vec3(ancho, alto, 1.0f));
			glUniformMatrix4fv(shaderList[1].GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
			if (menuAbierto) menuTexture[zona].UseTexture(); else pestanaTexture.UseTexture();
			meshListModel[CUADRO]->RenderMeshModel();
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
