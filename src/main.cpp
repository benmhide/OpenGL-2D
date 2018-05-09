#include "Includes.h"
#include "Player.h"
#include "Ball.h"
#include "Background.h"
#include "Square.h"
#include "Brick.h"
#include "GameObject.h"
#include "Texture.h"
#include "GLWindow.h"
#include "GLProgram.h"

//Game states
enum class GameState { Play, Win, Lose, Exit };
GameState state;

//Game elements
GLWindow window;
GLProgram glProgram;

//Screen dimensions
GLint screenWidth;
GLint screenHeight;
GLint boundLeft;
GLint boundRight;
GLint boundBottom;
GLint boundTop;
GLfloat aspectRatio;

//Tranform matrices 
glm::mat4 modelTranslate;
glm::mat4 modelScale;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

//Background and parallax
Background background;
Background parallaxForeground;
Background parallaxMidground;
glm::mat4 backgroundScale;
glm::mat4 backgroundTranslate;
Background parallaxBackground;
glm::mat4 parallaxForeTranslate;
glm::mat4 parallaxForeScale;
glm::mat4 parallaxMidTranslate;
glm::mat4 parallaxMidScale;
glm::mat4 parallaxBackTranslate;
glm::mat4 parallaxBackScale;

//Ball
Ball ball;
glm::vec3 ballVelocity = { -3.0f, -5.5f, 0.0f };
bool stuckToPaddle;

//Player
Player player;
glm::vec3 playerVelocity = { 10.0f, 0.0f, 0.0f };
bool moveLeft;
bool moveRight;
GLint mouseX;
GLint mouseY;
int score;

//Bricks
const GLint numbBricksHigh = 5;
const GLint numbBricksWide = 10;
Brick bricks[numbBricksHigh][numbBricksWide];

//Game objects
GameObject lives;
GameObject win;
GameObject gameover;
GameObject leftBound;
GameObject rightBound;
GameObject topBound;
GameObject powerUp;
GLfloat offset;
bool gameWon;
std::vector<Texture> scoreText;
std::vector<GameObject> scoreObject;
int powerUps;

//Game functions
void init();
void buildLevel();
void loadScore();
void setScore();
void gameLoop();
bool wonGame();

//Power-up functions
void setPowerUp();
void powerUpActive(int x, int y);
void maxHits();

//Brick functions
void setCrackedBrick(int x, int y);
void setDeadBrick(int x, int y);
void setDyingBrick(int x, int y);

//Collision functions
bool collisionDetection(Ball &ball, Brick &brick);
bool collisionDetection(Ball &ball, Player &player);
bool collisionDetection(GameObject &powerUp, Player &player);

//Input function
void input();

//Update functions
void update();
void updateBall();
void updatePlayer();

//Render functions
void startRender();
void render();
void resetMatrices();
void renderObject(glm::mat4 translation, glm::mat4 scale, glm::vec3 colour, Texture &texture);
void endRender();

//Aspect ratio and fullscreen
void maintainAspectRatio();
void toggleFullScreen();
bool fullscreen;

//Main program
int main(int argc, char* argv[])
{
	//Initialisation
	init();

	//Game loop
	gameLoop();

	//End game
	return 0;
}

