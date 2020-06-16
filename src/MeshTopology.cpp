/*
 * MeshTopology.cpp
 *
 *  Created on: Dec 4, 2018
 *      Author: yichen.ding
 */

#include "MeshTopology.h"
#define MIN_DIST 0.00001

using namespace glm;
using namespace std;

//constructor
MeshTopology::MeshTopology() {
	has_branching_face = false;
	s_face_num = 0;
}

//check if point p_i is in the triangle c_t
bool point_not_in_tri(MeshTopology::CDT_Triangle c_t, int p_i) {
	if (p_i != c_t.v[0] && p_i != c_t.v[1] && p_i != c_t.v[2]) {
		return true;
	} else {
		return false;
	}
}

//generate a quad with two adjacent triangle cdt_tri_vec[curri] and cdt_tri_vec[ne_i]
int MeshTopology::generate_quad_with_two_t(int curr_i, int ne_i) {
	MeshTopology::S_Face s_f;
	s_f.quad_v.clear();
	for (int i = 0; i < 4; i++) {
		s_f.quad_v.push_back(-1);
	}

	int v4;
	int v4_i_in_neigh = 0;
	int next_v_i;
	//find the vertex v4 which is in the triangle curr_i, but not in triangle ne_i
	if (point_not_in_tri(cdt_tri_vec[curr_i], cdt_tri_vec[ne_i].v[0])) {
		v4 = cdt_tri_vec[ne_i].v[0];
		v4_i_in_neigh = 0;
	} else if (point_not_in_tri(cdt_tri_vec[curr_i], cdt_tri_vec[ne_i].v[1])) {
		v4 = cdt_tri_vec[ne_i].v[1];
		v4_i_in_neigh = 1;
	} else {
		v4 = cdt_tri_vec[ne_i].v[2];
		v4_i_in_neigh = 2;
	}

	//add the four vertices into the sleeve face s_f in counterclockwise order
	s_f.quad_v[0] = v4;
	s_f.quad_v[1] = cdt_tri_vec[ne_i].v[(v4_i_in_neigh + 1) % 3];
	//find the vertex which in the triangle ne_i, and also the next vertex from v4 in the triangle curr_i in counterclockwise order
	for (int i = 0; i < 3; i++) {
		if (s_f.quad_v[1] == cdt_tri_vec[curr_i].v[i]) {
			next_v_i = (i + 1) % 3;
			break;
		}
	}
	s_f.quad_v[2] = cdt_tri_vec[curr_i].v[next_v_i];
	s_f.quad_v[3] = cdt_tri_vec[curr_i].v[(next_v_i + 1) % 3];
	//add the temperary sleeve face s_f to the sleeve face vector
	s_face_vec.push_back(s_f);
	s_face_num++;
	return 0;
}

