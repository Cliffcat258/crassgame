#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "base.h"
#include "funcs.h"
#include "globalvars.h"
#include "models.h"
#include "chunks.h"

	static float[] Vec3ToFloatArrAndUVData(Vector3[] vector3s, Vector2[] UVs) {
		float[] floats = new float[vector3s.Length * 5];
	
		for(int i = 0; i < vector3s.Length; i++) {
			floats[(i * 5) + 0] = vector3s[i].X;
			floats[(i * 5) + 1] = vector3s[i].Y;
			floats[(i * 5) + 2] = vector3s[i].Z;
			floats[(i * 5) + 3] = UVs[i].X;
			floats[(i * 5) + 4] = UVs[i].Y;
		}
	
		return floats;
	}
	
	static uint[] FaceToUIntArr(int[][][] ints) {
		uint[] uints = new uint[ints.Length * 3];
	
		for(int i = 0; i < ints.Length; i++) {
			uints[i * 3 + 0] = Convert.ToUInt32(ints[i][0][0]);
			uints[i * 3 + 1] = Convert.ToUInt32(ints[i][1][0]);
			uints[i * 3 + 2] = Convert.ToUInt32(ints[i][2][0]);
		}
		return uints;
	}
	
	bool OnUpdatePhysics() {
		bool change = 0;
		change = LoadUnloadChunks2();
		return change;
	}
	void UpdatePlayer() {
		float c = 0.09f; float c2 = 0.96f;
		camerac.velocity.Y -= 0.4f;
		camerac.position += new Vector3(camerac.velocity.X * c, camerac.velocity.Y * 1.66f * c, camerac.velocity.Z * c);
		camerac.velocity *= c2;
		camerac.onground -= 1;
		if(camerac.onground == 5) {
			camerac.velocity.Y = 0f;
		}
	}
	void SaveChunk(int i) {
		string[] strings = new string[1000]; string s; int o = 0;
		try {
			StreamReader sr3 = new StreamReader(Directory.GetCurrentDirectory() + "/stuff/chunks/init.txt");
			for(int i2 = 0; i2 < 1000; i2++) {
				s = sr3.ReadLine();
				if(s == null) {
					break;
				}
				strings[i2] = s;
				o++;
			}
			sr3.Close();
		}
		catch(Exception) {
		}
		StreamWriter sr = new StreamWriter(Directory.GetCurrentDirectory() + "/stuff/chunks/init.txt");
		for(int i2 = 0; i2 < o; i2++) {
			sr.WriteLine(strings[i2]);
		}
	
		byte[] bytes = new byte[4096];
		if(chunksglobal[i].shouldbesaved == 1) {
			Vector3 v = chunksglobal[i].offset; bool b = 0;
			for(int i2 = 0; i2 < strings.Length; i2++) {
				if(strings[i2] == (string)(v.X + " " + v.Y + " " + v.Z)) {
					b = 1;
				}
			}
			if(b == 0) {
				sr.WriteLine(v.X + " " + v.Y + " " + v.Z);
			}
			var sr2 = File.OpenWrite(Directory.GetCurrentDirectory() + "/stuff/chunks/c" + v.X + " " + v.Y + " " + v.Z + ".bin");
			for(int i2 = 0; i2 < 4096; i2++) {
				bytes[i2] = (byte)chunksglobal[i].bs[i2];
			}
			sr2.Write(bytes);
			sr2.Close();
		}
	}
	void SaveChunks() {
		string[] strings = new string[1000]; string s; int o = 0;
		try {
			StreamReader sr3 = new StreamReader(Directory.GetCurrentDirectory() + "/stuff/chunks/init.txt");
			for(int i = 0; i < 1000; i++) {
				s = sr3.ReadLine();
				if(s == null) {
					break;
				}
				strings[i] = s;
				o++;
			}
			sr3.Close();
		}
		catch(Exception) {
		}
		StreamWriter sr = new StreamWriter(Directory.GetCurrentDirectory() + "/stuff/chunks/init.txt");
		for(int i = 0; i < o; i++) {
			sr.WriteLine(strings[i]);
		}
		for(int i = 0; i < chunksglobal.Length; i++) {
			byte[] bytes = new byte[4096];
			if(chunksglobal[i].shouldbesaved == 1) {
				Vector3 v = chunksglobal[i].offset; bool b = 0;
				for(int i2 = 0; i2 < strings.Length; i2++) {
					if(strings[i2] == (string)(v.X + " " + v.Y + " " + v.Z)) {
						b = 1;
					}
				}
				if(b == 0) {
					sr.WriteLine(v.X + " " + v.Y + " " + v.Z);
				}
				var sr2 = File.OpenWrite(Directory.GetCurrentDirectory() + "/stuff/chunks/c" + v.X + " " + v.Y + " " + v.Z + ".bin");
				for(int i2 = 0; i2 < 4096; i2++) {
					bytes[i2] = (byte)chunksglobal[i].bs[i2];
				}
				sr2.Write(bytes);
				sr2.Close();
			}
		}
		sr.Close();
		return;
	}
	void PushEntityOutOfBlock2() {
		Vector3 camerapos = Offset2(camerac.position, 2) / 4; // pos in world space   inf - inf
		if(camerapos.Y < 5.5f) {
			return;
		}
		//lots of bools
		bool x = 1; bool z = 1;
		for(int i = 0; i < 2; i++) {
			if(i == 0) {
				camerapos += new Vector3(0, -1.5f * 3, 0);
				//Console.WriteLine(MathF.Round(camerapos.X) + " " + MathF.Round(camerapos.Y) + " " + MathF.Round(camerapos.Z));
				Vector3 lastpos = Offset2(oldpos, 2) / 4; // pos in world space   inf - inf
				lastpos += new Vector3(0, -1.5f * 3, 0);
				Vector3 movement = camerapos - lastpos; // diff in world space   inf - inf but small
				Vector3 chunkpos = new Vector3(MathF.Floor(lastpos.X / 16), MathF.Floor(lastpos.Y / 16), MathF.Floor(lastpos.Z / 16)); // pos in chunks space?
				int j = GetChunkByOffset(chunkpos);
				if(chunkpos.Y == 7 || j == 99999) {
					return;
				}
				Vector3 inchunkpos = Vec3Normalize(lastpos, 16); // pos in chunk space   0 - 16
				Vector3 t = Vec3Normalize(camerapos, 16); // pos in chunk space   0 - 16
				Vector3 inblockpos = Vec3Normalize(camerapos, 1); // pos in block space   0 - 1
				Vector3 s; Chunk16 chunk = chunksglobal[j]; Chunk16 chunk2;
				if(InChunk2(inchunkpos + movement + new Vector3(0.2f * GetSign(movement.X), 0.2f * GetSign(movement.Y), 0.2f * GetSign(movement.Z)))) {
					s = inchunkpos + new Vector3(movement.X + (0.2f * GetSign(movement.X)), 0, 0);
					if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0 && x) {
						camerac.position.X -= movement.X * 4; x = 0;
					}
					s = inchunkpos + new Vector3(0, movement.Y + (0.2f * GetSign(movement.Y)), 0);
					if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0) {
						camerac.position.Y -= movement.Y * 4;
						camerac.onground = 6;
					}
					s = inchunkpos + new Vector3(0, 0, movement.Z + (0.2f * GetSign(movement.Z)));
					if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0 && z) {
						camerac.position.Z -= movement.Z * 4; z = 0;
					}
				} else {
					s = inchunkpos + new Vector3(movement.X + (0.2f * GetSign(movement.X)), 0, 0);
					if(InChunk2(s)) {
						if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0 && x) {
							camerac.position.X -= movement.X * 4; x = 0;
						}
					} else if(InChunk2(s) == 0) { // if axis movement results in it going outside chunk
						if(movement.X > 0) { // if movement is pos (so the coord is > 15 and must be 0)
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(1, 0, 0))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3(0, (int)t.Y, (int)t.Z))] != 0 && x) {
								camerac.position.X -= movement.X * 4; x = 0;
							}
						} else if(movement.X < 0) {
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(-1, 0, 0))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3(15, (int)t.Y, (int)t.Z))] != 0 && x) {
								camerac.position.X -= movement.X * 4; x = 0;
							}
						}
					}
					s = inchunkpos + new Vector3(0, movement.Y + (0.2f * GetSign(movement.Y)), 0);
					if(InChunk2(s)) {
						if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0) {
							camerac.position.Y -= movement.Y * 4;
							camerac.onground = 6;
						}
					} else if(InChunk2(s) == 0) { // if axis movement results in it going outside chunk
						if(movement.Y > 0) { // if movement is pos (so the coord is > 15 and must be 0)
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(0, 1, 0))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3((int)t.X, 0, (int)t.Z))] != 0) {
								camerac.position.Y -= movement.Y * 4;
							}
						} else if(movement.Y < 0) {
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(0, -1, 0))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3((int)t.X, 15, (int)t.Z))] != 0) {
								camerac.position.Y -= movement.Y * 4;
								camerac.onground = 6;
							}
						}
					}
					s = inchunkpos + new Vector3(0, 0, movement.Z + (0.2f * GetSign(movement.Z)));
					if(InChunk2(s)) {
						if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0 && z) {
							camerac.position.Z -= movement.Z * 4; z = 0;
						}
					} else if(InChunk2(s) == 0) { // if axis movement results in it going outside chunk
						if(movement.Z > 0) { // if movement is pos (so the coord is > 15 and must be 0)
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(0, 0, 1))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3((int)t.X, (int)t.Y, 0))] != 0 && z) {
								camerac.position.Z -= movement.Z * 4; z = 0;
							}
						} else if(movement.Z < 0) {
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(0, 0, -1))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3((int)t.X, (int)t.Y, 15))] != 0 && z) {
								camerac.position.Z -= movement.Z * 4; z = 0;
							}
						}
					}
				}
			} else {
				camerapos -= new Vector3(0, -1.5f * 3, 0);
				//Console.WriteLine(MathF.Round(camerapos.X) + " " + MathF.Round(camerapos.Y) + " " + MathF.Round(camerapos.Z));
				Vector3 lastpos = Offset2(oldpos, 2) / 4; // pos in world space   inf - inf
				lastpos += new Vector3(0, 0, 0);
				Vector3 movement = camerapos - lastpos; // diff in world space   inf - inf but small
				Vector3 chunkpos = new Vector3(MathF.Floor(lastpos.X / 16), MathF.Floor(lastpos.Y / 16), MathF.Floor(lastpos.Z / 16)); // pos in chunks space?
				int j = GetChunkByOffset(chunkpos);
				if(chunkpos.Y == 7 || j == 99999) {
					return;
				}
				Vector3 inchunkpos = Vec3Normalize(lastpos, 16); // pos in chunk space   0 - 16
				Vector3 t = Vec3Normalize(camerapos, 16); // pos in chunk space   0 - 16
				Vector3 inblockpos = Vec3Normalize(camerapos, 1); // pos in block space   0 - 1
				Vector3 s; Chunk16 chunk = chunksglobal[j]; Chunk16 chunk2;
				if(InChunk2(inchunkpos + movement + new Vector3(0.2f * GetSign(movement.X), 0.2f * GetSign(movement.Y), 0.2f * GetSign(movement.Z)))) {
					s = inchunkpos + new Vector3(movement.X + (0.2f * GetSign(movement.X)), 0, 0);
					if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0 && x) {
						camerac.position.X -= movement.X * 4; x = 0;
					}
					s = inchunkpos + new Vector3(0, movement.Y + (0.2f * GetSign(movement.Y)), 0);
					if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0) {
						camerac.position.Y -= movement.Y * 4;
						camerac.onground = 6;
					}
					s = inchunkpos + new Vector3(0, 0, movement.Z + (0.2f * GetSign(movement.Z)));
					if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0 && z) {
						camerac.position.Z -= movement.Z * 4; z = 0;
					}
				} else {
					s = inchunkpos + new Vector3(movement.X + (0.2f * GetSign(movement.X)), 0, 0);
					if(InChunk2(s)) {
						if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0 && x) {
							camerac.position.X -= movement.X * 4; x = 0;
						}
					} else if(InChunk2(s) == 0) { // if axis movement results in it going outside chunk
						if(movement.X > 0) { // if movement is pos (so the coord is > 15 and must be 0)
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(1, 0, 0))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3(0, (int)t.Y, (int)t.Z))] != 0 && x) {
								camerac.position.X -= movement.X * 4; x = 0;
							}
						} else if(movement.X < 0) {
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(-1, 0, 0))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3(15, (int)t.Y, (int)t.Z))] != 0 && x) {
								camerac.position.X -= movement.X * 4; x = 0;
							}
						}
					}
					s = inchunkpos + new Vector3(0, movement.Y + (0.2f * GetSign(movement.Y)), 0);
					if(InChunk2(s)) {
						if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0) {
							camerac.position.Y -= movement.Y * 4;
							camerac.onground = 6;
						}
					} else if(InChunk2(s) == 0) { // if axis movement results in it going outside chunk
						if(movement.Y > 0) { // if movement is pos (so the coord is > 15 and must be 0)
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(0, 1, 0))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3((int)t.X, 0, (int)t.Z))] != 0) {
								camerac.position.Y -= movement.Y * 4;
							}
						} else if(movement.Y < 0) {
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(0, -1, 0))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3((int)t.X, 15, (int)t.Z))] != 0) {
								camerac.position.Y -= movement.Y * 4;
								camerac.onground = 6;
							}
						}
					}
					s = inchunkpos + new Vector3(0, 0, movement.Z + (0.2f * GetSign(movement.Z)));
					if(InChunk2(s)) {
						if(chunk.bs[Vec3ToIntChunk(new Vector3(s.X, s.Y, s.Z))] != 0 && z) {
							camerac.position.Z -= movement.Z * 4; z = 0;
						}
					} else if(InChunk2(s) == 0) { // if axis movement results in it going outside chunk
						if(movement.Z > 0) { // if movement is pos (so the coord is > 15 and must be 0)
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(0, 0, 1))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3((int)t.X, (int)t.Y, 0))] != 0 && z) {
								camerac.position.Z -= movement.Z * 4; z = 0;
							}
						} else if(movement.Z < 0) {
							chunk2 = chunksglobal[GetChunkByOffset(chunkpos + new Vector3(0, 0, -1))]; // gets the right chunk
							if(chunk2.bs[Vec3ToIntChunk(new Vector3((int)t.X, (int)t.Y, 15))] != 0 && z) {
								camerac.position.Z -= movement.Z * 4; z = 0;
							}
						}
					}
				}
			}
		}
		return;
	}
	(Vector3, int, Vector3, int) GetLookAtBlock(Vector3 forward) {
		Vector3 pos = Offset2(camerac.position, 2) / 4; // pos in world space   inf - inf
		Vector3 u = new Vector3(0, 0, 0); Vector3 resultpos; Vector3 chunkspos; Vector3 inchunkpos; int j;
		float viewdist = 6; Vector3 u2 = new Vector3(); int j2 = 0;
		for(float i = 0; i < viewdist; i += 0.02f) {
			resultpos = pos + (forward * i);
			chunkspos = new Vector3(MathF.Floor(resultpos.X / 16), MathF.Floor(resultpos.Y / 16), MathF.Floor(resultpos.Z / 16));
			inchunkpos = Vec3Normalize(resultpos, 16);
			j = GetChunkByOffset(chunkspos);
			if(j == 99999) {
				continue;
			}
			if(chunksglobal[j].bs[Vec3ToIntChunk(inchunkpos)] != 0) {
				Console.SetCursorPosition(0, 2);
				Console.Write(i + "  ");
				Console.Write(chunksglobal[j].bs[Vec3ToIntChunk(new Vector3(((int)inchunkpos.X), (int)inchunkpos.Z, (int)inchunkpos.Y))]);
				return (resultpos, j, u2, j2);
			}
			u2 = resultpos; j2 = j;
		}
		return (u, 0, u, 0);
	}
	void UpdateNearbyChunks(int j2) {
		chunksglobal[j2].changed = 1;
		(vertsglobal[j2], indicesglobal[j2]) = ChunkToArrs(chunksglobal, modelsglobal, j2);
		chunksglobal[j2].changed = 0;
		int j3 = GetChunkByOffset(chunksglobal[j2].offset + new Vector3(1, 0, 0));
		if(j3 != 99999) {
			chunksglobal[j3].changed = 1;
			(vertsglobal[j3], indicesglobal[j3]) = ChunkToArrs(chunksglobal, modelsglobal, j3);
			chunksglobal[j3].changed = 0;
		}
		j3 = GetChunkByOffset(chunksglobal[j2].offset + new Vector3(-1, 0, 0));
		if(j3 != 99999) {
			chunksglobal[j3].changed = 1;
			(vertsglobal[j3], indicesglobal[j3]) = ChunkToArrs(chunksglobal, modelsglobal, j3);
			chunksglobal[j3].changed = 0;
		}
		j3 = GetChunkByOffset(chunksglobal[j2].offset + new Vector3(0, 1, 0));
		if(j3 != 99999) {
			chunksglobal[j3].changed = 1;
			(vertsglobal[j3], indicesglobal[j3]) = ChunkToArrs(chunksglobal, modelsglobal, j3);
			chunksglobal[j3].changed = 0;
		}
		j3 = GetChunkByOffset(chunksglobal[j2].offset + new Vector3(0, -1, 0));
		if(j3 != 99999) {
			chunksglobal[j3].changed = 1;
			(vertsglobal[j3], indicesglobal[j3]) = ChunkToArrs(chunksglobal, modelsglobal, j3);
			chunksglobal[j3].changed = 0;
		}
		j3 = GetChunkByOffset(chunksglobal[j2].offset + new Vector3(0, 0, 1));
		if(j3 != 99999) {
			chunksglobal[j3].changed = 1;
			(vertsglobal[j3], indicesglobal[j3]) = ChunkToArrs(chunksglobal, modelsglobal, j3);
			chunksglobal[j3].changed = 0;
		}
		j3 = GetChunkByOffset(chunksglobal[j2].offset + new Vector3(0, 0, -1));
		if(j3 != 99999) {
			chunksglobal[j3].changed = 1;
			(vertsglobal[j3], indicesglobal[j3]) = ChunkToArrs(chunksglobal, modelsglobal, j3);
			chunksglobal[j3].changed = 0;
		}
		return;
	}
	Vector2[] GetTextureFromID(int id, Vector2[] uvs2) {
		Vector2[] uvs = new Vector2[uvs2.Length];
		for(int i = 0; i < uvs2.Length; i++) {
			uvs[i] = uvs2[i];
		}
		if(id < 128 * 3) {
			for(int i = 0; i < uvs.Length; i++) {
				uvs[i].X = uvs[i].X * (1f / 128f) + (id % 128f) / 128f;
				uvs[i].Y = uvs[i].Y * (1f / 128f) + MathF.Floor(id / 128f) / 128f;
			}
		} else {
			//id = (int)MathF.Floor(id / 3f);
			for(int i = 0; i < uvs.Length; i++) {
				uvs[i].X = uvs[i].X * (1f / 128f) + (id % 128f) / 128f;
				uvs[i].Y = uvs[i].Y * (1f / 128f) + MathF.Floor(id / 128f) / 128f;
			}
		}
		return uvs;
	}
	Vector3 LerpSomething(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, float y, float x) { //lerp = A + t(B - A)
		Vector3 pos1; Vector3 pos2; // x axis
		pos1 = v4 + y * (v1 - v4);
		pos2 = v3 + y * (v2 - v3);
		Vector3 pos = pos1 + x * (pos2 - pos1);
		return pos;
	}
	bool ChunkHasData(Vector3 offset) {
		if(offset == new Vector3(0, 0, 0)) {
			return 0;
		}
		for(int i = 0; i < savedchunks.Length; i++) {
			if (offset == savedchunks[i]) {
				return 1;
			}
		}
		return 0;
	}
	Chunk16 LoadChunkFromFile(Vector3 v, int b) {
		Chunk16 chunk = InitChunk(v, b);
		chunk.empty = 0;
		byte[] bytes = new byte[4096];
		var sr2 = File.OpenRead(Directory.GetCurrentDirectory() + "/stuff/chunks/c" + v.X + " " + v.Y + " " + v.Z + ".bin");
		sr2.Read(bytes, 0, 4096);
		sr2.Close();
		for(int i2 = 0; i2 < 4096; i2++) {
			chunk.bs[i2] = bytes[i2];
		}
		return chunk;
	}
	
	bool Vec3Same(Vector3 v1, Vector3 v2) {
		if(MathF.Round(v1.X, 2) == MathF.Round(v2.X, 2) && MathF.Round(v1.Y, 2) == MathF.Round(v2.Y, 2) && MathF.Round(v1.Z, 2) == MathF.Round(v2.Z, 2)) {
			return 1;
		}
		return 0;
	}
	Vector3 FindNextPos(Vector3 pos) {
		bool b = 0;
		
		for(int i = 0; i < 200; i++) { //try 200 times, adds personality lol, or you could say that they're dumb
			int h = Vec3ToIntChunk(new Vector3(Random2(4) - 2, Random2(4) - 2, Random2(4) - 2));
			int h2 = (int)Random2(4096);
			Vector3 h3 = IntToVec3Chunk(h2);
			if(chunksglobal[h].bs[h2] == 0) {
				for(int i2 = 0; i2 < h3.Y; i2++) {
					if(chunksglobal[h].bs[Vec3ToIntChunk(h3 - new Vector3(0,i2,0))] != 0) {
						return chunksglobal[h].offset * 16 + (h3 - new Vector3(0, i2, 0));
					}
				}
			}
		}
		return new Vector3();
	}
	void MoveEntities(int j) { //pathfinding algorithm, seems kinda hard to implement ngl
		//ok idea, do A* (or smth) in 3 chunk radius and player places so called checkpoints it pathfinds to?
		//lets just skip that for now lol
		//TODO add pathfind
	}
	void MoveEntitiesSimple(int j) {
		entitiesglobal[j].pos += entitiesglobal[j].heading * entitiesglobal[j].speed;
	}
	Vector3 VoxelRaycast(Vector3 origin, Vector3 heading) { //1 block = 1 unit
		float maxdist = 7; //blocks (hopefully)
		Vector3 chunkspos = new Vector3(MathF.Floor(origin.X / 16), MathF.Floor(origin.Y / 16), MathF.Floor(origin.Z / 16));
		Vector3 inchunkpos = Vec3Normalize(origin, 16);
		int j = GetChunkByOffset(chunkspos);
		if(j != 99999) {
			//now the thing of smth idk
			float a1 = 0; float a2 = 0; float a3 = 0;
			bool a = 1;
			float dist = 0;
			for(;dist < maxdist;) { //lol this is valid c# ig
				//hmm robust voxel raycast kinda difficult ngl
				a1 = dist / heading.X; a2 = dist / heading.Y; a3 = dist / heading.Z;
				if(a1 <= a2 && a1 <= a3) {
					inchunkpos += heading * 
				}
				if(a2 <= a1 && a2 <= a3) { }
				if(a3 <= a2 && a3 <= a1) { }
				a = 0;
			}
		}
		return new Vector3(0, 0, 0);
	}
	void ExplodeSelf(int j) { //lol
		//TODO this
		//VoxelRaycast(entitiesglobal[j].pos, new Vector3(0, 1, 0));
		entitiesglobal.RemoveAt(j);
	}
	bool CollisionCheck(Vector3 pos) {
		Vector3 resultpos = pos / 2f;
		Vector3 chunkspos = new Vector3(MathF.Floor(resultpos.X / 16), MathF.Floor(resultpos.Y / 16), MathF.Floor(resultpos.Z / 16));
		Vector3 inchunkpos = Vec3Normalize(resultpos, 16);
		int j = GetChunkByOffset(chunkspos);
		if(j != 99999) {
			if(chunksglobal[j].bs[Vec3ToIntChunk(inchunkpos)] != 0) {
				return 1;
			}
		}
		return 0;
	}
	void EntityDeath(int i) {
		if(entitiesglobal[i].collisiontype == 0) {
			entitiesglobal.RemoveAt(i);
			return;
		}
		if(entitiesglobal[i].collisiontype == 1) {
			ExplodeSelf(i);
			return;
		}
	}
	void ProcessEntities() {
		for(int i = 0; i < entitiesglobal.Count; i++) {
			if(entitiesglobal[i].timer != -1) {
				if(entitiesglobal[i].timer == 0) {
					EntityDeath(i);
					continue;
				} else {
					entitiesglobal[i].timer -= 1;
				}
			}
			if(entitiesglobal[i].aitype == 0) {
				MoveEntitiesSimple(i);
				if(entitiesglobal[i].hascollision) {
					if(CollisionCheck(entitiesglobal[i].pos)) {
						EntityDeath(i);
					}
				}
				continue;
			}
			if(Vec3Same(entitiesglobal[i].pos, entitiesglobal[i].targetpos)) {
				entitiesglobal[i].targetpos = FindNextPos(entitiesglobal[i].pos);
			} else {
				MoveEntities(i);
			}
		}
	}
	void RenderEntitiesToArrs() {
		List<float> vertices7 = vertices6.ToList();
		List<uint> indices7 = indices6.ToList();
		Model model = new Model();
		for(int i = 0; i < entitiesglobal.Count; i++) {
			model = CopyModel(modelsglobal[entitiesglobal[i].model]);
			(vertices7, indices7) = AddModelToArrs(vertices7, indices7, entitiesglobal[i].pos, ScaleModel(RotateModel2(model, entitiesglobal[i].heading), entitiesglobal[i].scale));
		}
		vertices6 = vertices7.ToArray();
		indices6 = indices7.ToArray();
		
		//Console.WriteLine(vertices6.Length);
	}
	Vector3 EulerToVec3(Vector3 euler) { //like useless bc vey wrong, should not need that anyways lol use libraries
		//Console.WriteLine(euler.X + " " + euler.Y);
		float yaw = (euler.X + 0) / (180f / MathF.PI);
		float pitch = (euler.Y + 90) / (180f / MathF.PI);
		if(((euler.Y + 90f) % 360) < 180 && 0) { 
			return Vector3.Normalize(new Vector3(MathF.Cos(yaw) * MathF.Cos(pitch), MathF.Sin(yaw) * MathF.Cos(pitch), -MathF.Sin(pitch)));
		}
		//return Vector3.Normalize(new Vector3(MathF.Cos(yaw) * MathF.Cos(pitch), MathF.Sin(yaw) * -MathF.Cos(pitch), -MathF.Sin(pitch)));
		return Vector3.Normalize(new Vector3(MathF.Cos(yaw) * MathF.Cos(pitch), MathF.Sin(yaw), -MathF.Sin(pitch)));
		//return new Vector3(MathF.Cos(yaw) * MathF.Cos(pitch), 0, -MathF.Sin(pitch));
	}
	void SimulateParticles() {
		//vertexbufferobject4 or smth
		List<float> vertices7 = new List<float>();
		List<uint> indices7 = new List<uint>();
		vertices7.Capacity = particles.Length * 8 * 4;
		//note 8 * 4 is better than 32 bc it makes code easier to read //note 2: 8 floats per vert, 4 verts per quad
		indices7.Capacity = particles.Length * 6;
		//3 indices per quad
		for(int i = 0; i < particles.Length; i++) {
			if(particles[i] == null) { particles[i] = new Particle(); }
			particles[i].timer -= (float)particletimer.ElapsedMilliseconds / 1000f;
			if(particles[i].timer2 <= 0) { 
				Vector3 resultpos = particles[i].pos / 2f;
				Vector3 chunkspos = new Vector3(MathF.Floor(resultpos.X / 16), MathF.Floor(resultpos.Y / 16), MathF.Floor(resultpos.Z / 16));
				Vector3 inchunkpos = Vec3Normalize(resultpos, 16);
				int j = GetChunkByOffset(chunkspos);
				if(j != 99999) {
					if(chunksglobal[j].bs[Vec3ToIntChunk(inchunkpos)] != 0) {
						particles[i].move = new Vector3(0, 0, 0);
					}
				}
				particles[i].timer2 = 3; 
			}
			particles[i].timer2 -= 1;
			if(particles[i].timer <= 0) {
				particles[i] = new Particle();
				particles[i].pos = new Vector3(Random4() * 100f, Random4() * 100f, Random4() * 100f) + camerac.position / 2f;
				particles[i].move = new Vector3(Random4() / 50f, -1f / 10f, Random4() / 50f); //self explanatory
				//particles[i].color = (int)MathF.Floor((Random4() + 1) * 100f); //in the voxel color texture thingy
				particles[i].color = 7; //white
				particles[i].timer = 5 + Random4() * 4; //seconds
			}
		}
		particletimer.Restart();
		for(int i = 0; i < particles.Length; i++) {
			particles[i].pos += particles[i].move;
		}
		//now to make particles into vertices6 and indices6 //particles are player relative so have to add camerac.pos
		//also particles are square bc why not //square should be modelsglobal[4];
		Vector3 heading = new Vector3();
		for(int i = 0; i < particles.Length; i++) {
			heading = (camerac.position / 2) - particles[i].pos; //ah yes adding fake 3d to an actual 3d game lol, so dumb
			heading = Vector3.Normalize(heading);
			(vertices7, indices7) = AddModelToArrs(vertices7, indices7, particles[i].pos, ScaleModel(RotateModel2(GetModelTexture3(modelsglobal[4], particles[i].color), heading), 0.2f));
		}
		vertices6 = vertices7.ToArray();
		//vertices6 = new float[500 * 4 * 8];
		//for(int i = 0; i < vertices6.Length; i++) {
		//	vertices6[i] = Random4() * 100f;
		//}
		indices6 = indices7.ToArray();
		//for(int i = 0; i < indices6.Length; i++) {
		//	Console.Write(vertices6[i] + " ");
		//}
		//return (vertices6, indices6); //no bc 6 is global
	}
	protected override void OnUpdateFrame(FrameEventArgs args) {
		base.OnUpdateFrame(args);
	
		if(KeyboardState.IsKeyDown(Keys.Escape)) {
			SaveChunks();
			Close();
		}
		//all controls go here
		if(KeyboardState.IsKeyDown(Keys.LeftShift)) { speed = 0.7f; } else { speed = 0.35f; }
		if(KeyboardState.IsKeyDown(Keys.S)) { camerac.velocity.Z += Convert.ToSingle(args.Time) * speed * (MathF.Abs((MathF.Abs(camerac.roteuler.Y) % 360) - 180) - 90); camerac.velocity.X += Convert.ToSingle(args.Time) * speed * (MathF.Abs((MathF.Abs(camerac.roteuler.Y - 90) % 360) - 180) - 90); }
		if(KeyboardState.IsKeyDown(Keys.W)) { camerac.velocity.Z -= Convert.ToSingle(args.Time) * speed * (MathF.Abs((MathF.Abs(camerac.roteuler.Y) % 360) - 180) - 90); camerac.velocity.X -= Convert.ToSingle(args.Time) * speed * (MathF.Abs((MathF.Abs(camerac.roteuler.Y - 90) % 360) - 180) - 90); }
		if(KeyboardState.IsKeyDown(Keys.D)) { camerac.velocity.X += Convert.ToSingle(args.Time) * speed * (MathF.Abs((MathF.Abs(camerac.roteuler.Y) % 360) - 180) - 90); camerac.velocity.Z -= Convert.ToSingle(args.Time) * speed * (MathF.Abs((MathF.Abs(camerac.roteuler.Y - 90) % 360) - 180) - 90); }
		if(KeyboardState.IsKeyDown(Keys.A)) { camerac.velocity.X -= Convert.ToSingle(args.Time) * speed * (MathF.Abs((MathF.Abs(camerac.roteuler.Y) % 360) - 180) - 90); camerac.velocity.Z += Convert.ToSingle(args.Time) * speed * (MathF.Abs((MathF.Abs(camerac.roteuler.Y - 90) % 360) - 180) - 90); }
		if(KeyboardState.IsKeyDown(Keys.D1)) { inventoryslot = 1; }
		if(KeyboardState.IsKeyDown(Keys.D2)) { inventoryslot = 2; }
		if(KeyboardState.IsKeyDown(Keys.D3)) { inventoryslot = 3; }
		if(KeyboardState.IsKeyDown(Keys.D4)) { inventoryslot = 4; }
		if(KeyboardState.IsKeyDown(Keys.D5)) { inventoryslot = 5; }
		if(KeyboardState.IsKeyDown(Keys.D6)) { inventoryslot = 6; }
		if(KeyboardState.IsKeyDown(Keys.D7)) { inventoryslot = 7; }
		if(KeyboardState.IsKeyDown(Keys.D8)) { inventoryslot = 8; }
		float sensitivity = 0.08f;
		if(CursorState == CursorState.Grabbed) {
			camerac.roteuler += new Vector3(-MouseState.Delta.Y, -MouseState.Delta.X, 0) * sensitivity;
		}
		if(KeyboardState.IsKeyPressed(Keys.F)) {
			if(CursorState == CursorState.Grabbed) {
				CursorState = CursorState.Normal;
			} else {
				CursorState = CursorState.Grabbed;
			}
		}
		if(KeyboardState.IsKeyDown(Keys.Space) && camerac.onground > 0) { camerac.velocity.Y += Convert.ToSingle(args.Time) * 350f; }
		if(KeyboardState.IsKeyDown(Keys.Space)) { camerac.velocity.Y += 0.1f; }
		if(KeyboardState.IsKeyDown(Keys.LeftControl)) { camerac.position.Y -= Convert.ToSingle(args.Time) * 60f * speed; }
		if(KeyboardState.IsKeyDown(Keys.U)) { camerac.position.Y = 200; }
		if(KeyboardState.IsKeyDown(Keys.R)) { meshchanged = 1; }
		if(KeyboardState.IsKeyDown(Keys.L)) { fogstrength += 0.001f; }
		if(KeyboardState.IsKeyDown(Keys.O)) {  fogstrength -= 0.001f; }
		camerac.roteuler.X = Math.Clamp(camerac.roteuler.X, -85, 85);
		//update player physics
		for(int i = 0; i < 1; i++) {
			UpdatePlayer();
		}
		//collision and other things
		if(!KeyboardState.IsKeyDown(Keys.T)) {
			PushEntityOutOfBlock2();
		}
	
		ProcessEntities(); //self explanatory
		oldpos = camerac.position;
	
		Quaternion orientation = Quaternion.FromAxisAngle(Vector3.UnitY, MathHelper.DegToRad * camerac.roteuler.Y) * Quaternion.FromAxisAngle(Vector3.UnitX, MathHelper.DegToRad * camerac.roteuler.X);
	
		forward = Vector3.Normalize(Vector3.Transform(-Vector3.UnitZ, orientation));
		right = Vector3.Normalize(Vector3.Transform(Vector3.UnitX, orientation));
		up = Vector3.Normalize(Vector3.Transform(Vector3.UnitY, orientation));
		view = Matrix4.LookAt(camerac.position, camerac.position + forward, up);
		time += args.Time;
	
		var k = new Chunk16[0];
	
		if(!inventory.opened) {
			(Vector3 lookatpos2, int j2, Vector3 lookatpos3, int j3) = GetLookAtBlock(forward);
			if(lookatpos2 != new Vector3(0, 0, 0)) {
				Vector3 lookatpos = Vec3Normalize(lookatpos2, 16);
				if(MouseState.IsButtonPressed(MouseButton.Left)) {
					int blockid = chunksglobal[j2].bs[Vec3ToIntChunk(new Vector3((int)lookatpos.X, (int)lookatpos.Y, (int)lookatpos.Z))];
					chunksglobal[j2].bs[Vec3ToIntChunk(new Vector3((int)lookatpos.X, (int)lookatpos.Y, (int)lookatpos.Z))] = 0;
					meshchanged2 = 1;
					lock(_lock) {
						UpdateNearbyChunks(j2);
					}
					chunksglobal[j2].shouldbesaved = 1;
					int slot = GetNextSlot(blockid - 1);
					if(slot >= 0) {
						inventory.amounts[slot] += 1;
						inventory.items[slot].id = (short)(blockid - 1);
					}
				}
				Vector3 lookatpos4 = Vec3Normalize(lookatpos3, 16);
				if(MouseState.IsButtonPressed(MouseButton.Right)) {
					chunksglobal[j3].bs[Vec3ToIntChunk(new Vector3((int)lookatpos4.X, (int)lookatpos4.Y, (int)lookatpos4.Z))] = inventoryslot;
					meshchanged2 = 1;
					lock(_lock) {
						UpdateNearbyChunks(j3);
					}
					chunksglobal[j3].shouldbesaved = 1;
				}
			} else { //so if can't place blocks
			}
		}
	
		if(KeyboardState.IsKeyPressed(Keys.G)) {			
			Entity e = new Entity {
				hascollision = 1,
				speed = 0.02f,
				//heading = EulerToVec3(camerac.roteuler),
				heading = forward,
				pos = camerac.position / 2f,
				timer = 500,
				collisiontype = 0
			};
			entitiesglobal.Add(e);
			//Console.WriteLine(entitiesglobal.Count); Console.WriteLine(entitiesglobal.Count); Console.WriteLine(entitiesglobal.Count); Console.WriteLine(entitiesglobal.Count);
		}
		if(physicstask == null) {
			physicstask = Task.Run(() => {
				//do the physics for everything else
				meshchanged = OnUpdatePhysics(); //no lock bc OnUpdatePhysics should only update uninited chunks and nothing else should do that
				return meshchanged;
			});
		}
		if(physicstask != null && physicstask.IsCompleted) {
			meshchanged = physicstask.Result;
			physicstask = null;
		}
	
		if(meshchanged && remeshtask == null) { // turns right chunks to verts[][] and indices[][]
			remeshtask = Task.Run(() => {
				lock (_lock) {
					(vertsglobal, indicesglobal) = ChunksToFloatArr2(chunksglobal, modelsglobal, vertsglobal, indicesglobal);
				}
				return (vertsglobal, indicesglobal);
			});
		}
		if(remeshtask != null && remeshtask.IsCompleted) {
			lock(_lock) {
				(vertsglobal, indicesglobal) = remeshtask.Result;
			}
			remeshtask = null;
			meshchanged2 = 1; meshchanged = 0;
		}
	
		if(!KeyboardState.IsKeyDown(Keys.R)) {
			//RenderUI();
		}
		if(meshchanged2) {
			lock(_lock) {
				(vertices4, indices4) = MergeFloatArrs4(vertsglobal, indicesglobal); // around 15 ms for 16x16 render dist so should be fine
			}
			(vertices5, indices5) = (vertices4, indices4);
			GL.BindVertexArray(VertexArrayObject);
			GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject);
			GL.BufferData(BufferTarget.ArrayBuffer, vertices5.Length * sizeof(float), vertices5, BufferUsageHint.DynamicDraw);
	
			GL.BindBuffer(BufferTarget.ElementArrayBuffer, ElementBufferObject);
			GL.BufferData(BufferTarget.ElementArrayBuffer, indices5.Length * sizeof(uint), indices5, BufferUsageHint.DynamicDraw);
			meshchanged2 = 0;
		} else { // do vertices 5 and UI together should be fast enough, nope will redo that
			(vertices5, indices5) = (vertices4, indices4);
			GL.BindVertexArray(VertexArrayObject);
			GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject);
			GL.BufferData(BufferTarget.ArrayBuffer, vertices5.Length * sizeof(float), vertices5, BufferUsageHint.DynamicDraw);
	
			GL.BindBuffer(BufferTarget.ElementArrayBuffer, ElementBufferObject);
			GL.BufferData(BufferTarget.ElementArrayBuffer, indices5.Length * sizeof(uint), indices5, BufferUsageHint.DynamicDraw);
		}
	
		SimulateParticles();
		RenderEntitiesToArrs(); //Renders to vertices6 and indices6 with the same draw call
		//one big texture for all the textures of entities, or maybe more
	
		//these are going to update every frame probably no need to check if they do
		Console.SetCursorPosition(0, 5);
		Console.Write(vertices6.Length);
		GL.BindVertexArray(VertexArrayObject3); //3 for 3+2+3 vao
		GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject4);
		GL.BufferData(BufferTarget.ArrayBuffer, vertices6.Length * sizeof(float), vertices6, BufferUsageHint.DynamicDraw);
	
		GL.BindBuffer(BufferTarget.ElementArrayBuffer, ElementBufferObject4);
		GL.BufferData(BufferTarget.ElementArrayBuffer, indices6.Length * sizeof(uint), indices6, BufferUsageHint.DynamicDraw);
	}
	
	protected override void OnLoad() //basically init
	{
		entitiesglobal.Add(new Entity());
		entitiesglobal[0] = new Entity();
		entitiesglobal[0].model = 1;
		entitiesglobal[0].pos = new Vector3(35, 8, 90);
		entitiesglobal[0].heading = Vector3.Normalize(new Vector3(1, 0, 0));
		entitiesglobal[0].speed = 0.1f;
		entitiesglobal[0].scale = 5f;
		Console.Clear();
		for(int i = 0; i < uiindices.Length; i++) {
			uiindices[i] = new uint[0];
		}
		for(int i = 0; i < uiverts.Length; i++) {
			uiverts[i] = new float[0];
		}
		randomseed = 1456545147;
		//networking stuff
	
		//game stuff
		ItemManager.LoadAtts();
		//inventory bc i don't want to scroll far lol
		for(int i = 0; i < 10; i++) {
			inventory.items[i].id = (short)i; inventory.amounts[i] = 999;
		}
		//stuff init
		string dir = Directory.GetCurrentDirectory();
		int scrwidth = 1920; int scrheight = 1080;
		screenwidth = scrwidth; screenheight = scrheight;
		Random random = new Random();
	
		//mesh inits many
		string line;
		string[] lines = new string[4];
		string[][] lines2 = new string[3][];
		int[][] lines3 = new int[3][];
		for(int i = 0; i < 3; i++) {
			lines2[i] = new string[3];
			lines3[i] = new int[3];
		}
		Model[] models = new Model[6];
		//Camera cam2 = new Camera();
	
		//counters
		int c1; int c2; int c3; int c4;
		int g1; int g2; int g3; int g4;
	
		//loops through all available model files
		for(int i = 0; i < 10; i++) {
			c1 = 0; c2 = 0; c3 = 0; c4 = 0;
			g1 = 0; g2 = 0; g3 = 0; g4 = 0;
			StreamReader sr;
			try {
				sr = new StreamReader(dir + "/stuff/meshes/mesh" + i + ".txt");
			}
			catch(Exception) {
				continue;
			}
	
			//reads through and marks amount of verts uvs and so on
			while(1) {
				line = sr.ReadLine();
				if(line != null) {
					if(line[0] == 'v' && line[1] == ' ') { g1++; }
					if(line[0] == 'v' && line[1] == 'n') { g2++; }
					if(line[0] == 'v' && line[1] == 't') { g3++; }
					if(line[0] == 'f' && line[1] == ' ') { g4++; }
				} else { break; }
			}
			models[i].faces = new int[g4][][];
			//color
			models[i].color = new int[g4];
			for(int i2 = 0; i2 < g4; i2++) {
				models[i].color[i2] = random.Next(255);
			}
			for(int i2 = 0; i2 < g4; i2++) {
				models[i].faces[i2] = new int[3][];
				for(int i3 = 0; i3 < 3; i3++) {
					models[i].faces[i2][i3] = new int[3];
				}
			}
			models[i].verts = new Vector3[g1];
			models[i].normals = new Vector3[g2];
			models[i].uvs = new Vector2[g3];
			sr.Close();
	
			//now read actual data
			sr = new StreamReader(dir + "/stuff/meshes/mesh" + i + ".txt");
			while(1) {
				line = sr.ReadLine();
				if(line != null) {
					if(line[0] == 'v' && line[1] == 'n') {
						lines = line.Split(' ');
						models[i].normals[c1].X = Convert.ToSingle(lines[1]);
						models[i].normals[c1].Y = Convert.ToSingle(lines[2]);
						models[i].normals[c1].Z = Convert.ToSingle(lines[3]);
						c1++;
					} else if(line[0] == 'v' && line[1] == 't') {
						lines = line.Split(' ');
						models[i].uvs[c2].X = Convert.ToSingle(lines[1]);
						models[i].uvs[c2].Y = Convert.ToSingle(lines[2]);
						c2++;
					} else if(line[0] == 'v' && line[1] == ' ') {
						lines = line.Split(' ');
						models[i].verts[c3].X = Convert.ToSingle(lines[1]);
						models[i].verts[c3].Y = Convert.ToSingle(lines[2]);
						models[i].verts[c3].Z = Convert.ToSingle(lines[3]);
						c3++;
					} else if(line[0] == 'f') {
						lines = line.Split(' ');
						for(int i2 = 1; i2 < lines.Length; i2++) {
							lines2[i2 - 1] = lines[i2].Split('/');
						}
						for(int i3 = 0; i3 < 3; i3++) {
							for(int i4 = 0; i4 < 3; i4++) {
								models[i].faces[c4][i3][i4] = Convert.ToInt32(lines2[i3][i4]) - 1;
							}
						}
						c4++;
					}
				} else { break; }
			}
			sr.Close();
		}
		Console.SetCursorPosition(0, 1);
		Console.Write("Loaded models");
	
		modelsglobal = models;
		//feature loading
	
		Chunk16[] features = new Chunk16[10];
	
		(features, featureoffsetsg) = LoadFeaturesFromTxt(features);
	
		featuresglobal = features;
		Console.SetCursorPosition(0, 1);
		Console.Write("Loaded Structures"); // called features because I couldn't remeber the right word lol
	
		//world init
		chunkdist = 32;
		chunksglobal = new Chunk16[(chunkdist) * (chunkdist) * 8];
		for(int i = 0; i < chunksglobal.Length; i++) {
			chunksglobal[i] = new Chunk16();
		}
	
		string path = "/stuff/textures/noiseTexture.png";
		dir = Directory.GetCurrentDirectory();
		ImageResult image = ImageResult.FromStream(File.OpenRead(dir + path), ColorComponents.Grey);
		imageglobal = image;
	
		//ConnectToMultiplayer("localhost", 1069, "Player");
	
		if(1) { // so doesn't even try to load chunks from memory when joining multiplayer, very elegant, don't have to change much in code yay
			string[] strings = new string[1000]; string s; int o = 0; string[] s2 = new string[3];
			try {
				StreamReader sr3 = new StreamReader(Directory.GetCurrentDirectory() + "/stuff/chunks/init.txt");
				for(int i = 0; i < 1000; i++) {
					s = sr3.ReadLine();
					if(s == null) {
						break;
					}
					strings[i] = s;
					o++;
				}
				sr3.Close();
			}
			catch(Exception) {
			}
			for(int i = 0; i < o; i++) {
				s2 = strings[i].Split(' ');
				savedchunks[i].X = Convert.ToSingle(s2[0]);
				savedchunks[i].Y = Convert.ToSingle(s2[1]);
				savedchunks[i].Z = Convert.ToSingle(s2[2]);
			}
		}
	
		//in some radius gen chunks 
		Vector3 offset = new Vector3 { X = 0, Y = 0, Z = 0 };
		for(int i1 = 0; i1 < chunkdist; i1++) // uhh no chunk order it is fucked, anyways to difficult to add use NeighbouringChunk
		{
			for(int i2 = 0; i2 < chunkdist; i2++) {
				for(int i3 = 0; i3 < 8; i3++) {
					offset = new Vector3 { X = i1, Y = i3, Z = i2 };
					int j = -1;
					for(int i = 0; i < chunksglobal.Length; i++) {
						if(chunksglobal[i].inited == 0) {
							j = i;
							break;
						}
					}
					if(j != -1) {
						chunksglobal[j] = GenChunk16(offset, image, features, featureoffsetsg, j);
					}
					//Loading bar YAYAYAYAY, taking up like 5% of the time lol
					Console.SetCursorPosition(0, 0);
					string s = "Loading chunks[";
					for(int i4 = 1; i4 < 15; i4++) {
						if(j / (float)chunksglobal.Length > i4 / 15f) {
							s += "#";
						} else {
							s += "-";
						}
					}
					s += "]";
					Console.Write(s);
	
					if(ChunkHasData(offset)) {
						chunksglobal[j] = LoadChunkFromFile(offset, j);
						meshchanged = 1;
					}
				}
			}
		}
	
		//no more game stuff
		fov = 85; near = 0.1f; far = 10000.0f;
		projection = Matrix4.CreatePerspectiveFieldOfView(MathHelper.DegreesToRadians(fov), screenwidth2 / screenheight2, near, far);
		scale = Matrix4.CreateScale(2f);
	
		//useful ig
		int modelint = 3;
		indices = FaceToUIntArr(models[modelint].faces);
		float[] vertices = Vec3ToFloatArrAndUVData(models[modelint].verts, models[modelint].uvs);
	
		vertsglobal = new float[chunksglobal.Length][]; //init arrs
		indicesglobal = new uint[chunksglobal.Length][];
		for(int i = 0; i < chunksglobal.Length; i++) {
			vertsglobal[i] = new float[0];
			indicesglobal[i] = new uint[0];
		}
	
		(vertices, indices) = ChunksToFloatArr(chunksglobal, models, vertices, indices, vertsglobal);
	
		//Console.WriteLine(vertices.Length);
	
		camerac = new Camera { roteuler = new Vector3(0, 0, 0), position = new Vector3(100, 500, 100), velocity = new Vector3(0, 0, 0) };
	
		//rendering (all things should be put into vertices and indices by now)
	
		base.OnLoad();
	
		Texture.Use();
		GL.ClearColor(0.4f, 0.6f, 0.7f, 1.0f);
	
		VertexBufferObject = GL.GenBuffer();
	
		GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject);
	
		GL.BufferData(BufferTarget.ArrayBuffer, vertices.Length * sizeof(float), vertices, BufferUsageHint.DynamicDraw);
	
		shader = new Shader("shader.vert", "shader.frag");
	
		VertexArrayObject = GL.GenVertexArray();
		GL.BindVertexArray(VertexArrayObject);
	
		ElementBufferObject = GL.GenBuffer();
		GL.BindBuffer(BufferTarget.ElementArrayBuffer, ElementBufferObject);
		GL.BufferData(BufferTarget.ElementArrayBuffer, indices.Length * sizeof(uint), indices, BufferUsageHint.DynamicDraw);
	
		GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, 0, 7 * sizeof(float), 0);
		GL.EnableVertexAttribArray(0);
	
		GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, 0, 7 * sizeof(float), 3 * sizeof(float));
		GL.EnableVertexAttribArray(1);
	
		GL.VertexAttribPointer(2, 1, VertexAttribPointerType.Float, 0, 7 * sizeof(float), 6 * sizeof(float));
		GL.EnableVertexAttribArray(2);
	
		//2
		VertexBufferObject2 = GL.GenBuffer();
	
		GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject2);
	
		float[] square = [-1f, -1f, 0f, 1f, -1f, 0f, 1f,  1f, 0f, -1f,  1f, 0f];
		GL.BufferData(BufferTarget.ArrayBuffer, 12 * sizeof(float), square, BufferUsageHint.StaticDraw);
	
		shader2 = new Shader("shader2.vert", "shader2.frag");
	
		VertexArrayObject2 = GL.GenVertexArray();
		GL.BindVertexArray(VertexArrayObject2);
	
		ElementBufferObject2 = GL.GenBuffer();
		GL.BindBuffer(BufferTarget.ElementArrayBuffer, ElementBufferObject2);
	
		uint[] squareuint = [0, 1, 2, 0, 2, 3];
		GL.BufferData(BufferTarget.ElementArrayBuffer, 6 * sizeof(uint), squareuint, BufferUsageHint.StaticDraw);
	
		GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, 0, 3 * sizeof(float), 0);
		GL.EnableVertexAttribArray(0);
	
		//3 (vao aand vbo and whatever)
		
		VertexBufferObject3 = GL.GenBuffer();
	
		shader6 = new Shader("shader6.vert", "shader6.frag");
	
		VertexArrayObject3 = GL.GenVertexArray();
		GL.BindVertexArray(VertexArrayObject3);
	
		ElementBufferObject3 = GL.GenBuffer();
	
		//2.5 (for particle system), so the buffer doesn't get absolutely huge
		VertexBufferObject4 = GL.GenBuffer();
		GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject4);
	
		ElementBufferObject4 = GL.GenBuffer();
	
		GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, 0, 8 * sizeof(float), 0);
		GL.EnableVertexAttribArray(0);
	
		GL.VertexAttribPointer(1, 2, VertexAttribPointerType.Float, 0, 8 * sizeof(float), 3 * sizeof(float));
		GL.EnableVertexAttribArray(1);
	
		GL.VertexAttribPointer(2, 3, VertexAttribPointerType.Float, 0, 8 * sizeof(float), 5 * sizeof(float));
		GL.EnableVertexAttribArray(2);
		//3 (only shaders) also img and fbo
	
		GL.ActiveTexture(TextureUnit.Texture1);
		shader3 = new Shader("shader3.vert", "shader3.frag");
		shader4 = new Shader("shader4.vert", "shader4.frag");
		shader5 = new Shader("shader5.vert", "shader5.frag");
	
		int[] fbos = new int[4]; int[] textures = new int[5];
	
		GL.GenFramebuffers(4, fbos);
		GL.GenTextures(5, textures);
	
		fbo3 = fbos[0]; shadowtexture = textures[0];
	
		GL.BindTexture(TextureTarget.Texture2D, shadowtexture);
	
		GL.BindFramebuffer(FramebufferTarget.Framebuffer, fbo3);
		
		//GL.Clear(ClearBufferMask.DepthBufferBit | ClearBufferMask.ColorBufferBit);
		GL.DrawBuffer(DrawBufferMode.None);
		GL.ReadBuffer(ReadBufferMode.None);
	
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.Nearest);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Nearest);
		//everything outside the border is white so no shadow instead of all shadow
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (float)TextureWrapMode.ClampToBorder);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (float)TextureWrapMode.ClampToBorder);
		float[] borderColor = [1.0f, 1.0f, 1.0f, 1.0f];
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureBorderColor, borderColor);
		GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.DepthComponent, screenwidth2 * shadowq, screenheight2 * shadowq, 0, PixelFormat.DepthComponent, PixelType.Float, nint.Zero);
	
		GL.FramebufferTexture2D(FramebufferTarget.Framebuffer, FramebufferAttachment.DepthAttachment, TextureTarget.Texture2D, shadowtexture, 0);
	
		var status = GL.CheckFramebufferStatus(FramebufferTarget.Framebuffer);
		if(status != FramebufferErrorCode.FramebufferComplete) {
			Console.WriteLine("Framebuffer incomplete!"); //привет лол я рассказу в русски языком, у меня ест пресентацы савтра, я знаю эта языком оцен плоха
		}
	
		//texture 2
		GL.ActiveTexture(TextureUnit.Texture2);
	
		fbo4 = fbos[1]; shadowtexture2 = textures[1];
	
		GL.BindTexture(TextureTarget.Texture2D, shadowtexture2);
	
		GL.BindFramebuffer(FramebufferTarget.Framebuffer, fbo4);
	
		//GL.Clear(ClearBufferMask.DepthBufferBit | ClearBufferMask.ColorBufferBit);
		GL.DrawBuffer(DrawBufferMode.None);
		GL.ReadBuffer(ReadBufferMode.None);
	
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.Nearest);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Nearest);
		//everything outside the border is white so no shadow instead of all shadow
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (float)TextureWrapMode.ClampToBorder);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (float)TextureWrapMode.ClampToBorder);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureBorderColor, borderColor);
		GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.DepthComponent, screenwidth2 * shadowq, screenheight2 * shadowq, 0, PixelFormat.DepthComponent, PixelType.Float, nint.Zero);
	
		GL.FramebufferTexture2D(FramebufferTarget.Framebuffer, FramebufferAttachment.DepthAttachment, TextureTarget.Texture2D, shadowtexture2, 0);
	
	   //texture 3 //da sky
		GL.ActiveTexture(TextureUnit.Texture3);
	
		fbo5 = fbos[2]; skytexture = textures[2];
	
		GL.BindFramebuffer(FramebufferTarget.Framebuffer, fbo5);
	
		GL.BindTexture(TextureTarget.Texture2D, skytexture);
	
		GL.DrawBuffer(DrawBufferMode.Front);
		GL.ReadBuffer(ReadBufferMode.None);
	
		GL.Clear(ClearBufferMask.DepthBufferBit | ClearBufferMask.ColorBufferBit);
	
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.Linear);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Linear);
	
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (float)TextureWrapMode.ClampToBorder);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (float)TextureWrapMode.ClampToBorder);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureBorderColor, borderColor);
		GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.Rgba, screenwidth2, screenheight2, 0, PixelFormat.Rgba, PixelType.UnsignedByte, nint.Zero);
	
		GL.FramebufferTexture2D(FramebufferTarget.Framebuffer, FramebufferAttachment.ColorAttachment0, TextureTarget.Texture2D, skytexture, 0);
	
		//main texture thing
		GL.ActiveTexture(TextureUnit.Texture4);
	
		fbo6 = fbos[3]; maintexture = textures[3];
	
		GL.BindFramebuffer(FramebufferTarget.Framebuffer, fbo6);
	
		GL.BindTexture(TextureTarget.Texture2D, maintexture);
	
		//GL.DrawBuffer(DrawBufferMode.Front);
		//GL.ReadBuffer(ReadBufferMode.Front);
	
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.Nearest);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Nearest);
	
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (float)TextureWrapMode.ClampToBorder);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (float)TextureWrapMode.ClampToBorder);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureBorderColor, borderColor);
		GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.Rgba, screenwidth2, screenheight2, 0, PixelFormat.Rgba, PixelType.UnsignedByte, nint.Zero);
	
		GL.FramebufferTexture2D(FramebufferTarget.Framebuffer, FramebufferAttachment.ColorAttachment0, TextureTarget.Texture2D, maintexture, 0);
	
		//main texture shadow (fbo6)
	
		GL.ActiveTexture(TextureUnit.Texture5);
	
		shadowtexture3 = textures[4];
	
		GL.BindTexture(TextureTarget.Texture2D, shadowtexture3);
	
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.Nearest);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Nearest);
		//everything outside the border is white so no shadow instead of all shadow
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (float)TextureWrapMode.ClampToBorder);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (float)TextureWrapMode.ClampToBorder);
		GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureBorderColor, borderColor);
		GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.DepthComponent, screenwidth2, screenheight2, 0, PixelFormat.DepthComponent, PixelType.Float, nint.Zero);
	
		GL.FramebufferTexture2D(FramebufferTarget.Framebuffer, FramebufferAttachment.DepthAttachment, TextureTarget.Texture2D, shadowtexture3, 0);
	
	}
	
	protected override void OnRenderFrame(FrameEventArgs args) {
	
		base.OnRenderFrame(args);
	
		if(1) {
		Matrix4 model = scale * rotation * translation;
	
		float[] viewf = Matrix4ToFloatArr(view);
		float[] projectionf = Matrix4ToFloatArr(projection);
		float[] modelf = Matrix4ToFloatArr(model);
	
		Vector3 sunpos = GetSunPosFake();
		Matrix4 projection2 = Matrix4.CreateOrthographic(1500, 1500, 0.1f, 1000);
		//Matrix4 projection3 = Matrix4.CreatePerspectiveFieldOfView(180, screenwidth / screenheight, 0.05f, 100f);
		view2 = Matrix4.LookAt(Vector3.Normalize(new Vector3(sunpos.X, -sunpos.Y, sunpos.Z)) * 500, new Vector3(0, 0, 0), new Vector3(0, 1, 0));
	
		float[] viewf2 = Matrix4ToFloatArr(view2);
		float[] projectionf2 = Matrix4ToFloatArr(projection2);
	
		float pitch = camerac.roteuler.X * (float.Pi / 180f); float yaw = camerac.roteuler.Y * (float.Pi / 180f);
		Vector3 viewdir = new Vector3(MathF.Cos(pitch) * MathF.Sin(yaw), MathF.Sin(pitch), MathF.Cos(pitch) * MathF.Cos(yaw));
	
		//other renderings put away for ease of view
		(float[] viewf3, float[] projectionf3) = RenderShadowToTexture(1, sunpos); // two shadow maps yay
		(float[] viewf4, float[] projectionf4) = RenderShadowToTexture(2, sunpos);
	
		RenderSky(viewdir);
		RenderSkyToView();
	
		//now the main pass
	
		RenderMain(viewf3, projectionf3, viewf4, projectionf4);
		//RenderEntities();
		RenderParticles(viewf3, projectionf3, viewf4, projectionf4);
		}
	
		if(1) {
		//and the other thing
		shader5.Use();
		GL.BindFramebuffer(FramebufferTarget.Framebuffer, 0);
	
		GL.Viewport(0, 0, screenwidth, screenheight);
		GL.BindVertexArray(VertexArrayObject2);
		GL.BindBuffer(BufferTarget.ElementArrayBuffer, ElementBufferObject2);
		GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject2);
	
		GL.Disable(EnableCap.DepthTest);
	
		GL.ActiveTexture(TextureUnit.Texture4);
		int location = GL.GetUniformLocation(shader5.Handle, "texture4");
		GL.Uniform1(location, 4);
		int location9 = GL.GetUniformLocation(shader5.Handle, "imgres");
		GL.Uniform2(location9, new Vector2(screenwidth2, screenheight2));
		int location10 = GL.GetUniformLocation(shader5.Handle, "finalres");
		GL.Uniform2(location10, new Vector2(screenwidth, screenheight));
		int location11 = GL.GetUniformLocation(shader5.Handle, "seed");
		GL.Uniform1(location11, 678f);
	
		GL.DrawElements(PrimitiveType.Triangles, 6, DrawElementsType.UnsignedInt, 0);
		}
	
		SwapBuffers();
	
	
		while(timer.ElapsedMilliseconds < 10) {
			Thread.SpinWait(10);
		}
		timer.Restart();
	
	
		Console.SetCursorPosition(0, 3);
		Console.Write(args.Time + "                       ");
		Console.SetCursorPosition(0, 4);
		Console.Write(vertices4.Length + "   ");
	}
	
	//ok here can go more functions
	void RenderParticles(float[] viewf3, float[] projectionf3, float[] viewf4, float[] projectionf4) {
		//Same as RenderMain, but different vbo vao ebo
		Matrix4 model = scale * rotation * translation;
	
		float[] viewf = Matrix4ToFloatArr(view);
		float[] projectionf = Matrix4ToFloatArr(projection);
		float[] modelf = Matrix4ToFloatArr(model);
	
		Vector3 sunpos = GetSunPosFake();
		Matrix4 projection2 = Matrix4.CreateOrthographic(1500, 1500, 0.1f, 1000);
		//Matrix4 projection3 = Matrix4.CreatePerspectiveFieldOfView(180, screenwidth / screenheight, 0.05f, 100f);
		view2 = Matrix4.LookAt(Vector3.Normalize(new Vector3(sunpos.X, -sunpos.Y, sunpos.Z)) * 500, new Vector3(0, 0, 0), new Vector3(0, 1, 0));
	
		float[] viewf2 = Matrix4ToFloatArr(view2);
		float[] projectionf2 = Matrix4ToFloatArr(projection2);
	
		float pitch = camerac.roteuler.X * (float.Pi / 180f); float yaw = camerac.roteuler.Y * (float.Pi / 180f);
		Vector3 viewdir = new Vector3(MathF.Cos(pitch) * MathF.Sin(yaw), MathF.Sin(pitch), MathF.Cos(pitch) * MathF.Cos(yaw));
	
		GL.Viewport(0, 0, screenwidth2, screenheight2);
		shader6.Use();
	
		GL.BindFramebuffer(FramebufferTarget.Framebuffer, fbo6);
	
		GL.Enable(EnableCap.DepthTest);
		GL.ClearDepth(1.0);
		//GL.Clear(ClearBufferMask.DepthBufferBit);
	
		GL.BindVertexArray(VertexArrayObject3);
		GL.BindBuffer(BufferTarget.ElementArrayBuffer, ElementBufferObject4);
		//GL.BufferData(BufferTarget.ElementArrayBuffer, indices6.Length * sizeof(uint), indices6, BufferUsageHint.DynamicDraw);
		GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject4);
		//GL.BufferData(BufferTarget.ArrayBuffer, vertices6.Length * sizeof(float), vertices6, BufferUsageHint.DynamicDraw);
		//for(int i = 0; i < indices6.Length; i++) {
		//	Console.Write(vertices6[i] + " ");
		//}
	
		int location = GL.GetUniformLocation(shader6.Handle, "model");
		GL.UniformMatrix4(location, 1, 0, modelf);
		int location2 = GL.GetUniformLocation(shader6.Handle, "view");
		GL.UniformMatrix4(location2, 1, 0, viewf);
		int location3 = GL.GetUniformLocation(shader6.Handle, "projection");
		GL.UniformMatrix4(location3, 1, 0, projectionf);
		int location4 = GL.GetUniformLocation(shader6.Handle, "lightdir");
		GL.Uniform3(location4, GetSunPosFake());
		int location5 = GL.GetUniformLocation(shader6.Handle, "lightcol");
		GL.Uniform3(location5, 0.7f, 0.7f, 1f);
		int location6 = GL.GetUniformLocation(shader6.Handle, "viewdir");
		GL.Uniform3(location6, viewdir);
		GL.ActiveTexture(TextureUnit.Texture0);
		int location8 = GL.GetUniformLocation(shader6.Handle, "texture0");
		GL.Uniform1(location8, 0);
		GL.ActiveTexture(TextureUnit.Texture1);
		int location7 = GL.GetUniformLocation(shader6.Handle, "texture1");
		GL.Uniform1(location7, 1);
		GL.ActiveTexture(TextureUnit.Texture2);
		int location14 = GL.GetUniformLocation(shader6.Handle, "texture2");
		GL.Uniform1(location14, 2);
		int location9 = GL.GetUniformLocation(shader6.Handle, "resolution");
		GL.Uniform2(location9, new Vector2(screenwidth2, screenheight2));
		int location12 = GL.GetUniformLocation(shader6.Handle, "model2");
		GL.UniformMatrix4(location12, 1, 0, modelf);
		int location13 = GL.GetUniformLocation(shader6.Handle, "shadowSmoothingLvl");
		GL.Uniform1(location13, 1);
		int location15 = GL.GetUniformLocation(shader6.Handle, "view3");
		GL.UniformMatrix4(location15, 1, 0, viewf3);
		int location16 = GL.GetUniformLocation(shader6.Handle, "projection3");
		GL.UniformMatrix4(location16, 1, 0, projectionf3);
		int location17 = GL.GetUniformLocation(shader6.Handle, "view4");
		GL.UniformMatrix4(location17, 1, 0, viewf4);
		int location18 = GL.GetUniformLocation(shader6.Handle, "projection4");
		GL.UniformMatrix4(location18, 1, 0, projectionf4);
		int location19 = GL.GetUniformLocation(shader6.Handle, "ranseed");
		GL.Uniform1(location19, Random3(uint.MaxValue) / (float)uint.MaxValue);
		int location20 = GL.GetUniformLocation(shader6.Handle, "fogstrength");
		GL.Uniform1(location20, fogstrength);
	
		GL.DrawElements(PrimitiveType.Triangles, indices6.Length, DrawElementsType.UnsignedInt, 0);
	}
	void RenderEntities() {
		
	}
	
	void RenderMain(float[] viewf3, float[] projectionf3, float[] viewf4, float[] projectionf4) {
		Matrix4 model = scale * rotation * translation;
	
		float[] viewf = Matrix4ToFloatArr(view);
		float[] projectionf = Matrix4ToFloatArr(projection);
		float[] modelf = Matrix4ToFloatArr(model);
	
		Vector3 sunpos = GetSunPosFake();
		Matrix4 projection2 = Matrix4.CreateOrthographic(1500, 1500, 0.1f, 1000);
		//Matrix4 projection3 = Matrix4.CreatePerspectiveFieldOfView(180, screenwidth / screenheight, 0.05f, 100f);
		view2 = Matrix4.LookAt(Vector3.Normalize(new Vector3(sunpos.X, -sunpos.Y, sunpos.Z)) * 500, new Vector3(0, 0, 0), new Vector3(0, 1, 0));
	
		float[] viewf2 = Matrix4ToFloatArr(view2);
		float[] projectionf2 = Matrix4ToFloatArr(projection2);
	
		float pitch = camerac.roteuler.X * (float.Pi / 180f); float yaw = camerac.roteuler.Y * (float.Pi / 180f);
		Vector3 viewdir = new Vector3(MathF.Cos(pitch) * MathF.Sin(yaw), MathF.Sin(pitch), MathF.Cos(pitch) * MathF.Cos(yaw));
	
	
		//da sky
		GL.Viewport(0, 0, screenwidth2, screenheight2);
		shader.Use();
	
		GL.BindFramebuffer(FramebufferTarget.Framebuffer, fbo6);
	
		GL.Enable(EnableCap.DepthTest);
		GL.ClearDepth(1.0);
		GL.Clear(ClearBufferMask.DepthBufferBit);
	
		GL.BindVertexArray(VertexArrayObject);
		GL.BindBuffer(BufferTarget.ElementArrayBuffer, ElementBufferObject);
		GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject);
	
		int location = GL.GetUniformLocation(shader.Handle, "model");
		GL.UniformMatrix4(location, 1, 0, modelf);
		int location2 = GL.GetUniformLocation(shader.Handle, "view");
		GL.UniformMatrix4(location2, 1, 0, viewf);
		int location3 = GL.GetUniformLocation(shader.Handle, "projection");
		GL.UniformMatrix4(location3, 1, 0, projectionf);
		int location4 = GL.GetUniformLocation(shader.Handle, "lightdir");
		GL.Uniform3(location4, GetSunPosFake());
		int location5 = GL.GetUniformLocation(shader.Handle, "lightcol");
		GL.Uniform3(location5, 0.7f, 0.7f, 1f);
		int location6 = GL.GetUniformLocation(shader.Handle, "viewdir");
		GL.Uniform3(location6, viewdir);
		GL.ActiveTexture(TextureUnit.Texture0);
		int location8 = GL.GetUniformLocation(shader.Handle, "texture0");
		GL.Uniform1(location8, 0);
		GL.ActiveTexture(TextureUnit.Texture1);
		int location7 = GL.GetUniformLocation(shader.Handle, "texture1");
		GL.Uniform1(location7, 1);
		GL.ActiveTexture(TextureUnit.Texture2);
		int location14 = GL.GetUniformLocation(shader.Handle, "texture2");
		GL.Uniform1(location14, 2);
		int location9 = GL.GetUniformLocation(shader.Handle, "resolution");
		GL.Uniform2(location9, new Vector2(screenwidth2, screenheight2));
		int location12 = GL.GetUniformLocation(shader.Handle, "model2");
		GL.UniformMatrix4(location12, 1, 0, modelf);
		int location13 = GL.GetUniformLocation(shader.Handle, "shadowSmoothingLvl");
		GL.Uniform1(location13, 1);
		int location15 = GL.GetUniformLocation(shader.Handle, "view3");
		GL.UniformMatrix4(location15, 1, 0, viewf3);
		int location16 = GL.GetUniformLocation(shader.Handle, "projection3");
		GL.UniformMatrix4(location16, 1, 0, projectionf3);
		int location17 = GL.GetUniformLocation(shader.Handle, "view4");
		GL.UniformMatrix4(location17, 1, 0, viewf4);
		int location18 = GL.GetUniformLocation(shader.Handle, "projection4");
		GL.UniformMatrix4(location18, 1, 0, projectionf4);
		int location19 = GL.GetUniformLocation(shader.Handle, "ranseed");
		GL.Uniform1(location19, Random3(uint.MaxValue) / (float)uint.MaxValue);
		int location20 = GL.GetUniformLocation(shader.Handle, "fogstrength");
		GL.Uniform1(location20, fogstrength);
	
		GL.DrawElements(PrimitiveType.Triangles, indices.Length, DrawElementsType.UnsignedInt, 0);
	}
	void RenderSky(Vector3 viewdir) {
		//da sky
		GL.Viewport(0, 0, screenwidth2, screenheight2);
		shader2.Use();
	
		GL.Disable(EnableCap.DepthTest);
		GL.BindFramebuffer(FramebufferTarget.Framebuffer, fbo5);
		GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
	
		GL.BindVertexArray(VertexArrayObject2);
		GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject2);
		GL.BindBuffer(BufferTarget.ElementArrayBuffer, ElementBufferObject2);
	
		int location24 = GL.GetUniformLocation(shader2.Handle, "lightdir");
		GL.Uniform3(location24, GetSunPosFake());
		int location25 = GL.GetUniformLocation(shader2.Handle, "skycol");
		GL.Uniform4(location25, 0.4f, 0.6f, 0.7f, 1f);
		int location26 = GL.GetUniformLocation(shader2.Handle, "viewdir");
		GL.Uniform3(location26, viewdir);
		int location27 = GL.GetUniformLocation(shader2.Handle, "resolution");
		GL.Uniform2(location27, new Vector2(screenwidth2, screenheight2));
		int location2 = GL.GetUniformLocation(shader2.Handle, "ranseed");
		GL.Uniform1(location2, Random3(uint.MaxValue) / (float)uint.MaxValue);
	
		GL.DrawElements(PrimitiveType.Triangles, 6, DrawElementsType.UnsignedInt, 0);
	}
	void RenderSkyToView() {
		//render the texture created as fullscreen quad (skytexture)
		
		GL.Viewport(0, 0, screenwidth2, screenheight2);
		shader4.Use();
	
		GL.Disable(EnableCap.DepthTest);
		GL.BindFramebuffer(FramebufferTarget.Framebuffer, fbo6);
	
		GL.BindVertexArray(VertexArrayObject2);
		GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject2);
		GL.BindBuffer(BufferTarget.ElementArrayBuffer, ElementBufferObject2);
	
		GL.ActiveTexture(TextureUnit.Texture3);
		int location = GL.GetUniformLocation(shader4.Handle, "texture3");
		GL.Uniform1(location, 3);
		int location9 = GL.GetUniformLocation(shader4.Handle, "resolution");
		GL.Uniform2(location9, new Vector2(screenwidth2, screenheight2));
	
		GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
	
		GL.DrawElements(PrimitiveType.Triangles, 6, DrawElementsType.UnsignedInt, 0);
	}
	
	(float[], float[]) RenderShadowToTexture(int level, Vector3 sunpos) {
		//ok now time for the shadows
		GL.Viewport(0, 0, screenwidth2 * shadowq, screenheight2 * shadowq);
	
		shader3.Use();
	
		Matrix4 model = scale * rotation * translation;
		Matrix4 projection2 = new Matrix4();
		Matrix4 view = new Matrix4();
		//Vector3 a = Vector3.Normalize(forward) * 750 + camerac.position;
		//Vector3 a2 = Vector3.Normalize(forward) * 150 + camerac.position;
		Vector3 a = camerac.position;
		Vector3 a2 = camerac.position;
		if(level == 1) {
			projection2 = Matrix4.CreateOrthographic(1500, 1500 , 0.1f, 2000);
			view = Matrix4.LookAt(a + Vector3.Normalize(sunpos * new Vector3(1, -1, 1)) * 1000, a, new Vector3(0, 1, 0));
		}
		if(level == 2) {
			projection2 = Matrix4.CreateOrthographic(300, 300, 0.1f, 2000);
			view = Matrix4.LookAt(a2 + Vector3.Normalize(sunpos * new Vector3(1, -1, 1)) * 1000, a2, new Vector3(0, 1, 0));
		}
		//Matrix4 view2 = Matrix4.LookAt(camerac.position + GetSunPosFake() * 100, camerac.position, new Vector3(0, 1, 0));
	
		float[] viewf2 = Matrix4ToFloatArr(view);
		float[] projectionf2 = Matrix4ToFloatArr(projection2);
		float[] modelf = Matrix4ToFloatArr(model);
	
		GL.BindVertexArray(VertexArrayObject);
		GL.BindBuffer(BufferTarget.ElementArrayBuffer, ElementBufferObject);
		GL.BindBuffer(BufferTarget.ArrayBuffer, VertexBufferObject);
	
		GL.Enable(EnableCap.DepthTest);
	
		if(level == 1) {
			//GL.BindTexture(TextureTarget.Texture2D, shadowtexture);
			GL.BindFramebuffer(FramebufferTarget.Framebuffer, fbo3);
		}
		if(level == 2) {
			//GL.BindTexture(TextureTarget.Texture2D, shadowtexture2);
			GL.BindFramebuffer(FramebufferTarget.Framebuffer, fbo4);
		}
	
		GL.Clear(ClearBufferMask.DepthBufferBit);
	
		int location = GL.GetUniformLocation(shader3.Handle, "model");
		GL.UniformMatrix4(location, 1, 0, modelf);
		int location2 = GL.GetUniformLocation(shader3.Handle, "view");
		GL.UniformMatrix4(location2, 1, 0, viewf2);
		int location3 = GL.GetUniformLocation(shader3.Handle, "projection");
		GL.UniformMatrix4(location3, 1, 0, projectionf2);
	
		GL.DrawElements(PrimitiveType.Triangles, indices.Length, DrawElementsType.UnsignedInt, 0);
	
		return (viewf2, projectionf2); 
	}
	
	
	Vector3 GetSunPosFake() { //kind of wanted lore accurate, but also don't want to use chatgpt code so this will suffice //got it anyways //uhh good enough this is not something i understan
		float time = timeglobal.ElapsedMilliseconds / 10000f;
		int day = 150; float hours = 20f; hours = (time) % 24; float fractday = hours / 24f;
		float latitude = 59.437f * MathF.PI / 180; float longitude = 24.475f * MathF.PI / 180;
		float n = -1.5f + day + fractday;
		float epsilon = (23.440f - 0.0000004f * n) * MathF.PI / 180;
		float g = 357.528f + 0.9856003f * n * MathF.PI / 180;
		float lambda = ((280.466f + 0.9856474f * n) + 1.915f * MathF.Sin(g) + 0.02f * MathF.Sin(2f * g)) * MathF.PI / 180;
		float latitude2 = MathF.Asin(MathF.Sin(epsilon) * MathF.Sin(lambda)) * (180f / MathF.PI);
		float longitude2 = -15f * (hours - 12f) * MathF.PI / 180;
		return new Vector3(MathF.Cos(latitude2) * MathF.Sin(longitude2 - longitude), MathF.Cos(latitude) * MathF.Sin(latitude2) - MathF.Sin(latitude) * MathF.Cos(latitude2) * MathF.Cos(longitude2 - longitude), MathF.Sin(latitude) * MathF.Sin(latitude2) + MathF.Cos(latitude) * MathF.Cos(latitude2) * MathF.Cos(longitude2 - longitude));
		float pitch = -MathF.Abs(MathF.Sin(timeglobal.ElapsedMilliseconds / 10000f)); float yaw = -MathF.Abs(MathF.Sin(timeglobal.ElapsedMilliseconds / 10000f)) + 0.2f;
		//pitch = -33.97f; yaw = 210.06f; //pitch negative lol
		return new Vector3(MathF.Cos(pitch) * MathF.Sin(yaw), MathF.Sin(pitch), MathF.Cos(pitch) * MathF.Cos(yaw));
	}
	
	protected override void OnFramebufferResize(FramebufferResizeEventArgs e) {
		base.OnFramebufferResize(e);
	
		GL.Viewport(0, 0, e.Width, e.Height);
	
		screenheight = e.Height;
		screenwidth = e.Width;
	}
	
	protected override void OnUnload() {
		base.OnUnload();
	
		shader.Dispose();
	}
	}
	
	public class Shader {
	public int Handle;
	public Shader(string vertexPath, string fragmentPath) {
		string VertexShaderSource = File.ReadAllText(vertexPath);
	
		string FragmentShaderSource = File.ReadAllText(fragmentPath);
	
		//create
		int VertexShader = GL.CreateShader(ShaderType.VertexShader);
		GL.ShaderSource(VertexShader, VertexShaderSource);
	
		int FragmentShader = GL.CreateShader(ShaderType.FragmentShader);
		GL.ShaderSource(FragmentShader, FragmentShaderSource);
	
		//compile
		GL.CompileShader(VertexShader);
	
		GL.GetShader(VertexShader, ShaderParameter.CompileStatus, out int success);
		if(success == 0) {
			string infoLog = GL.GetShaderInfoLog(VertexShader);
			Console.WriteLine(infoLog);
			Console.ReadLine();
		}
	
		GL.CompileShader(FragmentShader);
	
		GL.GetShader(FragmentShader, ShaderParameter.CompileStatus, out success);
		if(success == 0) {
			string infoLog = GL.GetShaderInfoLog(FragmentShader);
			Console.WriteLine(infoLog);
			Console.ReadLine();
		}
	
		//link
		Handle = GL.CreateProgram();
	
		GL.AttachShader(Handle, VertexShader);
		GL.AttachShader(Handle, FragmentShader);
	
		GL.LinkProgram(Handle);
	
		GL.GetProgram(Handle, GetProgramParameterName.LinkStatus, out success);
		if(success == 0) {
			string infoLog = GL.GetProgramInfoLog(Handle);
			Console.WriteLine(infoLog);
			Console.ReadLine();
		}
	
		//delete shaders after being linked
		GL.DetachShader(Handle, VertexShader);
		GL.DetachShader(Handle, FragmentShader);
		GL.DeleteShader(FragmentShader);
		GL.DeleteShader(VertexShader);
	}
	public void Use() {
		GL.UseProgram(Handle);
	}
	
	//delete things ig
	private bool disposedValue = 0;
	
	protected virtual void Dispose(bool disposing) {
		if(!disposedValue) {
			GL.DeleteProgram(Handle);
	
			disposedValue = 1;
		}
	}
	
	~Shader() {
		if(disposedValue == 0) {
			Console.WriteLine("GPU Resource leak! Did you forget to call Dispose()?");
		}
	}
	
	
	public void Dispose() {
		Dispose(1);
		GC.SuppressFinalize(this);
	}