//Initialisation
void init()
{
	//Window
	window.init("Benjamin Hide // HID16605093 // Graphics // CGP2012M-1718", screenWidth, screenHeight);

	//Game state
	state = GameState::Play;
	bool gameWon = false;
	powerUps = 0;

	//Player score
	score = 0;

	//Ball stuck to paddle
	stuckToPaddle = true;

	//Player controls
	moveLeft = false;
	moveRight = false;
	mouseX = 0;
	mouseY = 0;

	//Game bounds
	boundLeft = 0;
	boundRight = screenWidth;
	boundTop = 0;
	boundBottom = screenHeight;
	offset = 0.75f;

	//Initialise shaders
	glProgram.init("..//shaders//shader_projection.vert", "..//shaders//shader_projection.frag");

	//Set aspect ratio
	aspectRatio = (GLfloat)screenHeight / (GLfloat)screenWidth;
	fullscreen = false;

	//Orthographic projection and view matrices
	projectionMatrix = glm::ortho(0.0f, (GLfloat)screenWidth, (GLfloat)screenHeight, 0.0f, -1.0f, 1.0f);
	viewMatrix = glm::mat4(1.0f);

	//Use the shader program
	glProgram.use();


	//Backgrounds and parallax
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//Set background
		background.setBuffers();
		background.scale = glm::vec3((GLfloat)screenWidth + 200.0f, (GLfloat)screenHeight, 1.0f);
		background.position = glm::vec3(-100.0f, 0.0f, 0.0f);


		//Background texture
		background.texture.load("..//content//sky2.png");
		background.texture.setBuffers();

		//Position and scale matrices
		backgroundTranslate = glm::translate(backgroundTranslate, background.position);
		backgroundScale = glm::scale(backgroundScale, background.scale);
	}
	{
		//Set parallax background
		parallaxBackground.setBuffers();
		parallaxBackground.scale = glm::vec3((GLfloat)screenWidth - 100.0f, (GLfloat)(screenHeight / 2.5), 1.0f);
		parallaxBackground.position = glm::vec3(50.0f, 0.0f, 0.0f);

		//Parallax texture
		parallaxBackground.texture.load("..//content//clouds1.png");
		parallaxBackground.texture.setBuffers();

		//Position and scale matrices
		parallaxBackTranslate = glm::translate(parallaxBackTranslate, parallaxBackground.position);
		parallaxBackScale = glm::scale(parallaxBackScale, parallaxBackground.scale);
	}
	{
		//Set parallax midground
		parallaxMidground.setBuffers();
		parallaxMidground.scale = glm::vec3((GLfloat)screenWidth, (GLfloat)(screenHeight / 1.75), 1.0f);
		parallaxMidground.position = glm::vec3(0.0f, 50.0f, 0.0f);

		//Parallax texture
		parallaxMidground.texture.load("..//content//clouds2.png");
		parallaxMidground.texture.setBuffers();

		//Position and scale matrices
		parallaxMidTranslate = glm::translate(parallaxMidTranslate, parallaxMidground.position);
		parallaxMidScale = glm::scale(parallaxMidScale, parallaxMidground.scale);
	}
	{
		//Set parallax foreground
		parallaxForeground.setBuffers();
		parallaxForeground.scale = glm::vec3((GLfloat)screenWidth + 600.0f, (GLfloat)(screenHeight / 1.25), 1.0f);
		parallaxForeground.position = glm::vec3(-300.0f, (GLfloat)(screenHeight / 2), 0.0f);

		//Parallax texture
		parallaxForeground.texture.load("..//content//clouds1.png");
		parallaxForeground.texture.setBuffers();

		//Position and scale matrices
		parallaxForeTranslate = glm::translate(parallaxForeTranslate, parallaxForeground.position);
		parallaxForeScale = glm::scale(parallaxForeScale, parallaxForeground.scale);
	}

	//Game bounds
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//Set bound
		leftBound.setBuffers();
		leftBound.scale = glm::vec3(30.0f, (GLfloat)screenHeight, 1.0f);
		leftBound.position = glm::vec3(-20.0f, 0.0f, 0.0f);

		//Bound texture
		leftBound.texture.load("..//content//sideLeft.png");
		leftBound.texture.setBuffers();
		leftBound.colour = { 1.0f, 1.0f, 0.0f };
	}
	{
		//Set bound
		rightBound.setBuffers();
		rightBound.scale = glm::vec3(30.0f, (GLfloat)screenHeight, 1.0f);
		rightBound.position = glm::vec3((GLfloat)screenWidth - 10.0f, 0.0f, 0.0f);

		//Bound texture
		rightBound.texture.load("..//content//sideRight.png");
		rightBound.texture.setBuffers();
		rightBound.colour = { 1.0f, 1.0f, 0.0f };
	}
	{
		//Set bound
		topBound.setBuffers();
		topBound.scale = glm::vec3((GLfloat)screenWidth - 20.0f, 20.0f, 1.0f);
		topBound.position = glm::vec3(10.0f, -10.0f, 0.0f);

		//Bound texture
		topBound.texture.load("..//content//topSide.png");
		topBound.texture.setBuffers();
		topBound.colour = { 1.0f, 1.0f, 0.0f };
	}

	//Player
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//Set player
		player.setBuffers();
		player.scale = glm::vec3(200.0f, 20.0f, 1.0f);
		player.position = glm::vec3
		(
			((GLfloat)screenWidth / 2) - player.scale.x / 2,
			(GLfloat)screenHeight - player.scale.y,
			0.0f
		);

		//Player texture
		player.texture.load("..//content//player.png");
		player.texture.setBuffers();
	}

	//Power up
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//Set player
		powerUp.setBuffers();
		powerUp.scale = glm::vec3(30.0f, 30.0f, 1.0f);

		//Player texture
		powerUp.texture.load("..//content//powerUp.png");
		powerUp.texture.setBuffers();
		powerUp.active = false;
	}

	//Ball
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//Set ball
		ball.setBuffers();
		ball.scale = glm::vec3(20.0f, 20.0f, 1.0f);
		ball.radius = ball.scale.x / 2;
		ball.position = glm::vec3
		(
			player.position.x + (player.scale.x / 2) - (ball.scale.x / 2),
			player.position.y - ball.scale.y,
			player.position.z
		);

		//Ball texture
		ball.texture.load("..//content//ball.png");
		ball.texture.setBuffers();
		gameover.active = false;
	}

	//Level
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//Set level
		buildLevel();
	}

	//Lives
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//Set lives
		lives.setBuffers();
		lives.scale = glm::vec3(30.0f, 30.0f, 1.0f);
		lives.position = glm::vec3
		(
			30.0f,
			(GLfloat)screenHeight - lives.scale.y - 5.0f,
			0.0f
		);

		//Lives texture
		lives.texture.load("..//content//heart.png");
		lives.texture.setBuffers();
	}

	//Win and gameover
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//Set win
		win.setBuffers();
		win.scale = glm::vec3(screenWidth / 2, screenHeight / 2, 1.0f);
		win.position = glm::vec3
		(
			screenWidth / 4, 
			screenHeight / 4,
			0.0f
		);

		//Win texture
		win.texture.load("..//content//youWin.png");
		win.texture.setBuffers();
		win.active = false;
	}
	{
		//Set gameover
		gameover.setBuffers();
		gameover.scale = glm::vec3(screenWidth / 2, screenHeight / 2, 1.0f);
		gameover.position = glm::vec3
		(
			screenWidth / 4,
			screenHeight / 4,
			0.0f
		);

		//Lives texture
		gameover.texture.load("..//content//gameOver.png");
		gameover.texture.setBuffers();
		gameover.active = false;
	}

	//Score
	////////////////////////////////////////////////////////////////////////////////////////
	{
		loadScore();
	}
}

