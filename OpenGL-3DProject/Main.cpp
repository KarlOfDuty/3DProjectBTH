#include <GL\glew.h>
#include <GL\GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <SFML\Window.hpp>
#include <SFML\OpenGL.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include <AntTweakBar.h>
#include <SOIL.h>
#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#pragma comment(lib, "opengl32.lib")
//Initial resolutions
const int RESOLUTION_WIDTH = sf::VideoMode::getDesktopMode().width;
const int RESOLUTION_HEIGHT = sf::VideoMode::getDesktopMode().height;
const int windowWidth = 800;
const int windowHeight = 600;
bool debug = false;
float stuff = 4345435.0;
//Camera
Camera playerCamera;
//gBuffer
GLuint gBuffer;
//gBuffer Shaders
Shader shaderGeometryPass;
Shader shaderLightningPass;
//gBuffer Textures
GLuint gPosition, gNormal, gAlbedoSpec;
//Quad VAO and VBO
GLuint quadVAO = 0;
GLuint quadVBO;

//Temp values for textures and all the lights
GLuint diffuseTexture;
GLuint specularTexture;
const GLuint NR_LIGHTS = 32;
std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;

//Timing control for controls and camera
sf::Clock deltaClock;
sf::Time deltaTime;
//Matrices
glm::mat4 viewMatrix = glm::lookAt(
	glm::vec3(0, 0, 2),
	glm::vec3(0, 0, 0),
	glm::vec3(0, 1, 0));
glm::mat4 projectionMatrix = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 20.0f);
//All models in the program
std::vector<Model> allModels;
//AntTweakBar
TwBar *debugInterface;

bool testIntersection(
	glm::vec3 ray_origin,
	glm::vec3 ray_direction,
	glm::vec3 aabb_min,
	glm::vec3 aabb_max,
	glm::mat4 ModelMatrix,
	float& intersection_distance
)
{
	// Intersection method from Real-Time Rendering and Essential Mathematics for Games
	
	float tMin = 0.0f;
	float tMax = 100000.0f;

	glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

	glm::vec3 delta = OBBposition_worldspace - ray_origin;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
		float e = glm::dot(xaxis, delta);
		float f = glm::dot(ray_direction, xaxis);

		if (fabs(f) > 0.001f) { // Standard case

			float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
			float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
											 // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

											 // We want t1 to represent the nearest intersection, 
											 // so if it's not the case, invert t1 and t2
			if (t1>t2) {
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax)
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin)
				tMin = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (tMax < tMin)
				return false;

		}
		else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
		float e = glm::dot(yaxis, delta);
		float f = glm::dot(ray_direction, yaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.y) / f;
			float t2 = (e + aabb_max.y) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
		float e = glm::dot(zaxis, delta);
		float f = glm::dot(ray_direction, zaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.z) / f;
			float t2 = (e + aabb_max.z) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;
}

void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

}

