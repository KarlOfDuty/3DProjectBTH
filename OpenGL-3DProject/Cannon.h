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
		double velocity;
		//double angle;
		glm::vec3 direction;
		double radius; // m
		double density; // kg/m^3
		double mass; // kg
		glm::vec3 rotation; // rad/s
		//double spin;
		//float omega;
		
		double time;
		int loading;
	};
private:

public:
	std::vector<CannonBall> allCannonBalls;
	int tests;
	float gravity;
	float windVelocity;
	double airDensity;
	double dragCoefficientSphere;
	glm::vec3 windDirection;
	Cannon();
	~Cannon();
	void update(float dt);
	void draw(Shader shader);
	void shoot(glm::vec3 originPos);
	glm::vec3 getMovementVector(int cannonBallNr);
};
#endif