//Game Loop
void gameLoop()
{
	//Game Loop
	//While game is not in the exit state
	while (state != GameState::Exit)
	{
		//Handle input
		input();

		//Handle updates
		update();

		//Handle rendering
		render();
	}
}

//Input handler function
void input()
{
	//SDL event
	SDL_Event events;

	//While there are events to process
	while (SDL_PollEvent(&events))
	{
		//Toggle fullscreen and quit the game
		if (events.key.keysym.sym == SDLK_F1) SDL_SetWindowFullscreen(window.window(), SDL_WINDOW_FULLSCREEN_DESKTOP);
		if (events.key.keysym.sym == SDLK_F2) SDL_SetWindowFullscreen(window.window(), 0);

		//Switch on event type
		switch (events.type)
		{
		//Quit the game
		case SDL_QUIT: state = GameState::Exit; break;

		//Catch the key down key
		case SDL_KEYDOWN:
			//Move player left and right and release the ball
			if (events.key.keysym.sym == SDLK_LEFT)	moveLeft = true;
			if (events.key.keysym.sym == SDLK_RIGHT) moveRight = true;
			if (events.key.keysym.sym == SDLK_SPACE) stuckToPaddle = false;
			if (events.key.keysym.sym == SDLK_ESCAPE) state = GameState::Exit;
			break;

		//Catch the key up key
		case SDL_KEYUP:		
			//Don't move player left or right
			if (events.key.keysym.sym == SDLK_LEFT)	moveLeft = false;
			if (events.key.keysym.sym == SDLK_RIGHT) moveRight = false;
			break;

		////Catch the mouse motion
		//case SDL_MOUSEMOTION: if (events.motion.x) SDL_GetMouseState(&mouseX, &mouseY); break;

		//Catch the window event
		case SDL_WINDOWEVENT:
			if (events.window.event == SDL_WINDOWEVENT_RESIZED && events.window.event != SDL_WINDOW_MAXIMIZED) maintainAspectRatio();
			if (events.window.event == SDL_WINDOW_MAXIMIZED) SDL_SetWindowFullscreen(window.window(), SDL_WINDOW_FULLSCREEN_DESKTOP); 
			break;
		}
 	}
}

//Game update handler function
void update()
{
	//Finite state machine - play state
	if (state == GameState::Play)
	{
		//Is the game won
		gameWon = wonGame();

		//If the game is won change to winning state
		if (gameWon) state = GameState::Win;

		//Update the pladdle
		updatePlayer();

		//Update the ball
		updateBall();
	}

	//Win state
	else if (state == GameState::Win) win.active = true;

	//Lose state
	else if (state == GameState::Lose) gameover.active = true;
}

