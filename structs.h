#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "base.h"

//made for simpler functions and all the structs so that I could test them independently

typedef struct {
	int inited; int changed; int changed2;
	int empty; //if chunk only contains air
	/*int[4096]*/list4 bs; vec3 offset;
	int shouldbesaved;
} Chunk16;

typedef struct {
	float size;
	vec3 pos;
	int color;
	vec3 move;
	float timer;
	float timer2; //used for collision detection
} Particle;

typedef struct {
	int id;
	int aitype; //0 for static, 1 for simple movement, 2 for pathfind etc idk //outdated lol
	int model;
	vec3 pos;
	float scale;
	int mode; //for advanced ai or smth, probably never gonna touch this
	int target;
	vec3 targetpos;
	vec3 heading;
	float speed; //blocks per tick
	int timer; // for timer based activities //also if timer != -1 then it is timer based, timer counts down to 0
	int hascollision; //so no collision check for every thing, optimization :3
	int collisiontype; //what it does when collides. //0 for dissapear 1 for explode
} Entity;

typedef struct {
	/*Vector3[]*/list4f normals; /*Vector3[]*/list4f verts; /*Vector2[]*/list4f uvs;
	/*int[][][]*/ list4 faces; /*int[]*/list4 color;
} Model;

typedef struct {
	vec3 roteuler; // rotation in degrees (pitch, yaw, roll)
	vec3 position;
	vec3 velocity;
	int onground;
} Camera;

typedef struct {
	uint ID;
	string vertexPath;
	string fragmentPath;
} Shader;
