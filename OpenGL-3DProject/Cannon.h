#ifndef CANNON_H
#define CANNON_H
#include "Model.h"
#include <SFML\Window.hpp>
#include <glm\glm.hpp>
class Cannon
{
	struct CannonBall
	{
		Model* ballModel;
		float initialVelocity;
		glm::vec3 speedVector;
		glm::vec3 accelVector;
		float velocity;
		glm::vec3 direction;
		float radius; // m
		float density; // kg/m^3
		float mass; // kg
		glm::vec3 rotation; // rad/s
		
		double time;
		int loading;
		~CannonBall()
		{
			delete ballModel;
		}
	};
private:
	CannonBall* aCannonBall;
	Model cannonModel;
	Model cannonModel2;
	Model targetModel;
	float angle;
	float gravity;
	float windVelocity;
	float airDensity;
	float dragCoefficientSphere;
	glm::vec3 windDirection;
	int amountOfHits;
	int triesLeft;
public:
	Cannon(Model &sphere);
	Cannon();
	~Cannon();
	int getAmountOfHits();
	int getTriesLeft();
	void update(float dt, std::vector<glm::vec3> &lightPositions);
	void draw(Shader shader);
	void shoot(glm::vec3 originPos, Model ball);
};
#endif