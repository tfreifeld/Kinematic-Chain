#include <Windows.h>
#include <iostream>
#include "display.h"
#include "mesh.h"
#include "shader.h"
#include "inputManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_access.hpp> 
#include "main.h"
#include <string>
#include <stb_image.h>
#define M_PI   3.14159265358979323846264338327950288

using namespace glm;

static const int DISPLAY_WIDTH = 800;
static const int DISPLAY_HEIGHT = 800;
static const float CAMERA_ANGLE = 60.0f;
static const float _NEAR = 0.1f;
static const float _FAR = 100.0f;
static GLuint textures[2];
unsigned char* chainTextureData;
unsigned char* cubeTextureData;

//Camera position
vec3 pos = vec3(0, 0, -30);
// Camera forward vector
vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
// Camera up vector
vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
// Projection matrix
mat4 P;

mat4 objectTransformations[5];
mat4 axesTransformations[5];

mat4 originAxes;

vec4 basePositions[5];
vec4 destinationLocation;

glm::vec3 backFrameColors[5];


float initialDistanceBetweenLinksParam = 2.05;
static const float CHAIN_LENGTH = 8 * initialDistanceBetweenLinksParam;
float IKdistance;
bool isIKSolverOn = false;


// -1 is background, 0 is destination cube, 1-4 is chain links
int pickedIndex = -2;

Vertex vertices[] =
{
	// pos, texCoord, normal, color

	// Blue
	Vertex(glm::vec3(-1, -1, -1), glm::vec2(1, 0), glm::vec3(0, 0, -1),glm::vec3(0, 0, 1)),
	Vertex(glm::vec3(-1, 1, -1), glm::vec2(0, 0), glm::vec3(0, 0, -1),glm::vec3(0, 0, 1)),
	Vertex(glm::vec3(1, 1, -1), glm::vec2(0, 1), glm::vec3(0, 0, -1),glm::vec3(0, 0, 1)),
	Vertex(glm::vec3(1, -1, -1), glm::vec2(1, 1), glm::vec3(0, 0, -1),glm::vec3(0, 0, 1)),

	// Magenta
	Vertex(glm::vec3(-1, -1, 1), glm::vec2(1, 0), glm::vec3(0, 0, 1),glm::vec3(1, 0, 1)),
	Vertex(glm::vec3(-1, 1, 1), glm::vec2(0, 0), glm::vec3(0, 0, 1),glm::vec3(1, 0, 1)),
	Vertex(glm::vec3(1, 1, 1), glm::vec2(0, 1), glm::vec3(0, 0, 1),glm::vec3(1, 0, 1)),
	Vertex(glm::vec3(1, -1, 1), glm::vec2(1, 1), glm::vec3(0, 0, 1),glm::vec3(1, 0, 1)),

	// Green
	Vertex(glm::vec3(-1, -1, -1), glm::vec2(0, 1), glm::vec3(0, -1, 0),glm::vec3(0, 1, 0)),
	Vertex(glm::vec3(-1, -1, 1), glm::vec2(1, 1), glm::vec3(0, -1, 0),glm::vec3(0, 1, 0)),
	Vertex(glm::vec3(1, -1, 1), glm::vec2(1, 0), glm::vec3(0, -1, 0),glm::vec3(0, 1, 0)),
	Vertex(glm::vec3(1, -1, -1), glm::vec2(0, 0), glm::vec3(0, -1, 0),glm::vec3(0, 1, 0)),

	// Yellow
	Vertex(glm::vec3(-1, 1, -1), glm::vec2(0, 1), glm::vec3(0, 1, 0),glm::vec3(1, 1, 0)),
	Vertex(glm::vec3(-1, 1, 1), glm::vec2(1, 1), glm::vec3(0, 1, 0),glm::vec3(1, 1, 0)),
	Vertex(glm::vec3(1, 1, 1), glm::vec2(1, 0), glm::vec3(0, 1, 0),glm::vec3(1, 1, 0)),
	Vertex(glm::vec3(1, 1, -1), glm::vec2(0, 0), glm::vec3(0, 1, 0),glm::vec3(1, 1, 0)),

	// Red
	Vertex(glm::vec3(-1, -1, -1), glm::vec2(1, 1), glm::vec3(-1, 0, 0),glm::vec3(1, 0, 0)),
	Vertex(glm::vec3(-1, -1, 1), glm::vec2(1, 0), glm::vec3(-1, 0, 0),glm::vec3(1, 0, 0)),
	Vertex(glm::vec3(-1, 1, 1), glm::vec2(0, 0), glm::vec3(-1, 0, 0),glm::vec3(1, 0, 0)),
	Vertex(glm::vec3(-1, 1, -1), glm::vec2(0, 1), glm::vec3(-1, 0, 0),glm::vec3(1, 0, 0)),

	// Cyan
	Vertex(glm::vec3(1, -1, -1), glm::vec2(1, 1), glm::vec3(1, 0, 0),glm::vec3(0, 1, 1)),
	Vertex(glm::vec3(1, -1, 1), glm::vec2(1, 0), glm::vec3(1, 0, 0),glm::vec3(0, 1, 1)),
	Vertex(glm::vec3(1, 1, 1), glm::vec2(0, 0), glm::vec3(1, 0, 0),glm::vec3(0, 1, 1)),
	Vertex(glm::vec3(1, 1, -1), glm::vec2(0, 1), glm::vec3(1, 0, 0),glm::vec3(0, 1, 1))
};

