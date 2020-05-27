#include "Game.h"
#include <iostream>

#define DEBUG

void Game::Init()
{
	//SDL kutuphanelerini init ediyorum.
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);

	Window = SDL_CreateWindow("Chess by Osman v4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, NULL);
	Renderer = SDL_CreateRenderer(Window, -1, 0);

	SDL_SetWindowInputFocus(Window);


	ChessBoard.Init(Renderer);
	ChessBoard.DrawBoard(this);

	Tools::AlignPieces(this);


	SDL_RenderPresent(Renderer);
}
TilePos::TilePos(int X, int Y) : x{ X }, y{ Y }
{

}
TilePos::TilePos()
{

}
void Game::HandleEvents()
{
	SDL_Event Event;
	int HoldItemIndex = -1;


	while (Running)
	{

		Uint64 StartTime = SDL_GetPerformanceCounter();
		if (SDL_PollEvent(&Event))
		{
			int x, y;
			switch (Event.type)
			{
			case(SDL_QUIT):
				Running = false;
				break;
			case(SDL_KEYDOWN):
				switch (Event.key.keysym.sym)
				{
				case(SDLK_ESCAPE):
					Running = false;
					break;
				}
			case(SDL_MOUSEBUTTONDOWN):

				SDL_GetMouseState(&x, &y);
				x = x / RectSize;
				y = y / RectSize;

				for (int i = 0; i < PieceCount; i++)
				{
					if (PieceC.Pieces[i]->Pos.x == x && PieceC.Pieces[i]->Pos.y == y)
					{
						PieceC.Pieces[i]->IsFloating = true;
						HoldItemIndex = i;
					}
				}
				break;
			case(SDL_MOUSEBUTTONUP):
				if (HoldItemIndex != -1)
				{

					PieceC.Pieces[HoldItemIndex]->IsFloating = false;
					SDL_GetMouseState(&x, &y);
					x = x / RectSize;
					y = y / RectSize;
					PieceC.Pieces[HoldItemIndex]->Move(TilePos(x, y), this);
					HoldItemIndex = -1;
				}
				break;


			}

		}
		Render();
		/*
		Uint64 EndTime = SDL_GetPerformanceCounter();
		double DeltaTime = (double)((EndTime - StartTime) * 1000) / (double)SDL_GetPerformanceFrequency();
		if (DeltaTime < TargetTimeForFrame)
		{
			SDL_Delay(TargetTimeForFrame - DeltaTime); // yukardaki islemden cikan sec bana ms lazim odan 1000 ile carpma var
			EndTime = SDL_GetPerformanceCounter();

		}
		SDL_Log("Time elapsed for frame is %f", DeltaTime);
		SDL_Log("FPS: %f", 1000/DeltaTime);
		SDL_Log("%f", DeltaTime);
		*/

	}

}
void Game::Render()
{
	SDL_RenderClear(Renderer);
	ChessBoard.DrawBoard(this);
	PieceC.RenderAll(this);
	SDL_RenderPresent(Renderer);
}






Piece::Piece(Game* game)
{
	PieceIndex = game->PieceCount++;
	//game->PieceC.PiecePositions[PieceIndex] = &Pos;
	game->PieceC.Pieces[PieceIndex] = this;

}
void PieceController::RenderAll(Game* game)
{
	int FloatingPieceIndex = -1;
	for (int i = 0; i < game->PieceCount; i++)
	{
		if (Pieces[i]->IsFloating)
		{
			FloatingPieceIndex = i;
			continue;
		}
		Pieces[i]->Render(game->Renderer);
	}

	if (FloatingPieceIndex != -1)
	{
		Pieces[FloatingPieceIndex]->Render(game->Renderer);
	}


}



