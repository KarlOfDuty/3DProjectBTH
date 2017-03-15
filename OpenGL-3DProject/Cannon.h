#ifndef CANNON_H
#define CANNON_H
#include "Model.h"
#include <glm\glm.hpp>
class Cannon
{
	struct CannonBall
	{
		Model ballModel;
		float initialVelocity;
		glm::vec3 speedVector;
		glm::vec3 accelVector;
		float velocity;
		//double angle;
		glm::vec3 direction;
		float radius; // m
		float density; // kg/m^3
		float mass; // kg
		glm::vec3 rotation; // rad/s
		
		double time;
		int loading;
	};
private:

public:
	std::vector<CannonBall> allCannonBalls;
	int tests;
	float gravity;
	float windVelocity;
	float airDensity;
	float dragCoefficientSphere;
	glm::vec3 windDirection;
	Cannon();
	~Cannon();
	void update(float dt);
	void draw(Shader shader);
	void shoot(glm::vec3 originPos);
};
#endif