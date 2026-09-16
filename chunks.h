#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include "base.h"
#include "globalvars.h"
#include "structs.h"
#include "funcs.h"

//for chunk related functions such as gen, load unload, save, turn to vert/indices etc.

Chunk16* InitChunk(vec3 v) {
	Chunk16 *c = malloc(sizeof *c);
	c->bs = malloc(sizeof(list4));
	*(c->bs) = list4_new(4096);
	c->empty = 0;
	c->offset = v;
	c->inited = 1;
	c->changed = 1;
	c->changed2 = 1;
	return c;
}

void Chunk16_free(Chunk16 *c) {
	free(c->bs->d);
	free(c->bs);
	free(c);
}

Chunk16* GenChunk16(vec3 offset) {
	return InitChunk(offset);
}

Chunk16* RotateChunk(Chunk16 *chunk, char axis) {
	vec3 v3 = vec3_new(0,0,0);
	Chunk16 *chunk2 = InitChunk(chunk->offset);
	if(axis == 'x') {
		for(int i2 = 0; i2 < 4096; i2++) {
			v3 = IntToVec3Chunk(i2);
			chunk2->bs->d[Vec3ToIntChunk(vec3_new(v3.x, v3.z, v3.y))] = chunk->bs->d[i2];
		}
	}
	if(axis == 'z') {
		for(int i2 = 0; i2 < 4096; i2++) {
			v3 = IntToVec3Chunk(i2);
			chunk2->bs->d[Vec3ToIntChunk(vec3_new(v3.y, v3.x, v3.z))] = chunk->bs->d[i2];
		}
	}
	chunk2->shouldbesaved = chunk->shouldbesaved; chunk2->changed2 = chunk->changed2; chunk2->empty = chunk->empty; chunk2->offset = chunk->offset; chunk2->inited = chunk->inited; chunk2->changed = chunk->changed;
	return chunk2;
}

void FlipBitmap(list4 *bitmap) {
	list4 bitmap2 = list4_new(256);
	for(int i = 0; i < 15; i++) {
		for(int i2 = 0; i2 < 15; i2++) {
			bitmap2.d[i * 16 + (15 - i2)] = bitmap->d[i * 16 + i2];
		}
	}
	for(int i = 0; i < 15; i++) {
		for(int i2 = 0; i2 < 15; i2++) {
			bitmap->d[(15 - i) * 16 + i2] = bitmap2.d[i * 16 + i2];
		}
	}
	return;
}

/*int[]*/ list4* GreedyMeshingMeshGen(list4 *bitmap) {
	//List<int> quads = new List<int>();
	list4 *quads = malloc(sizeof(*quads));
	*quads = list4_new(0);
	int a = 1; int c = 0; int d = 0; int e = 0; //temp lens, good luck me on figuring what the fuck i was doing lol
	//bool b = 0; //temp bool
	int b = 0;
	for(int i = 0; i < 256; i++) { // 16x16 = 256 wow groundbreaking //this is why I write comments, truly.. so clever and funny I am
		a = 1; c = 0; d = 0; b = 0;
		if(bitmap->d[i] != 0) {
			e = bitmap->d[i];
			bitmap->d[i] = 0;
			c = i;
			//quads.Add(i / 16); //flipped rn
			list4_add1(i / 16, quads);

			//quads.Add(i % 16); //start
			list4_add1(i % 16, quads);
			if(i % 16 == 15) {
				//while(b == 0) {
				for(;!b;) {
					d++;
					if((d * 16) + c < 256 && bitmap->d[(d * 16) + c] == e) { bitmap->d[(d * 16) + c] = 0; } else { b = 1; }
				}
				//quads.Add(d); quads.Add(1); quads.Add(e);
				list4_add1(d, quads);
				list4_add1(1, quads);
				list4_add1(e, quads);
				continue;
			}

			int b2 = 1;
			for(;b2;) {
				i++;
				if(i % 16 != 0 && bitmap->d[i] == e) {
					a++;
					bitmap->d[i] = 0;
				} else { i--; break; }
			}
			//while(b == 0) { //now the other thing y (or x)
			for(;!b;) {
				d++;
				for(int i2 = 0; i2 < a; i2++) {
					if((d * 16) + c + i2 < 256 && bitmap->d[(d * 16) + c + i2] == e) { } else { b = 1; break; }
				}
				if(b) {
					break;
				}
				for(int i2 = 0; i2 < a; i2++) {
					bitmap->d[(d * 16) + c + i2] = 0;
				}
			}
			//quads.Add(d); quads.Add(a); //flipped rn quads.Add(e);
			list4_add1(d, quads);
			list4_add1(a, quads);
			list4_add1(e, quads);
		}
	}
	return quads;
}

