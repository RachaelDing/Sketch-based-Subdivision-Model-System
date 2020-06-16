/*
 * Camera.h
 *
 *  Created on: Nov 30, 2018
 *      Author: xi.wang1
 */

#ifndef SRC_CAMERA_H_
#define SRC_CAMERA_H_

#include <glm/glm.hpp>

class Camera{
public:
	glm::vec3 dir, right, up, pos, centre;
	float radius;

	Camera():dir(glm::vec3(0, 0, -1)), right(glm::vec3(1, 0, 0)), up(glm::vec3(0, 1, 0)), pos(glm::vec3(0,0,1)), radius(1), centre(0,0,0)
			{}
	Camera(glm::vec3 dir, glm::vec3 right, glm::vec3 up, glm::vec3 pos):dir(dir), right(right), up(up), pos(pos)
			{}


	glm::mat4 viewMatrix() const;		//constant member function, can't modify the object
										//inside const funtion, only call const functions

	void rotateVertical(float radians);
	void rotateHorizontal(float radians);
	void move(glm::vec3 movement);		//Moves in rotated frame
};



#endif /* SRC_CAMERA_H_ */