unsigned int indices[] = { 0, 1, 2,
0, 2, 3,

6, 5, 4,
7, 6, 4,

10, 9, 8,
11, 10, 8,

12, 13, 14,
12, 14, 15,

16, 17, 18,
16, 18, 19,

22, 21, 20,
23, 22, 20
};



/* Initialize axes*/
void initializeAxes() {

	//This will remain blank for convenience purposes
	axesTransformations[0] = mat4(0);

	originAxes = glm::rotate(mat4(1), 90.0f, vec3(-1, 0, 0));


	basePositions[0] = vec4(0, 0, 0, 1);
	basePositions[1] = vec4(0, 2 * initialDistanceBetweenLinksParam, 0, 1);
	basePositions[2] = vec4(0, 4 * initialDistanceBetweenLinksParam, 0, 1);
	basePositions[3] = vec4(0, 6 * initialDistanceBetweenLinksParam, 0, 1);
	basePositions[4] = vec4(0, 8 * initialDistanceBetweenLinksParam, 0, 1);

	for (int i = 1; i <= 4; i++) {
		axesTransformations[i] = glm::rotate(mat4(1), 90.0f, vec3(-1, 0, 0));
	}

	// Second link in chain
	axesTransformations[2] = glm::translate(axesTransformations[2], vec3(0, 0, 2 * initialDistanceBetweenLinksParam));
	// Third link in chain
	axesTransformations[3] = glm::translate(axesTransformations[3], vec3(0, 0, 4 * initialDistanceBetweenLinksParam));
	// Fourth link in chain
	axesTransformations[4] = glm::translate(axesTransformations[4], vec3(0, 0, 6 * initialDistanceBetweenLinksParam));

}

/* Initialize back frame colors*/
void initializeBackFrameColors() {

	backFrameColors[0] = vec3(1.0, 0.0, 0.0);
	backFrameColors[1] = vec3(1.0, 1.0, 0.0);
	backFrameColors[2] = vec3(1.0, 0.0, 1.0);
	backFrameColors[3] = vec3(0.0, 1.0, 1.0);
	backFrameColors[4] = vec3(0.0, 0.0, 1.0);
}

/* Initialize scene objects*/
void initializeObjects() {

	// Destination cube
	objectTransformations[0] = glm::rotate(mat4(1), 90.0f, vec3(-1, 0, 0)) * glm::translate(mat4(1), vec3(8, 0, 1));
	destinationLocation = glm::rotate(mat4(1), 90.0f, vec3(-1, 0, 0)) * vec4(8, 0, 1, 1);

	/* Chain */

	// Scale and rotate
	for (int i = 1; i <= 4; i++)
		objectTransformations[i] = glm::rotate(mat4(1), 90.0f, vec3(-1, 0, 0)) * glm::scale(mat4(1), vec3(1, 1, 2));

	// Translate first link in chain
	objectTransformations[1] = glm::translate(objectTransformations[1], vec3(0, 0, 0.5 * initialDistanceBetweenLinksParam));
	// Translate second link in chain
	objectTransformations[2] = glm::translate(objectTransformations[2], vec3(0, 0, 1.5 * initialDistanceBetweenLinksParam));
	// Translate third link in chain
	objectTransformations[3] = glm::translate(objectTransformations[3], vec3(0, 0, 2.5 * initialDistanceBetweenLinksParam));
	// Translate fourth link in chain
	objectTransformations[4] = glm::translate(objectTransformations[4], vec3(0, 0, 3.5 * initialDistanceBetweenLinksParam));

	initializeBackFrameColors();
	initializeAxes();

}



