// Práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>
// glm
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtc\random.hpp>
// clases para dar orden y limpieza al código
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

// pirámide con color por vértice
MeshColor* piramideColor;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20);

void CrearCubo() {
    unsigned int cubo_indices[] = { 0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7, 4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3 };
    GLfloat cubo_vertices[] = { -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f };
    Mesh* cubo = new Mesh();
    cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
    meshList.push_back(cubo);
}

void CrearPiramideTriangular() {
    unsigned int indices_piramide_triangular[] = { 0,1,2, 1,3,2, 3,0,2, 1,0,3 };
    GLfloat vertices_piramide_triangular[] = { -0.5f, -0.5f,0.0f,  0.5f,-0.5f,0.0f,  0.0f,0.5f, -0.25f,  0.0f,-0.5f,-0.5f };
    Mesh* piramidet = new Mesh();
    piramidet->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
    meshList.push_back(piramidet);
}

void CrearPiramideCuadrangular() {
    unsigned int piramidecuadrangular_indices[] = { 0,3,4, 3,2,4, 2,1,4, 1,0,4, 0,1,2, 0,2,3 };
    GLfloat piramidecuadrangular_vertices[] = { 0.5f,-0.5f,0.5f,  0.5f,-0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,  -0.5f,-0.5f,0.5f,  0.0f,0.5f,0.0f };
    Mesh* piramidec = new Mesh();
    piramidec->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
    meshList.push_back(piramidec);
}

void CrearCilindro(int res, float R) {
    int n, i;
    GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
    vector<GLfloat> vertices;
    vector<unsigned int> indices;

    for (n = 0; n <= (res); n++) {
        if (n != res) { x = R * cos((n)*dt); z = R * sin((n)*dt); }
        else { x = R * cos((0) * dt); z = R * sin((0) * dt); }
        for (i = 0; i < 6; i++) {
            switch (i) {
            case 0: vertices.push_back(x); break; case 1: vertices.push_back(y); break;
            case 2: vertices.push_back(z); break; case 3: vertices.push_back(x); break;
            case 4: vertices.push_back(0.5); break; case 5: vertices.push_back(z); break;
            }
        }
    }
    for (n = 0; n <= (res); n++) {
        x = R * cos((n)*dt); z = R * sin((n)*dt);
        for (i = 0; i < 3; i++) {
            switch (i) {
            case 0: vertices.push_back(x); break; case 1: vertices.push_back(-0.5f); break;
            case 2: vertices.push_back(z); break;
            }
        }
    }
    for (n = 0; n <= (res); n++) {
        x = R * cos((n)*dt); z = R * sin((n)*dt);
        for (i = 0; i < 3; i++) {
            switch (i) {
            case 0: vertices.push_back(x); break; case 1: vertices.push_back(0.5); break;
            case 2: vertices.push_back(z); break;
            }
        }
    }
    for (i = 0; i < vertices.size(); i++) indices.push_back(i);
    Mesh* cilindro = new Mesh();
    cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
    meshList.push_back(cilindro);
}

void CrearCono(int res, float R) {
    int n, i;
    GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
    vector<GLfloat> vertices;
    vector<unsigned int> indices;

    vertices.push_back(0.0); vertices.push_back(0.5); vertices.push_back(0.0);
    for (n = 0; n <= (res); n++) {
        x = R * cos((n)*dt); z = R * sin((n)*dt);
        for (i = 0; i < 3; i++) {
            switch (i) {
            case 0: vertices.push_back(x); break; case 1: vertices.push_back(y); break;
            case 2: vertices.push_back(z); break;
            }
        }
    }
    vertices.push_back(R * cos(0) * dt); vertices.push_back(-0.5); vertices.push_back(R * sin(0) * dt);
    for (i = 0; i < res + 2; i++) indices.push_back(i);
    Mesh* cono = new Mesh();
    cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
    meshList.push_back(cono);
}

