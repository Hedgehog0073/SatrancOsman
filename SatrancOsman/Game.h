#pragma once
#include "SDL.h"
#include "SDL_image.h"

#include <time.h>
//OYUN AYARLARI
#define RectSize 60 // Kare boyutu. Kullandigim resimler 60x60 piksel oldugu icin bunu kullandim.
#define WindowWidth RectSize*11 // Pencere genisligi
#define WindowHeight RectSize*9 // Pencere yuksekligi
#define ColorAtBottom Side_White// Alt tarafta durucak takim
#define TargetTimeForFrame 10 // 1 kare icin harcanacak hedef sure.


//DEBUG VE DEBUG TOOLARI
//#define DEBUG // Debug texturelerini yuklemek icin.
//#define MOVEMAP //Takimlarin MoveMapini gormek icin. Siyah MoveMap siyah unlem, Beyaz MoveMap beyaz unlem.
//#define DANGERMAP //Takimlarin DangerMapini gormek icin. Siyah DangerMap siyah unlem, Beyaz DangerMap beyaz unlem.



//if Place empty true else return
//Eger placeMapdaki yer bos ise MoveMapi true yap degil ise return yap;
#define CheckSetReturn(MoveMap,PlaceMap,x,y) if (PlaceMap[y][x].Color == Side_Empty && y >= 0 && x >=0 && y<8 && x<8)MoveMap[y][x] = true;\
else return;
//CheckSet fonksiyonu dogru donerse devam et donmezse break;
//Bunu fonksiyona koydum cunku debug yaparken makrolar precompileda yazildigi icin bakamiyorsunuz. Buda bana sikinti cikartinca boyle bir cozum buldum.
#define CheckSetBreak(Map,x,y) if(game->PieceC.CheckSetFonc(Map,x,y,Color,true) == false)break;
//Ustteki macro ile ayni. Sadece danger icin calisiyor bu onuda fonksiyona parametre olarak soyluyoruz.
#define DangerCheckSetBreak(Map,x,y) if(game->PieceC.CheckSetFonc(Map,x,y,Color,false) == false)break;
//Bu sah icin olan bir macro eger sahin gidecegi yer DangerMapta isaretlise gidebilecegi yeri false yapiyoruz.
#define RemoveSet(DangerMap,y,x) if (x >= 0 && y >= 0 && x < 8 && y < 8 && DangerMap[y][x]) MoveMap[y][x] = false;

//Sadece sinirlar icinde ve x y de kendi takimimizdan tas yok ise isaretliyoruz.
#define Set(Map,x,y) game->PieceC.CheckSetFonc(Map,x,y,Color,true);
// Ayni sekilde calisiyor fakat bunda gitcegimiz yerde kendi tasimiz var ise onuda isaretliyoruz.
#define DangerSet(Map,x,y) game->PieceC.CheckSetFonc(Map,x,y,Color,false);
//gidecegimiz yerde tas var ise isaretliyoruz bu piyonlarin capraz haraketi icin.
#define ReverseCheckSet(MoveMap,PlaceMap,x,y) if (PlaceMap[y][x].Color != Side_Empty && PlaceMap[y][x].Color != Color && y >= 0 && x >=0 && y<8 && x<8)MoveMap[y][x] = true;//if Place occipied



struct Game; // bazi yerlerde kullanicagimizdan ilk olarak declare ediyorum.
struct PieceController;


