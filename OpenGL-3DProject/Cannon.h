#ifndef CANNON_H
#define CANNON_H
#include "Model.h"
#include <glm\glm.hpp>
class Cannon
{
	struct CannonBall
	{
		Model ballModel;
		
		glm::vec3 speedVector;
		glm::vec3 accelVector;
		float velocity;
		float angle;
		
		float radius;
		float density;
		float mass;

		float spin;
		float omega;
	};
private:

public:
	std::vector<CannonBall> allCannonBalls;
	float gravity;
	Cannon();
	~Cannon();
	void update(float dt);
	void draw(Shader shader);
	void shoot(glm::vec3 originPos);
	glm::vec3 getMovementVector(int cannonBallNr);
};
#endif