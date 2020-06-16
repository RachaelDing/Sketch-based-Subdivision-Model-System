#include <stdio.h>
#include <stdlib.h>

#include "Program.h"

#define PI_F 3.14159f
using namespace std;
using namespace glm;


vector<glm::vec3> E;
Geometry c_points;

float x_pos_weighted;
float y_pos_weighted;

double x_pos;
double y_pos;
int width;
int height;


Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
}

// Error callback for glfw errors
void Program::error(int error, const char* description) {
	std::cerr << description << std::endl;
}

// Called to start the program
void Program::start() {
	setupWindow();
	GLenum err = glewInit();
	if (glewInit() != GLEW_OK) {
		std::cerr << glewGetErrorString(err) << std::endl;
	}

	renderEngine = new RenderEngine(window);
	InputHandler::setUp(renderEngine);
	mainLoop();
}

// Creates GLFW window for the program and sets callbacks for input
void Program::setupWindow() {
	glfwSetErrorCallback(Program::error);
	if (glfwInit() == 0) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(1024, 1024, "CPSC589 Project", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // V-sync on

	glfwSetKeyCallback(window, InputHandler::key);
	glfwSetMouseButtonCallback(window, InputHandler::mouse);
	glfwSetCursorPosCallback(window, InputHandler::motion);
	glfwSetScrollCallback(window, InputHandler::scroll);
	glfwSetWindowSizeCallback(window, InputHandler::reshape);
}

Geometry points = Geometry();
Geometry coarse = Geometry();
Geometry coarse_tri = Geometry();
Geometry sub0 = Geometry();
Geometry sub1 = Geometry();
Geometry sub2 = Geometry();
Geometry sub3 = Geometry();
Geometry sub4 = Geometry();
Geometry delauny_triangles = Geometry();
Geometry delauny_triangle_lines = Geometry();
Geometry quads = Geometry();
Geometry quad_lines = Geometry();
Geometry n_side_face_lines = Geometry();

MeshTopology mesh_topology = MeshTopology();

Mesh mesh0, mesh1, mesh2, mesh3, mesh4;;
int sub_flg[10];
int face_norm = 0;

// Main loop
void Program::mainLoop() {

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// three vertex positions and assocated colours of a triangle
	//Fill in with Perspective Matrix
	glm::mat4 perspectiveMatrix = glm::perspective(PI_F * .5f,
			float(width) / float(height), 0.0001f, 20.f);//last 2 arg, nearst and farest

//	Camera cam;
	vec2 lastCursorPos;
	float cam_phi, cam_theta;					//			cdt_y.v1 = vec3(0.0, 0.0, 0.0);
	float cursorSensitivity = PI_F / 500.f;	//PI/hundred pixels
	vec3 cam_transition = vec3(0, 0, 0);
	cam_phi = PI_F / 2.f;
	cam_theta = PI_F / 2.f;
	cam.pos.y = cam.radius * cos(cam_phi);
	cam.pos.x = cam.radius * abs(sin(cam_phi)) * cos(cam_theta);
	cam.pos.z = cam.radius * abs(sin(cam_phi)) * sin(cam_theta);

	cout << "***************************************************************" << endl;
	cout << "***   1. Press left mouse button to start drawing.          ***" << endl;
	cout << "***   2. Left click to show the coarse polygon.             ***" << endl;
	cout << "***   3. Left click to show the triangulation.              ***" << endl;
	cout << "***   4. Left click to show the quads and n-sided faces.    ***" << endl;
	cout << "***   5. Press UP key to show the 3D polygon.               ***" << endl;
	cout << "***   5. Press UP and DOWN key to show subdivision in       ***" << endl;
	cout << "***      different order.                                   ***" << endl;
	cout << "***   6. Press F key to toggle between framework,           ***" << endl;
	cout << "***      face-normal surface, and vertex-normal surface.    ***" << endl;
	cout << "***   7. Left click to drag and rotate the model.           ***" << endl;
	cout << "***************************************************************" << endl;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		if(stage == 0){	// adding points
			glfwGetWindowSize(window, &width, &height);
			glfwGetCursorPos(window, &x_pos, &y_pos);

			x_pos_weighted = (double) x_pos * (double) 2.0 / (double) width
					- (double) 1.0;
			y_pos_weighted = ((double) height - (float) y_pos) * (double) 2.0
					/ (double) height - (double) 1.0;

			vec3 newpoint = vec3(x_pos_weighted, y_pos_weighted, 0.0);
			int size = E.size();
			if (size < 3) {
				E.push_back(newpoint);
			} else if (pointDistance(newpoint, E.at(size - 1)) > 0.01) {
				E.push_back(newpoint);
			}

			objects.clear();
			points = Geometry::makeSpline(&E, 0.001f, 4);
			renderEngine->assignBuffers(points);
			renderEngine->updateBuffers(points);
			objects.push_back(&points);
		}
		else if(stage == 1){	// show polygon

			if(sub_flg[1] != 1){
				// Draw the coarse polygon
				if (E.size() > 6) {
					E.push_back(E.at(0));
					E.push_back(E.at(1));
					E.push_back(E.at(2));
				}else{
					stage = 0;
					continue;					mesh2 = mesh1;
				}

				objects.clear();

				points = Geometry::makeSpline(&E, 0.001f, 4);
				renderEngine->assignBuffers(points);
				renderEngine->updateBuffers(points);
				objects.push_back(&points);

				coarse = Geometry::reduceResolution_noBoundary(&(points.verts));
				coarse = Geometry::reduceResolution_noBoundary(&(coarse.verts));
				coarse = Geometry::reduceResolution_noBoundary(&(coarse.verts));
				coarse = Geometry::reduceResolution_noBoundary(&(coarse.verts));
				renderEngine->assignBuffers(coarse);
				renderEngine->updateBuffers(coarse);
				objects.push_back(&coarse);

				sub_flg[1] = 1;
			}

		}
		else if(stage == 2){	// show triangle
			if(sub_flg[2] != 2){

				objects.clear();
				// Triangle polygon

				mesh_topology = MeshTopology::build_triangles(&coarse.verts);
				delauny_triangles = Geometry();
				mesh_topology.show_triangles(&delauny_triangles);
				renderEngine->assignBuffers(delauny_triangles);
				renderEngine->updateBuffers(delauny_triangles);
				objects.push_back(&delauny_triangles);

				mesh_topology.show_triangle_lines(&delauny_triangle_lines);
				renderEngine->assignBuffers(delauny_triangle_lines);
				renderEngine->updateBuffers(delauny_triangle_lines);
				objects.push_back(&delauny_triangle_lines);
				cout << "Finishing triangulation" << endl;

				sub_flg[2] = 2;
			}

		}else if(stage == 8){	// show quad and n-sided face polygon
			if(sub_flg[8] != 8){

				objects.clear();
				// Triangle polygon

				mesh_topology.build_2D_mesh();
				mesh_topology.show_quads(&quads);
				renderEngine->assignBuffers(quads);
				renderEngine->updateBuffers(quads);
				objects.push_back(&quads);

				mesh_topology.show_quad_lines(&quad_lines);
				renderEngine->assignBuffers(quad_lines);
				renderEngine->updateBuffers(quad_lines);
				objects.push_back(&quad_lines);
				//show n side face as well as some special face
				mesh_topology.show_n_side_face_lines(&n_side_face_lines);
				renderEngine->assignBuffers(n_side_face_lines);
				renderEngine->updateBuffers(n_side_face_lines);
				objects.push_back(&n_side_face_lines);

				sub_flg[8] = 8;
			}

		}
		else{

			/* Camera movement */
			//Rotation
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			vec2 cursorPos(xpos, ypos);
			vec2 cursorChange = cursorPos - lastCursorPos;

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				cam_phi += cursorChange.y * cursorSensitivity;	// along longitude
				cam_theta += cursorChange.x * cursorSensitivity;// along latitude
				if (cam_theta > 2 * PI_F)
					cam_theta -= 2 * PI_F;
				else if (cam_theta < -2 * PI_F)	//			Geometry::CDT_Triangle cdt_y;
					cam_theta += 2 * PI_F;
				if (cam_phi > PI_F - 0.001)
					cam_phi = PI_F - 0.001;
				if (cam_phi < 0.001)
					cam_phi = 0.001;
			}
			lastCursorPos = cursorPos;

			cam.pos.y = cam.radius * cos(cam_phi);
			cam.pos.x = cam.radius * sin(cam_phi) * cos(cam_theta);
			cam.pos.z = cam.radius * sin(cam_phi) * sin(cam_theta);

			cam.centre = vec3(0, 0, 0) + cam_transition;
			cam.pos = cam.pos + cam_transition;

			glm::mat4 modelMatrix = { 2.f / width, 0, 0, 0, 0, 2.f / height, 0, 0,
					0, 0, 1, 0, -1, -1, 0, 1 };

			if(stage == 3){	// show 0 sub
				if(sub_flg[3] != 3){
					//BasicMeshGenerate(&mesh0, &mesh_topology);
					make_basic_mesh(&mesh0, &mesh_topology);
					mesh1 = mesh0;
					drawRawMesh(&mesh0, &sub0);
					objects.clear();
					renderEngine->assignBuffers(sub0);
					renderEngine->updateBuffers(sub0);
					objects.push_back(&sub0);
					sub_flg[3] = 3;
				}else{
					objects.clear();
					renderEngine->assignBuffers(sub0);
					renderEngine->updateBuffers(sub0);
					objects.push_back(&sub0);
				}

			}else if(stage == 4){	// show 1st sub
				if(sub_flg[4] != 4){
					cout << "1st subdividing..." << endl;
					Catmull(&mesh1);
					drawMesh(&mesh1, &sub1, face_norm);
					objects.clear();
					renderEngine->assignBuffers(sub1);
					renderEngine->updateBuffers(sub1);
					objects.push_back(&sub1);
					sub_flg[4] = 4;
					cout << "1st subdivision finished..." << endl;
					mesh2 = mesh1;
				}else{
					objects.clear();
					if(face_flg == 1){
						if(face_norm == 1)face_norm = 0;
						else if(face_norm == 2)face_norm = 1;
						else if(face_norm == 0) face_norm = 2;
						drawMesh(&mesh1, &sub1, face_norm);
						face_flg = 0;
					}
					renderEngine->assignBuffers(sub1);
					renderEngine->updateBuffers(sub1);
					objects.push_back(&sub1);
				}

			}else if(stage == 5){	// show 2nd sub
				if(sub_flg[5] != 5){
					cout << "2nd subdividing..." << endl;
					Catmull(&mesh2);
					drawMesh(&mesh2, &sub2, face_norm);
					objects.clear();
					renderEngine->assignBuffers(sub2);
					renderEngine->updateBuffers(sub2);
					objects.push_back(&sub2);
					sub_flg[5] = 5;
					cout << "2nd subdivision finished..." << endl;
					mesh3 = mesh2;
				}else {
					objects.clear();
					if(face_flg == 1){
						if(face_norm == 1)face_norm = 0;
						else if(face_norm == 2)face_norm = 1;
						else if(face_norm == 0) face_norm = 2;
						drawMesh(&mesh2, &sub2, face_norm);
						face_flg = 0;
					}
					renderEngine->assignBuffers(sub2);
					renderEngine->updateBuffers(sub2);
					objects.push_back(&sub2);
				}
			}else if(stage == 6){	// show 3 sub
				if(sub_flg[6] != 6){
					cout << "3rd subdividing..." << endl;
					Catmull(&mesh3);
					drawMesh(&mesh3, &sub3, face_norm);
					objects.clear();
					renderEngine->assignBuffers(sub3);
					renderEngine->updateBuffers(sub3);
					objects.push_back(&sub3);
					sub_flg[6] = 6;
					cout << "3rd subdivision finished..." << endl;
					mesh4 = mesh3;
				}else {
					objects.clear();
					if(face_flg == 1){
						if(face_norm == 1)face_norm = 0;
						else if(face_norm == 2)face_norm = 1;
						else if(face_norm == 0) face_norm = 2;
						drawMesh(&mesh3, &sub3, face_norm);
						face_flg = 0;
					}
					//renderEngine->assignBuffers(sub3);
					renderEngine->updateBuffers(sub3);
					objects.push_back(&sub3);
				}
			}else if(stage == 7){	// show 4 sub
				if(sub_flg[7] != 7){
					cout << "4th subdividing..." << endl;
					Catmull(&mesh4);
					drawMesh(&mesh4, &sub4, face_norm);
					objects.clear();
					renderEngine->assignBuffers(sub4);
					renderEngine->updateBuffers(sub4);cout << "4th subdividing..." << endl;
					objects.push_back(&sub4);
					sub_flg[7] = 7;
					cout << "4th subdivision finished..." << endl;
				}else {
					objects.clear();
					if(face_flg == 1){
						if(face_norm == 1)face_norm = 0;
						else if(face_norm == 2)face_norm = 1;
						else if(face_norm == 0) face_norm = 2;
						drawMesh(&mesh4, &sub4, face_norm);
						face_flg = 0;
					}
					//renderEngine->assignBuffers(sub4);
					renderEngine->updateBuffers(sub4);
					objects.push_back(&sub4);
				}
			}
		}

		renderEngine->render(objects, perspectiveMatrix * cam.viewMatrix());

		glfwSwapBuffers(window);
	}

	// Clean up, program needs to exit
	glfwDestroyWindow(window);
	delete renderEngine;
	glfwTerminate();
}