//Do I really have to rewrite whatever this is lol //I don't even remember what this function does or why it's used //at least lists
void UpdateLists(list4 *quads, list4f *floats, list4 *indices, vec3 offset, vec3 orientation, char rotation, int offset2) { //hell yeah some style here
	//do the thing of quads(int[]) to actual tris //ahh so that's what this does ok
	int len = floats->size; int len2 = indices->size; //so the triangles go like //idk what tf i was thinking

	for(int i = 0; i < quads->size / 5; i++) { //runs once per quad
		for(int i2 = 0; i2 < 4; i2++) {
			if(rotation == 'n') { //basically _ to _ (facing up)
				//if(i2 == 0) {
				if(!i2) {
					list4f_add1(offset.x + quads->d[i * 5], floats);
					list4f_add1(offset.y + offset2, floats);
					list4f_add1(offset.z + quads->d[i * 5 + 1], floats);
				} else if(i2 == 1) { //cannot use if(i2) bc that is 1 for any non 0 int
					list4f_add1(offset.x + quads->d[i * 5] + quads->d[i * 5 + 2], floats);
					list4f_add1(offset.y + offset2, floats);
					list4f_add1(offset.z + quads->d[i * 5 + 1], floats);
				} else if(i2 == 2) {
					list4f_add1(offset.x + quads->d[i * 5], floats);
					list4f_add1(offset.y + offset2, floats);
					list4f_add1(offset.z + quads->d[i * 5 + 1] + quads->d[i * 5 + 3], floats);
				} else if(i2 == 3) {
					list4f_add1(offset.x + quads->d[i * 5] + quads->d[i * 5 + 2], floats);
					list4f_add1(offset.y + offset2, floats);
					list4f_add1(offset.z + quads->d[i * 5 + 1] + quads->d[i * 5 + 3], floats);
				}
			} else if(rotation == 'x') { //basically _ to | (facing left)
				int p = quads->d[i * 5 + 1]; // * sin90 which is 1 //y'
				int p2 = quads->d[i * 5 + 1] + quads->d[i * 5 + 3];   //y' but case 2
				if(!i2) {
					list4f_add1(offset.x + quads->d[i * 5], floats);
					list4f_add1(offset.y + p, floats);
					list4f_add1(offset.z + offset2, floats);
				} else if(i2 == 1) {
					list4f_add1(offset.x + quads->d[i * 5] + quads->d[i * 5 + 2], floats);
					list4f_add1(offset.y + p, floats);
					list4f_add1(offset.z + offset2, floats);
				} else if(i2 == 2) {
					list4f_add1(offset.x + quads->d[i * 5], floats);
					list4f_add1(offset.y + p2, floats);
					list4f_add1(offset.z + offset2, floats);
				} else if(i2 == 3) {
					list4f_add1(offset.x + quads->d[i * 5] + quads->d[i * 5 + 2], floats);
					list4f_add1(offset.y + p2, floats);
					list4f_add1(offset.z + offset2, floats);
				}
			} else if(rotation == 'z') { //basically _ to # (facing "camera")
				if(!i2) {
					list4f_add1(offset.x + offset2, floats);
					list4f_add1(offset.y + quads->d[i * 5], floats);
					list4f_add1(offset.z + quads->d[i * 5 + 1], floats);
				} else if(i2 == 1) {
					list4f_add1(offset.x + offset2, floats);
					list4f_add1(offset.y + quads->d[i * 5] + quads->d[i * 5 + 2], floats);
					list4f_add1(offset.z + quads->d[i * 5 + 1], floats);
				} else if(i2 == 2) {
					list4f_add1(offset.x + offset2, floats);
					list4f_add1(offset.y + quads->d[i * 5], floats);
					list4f_add1(offset.z + quads->d[i * 5 + 1] + quads->d[i * 5 + 3], floats);
				} else if(i2 == 3) {
					list4f_add1(offset.x + offset2, floats);
					list4f_add1(offset.y + quads->d[i * 5] + quads->d[i * 5 + 2], floats);
					list4f_add1(offset.z + quads->d[i * 5 + 1] + quads->d[i * 5 + 3], floats);
				}
			} //hell yeah sin and cos when a = 90 is just 1 and 0 yayay, no matrix mult needed
			list4f_add1(orientation.x, floats);
			list4f_add1(orientation.y, floats);
			list4f_add1(orientation.z, floats);
			list4f_add1(quads->d[i * 5 + 4], floats);
		}
		//1
		list4_add1((len / 7) + i * 4, indices); //bottom right
		list4_add1((len / 7) + i * 4 + 1, indices); //top right
		list4_add1((len / 7) + i * 4 + 2, indices); //bottom left
		//2
		list4_add1((len / 7) + i * 4 + 2, indices); //bottom left
		list4_add1((len / 7) + i * 4 + 1, indices); //top right
		list4_add1((len / 7) + i * 4 + 3, indices); //top left
	}
	return;
}
//ok it wasn't that bad bc Vim is great :D

