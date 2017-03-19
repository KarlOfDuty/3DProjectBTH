#include "Cannon.h"
///https://en.wikipedia.org/wiki/Drag_coefficient
///https://en.wikipedia.org/wiki/Dynamic_pressure
///https://en.wikipedia.org/wiki/Drag_equation
const double PI = 3.14159265358979323846;
bool checkDistance = true;
Cannon::Cannon()
{
	this->allCannonBalls = std::vector<CannonBall*>();
	this->gravity = 9.82; // m/s^2
	this->tests = 0;
	this->windDirection = glm::vec3(1,0,0); // Used for direction only, thus should always be normalized
	this->windDirection = glm::normalize(windDirection);
	this->windVelocity = 10; // m/s
	this->airDensity = 1.293; // kg/m^3
	this->dragCoefficientSphere = 0.29;
	this->angle = 42*PI/180;
}
Cannon::~Cannon()
{

}
void Cannon::loadModel(Model model, Model model2)
{
	this->cannonModel = Model(model);
	this->cannonModel2 = Model(model2);
	this->cannonModel.setRotationMatrix(glm::rotate(glm::mat4(), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	this->cannonModel2.setRotationMatrix(glm::rotate(glm::mat4(), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	this->cannonModel.rotate();
	this->cannonModel2.rotate();

	glm::mat4 modelMat2 = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-2, 0, -5, 1.0
	);
	allTargets.push_back(new Model("models/cube/cube.obj", modelMat2));
}
void Cannon::update(float dt)
{
	for (int i = 0; i < allCannonBalls.size(); i++)
	{
		if (allCannonBalls[i]->ballModel->getModelMatrix()[3][1] < -10)
		{
			delete allCannonBalls[i];
			allCannonBalls.erase(allCannonBalls.begin()+i);
		}
		else
		{
			//Calculates the area the sphere would have if it would be orthagonally projected (as in making it a 2D circle)
			float area = pow(allCannonBalls[i]->radius, 2) * PI;
			//Calculates drag to be used in wind and air resistance
			float drag = dragCoefficientSphere*(airDensity*area / 2);
			//Calculate the force of the wind
			glm::vec3 wind;
			wind.x = drag * pow((windDirection.x * windVelocity), 2);
			wind.y = drag * pow((windDirection.y * windVelocity), 2);
			wind.z = drag * pow((windDirection.z * windVelocity), 2);
			//Update speed for the ball
			allCannonBalls[i]->speedVector = allCannonBalls[i]->speedVector + (allCannonBalls[i]->accelVector * dt);
			//Calculate rotation for magnus force
			glm::vec3 angularVelocityX = glm::vec3(1, 0, 0) * allCannonBalls[i]->rotation.x;
			glm::vec3 angularVelocityY = glm::vec3(0, 1, 0) * allCannonBalls[i]->rotation.y;
			glm::vec3 angularVelocityZ = glm::vec3(0, 0, 1) * allCannonBalls[i]->rotation.z;
			//Gather the magnus forces from the rotations around each axis
			glm::vec3 magnusAroundXAxis = glm::vec3(pow(PI, 2) * pow(allCannonBalls[i]->radius, 3) * airDensity) * (cross(angularVelocityX*dt, allCannonBalls[i]->speedVector*dt));
			glm::vec3 magnusAroundYAxis = glm::vec3(pow(PI, 2) * pow(allCannonBalls[i]->radius, 3) * airDensity) * (cross(angularVelocityY*dt, allCannonBalls[i]->speedVector*dt));
			glm::vec3 magnusAroundZAxis = glm::vec3(pow(PI, 2) * pow(allCannonBalls[i]->radius, 3) * airDensity) * (cross(angularVelocityZ*dt, allCannonBalls[i]->speedVector*dt));
			//Update the total speed from all directions
			allCannonBalls[i]->velocity = sqrt(pow(allCannonBalls[i]->speedVector.z, 2) + pow(allCannonBalls[i]->speedVector.y, 2) + pow(allCannonBalls[i]->speedVector.z, 2));
			//Update the angle
			allCannonBalls[i]->direction = allCannonBalls[i]->speedVector;
			allCannonBalls[i]->direction = glm::normalize(allCannonBalls[i]->direction);
			//Update the acceleration for the ball
			//Wind and drag added to all axies accelerations
			allCannonBalls[i]->accelVector = (wind + magnusAroundXAxis + magnusAroundYAxis + magnusAroundZAxis + (-drag*pow(allCannonBalls[i]->velocity, 2)*allCannonBalls[i]->direction)) / allCannonBalls[i]->mass;
			//Gravitation only on the Y-axis
			allCannonBalls[i]->accelVector.y = -gravity;
			//Translate to new position
			allCannonBalls[i]->ballModel->setModelMatrix(
				glm::translate(allCannonBalls[i]->ballModel->getModelMatrix(), allCannonBalls[i]->speedVector*dt)
			);
			
			glm::vec3 cannonPos(allCannonBalls[i]->ballModel->getModelMatrix()[3][0], allCannonBalls[i]->ballModel->getModelMatrix()[3][1], allCannonBalls[i]->ballModel->getModelMatrix()[3][2]);
			glm::vec3 boxPos(allTargets[0]->getModelMatrix()[3][0], allTargets[0]->getModelMatrix()[3][1], allTargets[0]->getModelMatrix()[3][2]);
			
			glm::vec3 cannonMin = cannonPos + allCannonBalls[i]->ballModel->getMinBounding();
			glm::vec3 cannonMax = cannonPos + allCannonBalls[i]->ballModel->getMaxBounding();

			glm::vec3 boxMin = boxPos + allTargets[0]->getMinBounding();
			glm::vec3 boxMax = boxPos + allTargets[0]->getMaxBounding();
			if (cannonMax.x > boxMin.x &&
				cannonMin.x < boxMax.x &&
				cannonMax.y > boxMin.y &&
				cannonMin.y < boxMax.y &&
				cannonMax.z > boxMin.z &&
				cannonMin.z < boxMax.z)
			{
				std::cout << "Träff!" << std::endl;
				delete allCannonBalls[i];
				allCannonBalls.erase(allCannonBalls.begin() + i);
				//delete allTargets[0];
				//allTargets.erase(allTargets.begin());
				//srand(time(0));
				float randomY = (rand() % 5);
				float randomZ = -(rand() % 10 + 2);
				glm::mat4 modelMat2 = glm::mat4(
					1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					-2, randomY, randomZ, 1.0
				);
				allTargets[0]->setModelMatrix(modelMat2);
			}
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && angle < 90*PI/180)
	{
		this->cannonModel.setRotationMatrix(glm::rotate(glm::mat4(), glm::radians(-0.55f), glm::vec3(1.0f, 0.0f, 0.0f)));
		this->cannonModel.rotate();
		this->angle += 0.01;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && angle > 0)
	{
		this->cannonModel.setRotationMatrix(glm::rotate(glm::mat4(), glm::radians(0.55f), glm::vec3(1.0f, 0.0f, 0.0f)));
		this->cannonModel.rotate();
		this->angle -= 0.01;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Home))
	{
		std::cout << this->cannonModel.getModelMatrix()[1][1] << std::endl;
		std::cout << this->angle << std::endl;
	}
}
void Cannon::draw(Shader shader)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &cannonModel.getModelMatrix()[0][0]);
	cannonModel.draw(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &cannonModel2.getModelMatrix()[0][0]);
	cannonModel2.draw(shader);
	for (int i = 0; i < allCannonBalls.size(); i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &allCannonBalls[i]->ballModel->getModelMatrix()[0][0]);
		allCannonBalls[i]->ballModel->draw(shader);
	}
	for (int i = 0; i < allTargets.size(); i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &allTargets[i]->getModelMatrix()[0][0]);
		allTargets[i]->draw(shader);
	}
}
void Cannon::shoot(glm::vec3 originPos, Model ball)
{
	originPos = glm::vec3(this->cannonModel.getModelMatrix()[3][0], this->cannonModel.getModelMatrix()[3][1], this->cannonModel.getModelMatrix()[3][2]);

	CannonBall* newBall = new CannonBall();
	glm::mat4 modelMat = glm::mat4(
		0.3, 0.0, 0.0, 0.0,
		0.0, 0.3, 0.0, 0.0,
		0.0, 0.0, 0.3, 0.0,
		originPos.x, originPos.y+0.5, originPos.z, 1.0
	);
	newBall->ballModel = new Model(ball, modelMat);

	newBall->velocity = 20; // m/s
	
	newBall->rotation = glm::vec3(0,0,0);

	//Angle which the ball starts from
	newBall->direction = glm::vec3(0,sin(this->angle),-cos(this->angle));
	newBall->direction = glm::normalize(newBall->direction);
	newBall->initialVelocity = 20; // m/s

	//A vector with the velocity in each direction
	newBall->speedVector = newBall->direction * newBall->initialVelocity;

	//A vector with the acceleration in each direction
	newBall->accelVector.x = 0;
	newBall->accelVector.y = 1;
	newBall->accelVector.z = -2;
	
	//Density of concrete
	newBall->density = 2000; // kg/m^3
	newBall->radius = 0.5; // m
	float volume = (4 / 3)*PI*pow(newBall->radius, 3); // m^3
	newBall->mass = volume * newBall->density; // kg
	
	newBall->loading = 0;
	newBall->time = 0.0f;
	allCannonBalls.push_back(newBall);
}