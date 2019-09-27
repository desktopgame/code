#include "MainMenu.h"
#include "Game.h"
#include "DialogBox.h"
#include <SDL/SDL.h>

MainMenu::MainMenu(Game * game) : UIScreen(game)
{
	mGame->SetState(Game::EMainMenu);
	SetRelativeMouseMode(false);
	SetTitle("MainMenuTitle");
	AddButton("StartButton", [this]() {
		Close();
	});
	AddButton("QuitButton", [this]() {
		new DialogBox(mGame, "QuitText",
			[this]() {
			mGame->SetState(Game::EQuit);
		});
	});
}

MainMenu::~MainMenu()
{
	SetRelativeMouseMode(true);
	mGame->SetState(Game::EGameplay);
}
