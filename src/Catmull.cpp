/*
 * Catmull.c
 *
 *  Created on: Dec 4, 2018
 *      Author: xi.wang1
 */

#include "Catmull.h"
#define PI_F 3.14159f
using namespace std;
using namespace glm;

vec3 obj_color = vec3(0, 1, 1);
vec3 ambient = vec3(.7f, .7f, .7f);
vec3 light_pos = vec3(1, -1, 1);

Camera cam;

struct Face;
struct Edge;
struct Mesh;
struct Vertex;

void debug_p(vec3 a) {
	cout << a.x << "," << a.y << "," << a.z << endl;
}

vec3 avg_fv(Face f, Mesh *mesh) {
	vec3 avg = vec3(0, 0, 0);
	int i = 0;
	for (; i < f.V.size(); i++) {
		avg += (mesh->V).at(f.V.at(i)).pos;
	}
	if (i > 0)
		avg /= (float) i;
	return avg;
}

void update_ne(int v, Mesh *mesh) {
	mesh->V.at(v).ne.clear();
	for (int i = 0; i < mesh->E.size(); i++) {
		if (mesh->E.at(i).va == v) {
			mesh->V.at(v).ne.push_back(i);
		}
	}
}

void update_nf(int v, Mesh *mesh) {
	mesh->V.at(v).nf.clear();
	for (int i = 0; i < mesh->F.size(); i++) {
		for (int vv : mesh->F.at(i).V) {
			if (vv == v) {
				mesh->V.at(v).nf.push_back(i);
				break;
			}
		}
	}
}

void edge_of_face(Face *f, Mesh *mesh) {
	f->ne.clear();
	int size = f->V.size();
	for (int i = 0; i < size; i++) {	// counterclockwise
		for (int j = 0; j < mesh->E.size(); j++) {
			Edge e = mesh->E.at(j);
			if (e.va == f->V.at(i) && e.vb == f->V.at((i + 1) % size)) {
				f->ne.push_back(j);
			}
		}
	}
}

void update_fr_fl(Edge *e, Mesh *mesh) {
	int found = 0;
	for (int i = 0; i < mesh->F.size() && found != 2; i++) {

		//cout << "frfl i : " << i << endl;

		Face *f = &(mesh->F.at(i));
		for (int j = 0; j < f->ne.size(); j++) {
			int fe = f->ne.at(j);
			if (e->va == mesh->E.at(fe).va && e->vb == mesh->E.at(fe).vb) {
				e->fl = i;
				found++;
				break;
			} else if (e->va == mesh->E.at(fe).vb
					&& e->vb == mesh->E.at(fe).va) {
				e->fr = i;
				found++;
				break;
			}
		}
	}
}

void update_v_neighbor(Mesh *mesh) {
	for (int i = 0; i < mesh->V.size(); i++) {
		update_ne(i, mesh);
		update_nf(i, mesh);
	}
}

void get_face_normal(Mesh *mesh) {
	for (int i = 0; i < mesh->F.size(); i++) {
		Face *f = &(mesh->F.at(i));
		vec3 v1 = mesh->V.at(f->V.at(1)).pos - mesh->V.at(f->V.at(0)).pos;
		vec3 v2 = mesh->V.at(f->V.at(2)).pos - mesh->V.at(f->V.at(1)).pos;
		f->norm = normalize(cross(v1, v2));
	}
}

void get_avg_v_normal(Mesh *mesh) {
	for (int i = 0; i < mesh->V.size(); i++) {
		Vertex *v = &(mesh->V.at(i));
		vec3 norm = vec3(0, 0, 0);
		for (int j = 0; j < v->nf.size(); j++) {
			int f = v->nf.at(j);
			//cout << "i = " <<i <<": " << f << endl;
			norm += mesh->F.at(f).norm;
		}
		v->norm = normalize(norm / (float) v->nf.size());
	}
}

