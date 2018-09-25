#pragma once

void drawCubes(int i, Shader &shader);

void drawAxes( int i, Shader &shader);

void IKsolver();

void draw(Shader &shader);

void rotateLinksEuler(int i, float deltaX, float deltaY, int baseIndex);

void identifyPickedObject(glm::vec3 &pickedColor);