//Oh this function...
//You know what I'll just take a break :3
//Half this code is just absolutlely useless comments lolol

/*(float[], uint[])*/list8* ChunkToArrs(/*Chunk16[]*/list8 *chunks, /*Model[]*/list8 *models, int index) { //this is a nightmare, why do i do this?
	//Model model = models[4];
	Model *model = (Model *)models->d[4]; //yes switching to C was definitely the right choice lolol, however this makes sense actually //TODO crashes here rn bc no models
	if(((Chunk16 *)chunks->d[index])->inited && !((Chunk16 *)chunks->d[index])->empty) { //but there is no other choice, to obtain speeeeed! //two different stories intersecting lolol //also this line is pretty cursed but totally normal C code like..
		Chunk16 *chunk = (Chunk16 *)chunks->d[index]; //ok but is like this line wrong or wtf //but just maybe, this is not the goal after all
		Chunk16 *chunk2 = RotateChunk(chunk, 'x'); //ERROR
		Chunk16 *chunk3 = RotateChunk(chunk, 'z');
		//List<float> floats = new List<float>(); //maybe I just want to write my story in code comments and need some code to comment
		list4f *floats = malloc(sizeof *floats);
		*floats = list4f_new(0);
		//List<uint> indices = new List<uint>(); //whatever i guess i'll just get back to this nightmare of a code
		list4 *indices = malloc(sizeof *indices);
		*indices = list4_new(0);
		//list8 flindiceso = list8_new(0); //works as tuple of floats and indices, very funne name
		//list8 *flindices = &flindiceso;
		list8 *flindices = malloc(sizeof *flindices);
		*flindices = list8_new(0);
		//Chunk16[] chunks2 = new Chunk16[6]; //bc why not
		Chunk16* chunks2[6];
		//chunks2[3] = GetNeighbouringChunk(new Vector3 { X = 0, Y = -1, Z = 0 }, chunks, index);
		chunks2[3] = GetNeighbouringChunk(vec3_new(0,-1,0), chunks, index); //TODO figure out if I shouldn't be passing 'chunks' everywhere, bc it is just a list8 of the existing chunks.
		//chunks2[1] = RotateChunk(GetNeighbouringChunk(new Vector3 { X = 0, Y = 0, Z = -1 }, chunks, index), "x");
		chunks2[1] = GetNeighbouringChunk(vec3_new(0,0,-1), chunks, index);
		//chunks2[5] = RotateChunk(GetNeighbouringChunk(new Vector3 { X = -1, Y = 0, Z = 0 }, chunks, index), "z"); //honestly what was I doing here... it work(s/ed) I suppose
		chunks2[5] = GetNeighbouringChunk(vec3_new(-1,0,0), chunks, index);
		//so many comments here, not gonna clean it up though lolol
		//this is so bad coding etiquette, however I don't care bc this is not how to code, this is art.

		//int[] quads = new int[0]; //honestly this wasn't that bad lol
		list4 *quads;
		//int[] bitmap = new int[256];
		list4 bitmapv = list4_new(256);
		list4 *bitmap = &bitmapv;
		//Vector3 offset = chunk.offset;
		vec3 offset = chunk->offset;
		//bool a;
		int a;
		//bottom face
		//first layer with edge case
		a = 0;
		for(int i2 = 0; i2 < 256; i2++) {
			//if(chunks2[3].time != 69) { //WHAT THE FUCK IS TIME AND WHY IS IT 69 I HATE YOU SO MUCH WHAT IS HONESTLY WRONG WITH YOU, fix: just comment out the line
			//printf("%d\n", chunks2[3]->inited);
			if(chunks2[3]->bs->d[i2 * 16 + 15] == 0 && chunk->bs->d[i2 * 16] != 0) {
				bitmap->d[i2] = chunk->bs->d[i2 * 16]; a = 1;
			} else { bitmap->d[i2] = 0; }
			//}
		}
		if(a) { quads = GreedyMeshingMeshGen(bitmap); /*(floats, indices)*/UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(0,1,0), 'n', 0); free(quads->d); free(quads); }
		//other layers
		for(int i = 0; i < 15; i++) { //loops through all layers and creates bitmap for the down facing things
			a = 0;
			for(int i2 = 0; i2 < 256; i2++) {
				if(chunk->bs->d[i2 * 16 + i] == 0 && chunk->bs->d[i2 * 16 + i + 1] != 0) {
					bitmap->d[i2] = chunk->bs->d[i2 * 16 + i + 1]; a = 1;
				} else { bitmap->d[i2] = 0; }
			}
			if(a) { quads = GreedyMeshingMeshGen(bitmap); /*(floats, indices)*/UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(0,1,0), 'n', i + 1); free(quads->d); free(quads); }
		}


		//top face
		//first layer with edge case
		a = 0;
		for(int i2 = 0; i2 < 256; i2++) {
			if(chunks2[3]->inited && chunks2[3]->bs->d[i2 * 16 + 15] != 0 && chunk->bs->d[i2 * 16] == 0) {
				bitmap->d[i2] = chunks2[3]->bs->d[i2 * 16 + 15]; a = 1;
			} else { bitmap->d[i2] = 0; }
		}
		if(a) { quads = GreedyMeshingMeshGen(bitmap); UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(0,-1,0), 'n', 0); free(quads->d); free(quads); }
		//other layers
		for(int i = 0; i < 15; i++) { //loops through all layers and creates bitmap for the down facing things
			a = 0;
			for(int i2 = 0; i2 < 256; i2++) {
				if(chunk->bs->d[i2 * 16 + i] != 0 && chunk->bs->d[i2 * 16 + i + 1] == 0) {
					bitmap->d[i2] = chunk->bs->d[i2 * 16 + i]; a = 1;
				} else { bitmap->d[i2] = 0; }
			}
			if(a) { quads = GreedyMeshingMeshGen(bitmap); UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(0,-1,0), 'n', i + 1); free(quads->d); free(quads); }
		}

		//right face // | to _ (counterclockwise rotation) (so down is actually left)
		//first layer with edge case //wrong rn //now right :)
		a = 0;
		for(int i2 = 0; i2 < 256; i2++) {
			if(chunks2[1]->inited && chunks2[1]->bs->d[i2 * 16 + 15] != 0 && chunk2->bs->d[i2 * 16] == 0) {
				bitmap->d[i2] = chunks2[1]->bs->d[i2 * 16 + 15]; a = 1;
			} else { bitmap->d[i2] = 0; }
		}
		if(a) { quads = GreedyMeshingMeshGen(bitmap); UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(0,0,1), 'x', 0); free(quads->d); free(quads); }
		//other layers
		for(int i = 0; i < 15; i++) { //loops through all layers and creates bitmap for the down facing things
			a = 0;
			for(int i2 = 0; i2 < 256; i2++) {
				if(chunk2->bs->d[i2 * 16 + i] != 0 && chunk2->bs->d[i2 * 16 + i + 1] == 0) {
					bitmap->d[i2] = chunk2->bs->d[i2 * 16 + i]; a = 1;
				} else { bitmap->d[i2] = 0; }
			}
			if(a) { quads = GreedyMeshingMeshGen(bitmap); UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(0,0,1), 'x', i + 1); free(quads->d); free(quads); }
		}

		//left face
		//first layer with edge case
		a = 0;
		for(int i2 = 0; i2 < 256; i2++) {
			if(chunks2[1]->inited && chunks2[1]->bs->d[i2 * 16 + 15] == 0 && chunk2->bs->d[i2 * 16] != 0) {
				bitmap->d[i2] = chunk2->bs->d[i2 * 16]; a = 1;
			} else { bitmap->d[i2] = 0; }
		}
		if(a) { quads = GreedyMeshingMeshGen(bitmap); UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(0,0,-1), 'x', 0); free(quads->d); free(quads); }
		//other layers
		for(int i = 0; i < 15; i++) { //loops through all layers and creates bitmap for the down facing things
			a = 0;
			for(int i2 = 0; i2 < 256; i2++) {
				if(chunk2->bs->d[i2 * 16 + i] == 0 && chunk2->bs->d[i2 * 16 + i + 1] != 0) {
					bitmap->d[i2] = chunk2->bs->d[i2 * 16 + i + 1]; a = 1;
				} else { bitmap->d[i2] = 0; }
			}
			if(a) { quads = GreedyMeshingMeshGen(bitmap); UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(0,0,-1), 'x', i + 1); free(quads->d); free(quads); }
		}

		//front face (or back idk)
		//first layer with edge case //wrong rn //right
		a = 0;
		for(int i2 = 0; i2 < 256; i2++) {
			if(chunks2[5]->bs->d[i2 * 16 + 15] != 0 && chunk3->bs->d[i2 * 16] == 0) {
				bitmap->d[i2] = chunks2[5]->bs->d[i2 * 16 + 15]; a = 1;
			} else { bitmap->d[i2] = 0; }
		}
		if(a) { quads = GreedyMeshingMeshGen(bitmap); UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(1,0,0), 'z', 0); free(quads->d); free(quads); }
		//other layers
		for(int i = 0; i < 15; i++) { //loops through all layers and creates bitmap for the down facing things
			a = 0;
			for(int i2 = 0; i2 < 256; i2++) {
				if(chunk3->bs->d[i2 * 16 + i] != 0 && chunk3->bs->d[i2 * 16 + i + 1] == 0) {
					bitmap->d[i2] = chunk3->bs->d[i2 * 16 + i]; a = 1;
				} else { bitmap->d[i2] = 0; }
			}
			if(a) { quads = GreedyMeshingMeshGen(bitmap); UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(1,0,0), 'z', i + 1); free(quads->d); free(quads); }
		}

		//back face (or front idk)
		//first layer with edge case //wrong rn
		a = 0;
		for(int i2 = 0; i2 < 256; i2++) {
			if(chunks2[5]->bs->d[i2 * 16 + 15] == 0 && chunk3->bs->d[i2 * 16] != 0) {
				bitmap->d[i2] = chunk3->bs->d[i2 * 16]; a = 1;
			} else { bitmap->d[i2] = 0; }
		}
		if(a) { quads = GreedyMeshingMeshGen(bitmap); UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(-1,0,0), 'z', 0); free(quads->d); free(quads); }
		//other layers
		for(int i = 0; i < 15; i++) { //loops through all layers and creates bitmap for the down facing things
			a = 0;
			for(int i2 = 0; i2 < 256; i2++) {
				if(chunk3->bs->d[i2 * 16 + i] == 0 && chunk3->bs->d[i2 * 16 + i + 1] != 0) {
					bitmap->d[i2] = chunk3->bs->d[i2 * 16 + i + 1]; a = 1;
				} else { bitmap->d[i2] = 0; }
			}
			if(a) { quads = GreedyMeshingMeshGen(bitmap); UpdateLists(quads, floats, indices, vec3_mult(offset, 16), vec3_new(-1,0,0), 'z', i + 1); free(quads->d); free(quads); }
		}

		//for(int i = 0; i < floats->size; i++) { //idk what that did lol
			//floats[i] = (floats[i] - 0.5f) * 2;
			//((list4f *)flindices->d[0])->d[i] = ( ((list4f *)flindices->d[0])->d[i] - 0.5 ) * 2; //uh no what is this lol
			//floats->d[i] = (floats->d[i] - 0.5) * 2;
		//}
		//list4f_print(floats);
		list8_add1(floats, flindices); list8_add1(indices, flindices);
		Chunk16_free(chunk2); Chunk16_free(chunk3);
		free(bitmap->d);
		return flindices;
	}
	return NULL;
}