void Catmull(Mesh *mesh) {
	int vert_cnt = mesh->V.size();
	vector<Vertex> newvert;

	// calculate fv
	for (int i = 0; i < mesh->F.size(); i++) {
		mesh->F.at(i).fv = avg_fv(mesh->F.at(i), mesh);
		mesh->F.at(i).fv_idx = vert_cnt;

		Vertex vv;
		vv.pos = mesh->F.at(i).fv;
		//vv.index = vert_cnt;
		newvert.push_back(vv);
		vert_cnt++;
	}

	vector<Edge> done;
	// calculate ev
	for (int i = 0; i < mesh->E.size(); i++) {
		Edge *e = &(mesh->E.at(i));
		e->ev = 0.25f
				* ((mesh->V).at(e->va).pos + (mesh->V).at(e->vb).pos
						+ mesh->F.at(e->fl).fv + mesh->F.at(e->fr).fv);
		int flg = 0;
		for (Edge ep : done) {
			if (ep.va == e->vb && ep.vb == e->va) {
				flg = 1;
				e->ev_idx = ep.ev_idx;
				break;
			}
		}
		if (flg == 0) {
			e->ev_idx = vert_cnt;
			Vertex vv;
			vv.pos = e->ev;
			//vv.index = vert_cnt;
			newvert.push_back(vv);
			vert_cnt++;
		}
		done.push_back(*e);
	}

	// update v's neighbor
	//update_v_neighbor(mesh);
	// calculate vv
	for (int i = 0; i < mesh->V.size(); i++) {
		Vertex *v = &(mesh->V.at(i));
		vec3 avg_f = vec3(0, 0, 0);	// average fv of all the adjacent faces
		vec3 avg_e = vec3(0, 0, 0);	// average edge_mid of all the adjacent edges
		float n = (float) v->nf.size();
		for (int f : v->nf) {
			avg_f += mesh->F.at(f).fv;
		}
		avg_f /= n;
		for (int e : v->ne) {
			avg_e += 0.5f
					* (mesh->V.at(mesh->E.at(e).va).pos
							+ mesh->V.at(mesh->E.at(e).vb).pos);
		}
		avg_e /= n;
		v->pos = (avg_f + avg_e * 2.f + (n - 3.f) * v->pos) / (float) n;
	}

	// push the new vertices
	for (Vertex vnew : newvert) {
		mesh->V.push_back(vnew);
	}

	// find the edges of a faces
	for (int i = 0; i < mesh->F.size(); i++) {
		edge_of_face(&(mesh->F.at(i)), mesh);
	}

	// split the faces
	vector<Face> newface;
	vector<Edge> newedge;
	for (Face f : mesh->F) {
		int size = f.ne.size();
		for (int i = 0; i < size; i++) {
			Edge e;
			e = mesh->E.at(f.ne.at(i));
			Face newf;
			Edge newe;

			// circle 1
			newf.V.push_back(e.va);		//ld
			newf.V.push_back(e.ev_idx);	//rd
			newf.V.push_back(f.fv_idx);	//ru
			newf.V.push_back(mesh->E.at(f.ne.at((i + size - 1) % size)).ev_idx);//lu

			newe.va = e.va;
			newe.vb = e.ev_idx;

			newedge.push_back(newe);
			newf.ne.push_back(newedge.size() - 1);

			newe.va = e.ev_idx;
			newe.vb = f.fv_idx;
			newedge.push_back(newe);
			newf.ne.push_back(newedge.size() - 1);

			newe.va = f.fv_idx;
			newe.vb = mesh->E.at(f.ne.at((i + size - 1) % size)).ev_idx;
			newedge.push_back(newe);
			newf.ne.push_back(newedge.size() - 1);

			newe.va = mesh->E.at(f.ne.at((i + size - 1) % size)).ev_idx;
			newe.vb = e.va;
			newedge.push_back(newe);
			newf.ne.push_back(newedge.size() - 1);

			newface.push_back(newf);
		}
	}
	mesh->F.clear();
	for (Face f : newface) {
		mesh->F.push_back(f);
	}

	mesh->E.clear();
	for (Edge e : newedge) {
		mesh->E.push_back(e);
	}

	// update fr and fl
	for (int i = 0; i < mesh->E.size(); i++) {
		update_fr_fl(&(mesh->E.at(i)), mesh);
	}

	get_face_normal(mesh);
	update_v_neighbor(mesh);
	get_avg_v_normal(mesh);

}

