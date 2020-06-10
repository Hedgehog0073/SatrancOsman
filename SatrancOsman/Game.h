#pragma once
#include "SDL.h"
#include "SDL_image.h"

#include <time.h>
//OYUN AYARLARI
#define WindowWidth 60*11
#define WindowHeight 60*9
#define RectSize 60
#define ColorAtBottom Side_White
#define TargetTimeForFrame 10


//DEBUG VE DEBUG TOOLARI
//#define MOVEMAP
#define DEBUG
//#define DANGERMAP



//if Place empty true else return;
#define CheckSetReturn(MoveMap,PlaceMap,x,y) if (PlaceMap[y][x].Color == Side_Empty && y >= 0 && x >=0 && y<8 && x<8)MoveMap[y][x] = true;\
else return;

#define CheckSetBreak(Map,x,y) if(game->PieceC.CheckSetFonc(Map,x,y,Color,true) == false)break;
#define DangerCheckSetBreak(Map,x,y) if(game->PieceC.CheckSetFonc(Map,x,y,Color,false) == false)break;

#define ForEach(i) for (int i = 0; i < game->PieceCount && game.Pieces[i].IsDestroyed == false; i++)

#define RemoveSet(DangerMap,y,x) if (x >= 0 && y >= 0 && x < 8 && y < 8 && DangerMap[y][x]) MoveMap[y][x] = false;

#define Set(Map,x,y) game->PieceC.CheckSetFonc(Map,x,y,Color,true);
#define DangerSet(Map,x,y) game->PieceC.CheckSetFonc(Map,x,y,Color,false);

#define CheckSet(MoveMap,PlaceMap,x,y) if (!PlaceMap[y][x])MoveMap[y][x] = true;//if Place is empty
#define ReverseCheckSet(MoveMap,PlaceMap,x,y) if (PlaceMap[y][x].Color != Side_Empty && PlaceMap[y][x].Color != Color && y >= 0 && x >=0 && y<8 && x<8)MoveMap[y][x] = true;//if Place occipied





struct Game;
struct PieceController;

struct TilePos
{
	TilePos(int, int);
	TilePos();
	int x;
	int y;
	bool operator==(const TilePos& Pos2)
	{
		if (Pos2.x == this->x && Pos2.y == this->y)
			return true;
		else return false;
	}
};
struct Vector2
{
	float x;
	float y;
};
enum Move
{
	Move_Sucsess = 1,
	Move_SameColor,
	Move_MoveMapIsNotTrue,
	Move_Imposible
};
enum Side
{
	Side_Empty = 0,
	Side_Black,
	Side_White
};
enum PieceType
{
	Type_Empty = 0,
	Type_Pawn = 1,
	Type_Rook,
	Type_Knight,
	Type_Bishop,
	Type_Queen,
	Type_King,
	Type_Dummy
};
enum Condution
{
	Condution_Empty = 0,
	Condution_WhiteCheck,
	Condution_BlackCheck,
	Condution_WhiteMate,
	Condution_BlackMate,
	Condution_Draw

};
struct Castling
{
	bool IsValid = false;
	TilePos Pos;
	int RookIndex;

};
struct CaptureWhilePassing
{
	bool IsValid = false;
	TilePos Pos;
	int PawnIndex;

};
struct Tile
{
	Side Color;
	PieceType Type;
	int PieceID;
};
	
struct Piece
{


	int PieceMap[8][8];
	bool IsDestroyed = false;
	bool IsFloating = false;
	Vector2 FloatPos;
	TilePos Pos;
	SDL_Texture* Texture;
	int PieceIndex;
	PieceType Type;
	Side Color;
	bool Bonus = true;
	bool MoveMap[8][8] = {};
	bool DangerMap[8][8] = {};
	Piece(Game* game);
	Move Move(TilePos Pos2, Game*);
	void Destroy(); 

	virtual void CalcMove(Game*);
	virtual void CalcDanger(Game* game);

	void Render(SDL_Renderer* renderer);


};
struct PieceController
{
	Piece* Pieces[34];
	bool WhiteMoveMap[8][8];
	bool BlackMoveMap[8][8];
	bool WhiteDangerMap[8][8]; //Squares dangerours for White pieces //Beyaz icin tehlikeli kareler.
	bool BlackDangerMap[8][8];	//Squares dangerours for Black pieces //Siyah icin tehlikeli kareler.
	Tile PlaceMap[8][8]; 

	void CalcMoveMap(Game*);
	Side CalcDangerMap(Game*, bool);

	void CalcPiecePositions(Game*);
	void RenderAll(Game* game);
	bool CheckSetFonc(bool Movemap[8][8], int x, int y, Side, bool);
	void CalcAll(Game*);


};
struct Pawn :Piece
{
	Pawn(Game* game, TilePos pos, Side color, PieceType type);
	bool CanCapturedWhilePassing = false;
	bool DontRepeat = true;
	CaptureWhilePassing CWP[2];
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;
	void Promotion(Game*);

};
struct Rook :Piece
{
	Rook(Game* game, TilePos pos, Side color, PieceType type);
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;

};
struct Knight :Piece
{
	Knight(Game* game, TilePos pos, Side color, PieceType type);
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;

};
struct Bishop :Piece
{
	Bishop(Game* game, TilePos pos, Side color, PieceType type);
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;

};
struct Queen :Piece
{
	Queen(Game* game, TilePos pos, Side color, PieceType type);
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;

};
struct King :Piece
{
	King(Game* game, TilePos pos, Side color, PieceType type);
	SDL_Texture* CheckTexture;
	bool IsCheck = false;
	bool IsTesting = false;
	void Check(Game*);
	void Mate(Game*);
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;
	Castling Cast[2];
};
struct MateDummy : Piece
{
	MateDummy(Game* game, TilePos pos, Side color, PieceType type);
	int Test(Game*, int x, int y);

};

struct Board
{
	SDL_Texture* Rects[2];
	SDL_Texture* Debugs[3];
	SDL_Texture* Letters[8];
	SDL_Texture* Numbers[8];
	SDL_Texture* Sides[2];
	SDL_Texture* Condutions[2];
	SDL_Texture* Float[2];

	void Init(SDL_Renderer* renderer);

	void DrawBoard(Game* game);

};

struct Game
{
	void Init();
	void HandleEvents();
	SDL_Window* Window;
	SDL_Renderer* Renderer = nullptr;
	bool Running = true;
	Board ChessBoard;
	int PieceCount = 0;
	PieceController PieceC;
	void Render();
	Condution Cond = Condution_Empty;
	int HoldItemIndex = -1;
	Side Turn = Side_White;
	void EndGame();
};


namespace Tools
{
	void CreateTexture(SDL_Renderer* renderer, const char* IMGDir, SDL_Texture** texture);
	void AlignPieces(Game* game);

}
