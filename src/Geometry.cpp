#include "Geometry.h"

#include <cmath>
using namespace glm;
using namespace std;

float As2[] = { -0.352518, 0.705036, 0.97123, -0.431655, 0.107914 };
float As3[] = { 0.18, -0.36, -0.08, 1.28, 0.43, -0.6, 0.15 };
float Ar[] = { 0.117347, -0.469388, 0.321429, 1.061224, 0.321429, -0.469388,
		0.117347 };

Geometry::Geometry() {
	drawMode = GL_TRIANGLES;
	vao = 0;
	vertexBuffer = 0;
	colourBuffer = 0;
	modelMatrix = glm::mat4(1.f);
	pointSize = 10.0f;
}

void Geometry::clear(){
	verts.clear();
	colours.clear();
}

Geometry Geometry::makeCircle(float radius, float uInc) {

	Geometry circle= Geometry();

	for (double u = 0.0; u <= 2.0 * M_PI; u += uInc) {
		circle.verts.push_back(radius * glm::vec3(cos(u), sin(u), 0));
		circle.colours.push_back(glm::vec3(0.f, 1.f, 0.f));
	}
	circle.drawMode = GL_LINE_STRIP;
	return circle;
}

Geometry Geometry::makePoint(float x, float y, float z) {
	Geometry point= Geometry();

	point.verts.push_back(glm::vec3(x, y, 0));
	point.colours.push_back(glm::vec3(1.f, 0.f, 0.f));
	point.drawMode = GL_POINTS;

	return point;
}

Geometry Geometry::makePoints() {
	Geometry point= Geometry();

	point.verts.push_back(glm::vec3(-1, -1, 0));
	point.colours.push_back(glm::vec3(1.f, 0.f, 0.f));

	point.verts.push_back(glm::vec3(-1, 1, 0));
	point.colours.push_back(glm::vec3(0.f, 1.f, 0.f));

	point.verts.push_back(glm::vec3(1, -1, 0));
	point.colours.push_back(glm::vec3(0.f, 0.f, 1.f));

	point.verts.push_back(glm::vec3(1, 1, 0));
	point.colours.push_back(glm::vec3(1.f, 1.f, 0.f));
	point.drawMode = GL_POINTS;

	return point;
}

int getDelta(vector<float> *U, float u, int k, int m){	// return the index of knot
	for(int i=k-1; i<m+k-1; i++){
		if(u>=U->at(i) && u<U->at(i+1))return i;
	}
	return m+k-2;
}

vec3 E_delta_1(vector<vec3> *E, vector<float> *U, float u, int k, int m){
	int d = getDelta(U, u, k, m);
	vector<vec3> C;
	for(int i=0; i<k; i++){
		C.push_back(E->at(d-i));
	}

	for(int r = k; r>=2; r--){
		int i=d;
		for(int s=0; s<=r-2; s++){
			float omega = (u-U->at(i))/(U->at(i+r-1) - U->at(i));
			C.at(s) = C.at(s)*omega + (1.f-omega)*C.at(s+1);
			i--;
		}
	}
	return C.at(0);
}

Geometry Geometry::makeSpline(std::vector<glm::vec3> *P,float u_inc, int k){
	Geometry spline= Geometry();
	int m = P->size()-1;
	if(m<k-1)return spline;
	float step = 1.f/(float)(m-k+2);
	vector<float> U;	//knot sequence
	for(int i=0; i<k; i++){
		U.push_back(0);
	}

	for(int i=1; i<=m-k+1; i++){
		U.push_back(step*i);
	}

	for(int i=0; i<k; i++){
		U.push_back(1);
	}

	for(float u=0.f; u<=1; u+=u_inc){
		vec3 E = E_delta_1(P, &U, u, k, m);
		spline.verts.push_back(E);
		spline.colours.push_back(vec3(1,0,0));
	}

	spline.drawMode = GL_LINE_STRIP;
	return spline;
}

