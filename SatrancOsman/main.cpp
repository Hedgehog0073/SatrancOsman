#include "Game.h"






int main(int argc, char* argv[])
{
	//Stackta oyun için memory allocate ediyorum.
	Game* myGame = new Game();


	myGame->Init(); //Oyunu baslatıyorum 
	myGame->HandleEvents();//input bekliyorum.
	
	


	return 0;
}