void Board::Init(SDL_Renderer* renderer)
{

	//TODO(osman): Buraya daha sonra wood texture yukleyebilirsin.
	Tools::CreateTexture(renderer, "Images/BlackRect.png", &Rects[0]);
	Tools::CreateTexture(renderer, "Images/WhiteRect.png", &Rects[1]);
	Tools::CreateTexture(renderer, "Images/WhiteExclamation.png", &Debugs[0]);
	Tools::CreateTexture(renderer, "Images/BlackExclamation.png", &Debugs[1]);
	Tools::CreateTexture(renderer, "Images/RedExclamation.png", &Debugs[2]);



}

void Board::DrawBoard(Game* game)
{
#ifdef DEBUG
	game->PieceC.CalcMoveMap(game);
#endif // DEBUG



	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{



			SDL_Rect Rect{ i * RectSize,j * RectSize,RectSize,RectSize };
			SDL_RenderCopy(game->Renderer, this->Rects[(i + j) % 2], 0, &Rect);


#ifdef DEBUG

			
			if (game->PieceC.WhiteMoveMap[j][i] && game->PieceC.BlackMoveMap[j][i])
			{
				SDL_RenderCopy(game->Renderer, this->Debugs[2], 0, &Rect);
			}
			else if (game->PieceC.BlackMoveMap[j][i])
			{
				SDL_RenderCopy(game->Renderer, this->Debugs[1], 0, &Rect);
			}
			else if (game->PieceC.WhiteMoveMap[j][i])
			{
				SDL_RenderCopy(game->Renderer, this->Debugs[0], 0, &Rect);
			}
#endif // DEBUG

		}
	}
}



void Piece::Render(SDL_Renderer* renderer)
{
	if (IsFloating)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		x -= RectSize / 2;
		y -= RectSize / 2;
		SDL_Rect Rect{ x,y ,RectSize,RectSize };
		SDL_RenderCopy(renderer, Texture, 0, &Rect);


	}
	else
	{
		SDL_Rect Rect{ Pos.x * RectSize,Pos.y * RectSize,RectSize,RectSize };
		SDL_RenderCopy(renderer, Texture, 0, &Rect);
	}


}

void Piece::Init()
{

}



Pawn::Pawn(Game* game, TilePos pos, Side color, PieceType type)
	:Piece(game)
{
	Pos = pos;
	Color = color;
	Type = type;
	if (color == Side_White)
	{
		Tools::CreateTexture(game->Renderer, "Images/PawnW.png", &Texture);
	}
	else
	{
		Tools::CreateTexture(game->Renderer, "Images/PawnB.png", &Texture);
	}
}
Rook::Rook(Game* game, TilePos pos, Side color, PieceType type)
	:Piece(game)
{
	Pos = pos;
	Color = color;
	Type = type;
	if (color == Side_White)
	{
		Tools::CreateTexture(game->Renderer, "Images/RookW.png", &Texture);
	}
	else
	{
		Tools::CreateTexture(game->Renderer, "Images/RookB.png", &Texture);
	}
}
Knight::Knight(Game* game, TilePos pos, Side color, PieceType type)
	:Piece(game)
{
	Pos = pos;
	Color = color;
	Type = type;
	if (color == Side_White)
	{
		Tools::CreateTexture(game->Renderer, "Images/KnightW.png", &Texture);
	}
	else
	{
		Tools::CreateTexture(game->Renderer, "Images/KnightB.png", &Texture);
	}
}
Bishop::Bishop(Game* game, TilePos pos, Side color, PieceType type)
	:Piece(game)
{
	Pos = pos;
	Color = color;
	Type = type;
	if (color == Side_White)
	{
		Tools::CreateTexture(game->Renderer, "Images/BishopW.png", &Texture);
	}
	else
	{
		Tools::CreateTexture(game->Renderer, "Images/BishopB.png", &Texture);
	}
}
Queen::Queen(Game* game, TilePos pos, Side color, PieceType type)
	:Piece(game)
{
	Pos = pos;
	Color = color;
	Type = type;
	if (color == Side_White)
	{
		Tools::CreateTexture(game->Renderer, "Images/QueenW.png", &Texture);
	}
	else
	{
		Tools::CreateTexture(game->Renderer, "Images/QueenB.png", &Texture);
	}
}
King::King(Game* game, TilePos pos, Side color, PieceType type)
	:Piece(game)
{
	Pos = pos;
	Color = color;
	Type = type;
	if (color == Side_White)
	{
		Tools::CreateTexture(game->Renderer, "Images/KingW.png", &Texture);
	}
	else
	{
		Tools::CreateTexture(game->Renderer, "Images/KingB.png", &Texture);
	}
}

