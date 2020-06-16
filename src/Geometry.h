#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <vector>
using namespace glm;
using namespace std;


class Geometry {

public:
	Geometry();
	void clear();

	GLuint drawMode;
	GLuint vao;
	GLuint vertexBuffer;
	GLuint colourBuffer;
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> colours;
	glm::mat4 modelMatrix;
	float pointSize;


	static Geometry makePoints();
	static Geometry makePoint(float x, float y, float z);
	static Geometry makeCircle(float radius, float uInc);
	static Geometry makeSpline(std::vector<glm::vec3> *P,float u_inc, int k);

	static Geometry reduceResolution(vector<vec3> * F);
	static Geometry reduceResolution_noBoundary(vector<vec3> * F);
};