struct TilePos
{
	TilePos(int, int);//girilen degerleri x ve y'ye esitleyen basit bir constructor.
	TilePos();
	int x;
	int y;
	bool operator==(const TilePos& Pos2) // Karlisaltirmada kullaniyorum. Operotor overload ile x ve y degerleri esit ise true donuyorum.
	{
		if (Pos2.x == this->x && Pos2.y == this->y)
			return true;
		else return false;
	}
};
struct Vector2 // Bu sadece floating cisimler icin cok fazla kullanmadigim icin operatorlari eklemedim.
{
	float x;
	float y;
};
enum Move // Haraket fonksiyonu icin neden haraket edemedigini donebiliyoruz.
{
	Move_Sucsess = 1,
	Move_SameColor,
	Move_MoveMapIsNotTrue,
	Move_Imposible
};
enum Side // tahtadaki taraflar.
{
	Side_Empty = 0,
	Side_Black,
	Side_White
};
enum PieceType //Tas tipleri.
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
enum Condution // Oyun durumlari.
{
	Condution_Empty = 0,
	Condution_WhiteCheck,
	Condution_BlackCheck,
	Condution_WhiteMate,
	Condution_BlackMate,
	Condution_Draw

};
struct Castling //Rok icin
{
	bool IsValid = false; //Gecerli mi ?
	TilePos Pos; // Rok yapmasi icin gitmesi gereken pozisyon.
	int RookIndex; // Rok yapabilecegi tasin ID'si

};
struct CaptureWhilePassing //Gecerken alma icin.
{
	bool IsValid = false; // Gecerli mi?
	TilePos Pos; // Yapmasi icin gitmesi gereken posizyon.
	int PawnIndex; // CWP yapacagi tas.

};
struct Tile // Kare
{
	Side Color; //Karede bulunan tasin rengi.
	PieceType Type; //Karede bulunan tasin tipi
	int PieceID; // Bulunan tasin ID'si
};
	
struct Piece
{
	bool IsDestroyed = false; // Yok oldu mu ?
	bool IsFloating = false; // Kullanici tarafindan tutuluyor mu ?
	Vector2 FloatPos; //Kullanici tarafindan tutuluyor ise Pozisyonu.
	TilePos Pos; // Tahtadaki pozisyonu.
	SDL_Texture* Texture; // Resimi
	int PieceIndex; // ID'si
	PieceType Type; // Tasin Tipi
	Side Color; // Tasin rengi
	bool Bonus = true; // Bonusu var mi? bu tum taslarda yok kolaylik olsun diye buraya koydum.
	bool MoveMap[8][8] = {}; // Hareket haritasi
	bool DangerMap[8][8] = {}; // Tehdit haritasi
	Piece(Game* game); // Constructor Arrye eklemesi icin.
	Move Move(TilePos Pos2, Game*); // Haraket fonksiyonu
	void Destroy(); // Yok olma fonksiyonu. Ilerde isime yarayabilir diye yaptim.

	virtual void CalcMove(Game*); // Haraket hesapla virtual her tip icin ayri bir fonksiyon.
	virtual void CalcDanger(Game* game); // Tehdit hesapla virtual her tip icin ayri bir fonksiyon.

	void Render(SDL_Renderer* renderer); // Render yani renderere eklemek icin.


};
struct PieceController
{
	Piece* Pieces[34]; // Oyundaki tum taslar Bu eger ileride sorun cikarirsa std::vector ile dynamic bir arraye aktarilabilir.
	bool WhiteMoveMap[8][8]; // Beyaz taslarin toplu hareket haritasi.
	bool BlackMoveMap[8][8]; // Siyah taslarin toplu hareket haritasi.
	bool WhiteDangerMap[8][8]; //Squares dangerours for White pieces //Beyaz icin tehlikeli kareler.
	bool BlackDangerMap[8][8];	//Squares dangerours for Black pieces //Siyah icin tehlikeli kareler.
	Tile PlaceMap[8][8]; // Taslarin Konum haritasi.

	void CalcMoveMap(Game*); // Tum tas ve takimlarin hareket haritasini hesaplamak icin.
	Side CalcDangerMap(Game*, bool); // Tum tas ve takimlarin tehdit haritasini hesaplamak icin.

