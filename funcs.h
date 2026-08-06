#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "base.h"
#include "globalvars.h"
#include "structs.h"

//made for simpler functions so that I could test them independently

int Vec3ToIntChunk(vec3 vec) {
	int j = (int)vec.x * 256 + (int)vec.z * 16 + (int)vec.y;
	return j;
}

int InChunk(vec3 offset) {
	if(offset.x < 0 || offset.x >= 16 || offset.y < 0 || offset.y >= 16 || offset.z < 0 || offset.z >= 16) {
		return 0;
	}
	return 1;
}

Chunk16* GetNeighbouringChunk(vec3 offset, /*Chunk16[]*/list8 *chunks, int index) {
	for(int i = 0; i < chunks->size; i++) {
		if(vec3_same(((Chunk16*)chunks->d[i])->offset, vec3_add(((Chunk16*)chunks->d[i])->offset, offset))) {
			return (Chunk16*)chunks->d[i];
		}
	}
	//c.time = 69; //why exactly do I need a time variable and it to be set to 69? idk but it will stay this way lol //NO THIS IS NOT STAYING LIKE THIS IT BREAKS THINGS AND IS FUCKING CONFUSING anyways
	return NULL;
}

vec3 IntToVec3Chunk(int a) { //XZY
	vec3 vec = vec3_new((a / 256) % 16, a % 16, (a / 16) % 16);
	return vec;
}

vec3 Vec3Normalize(vec3 v, int a) {
	vec3 vec = vec3_new(((v.x % a) + a) % a, ((v.y % a) + a) % a, ((v.z % a) + a) % a);
	return vec;
}

vec3 Offset2(vec3 v, float a) {
	return vec3_new(v.x + a, v.y + a, v.z + a);
}

int InChunk2(vec3 offset) {
	if(offset.x < 0 || offset.x >= 16 || offset.y < 0 || offset.y >= 16 || offset.z < 0 || offset.z >= 16) {
		return 0;
	}
	return 1;
}

int GetSign(float a) {
	if(a < 0) {
		return -1;
	}
	if(a > 0) {
		return 1;
	}
	return 0;
}

int GetChunkByOffset(vec3 chunkpos) {
	int a;
	//if(offsetdict.TryGetValue(chunkpos, out a)) {
	for(int i = 0; i < chunksglobal->size; i++) {
		if(vec3_same(((Chunk16 *)chunksglobal->d[i])->offset, chunkpos)) {
			return a;
		}
	}
	return 99999; // big number so it would crash //bad coding practice, but whatever
}

