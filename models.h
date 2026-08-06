#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "base.h"
#include "globalvars.h"
#include "structs.h"

Model CopyModel(Model *model) {
	Model model2;

	model2.verts = list4f_new(model->verts.size);
	for(int i = 0; i < model->verts.size; i++) {
		model2.verts.d[i] = model->verts.d[i];
	}
	model2.uvs = list4f_new(model->uvs.size);
	for(int i = 0; i < model->uvs.size; i++) {
		model2.uvs.d[i] = model->uvs.d[i];
	}
	model2.normals = list4f_new(model->normals.size);
	for(int i = 0; i < model->normals.size; i++) {
		model2.normals.d[i] = model->normals.d[i];
	}
	model2.faces = list4_new(model->faces.size);
	for(int i = 0; i < model->normals.size; i++) {
		model2.faces.d[i] = model->faces.d[i];
	}
	return model2;
}

/*(List<float>, List<uint>)*/void AddModelToArrs(list4f *floats, list4 *indices, vec3 offset, Model *model) { //void bc it manipulates the memory so doesn't need to return anything
	//if(model->normals == null) { //idk how to check this rn
	//	return (floats, indices);
	//}
	//floats.Capacity += model.verts.Length * 8; //uhh yeah for now it can do bc ChunkToArr already does this, but later make this functional
	//indices.Capacity += model.faces.Length * 3; //now the list data type does that bc i'm very cool and good at programming B)
	int floatslen = floats->size / 8;
	for(int i = 0; i < model->verts.size; i++) {
		//floats.Add(model->verts[i].x + offset.x);
		list4f_add1(model->verts.d[i * 3 + 0] + offset.x, floats);
		list4f_add1(model->verts.d[i * 3 + 1] + offset.y, floats);
		list4f_add1(model->verts.d[i * 3 + 2] + offset.z, floats);
		list4f_add1(model->uvs.d[i * 2 + 0], floats);
		list4f_add1(model->uvs.d[i * 2 + 1], floats);
		list4f_add1(model->normals.d[i * 3 + 0], floats);
		list4f_add1(model->normals.d[i * 3 + 1], floats);
		list4f_add1(model->normals.d[i * 3 + 2], floats);
	}
	for(int i = 0; i < model->faces.size; i++) {
		//indices.Add(Convert.ToUInt32(model->faces[i][0][0] + floatslen));
		list4_add1(model->faces.d[i * 9 + 0] + floatslen, indices);
		list4_add1(model->faces.d[i * 9 + 3] + floatslen, indices);
		list4_add1(model->faces.d[i * 9 + 6] + floatslen, indices);
	}
	//return(floats, indices);
	return;
}

Model RotateModel(Model *model, char axis, double angle) {
	Model model2 = CopyModel(model);
	//angle = angle * MathHelper.DegToRad;
	angle = angle * pi / 180.0;
	if(axis == 'x') {
		for(int i = 0; i < model->verts.size; i++) {
			vec4 v = vec4_mult_mat4(vec4_new(model->verts.d[i * 3 + 0], model->verts.d[i * 3 + 1], model->verts.d[i * 3 + 2], 1.0), mat4_rotx(angle)); //TODO huh if find out also fix other axis
			model2.verts.d[i * 3 + 0] = v.x;
			model2.verts.d[i * 3 + 1] = v.y;
			model2.verts.d[i * 3 + 2] = v.z;
		}
		//for(int i = 0; i < model->normals.size; i++) { //TODO find out if this was even useful I think it was not but idk
		//	model2.normals[i] = model.normals[i] * Matrix3.CreateRotationX(angle);
		//}
	}
	if(axis == 'y') {
		for(int i = 0; i < model->verts.size; i++) {
			vec4 v = vec4_mult_mat4(vec4_new(model->verts.d[i * 3 + 0], model->verts.d[i * 3 + 1], model->verts.d[i * 3 + 2], 1.0), mat4_roty(angle));
			model2.verts.d[i * 3 + 0] = v.x;
			model2.verts.d[i * 3 + 1] = v.y;
			model2.verts.d[i * 3 + 2] = v.z;
		}
	}
	if(axis == 'z') {
		for(int i = 0; i < model->verts.size; i++) {
			vec4 v = vec4_mult_mat4(vec4_new(model->verts.d[i * 3 + 0], model->verts.d[i * 3 + 1], model->verts.d[i * 3 + 2], 1.0), mat4_roty(angle));
			model2.verts.d[i * 3 + 0] = v.x;
			model2.verts.d[i * 3 + 1] = v.y;
			model2.verts.d[i * 3 + 2] = v.z;
		}
	}
	return model2;
}

Model RotateModel2(Model *model, vec3 heading) { //ok so this should return a rotated model
	double yaw = atan(heading.x / -heading.z) * (180.0 / pi);
	double pitch = asin(heading.y) * (180.0 / pi);

	Model model2 = RotateModel(model, 'z', 90.0);
	model2 = RotateModel(&model2, 'y', -yaw + 90.0);
	return model2;
}

Model ScaleModel(Model *model, float scale) {
	Model model2 = CopyModel(model);
	for(int i = 0; i < model2.verts.size; i++) {
		model2.verts.d[i] *= scale;
	}
	return model2;
}

Model GetModelTexture2(Model *model2, int id) {
	Model model = CopyModel(model2);
	if(id < 128 * 3) {
		for(int i = 0; i < model.uvs.size / 2; i++) { // /2 bc uvs are in a single list not a vec2 list
			model.uvs.d[i * 2 + 0] = model.uvs.d[i * 2 + 0] * (1.0 / 128.0) + (id % 128) / 128.0;
			model.uvs.d[i * 2 + 1] = model.uvs.d[i * 2 + 1] * (1.0 / 128.0) + floor(id / 128.0) / 128.0;
		}
	} else {
		id = (int)floor(id / 3.0);
		for(int i = 0; i < model.uvs.size / 2; i++) { // /2 bc uvs are in a single list not a vec2 list
			model.uvs.d[i * 2 + 0] = model.uvs.d[i * 2 + 0] * (1.0 / 128.0) + (id % 128) / 128.0;
			model.uvs.d[i * 2 + 1] = model.uvs.d[i * 2 + 1] * (1.0 / 128.0) + floor(id / 128.0) / 128.0;
		}
	}
	return model;
}
