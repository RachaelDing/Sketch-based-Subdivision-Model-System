/*
 * Catmull.h
 *
 *  Created on: Dec 4, 2018
 *      Author: xi.wang1
 */
#ifndef SRC_CATMULL_H_
#define SRC_CATMULL_H_

#ifdef SINGLE
#define REAL float
#else /* not SINGLE */
#define REAL double
#endif /* not SINGLE */

#include "Camera.h"
#include "Program.h"
#include "MeshTopology.h"
using namespace std;
using namespace glm;

struct Face;
struct Edge;
struct Mesh;
struct Vertex;

struct Face{
	//vector<Edge> ne;
	vector<int> V;
	vec3 fv;	//face vertex
	int fv_idx;
	vec3 norm;
	vector<int>ne;
};

struct Edge{
	int va, vb;
	int fl, fr;
	vec3 ev;	// edge vertices
	int ev_idx;
	int boundary = 0;
	int ep;
};

struct Vertex{
	vec3 pos;
	//vector<Edge> ne;	// adjacent edge
	//vector<Face> nf;  // adjacent face
	vec3 norm;
	vector<int> ne;
	vector<int> nf;
};

struct Mesh{
	vector<Face> F;
	vector<Vertex> V;
	vector<Edge> E;
};

extern Camera cam;


void Catmull(Mesh *mesh);
void init_cube(Mesh *cube_mesh);
void drawRawMesh(Mesh *mesh, Geometry *obj);
void drawMesh(Mesh *mesh, Geometry *obj, int face_flg);
vec3 shade_color(vec3 v_pos, vec3 norm);
void debug_p(vec3 a);
void make_basic_mesh(Mesh *mesh, MeshTopology *m_t);

#endif /* SRC_CATMULL_H_ */
