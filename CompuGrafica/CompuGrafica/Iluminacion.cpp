//Daniel Imanol Gomez Varela
//Practica 8
// 10 octubre 2026


// Std. Includes
#include <string>
#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"
#include "stb_image.h"
// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();


// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
float rot = 0.0f;
bool activanim = false;

//  SOL Y LUNA 
// Un solo angulo controla a los dos cubos 
float angulo = 90.0f;                              // 90 grados = sol en lo mas alto (mediodia)
const float PASO_ROTACION = 5.0f;                  // grados que gira cada vez que se presiona O o L
const float RADIO_ORBITA = 4.0f;                   // radio del circulo que recorren
const glm::vec3 CENTRO_ORBITA(0.0f, 0.0f, 0.0f);   // centro de la orbita (ajustalo si no los ves)

// Colores del cielo
const glm::vec3 CIELO_DIA(0.53f, 0.81f, 0.92f);
const glm::vec3 CIELO_NOCHE(0.02f, 0.02f, 0.08f);

// Colores de los cubos (encendido / apagado)
const glm::vec3 SOL_ENCENDIDO(1.0f, 0.85f, 0.1f);
const glm::vec3 SOL_APAGADO(0.25f, 0.2f, 0.05f);
const glm::vec3 LUNA_ENCENDIDA(0.85f, 0.9f, 1.0f);
const glm::vec3 LUNA_APAGADA(0.2f, 0.2f, 0.25f);
// 

int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica  8 Daniel Gomez", nullptr, nullptr);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Set the required callback functions
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Define the viewport dimensions
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Setup and compile our shaders
    Shader shader("Shader/modelLoading.vs", "Shader/modelLoading.frag");
    Shader lampshader("Shader/lamp.vs", "Shader/lamp.frag");
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");

    // Load models
    Model red_dog((char*)"Models/RedDog.obj");
    Model micro((char*)"Models/microfono.obj");
    Model bocina((char*)"Models/bafle.obj");
    Model piano((char*)"Models/piano.obj");
    Model silla((char*)"Models/silla.obj");
    Model escenario((char*)"Models/emptystagedesing.obj");


    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    float vertices[] = {
      -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    // First, set the container's VAO (and VBO)
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load textures
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    int textureWidth, textureHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

    image = stbi_load("Models/Texture_albedo.jpg", &textureWidth, &textureHeight, &nrChannels, 0);
    if (image)   
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(image);


    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        DoMovement();

        //1) Posicion del sol y la luna (coordenadas polares) 
        float rad = glm::radians(angulo);
        glm::vec3 dir(cos(rad), sin(rad), 0.0f);                  // punto en el circulo unitario
        glm::vec3 posSol = CENTRO_ORBITA + dir * RADIO_ORBITA;
        glm::vec3 posLuna = CENTRO_ORBITA - dir * RADIO_ORBITA;   // lado opuesto

        // 2) Dia o noche: depende de quien esta arriba 
        float altura = sin(rad);          // +1 = sol arriba, -1 = luna arriba
        bool esDeDia = (altura >= 0.0f);

        // 3) Color del cielo con transicion suave (atardecer) 
        float t = glm::smoothstep(-0.2f, 0.2f, altura);   // 0 = noche, 1 = dia
        glm::vec3 cielo = glm::mix(CIELO_NOCHE, CIELO_DIA, t);
        glClearColor(cielo.r, cielo.g, cielo.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //  4) Elegir la luz activa 
        glm::vec3 posLuz, luzAmbient, luzDiffuse, luzSpecular;
        if (esDeDia)
        {
            posLuz = posSol;
            luzAmbient = glm::vec3(0.3f, 0.3f, 0.3f);
            luzDiffuse = glm::vec3(1.0f, 0.95f, 0.8f);   // luz calida
            luzSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        else
        {
            posLuz = posLuna;
            luzAmbient = glm::vec3(0.05f, 0.05f, 0.1f);
            luzDiffuse = glm::vec3(0.25f, 0.3f, 0.6f);   // luz fria y tenue
            luzSpecular = glm::vec3(0.3f, 0.3f, 0.5f);
        }

        lightingShader.Use();
        GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "light.position");
        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(lightPosLoc, posLuz.x, posLuz.y, posLuz.z);
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // Set lights properties
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.ambient"), luzAmbient.x, luzAmbient.y, luzAmbient.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.diffuse"), luzDiffuse.x, luzDiffuse.y, luzDiffuse.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.specular"), luzSpecular.x, luzSpecular.y, luzSpecular.z);

        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Set material properties
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.ambient"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 1.0f, 1.0f, 1.0f); // blanco: la textura muestra sus colores reales
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f); 

        // Draw the loaded model
        glm::mat4 model(1);
        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        red_dog.Draw(lightingShader);

        //Piano
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f, -1.0f, -3.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        piano.Draw(lightingShader);

        //Silla
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f, -1.5f, -2.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        silla.Draw(lightingShader);

        // Micrófono
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, -1.4f, -0.7f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        micro.Draw(lightingShader);

        // Bocina derecha
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.5f, -1.2f, -1.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        bocina.Draw(lightingShader);

        // Bocina izquierda (el mismo modelo, dibujado otra vez en otra posición)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.5f, -1.2f, -1.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        bocina.Draw(lightingShader);

        // escenario
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.5f, -2.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        escenario.Draw(lightingShader);



        glBindVertexArray(0);


        //  5) Dibujar los dos cubos (sol y luna) 
        lampshader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        GLint modelLampLoc = glGetUniformLocation(lampshader.Program, "model");
        GLint lampColorLoc = glGetUniformLocation(lampshader.Program, "lampColor");
        glBindVertexArray(VAO);

        // Cubo 1: Sol
        glm::vec3 colorSol = esDeDia ? SOL_ENCENDIDO : SOL_APAGADO;
        model = glm::mat4(1.0f);
        model = glm::translate(model, posSol);
        model = glm::scale(model, glm::vec3(0.5f));
        glUniformMatrix4fv(modelLampLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(lampColorLoc, colorSol.x, colorSol.y, colorSol.z);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Cubo 2: Luna
        glm::vec3 colorLuna = esDeDia ? LUNA_APAGADA : LUNA_ENCENDIDA;
        model = glm::mat4(1.0f);
        model = glm::translate(model, posLuna);
        model = glm::scale(model, glm::vec3(0.4f));
        glUniformMatrix4fv(modelLampLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(lampColorLoc, colorLuna.x, colorLuna.y, colorLuna.z);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}


// Moves/alters the camera positions based on user input
void DoMovement()
{
    // Camera controls
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (activanim)
    {
        if (rot > -90.0f)
            rot -= 0.1f;
    }

}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }

    // Rotacion del sol y la luna
    if (keys[GLFW_KEY_O])
    {
        angulo += PASO_ROTACION;
    }

    if (keys[GLFW_KEY_L])
    {
        angulo -= PASO_ROTACION;
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}