//Update the paddle
void updatePlayer()
{
	//Player is alive
	if (player.lives > 0)
	{
		//Move the paddle left
		if (moveLeft)
		{
			//If player not at the edge of screen
			if (player.position.x >= (GLfloat)boundLeft - viewMatrix[3].x + leftBound.scale.x)
			{
				player.position.x -= playerVelocity.x;
				lives.position.x -= offset;
				viewMatrix = glm::translate(viewMatrix, glm::vec3(playerVelocity.x / 20.0f, 0.0f, 0.0f));
				parallaxForeTranslate = glm::translate(parallaxForeTranslate, glm::vec3(playerVelocity.x / 10.0f, 0.0f, 0.0f));
				parallaxMidTranslate = glm::translate(parallaxMidTranslate, glm::vec3(playerVelocity.x / 5.0f, 0.0f, 0.0f));
				parallaxBackTranslate = glm::translate(parallaxBackTranslate, glm::vec3(playerVelocity.x / 2.5f, 0.0f, 0.0f));
			}
		}

		//Move the paddle right
		if (moveRight)
		{
			//If player not at the edge of screen
			if (player.position.x <= (GLfloat)boundRight - player.scale.x - viewMatrix[3].x - rightBound.scale.x)
			{
				player.position.x += playerVelocity.x;
				lives.position.x += offset;
				viewMatrix = glm::translate(viewMatrix, glm::vec3(-playerVelocity.x / 20.0f, 0.0f, 0.0f));
				parallaxForeTranslate = glm::translate(parallaxForeTranslate, glm::vec3(-playerVelocity.x / 10.0f, 0.0f, 0.0f));
				parallaxMidTranslate = glm::translate(parallaxMidTranslate, glm::vec3(-playerVelocity.x / 5.0f, 0.0f, 0.0f));
				parallaxBackTranslate = glm::translate(parallaxBackTranslate, glm::vec3(-playerVelocity.x / 2.5f, 0.0f, 0.0f));
			}
		}
		
		////If not moving with the keys
		//if (!moveLeft && !moveRight)
		//{
		//	//Move the paddle with the mouse
		//	player.position.x = mouseX - (player.scale.x / 2);
		//}

		//Ball stuck to paddle
		if (stuckToPaddle)
		{
			//Set the ball position
			ball.position.x = player.position.x + (player.scale.x / 2) - (ball.scale.x / 2);
			ball.position.y = player.position.y - ball.scale.y;
		}
	}

	//Player is dead
	else if (player.lives <= 0)	state = GameState::Lose;

	////Use for testing - delete when finished
	//if (!stuckToPaddle)	player.position.x = ball.position.x - player.scale.x / 2;
}

