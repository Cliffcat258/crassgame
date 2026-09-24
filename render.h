#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "glad.h"
#include <GLFW/glfw3.h>
#include "base.h"
#include "globalvars.h"
#include "structs.h"
#include "render2.h"

unsigned int VBO;
unsigned int VAO;
unsigned int shaderProgram;
renderContext *r;
float timev;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

GLFWwindow* RenderInit() { //OpenGL stuff and GLFW idk
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	
	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
	    printf("Failed to create GLFW window");
	    glfwTerminate();
	    return NULL;
	}
	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed To Initialize GLAD");
		return NULL;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

	//shaders
	if(0) {
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char *vertexShaderSource = readfile("shader.vert")->d;
	glShaderSource(vertexShader, 1, (const GLchar * const*)&vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success) {
	    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR loading shaders: %s\n", infoLog);
	}
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char *fragmentShaderSource = readfile("shader.frag")->d;
	glShaderSource(fragmentShader, 1, (const GLchar * const*)&fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success) {
	    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR loading shaders: %s\n", infoLog);
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	//idk
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	float vertices[] = { -0.5, -0.5, 0.0, 0.5, -0.5, 0.0, 0.0,  0.5, 0.0 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}
	//shorter version
	r = malloc(sizeof(*r));
	*r = rc_new("shader.vert", "shader.frag");
	
	list4f vertices = list4f_new(0);
	float verticeso[] = { 0.5f,  0.5f, 5.0f, 0.5f, -0.5f, 5.0f, -0.5f,  0.5f, 5.0f, 0.5f, -0.5f, 5.0f, -0.5f, -0.5f, 5.0f, -0.5f,  0.5f, 5.0f };
	list4f_add(&verticeso[0], sizeof(verticeso), &vertices);
	list4 indices = list4_new(0);
	int indiceso[] = { 0, 1, 2, 3, 4, 5 };
	list4_add(&indiceso[0], sizeof(indiceso), &indices);

	rc_update(r, &vertices, &indices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	return window;
}

void OnFrameUpdate(GLFWwindow *window) {
	glfwPollEvents();
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
	}
	return;
}

void OnFrameRender(GLFWwindow *window) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if(0) {
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	rc_use(r);

	vec3 v = vec3_new(0, 1, 0);
	view = CreateFromAxisAngle(&v, timev);
	timev += 0.01;
	int location = glGetUniformLocation(r->shaderProgram, "view");
	glUniformMatrix4fv(location, 1, GL_FALSE, &view.d[0]);
	projection = CreatePerspective(80, 1.5, 0.0001, 10000);
	int location1 = glGetUniformLocation(r->shaderProgram, "projection");
	glUniformMatrix4fv(location1, 1, GL_FALSE, &projection.d[0]);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glfwSwapBuffers(window);
	return;
}