vec3 shade_color(vec3 v_pos, vec3 norm) {
	vec3 c;
	vec3 to_light = normalize(light_pos - v_pos);
	vec3 to_eye = -normalize(cam.pos - v_pos);
	float diffuse = dot(to_light, norm);
	vec3 ref = 2.f * diffuse * norm - to_light;
	diffuse = diffuse > 0 ? diffuse : 0;

	float spec_ratio = dot(ref, to_eye);
	spec_ratio = spec_ratio > 0 ? spec_ratio : 0;
	spec_ratio = 0.7 * pow(spec_ratio, 2) * diffuse;

	diffuse = diffuse + 0.7 > 1 ? 1 : diffuse + 0.7;
	c = diffuse * obj_color + spec_ratio * vec3(1, 1, 1);

	return c;
}

void drawMesh(Mesh *mesh, Geometry *obj, int flg_face) {
	if (flg_face == 0) {
		for (Face f : mesh->F) {	// vertex normal
			if (f.V.size() > 3) {
				obj->verts.push_back((mesh->V).at(f.V.at(0)).pos);
				obj->verts.push_back((mesh->V).at(f.V.at(1)).pos);
				obj->verts.push_back((mesh->V).at(f.V.at(2)).pos);
				obj->verts.push_back((mesh->V).at(f.V.at(3)).pos);

				obj->colours.push_back(
						shade_color((mesh->V).at(f.V.at(0)).pos,
								(mesh->V).at(f.V.at(0)).norm));
				obj->colours.push_back(
						shade_color((mesh->V).at(f.V.at(1)).pos,
								(mesh->V).at(f.V.at(1)).norm));
				obj->colours.push_back(
						shade_color((mesh->V).at(f.V.at(2)).pos,
								(mesh->V).at(f.V.at(2)).norm));
				obj->colours.push_back(
						shade_color((mesh->V).at(f.V.at(2)).pos,
								(mesh->V).at(f.V.at(3)).norm));
			}
		}
		obj->drawMode = GL_QUADS;
	} else if (flg_face == 2) {	// face normal
		for (Face f : mesh->F) {
			if (f.V.size() > 3) {
				obj->verts.push_back((mesh->V).at(f.V.at(0)).pos);
				obj->verts.push_back((mesh->V).at(f.V.at(1)).pos);
				obj->verts.push_back((mesh->V).at(f.V.at(2)).pos);
				obj->verts.push_back((mesh->V).at(f.V.at(3)).pos);

				obj->colours.push_back(
						shade_color((mesh->V).at(f.V.at(0)).pos, f.norm));
				obj->colours.push_back(
						shade_color((mesh->V).at(f.V.at(1)).pos, f.norm));
				obj->colours.push_back(
						shade_color((mesh->V).at(f.V.at(2)).pos, f.norm));
				obj->colours.push_back(
						shade_color((mesh->V).at(f.V.at(2)).pos, f.norm));
			}
		}
		obj->drawMode = GL_QUADS;
	} else if(flg_face == 1){
		for (Edge e : mesh->E) {
			obj->verts.push_back((mesh->V).at(e.va).pos);
			obj->colours.push_back(vec3(1, 0, 1));
			obj->verts.push_back((mesh->V).at(e.vb).pos);
			obj->colours.push_back(vec3(1, 0, 1));
		}
		obj->drawMode = GL_LINES;
	}
}

void drawRawMesh(Mesh *mesh, Geometry *obj) {
	for (Edge e : mesh->E) {
		obj->verts.push_back((mesh->V).at(e.va).pos);
		obj->colours.push_back(vec3(1, 0, 1));
		obj->verts.push_back((mesh->V).at(e.vb).pos);
		obj->colours.push_back(vec3(1, 0, 1));
	}

	obj->drawMode = GL_LINES;
}