//generate a quad with one triangle cdt_tri_vec[curri]
int MeshTopology::generate_quad_with_one_t(int last_i, int curr_i) {
	MeshTopology::S_Face s_f;
	s_f.quad_v.clear();
	for (int i = 0; i < 4; i++) {
		s_f.quad_v.push_back(0);
	}
	MeshTopology::Vertices new_v;
	MeshTopology::Original_Edges o_e;
	int v4;
	int v4_i;
	int v_last;
	int v_last_i;
	double x_new;
	double y_new;
	double z_new;
	//find the vertex v4 which is in the triangle curr_i, but not in triangle ne_i
	if (point_not_in_tri(cdt_tri_vec[last_i], cdt_tri_vec[curr_i].v[0])) {
		v4 = cdt_tri_vec[curr_i].v[0];
		v4_i = 0;
	} else if (point_not_in_tri(cdt_tri_vec[last_i],
			cdt_tri_vec[curr_i].v[1])) {
		v4 = cdt_tri_vec[curr_i].v[1];
		v4_i = 1;
	} else {
		v4 = cdt_tri_vec[curr_i].v[2];
		v4_i = 2;
	}

	//add a new vertex whose position is in the middle of one edge connected to v4 in triangle curr_i
	x_new = (v_vec[cdt_tri_vec[curr_i].v[v4_i]].v_pos.x
			+ v_vec[cdt_tri_vec[curr_i].v[(v4_i + 1) % 3]].v_pos.x)
			/ (double) 2.0;
	y_new = (v_vec[cdt_tri_vec[curr_i].v[v4_i]].v_pos.y
			+ v_vec[cdt_tri_vec[curr_i].v[(v4_i + 1) % 3]].v_pos.y)
			/ (double) 2.0;
	z_new = (v_vec[cdt_tri_vec[curr_i].v[v4_i]].v_pos.z
			+ v_vec[cdt_tri_vec[curr_i].v[(v4_i + 1) % 3]].v_pos.z)
			/ (double) 2.0;
	new_v.v_pos = vec3(x_new, y_new, z_new);
	new_v.index = v_vec.size();
	v_vec.push_back(new_v);
	cout << "Add a new vertex: x=" << x_new << "    y=" << y_new << "    z="
			<< z_new << endl;

	//move v4 to the middle of the other edge connected to v4 in triangle curr_i
	x_new = (v_vec[cdt_tri_vec[curr_i].v[v4_i]].v_pos.x
			+ v_vec[cdt_tri_vec[curr_i].v[(v4_i + 2) % 3]].v_pos.x)
			/ (double) 2.0;
	y_new = (v_vec[cdt_tri_vec[curr_i].v[v4_i]].v_pos.y
			+ v_vec[cdt_tri_vec[curr_i].v[(v4_i + 2) % 3]].v_pos.y)
			/ (double) 2.0;
	z_new = (v_vec[cdt_tri_vec[curr_i].v[v4_i]].v_pos.z
			+ v_vec[cdt_tri_vec[curr_i].v[(v4_i + 2) % 3]].v_pos.z)
			/ (double) 2.0;
	v_vec[cdt_tri_vec[curr_i].v[v4_i]].v_pos = vec3(x_new, y_new, z_new);

	//add the four vertices into the sleeve face s_f in counterclockwise order
	s_f.quad_v[0] = v4;
	s_f.quad_v[1] = new_v.index;
	s_f.quad_v[2] = cdt_tri_vec[curr_i].v[(v4_i + 1) % 3];
	s_f.quad_v[3] = cdt_tri_vec[curr_i].v[(v4_i + 2) % 3];

	//add new edges which are boundaries
	o_e.v[0] = s_f.quad_v[0];
	o_e.v[1] = s_f.quad_v[1];
	o_e.boundary = 1;
	e_vec.push_back(o_e);

	o_e.v[0] = s_f.quad_v[1];
	o_e.v[1] = s_f.quad_v[2];
	o_e.boundary = 1;
	e_vec.push_back(o_e);

	//add the temperary sleeve face s_f to the sleeve face vector
	s_face_vec.push_back(s_f);
	s_face_num++;
	return 0;
}