//Update the ball
void updateBall()
{
	//Ball stuck to paddle
	if (!stuckToPaddle)
	{
		//Move the ball in x
		ball.position.x += ballVelocity.x;

		//Keep ball inside the bounds of the window - left side
		if (ball.position.x <= (GLfloat)boundLeft - viewMatrix[3].x + rightBound.scale.x)
		{
			ballVelocity.x = -ballVelocity.x;
			ball.position.x = 0.0f - viewMatrix[3].x + rightBound.scale.x;
		}

		//Right side
		else if (ball.position.x + ball.scale.x >= (GLfloat)boundRight - viewMatrix[3].x - rightBound.scale.x)
		{
			ballVelocity.x = -ballVelocity.x;
			ball.position.x = (GLfloat)boundRight - ball.scale.x - viewMatrix[3].x - rightBound.scale.x;
		}

		//Check each brick for collision on the left and right
		for (int y = 0; y < numbBricksHigh; y++)
		{
			for (int x = 0; x < numbBricksWide; x++)
			{
				//If the brick is active
				if (bricks[y][x].brickAlive)
				{
					//If a collision occurs between the ball and brick
					if (collisionDetection(ball, bricks[y][x]))
					{
						setCrackedBrick(x, y);
						ballVelocity.x = -ballVelocity.x;
						ball.position.x += ballVelocity.x;
					}
					if (bricks[y][x].hits < 0) setDeadBrick(x, y);
				}
				//Brick is dying
				if (bricks[y][x].brickDying) setDyingBrick(x, y);

				//Brick is gone
				if (bricks[y][x].position.y > (GLfloat)boundBottom) bricks[y][x].brickDying = false;
			}
		}

		//Move the ball in y
		ball.position.y += ballVelocity.y;

		//Top side
		if (ball.position.y <= (GLfloat)boundTop + (topBound.scale.y / 2))
		{
			ballVelocity.y = -ballVelocity.y;
			ball.position.y = (GLfloat)boundTop + (topBound.scale.y / 2);
		}

		//Bottom side
		else if (ball.position.y >= (GLfloat)boundBottom)
		{
			player.lives--;
			stuckToPaddle = true;
			ballVelocity.x = -ballVelocity.x - 0.75f;
		}

		//Player side - avoid ball sticking in paddle
		else if (collisionDetection(ball, player)) ballVelocity.y = -1 * abs(ballVelocity.y);

		//Check each brick for collision on the top and bottom
		for (int y = 0; y < numbBricksHigh; y++)
		{
			for (int x = 0; x < numbBricksWide; x++)
			{
				//If the brick is active
				if (bricks[y][x].brickAlive)
				{
					//If a collision occurs between the ball and brick
					if (collisionDetection(ball, bricks[y][x]))
					{
						setCrackedBrick(x, y);
						ballVelocity.y = -ballVelocity.y;
						ball.position.y += ballVelocity.y;
					}
					if (bricks[y][x].hits < 0) setDeadBrick(x, y);
				}
				//Brick is dying
				if (bricks[y][x].brickDying) setDyingBrick(x, y);

				//Brick is gone
				if (bricks[y][x].position.y >(GLfloat)boundBottom) bricks[y][x].brickDying = false;
			}
		}

		//If the power up is active
		powerUpActive(2, 4);
		powerUpActive(4, 3);
		powerUpActive(6, 2);

		//Move the power up
		if (powerUp.active)
		{
			//Move the power up down
			powerUp.position.y += 1.5f;

			//Player collects power up 
			if (collisionDetection(powerUp, player))
			{
				powerUp.active = false;
				powerUp.position.y = 0.0f;
				powerUps++;
				setPowerUp();
			}
		}

		//Power up lost
		if (powerUp.position.y > (GLfloat)boundBottom + 40.0f)
		{
			powerUp.active = false;
			powerUp.position.y = 0.0f;
		}
	}
}

//Start the rendering process
void startRender()
{
	//Start the rendering process
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	////Set to wireframe so we can see the triangles
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Use the shader program
	glProgram.use();
}