void initializeTextures() {

	int width;
	int height;
	std::string fileName;
	int numComponents = 4;

	width = 1024;
	height = 512;
	fileName = "chain.JPG";
	chainTextureData = stbi_load((fileName).c_str(), &width, &height,
		&numComponents, 4);
	if (chainTextureData == nullptr)
		printf("Failed to load texture0");

	width = 512;
	height = 256;
	fileName = "cube.JPG";
	cubeTextureData = stbi_load((fileName).c_str(), &width, &height,
		&numComponents, 4);

	if (cubeTextureData == nullptr)
		printf("Failed to load texture1");



	glGenTextures(2, textures);


}

/*Draw the cubes, called from glfw loop*/
void drawCubes(int i, Shader &shader)
{

	int textureIndex;
	if (i == 0)
		textureIndex = 1;
	else
		textureIndex = 0;



	// Cubes ModelViewProjection matrix
	mat4 MVP = P * objectTransformations[i];
	shader.Update(MVP, objectTransformations[i], backFrameColors[i], textureIndex);
	Mesh mesh(vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0]));
	mesh.Draw();

}

/*Draws the axes*/
void drawAxes(int i, Shader &shader)
{


	vec3 colorTemp = backFrameColors[i];

	// Axes ModelViewProjection matrix
	mat4 MVP = P * axesTransformations[i];

	shader.Update(MVP, axesTransformations[i], colorTemp, 0);

	if (i == 0) { // Scene axes
		colorTemp = vec3(0.0, 0.0, 0.0);
		MVP = P * originAxes;
		shader.Update(MVP, originAxes, colorTemp, 0);

		glBegin(GL_LINES);
		glVertex3f(-10, 0, 0);
		glVertex3f(10, 0, 0);
		glVertex3f(0, 0, -10);
		glVertex3f(0, 0, 10);
		glEnd();
	}

	// Chain
	else if (i != 0) {
		glBegin(GL_LINES);
		glVertex3f(-5, 0, 0);
		glVertex3f(5, 0, 0);
		glVertex3f(0, 0, -5);
		glVertex3f(0, 0, 5);
		glEnd();
	}
}

/* Identify the object picked using the mouse cursor */
void identifyPickedObject(glm::vec3 &pickedColor)
{

	if (pickedColor == vec3(1, 0, 0))
		pickedIndex = 0; // Destination cube
	else if (pickedColor == vec3(1, 1, 0))
		pickedIndex = 1;
	else if (pickedColor == vec3(1, 0, 1))
		pickedIndex = 2;
	else if (pickedColor == vec3(0, 1, 1))
		pickedIndex = 3;
	else if (pickedColor == vec3(0, 0, 1))
		pickedIndex = 4;
	else if (pickedColor == vec3(1, 1, 1))
		pickedIndex = -1; // Background

}

void InputManager::processMouse(double xpos, double ypos) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Shader shader("./res/shaders/basicShaderBackFrame");
	shader.Bind();
	draw(shader);


	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLfloat pixels[3];
	glReadPixels(xpos, viewport[3] - ypos, 1, 1, GL_RGB, GL_FLOAT, pixels);

	vec3 pickedColor = vec3(pixels[0], pixels[1], pixels[2]);

	identifyPickedObject(pickedColor);

}

/* Update base position after transformation*/
void updateBasePositions(int beginIndex, float deltaX, float deltaY) {

	mat4 translationMat = glm::translate(vec3(-basePositions[beginIndex - 1]));
	mat4 inverseTranslationMat = glm::inverse(translationMat);

	for (int i = beginIndex; i <= 4; i++) {
		basePositions[i] = translationMat * basePositions[i];
		basePositions[i] = glm::rotate(mat4(1), deltaY / 2, glm::cross(up, forward)) * basePositions[i];
		basePositions[i] = glm::rotate(mat4(1), deltaX / 2, -up) * basePositions[i];
		basePositions[i] = inverseTranslationMat * basePositions[i];
	}

}

/* Update base position after transformation using IK*/
void updateBasePositionsForIK(int beginIndex, float alpha, vec3 rotationVec) {

	mat4 translationMat = glm::translate(vec3(-basePositions[beginIndex - 1]));
	mat4 inverseTranslationMat = glm::inverse(translationMat);

	for (int i = beginIndex; i <= 4; i++) {
		basePositions[i] = translationMat * basePositions[i];
		basePositions[i] = glm::rotate(alpha, rotationVec) * basePositions[i];
		basePositions[i] = inverseTranslationMat * basePositions[i];
	}

}

