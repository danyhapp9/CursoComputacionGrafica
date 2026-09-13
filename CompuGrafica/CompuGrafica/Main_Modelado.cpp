//Daniel Imanol Gomez Varela
//Practica 4
//LabComputacion Grafica grupo 14
//12 septiembre 2026

#include<iostream>

//#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shaders
#include "Shader.h"

void Inputs(GLFWwindow* window);


void DrawCube(GLint modelLoc, GLint colorLoc,  //se tiene lo necesario para crear instancia del cubo base con caracteristicas
	const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& color,
	float angleDeg = 0.0f, const glm::vec3& axis = glm::vec3(1.0f, 0.0f, 0.0f))
{
	glm::mat4 model = glm::mat4(1.0f);             //matriz identidad
	model = glm::translate(model, pos);				//identidad por traslacion 
	if (angleDeg != 0.0f)
		model = glm::rotate(model, glm::radians(angleDeg), axis);
	model = glm::scale(model, scale);            //escalada del cubo

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));      //manda matriz armada a la gpu
	glUniform3fv(colorLoc, 1, glm::value_ptr(color));                    //color al fragment shader
	glDrawArrays(GL_TRIANGLES, 0, 36);   //se manda a dibujar los triangulos en el VAo en lazado
}

const GLint WIDTH = 800, HEIGHT = 600;
float movX = 0.0f;
float movY = 0.0f;
float movZ = -10.0f;
float rot = 0.0f;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Daniel Gomez Practica 4 ", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialise GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	Shader ourShader("Shader/core.vs", "Shader/core.frag");

	// Cubo bas con SOLO posiciones (3 floats por vertice). Ya no hay atributo de color.
	float vertices[] = {
		-0.5f, -0.5f, 0.5f,//Front
		 0.5f, -0.5f, 0.5f,
		 0.5f,  0.5f, 0.5f,
		 0.5f,  0.5f, 0.5f,
		-0.5f,  0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,

		-0.5f, -0.5f,-0.5f,//Back
		 0.5f, -0.5f,-0.5f,
		 0.5f,  0.5f,-0.5f,
		 0.5f,  0.5f,-0.5f,
		-0.5f,  0.5f,-0.5f,
		-0.5f, -0.5f,-0.5f,

		 0.5f, -0.5f,  0.5f,//Right
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,//Left
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,//Bottom
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,//Top
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Posicion (unico atributo, stride = 3 floats)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glm::mat4 projection = glm::mat4(1);
	projection = glm::perspective(glm::radians(45.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window))
	{
		Inputs(window);
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.Use();

		glm::mat4 view = glm::mat4(1);
		view = glm::translate(view, glm::vec3(movX, movY, movZ));
		view = glm::rotate(view, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));

		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projecLoc = glGetUniformLocation(ourShader.Program, "projection");
		GLint colorLoc = glGetUniformLocation(ourShader.Program, "objectColor");

		glUniformMatrix4fv(projecLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glBindVertexArray(VAO);

		glm::vec3 colorCuerpo(0.95f, 0.68f, 0.72f);
		glm::vec3 colorOscuro(0.85f, 0.50f, 0.56f);
		glm::vec3 colorNegro(0.05f, 0.05f, 0.05f);

		// CUERPO 
		DrawCube(modelLoc, colorLoc, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.2f, 1.6f, 3.4f), colorCuerpo);

		//  CABEZA 
		DrawCube(modelLoc, colorLoc, glm::vec3(0.0f, 0.55f, 2.35f), glm::vec3(1.7f, 1.55f, 1.5f), colorCuerpo);

		//  HOCICO 
		DrawCube(modelLoc, colorLoc, glm::vec3(0.0f, 0.05f, 3.35f), glm::vec3(0.9f, 0.7f, 0.55f), colorOscuro);
		DrawCube(modelLoc, colorLoc, glm::vec3(-0.22f, 0.05f, 3.66f), glm::vec3(0.12f, 0.18f, 0.06f), colorNegro); //fosa izq
		DrawCube(modelLoc, colorLoc, glm::vec3(0.22f, 0.05f, 3.66f), glm::vec3(0.12f, 0.18f, 0.06f), colorNegro); //fosa der

		// OJOS 
		DrawCube(modelLoc, colorLoc, glm::vec3(-0.5f, 0.85f, 3.05f), glm::vec3(0.16f, 0.16f, 0.06f), colorNegro);
		DrawCube(modelLoc, colorLoc, glm::vec3(0.5f, 0.85f, 3.05f), glm::vec3(0.16f, 0.16f, 0.06f), colorNegro);

		// OREJAS
		DrawCube(modelLoc, colorLoc, glm::vec3(-0.55f, 1.5f, 1.95f), glm::vec3(0.5f, 0.55f, 0.15f), colorCuerpo);
		DrawCube(modelLoc, colorLoc, glm::vec3(0.55f, 1.5f, 1.95f), glm::vec3(0.5f, 0.55f, 0.15f), colorCuerpo);

		// PATAS 
		DrawCube(modelLoc, colorLoc, glm::vec3(0.85f, -1.05f, 1.3f), glm::vec3(0.55f, 1.0f, 0.55f), colorCuerpo);   //frente-derecha
		DrawCube(modelLoc, colorLoc, glm::vec3(-0.85f, -1.05f, 1.3f), glm::vec3(0.55f, 1.0f, 0.55f), colorCuerpo);  //frente-izquierda
		DrawCube(modelLoc, colorLoc, glm::vec3(0.85f, -1.05f, -1.3f), glm::vec3(0.55f, 1.0f, 0.55f), colorCuerpo);  //atras-derecha
		DrawCube(modelLoc, colorLoc, glm::vec3(-0.85f, -1.05f, -1.3f), glm::vec3(0.55f, 1.0f, 0.55f), colorCuerpo); //atras-izquierda

		// COLA 
		DrawCube(modelLoc, colorLoc, glm::vec3(0.0f, 0.7f, -1.85f), glm::vec3(0.22f, 0.22f, 0.55f), colorOscuro);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return EXIT_SUCCESS;
}

void Inputs(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		movX += 0.04f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		movX -= 0.04f;
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		movY += 0.04f;
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		movY -= 0.04f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		movZ -= 0.04f;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		movZ += 0.04f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rot += 0.4f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rot -= 0.4f;
}