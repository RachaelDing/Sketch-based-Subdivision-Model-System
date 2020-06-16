/*
 * MeshTopology.h
 *
 *  Created on: Dec 4, 2018
 *      Author: yichen.ding
 */
#pragma once

#ifdef SINGLE
#define REAL float
#else /* not SINGLE */
#define REAL double
#endif /* not SINGLE */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include <vector>

#include "Geometry.h"
#include "triangle.h"

using namespace glm;
using namespace std;

class MeshTopology {

public:
#define TERMINAL 1
#define SLEEVE 2
#define BRANCHING 3
	typedef struct Vertices {
		int index = 0;
		vec3 v_pos = vec3((double) 0.0, (double) 0.0, (double) 0.0);
	} Vertices;

	typedef struct Orignal_Edges {
		int v[2] = { -1, -1 };
		int boundary = 0;
	} Original_Edges;

	typedef struct CDT_Triangle {
		int v[3] = { -1, -1, -1 };
		int index = 0;
		int type = 0;
		int neighbour[3] = { -1, -1, -1 };
		bool decomposed = false;
		bool single_s = false;
	} CDT_Triangle;

	typedef struct Branching_Triangle {
		int tri_index = -1;
		int neighbour[3] = { -1, -1, -1 };
	} Brunching_Triangle;

	typedef struct B_Face {
		int num_of_edges = 0;
		vector<int> v;
	} B_Face;

	typedef struct S_Face {
		vector<int> quad_v;
	} S_Face;

	MeshTopology();

	vector<Vertices> v_vec;
	vector<Original_Edges> e_vec;
	vector<B_Face> b_face_vec;
	vector<S_Face> s_face_vec;
	vector<CDT_Triangle> cdt_tri_vec;
	vector<Branching_Triangle> br_tri_vec;
	bool has_branching_face;
	int s_face_num;

	static MeshTopology build_triangles(vector<vec3> *c_p);
	void show_triangles(Geometry *d_t);
	void show_triangle_lines(Geometry *d_t);
	void build_2D_mesh();
	int make_n_sided_branch(int in_curr_i, int in_last_i);
	int decompose_sleeve(int in_i, int in_last_i);
	int generate_quad_with_two_t(int curr_i, int ne_i);
	int generate_quad_with_one_t(int last_i, int curr_i);
	void show_quads(Geometry *s_f);
	void show_quad_lines(Geometry *s_f);
	void show_n_side_face_lines(Geometry *b_f_lines);
};
