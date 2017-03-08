#ifndef CANNON_H
#define CANNON_H
#include "Model.h"
#include <glm\glm.hpp>
class Cannon
{
	struct CannonBall
	{
		Model ballModel;
		glm::vec3 initialVelocity;
		glm::vec3 speedVector;
		glm::vec3 accelVector;
		double velocity;
		double angle;
		
		double radius;
		double density;
		double mass;

		double spin;
		float omega;
		
		double time;
		int loading;

	};
private:

public:
	std::vector<CannonBall> allCannonBalls;
	int tests;
	float gravity;
	float wind;

	Cannon();
	~Cannon();
	void update(float dt);
	void draw(Shader shader);
	void shoot(glm::vec3 originPos);
	glm::vec3 getMovementVector(int cannonBallNr);
};
#endif