#pragma once
#include <SDL.h>
#include <SDL_image.h>

#include <time.h>

#define WindowWidth 60*8
#define WindowHeight 60*8
#define RectSize WindowWidth/8
#define ColorAtBottom Side_White
#define TargetTimeForFrame 10
#define ArrayCount(x) (sizeof(x)/sizeof(x[0]))


//if Place empty true else return;
#define CheckSetReturn(MoveMap,PlaceMap,x,y) if (!PlaceMap[y][x] && y >= 0 && x >=0 && y<8 && x<8)MoveMap[y][x] = true;\
else return;

#define CheckSetBreak(Map,x,y) if(game->PieceC.CheckSetFonc(Map,x,y,Color) == false)break;
#define Set(Map,x,y) game->PieceC.CheckSetFonc(Map,x,y,Color);

#define CheckSet(MoveMap,PlaceMap,x,y) if (!PlaceMap[y][x])MoveMap[y][x] = true;//if Place is empty
#define ReverseCheckSet(MoveMap,PlaceMap,x,y) if (PlaceMap[y][x])MoveMap[y][x] = true;//if Place occipied





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
	Move_MoveMapIsNotTrue

};
enum Side
{
	Side_Empty = 0,
	Side_Black,
	Side_White
};
enum PieceType
{
	Type_Pawn = 1,
	Type_Rook,
	Type_Knight,
	Type_Bishop,
	Type_Queen,
	Type_King,
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
	virtual void Init();
	virtual void CalcMove(Game*);
	virtual void CalcDanger(Game* game);
	void Render(SDL_Renderer* renderer); 
	
};
struct PieceController
{
	bool DebugMoveMap[8][8];
	Piece* Pieces[32];
	bool WhiteMoveMap[8][8];
	bool BlackMoveMap[8][8];
	Side PlaceMap[8][8]; 
	void CalcMoveMap(Game*);
	void CalcDangerMap(Game*);
	void CalcPiecePositions(Game*);
	void RenderAll(Game* game);
	bool CheckSetFonc(bool Movemap[8][8],int x,int y,Side);
	
};
struct Pawn :Piece
{
	Pawn(Game* game, TilePos pos, Side color, PieceType type);
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;

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
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;
	
};
struct Board
{
	SDL_Texture* Rects[2];
	SDL_Texture* Debugs[3];
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
};


namespace Tools
{
	void CreateTexture(SDL_Renderer* renderer, const char* IMGDir, SDL_Texture** texture);
	void AlignPieces(Game* game);

}