void Piece::CalcMove(Game* game)
{
	game->PieceC.CalcPiecePositions(game);
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
		{
			MoveMap[j][i] = false;
		}

}


void Pawn::CalcMove(Game* game)
{
	Piece::CalcMove(game);

	if (Color == ColorAtBottom)
	{
		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x + 1, Pos.y - 1);
		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x - 1, Pos.y - 1);
		CheckSetReturn(MoveMap, game->PieceC.PlaceMap, Pos.x, Pos.y - 1);
		if (Bonus)
		{
			CheckSetReturn(MoveMap, game->PieceC.PlaceMap, Pos.x, Pos.y - 2);
		}

	}
	else
	{
		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x + 1, Pos.y + 1);
		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x - 1, Pos.y + 1);
		CheckSetReturn(MoveMap, game->PieceC.PlaceMap, Pos.x, Pos.y + 1);
		if (Bonus)
		{
			CheckSetReturn(MoveMap, game->PieceC.PlaceMap, Pos.x, Pos.y + 2);
		}
	}

}
void Rook::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	
	int i = 1;
	while (1)
	{

		CheckSetBreak(MoveMap,Pos.x, Pos.y - i);//up // noktali virgullerin hic bir anlami yok bos satir yani empty instruduction olarak geciyor gozume guzel gozuksun diye koydum zaten release modda compiler onu optimize eder.
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x, Pos.y + i);//down
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x - i, Pos.y);//left
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x + i, Pos.y);//right
		i++;
	}

}
void Knight::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	Set(MoveMap,Pos.x + 1, Pos.y - 2)
	Set(MoveMap,Pos.x - 1, Pos.y - 2)
	Set(MoveMap,Pos.x + 1 , Pos.y + 2)
	Set(MoveMap,Pos.x - 1, Pos.y + 2)
	Set(MoveMap,Pos.x - 2, Pos.y + 1)
	Set(MoveMap,Pos.x - 2, Pos.y - 1)
	Set(MoveMap,Pos.x + 2, Pos.y + 1)
	Set(MoveMap,Pos.x + 2, Pos.y - 1)

}
void Bishop::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	int i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x - i, Pos.y - i)//up left
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x - i, Pos.y + i)//down left
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x + i, Pos.y - i)//up rignht
			i++;
	}
	i = 1;

	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x + i, Pos.y + i)//down right
			i++;
	}

}
void Queen::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	
	int i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x, Pos.y - i)//up 
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x, Pos.y + i)//down
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x - i, Pos.y)//left
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x + i, Pos.y)//right
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x - i, Pos.y - i);//up left
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x - i, Pos.y + i);//down left
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x + i, Pos.y - i);//up rignht
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap,Pos.x + i, Pos.y + i);//down right
		i++;
	}
	


}
void King::CalcMove(Game* game)
{

}



#pragma region Danger

void Piece::CalcDanger(Game* game)
{
	game->PieceC.CalcPiecePositions(game);
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
		{
			DangerMap[j][i] = false;
		}

}


