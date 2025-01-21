#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int GRID_SIZE = 20;

// Class for Game states
enum class GameState
{
	START_SCREEN,
	PLAYING,
	GAME_OVER
};

struct Point 
{
	int x, y;
};

// Global variables
std::vector<Point> snake = {{ 10, 10 }};
Point apple = { 15, 15 };
Point direction = { 1, 0 };
GameState currentGameState = GameState::START_SCREEN;

void drawSquare(float x, float y, float size)
{
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + size, y);
	glVertex2f(x + size, y + size);
	glVertex2f(x, y + size);
	glEnd();
}

void drawSnake()
{
	glColor3f(0.0f, 1.0f, 0.0f);
	for (const auto& segment : snake)
	{
		drawSquare(segment.x / float(GRID_SIZE), segment.y / float(GRID_SIZE), 1.0f / GRID_SIZE);
	}
}

void drawApple()
{
	glColor3f(1.0f, 0.0f, 0.0f);
	drawSquare(apple.x / float(GRID_SIZE), apple.y / float(GRID_SIZE), 1.0f / GRID_SIZE);
}

void drawGameOver()
{
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(0.3f, 0.45f);
	glVertex2f(0.7f, 0.45f);
	glVertex2f(0.7f, 0.55f);
	glVertex2f(0.3f, 0.55f);
	glEnd();

	std::cout << "Game Over\n";
}

void drawStartScreen()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex2f(0.3f, 0.45f);
	glVertex2f(0.7f, 0.45f);
	glVertex2f(0.7f, 0.55f);
	glVertex2f(0.3f, 0.55f);
	glEnd();

	std::cout << "Press SPACE to Start\n";
}

void updateSnake()
{
	if (currentGameState != GameState::PLAYING)
		return;

	// Move snake
	Point newHead = { snake[0].x + direction.x, snake[0].y + direction.y };

	// Check collision with wall
	if (newHead.x < 0 || newHead.x >= GRID_SIZE || newHead.y < 0 || newHead.y >= GRID_SIZE)
	{
		currentGameState = GameState::GAME_OVER;
		return;
	}

	// Check for self-collision
	for (const auto& segment : snake)
	{
		if (newHead.x == segment.x && newHead.y == segment.y)
		{
			currentGameState = GameState::GAME_OVER;
			return;
		}
	}

	// Add new head
	snake.insert(snake.begin(), newHead);

	if (newHead.x == apple.x && newHead.y == apple.y)
		apple = { rand() % GRID_SIZE, rand() % GRID_SIZE };
	else
		snake.pop_back();
}

void restartGame(int key)
{
	if (currentGameState == GameState::GAME_OVER && key == GLFW_KEY_SPACE)
	{
		currentGameState = GameState::START_SCREEN;
		snake = { { 10, 10 } };
		apple = { 15, 15 };
		direction = { 1, 0 };
	}
}

void keyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
	if (action == GLFW_PRESS)
	{
		if (currentGameState == GameState::START_SCREEN)
		{
			if (key == GLFW_KEY_SPACE)
			{
				std::cout << "Starting Game...\n";
				currentGameState = GameState::PLAYING;
			}
		}	
		else if (currentGameState == GameState::GAME_OVER)
		{
			if (key == GLFW_KEY_SPACE)
				restartGame(key);
			else if (key == GLFW_KEY_ESCAPE)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		switch (key)
		{
		case GLFW_KEY_W:
		case GLFW_KEY_UP:
			if (direction.y == 0) direction = { 0, 1 };
			break;
		case GLFW_KEY_S:
		case GLFW_KEY_DOWN:
			if (direction.y == 0) direction = { 0, -1 };
			break;
		case GLFW_KEY_A:
		case GLFW_KEY_LEFT:
			if (direction.x == 0) direction = { -1, 0 };
			break;
		case GLFW_KEY_D:
		case GLFW_KEY_RIGHT:
			if (direction.x == 0) direction = { 1, 0 };
			break;
		}
	}
}

int main()
{
	if (!glfwInit())
	{
		std::cout << "Failed to init GLFW" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake Game", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to init GLEW" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, keyboardInput);
	glOrtho(0, 1, 0, 1, -1, 1);

	srand(static_cast<unsigned int>(time(nullptr)));
	
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		if (currentGameState == GameState::PLAYING)
			updateSnake();

		if (currentGameState == GameState::START_SCREEN)
			drawStartScreen();
		else if (currentGameState == GameState::GAME_OVER)
		{
			drawGameOver();
		}
		else
		{
			drawSnake();
			drawApple();
		}
		glfwSwapBuffers(window);
		glfwPollEvents();

		glfwWaitEventsTimeout(0.1);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}