//decompose sleeves from a branch until reaching another branch or terminal
int MeshTopology::decompose_sleeve(int in_i, int in_last_i) {
	int curr_i = in_i;
	int last_i = in_last_i;
	int ne_i = -1;
	int new_ne_i = -1;
	bool reach_t_or_b = false;
	//loop until reach another branch or terminal
	while (!reach_t_or_b) {
		//check the type of the current triangle. If it is branching, stop looping. If it is terminal, decompose itself
		if (cdt_tri_vec[curr_i].type == BRANCHING) {
			reach_t_or_b = true;
			break;
		} else if (cdt_tri_vec[curr_i].type == TERMINAL) {
			generate_quad_with_one_t(last_i, curr_i);
			cdt_tri_vec[curr_i].decomposed = true;
			reach_t_or_b = true;
			break;
		}

		//if the current triangle is sleeve. find its next neighbor which has not been decomposed
		for (int j = 0; j < 3; j++) {
			ne_i = cdt_tri_vec[curr_i].neighbour[j];
			if (ne_i > -1 && ne_i != last_i) {
				//if the next neighbor is sleeve or terminal, continuing decomposing
				if (cdt_tri_vec[ne_i].type == SLEEVE
						|| cdt_tri_vec[ne_i].type == TERMINAL) {
					generate_quad_with_two_t(curr_i, ne_i);
					//updates states
					cdt_tri_vec[curr_i].decomposed = true;
					cdt_tri_vec[ne_i].decomposed = true;
					for (int k = 0; k < 3; k++) {
						new_ne_i = cdt_tri_vec[ne_i].neighbour[k];
						if (new_ne_i > -1 && new_ne_i != curr_i) {
							last_i = ne_i;
							curr_i = new_ne_i;
							ne_i = -1;
							new_ne_i = -1;
							break;
						}
					}
					if (cdt_tri_vec[ne_i].type == TERMINAL) {
						reach_t_or_b = true;
					}
					break;
					//if the next neighbor is branching, change the current triangle type to branching, so that it would be decomposed with branching triangles together
				} else if (cdt_tri_vec[ne_i].type == BRANCHING) {
					//cdt_tri_vec[curr_i].decomposed = false;
					cdt_tri_vec[curr_i].type = BRANCHING;
					Branching_Triangle b_t;
					b_t.tri_index = curr_i;
					b_t.neighbour[0] = cdt_tri_vec[curr_i].neighbour[0];
					b_t.neighbour[1] = cdt_tri_vec[curr_i].neighbour[1];
					b_t.neighbour[2] = cdt_tri_vec[curr_i].neighbour[2];
					br_tri_vec.push_back(b_t);
					reach_t_or_b = true;
					break;
				}
			}
		}

	}
	return 0;
}

//decompose branching triangles to n sided faces
int MeshTopology::make_n_sided_branch(int in_curr_i, int in_last_i) {
	int curr_i = in_curr_i;
	int last_i = in_last_i;
	int ne_i = -1;
	bool reach_end = false;
	MeshTopology::B_Face f_copy;
	while (!reach_end) {
		int v_out;
		int v_out_in_curr = 0;
		int v_first = 0;
		int v_origin_num = b_face_vec[b_face_vec.size() - 1].num_of_edges;

		//copy the last face in b_face_vec(which is the current face being decomposed)
		f_copy.v.clear();
		f_copy.num_of_edges = v_origin_num;
		for (int j = 0; j < v_origin_num; j++) {
			f_copy.v.push_back(b_face_vec[b_face_vec.size() - 1].v[j]);
		}

		//find the vertex v4 which is in the triangle curr_i, but not in current face(also not in the last triangle)
		if (point_not_in_tri(cdt_tri_vec[last_i], cdt_tri_vec[curr_i].v[0])) {
			v_out = cdt_tri_vec[curr_i].v[0];
			v_out_in_curr = 0;
		} else if (point_not_in_tri(cdt_tri_vec[last_i],
				cdt_tri_vec[curr_i].v[1])) {
			v_out = cdt_tri_vec[curr_i].v[1];
			v_out_in_curr = 1;
		} else {
			v_out = cdt_tri_vec[curr_i].v[2];
			v_out_in_curr = 2;
		}

		//find the vertex which in the current face, and also is the next vertex from v_out in the triangle curr_i in counterclockwise order
		for (int j = 0; j < v_origin_num; j++) {
			if (cdt_tri_vec[curr_i].v[(v_out_in_curr + 1) % 3]
					== b_face_vec[b_face_vec.size() - 1].v[j]) {
				v_first = j;
				break;
			}
		}

		//reorder vertices in the current face, such that the first vertex is the vertex we found above, and push the v_out into current face
		for (int j = 0; j < v_origin_num; j++) {
			b_face_vec[b_face_vec.size() - 1].v[j] = f_copy.v[(j + v_first
					+ v_origin_num) % v_origin_num];
			//cout << "debuging :" << (j - v_first + v_origin_num) % v_origin_num<< endl;
		}
		b_face_vec[b_face_vec.size() - 1].v.push_back(v_out);
		b_face_vec[b_face_vec.size() - 1].num_of_edges++;

		//updata edges
		for (int j = 0; j < e_vec.size(); j++) {
			if ((e_vec[j].v[0] == cdt_tri_vec[last_i].v[v_first]
					&& e_vec[j].v[1]
							== cdt_tri_vec[curr_i].v[(v_out_in_curr + 2) % 3])
					|| (e_vec[j].v[1] == cdt_tri_vec[last_i].v[v_first]
							&& e_vec[j].v[0]
									== cdt_tri_vec[curr_i].v[(v_out_in_curr + 2)
											% 3])) {
				e_vec.erase(e_vec.begin() + j);
				break;
			}
		}

		//update states
		cdt_tri_vec[curr_i].decomposed = true;
		bool find_neigh = false;
		for (int k = 0; k < 3; k++) {
			ne_i = cdt_tri_vec[curr_i].neighbour[k];
			if (ne_i > -1 && ne_i != last_i
					&& cdt_tri_vec[ne_i].type == BRANCHING
					&& cdt_tri_vec[ne_i].decomposed == false) {
				last_i = curr_i;
				curr_i = ne_i;
				ne_i = -1;
				find_neigh = true;
				break;
			}
		}
		if (!find_neigh) {
			reach_end = true;
		}
	}
	return 0;
}

