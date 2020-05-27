#include "Game.h"






int main(int argc, char* argv[])
{
	Game* myGame = new Game();
	myGame->Init(); //Oyunu baslatma //TODO Turn this into Constructor thing
	myGame->HandleEvents();
	
	


	return 0;
}