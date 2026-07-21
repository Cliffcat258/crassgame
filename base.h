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
	list a;
	a.size = size; a.capacity = capacity; a.d = malloc(capacity * 4);
	return a;
}

void list4_changesize(list *a, int capacity) {
	char *b = realloc(a->d, capacity * 4);
	a->d = b; a-> capacity = capacity;
	return;
}

void list4_add(int *a, int size, list *l) { //size == size of a in bytes //a == thing to add to list(same type as list)
	int amount = size / 4;
	for(;l->size + amount > l->capacity;) { //loop lol
		list_changesize(l, l->capacity * 2);
	}
	memcpy(l->d + l->size * 4, a, size);
	l->size += amount;
	return;
}

void list4_add1(int a, list *l) {
	if(l->size == l-> capacity) {
		list4_changesize
	}
	l->d[size + 1] = a;
	l->size += amount;
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