	void CalcPiecePositions(Game*); // Tum taslarin bulundugu yerleri PlaceMapa eklemek icin.
	void RenderAll(Game* game); // Tum taslari renderlemek icin.
	bool CheckSetFonc(bool Movemap[8][8], int x, int y, Side, bool); // Haraket ve tehdit hesaplamada kullaniliyor. Debug yapmasi kolay olsun diye fonksiyon yaptim.
	void CalcAll(Game*); // Tum haritalari hesaplamak icin.


};
//CalcDanger ve CalcMove taslara ozel harita hesaplamalari.
//Piyon
struct Pawn :Piece
{
	Pawn(Game* game, TilePos pos, Side color, PieceType type);
	bool CanCapturedWhilePassing = false; // Gecerken alinmaya uygun mu ?
	bool DontRepeat = true; // Bu Uygun olmasinin tekrarlanmamasi icin.
	CaptureWhilePassing CWP[2]; // gecerken alma var ise bu structi dolduruyoruz.
	void Promotion(Game*); // Piyon degisimi icin. 
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;

};
//Kale
struct Rook :Piece
{
	Rook(Game* game, TilePos pos, Side color, PieceType type);
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;

};
//At
struct Knight :Piece
{
	Knight(Game* game, TilePos pos, Side color, PieceType type);
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;

};
//Fil
struct Bishop :Piece
{
	Bishop(Game* game, TilePos pos, Side color, PieceType type);
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;

};
//Vezir
struct Queen :Piece
{
	Queen(Game* game, TilePos pos, Side color, PieceType type);
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;

};
//Sah
struct King :Piece 
{
	King(Game* game, TilePos pos, Side color, PieceType type);
	SDL_Texture* CheckTexture;
	bool IsCheck = false; // Sah mi yani tehdit altinda mi ?
	bool IsTesting = false; // Mat icin Test ediliyor mu ?
	void Check(Game*); // Sah fonksiyonumuz.
	void Mate(Game*); // Mat icin fonksiyon.
	void CalcDanger(Game* game) override;
	void CalcMove(Game*) override;
	Castling Cast[2];// Rok icin eger bulunuyor ise doldurmamiz gereken struct.
};
struct MateDummy : Piece // Mat hesaplamalari icin kullanacagimiz kukla.
{
	MateDummy(Game* game, TilePos pos, Side color, PieceType type);
	int Test(Game*, int x, int y); // İsteniler yerde tas yok ise kendini oraya yerlestiriyor var ise orda olan tasi yok edip ID'sini donuyor.

};

struct Board
{
	SDL_Texture* Rects[2]; // Kareler Siyah Beyaz
	SDL_Texture* Debugs[3]; // Debuglar beyaz,Siyah ikisinin kesistigi yer icin ise kirmizi.
	SDL_Texture* Letters[8]; // Harfler
	SDL_Texture* Numbers[8]; // Rakamlar
	SDL_Texture* Sides[2]; // Sag taraftaki bildirim icin Renkler
	SDL_Texture* Condutions[2]; // Sag taraftaki icin bildirimler; sah, sah mat.
	SDL_Texture* Float[2]; // Kullanici tasi tuttugundaki verdigimiz ipucular icin.

	void Init(SDL_Renderer* renderer); // Textureleri disten bellege yuklemek icin.
	
	void DrawBoard(Game* game); // Tahtayi cizmek icin.

};

struct Game
{
	void Init(); // Oyunu baslatmak icin.
	void HandleEvents(); // Oyunun input bekleyip gelen inputa cevap vermesi icin oyun sonuna kadar calisacak dongu.
	SDL_Window* Window; // Oyun Penceresi
	SDL_Renderer* Renderer = nullptr; // Oyun rendereri
	bool Running = true; // Handle eventsin icinde sonsuz dongunun oyun sonuna kadar devam etmesi icin calisiyor mu.
	Board ChessBoard; // Satranc tahtasi
	int PieceCount = 0; // Tas sayisi 
	PieceController PieceC; // tas kontrolcusu
	void Render(); // herseyi renderlemek icin
	Condution Cond = Condution_Empty; // Oyun durumu.
	int HoldItemIndex = -1; // Oyuncunun tuttugu tas var ise indexi yok ise -1
	Side Turn = Side_White; // Sira
	void EndGame(); // Oyunu bitirmek icin.
};


namespace Tools
{
	void CreateTexture(SDL_Renderer* renderr, const char* IMGDir, SDL_Texture** texture); // Diskten texture yuklemek icin.
	void AlignPieces(Game* game); // Taslari olusturup tahtaya cizmek icin.

}