void rotateLinksEuler(int beginIndex, float deltaX, float deltaY, int basePositionIndex)
{

	mat4 translationMat = glm::translate(vec3(-basePositions[basePositionIndex]));
	mat4 inverseTranslationMat = glm::inverse(translationMat);

	for (int i = beginIndex; i < 5; i++) {

		objectTransformations[i] = translationMat * objectTransformations[i];
		axesTransformations[i] = translationMat * axesTransformations[i];


		objectTransformations[i] = glm::rotate(mat4(1), deltaY / 2, glm::cross(up, forward)) * objectTransformations[i];
		objectTransformations[i] = glm::rotate(mat4(1), deltaX / 2, -up) *  objectTransformations[i];
		axesTransformations[i] = glm::rotate(mat4(1), deltaY / 2, glm::cross(up, forward)) * axesTransformations[i];
		axesTransformations[i] = glm::rotate(mat4(1), deltaX / 2, -up) *  axesTransformations[i];


		objectTransformations[i] = inverseTranslationMat * objectTransformations[i];
		axesTransformations[i] = inverseTranslationMat * axesTransformations[i];

	}
}

void rotateLinksForIK(int beginIndex, float alpha, vec3 rotationVec)
{

	mat4 translationMat = glm::translate(vec3(-basePositions[beginIndex - 1]));
	mat4 inverseTranslationMat = glm::inverse(translationMat);

	for (int i = beginIndex; i < 5; i++) {

		objectTransformations[i] = translationMat * objectTransformations[i];
		axesTransformations[i] = translationMat * axesTransformations[i];

		objectTransformations[i] = glm::rotate(alpha, rotationVec) * objectTransformations[i];
		axesTransformations[i] = glm::rotate(alpha, rotationVec) * axesTransformations[i];

		objectTransformations[i] = inverseTranslationMat * objectTransformations[i];
		axesTransformations[i] = inverseTranslationMat * axesTransformations[i];

	}
}

/* Handle rotations of picked object, or scene in case background was picked */
void InputManager::rotate(float deltaX, float deltaY) {

	// background picked
	if (pickedIndex == -1) {

		originAxes = glm::rotate(mat4(1), deltaY / 2, glm::cross(up, forward)) * originAxes;
		originAxes = glm::rotate(mat4(1), deltaX / 2, -up) *  originAxes;

		for (int i = 0; i < 5; i++) {

			objectTransformations[i] = glm::rotate(mat4(1), deltaY / 2, glm::cross(up, forward)) * objectTransformations[i];
			objectTransformations[i] = glm::rotate(mat4(1), deltaX / 2, -up) * objectTransformations[i];
			axesTransformations[i] = glm::rotate(mat4(1), deltaY / 2, glm::cross(up, forward)) * axesTransformations[i];
			axesTransformations[i] = glm::rotate(mat4(1), deltaX / 2, -up) * axesTransformations[i];
			basePositions[i] = glm::rotate(mat4(1), deltaY / 2, glm::cross(up, forward)) * basePositions[i];
			basePositions[i] = glm::rotate(mat4(1), deltaX / 2, -up) * basePositions[i];
		}


		destinationLocation = glm::rotate(mat4(1), deltaY / 2, glm::cross(up, forward)) * destinationLocation;
		destinationLocation = glm::rotate(mat4(1), deltaX / 2, -up) * destinationLocation;
	}


	// Entire chain
	else if (pickedIndex == 1) {

		rotateLinksEuler(1, deltaX, deltaY, 0);
		updateBasePositions(1, deltaX, deltaY);
	}

	// Second, third and top links
	else if (pickedIndex == 2) {

		rotateLinksEuler(2, deltaX, deltaY, 1);
		updateBasePositions(2, deltaX, deltaY);

	}

	// Third and top links
	else if (pickedIndex == 3) {

		rotateLinksEuler(3, deltaX, deltaY, 2);
		updateBasePositions(3, deltaX, deltaY);
	}

	// Top link
	else if (pickedIndex == 4) {

		rotateLinksEuler(4, deltaX, deltaY, 3);
		updateBasePositions(4, deltaX, deltaY);

	}

}

