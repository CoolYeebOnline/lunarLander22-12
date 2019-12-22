//include class' header 
#include "Game.h"
#include "utilityFunc.h"
#include "constants.h"
#include "anims.h"
#include <iostream>
#include <string>
#include <sstream>
#include "GameObjects.h"
#include "Score.h"

Game::Game()
{
	//constructor
}

void Game::initialise() {
	SetConsoleTitle("Title of my Console Window");
	SetConsoleScreenBufferSize(wHnd, buffersize);

	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
}

void Game::update(float deltaTime) {

	switch (currentGameState) {

	case SPLASH:
	{
		//creates a new splash screen
		static Splash splash;
		splash.Duration += deltaTime;
		if (splash.Duration < 3.0f)
		{
			//Title Screen
			WriteImageToBuffer(consoleBuffer, splash.CHARACTERS, splash.COLOURS, splash.HEIGHT, splash.WIDTH,
				(SCREEN_WIDTH / 2) - (splash.WIDTH / 2), (SCREEN_HEIGHT / 2) - (splash.HEIGHT / 2));
		}
		else {
			currentGameState = MENU;
		}
		break;
	}
	case MENU:
	{
		//clear any previous images
		ClearScreen(consoleBuffer);
		//Main Menu - Print out options
		WriteTextToBuffer(consoleBuffer, "1. PLAY", (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));
		WriteTextToBuffer(consoleBuffer, "2. OPTIONS", (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) + 1);
		WriteTextToBuffer(consoleBuffer, "3. QUIT ", (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) + 2);

		if (GetAsyncKeyState(KEY_ESC))
		{
			exitGame = true;
		}
		if (GetAsyncKeyState(KEY_1))
		{
			currentGameState = PLAY;
		}
		if (GetAsyncKeyState(KEY_2))
		{
			currentGameState = OPTIONS;
		}
		if (GetAsyncKeyState(KEY_3))
		{
			exitGame = true;
		}
		break;
	}
	case PLAY:
	{
		if (GetAsyncKeyState(KEY_ESC))
		{
			exitGame = true;
		}
		if (GetAsyncKeyState(KEY_R))
		{
			player.Reset();
			currentGameState = PLAY;
		}
		//take input
		if (!player.hasLanded && !player.hasCrashed && !player.fuelEmpty)
		{

			if (GetAsyncKeyState(KEY_W))
			{
				player.isAccelerating = true;
			}
			if (GetAsyncKeyState(KEY_A))
			{
				--player.XPos;
			}
			if (GetAsyncKeyState(KEY_S))
			{
				++player.YPos;
			}
			if (GetAsyncKeyState(KEY_D))
			{
				++player.XPos;
				WriteImageToBuffer(consoleBuffer, anim1, nullptr, player.HEIGHT, player.WIDTH, 0, 0);
			}

			if (player.isAccelerating) {
				player.acceleration += (ACCELERATION_RATE * deltaTime);
				player.fuel = player.fuel - 1;
			}
			else {
				player.acceleration -= (DECELERATION_RATE * deltaTime);
			}

			//reset acceleration flag
			player.isAccelerating = false;

			player.acceleration = ClampFloat(player.acceleration, 0.0f, 1.5f);

			if (player.acceleration >= 1.0f) // remove this later and put it at the top
			{
				--player.YPos;
			}
			else if (player.acceleration < 0.5f)
			{
				++player.YPos;
			}
			//clamp input
			player.XPos = ClampInt(player.XPos, 0, (SCREEN_WIDTH - player.WIDTH));
			player.YPos = ClampInt(player.YPos, 0, (SCREEN_HEIGHT - player.HEIGHT));

			//gets the two characters underneath the character array
			char bottomLeftchar = background.CHARACTERS[player.XPos + SCREEN_WIDTH * (player.YPos + (player.HEIGHT - 1))];
			char bottomRightchar = background.CHARACTERS[(player.XPos + (player.WIDTH - 1)) + SCREEN_WIDTH * (player.YPos + (player.HEIGHT - 1))];
			//get the two characters 2 spaces underneath the character array
			char scoreLeftchar = background.CHARACTERS[player.XPos + SCREEN_WIDTH * (player.YPos + (player.HEIGHT - 2))];
			char scoreRightchar = background.CHARACTERS[(player.XPos + (player.WIDTH - 1)) + SCREEN_WIDTH * (player.YPos + (player.HEIGHT - 2))];
			//check to either land or crash
			//check for landing
			if (bottomLeftchar != ' ' || bottomRightchar != ' ')
			{
				if (bottomLeftchar == '_' && bottomRightchar == '_')
				{
					player.hasLanded = true;
					currentGameState = GAME_OVER;
					
					if (scoreLeftchar != ' ' || scoreRightchar != ' ') {
						//adds 200 points to player score if the lander lands on x2
						if (scoreRightchar == '2') {
							player.score = player.score + 200;
						}
						//adds 400 points to player score if the lander lands on x3
						else if (scoreLeftchar == 'x' && scoreRightchar == '3') {
							player.score = player.score + 400;
						}
						//adds 600 points to player score if the lander lands on x4
						else if (scoreLeftchar == 'x' && scoreRightchar == '4') {
							player.score = player.score + 600;
						}
						//adds 800 points to player score if the lander lands on x5
						else if (scoreLeftchar == 'x' && scoreRightchar == '5') {
							player.score = player.score + 800;
						}
					}
				}
				//check to see it the lander hits something that isn't a landing zone
				else if (bottomLeftchar != ' ' || bottomRightchar != ' ')
				{
					player.score = player.score - 1;
					player.lives = player.lives - 1;
					player.hasCrashed = true;
					currentGameState = GAME_OVER;
				}
				if (player.fuelEmpty) {
					player.score = player.score - 1;
					player.lives = player.lives - 1;
					currentGameState = GAME_OVER;
				}
				
			}
		}

		//clears the previous frame of animation before drawing the next one
		ClearScreen(consoleBuffer);

		//Draw Background Image
		WriteImageToBuffer(consoleBuffer, background.CHARACTERS, nullptr, SCREEN_HEIGHT, SCREEN_WIDTH, 0, 0);

		//Draw Player Image
		WriteImageToBuffer(consoleBuffer, player.CHARACTERS, player.COLOURS, player.HEIGHT, player.WIDTH, player.XPos, player.YPos);

		//Draw UI text
		WriteTextToBuffer(consoleBuffer, "SCORE: " + std::to_string(player.score), 1, 0);
		WriteTextToBuffer(consoleBuffer, "TIME: " + std::to_string(deltaTime), 1, 1);
		WriteTextToBuffer(consoleBuffer, "FUEL: " + std::to_string(player.fuel), 1, 2);
		WriteTextToBuffer(consoleBuffer, "SPEED: " + std::to_string(player.acceleration), 1, 3);
		WriteTextToBuffer(consoleBuffer, "LIVES: " + std::to_string(player.lives), 1, 4);

		break;
	}
	case OPTIONS:
	{
		//options Menu
		break;
	}
	case GAME_OVER:
	{
		if (GetAsyncKeyState(KEY_ESC))
		{
			exitGame = true;
		}
		if (GetAsyncKeyState(KEY_ENT))
		{
			player.Reset();
			currentGameState = MENU;
		}
		if (GetAsyncKeyState(KEY_R))
		{
			player.Reset();
			currentGameState = PLAY;
		}
		//check fuel count
		if (player.fuel == 0)
		{
			player.fuelEmpty = true;
		}
		//check lives
		if (player.lives == 0) {
			exitGame = true;
		}
		if (player.score > 1000) {
			player.lives = player.lives + 1;
		}
	
		//end game text
		if (player.fuelEmpty) {
			WriteTextToBuffer(consoleBuffer, "Oops! you ran out of fuel! prepare for crash landing", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			WriteTextToBuffer(consoleBuffer, "Press [Enter] to return to the main menu", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);
			WriteTextToBuffer(consoleBuffer, "Press [R] to reset", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4);
		}
		if (player.hasLanded)
		{
			WriteTextToBuffer(consoleBuffer, "Touchdown! You did it!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			WriteTextToBuffer(consoleBuffer, "Press [Enter] to return to the main menu", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);
			WriteTextToBuffer(consoleBuffer, "Press [R] to reset", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4);
		}
		else if (player.hasCrashed)
		{
			WriteTextToBuffer(consoleBuffer, "Oops! Lander Down! ", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			WriteTextToBuffer(consoleBuffer, "Press [Enter] to return to the main menu", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);
			WriteTextToBuffer(consoleBuffer, "Press [R] to reset", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4);
		}
		//Game over
		break;
	}

	}
}

void Game::draw()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, characterBufferSize, characterPosition, &consoleWriteArea);
}