#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "base.h"
#include "structs.h"

//all the global variables so code is cleaner

//
//most important things ig //
//

double pi = 3.1415926535897932385;
/*Chunk16[]*/list8 *chunksglobal; // deliberately very bad name ofc
/*Model[]*/list8 *modelsglobal; // also misleading but idc
////Vector3[] savedchunks = new Vector3[1000];
////Dictionary<Vector3, int> offsetdict = new Dictionary<Vector3, int>();

//
//vao, vbo and shaders //
//

int VertexArrayObject;
int VertexArrayObject2;
int VertexArrayObject3;

int VertexBufferObject;
int VertexBufferObject2;
int VertexBufferObject3;
int VertexBufferObject4;

int ElementBufferObject;
int ElementBufferObject2;
int ElementBufferObject3;
int ElementBufferObject4;


Shader shader;
Shader shader2;
Shader shader3;
Shader shader4;
Shader shader5;
Shader shader6;

//
//other gpu related things //
//

int screenwidth2 = 640; int screenheight2 = 480;
int screenwidth; int screenheight;
float near; float far;
float fov;

mat4 projection;// = mat4_identity();
mat4 translation;// = mat4_identity();
mat4 scale;// = /*Matrix4.CreateScale(1, 2, 1);*/mat4_scale(2);
mat4 rotation;// = mat4_identity();
mat4 view;
mat4 view2;

int shadowtexture;
int shadowtexture2;
int shadowtexture3;

int maindepthtexture;
int fbo3;
int fbo4;
int fbo5;
int fbo6;
int skytexture;
int maintexture;

float fogstrength = 0.004;
int shadowq = 4;
int skyquality;

//
// timers //
//
////Stopwatch timer = Stopwatch.StartNew();
////Stopwatch particletimer = Stopwatch.StartNew();	
////Stopwatch timeglobal = Stopwatch.StartNew();

//
// verts, indices //
//

/*uint[]*/list4 indices;
float vertices4[0]; uint indices4[0];
float vertices5[0]; uint indices5[0];
float vertices6[0]; uint indices6[0];
////float vertsglobal[][];
////uint indicesglobal[][];

//
// others //
//

Camera camerac;

int chunkdist;
	
float speed = 0.2;
float speed2 = 40;
////double time;

vec3 oldpos;

//List<Entity> entitiesglobal = new List<Entity>();

/*bool*/int meshchanged2 = 0;

Particle particles[0];/* = new Particle[500];*/
////private static readonly object _lock = new();

vec3 forward;
vec3 right;
vec3 up;

////Task<bool> physicstask = null;
////Task<(float[][], uint[][])> remeshtask = null;
////Task playertask = null;

int meshchanged = 0;
