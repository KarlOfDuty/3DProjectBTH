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
		//double angle;
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

public:
	std::vector<CannonBall*> allCannonBalls;
	Model cannonModel;
	Model cannonModel2;
	std::vector<Model*> allTargets;
	float angle;
	int tests;
	float gravity;
	float windVelocity;
	float airDensity;
	float dragCoefficientSphere;
	glm::vec3 windDirection;
	Cannon();
	~Cannon();
	void loadModel(Model model, Model model2);
	void update(float dt);
	void draw(Shader shader);
	void shoot(glm::vec3 originPos, Model ball);
};
#endif