//Render the scene
void render()
{
	//Start rendering
	startRender();

	//Render background and parallax
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//Render background
		renderObject(backgroundTranslate, backgroundScale, glm::vec3(1.0f), background.texture);
		background.render();
	}
	{
		//Parallax background
		renderObject(parallaxBackTranslate, parallaxBackScale, glm::vec3(1.0f), parallaxBackground.texture);
		background.render();
	}
	{
		//Parallax midground
		renderObject(parallaxMidTranslate, parallaxMidScale, glm::vec3(1.0f), parallaxMidground.texture);
		parallaxMidground.render();
	}
	{
		//Parallax foreground
		renderObject(parallaxForeTranslate, parallaxForeScale, glm::vec3(1.0f), parallaxForeground.texture);
		parallaxForeground.render();
	}

	//Render the level
	////////////////////////////////////////////////////////////////////////////////////////
	{
		for (int y = 0; y < numbBricksHigh; y++)
		{
			for (int x = 0; x < numbBricksWide; x++)
			{
				//If the brick is active and not dying
				if (bricks[y][x].brickAlive && !bricks[y][x].brickDying)
				{
					resetMatrices();
					modelTranslate = glm::translate(modelTranslate, bricks[y][x].position);
					modelScale = glm::scale(modelScale, bricks[y][x].scale);
					renderObject(modelTranslate, modelScale, bricks[y][x].colour, bricks[y][x].texture);
					bricks[y][x].render();
				}
				//If the brick is disabled and dying
				else if (!bricks[y][x].brickAlive && bricks[y][x].brickDying)
				{
					resetMatrices();
					modelTranslate = glm::translate(modelTranslate, bricks[y][x].position);
					modelScale = glm::scale(modelScale, bricks[y][x].scale);
					modelTranslate = glm::rotate(modelTranslate, bricks[y][x].rotation, glm::vec3(0.0f, 0.0f, 1.0f));
					renderObject(modelTranslate, modelScale, bricks[y][x].colour, bricks[y][x].texture);
					bricks[y][x].render();
				}
			}
		}
	}

	//Render player
	////////////////////////////////////////////////////////////////////////////////////////
	{
		resetMatrices();
		modelTranslate = glm::translate(modelTranslate, player.position);
		modelScale = glm::scale(modelScale, player.scale);
		renderObject(modelTranslate, modelScale, player.colour, player.texture);
		player.render();
	}

	//Render power up
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//If power up is active
		if (powerUp.active)
		{
			resetMatrices();
			modelTranslate = glm::translate(modelTranslate, powerUp.position);
			modelScale = glm::scale(modelScale, powerUp.scale);
			renderObject(modelTranslate, modelScale, glm::vec3(1.0f), powerUp.texture);
			powerUp.render();
		}
	}

	//Render bounds
	////////////////////////////////////////////////////////////////////////////////////////
	{
		resetMatrices();
		modelTranslate = glm::translate(modelTranslate, leftBound.position);
		modelScale = glm::scale(modelScale, leftBound.scale);
		renderObject(modelTranslate, modelScale, leftBound.colour, leftBound.texture);
		leftBound.render();
	}
	{
		resetMatrices();
		modelTranslate = glm::translate(modelTranslate, rightBound.position);
		modelScale = glm::scale(modelScale, rightBound.scale);
		renderObject(modelTranslate, modelScale, rightBound.colour, rightBound.texture);
		rightBound.render();
	}
	{
		resetMatrices();
		modelTranslate = glm::translate(modelTranslate, topBound.position);
		modelScale = glm::scale(modelScale, topBound.scale);
		renderObject(modelTranslate, modelScale, topBound.colour, topBound.texture);
		topBound.render();
	}

	//Render ball
	////////////////////////////////////////////////////////////////////////////////////////
	{
		resetMatrices();
		modelTranslate = glm::translate(modelTranslate, ball.position);
		modelScale = glm::scale(modelScale, ball.scale);
		renderObject(modelTranslate, modelScale, ball.colour, ball.texture);
		ball.render();
	}

	//Render lives
	////////////////////////////////////////////////////////////////////////////////////////
	{
		for (int i = 0; i < player.lives; i++)
		{
			resetMatrices();
			modelTranslate = glm::translate(modelTranslate, glm::vec3(lives.position.x + (i * 40.0f), lives.position.y, lives.position.z));
			modelScale = glm::scale(modelScale, lives.scale);
			renderObject(modelTranslate, modelScale, lives.colour, lives.texture);
			lives.render();
		}
	}

	//Render win/gameover texture
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//If winning the game render
		if (win.active)
		{
			resetMatrices();
			modelTranslate = glm::translate(modelTranslate, win.position);
			modelScale = glm::scale(modelScale, win.scale);
			renderObject(modelTranslate, modelScale, glm::vec3(1.0f), win.texture);
			win.render();
		}

		//If lossing the game render
		if (gameover.active)
		{
			resetMatrices();
			modelTranslate = glm::translate(modelTranslate, gameover.position);
			modelScale = glm::scale(modelScale, gameover.scale);
			renderObject(modelTranslate, modelScale, glm::vec3(1.0f), gameover.texture);
			gameover.render();
		}
	}

	//Render score
	////////////////////////////////////////////////////////////////////////////////////////
	{
		//Render the score objects
		for (auto& go : scoreObject)
		{
			resetMatrices();
			modelTranslate = glm::translate(modelTranslate, go.position);
			modelScale = glm::scale(modelScale, go.scale);
			renderObject(modelTranslate, modelScale, glm::vec3(1.0f), go.texture);
			go.render();
		}
	}

	//End rendering
	endRender();
}

//Reset the model and scale matrices
void resetMatrices()
{
	modelTranslate = glm::mat4(1.0f);
	modelScale = glm::mat4(1.0f);
}

//Render the level
void buildLevel()
{
	//Brick texture
	Texture texture;
	Texture cracked;
	texture.load("..//content//block.png");
	texture.setBuffers();
	cracked.load("..//content//crackedBlock.png");
	cracked.setBuffers();
	
	//Position and scale of the bricks
	for (int y = 0; y < numbBricksHigh; y++)
	{ 
		for (int x = 0; x < numbBricksWide; x++)
		{
			//Brick
			Brick brick;

			//Set brick
			brick.setBuffers();
			brick.scale = glm::vec3((screenWidth / (GLfloat)numbBricksWide) - 20.0f, ((screenHeight / 2) / (GLfloat)numbBricksHigh) -20.0f, 1.0f);
			brick.position = glm::vec3
			(
				55.0f + (((screenWidth / (GLfloat)numbBricksWide) - 10.0f) * x),
				20.0f + (((screenHeight / 2) / (GLfloat)numbBricksHigh) * y),
				0.0f
			);

			//Set brick colours
			if (y == 0 || y == 5)	   brick.colour = { 1.0f, 0.5f, 0.0f };
			else if (y == 1 || y == 6) brick.colour = { 0.5f, 1.0f, 0.5f };
			else if (y == 2 || y == 7) brick.colour = { 0.0f, 0.5f, 1.0f };
			else if (y == 3 || y == 8) brick.colour = { 1.0f, 1.0f, 0.0f };
			else if (y == 4 || y == 9) brick.colour = { 1.0f, 0.0f, 0.0f };
			else					   brick.colour = { 1.0f, 1.0f, 1.0f };

			//Player texture
			brick.texture = texture;
			brick.cracked = cracked;

			//Set the bricks
			bricks[y][x] = brick;
		}
	}
}