//(float[][], uint[][]) == list8 of 2 list8s of list4fs and list4s, banger
void ChunksToFloatArr2(list8 *chunks, list8 *models, list8 *flindices2 /*the greatest data type*/) {
	list8 *temp;
	for(int i = 0; i < chunks->size; i++) {
		if(((Chunk16 *)chunks->d[i])->inited) {
			if(((Chunk16 *)chunks->d[i])->changed) { // if chunk has been changed
				//printf("%d\n", i);
				temp = ChunkToArrs(chunks, models, i);
				if(temp != NULL) {
					if(flindices2->size == i) { 
						list4f *a = malloc(sizeof(list4f));
						*a = list4f_new(0);
						list4 *b = malloc(sizeof(list4));
						*b = list4_new(0);
						list8 *c = malloc(sizeof(list8));
						*c = list8_new(0);
						list8_add1(a, c); list8_add1(b, c);
						list8_add1(c, flindices2);
					}
					list8 *pair = (list8 *)flindices2->d[i];
					list4f *floats = (list4f *)pair->d[0];
					list4 *indices = (list4 *)pair->d[1];
					free(floats->d); free(indices->d);
					list4f *floats2 = (list4f *)temp->d[0];
					list4 *indices2 = (list4 *)temp->d[1];
					floats->d = malloc(floats2->size * sizeof(float));
					list4f_add(floats2->d, 4 * floats2->size, floats);
					free(floats2->d);
					//free(floats->d); free(floats);
					indices->d = malloc(indices2->size * sizeof(int));
					list4_add(indices2->d, 4 * indices2->size, indices);
					free(indices2->d);
					//free(indices->d); free(indices);
					free(temp->d); free(temp);
				}
				((Chunk16 *)chunks->d[i])->changed = 0;
			}
		}
	}
	return;
}

