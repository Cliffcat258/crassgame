#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "base.h"
#include "structs.h"
#include "funcs.h"
#include "globalvars.h"
#include "models.h"
#include "chunks.h"
#include "render.h"

int main() {
	list8 *a = malloc(sizeof(*a));
	*a = list8_new(0);
	a = LoadMap(a);
	list8 *b = LoadModels();
	printf("hello\n");
	list8 *c = malloc(sizeof(*c));
	*c = list8_new(0);
	ChunksToFloatArr2(a, b, c);
	GLFWwindow *window;
	window = RenderInit();
	while(!glfwWindowShouldClose(window)) {
		OnFrameUpdate(window);
		OnFrameRender(window);
	}
	glfwTerminate();
	return 0;
}
