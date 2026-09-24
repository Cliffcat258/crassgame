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

typedef struct {
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	unsigned int shaderProgram;
} renderContext;

renderContext rc_new(char vertSource[], char fragSource[]) {
	renderContext a;
	a.vao = 0; glGenVertexArrays(1, &a.vao);
	a.vbo = 0; glGenBuffers(1, &a.vbo);
	a.ebo = 0; glGenBuffers(1, &a.ebo);
	//vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char *vertexShaderSource = readfile(vertSource)->d;
	glShaderSource(vertexShader, 1, (const GLchar * const*)&vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success) {
	    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR loading shaders: %s\n", infoLog);
	}
	//fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char *fragmentShaderSource = readfile(fragSource)->d;
	glShaderSource(fragmentShader, 1, (const GLchar * const*)&fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success) {
	    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR loading shaders: %s\n", infoLog);
	}

	a.shaderProgram = glCreateProgram();
	glAttachShader(a.shaderProgram, vertexShader);
	glAttachShader(a.shaderProgram, fragmentShader);
	glLinkProgram(a.shaderProgram);

	glGetProgramiv(a.shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
	    glGetProgramInfoLog(a.shaderProgram, 512, NULL, infoLog);
	    printf("ERROR linking program: %s\n", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return a;
}

void rc_update(renderContext* r, list4f *vertices, list4 *indices) { //VertexAttribPointer should be done right after this function call or whenever ig
	glBindVertexArray(r->vao);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices->size * sizeof(float), vertices->d, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size * sizeof(int), indices->d, GL_DYNAMIC_DRAW);
	
	glUseProgram(r->shaderProgram);
}

void rc_use(renderContext* r) {
	glBindVertexArray(r->vao);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->ebo);
	glUseProgram(r->shaderProgram);
}