//Render objects
void renderObject(glm::mat4 translation, glm::mat4 scale, glm::vec3 colour, Texture &texture)
{
	//Render the bricks
	glUniformMatrix4fv(glGetUniformLocation(glProgram.getProgramID(), "uModel"), 1, GL_FALSE, glm::value_ptr(translation * scale));
	glUniformMatrix4fv(glGetUniformLocation(glProgram.getProgramID(), "uView"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(glProgram.getProgramID(), "uProjection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniform3f(glGetUniformLocation(glProgram.getProgramID(), "uColour"), colour.x, colour.y, colour.z);
	glBindTexture(GL_TEXTURE_2D, texture.texture);
}

//End the rendering process
void endRender()
{
	//End the rendering process
	//Swap the buffer to render to screen
	window.swapBuffer();
	glProgram.unuse();
}

//Colision detection ball to brick
bool collisionDetection(Ball &ball, Brick &brick)
{
	//The sides of ball and brick
	GLfloat ballLeft, brickLeft, ballRight, brickRight, ballTop, brickTop, ballBottom, brickBottom;

	//Calculate the sides of ball
	ballLeft = ball.position.x; ballRight = ball.position.x + ball.scale.x; ballTop = ball.position.y; ballBottom = ball.position.y + ball.scale.y;

	//Calculate the sides of brick
	brickLeft = brick.position.x; brickRight = brick.position.x + brick.scale.x; brickTop = brick.position.y; brickBottom = brick.position.y + brick.scale.y;

	//If any of the sides from the ball are outside of the brick
	if (ballBottom <= brickTop) return false;
	if (ballTop >= brickBottom) return false;
	if (ballRight <= brickLeft) return false;
	if (ballLeft >= brickRight) return false;

	//If any of the sides from the ball are inside of the brick
	return true;
}

//Colision detection power up to player
bool collisionDetection(GameObject &powerUp, Player &player)
{
	//The sides of ball and player
	GLfloat powerUpLeft, playerLeft, powerUpRight, playerRight, powerUpTop, playerTop, powerUpBottom, playerBottom;

	//Calculate the sides of ball
	powerUpLeft = powerUp.position.x; powerUpRight = powerUp.position.x + powerUp.scale.x; powerUpTop = powerUp.position.y; powerUpBottom = powerUp.position.y + powerUp.scale.y;

	//Calculate the sides of player
	playerLeft = player.position.x; playerRight = player.position.x + player.scale.x; playerTop = player.position.y; playerBottom = player.position.y + player.scale.y;

	//If any of the sides from the powerUp are outside of the player
	if (powerUpBottom <= playerTop) return false;
	if (powerUpTop >= playerBottom) return false;
	if (powerUpRight <= playerLeft) return false;
	if (powerUpLeft >= playerRight) return false;

	//If any of the sides from the powerUp are inside of the brick
	return true;
}

//Colision detection ball to player
bool collisionDetection(Ball &ball, Player &player)
{
	//The sides of ball and player
	GLfloat ballLeft, playerLeft, ballRight, playerRight, ballTop, playerTop, ballBottom, playerBottom;

	//Calculate the sides of ball
	ballLeft = ball.position.x; ballRight = ball.position.x + ball.scale.x; ballTop = ball.position.y; ballBottom = ball.position.y + ball.scale.y;

	//Calculate the sides of player
	playerLeft = player.position.x; playerRight = player.position.x + player.scale.x; playerTop = player.position.y; playerBottom = player.position.y + player.scale.y;

	//If any of the sides from the ball are outside of the player
	if (ballBottom <= playerTop) return false;
	if (ballTop >= playerBottom) return false;
	if (ballRight <= playerLeft) return false;
	if (ballLeft >= playerRight) return false;

	//If any of the sides from the ball are inside of the player
	return true;
}

//Is the game won
bool wonGame()
{
	//Check if the bricks are all dead
	for (int y = 0; y < numbBricksHigh; y++)
	{
		for (int x = 0; x < numbBricksWide; x++)
		{
			//If the bricks alive game not won
			if (bricks[y][x].brickAlive)
			{
				return !bricks[y][x].brickAlive;
			}
		}
	}
	//Game won
	return true;
}

//Maintain the aspect ratio
void maintainAspectRatio()
{
	//Get the window height and width
	int width;
	int height;
	int oldwidth = screenWidth;
	int oldheight = screenHeight;

	SDL_GetWindowSize(window.window(), &width, &height);

	//Check aspect ratio
	if (width != oldwidth) height = (GLint)(width * aspectRatio);
	else width = (GLint)(height * (1 / aspectRatio));

	//Set viewport
	glViewport(0, 0, width, height);
	SDL_SetWindowSize(window.window(), width, height);
	SDL_SetWindowPosition(window.window(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

//Toggle fullscreen
void toggleFullScreen()
{
}

//Load the score gameobjects
void loadScore()
{
	//Scale of numbers
	glm::vec3 scale = glm::vec3(30.0f, 30.0f, 1.0f);

	//Load in score game objects
	for (int i = 0; i < 4; i++)
	{
		//Score object
		GameObject go;
		go.setBuffers();

		//Set the score texture
		if (i == 0)
		{
			go.scale = glm::vec3(60.0f, 30.0f, 1.0f);
			go.position = glm::vec3
			(
				(GLfloat)screenWidth - 180.0f,
				(GLfloat)screenHeight - go.scale.y - 5.0f,
				0.0f
			);
			go.texture.load("..//content//score.png");
		}
		//Set the defualt score value textures
		else
		{
			go.scale = scale;
			go.position = glm::vec3
			(
				(GLfloat)screenWidth - 140.0f + (i * 30.0f),
				(GLfloat)screenHeight - go.scale.y - 5.0f,
				0.0f
			);
			go.texture.load("..//content//0.png");
		}
		go.texture.setBuffers();
		scoreObject.push_back(go);
	}

	//Load each number texture
	for (int i = 0; i < 10; i++)
	{
		//Brick texture
		Texture texture;
		std::string file = "..//content//" + std::to_string(i) + ".png";
		texture.load(file);
		texture.setBuffers();
		scoreText.push_back(texture);
	}
}

//Set the score
void setScore()
{
	//Get the score as a string
	std::string scoreStr = std::to_string(score);

	//Get the end of the vector
	int pos = static_cast<int>(scoreObject.size()) - 1;

	//Loop through the score string from the end to start
	for (int i = (int)scoreStr.length() - 1; i >= 0; i--)
	{
		int value = scoreStr[i] - '0';
		scoreObject[pos].texture = scoreText[value];
		pos--;
	}
}

//Set the power up of the player
void setPowerUp()
{
	//Big ball
	if (powerUps == 1)
	{
		player.colour = glm::vec3(0.5f, 0.0f, 1.0f);
		ball.scale *= 2.0f;
	}

	//Max brick hits
	else if (powerUps == 2)
	{
		player.colour = glm::vec3(0.0f, 1.0f, 0.5f);
		maxHits();
	}

	//Slow ball
	else if (powerUps == 3)
	{
		player.colour = glm::vec3(1.0f, 0.5f, 0.0f);
		ballVelocity *= 1.5f;
		playerVelocity *= 1.5f;
	}
}

//Max hit power up
void maxHits()
{
	//Loop through all bricks
	for (int y = 0; y < numbBricksHigh; y++)
	{
		for (int x = 0; x < numbBricksWide; x++)
		{
			//If the brick is active
			if (bricks[y][x].brickAlive) setCrackedBrick(x, y);
			if (bricks[y][x].hits < 0) setDeadBrick(x, y);
		}
	}
}

//Crack the brick
void setCrackedBrick(int x, int y)
{
	bricks[y][x].hits -= 1;
	bricks[y][x].texture = bricks[y][x].cracked;
	score += 3;
	setScore();
}

//Disable the brick
void setDeadBrick(int x, int y)
{
	bricks[y][x].brickDying = true;
	bricks[y][x].brickAlive = false;
	score += 10;
	setScore();
}

//Set brick as dying
void setDyingBrick(int x, int y)
{
	bricks[y][x].position.y += 6.5f;
	bricks[y][x].rotation += 0.075f;
	bricks[y][x].scale -= 0.75f;
}

//Activate a power up
void powerUpActive(int x, int y)
{
	//If the power up is active
	if (bricks[y][x].brickDying && !powerUp.active)
	{
		powerUp.active = true;
		powerUp.position = bricks[y][x].position;
	}
}