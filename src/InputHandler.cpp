#include "InputHandler.h"

RenderEngine* InputHandler::renderEngine;
int InputHandler::mouseOldX;
int InputHandler::mouseOldY;
using namespace std;
using namespace glm;


#define P_RADIUS .23f
#define MAX_ORDER 10
#define MAX_INCREMENT 1
#define U_INC_KEY 0.02f
#define NURBS_W_INC 0.2f

bool adding_points = true;
bool finish_adding_points = false;

int stage = -1;
int face_flg = 0;

int order_k = 4;
float increment_u = 0.01f;
float u_pos=0.f;	// for geometric display
int flg_G_mode = 0;		// display in a geometric fashion

int flg_NURBS_mode = 0;
vector<float> weights;

std::vector<glm::vec3> control_points;
int flg_point_selected = -1;
int flg_point_drag = 0;
vec2 drag_point_press_pos;
vec2 drag_point_release_pos;


double pointDistance(glm::vec3 a, glm::vec3 b){
	double x = a.x-b.x;
	double y = a.y-b.y;
	double z = a.z-b.z;
	return sqrt(x*x+y*y+z*z);
}

// Find if p is in the point list, return the index. if not exist, return -1
int selectPoint(vec3 p, vector<vec3> *list, double range){
	for(int i=0; i<list->size(); i++){
		if(pointDistance(p, list->at(i)) < 2*P_RADIUS)return i;
	}
	return -1;
}

// Must be called before processing any GLFW events
void InputHandler::setUp(RenderEngine* renderEngine) {
	InputHandler::renderEngine = renderEngine;
}

// Callback for key presses
void InputHandler::key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE) {
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(0);
	}
	else if(key == GLFW_KEY_UP && action == GLFW_PRESS){
		if(stage == 3){	// show 0st subdivision
			stage = 4;
			return ;
		}else if(stage == 4){	// show 1st subdivision
			stage = 5;
		}else if(stage == 5){	// show 1st subdivision
			stage = 6;
		}
		else if(stage == 6){	// show 1st subdivision
			stage = 7;
		}
		else if(stage == 8){
			stage = 3;
		}
	}
	else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
		if(stage == 5){
			stage = 4; return;
		}else if(stage == 6){	// show 1st subdivision
			stage = 5;
		}else if(stage == 4){
			stage = 3; return;
		}else if(stage == 7){	// show 1st subdivision
			stage = 6;
		}
	}
	else if(key == GLFW_KEY_F && action == GLFW_PRESS){
		if(face_flg == 0)face_flg = 1;
	}
}

// Callback for mouse button presses
void InputHandler::mouse(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {

			if(stage == 1){	//show point spline
				stage = 2;
				return;
			}else if(stage == -1){
				stage = 0; return;
			}else if(stage == 2){
				stage = 8; return;
			}

		} else if (action == GLFW_RELEASE) {
			if(stage == 0){	// add points
				stage = 1;  return;
			}
		}
	}

}

// Callback for mouse motion
void InputHandler::motion(GLFWwindow* window, double x, double y) {
	if(1 == flg_point_drag){
		double xpos = 10.f*(x-256)/256.f;
		double ypos = -10.f*(y-256.f)/256.f;
		vec3 p = vec3(xpos, ypos, 0);
		if(pointDistance(p, control_points.at(flg_point_selected))>0.2f*P_RADIUS){
			control_points.at(flg_point_selected) = p;
		}
	}
	mouseOldX = x;
	mouseOldY = y;
}

// Callback for mouse scroll
void InputHandler::scroll(GLFWwindow* window, double x, double y) {
	if(flg_point_selected!=-1&&flg_NURBS_mode == 1){
		weights.at(flg_point_selected) += y*NURBS_W_INC;
	}
}

// Callback for window reshape/resize
void InputHandler::reshape(GLFWwindow* window, int width, int height) {
	renderEngine->setWindowSize(width, height);
}