void MeshTopology::build_2D_mesh() {
	//if there is at least an branching face, decompose from the sleeve faces near the branching faces
	if (has_branching_face) {
		int neigh_i = -2;
		printf("bbbbbb_tri_vec size: %d\n", br_tri_vec.size());
		//int branch_s = br_tri_vec.size();
		//traverse all branching faces, decompose from each neighbor of them
		for (int i = 0; i < br_tri_vec.size(); i++) {
			for (int j = 0; j < 3; j++) {
				neigh_i = br_tri_vec[i].neighbour[j];
				if (neigh_i > -1) {
					if ((cdt_tri_vec[neigh_i].type == SLEEVE)
							&& (cdt_tri_vec[neigh_i].decomposed == false)) {
						decompose_sleeve(neigh_i, br_tri_vec[i].tri_index);
					} else if ((cdt_tri_vec[neigh_i].type == TERMINAL)) {
						generate_quad_with_one_t(br_tri_vec[i].tri_index,
								neigh_i);
						cdt_tri_vec[neigh_i].decomposed = true;
					}
				}
			}

		}

		//decompose each branching face(including the sleeve face marked as branching face)
		for (int i = 0; i < cdt_tri_vec.size(); i++) {
			int b_neigh_counter = 0;
			neigh_i = -2;
			if (cdt_tri_vec[i].decomposed
					== false&& cdt_tri_vec[i].type == BRANCHING) {
				for (int j = 0; j < 3; j++) {
					if (cdt_tri_vec[i].neighbour[j] > -1) {
						if (cdt_tri_vec[cdt_tri_vec[i].neighbour[j]].type
								== BRANCHING
								&& cdt_tri_vec[cdt_tri_vec[i].neighbour[j]].decomposed
										== false) {
							b_neigh_counter++;
							neigh_i = j;
						}
					}
				}
				if (b_neigh_counter == 1) {
					MeshTopology::B_Face b_f;
					//cout << "bfffffffffffffff_f v size" << b_f.v.size() << endl;
					b_f.num_of_edges = 3;
					b_f.v.push_back(cdt_tri_vec[i].v[0]);
					b_f.v.push_back(cdt_tri_vec[i].v[1]);
					b_f.v.push_back(cdt_tri_vec[i].v[2]);
					b_face_vec.push_back(b_f);
					cdt_tri_vec[i].decomposed = true;
					make_n_sided_branch(cdt_tri_vec[i].neighbour[neigh_i], i);
					cout << "finish making an n sided branch face with " <<b_face_vec[b_face_vec.size()-1].v.size()<<" vertices. "<< endl;
				}
			}
		}

	}

	//deal with the special case that there are only sleeve faces and terminal faces
	for (int i = 0; i < cdt_tri_vec.size(); i++) {
		int neigh_i = -2;
		if (cdt_tri_vec[i].decomposed
				== false && cdt_tri_vec[i].type == TERMINAL) {
			for(int j = 0; j < 3;j++){
				neigh_i = cdt_tri_vec[i].neighbour[j];
				if(neigh_i>-1){
					decompose_sleeve(neigh_i,i);
				}
			}
		}

	}

	//store the single branching faces and special faces left as triangles
	for (int i = 0; i < cdt_tri_vec.size(); i++) {
		//if (cdt_tri_vec[i].decomposed == false&& cdt_tri_vec[i].type == BRANCHING) {
		if (cdt_tri_vec[i].decomposed == false){
			MeshTopology::B_Face b_f;
			b_f.num_of_edges = 3;
			b_f.v.push_back(cdt_tri_vec[i].v[0]);
			b_f.v.push_back(cdt_tri_vec[i].v[1]);
			b_f.v.push_back(cdt_tri_vec[i].v[2]);
			b_face_vec.push_back(b_f);
			cdt_tri_vec[i].decomposed = true;
			cout<<"add a single branching face or a special face as triangle"<<endl;
		}
	}

	printf("Finish building 2D mesh\n");
}

