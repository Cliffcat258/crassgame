#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

typedef struct {
	int size;
	int *d;
} arr;

arr arr_new(int size) {
	arr arr1;
	arr1.size = size; arr1.d = malloc(size * 4);
	return arr1;
}

typedef struct {
	int stride;
	int capacity;
	int size;
	char *d;
} list;

list list_new(int size, int stride) {
	int capacity = size * 2;
	if(capacity == 0) { capacity = 1; }
	list a;
	a.size = size; a.capacity = capacity; a.stride = stride; a.d = malloc(capacity * stride);
	return a;
}

void list_changesize(list *a, int capacity) {
	char *b = realloc(a->d, capacity * a->stride);
	a->d = b; a-> capacity = capacity;
	return;
}

void list_add(char *a, int size, list *l) { //size == size of a in bytes //a == thing to add to list(same type as list)
	int amount = size / l->stride;
	for(;l->size + amount > l->capacity;) { //loop lol
		list_changesize(l, l->capacity * 2);
	}
	memcpy((char *)l->d + l->size * l->stride, a, size);
	l->size += amount;
	return;
}

typedef struct {
	int capacity;
	int size;
	int *d;
} list4;

list4 list4_new(int size) {
	int capacity = size * 2;
	if(capacity == 0) { capacity = 1; }
	list4 a;
	a.size = size; a.capacity = capacity; a.d = malloc(capacity * 4);
	return a;
}

void list4_changesize(list4 *a, int capacity) {
	int *b = realloc(a->d, capacity * 4);
	a->d = b; a-> capacity = capacity;
	return;
}

void list4_add(int *a, int size, list4 *l) { //size == size of a in bytes //a == thing to add to list(same type as list)
	int amount = size / 4;
	for(;l->size + amount > l->capacity;) { //loop lol
		list4_changesize(l, l->capacity * 2);
	}
	memcpy(l->d + l->size, a, size);
	l->size += amount;
	return;
}

void list4_add1(int a, list4 *l) {
	if(l->size == l-> capacity) {
		list4_changesize(l, l->capacity * 2);
	}
	memcpy(l->d + l->size, &a, 4);
	l->size += 1;
	return;
}

void list4_print(list4 *h) {
	for(int i = 0; i < h->size; i++) {
		printf("%d \n", h->d[i]);
	}
}

typedef struct {
	int capacity;
	int size;
	float *d;
} list4f;

list4f list4f_new(int size) {
	int capacity = size * 2;
	if(capacity == 0) { capacity = 1; }
	list4f a;
	a.size = size; a.capacity = capacity; a.d = malloc(capacity * 4);
	return a;
}

void list4f_changesize(list4f *a, int capacity) {
	float *b = realloc(a->d, capacity * 4);
	a->d = b; a-> capacity = capacity;
	return;
}

void list4f_add(float *a, int size, list4f *l) { //size == size of a in bytes //a == thing to add to list(same type as list)
	int amount = size / 4;
	for(;l->size + amount > l->capacity;) { //loop lol
		list4f_changesize(l, l->capacity * 2);
	}
	memcpy(l->d + l->size, a, size);
	l->size += amount;
	return;
}

void list4f_add1(float a, list4f *l) {
	for(;l->size == l-> capacity;) {
		list4f_changesize(l, l->capacity * 2);
	}
	memcpy(l->d + l->size, &a, 4);
	l->size += 1;
	return;
}

void list4f_print(list4f *h) {
	for(int i = 0; i < h->size; i++) {
		printf("%f \n", h->d[i]);
	}
}

typedef struct {
	int capacity;
	int size;
	void **d;
} list8;

list8 list8_new(int size) {
	int capacity = size * 2;
	if(capacity == 0) { capacity = 1; }
	list8 a;
	a.size = size; a.capacity = capacity; a.d = malloc(capacity * 8);
	return a;
}

void list8_init(list8 *a) {
	a->capacity = 0;
	a->size = 0;
	a->d = NULL;
}

void list8_changesize(list8 *a, int capacity) {
	void **b = realloc(a->d, capacity * 8);
	a->d = b; a-> capacity = capacity;
	return;
}

void list8_add(void *a, int size, list8 *l) { //size == size of a in bytes //a == thing to add to list(same type as list)
	int amount = size / 8;
	for(;l->size + amount > l->capacity;) { //loop lol
		list8_changesize(l, l->capacity * 2);
	}
	memcpy(l->d + l->size, a, size);
	l->size += amount;
	return;
}

void list8_add1(void *a, list8 *l) {
	for(;l->size == l-> capacity;) {
		list8_changesize(l, l->capacity * 2);
	}
	memcpy(l->d + l->size, &a, 8);
	l->size += 1;
	return;
}

typedef struct {
	int x; int y; int z;
} vec3;

vec3 vec3_new(float x, float y, float z) {
	vec3 a;
	a.x = x; a.y = y; a.z = z;
	return a;
}

vec3 vec3_add(vec3 a, vec3 b) {
	vec3 c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
	return c;
}

vec3 vec3_mult(vec3 a, int b) {
	vec3 c;
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
	return c;
}

int vec3_same(vec3 a, vec3 b) {
	if(a.x == b.x && a.y == b.y && a.z == b.z) {
		return 1;
	}
	return 0;
}
vec3 vec3_normalize(vec3 a) {
	return a; //TODO
}

typedef struct {
	int x; int y; int z; int w;
} vec4;

vec4 vec4_new(float x, float y, float z, float w) {
	vec4 a;
	a.x = x; a.y = y; a.z = z; a.w = w;
	return a;
}

