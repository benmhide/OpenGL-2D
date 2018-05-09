#pragma once
#include "Square.h"
#include "Texture.h"

//Player class - inherits from square
class Player : public Square
{
public:
	//Constructors
	Player();

	//Player member variables for:
	//position, scale, colour and rotation
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 colour;
	GLfloat rotation;

	//Player texture
	Texture texture;

	//Player lives
	int lives;
	int score;
};