/* Handle translation of destination cube or chain in parallel to the screen */
void InputManager::translate(float deltaX, float deltaY, float newXposition, float newYposition) {

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLfloat depth[1];
	glReadPixels(newXposition, viewport[3] - newYposition, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, depth);


	float z = _FAR + *depth * (_NEAR - _FAR);
	float transX = (deltaX / 2) / (float)(viewport[2]) * _NEAR * 2.0 *
		glm::tan(CAMERA_ANGLE * M_PI / 360.0) * (_FAR / z);
	float transY = (deltaY / 2) / (float)(viewport[3]) * _NEAR * 2.0 *
		glm::tan(CAMERA_ANGLE * M_PI / 360.0) * (_FAR / z);


	// Destination cube
	if (pickedIndex == 0) {

		objectTransformations[0] = glm::translate(vec3(transX, transY, 0))
			* objectTransformations[0];
		destinationLocation = glm::translate(vec3(transX, transY, 0))
			* destinationLocation;
	}
	// Chain
	else if (pickedIndex >= 1 && pickedIndex <= 5) {

		for (int i = 1; i <= 4; i++) {
			objectTransformations[i] = glm::translate(vec3(transX, transY, 0)) * objectTransformations[i];
			axesTransformations[i] = glm::translate(vec3(transX, transY, 0)) * axesTransformations[i];
		}

		for (int i = 0; i < 5; i++)
			basePositions[i] = glm::translate(vec3(transX, transY, 0)) * basePositions[i];

	}

}

/* Handle zooming in or out of destination cube or chain*/
void InputManager::zoom(float yoffset) {

	// Destination cube
	if (pickedIndex == 0) {
		objectTransformations[0] = glm::translate(vec3(0, 0, yoffset)) * objectTransformations[0];
		destinationLocation = glm::translate(vec3(0, 0, yoffset)) * destinationLocation;
	}
	// Entire chain
	else if (pickedIndex >= 1 && pickedIndex <= 5) {


		for (int i = 1; i <= 4; i++) {
			objectTransformations[i] = glm::translate(vec3(0, 0, yoffset)) * objectTransformations[i];
			axesTransformations[i] = glm::translate(vec3(0, 0, yoffset)) * axesTransformations[i];
		}

		for (int i = 0; i < 5; i++)
			basePositions[i] = glm::translate(vec3(0, 0, yoffset)) * basePositions[i];
	}
}

/* Turns IKsolver on or off */
void InputManager::turnOnOffIKSolver() {

	if (isIKSolverOn)
		isIKSolverOn = false;
	else {
		if (glm::length(destinationLocation - basePositions[0]) <= CHAIN_LENGTH) {
			isIKSolverOn = true;
		}
		else
			std::cout << "cannot reach" << std::endl;
	}
}

void IKsolver()
{
	for (int i = 3; i >= 0; i--) {

		vec3 baseToEndpointVec = glm::normalize(vec3(basePositions[4] - basePositions[i]));
		vec3 baseToDestinationVec = glm::normalize(vec3(destinationLocation - basePositions[i]));

		float cosAlpha = glm::clamp(glm::dot(baseToDestinationVec, baseToEndpointVec), -1.0f, 1.0f);
		float alpha = glm::acos(cosAlpha);

		rotateLinksForIK(i + 1, alpha, glm::cross(baseToEndpointVec, baseToDestinationVec));

		updateBasePositionsForIK(i + 1, alpha, glm::cross(baseToEndpointVec, baseToDestinationVec));

		vec3 distanceVec = vec3(destinationLocation - basePositions[4]);
		IKdistance = glm::length(distanceVec);


	}
	std::string message("distance: ");

	if (IKdistance < 0.1) {
		isIKSolverOn = false;
		message = "destination reached! distance: ";
	}

	std::cout << message << IKdistance << std::endl;

}

void draw(Shader &shader)
{
	if (isIKSolverOn) {

		IKsolver();
	}
	for (int i = 0; i < 5; i++) {

		drawCubes(i, shader);
		drawAxes(i, shader);

	}
}

int main(int argc, char** argv)
{
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");

	Mesh mesh(vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0]));

	Shader shader("./res/shaders/basicShader");

	//Projection matrix
	P = glm::perspective(CAMERA_ANGLE, (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, _NEAR, _FAR);
	//ViewProjection matrix
	P = P * glm::lookAt(pos, pos + forward, up);

	// Set lines width
	glLineWidth(2.5);

	initializeObjects();
	initializeTextures();


	glfwSetKeyCallback(display.m_window, InputManager::key_callback);
	glfwSetCursorPosCallback(display.m_window, InputManager::cursor_position_callback);
	glfwSetMouseButtonCallback(display.m_window, InputManager::mouse_button_callback);
	glfwSetScrollCallback(display.m_window, InputManager::mouse_scroll_callback);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, chainTextureData);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, cubeTextureData);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	while (!glfwWindowShouldClose(display.m_window))
	{
		shader.Bind();
		display.Clear(1.0f, 1.0f, 1.0f, 1.0f);
		Sleep(3);
		draw(shader);

		display.SwapBuffers();

		glfwPollEvents();
	}

	return 0;
}