typedef struct {
	float d[16];
} mat4;

mat4 mat4_rotx(int a) { //TODO
	mat4 m = {0};
	return m;
}

mat4 mat4_roty(int a) { //TODO
	mat4 m = {0};
	return m;
}

mat4 mat4_rotz(int a) { //TODO
	mat4 m = {0};
	return m;
}

mat4 mat4_identity() {
	mat4 m = {0};
	m.d[0] = 1; m.d[5] = 1; m.d[10] = 1; m.d[15] = 1;
	return m;
}

mat4 mat4_transform(vec3 v) {
	mat4 m = mat4_identity();
	m.d[12] = v.x;
	m.d[13] = v.y;
	m.d[14] = v.z;
}

mat4 mat4_scale(int a) {
	mat4 m = {0};
	m.d[0] = a; m.d[5] = a; m.d[10] = a; m.d[15] = a;
	return m;
}

mat4 CreateFromAxisAngle(vec3 *axis, float angle) {
	mat4 result = {0};
	float axisX = axis->x; float axisY = axis->y; float axisZ = axis->z; 
	float cosv = cos(-angle);
	float sinv = sin(-angle);
	float t = 1.0 - cosv;

	float tXX = t * axisX * axisX;
	float tXY = t * axisX * axisY;
	float tXZ = t * axisX * axisZ;
	float tYY = t * axisY * axisY;
	float tYZ = t * axisY * axisZ;
	float tZZ = t * axisZ * axisZ;
	float sinX = sinv * axisX;
	float sinY = sinv * axisY;
	float sinZ = sinv * axisZ;
	
	result.d[0] = tXX + cosv;
	result.d[1] = tXY - sinZ;
	result.d[2] = tXZ + sinY;

	result.d[4] = tXY + sinZ;
	result.d[5] = tYY + cosv;
	result.d[6] = tYZ - sinX;

	result.d[8] = tXZ - sinY;
	result.d[9] = tYZ + sinX;
	result.d[10] = tZZ + cosv;

	result.d[15] = 1;
	return result;
}

mat4 CreatePerspective(float fovy, float aspect, float near, float far) {
	mat4 result = {0};
	float top = near * tan(0.5 * fovy);
	float bottom = -top;
	float left = bottom * aspect;
	float right = top * aspect;
	
	float x = 2.0 * near / (right - left);
	float y = 2.0 * near / (top - bottom);
	float a = (right + left) / (right - left);
	float b = (top + bottom) / (top - bottom);
	float c = -(near + far) / (far - near);
	float d = -(2.0 * near * far) / (far - near);
	
	result.d[0] = x;
	result.d[5] = y;
	result.d[8] = a;
	result.d[9] = b;
	result.d[10] = c;
	result.d[11] = -1;
	result.d[14] = d;
	return result;
}

vec4 vec4_mult_mat4(vec4 v, mat4 m) { //TODO
	return v;
}

typedef struct {
	int capacity;
	int size;
	char *d;
} listc;

listc listc_new(int size) {
	int capacity = size * 2;
	if(capacity == 0) { capacity = 1; }
	listc a;
	a.size = size; a.capacity = capacity; a.d = malloc(capacity);
	return a;
}

void listc_changesize(listc *a, int capacity) {
	char *b = realloc(a->d, capacity);
	a->d = b; a->capacity = capacity;
	return;
}

void listc_add(char *a, int size, listc *l) { //size == size of a in bytes //a == thing to add to list(same type as list)
	int amount = size / 1;
	for(;l->size + amount > l->capacity;) { //loop lol
		listc_changesize(l, l->capacity * 2);
	}
	memcpy(l->d + l->size, a, size);
	l->size += amount;
	return;
}

void listc_add1(char a, listc *l) {
	for(;l->size == l-> capacity;) {
		listc_changesize(l, l->capacity * 2);
	}
	memcpy(l->d + l->size, &a, 1);
	l->size += 1;
	return;
}

void listc_print(listc *h) {
	for(int i = 0; i < h->size; i++) {
		printf("%c", h->d[i]);
	}
	printf("\n");
}

void drawlisttoconsole(list *l, int width, int height) {
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			if(l->d[y * width + x] != 0) {
				printf("%c", l->d[y * width + x]);
			} else { printf(" "); }
		}
		printf("\n");
	}
	return;
}

void drawarrtoconsole(arr *l, int width, int height) {
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			if(l->d[y * width + x] != 0) {
				printf("%c", l->d[y * width + x]);
			} else { printf(" "); }
		}
		printf("\n");
	}
	return;
}

int getch() {
    struct termios old, raw;
    tcgetattr(STDIN_FILENO, &old);
    raw = old;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    int c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    return c;
}
int getch2() { //retains ascii characters and adds basic escapecode characters
	int a;
	a = getch();
	if(a == 27) {
		getch();
		return getch() + 128; //arrow keys are 65 66 67 68 so just add 128 to get out of ascii range else is good lol
	}
	return a;
}

void clearscreen() {
	printf("\e[1;1H\e[2J");
	return;
}

listc* readfile(char name[]) {
	FILE *file = fopen(name, "r");
	char out[100000];
	out[0] = '\0';
	for(;fgets(out + strlen(out), sizeof(out) - strlen(out), file);) {
		
	}
	listc *out2 = malloc(sizeof *out2);
	*out2 = listc_new(0);
	listc_add(&out[0], strlen(out) + 1, out2);
	return out2;
}
