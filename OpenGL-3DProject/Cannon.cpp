#include "Cannon.h"
const double PI = 3.14159265358979323846;
bool checkDistance = true;
Cannon::Cannon()
{
	this->allCannonBalls = std::vector<CannonBall>();
	this->gravity = 9.82;
	this->tests = 0;
}
Cannon::~Cannon()
{

}
void Cannon::update(float dt)
{
	if (tests < 100)
	{
		for (int i = 0; i < allCannonBalls.size(); i++)
		{
			if (allCannonBalls[i].loading > 10)
			{
				double drag = -0.29*(1.293*PI*pow(0.010, 2) / 2);
				double totalDrag = drag / allCannonBalls[i].mass;

				//double windDrag = 1.293*0.47*pow(allCannonBalls[i].velocity, 2) * (PI*pow(allCannonBalls[i].radius,2));

				double magnusEffect = (2 * pow(PI, 2)*1.293*allCannonBalls[i].velocity*pow(allCannonBalls[i].radius, 4)*allCannonBalls[i].omega) / (2 * allCannonBalls[i].radius);
				glm::vec3 velocityVec = glm::normalize(allCannonBalls[i].speedVector) * (float)allCannonBalls[i].velocity;
				glm::mat3 testMat = glm::mat3(
					0.0, 0.0, 0.0,
					0.0, allCannonBalls[i].omega, 0.0,
					0.0, 0.0, 0.0);
				glm::vec3 magnusEffect2 = (float)(pow(PI, 2)*pow(allCannonBalls[i].radius, 3)*1.293) * (testMat * velocityVec);

				std::cout << magnusEffect2.y << std::endl;

				//Update speed for the ball
				allCannonBalls[i].speedVector.z = allCannonBalls[i].speedVector.z - (allCannonBalls[i].accelVector.z) * 0.01f;
				allCannonBalls[i].speedVector.y = allCannonBalls[i].speedVector.y + (allCannonBalls[i].accelVector.y) * 0.01f;

				//Update the total speed from both directions
				allCannonBalls[i].velocity = sqrt(pow(allCannonBalls[i].speedVector.z, 2) + pow(allCannonBalls[i].speedVector.y, 2));

				//Update the angle
				allCannonBalls[i].angle = atan(allCannonBalls[i].speedVector.y / allCannonBalls[i].speedVector.z);

				//Update the acceleration for the ball
				allCannonBalls[i].accelVector.z = totalDrag*pow(allCannonBalls[i].velocity, 2)*cos(allCannonBalls[i].angle);
				allCannonBalls[i].accelVector.y = -totalDrag*pow(allCannonBalls[i].velocity, 2)*sin(allCannonBalls[i].angle) - gravity;

				//std::cout << "vx : " << allCannonBalls[i].speedVector.z <<  std::endl;
				//std::cout << "vy : " << allCannonBalls[i].speedVector.y << std::endl;
				//std::cout << "v : " << allCannonBalls[i].velocity << std::endl;
				//std::cout << "ay : " << allCannonBalls[i].accelVector.y << std::endl;
				//std::cout << "ax : " << allCannonBalls[i].accelVector.z << std::endl;

				//Translate to new position
				allCannonBalls[i].ballModel.setModelMatrix(
					glm::translate(allCannonBalls[i].ballModel.getModelMatrix(), allCannonBalls[i].speedVector*0.01f)
				);
				//allCannonBalls[i].ballModel.rotate();
				tests++;
				if (tests == 100)
				{
					std::cout << allCannonBalls[i].ballModel.getModelMatrix()[3][2] << std::endl;
				}
			}
			else
			{
				allCannonBalls[i].loading++;
			}
		}
	}
}
void Cannon::draw(Shader shader)
{
	for (int i = 0; i < allCannonBalls.size(); i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &allCannonBalls[i].ballModel.getModelMatrix()[0][0]);
		allCannonBalls[i].ballModel.draw(shader);
	}
}
void Cannon::shoot(glm::vec3 originPos)
{
	originPos = glm::vec3(0, 0, 0);
	glm::mat4 modelMat = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		originPos.x, originPos.y+3, originPos.z, 1.0
	);
	CannonBall newBall;
	newBall.ballModel = Model("models/sphere/sphere.obj", modelMat);

	newBall.velocity = 20;
	
	newBall.angle = 30 * PI / 180;

	newBall.initialVelocity.y = 0;
	newBall.initialVelocity.z = -40;

	newBall.speedVector.y = newBall.initialVelocity.y;
	newBall.speedVector.z = newBall.initialVelocity.z * cos(newBall.angle);

	newBall.accelVector.x = 0;
	newBall.accelVector.y = -gravity;
	newBall.accelVector.z = 0;
	
	newBall.density = 7870;
	newBall.radius = 0.100;
	newBall.mass = 0.0372;

	newBall.spin = 1;
	newBall.omega = newBall.spin/9.5493;
	
	newBall.loading = 0;
	newBall.time = 0.0f;
	newBall.ballModel.setRotationMatrix(glm::rotate(glm::mat4(), glm::radians(newBall.omega), glm::vec3(1.0f, 0.0f, 0.0f)));
	allCannonBalls.push_back(newBall);
}
glm::vec3 Cannon::getMovementVector(int cannonBallNr)
{
	if(cannonBallNr >= 0 && cannonBallNr <= this->allCannonBalls.size() && !this->allCannonBalls.empty())
	{
		return this->allCannonBalls[cannonBallNr].speedVector;
	}
	else
	{
		return glm::vec3(-1, -1, -1);
	}
}