void Pawn::CalcDanger(Game* game)
{
	Piece::CalcDanger(game);

	if (Color == ColorAtBottom)
	{
		ReverseCheckSet(DangerMap, game->PieceC.PlaceMap, Pos.x + 1, Pos.y - 1);
		ReverseCheckSet(DangerMap, game->PieceC.PlaceMap, Pos.x - 1, Pos.y - 1);
		CheckSetReturn(DangerMap, game->PieceC.PlaceMap, Pos.x, Pos.y - 1);
		if (Bonus)
		{
			CheckSetReturn(DangerMap, game->PieceC.PlaceMap, Pos.x, Pos.y - 2);
		}

	}
	else
	{
		ReverseCheckSet(DangerMap, game->PieceC.PlaceMap, Pos.x + 1, Pos.y + 1);
		ReverseCheckSet(DangerMap, game->PieceC.PlaceMap, Pos.x - 1, Pos.y + 1);
		CheckSetReturn(DangerMap, game->PieceC.PlaceMap, Pos.x, Pos.y + 1);
		if (Bonus)
		{
			CheckSetReturn(DangerMap, game->PieceC.PlaceMap, Pos.x, Pos.y + 2);
		}
	}

}
void Rook::CalcDanger(Game* game)
{
	Piece::CalcDanger(game);

	int i = 1;
	while (1)
	{

		CheckSetBreak(DangerMap, Pos.x, Pos.y - i);//up // noktali virgullerin hic bir anlami yok bos satir yani empty instruduction olarak geciyor gozume guzel gozuksun diye koydum zaten release modda compiler onu optimize eder.
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x, Pos.y + i);//down
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x - i, Pos.y);//left
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x + i, Pos.y);//right
		i++;
	}

}
void Knight::CalcDanger(Game* game)
{
	Piece::CalcDanger(game);
	Set(DangerMap, Pos.x + 1, Pos.y - 2)
		Set(DangerMap, Pos.x - 1, Pos.y - 2)
		Set(DangerMap, Pos.x + 1, Pos.y + 2)
		Set(DangerMap, Pos.x - 1, Pos.y + 2)
		Set(DangerMap, Pos.x - 2, Pos.y + 1)
		Set(DangerMap, Pos.x - 2, Pos.y - 1)
		Set(DangerMap, Pos.x + 2, Pos.y + 1)
		Set(DangerMap, Pos.x + 2, Pos.y - 1)

}
void Bishop::CalcDanger(Game* game)
{
	Piece::CalcDanger(game);
	int i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x - i, Pos.y - i)//up left
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x - i, Pos.y + i)//down left
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x + i, Pos.y - i)//up rignht
			i++;
	}
	i = 1;

	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x + i, Pos.y + i)//down right
			i++;
	}

}
void Queen::CalcDanger(Game* game)
{
	Piece::CalcDanger(game);

	int i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x, Pos.y - i)//up 
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x, Pos.y + i)//down
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x - i, Pos.y)//left
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x + i, Pos.y)//right
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x - i, Pos.y - i);//up left
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x - i, Pos.y + i);//down left
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x + i, Pos.y - i);//up rignht
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(DangerMap, Pos.x + i, Pos.y + i);//down right
		i++;
	}



}
void King::CalcDanger(Game* game)
{

}

#pragma endregion








Move Piece::Move(TilePos Pos2, Game* game)
{
	this->CalcMove(game);
	if (MoveMap[Pos2.y][Pos2.x])
	{
		if (game->PieceC.PlaceMap[Pos2.y][Pos2.x])
		{
			for (int i = 0; i < game->PieceCount; i++)
			{
				if (i != PieceIndex && game->PieceC.Pieces[i]->Pos == Pos2)
				{
					if (game->PieceC.Pieces[i]->Color != Color)
					{
						game->PieceC.Pieces[i]->Destroy();

					}
					else //
					{
						std::cout << "Move Map Error!!!" << std::endl;
						return Move_SameColor;
					}

				}

			}
		}

		Pos = Pos2;
		Bonus = false;
		//Piece destroying stuff.
#ifdef DEBUG
		this->CalcMove(game);
#endif // DEBUG


	}
	else return Move_MoveMapIsNotTrue;

	//game->PieceC.CalcMoveMap(Color, game); //TODO make it work again
	return Move_Sucsess;
}

void Piece::Destroy()
{
	IsDestroyed = true;
	Pos.x = -1;
	Pos.y = -1;
	SDL_DestroyTexture(Texture);
}