//call the triangle library, generating delauny triangles from coarse points c_p and store it into cdt_tri_vec
MeshTopology MeshTopology::build_triangles(vector<vec3> *c_p) {
	struct triangulateio in, out;
	MeshTopology m_t = MeshTopology();

	int c_p_size = c_p->size();
	in.numberofpoints = c_p_size;
	in.numberofpointattributes = 1;
	in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
	for (int i = 0; i < c_p_size; i++) {
		in.pointlist[i * 2] = c_p->at(i).x;
		in.pointlist[i * 2 + 1] = c_p->at(i).y;
	}

	in.pointattributelist = (REAL *) malloc(
			in.numberofpoints * in.numberofpointattributes * sizeof(REAL));
	for (int i = 0; i < c_p_size; i++) {
		in.pointattributelist[i] = in.pointlist[i * 2]
				+ in.pointlist[i * 2 + 1];
	}

	in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));
	for (int i = 0; i < c_p_size; i++) {
		in.pointmarkerlist[i] = 0;
	}

	in.numberofholes = 0;
	in.numberofregions = 0;

	in.numberofsegments = c_p_size;
	in.segmentlist = (int *) malloc(in.numberofsegments * 2 * sizeof(int));
	in.segmentlist[0] = 1;
	for (int i = 0; i < (in.numberofsegments - 1); i++) {
		in.segmentlist[i * 2 + 1] = i + 2;
		in.segmentlist[i * 2 + 2] = i + 2;
	}
	in.segmentlist[in.numberofsegments * 2 - 1] = 1;

	in.segmentmarkerlist = (int *) malloc(in.numberofsegments * sizeof(int));
	for (int i = 0; i < in.numberofsegments; i++) {
		in.segmentmarkerlist[i] = 0;
	}

	out.pointlist = (REAL *) NULL;
	out.pointattributelist = (REAL *) NULL;
	out.pointmarkerlist = (int *) NULL;
	out.trianglelist = (int *) NULL;

	out.triangleattributelist = (REAL *) NULL;
	out.neighborlist = (int *) NULL;

	out.segmentlist = (int *) NULL;

	out.segmentmarkerlist = (int *) NULL;
	out.edgelist = (int *) NULL;
	out.edgemarkerlist = (int *) NULL;

	triangulate("penD", &in, &out, (struct triangulateio *) NULL);

	for (int i = 0; i < out.numberofpoints; i++) {
		Vertices temp_v;
		temp_v.v_pos = vec3(out.pointlist[i * 2], out.pointlist[i * 2 + 1],
				0.0);
		temp_v.index = i;
		m_t.v_vec.push_back(temp_v);
	}

	for (int k = 0; k < out.numberofedges; k++) {
		Original_Edges o_e;
		o_e.v[0] = out.edgelist[k * 2] - 1;
		o_e.v[1] = out.edgelist[k * 2 + 1] - 1;
		o_e.boundary = out.edgemarkerlist[k];
		m_t.e_vec.push_back(o_e);
	}

	for (int i = 0; i < out.numberoftriangles; i++) {
		MeshTopology::CDT_Triangle cdt_y;
		cdt_y.v[0] = out.trianglelist[i * 3] - 1;
		cdt_y.v[1] = out.trianglelist[i * 3 + 1] - 1;
		cdt_y.v[2] = out.trianglelist[i * 3 + 2] - 1;

		cdt_y.neighbour[0] = out.neighborlist[i * 3] - 1;
		cdt_y.neighbour[1] = out.neighborlist[i * 3 + 1] - 1;
		cdt_y.neighbour[2] = out.neighborlist[i * 3 + 2] - 1;
		cdt_y.index = i;
		int neigh_counter = 0;
		for (int j = 0; j < 3; j++) {
			if (cdt_y.neighbour[j] > -1) {
				neigh_counter++;
			}
		}
		if (neigh_counter == 3) {
			cdt_y.type = BRANCHING;
			m_t.has_branching_face = true;
		} else if (neigh_counter == 2) {
			cdt_y.type = SLEEVE;
		} else {
			cdt_y.type = TERMINAL;
		}
		m_t.cdt_tri_vec.push_back(cdt_y);

		if (cdt_y.type == BRANCHING) {
			Branching_Triangle b_t;
			b_t.neighbour[0] = out.neighborlist[i * 3] - 1;
			b_t.neighbour[1] = out.neighborlist[i * 3 + 1] - 1;
			b_t.neighbour[2] = out.neighborlist[i * 3 + 2] - 1;
			b_t.tri_index = i;
			m_t.br_tri_vec.push_back(b_t);
		}
	}

	return m_t;
}

