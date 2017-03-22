#include "Cannon.h"
///https://en.wikipedia.org/wiki/Drag_coefficient
///https://en.wikipedia.org/wiki/Dynamic_pressure
///https://en.wikipedia.org/wiki/Drag_equation
const double PI = 3.14159265358979323846;
bool checkDistance = true;
Cannon::Cannon()
{
	this->gravity = 9.82; // m/s^2
	this->windDirection = glm::vec3(1,0,0); // Used for direction only, thus should always be normalized
	this->windDirection = glm::normalize(windDirection);
	this->windVelocity = 10; // m/s
	this->airDensity = 1.293; // kg/m^3
	this->dragCoefficientSphere = 0.29;
	this->angle = 42*PI/180;
	this->triesLeft = 3;
	this->amountOfHits = 0;
}
Cannon::~Cannon()
{

}
int Cannon::getAmountOfHits()
{
	return this->amountOfHits;
}
int Cannon::getTriesLeft()
{
	return this->triesLeft;
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
		0.6, 0.0, 0.0, 0.0,
		0.0, 0.6, 0.0, 0.0,
		0.0, 0.0, 0.2, 0.0,
		2, 3, -3, 1.0
	);
	targetModel = Model("models/sphere/sphere.obj", modelMat2);
}
void Cannon::update(float dt, std::vector<glm::vec3> &lightPositions)
{
	if (aCannonBall != nullptr)
	{
		if (aCannonBall->ballModel->getModelMatrix()[3][1] < -5)
		{
			delete aCannonBall;
			aCannonBall = nullptr;
			triesLeft--;
		}
		else
		{
			glm::vec3 cannonPos(aCannonBall->ballModel->getModelMatrix()[3]);
			glm::vec3 boxPos(targetModel.getModelMatrix()[3]);

			glm::vec3 cannonMin = cannonPos + aCannonBall->ballModel->getMinBounding();
			glm::vec3 cannonMax = cannonPos + aCannonBall->ballModel->getMaxBounding();

			glm::vec3 boxMin = boxPos + targetModel.getMinBounding();
			glm::vec3 boxMax = boxPos + targetModel.getMaxBounding();

			if (cannonMax.x > boxMin.x &&
				cannonMin.x < boxMax.x &&
				cannonMax.y > boxMin.y &&
				cannonMin.y < boxMax.y &&
				cannonMax.z > boxMin.z &&
				cannonMin.z < boxMax.z)
			{
				amountOfHits++;
				delete aCannonBall;
				aCannonBall = nullptr;
				float randomY = (rand() % 3);
				float randomZ = -(rand() % 8 + 2);
				glm::mat4 modelMat2 = glm::mat4(
					0.6, 0.0, 0.0, 0.0,
					0.0, 0.6, 0.0, 0.0,
					0.0, 0.0, 0.2, 0.0,
					2.0, randomY, randomZ, 1.0
				);
				targetModel.setModelMatrix(modelMat2);
			}
			else
			{
				//Calculates the area the sphere would have if it would be orthagonally projected (as in making it a 2D circle)
				float area = pow(aCannonBall->radius, 2) * PI;
				//Calculates drag to be used in wind and air resistance
				float drag = dragCoefficientSphere*(airDensity*area / 2);
				//Calculate the force of the wind
				glm::vec3 wind;
				wind.x = drag * pow((windDirection.x * windVelocity), 2);
				wind.y = drag * pow((windDirection.y * windVelocity), 2);
				wind.z = drag * pow((windDirection.z * windVelocity), 2);
				//Update speed for the ball
				aCannonBall->speedVector = aCannonBall->speedVector + (aCannonBall->accelVector * dt);
				//Calculate rotation for magnus force
				glm::vec3 angularVelocityX = glm::vec3(1, 0, 0) * aCannonBall->rotation.x;
				glm::vec3 angularVelocityY = glm::vec3(0, 1, 0) * aCannonBall->rotation.y;
				glm::vec3 angularVelocityZ = glm::vec3(0, 0, 1) * aCannonBall->rotation.z;
				//Gather the magnus forces from the rotations around each axis
				glm::vec3 magnusAroundXAxis = glm::vec3(pow(PI, 2) * pow(aCannonBall->radius, 3) * airDensity) * (cross(angularVelocityX*dt, aCannonBall->speedVector*dt));
				glm::vec3 magnusAroundYAxis = glm::vec3(pow(PI, 2) * pow(aCannonBall->radius, 3) * airDensity) * (cross(angularVelocityY*dt, aCannonBall->speedVector*dt));
				glm::vec3 magnusAroundZAxis = glm::vec3(pow(PI, 2) * pow(aCannonBall->radius, 3) * airDensity) * (cross(angularVelocityZ*dt, aCannonBall->speedVector*dt));
				//Update the total speed from all directions
				aCannonBall->velocity = sqrt(pow(aCannonBall->speedVector.z, 2) + pow(aCannonBall->speedVector.y, 2) + pow(aCannonBall->speedVector.z, 2));
				//Update the angle
				aCannonBall->direction = aCannonBall->speedVector;
				aCannonBall->direction = glm::normalize(aCannonBall->direction);
				//Update the acceleration for the ball
				//Wind and drag added to all axies accelerations
				aCannonBall->accelVector = (wind + magnusAroundXAxis + magnusAroundYAxis + magnusAroundZAxis + (-drag*pow(aCannonBall->velocity, 2)*aCannonBall->direction)) / aCannonBall->mass;
				//Gravitation only on the Y-axis
				aCannonBall->accelVector.y = -gravity;
				//Translate to new position
				aCannonBall->ballModel->setModelMatrix(
					glm::translate(aCannonBall->ballModel->getModelMatrix(), aCannonBall->speedVector*dt)
			);
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
	if (aCannonBall != nullptr)
	{
		lightPositions[0] = aCannonBall->ballModel->getModelMatrix()[3];
	}
	lightPositions[1] = glm::vec3(targetModel.getModelMatrix()[3][0], targetModel.getModelMatrix()[3][1], targetModel.getModelMatrix()[3][2]+1);
}
void Cannon::draw(Shader shader)
{
	glUniform1i(glGetUniformLocation(shader.program, "isMouseOvered"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &cannonModel.getModelMatrix()[0][0]);
	cannonModel.draw(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &cannonModel2.getModelMatrix()[0][0]);
	cannonModel2.draw(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &targetModel.getModelMatrix()[0][0]);
	targetModel.draw(shader);
	if (aCannonBall != nullptr)
	{
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &aCannonBall->ballModel->getModelMatrix()[0][0]);
		aCannonBall->ballModel->draw(shader);
	}
}
void Cannon::shoot(glm::vec3 originPos, Model ball)
{
	if (aCannonBall == nullptr && triesLeft > 0)
	{
		originPos = glm::vec3(this->cannonModel.getModelMatrix()[3][0], this->cannonModel.getModelMatrix()[3][1], this->cannonModel.getModelMatrix()[3][2]);

		CannonBall* newBall = new CannonBall();
		glm::mat4 modelMat = glm::mat4(
			0.3, 0.0, 0.0, 0.0,
			0.0, 0.3, 0.0, 0.0,
			0.0, 0.0, 0.3, 0.0,
			originPos.x, originPos.y + 0.5, originPos.z, 1.0
		);
		newBall->ballModel = new Model(ball, modelMat);

		newBall->velocity = 20; // m/s

		newBall->rotation = glm::vec3(0, 0, 0);

		//Angle which the ball starts from
		newBall->direction = glm::vec3(0, sin(this->angle), -cos(this->angle));
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
		aCannonBall = newBall;
	}
}