list8* LoadMap(list8 *chunks) {
	//free all the previous chunk memory
	if(chunks != NULL) {
		for(int i = 0; i < chunks->size; i++) {
			Chunk16 *c = (Chunk16 *)chunks->d[i];
			//free(c->bs.d); free(c);
			Chunk16_free(c);
		}
		free(chunks->d);
		*chunks = list8_new(0);
	} else {
		list8 *c = malloc(sizeof *c);
		*c = list8_new(0);
		chunks = c;
	}
	//now load new chunks
	FILE *file;
	file = fopen("stuff/chunks/init.txt", "r");
	if(file == NULL) { 
		printf("no init.txt in stuff/chunks\n");
		exit(EXIT_FAILURE);
	}
	char out_init[64];
	uint8_t bytes[4096];
	int i2 = 0;
	for(;fgets(out_init, 64, file);) {
		out_init[strcspn(out_init, "\n")] = 0;
		//printf("stuff/chunks/c%s.bin\n", out_init);
		FILE *file2;
		char filename[256];
		snprintf(filename, sizeof(filename), "stuff/chunks/c%s.bin", out_init);
		file2 = fopen(filename, "rb");
		if(file2 != NULL) {
			vec3 v = vec3_new(0,0,0);
			sscanf(out_init, "%f %f %f", &v.x, &v.y, &v.z);
			Chunk16 *c = InitChunk(v);
			list8_add1(c, chunks);
			fread(bytes, sizeof(uint8_t), 4096, file2);
			for(int i = 0; i < 4096; i++) {
				((Chunk16 *)chunks->d[i2])->bs->d[i] = (int)bytes[i];
			}
			i2++;
			fclose(file2);
		}
	}
	fclose(file);
	return chunks;
}