void make_basic_mesh(Mesh *mesh, MeshTopology *m_t) {

	// vertex.pos
	for (int i = 0; i < m_t->v_vec.size(); i++) {
		Vertex v;
		v.pos = vec3(m_t->v_vec[i].v_pos.x, m_t->v_vec[i].v_pos.y, 0.1f);
		mesh->V.push_back(v);
	}

	// the reflection verts
	for (int i = 0; i < m_t->v_vec.size(); i++) {
		Vertex v;
		v.pos = vec3(m_t->v_vec[i].v_pos.x, m_t->v_vec[i].v_pos.y, -.1f);
		mesh->V.push_back(v);
	}

	int num_verts = m_t->v_vec.size();

	// make face, edge(counterclockwise)
	for (int i = 0; i < m_t->b_face_vec.size(); i++) { // for each n side face

		Face f;
		for (int j = 0; j < m_t->b_face_vec[i].num_of_edges; j++) {
			int v_temp = m_t->b_face_vec[i].v[j];
			f.V.push_back(v_temp);
		}

		mesh->F.push_back(f);

		Edge e;

		for (int j = 0; j < (m_t->b_face_vec[i].num_of_edges - 1); j++) {
			e.fl = i;
			int v_temp = m_t->b_face_vec[i].v[j];
			e.va = v_temp;
			v_temp = m_t->b_face_vec[i].v[(j + 1)
					% (m_t->b_face_vec[i].num_of_edges)];
			e.vb = v_temp;
			mesh->E.push_back(e);
		}
		e.fl = i;
		int v_temp = m_t->b_face_vec[i].v[m_t->b_face_vec[i].num_of_edges - 1];
		e.va = v_temp;
		v_temp = m_t->b_face_vec[i].v[0];
		e.vb = v_temp;
		mesh->E.push_back(e);
	}

	int tmp_face_size = mesh->F.size();

	for (int i = 0; i < m_t->s_face_vec.size(); i++) {	// for each quad
		Face f;
		int va = m_t->s_face_vec[i].quad_v[0];
		int vb = m_t->s_face_vec[i].quad_v[1];
		int vc = m_t->s_face_vec[i].quad_v[2];
		int vd = m_t->s_face_vec[i].quad_v[3];

		f.V.push_back(va);
		f.V.push_back(vb);
		f.V.push_back(vc);
		f.V.push_back(vd);
		mesh->F.push_back(f);

		Edge e;
		e.fl = i + tmp_face_size;
		e.va = va;
		e.vb = vb;
		mesh->E.push_back(e);

		e.fl = i + tmp_face_size;
		e.va = vb;
		e.vb = vc;
		mesh->E.push_back(e);

		e.fl = i + tmp_face_size;
		e.va = vc;
		e.vb = vd;
		mesh->E.push_back(e);

		e.fl = i + tmp_face_size;
		e.va = vd;
		e.vb = va;
		mesh->E.push_back(e);
	}

	int fsize = mesh->F.size() - 1;
	int edgesize = mesh->E.size();

	// reflection
	for (int i = 0; i < m_t->b_face_vec.size(); i++) {	// for each n side face
		Face f;
		for (int j = m_t->b_face_vec[i].num_of_edges - 1; j >= 0; j--) {
			int v_temp = m_t->b_face_vec[i].v[j];
			v_temp += num_verts;
			f.V.push_back(v_temp);
		}
		mesh->F.push_back(f);

		Edge e;
		// the reflection edge
		for (int j = m_t->b_face_vec[i].num_of_edges - 1; j > 0; j--) {
			e.fl = mesh->F.size() - 1;
			int v_temp = m_t->b_face_vec[i].v[j];
			v_temp += num_verts;
			e.va = v_temp;
			v_temp = m_t->b_face_vec[i].v[j - 1];
			v_temp += num_verts;
			e.vb = v_temp;
			mesh->E.push_back(e);
			//cout << "i is:" << i << "       j is:" << j << endl;
		}

		e.fl = mesh->F.size() - 1;
		int v_temp = m_t->b_face_vec[i].v[0];
		v_temp += num_verts;
		e.va = v_temp;
		v_temp = m_t->b_face_vec[i].v[m_t->b_face_vec[i].num_of_edges - 1];
		v_temp += num_verts;
		e.vb = v_temp;
		mesh->E.push_back(e);
	}

	// reflection
	for (int i = 0; i < m_t->s_face_vec.size(); i++) {	// for each quad
		Face f;
		int va = m_t->s_face_vec[i].quad_v[0];
		int vb = m_t->s_face_vec[i].quad_v[1];
		int vc = m_t->s_face_vec[i].quad_v[2];
		int vd = m_t->s_face_vec[i].quad_v[3];

		Edge e;

		// reflection verts
		va += num_verts;
		vb += num_verts;
		vc += num_verts;
		vd += num_verts;

		// the reflection face
		f.V.push_back(vd);
		f.V.push_back(vc);
		f.V.push_back(vb);
		f.V.push_back(va);
		mesh->F.push_back(f);
		// the reflection edge

		e.fl = mesh->F.size() - 1;
		e.va = vd;
		e.vb = vc;
		mesh->E.push_back(e);

		e.fl = mesh->F.size() - 1;
		e.va = vc;
		e.vb = vb;
		mesh->E.push_back(e);

		e.fl = mesh->F.size() - 1;
		e.va = vb;
		e.vb = va;
		mesh->E.push_back(e);

		e.fl = mesh->F.size() - 1;
		e.va = va;
		e.vb = vd;
		mesh->E.push_back(e);
	}

	cout << "numberof mesh edge: " << mesh->E.size() << endl;
	// update edge boundary marker
	for (int i = 0; i < mesh->E.size(); i++) {
		Edge e = mesh->E.at(i);
		mesh->E.at(i).boundary = 0;
		for (int j = 0; j < m_t->e_vec.size(); j++) {
			if (e.va == m_t->e_vec[j].v[0] && e.vb == m_t->e_vec[j].v[1]
					|| e.vb == m_t->e_vec[j].v[0] && e.va == m_t->e_vec[j].v[1]) {
				mesh->E.at(i).boundary = m_t->e_vec[j].boundary;
				// reflection
				//mesh->E.at(i+mesh->E.size()/2).boundary = mesh->E.at(i).boundary;
				break;
			}
		}
	}

	// add boundary faces
	for (int i = 0; i < edgesize; i++) {
		if (mesh->E.at(i).boundary == 0) {
			continue;
		}

		int va = mesh->E.at(i).va;
		int vb = mesh->E.at(i).vb;

		Face newf;
		newf.V.push_back(va);
		newf.V.push_back(va + num_verts);
		newf.V.push_back(vb + num_verts);
		newf.V.push_back(vb);
		mesh->F.push_back(newf);

		int fl = mesh->F.size() - 1;

		Edge newedge;
		newedge.va = va;
		newedge.vb = va + num_verts;
		newedge.fl = fl;
		mesh->E.push_back(newedge);

		newedge.va = va + num_verts;
		newedge.vb = vb + num_verts;
		newedge.fl = fl;
		mesh->E.push_back(newedge);

		newedge.va = vb + num_verts;
		newedge.vb = vb;
		newedge.fl = fl;
		mesh->E.push_back(newedge);

		newedge.va = vb;
		newedge.vb = va;
		newedge.fl = fl;
		mesh->E.push_back(newedge);

	}

	// update E's fr
	for (int i = 0; i < mesh->E.size(); i++) {
		bool found = false;
		for (Edge e : mesh->E) {
			if (mesh->E.at(i).va == e.vb && mesh->E.at(i).vb == e.va) {
				mesh->E.at(i).fr = e.fl;
				found = true;
				break;
			}
		}

	}
	cout << "edge num: " << mesh->E.size() << endl;

	update_v_neighbor(mesh);

}

//void BasicMeshGenerate(Mesh *mesh, MeshTopology *m_t) {
//
//	make_basic_mesh(mesh, m_t);
////		Catmull(mesh);
////		cout << "1st Catmull finished." << endl;
////		//Catmull(mesh);
////		cout << "2nd Catmull finished." << endl;
////		//Catmull(mesh);
////		cout << "3rd Catmull finished." << endl;
////		//Catmull(mesh);
////		cout << "4th Catmull finished." << endl;
//}
