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

int main() {
	list8 *a = NULL;
	a = LoadMap(a);
	list8 *b = LoadModels();
	printf("hello\n");
	list8 *c = malloc(sizeof(*c));
	ChunksToFloatArr2(a, b, c);
	return 0;
}