//draw triangles(red for sleeve triangles, green for branching triangles, blue for branching triangles)
void MeshTopology::show_triangles(Geometry *d_t) {
	//Geometry d_t = Geometry();
	int v_i;
	for (int i = 0; i < cdt_tri_vec.size(); i++) {
		v_i = cdt_tri_vec[i].v[0];
		d_t->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = cdt_tri_vec[i].v[1];
		d_t->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = cdt_tri_vec[i].v[2];
		d_t->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));

		if (cdt_tri_vec[i].type == TERMINAL) {
			d_t->colours.push_back(glm::vec3(0.f, 0.f, 1.f));
			d_t->colours.push_back(glm::vec3(0.f, 0.f, 1.f));
			d_t->colours.push_back(glm::vec3(0.f, 0.f, 1.f));
		} else if (cdt_tri_vec[i].type == SLEEVE) {
			d_t->colours.push_back(glm::vec3(1.f, 0.f, 0.f));
			d_t->colours.push_back(glm::vec3(1.f, 0.f, 0.f));
			d_t->colours.push_back(glm::vec3(1.f, 0.f, 0.f));
		} else if (cdt_tri_vec[i].type == BRANCHING) {
			d_t->colours.push_back(glm::vec3(0.f, 1.f, 0.f));
			d_t->colours.push_back(glm::vec3(0.f, 1.f, 0.f));
			d_t->colours.push_back(glm::vec3(0.f, 1.f, 0.f));
		}

	}
	d_t->drawMode = GL_TRIANGLES;
}