void PieceController::CalcMoveMap(Game* game)
{
	//TODO maybe more cleaner solution
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
		{
			WhiteMoveMap[j][i] = false;
			BlackMoveMap[j][i] = false;
		}

	for (int a = 0; a < game->PieceCount; a++)
	{

		Pieces[a]->CalcMove(game);

		if (Pieces[a]->Color == Side_White)
		{
			for (int j = 0; j < 8; j++)
			{
				for (int i = 0; i < 8; i++)
				{
					WhiteMoveMap[j][i] = WhiteMoveMap[j][i] || Pieces[a]->MoveMap[j][i];
				}
			}
		}
		else
		{
			for (int j = 0; j < 8; j++)
			{
				for (int i = 0; i < 8; i++)
				{
					BlackMoveMap[j][i] = BlackMoveMap[j][i] || Pieces[a]->MoveMap[j][i];
				}
			}
		}



	}
}
void PieceController::CalcDangerMap(Game* game)
{

}
void PieceController::CalcPiecePositions(Game* game)
{
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
			PlaceMap[j][i] = Side_Empty;

	for (int i = 0; i < game->PieceCount; i++)
	{
		TilePos Pos = Pieces[i]->Pos;

		PlaceMap[Pos.y][Pos.x] = Pieces[i]->Color;
	}
}
inline bool PieceController::CheckSetFonc(bool MoveMap[8][8], int x, int y, Side color)
{
	if (PlaceMap[y][x] == Side_Empty && x >= 0 && x < 8 && y >= 0 && y < 8)
	{

		MoveMap[y][x] = true;
		return true;
	}
	else if (x < 0 || x > 7 || y < 0 || y > 7)
		return false;
	else if (PlaceMap[y][x] != color)
	{
		MoveMap[y][x] = true;
		return false;
	}
	else
	{
		return false;
	}
}





namespace Tools
{

	void CreateTexture(SDL_Renderer* renderer, const char* IMGDir, SDL_Texture** texture)
	{

		static SDL_Surface* loadingSrf = {};



		loadingSrf = IMG_Load(IMGDir);
		if (loadingSrf == NULL)
			SDL_LogError(1, IMG_GetError());

		*texture = SDL_CreateTextureFromSurface(renderer, loadingSrf);
		SDL_FreeSurface(loadingSrf);



	}

	void AlignPieces(Game* game)
	{
		for (int i = 0; i < 8; i++)
		{
			Pawn* PieceW = new Pawn(game, TilePos(i, 1), Side_Black, Type_Pawn);
			Pawn* PieceB = new Pawn(game, TilePos(i, 6), Side_White, Type_Pawn);

		}
		for (int i = 0; i < 2; i++)
		{
			Rook* PieceW = new Rook(game, TilePos(i * 7, 0), Side_Black, Type_Rook);
			Rook* PieceB = new Rook(game, TilePos(i * 7, 7), Side_White, Type_Rook);

		}
		for (int i = 0; i < 2; i++)
		{
			Knight* PieceW = new Knight(game, TilePos(1 + i * 5, 0), Side_Black, Type_Knight);
			Knight* PieceB = new Knight(game, TilePos(1 + i * 5, 7), Side_White, Type_Knight);

		}
		for (int i = 0; i < 2; i++)
		{
			Bishop* PieceW = new Bishop(game, TilePos(2 + i * 3, 0), Side_Black, Type_Bishop);
			Bishop* PieceB = new Bishop(game, TilePos(2 + i * 3, 7), Side_White, Type_Bishop);

		}
		Queen* QueenW = new Queen(game, TilePos(3, 0), Side_Black, Type_Queen);
		Queen* QueenB = new Queen(game, TilePos(3, 7), Side_White, Type_Queen);

		King* KingW = new King(game, TilePos(4, 0), Side_Black, Type_King);
		King* KingB = new King(game, TilePos(4, 7), Side_White, Type_King);

		


	}




}