void CreateGBuffer()
{
	shaderGeometryPass = Shader("gBufferGeometryVertex.glsl", "gBufferGeometryFragment.glsl");
	shaderLightningPass = Shader("gBufferLightningVertex.glsl", "gBufferLightningFragment.glsl");

	// Set samplers
	shaderLightningPass.use();
	glUniform1i(glGetUniformLocation(shaderLightningPass.program, "gPosition"), 0);
	glUniform1i(glGetUniformLocation(shaderLightningPass.program, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(shaderLightningPass.program, "gAlbedoSpec"), 2);

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// - Position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 800, 600, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - Normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 800, 600, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - Color + Specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void createModels()
{
	//Create the models and store them in the vector of all models
	
	/*allModels.push_back(Model("models/nanosuit/nanosuit.obj", {
		0.2, 0.0, 0.0, 0.0,
		0.0, 0.2, 0.0, 0.0,
		0.0, 0.0, 0.2, 0.0,
		0.0, -0.7, 0.0, 1.0 }));
	*/
	
	allModels.push_back(Model("models/cube/cube.obj", {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-1.0, 0.0, 0.0, 1.0 }));

	allModels.push_back(Model("models/cube/cube.obj", {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		1.0, 0.0, 0.0, 1.0 }));

	//Some light with random values
	std::srand(std::time(0));
	std::srand(13);
	for (int i = 0; i < NR_LIGHTS; i++)
	{
		GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// Also calculate random color
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	//Rotation for all models
	glm::mat4 rotation = glm::rotate(glm::mat4(), glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	for (int i = 0; i < allModels.size(); i++)
	{
		allModels[i].setRotationMatrix(rotation);
	}
}

void setUpTweakBar()
{
	debugInterface = TwNewBar("Debug Interface");
	TwAddVarRW(debugInterface, "Some stuff", TW_TYPE_FLOAT, &stuff, "");
}

void update(sf::Window &window)
{
	//Controls update timings
	deltaTime = deltaClock.restart();
	viewMatrix = playerCamera.Update(deltaTime.asSeconds(), window.hasFocus());

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
	{
		window.setMouseCursorVisible(true);
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			float x = (2.0f * sf::Mouse::getPosition(window).x) / window.getSize().x - 1.0f;
			float y = 1.0f - (2.0f * sf::Mouse::getPosition(window).y / window.getSize().y);
			float z = 1.0f;
			glm::vec3 ray_nds = glm::vec3(x, y, z);
			glm::vec4 ray_clip = glm::vec4(ray_nds.x,ray_nds.y,-1.0, 1.0);
			glm::vec4 ray_eye = glm::inverse(projectionMatrix) * ray_clip;
			ray_eye = glm::vec4(ray_eye.x,ray_eye.y, -1.0, 0.0);
			glm::vec3 ray_wor = glm::vec3((glm::inverse(viewMatrix)*ray_eye));
			//std::cout << "ray_wor : X = " << ray_wor.x << " : Y = " << ray_wor.y << " : Z " << ray_wor.z << std::endl;

			glm::vec3 aabb_min(-0.5f, -0.5f, -0.5f);
			glm::vec3 aabb_max(0.5f, 0.5f, 0.5f);
			float distance = 100;
			//std::cout << "ORIGIN = " << playerCamera.getCameraPos().x << ", " << playerCamera.getCameraPos().y << ", " << playerCamera.getCameraPos().z << std::endl;
			for (int i = 0; i < allModels.size(); i++)
			{
				glm::mat4 ModelMatrix = allModels[i].getModelMatrix();
				ModelMatrix *= allModels[i].getRotationMatrix();
				std::cout << "TESTING INTERSECTIONS WITH I = " << i << std::endl;
				if (testIntersection(playerCamera.getCameraPos(), ray_wor, aabb_min, aabb_max, ModelMatrix, distance))
				{
					std::cout << "INTERSECTS WITH MOUSE" << std::endl;
				}
				else
				{
					std::cout << "DOES NOT INTERSECT WITH MOUSE" << std::endl;
				}
			}
		}
	}
	else
	{
		window.setMouseCursorVisible(false);
	}
	for (int i = 0; i < allModels.size(); i++)
	{
		allModels[i].rotate();
	}
}

void render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderGeometryPass.use();

	GLint viewID = glGetUniformLocation(shaderGeometryPass.program, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
	GLint projectionID = glGetUniformLocation(shaderGeometryPass.program, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projectionMatrix[0][0]);
	for (int i = 0; i < allModels.size(); i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass.program, "model"), 1, GL_FALSE, &allModels[i].getModelMatrix()[0][0]);
		allModels.at(i).draw(shaderGeometryPass);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderLightningPass.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

	for (GLuint i = 0; i < lightPositions.size(); i++)
	{
		glUniform3fv(glGetUniformLocation(shaderLightningPass.program, ("lights[" + std::to_string(i) + "].Position").c_str()), 1, &lightPositions[i][0]);
		glUniform3fv(glGetUniformLocation(shaderLightningPass.program, ("lights[" + std::to_string(i) + "].Color").c_str()), 1, &lightColors[i][0]);
		// Update attenuation parameters and calculate radius
		const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const GLfloat linear = 0.7;
		const GLfloat quadratic = 1.8;
		glUniform1f(glGetUniformLocation(shaderLightningPass.program, ("lights[" + std::to_string(i) + "].Linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(shaderLightningPass.program, ("lights[" + std::to_string(i) + "].Quadratic").c_str()), quadratic);
	}

	glUniform3fv(glGetUniformLocation(shaderLightningPass.program, "viewPos"), 1, &playerCamera.getCameraPos()[0]);

	RenderQuad();
}

int main()
{
	//Create the window
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 2;
	sf::Window window(sf::VideoMode(windowWidth, windowHeight), "OpenGL", sf::Style::Default, settings);
	//Initialise AntTweakBar
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(windowWidth, windowHeight);
	setUpTweakBar();
	//V-Sync
	window.setVerticalSyncEnabled(true);
	//Disable cursor
	window.setMouseCursorVisible(false);
	//Load resources, initialize the OpenGL states, ...
	glewInit();
	//Enables depth test so vertices are drawn in the correct order
	glEnable(GL_DEPTH_TEST);
	//Create gBuffer
	CreateGBuffer();
	//Create models
	createModels();
	//Main loop
	bool running = true;
	while (running)
	{
		//Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				//End the program
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				//Resize the viewport when the window is resized
				glViewport(0, 0, event.size.width, event.size.height);
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
					running = false;
				}
			}
		}
		update(window);
		render();
		if(debug)TwDraw();
		//End the current frame (internally swaps the front and back buffers)
		window.display();
	}
	return 0;
}