//draw lines for the triangles
void MeshTopology::show_triangle_lines(Geometry *d_t_lines) {
	int v_i;
	for (int i = 0; i < cdt_tri_vec.size(); i++) {
		v_i = cdt_tri_vec[i].v[0];
		d_t_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = cdt_tri_vec[i].v[1];
		d_t_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = cdt_tri_vec[i].v[1];
		d_t_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = cdt_tri_vec[i].v[2];
		d_t_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = cdt_tri_vec[i].v[2];
		d_t_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = cdt_tri_vec[i].v[0];
		d_t_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		d_t_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		d_t_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		d_t_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		d_t_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		d_t_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		d_t_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
	}
	d_t_lines->drawMode = GL_LINES;
}

//draw quads
void MeshTopology::show_quads(Geometry *s_f) {
	int v_i;
	for (int i = 0; i < s_face_vec.size(); i++) {
		v_i = s_face_vec[i].quad_v[0];
		s_f->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = s_face_vec[i].quad_v[1];
		//printf("VVVV_IIIIIIII222222222 %d\n",v_i);
		s_f->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = s_face_vec[i].quad_v[2];
		//printf("VVVV_IIIIIII3333333333333 %d\n",v_i);
		s_f->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = s_face_vec[i].quad_v[3];
		//printf("VVVV_IIIIIII44444444444 %d \n",v_i);
		s_f->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));

		s_f->colours.push_back(glm::vec3(1.f, 0.f, 0.f));
		s_f->colours.push_back(glm::vec3(1.f, 0.f, 0.f));
		s_f->colours.push_back(glm::vec3(1.f, 0.f, 0.f));
		s_f->colours.push_back(glm::vec3(1.f, 0.f, 0.f));

	}
	s_f->drawMode = GL_QUADS;
}

//draw edges for each quad
void MeshTopology::show_quad_lines(Geometry *s_f_lines) {
	int v_i;
	for (int i = 0; i < s_face_vec.size(); i++) {
		v_i = s_face_vec[i].quad_v[0];
		s_f_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = s_face_vec[i].quad_v[1];
		s_f_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = s_face_vec[i].quad_v[1];
		s_f_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = s_face_vec[i].quad_v[2];
		s_f_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = s_face_vec[i].quad_v[2];
		s_f_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = s_face_vec[i].quad_v[3];
		s_f_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = s_face_vec[i].quad_v[3];
		s_f_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		v_i = s_face_vec[i].quad_v[0];
		s_f_lines->verts.push_back(
				vec3(v_vec[v_i].v_pos.x, v_vec[v_i].v_pos.y,
						v_vec[v_i].v_pos.z));
		s_f_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		s_f_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		s_f_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		s_f_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		s_f_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		s_f_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		s_f_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));
		s_f_lines->colours.push_back(glm::vec3(0.f, 0.f, 0.f));

	}
	s_f_lines->drawMode = GL_LINES;
}

//draw edges for each n side face
void MeshTopology::show_n_side_face_lines(Geometry *b_f_lines) {
	int v_j;
	for (int i = 0; i < b_face_vec.size(); i++) {
		for (int j = 0; j < b_face_vec[i].v.size(); j++) {
			v_j = b_face_vec[i].v[j];
			//printf("VVVV_IIIIIII44444444444 %d \n",v_i);
			b_f_lines->verts.push_back(
					vec3(v_vec[v_j].v_pos.x, v_vec[v_j].v_pos.y,
							v_vec[v_j].v_pos.z));
			v_j = b_face_vec[i].v[(j + 1) % (b_face_vec[i].v.size())];
			b_f_lines->verts.push_back(
					vec3(v_vec[v_j].v_pos.x, v_vec[v_j].v_pos.y,
							v_vec[v_j].v_pos.z));
			b_f_lines->colours.push_back(glm::vec3(1.f, 1.f, 1.f));
			b_f_lines->colours.push_back(glm::vec3(1.f, 1.f, 1.f));
			//cout<<v_vec[v_j].v_pos.x<<endl;
		}
	}

	b_f_lines->drawMode = GL_LINES;
}