Geometry Geometry::reduceResolution(vector<glm::vec3> * F) {
	//input fine points, output approximation control polygon
	Geometry C= Geometry();
	int m = F->size();
	if (m < 9)
		return C;
	// the start boundary
	//C1
	C.verts.push_back(F->at(0));
	C.colours.push_back(vec3(0, 1, 1));
	//C2
	vec3 tmp = vec3(0, 0, 0);
	for (int i = 0; i < 5; i++) {
		tmp = tmp + As2[i] * F->at(i);
	}
	C.verts.push_back(tmp);
	C.colours.push_back(vec3(0, 1, 1));
	//C3
	tmp = vec3(0, 0, 0);
	for (int i = 0; i < 7; i++) {
		tmp = tmp + As3[i] * F->at(i);
	}
	C.verts.push_back(tmp);
	C.colours.push_back(vec3(1, 0, 0));

	// regular vertices
	for (int i = 2; i < m - 8; i += 2) {
		tmp = vec3(0, 0, 0);
		for (int k = 0; k < 7; k++) {
			tmp = tmp + Ar[k] * F->at(i + k);
		}
		C.verts.push_back(tmp);
		C.colours.push_back(vec3(0, 1, 1));
	}
	// the end boundary
	// C_j
	tmp = vec3(0, 0, 0);
	for (int i = 6; i >= 0; i--) {
		tmp = tmp + As3[i] * F->at(m - i - 1);
	}
	C.verts.push_back(tmp);
	C.colours.push_back(vec3(0, 1, 1));
	// C_j+1
	tmp = vec3(0, 0, 0);
	for (int i = 4; i >= 0; i--) {
		tmp = tmp + As2[i] * F->at(m - i - 1);
	}
	C.verts.push_back(tmp);
	C.colours.push_back(vec3(0, 1, 1));
	// C_j+2
	C.verts.push_back(F->at(m - 1));
	C.colours.push_back(vec3(0, 1, 1));

	// connect to the first point
//	if (end_flg) {
//		C.verts.push_back(C.verts.at(0));
//		C.colours.push_back(vec3(0, 1, 1));
//	}

	C.drawMode = GL_LINE_STRIP;
	return C;
}

Geometry Geometry::reduceResolution_noBoundary(vector<glm::vec3> * F) {	//input fine points, output approximation control polygon
	Geometry C= Geometry();
	int m = F->size();
	if (m < 9)
		return C;
	// the start boundary
	vec3 tmp = vec3(0, 0, 0);
	//C1
	tmp += Ar[0] * F->at(m - 4);
	tmp += Ar[1] * F->at(m - 3);
	tmp += Ar[2] * F->at(m - 2);
	tmp += Ar[3] * F->at(m - 1);
	tmp += Ar[4] * F->at(0);
	tmp += Ar[5] * F->at(1);
	tmp += Ar[6] * F->at(2);
	C.verts.push_back(tmp);
	C.colours.push_back(vec3(0, 1, 1));

	//C2
	tmp = vec3(0, 0, 0);
	tmp += Ar[0] * F->at(m - 2);
	tmp += Ar[1] * F->at(m - 1);
	tmp += Ar[2] * F->at(0);
	tmp += Ar[3] * F->at(1);
	tmp += Ar[4] * F->at(2);
	tmp += Ar[5] * F->at(3);
	tmp += Ar[6] * F->at(4);
	C.verts.push_back(tmp);
	C.colours.push_back(vec3(0, 1, 1));

	//C3
	tmp = vec3(0, 0, 0);
	tmp += Ar[0] * F->at(0);
	tmp += Ar[1] * F->at(1);
	tmp += Ar[2] * F->at(2);
	tmp += Ar[3] * F->at(3);
	tmp += Ar[4] * F->at(4);
	tmp += Ar[5] * F->at(5);
	tmp += Ar[6] * F->at(6);
	C.verts.push_back(tmp);
	C.colours.push_back(vec3(1, 0, 0));

	// regular vertices
	for (int i = 2; i < m - 8; i += 2) {
		tmp = vec3(0, 0, 0);
		for (int k = 0; k < 7; k++) {
			tmp = tmp + Ar[k] * F->at(i + k);
		}
		C.verts.push_back(tmp);
		C.colours.push_back(vec3(0, 1, 1));
	}


	tmp = vec3(0, 0, 0);
	tmp += Ar[0] * F->at(m - 6);
	tmp += Ar[1] * F->at(m - 5);
	tmp += Ar[2] * F->at(m - 4);
	tmp += Ar[3] * F->at(m - 3);
	tmp += Ar[4] * F->at(m - 2);
	tmp += Ar[5] * F->at(m - 1);
	tmp += Ar[6] * F->at(0);
	C.verts.push_back(tmp);
	C.colours.push_back(vec3(0, 1, 1));


	C.drawMode = GL_LINE_STRIP;
	return C;
}