// Pirámide de la actividad 2 (figura del Juego de Ender)
// piramideColor tiene sus 4 caras de distinto color y la base azul
void CrearPiramideColor() {
    GLfloat vertices[] = {
        // base (azul)
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 1.0f,
        // cara de enfrente (roja)
        -0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,
         0.0f,  0.5f,  0.0f,     1.0f, 0.0f, 0.0f,
         // cara derecha (verde)
          0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,
          0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,
          0.0f,  0.5f,  0.0f,     0.0f, 1.0f, 0.0f,
          // cara de atrás (amarilla)
           0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,
          -0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,
           0.0f,  0.5f,  0.0f,     1.0f, 1.0f, 0.0f,
           // cara izquierda (magenta)
           -0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 1.0f,
           -0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 1.0f,
            0.0f,  0.5f,  0.0f,     1.0f, 0.0f, 1.0f
    };
    piramideColor = new MeshColor();
    piramideColor->CreateMeshColor(vertices, 18);
}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);

    Shader* shader2 = new Shader();
    shader2->CreateFromFiles(vShaderColor, fShader);
    shaderList.push_back(*shader2);
}

int main()
{
    mainWindow = Window(800, 600);
    mainWindow.Initialise();

    CrearCubo();
    CrearPiramideTriangular();
    CrearCilindro(30, 1.0f);
    CrearCono(25, 2.0f);
    CrearPiramideCuadrangular();
    CrearPiramideColor();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 1.5f, 0.2f);

    GLuint uniformProjection = 0;
    GLuint uniformModel = 0;
    GLuint uniformView = 0;

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

    sp.init(); sp.load();
    glm::mat4 model(1.0);

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS;
        lastTime = now;

        glfwPollEvents();
        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*
        // =========================================================
        // ACTIVIDAD 1: COHETE ESPACIAL (Comentado y guardado)
        // =========================================================
        // Para usarlo, borra el /* de arriba y el */ /* de abajo, y comenta la Actividad 2.

        shaderList[0].useShader();
        uniformModel = shaderList[0].getModelLocation();
        uniformProjection = shaderList[0].getProjectLocation();
        uniformView = shaderList[0].getViewLocation();
        GLuint uniformColor = shaderList[0].getColorLocation();

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

        glm::mat4 baseCohete(1.0);
        baseCohete = glm::translate(baseCohete, glm::vec3(0.0f, 0.0f, -10.0f));
        baseCohete = glm::rotate(baseCohete, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
        baseCohete = glm::rotate(baseCohete, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
        baseCohete = glm::rotate(baseCohete, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 modelCohete;
        glm::vec3 colorCohete;

        // Cuerpo Principal
        modelCohete = baseCohete;
        modelCohete = glm::scale(modelCohete, glm::vec3(1.0f, 3.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCohete));
        colorCohete = glm::vec3(0.85f, 0.85f, 0.85f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(colorCohete));
        meshList[2]->RenderMeshGeometry();

        // Punta del Cohete
        modelCohete = baseCohete;
        modelCohete = glm::translate(modelCohete, glm::vec3(0.0f, 2.25f, 0.0f));
        modelCohete = glm::scale(modelCohete, glm::vec3(0.5f, 1.5f, 0.5f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCohete));
        colorCohete = glm::vec3(0.8f, 0.1f, 0.1f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(colorCohete));
        meshList[3]->RenderMeshGeometry();

        // Aletas Estabilizadoras
        for(int i = 0; i < 4; i++) {
            modelCohete = baseCohete;
            modelCohete = glm::rotate(modelCohete, glm::radians(i * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelCohete = glm::translate(modelCohete, glm::vec3(1.0f, -1.0f, 0.0f));
            modelCohete = glm::rotate(modelCohete, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            modelCohete = glm::scale(modelCohete, glm::vec3(0.5f, 1.0f, 0.1f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCohete));
            colorCohete = glm::vec3(0.8f, 0.1f, 0.1f);
            glUniform3fv(uniformColor, 1, glm::value_ptr(colorCohete));
            meshList[4]->RenderMesh();
        }

        // Marco Ventana
        modelCohete = baseCohete;
        modelCohete = glm::translate(modelCohete, glm::vec3(0.0f, 0.5f, 0.95f));
        modelCohete = glm::scale(modelCohete, glm::vec3(0.6f, 0.6f, 0.1f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCohete));
        colorCohete = glm::vec3(0.3f, 0.3f, 0.3f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(colorCohete));
        meshList[0]->RenderMesh();

        // Cristal Ventana
        modelCohete = baseCohete;
        modelCohete = glm::translate(modelCohete, glm::vec3(0.0f, 0.5f, 1.0f));
        modelCohete = glm::scale(modelCohete, glm::vec3(0.25f, 0.25f, 0.05f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCohete));
        colorCohete = glm::vec3(0.0f, 0.7f, 1.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(colorCohete));
        sp.render();

        // Tobera Motor
        modelCohete = baseCohete;
        modelCohete = glm::translate(modelCohete, glm::vec3(0.0f, -1.6f, 0.0f));
        modelCohete = glm::rotate(modelCohete, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelCohete = glm::scale(modelCohete, glm::vec3(1.0f, 0.8f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCohete));
        colorCohete = glm::vec3(0.2f, 0.2f, 0.2f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(colorCohete));
        meshList[1]->RenderMesh();

        // Fuego Propulsor Animado
        glm::mat4 fireModel = baseCohete;
        fireModel = glm::translate(fireModel, glm::vec3(0.0f, -2.0f, 0.0f));
        float fireScale = 0.5f + (sin(now * 25.0f) * 0.1f);
        fireModel = glm::scale(fireModel, glm::vec3(fireScale, fireScale * 2.0f, fireScale));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(fireModel));
        colorCohete = glm::vec3(1.0f, 0.6f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(colorCohete));
        sp.render();
        */


        // =========================================================
        // ACTIVIDAD 2: figura del Juego de Ender (8 pirámides juntas)
        // =========================================================
        shaderList[1].useShader();
        uniformModel = shaderList[1].getModelLocation();
        uniformProjection = shaderList[1].getProjectLocation();
        uniformView = shaderList[1].getViewLocation();

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

        glm::mat4 baseEstrella(1.0);
        baseEstrella = glm::translate(baseEstrella, glm::vec3(0.0f, 0.0f, -5.0f));
        baseEstrella = glm::rotate(baseEstrella, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
        baseEstrella = glm::rotate(baseEstrella, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
        baseEstrella = glm::rotate(baseEstrella, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

        // giro para que se vea como diamante y no como cruz derecha
        baseEstrella = glm::rotate(baseEstrella, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        // giro extra para que quede en el mismo ángulo que la imagen que nos dieron
        baseEstrella = glm::rotate(baseEstrella, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        baseEstrella = glm::rotate(baseEstrella, glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // ángulos para acomodar los 4 brazos en cruz (arriba, abajo, derecha, izquierda)
        float angulosBrazos[4] = { 0.0f, 180.0f, -90.0f, 90.0f };

        // este giro es para que cada brazo muestre una cara distinta
        // (si no se pone, todos los brazos salen del mismo color)
        float giroColor[4] = { 0.0f, 90.0f, 180.0f, 270.0f };

        // aquí guardamos las 8 matrices (2 pirámides por brazo) para no
        // tener que calcularlas otra vez en el segundo dibujado
        glm::mat4 matrices[8];
        int idx = 0;

        for (int i = 0; i < 4; i++) {
            glm::mat4 baseBrazo = glm::rotate(baseEstrella, glm::radians(angulosBrazos[i]), glm::vec3(0.0f, 0.0f, 1.0f));
            baseBrazo = glm::rotate(baseBrazo, glm::radians(giroColor[i]), glm::vec3(0.0f, 1.0f, 0.0f));

            // pirámide de adentro (va volteada para cerrar el rombo)
            glm::mat4 inner = baseBrazo;
            inner = glm::scale(inner, glm::vec3(0.85f, 0.5f, 0.85f));
            inner = glm::translate(inner, glm::vec3(0.0f, 0.6f, 0.0f));
            inner = glm::rotate(inner, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            matrices[idx++] = inner;

            // pirámide de afuera (la punta que sobresale del brazo)
            glm::mat4 outer = baseBrazo;
            outer = glm::scale(outer, glm::vec3(0.85f, 0.5f, 0.85f));
            outer = glm::translate(outer, glm::vec3(0.0f, 2.0f, 0.0f));
            matrices[idx++] = outer;
        }

        // dibujamos las 8 pirámides de colores
        for (int i = 0; i < 8; i++) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrices[i]));
            piramideColor->RenderMeshColor();
        }

        glUseProgram(0);
        mainWindow.swapBuffers();
    }
    return 0;
}