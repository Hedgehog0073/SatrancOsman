#include "Game.h"
#include <iostream>



void Game::Init()
{
	//SDL kutuphanelerini init ediyorum.
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	//Pencere ve Renderer olusturuyorum
	Window = SDL_CreateWindow("Chess by Osman v4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, NULL);
	Renderer = SDL_CreateRenderer(Window, -1, 0);

	//Mouse icin focus windowu set ediyorum.
	SDL_SetWindowInputFocus(Window);
	//Arka Plan Rengini ayarliyorum.
	SDL_SetRenderDrawColor(Renderer, 0xEA, 0xD7, 0xC6, 255);

	//Satranc tahtasini init ediyorum. Yani tum gereken texturelari diskten bellege yukluyor.
	ChessBoard.Init(Renderer);
	//Satranc tahtasini render ediyorum.
	
   
	
	ChessBoard.DrawBoard(this);

	//Taslari olusturup tahtaya diziyorum.
	Tools::AlignPieces(this);

	//Renderledigim seyi gormek icin windowa yansitiyorum.
	SDL_RenderPresent(Renderer);
}

void Game::HandleEvents()
{
	/*Bu fonksiyon 3 asamadan olusuyor.
	1-Girdi al(Input)
	2-Tepki ver Hesapla(Calculate)
	3-Ciz(Render)
	oyun bitene kadar bu asamalari tekrarliyor.
	*/

	SDL_Event Event;//input icin event structi

	while (Running)
	{

		Uint64 StartTime = SDL_GetPerformanceCounter(); // Oyunun FPS'ini yani saniye basina kare sayisini hesaplamak icin.

		if (SDL_PollEvent(&Event))//PollEvent ile input aliyoruz.
		{
			int x, y;
			switch (Event.type)//Eventin tipine gore switch yapiyoruz.
			{
			case(SDL_QUIT)://Pencerenin Kapatma tusuna basildiginda kapatmak icin.
				Running = false;
				break;
			case(SDL_KEYDOWN)://Klavyede eger bir tusa basilirsa
				switch (Event.key.keysym.sym)
				{
				case(SDLK_ESCAPE):// Basilan tus escape ise programi kapatiyoruz.
					Running = false;
					break;
				case(SDLK_m):
					PieceC.CalcMoveMap(this);
					break;
				}
				break;
			case(SDL_MOUSEBUTTONDOWN)://kullanici mouseye bastiginda
				//Mousenin Pencerenin sol ustune gore konumunu input ediyoruz.
				SDL_GetMouseState(&x, &y);
				// Kare boyunuta bolerek mousenin kare olarak konumunu buluyorum.
				x = x / RectSize;
				y = y / RectSize;
				//Daha sonra mousenin konumunda olan bir tas ariyorum ve buldugumda onun indexini isaretliyorum.
				for (int i = 0; i < PieceCount; i++)
				{
					if (PieceC.Pieces[i]->IsDestroyed == true)
						continue;


					if (PieceC.Pieces[i]->Pos.x == x && PieceC.Pieces[i]->Pos.y == y && PieceC.Pieces[i]->Color == Turn)
					{
						PieceC.Pieces[i]->IsFloating = true;//Kullanicinin Mouse konumunda tasi renderlemek icin
						HoldItemIndex = i;
					}
				}
				break;

			case(SDL_MOUSEBUTTONUP)://Kullanici Mouseyi basip biraktiginda
				if (HoldItemIndex != -1)//Eger bastiginda tuttugu tas var ise
				{

					PieceC.Pieces[HoldItemIndex]->IsFloating = false;// Artik mouse konumunda tasi renderlememek icin.
					//Mouse konumunu aliyorum.
					SDL_GetMouseState(&x, &y);
					x = x / RectSize;
					y = y / RectSize;

					//Eger yapilan hareket basarili ise sirayi degistiriyorum.
					if (PieceC.Pieces[HoldItemIndex]->Move(TilePos(x, y), this) == Move_Sucsess)
						if (Turn == Side_White)
							Turn = Side_Black;
						else
							Turn = Side_White;


					HoldItemIndex = -1;// Isaretledigim tasin isaretini kaldiriyorum.

				}
				break;


			}

		}
		//Tum input islemleri tamamlandiktan sonra rendere basliyorum
		Render();


		//Burasi FPS'i sabit tutmak icin.
		/*Ornegin oyunumun FPS'sini eger 100 FPS olarak sabit tutmak istiyorsam:
		kod basladiginda cycle sayisini almistim zaten simdi ise kod bittigi zaman cycle sayisini aliyorum.
		Daha sonra sondaki cycledan bastaki cycleyi cikararak Delta cycleyi buluyorum.
		Daha sonra DeltaCycleyi Islemci hizina bolerek bastan sona gecen saniyeyi buluyorum ve buda DeltaTime oluyor.
		Delta time oyunlarda oyunun islemcinin hizina gore degilde zamana gore calismasi istendiginde kullaniliyor.
		En sonda DeltaTimeyi 1000 ile carparak ms birimine ceviriyorum.
		Oyunda istedigim FPS 100 oldugundan 1 saniyede 100 frame renderlemek istiyorsak ozaman 1 frame icin
		1s/100frame kadar zaman harcalamaliyiz saniyeyide ms'e cevirirsek 1000ms/100frame ozaman 1 frame icin 10ms harcamamiz
		lazim. bunun icinde oyunu 10-(gecen zaman) kadar uyutuyoruz.
		*/

		Uint64 EndTime = SDL_GetPerformanceCounter();
		double DeltaTime = (double)((EndTime - StartTime) * 1000) / (double)SDL_GetPerformanceFrequency();
		if (DeltaTime < TargetTimeForFrame)
		{
			SDL_Delay(TargetTimeForFrame - DeltaTime); // yukardaki islemden cikan sec bana ms lazim odan 1000 ile carpma var
			EndTime = SDL_GetPerformanceCounter();

		}
		EndTime = SDL_GetPerformanceCounter();
		DeltaTime = (double)((EndTime - StartTime) * 1000) / (double)SDL_GetPerformanceFrequency();
		/*SDL_Log("Time elapsed for frame is %f", DeltaTime); // 1 Kare icin gecen zaman ms olarak.
		SDL_Log("FPS: %f", 1000/DeltaTime);// 1 saniyede gecen kare
		SD_Log("%f", DeltaTime);*/


	}
	//WINNER ANNONCMENT
	SDL_Texture* Winnner;
	if (Cond == Condution_WhiteMate)
	{
		Tools::CreateTexture(Renderer, "Images/WinnerIsBlack.png", &Winnner);

		SDL_RenderCopy(Renderer, Winnner, 0, 0);
		SDL_RenderPresent(Renderer);
		std::cout << "Winner is Black" << std::endl;
		SDL_Delay(10000);
	}
	else if (Cond == Condution_BlackMate)
	{
		Tools::CreateTexture(Renderer, "Images/WinnerIsWhite.png", &Winnner);

		SDL_RenderCopy(Renderer, Winnner, 0, 0);
		SDL_RenderPresent(Renderer);

		std::cout << "Winner is White" << std::endl;
		SDL_Delay(10000);
	}
	else if (Cond == Condution_Draw)
	{
		Tools::CreateTexture(Renderer, "Images/Draw.png", &Winnner);

		SDL_RenderCopy(Renderer, Winnner, 0, 0);
		SDL_RenderPresent(Renderer);

		std::cout << "Draw" << std::endl;
		SDL_Delay(10000);
	}





}
void Game::Render()
{
	//Bir onceki frameden kalan seyleri ekrandan temizliyorum.
	SDL_RenderClear(Renderer);
	//Satranc Tahtasini ciziyorum.
	ChessBoard.DrawBoard(this);
	//Taslarin hepsini konumlarinda renderliyorum
	PieceC.RenderAll(this);
	//Renderde olan seyleri ekrana yansitiyorum.
	SDL_RenderPresent(Renderer);
}

void Game::EndGame()
{
	

	Running = false;


}
TilePos::TilePos(int X, int Y) : x{ X }, y{ Y }{}
TilePos::TilePos() {}



Piece::Piece(Game* game)
{
	//tasi arrarye yerlestirmek icin.
	PieceIndex = game->PieceCount++;
	game->PieceC.Pieces[PieceIndex] = this;


}
void PieceController::RenderAll(Game* game)
{
	int FloatingPieceIndex = -1;
	for (int i = 0; i < game->PieceCount; i++)
	{
		// Eger tas yok olmus ise renderlemiyorum.
		if (game->PieceC.Pieces[i]->IsDestroyed == true)
			continue;

		//Eger tas kullanici tarafindantutuluyorsa burda renderlemiyorum.
		//Cunku kullanicinin tuttugu tasin diger taslarin ustunde gozukmesini istiyorum bunun icinde kullanicinin tuttugu tasi
		// en son renderleyecegim.
		if (Pieces[i]->IsFloating)
		{
			FloatingPieceIndex = i;
			continue;
		}
		//tasa Renderlenmesini soyluyorum.
		Pieces[i]->Render(game->Renderer);
	}
	//Daha oncede dedigim gibi tasin diger taslarin ustunde gozukmesi icin en son renderliyorum.
	if (FloatingPieceIndex != -1)
	{
		Pieces[FloatingPieceIndex]->Render(game->Renderer);
	}


}



void Board::Init(SDL_Renderer* renderer)
{

	// Burada MoveMap ve DangerMap icin kullandigim debug textureleri yukluyorum.
	// Bunlari kullanmak icin Game.h dosyasindaki MOVEMAP veya DANGERMAP definelarini comment out yapin.
	Tools::CreateTexture(renderer, "Images/NewBlackRect.png", &Rects[0]);
	Tools::CreateTexture(renderer, "Images/NewWhiteRect.png", &Rects[1]);
#ifdef DEBUG
	Tools::CreateTexture(renderer, "Images/WhiteExclamation.png", &Debugs[0]);
	Tools::CreateTexture(renderer, "Images/BlackExclamation.png", &Debugs[1]);
	Tools::CreateTexture(renderer, "Images/RedExclamation.png", &Debugs[2]);
#endif // DEBUG



	//Tahtanin harf ve yazilari burada SDL_TTF kullanabilirdim fakat 
	//yazacagim yazi az oldugu icin ekstra bir kutuphane eklemek istemedim.
	Tools::CreateTexture(renderer, "Images/A.png", &Letters[0]);
	Tools::CreateTexture(renderer, "Images/B.png", &Letters[1]);
	Tools::CreateTexture(renderer, "Images/C.png", &Letters[2]);
	Tools::CreateTexture(renderer, "Images/D.png", &Letters[3]);
	Tools::CreateTexture(renderer, "Images/E.png", &Letters[4]);
	Tools::CreateTexture(renderer, "Images/F.png", &Letters[5]);
	Tools::CreateTexture(renderer, "Images/G.png", &Letters[6]);
	Tools::CreateTexture(renderer, "Images/H.png", &Letters[7]);

	Tools::CreateTexture(renderer, "Images/1.png", &Numbers[0]);
	Tools::CreateTexture(renderer, "Images/2.png", &Numbers[1]);
	Tools::CreateTexture(renderer, "Images/3.png", &Numbers[2]);
	Tools::CreateTexture(renderer, "Images/4.png", &Numbers[3]);
	Tools::CreateTexture(renderer, "Images/5.png", &Numbers[4]);
	Tools::CreateTexture(renderer, "Images/6.png", &Numbers[5]);
	Tools::CreateTexture(renderer, "Images/7.png", &Numbers[6]);
	Tools::CreateTexture(renderer, "Images/8.png", &Numbers[7]);


	// Bildirimler icin
	Tools::CreateTexture(renderer, "Images/LogWhite.png", &Sides[0]);
	Tools::CreateTexture(renderer, "Images/LogBlack.png", &Sides[1]);

	Tools::CreateTexture(renderer, "Images/LogCheck.png", &Condutions[0]);
	Tools::CreateTexture(renderer, "Images/LogMate.png", &Condutions[1]);

	//Kullaniciya hareket ipucusu vermek icin.

	Tools::CreateTexture(renderer, "Images/CanMove.png", &Float[0]);
	Tools::CreateTexture(renderer, "Images/MousePos.png", &Float[1]);


}

void Board::DrawBoard(Game* game)
{

	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			//Satranc tahtasini bir matris olarak dusunursek eger matrisnin x ve y bilesenlerinin toplami cift ise beyaz
			//Tek ise siyah olmali bunu tam tersinide yapabilirsiniz.
			SDL_Rect Rect{ i * RectSize,j * RectSize,RectSize,RectSize };
			SDL_RenderCopy(game->Renderer, this->Rects[(i + j) % 2], 0, &Rect);

			// MoveMap ve DangerMapi satranca yansitmak icin. Debug yaparken COOK yardimci oldu.
			// aktive etmek icin Game.h dosyasindaki MOVEMAP  veya DangerMap definelerini comment out yapin.
#ifdef MOVEMAP


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
#endif // MOVEMAPS
#ifdef DANGERMAP


			if (game->PieceC.WhiteDangerMap[j][i] && game->PieceC.BlackDangerMap[j][i])
			{
				SDL_RenderCopy(game->Renderer, this->Debugs[2], 0, &Rect);
}
			else if (game->PieceC.BlackDangerMap[j][i])
			{
				SDL_RenderCopy(game->Renderer, this->Debugs[1], 0, &Rect);
			}
			else if (game->PieceC.WhiteDangerMap[j][i])
			{
				SDL_RenderCopy(game->Renderer, this->Debugs[0], 0, &Rect);
			}
#endif // MOVEMAPS

		}
	}
	// Kullaniciya ipucu vermek icin.
	if (game->HoldItemIndex != -1)
	{
		//Kullanicinin tuttugu tasin Movemapini oyuna yansitiyoruz.
		for (int j = 0; j < 8; j++)
			for (int i = 0; i < 8; i++)
				if (game->PieceC.Pieces[game->HoldItemIndex]->MoveMap[j][i])
				{
					SDL_Rect Rect{ i * RectSize,j * RectSize,RectSize,RectSize };
					SDL_RenderCopy(game->Renderer, this->Float[0], 0, &Rect);
				}
		//Kullanicinin koydugu tasin nereye gidecegini gostermek icin Mouseposisyonunu tahtaya yansitiyoruz.
		int x, y;
		SDL_GetMouseState(&x, &y);//Mouse input
		//Mousenin Piksel pozisyonundan kurtulup Kare konumunu aliyoruz.
		x = x / RectSize;
		y = y / RectSize;
		// Mousenin bulundugu kare konumunda textureyi renderliyoruz.
		SDL_Rect Rect{ x * RectSize,y * RectSize,RectSize,RectSize };
		SDL_RenderCopy(game->Renderer, this->Float[1], 0, &Rect);
	}



	//Satranc tahtasinin Harf ve Sayilarini yazdiriyoruz.
	for (int i = 0; i < 8; i++)
	{
		int j = 8;
		SDL_Rect Rect{ i * RectSize,j * RectSize,RectSize,RectSize };
		SDL_RenderCopy(game->Renderer, Letters[i], 0, &Rect);

		SDL_Rect RectNumbers{ j * RectSize,(7 - i) * RectSize,RectSize,RectSize };
		SDL_RenderCopy(game->Renderer, Numbers[i], 0, &RectNumbers);
	}


	//Eger oyunda sah veya sahmat varsa sag taraftaki log kismina yazidiriyoruz.
	SDL_Rect Rect{ 9 * RectSize,0,2 * RectSize,RectSize };
	SDL_Rect Rect1{ 9 * RectSize,1 * RectSize,2 * RectSize,RectSize };
	switch (game->Cond)
	{


	case Condution_Empty:
		break;
	case Condution_WhiteCheck:

		SDL_RenderCopy(game->Renderer, Sides[0], 0, &Rect);

		SDL_RenderCopy(game->Renderer, Condutions[0], 0, &Rect1);
		break;
	case Condution_BlackCheck:
		SDL_RenderCopy(game->Renderer, Sides[1], 0, &Rect);
		SDL_RenderCopy(game->Renderer, Condutions[0], 0, &Rect1);
		break;
	case Condution_WhiteMate:
		SDL_RenderCopy(game->Renderer, Sides[0], 0, &Rect);
		SDL_RenderCopy(game->Renderer, Condutions[1], 0, &Rect1);
		break;
	case Condution_BlackMate:
		SDL_RenderCopy(game->Renderer, Sides[1], 0, &Rect);
		SDL_RenderCopy(game->Renderer, Condutions[1], 0, &Rect1);
		break;
	default:
		break;
	}

}



void Piece::Render(SDL_Renderer* renderer)
{
	// eger tas sah ve tehtit altindaysa yani sah cekilmisse tehtit olarak sahin arkasindaki textureyi yukluyorum.
	if (Type == Type_King)
	{
		King* king = (King*)this;//Saha ozel degiskenlere ulasmak icin sah oldugunu bildigim tasi saha cast ediyorum.
		if (king->IsCheck)//sah mi diye kontrol ediyorum.
		{
			SDL_Rect Rect{ Pos.x * RectSize,Pos.y * RectSize,RectSize,RectSize };
			SDL_RenderCopy(renderer, king->CheckTexture, 0, &Rect);

		}
	}
	//Eger kullanici tasi tutuyorsa mousenin oldugu yerde render ediyorum.
	if (IsFloating)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		//burda mouse pozisyonunun eksenlerinden tas boyutunun yarisi cikararak tasin mousenin tam tasin ortasinda olmasini sagliyorum.
		x -= RectSize / 2;
		y -= RectSize / 2;
		SDL_Rect Rect{ x,y ,RectSize,RectSize };
		SDL_RenderCopy(renderer, Texture, 0, &Rect);


	}
	else
	{
		//Eger kullanici tarafindan tutulmuyorsa bulundugu yerde render ediyorum.
		SDL_Rect Rect{ Pos.x * RectSize,Pos.y * RectSize,RectSize,RectSize };
		SDL_RenderCopy(renderer, Texture, 0, &Rect);
	}


}




//consturctorlarda istenilen tas olusturulup parametrede verilen Posisyonla kendi pozisyonu esitleniyor.
#pragma region PieceConstructors

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
	Cast[0].IsValid = false;
	Cast[1].IsValid = false;

	if (color == Side_White)
	{
		Tools::CreateTexture(game->Renderer, "Images/KingW.png", &Texture);
		Tools::CreateTexture(game->Renderer, "Images/Check.png", &CheckTexture);

	}
	else
	{
		Tools::CreateTexture(game->Renderer, "Images/KingB.png", &Texture);
		Tools::CreateTexture(game->Renderer, "Images/Check.png", &CheckTexture);


	}
}
MateDummy::MateDummy(Game* game, TilePos pos, Side, PieceType)
	:Piece(game)
{
	Type = Type_Dummy;
	Pos = pos;
	IsDestroyed = true;


}
#pragma endregion

//Bu bolgede her tasin kurallarina ozel olarak haraket haritalarini hesapliyoruz.
#pragma region CalcMove

void Piece::CalcMove(Game* game)
{
	//game->PieceC.CalcPiecePositions(game);

	//Her hesaplamadan once bu fonksiyonu cagirarak haritalari temizliyoruz.
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
		{
			MoveMap[j][i] = false;
		}

}


//Piyon icin
void Pawn::CalcMove(Game* game)
{
	Piece::CalcMove(game);




	//Bu tasin gecerken alma icin uygun olup olmadigini ayarliyoruz.
	if (CanCapturedWhilePassing == true && DontRepeat == true)
	{
		DontRepeat = false;
	}
	else if (CanCapturedWhilePassing == true)
	{
		CanCapturedWhilePassing = false;
	}


	if (Color == ColorAtBottom)//Alttaki taraftaki piyonlar icin
	{
		if (Pos.y == 0)//eger sona geldiyse promotion yap
		{
			Promotion(game);
		}
		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x + 1, Pos.y - 1);// Sag ustte tas varsa true yap
		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x - 1, Pos.y - 1);// Sol ustte tas varsa true yap

		//Burda sadece 2 işlem olduğu için macro yapmak istemedim.
		//Bu bolgede Gecerken almayi kontrol ediyoruz.
		//Eger saginda tas varsa , karsi takimdansa ve turu piyonsa
#pragma region Capture While Passing
		//eger saginda tas var,karsi takimdan ve piyonsa
		if (Pos.x + 1 < 8 &&
			game->PieceC.PlaceMap[Pos.y][Pos.x + 1].Color != Side_Empty &&
			game->PieceC.PlaceMap[Pos.y][Pos.x + 1].Color != Color &&
			game->PieceC.PlaceMap[Pos.y][Pos.x + 1].Type == Type_Pawn)
		{

			Pawn* TestPawn = (Pawn*)game->PieceC.Pieces[game->PieceC.PlaceMap[Pos.y][Pos.x + 1].PieceID];
			//Gecerken almaya uygun mu diye bak.
			if (TestPawn->CanCapturedWhilePassing)
			{
				//Uygunsa CWP'yi doldur.
				MoveMap[Pos.y - 1][Pos.x + 1] = true;
				CWP[0].IsValid = true;
				CWP[0].PawnIndex = TestPawn->PieceIndex;
				CWP[0].Pos = TilePos(Pos.x + 1, Pos.y - 1);
			}


		}
		//ayni islem ustte.
		if (Pos.x - 1 >= 0 &&
			game->PieceC.PlaceMap[Pos.y][Pos.x - 1].Color != Side_Empty &&
			game->PieceC.PlaceMap[Pos.y][Pos.x - 1].Color != Color &&
			game->PieceC.PlaceMap[Pos.y][Pos.x - 1].Type == Type_Pawn)
		{
			Pawn* TestPawn = (Pawn*)game->PieceC.Pieces[game->PieceC.PlaceMap[Pos.y][Pos.x - 1].PieceID];
			if (TestPawn->CanCapturedWhilePassing)
			{

				MoveMap[Pos.y - 1][Pos.x - 1] = true;
				CWP[1].IsValid = true;
				CWP[1].PawnIndex = TestPawn->PieceIndex;
				CWP[1].Pos = TilePos(Pos.x - 1, Pos.y - 1);;

			}

		}
#pragma endregion


		//eger ustunde tas yoksa true yap varsa return. 1 ilerinde tas varsa 2 ilerisine gidemez.
		CheckSetReturn(MoveMap, game->PieceC.PlaceMap, Pos.x, Pos.y - 1);
		if (Bonus)
		{
			CheckSetReturn(MoveMap, game->PieceC.PlaceMap, Pos.x, Pos.y - 2);//bonusun var ise ve 2 ilerinde tas yoksa true yap.
		}

	}
	else
	{
		//Sona geldiysen rutbe atla.
		if (Pos.y == 7)
		{
			Promotion(game);
		}

		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x + 1, Pos.y + 1);// Sag altta tas varsa true yap
		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x - 1, Pos.y + 1);// Sol altta tas varsa true yap
		//Gecerken Alma


#pragma region CaptureWhilePassing
		if (Pos.x + 1 < 8 &&
			game->PieceC.PlaceMap[Pos.y][Pos.x + 1].Color != Side_Empty &&
			game->PieceC.PlaceMap[Pos.y][Pos.x + 1].Color != Color &&
			game->PieceC.PlaceMap[Pos.y][Pos.x + 1].Type == Type_Pawn)
		{
			Pawn* TestPawn = (Pawn*)game->PieceC.Pieces[game->PieceC.PlaceMap[Pos.y][Pos.x + 1].PieceID];
			if (TestPawn->CanCapturedWhilePassing)
			{

				MoveMap[Pos.y + 1][Pos.x + 1] = true;
				CWP[0].IsValid = true;
				CWP[0].PawnIndex = TestPawn->PieceIndex;
				CWP[0].Pos = TilePos(Pos.x + 1, Pos.y + 1);

			}

		}
		if (Pos.x - 1 >= 0 &&
			game->PieceC.PlaceMap[Pos.y][Pos.x - 1].Color != Side_Empty &&
			game->PieceC.PlaceMap[Pos.y][Pos.x - 1].Color != Color &&
			game->PieceC.PlaceMap[Pos.y][Pos.x - 1].Type == Type_Pawn)
		{
			Pawn* TestPawn = (Pawn*)game->PieceC.Pieces[game->PieceC.PlaceMap[Pos.y][Pos.x - 1].PieceID];
			if (TestPawn->CanCapturedWhilePassing)
			{

				MoveMap[Pos.y + 1][Pos.x - 1] = true;
				CWP[1].IsValid = true;
				CWP[1].PawnIndex = TestPawn->PieceIndex;
				CWP[1].Pos = TilePos(Pos.x - 1, Pos.y + 1);;
			}


		}
#pragma endregion




		CheckSetReturn(MoveMap, game->PieceC.PlaceMap, Pos.x, Pos.y + 1);
		if (Bonus)
		{
			CheckSetReturn(MoveMap, game->PieceC.PlaceMap, Pos.x, Pos.y + 2);
		}
	}

}
void Pawn::Promotion(Game* game)
{
	TilePos QueenPos = { 10,3 };
	TilePos RookPos = { 10,4 };
	TilePos BishopPos = { 10,5 };
	TilePos KnightPos = { 10,6 };

	//Kullaniciya sag tarafta menu diziyoruz. Ekstra bir texture yuklemek istemedigim icin oyundaki olan textureleri yansittim.
	for (int i = 0; i < game->PieceCount; i++)
	{
		if (game->PieceC.Pieces[i]->Type == Type_Queen && game->PieceC.Pieces[i]->Color == Color)
		{
			SDL_Rect Rect = { QueenPos.x * RectSize, QueenPos.y * RectSize, RectSize, RectSize };

			SDL_RenderCopy(game->Renderer, game->PieceC.Pieces[i]->Texture, 0, &Rect);

		}
		if (game->PieceC.Pieces[i]->Type == Type_Rook && game->PieceC.Pieces[i]->Color == Color)
		{
			SDL_Rect Rect = { RookPos.x * RectSize, RookPos.y * RectSize, RectSize, RectSize };

			SDL_RenderCopy(game->Renderer, game->PieceC.Pieces[i]->Texture, 0, &Rect);

		}
		if (game->PieceC.Pieces[i]->Type == Type_Bishop && game->PieceC.Pieces[i]->Color == Color)
		{
			SDL_Rect Rect = { BishopPos.x * RectSize, BishopPos.y * RectSize, RectSize, RectSize };

			SDL_RenderCopy(game->Renderer, game->PieceC.Pieces[i]->Texture, 0, &Rect);

		}
		if (game->PieceC.Pieces[i]->Type == Type_Knight && game->PieceC.Pieces[i]->Color == Color)
		{
			SDL_Rect Rect = { KnightPos.x * RectSize, KnightPos.y * RectSize, RectSize, RectSize };

			SDL_RenderCopy(game->Renderer, game->PieceC.Pieces[i]->Texture, 0, &Rect);

		}
	}
	SDL_RenderPresent(game->Renderer);//Menuyu renderliyoruz.

	bool PickedPiece = false;
	TilePos Mouse;
	SDL_Event TempEvent;
	while (!PickedPiece)//secmedigi surece devam eden loop.
	{
		//Menuden bir sey sectimi diye bakiyoruz.
		//eger sectiyse bu tasin oldugu indexi sectigi turden olusturdugumuz bir tas ile degistiriyoruz.
		//PieceCount costructorda otomatik arttigi icin bizde 1 tane dusuruyoruz.
		if (SDL_PollEvent(&TempEvent) && TempEvent.type == SDL_MOUSEBUTTONDOWN)
		{
			SDL_GetMouseState(&Mouse.x, &Mouse.y);
			Mouse.x = Mouse.x / RectSize;
			Mouse.y = Mouse.y / RectSize;
			if (Mouse == QueenPos)
			{
				game->PieceC.Pieces[PieceIndex] = new Queen(game, Pos, Color, Type_Queen);
				game->PieceC.Pieces[PieceIndex]->PieceIndex = PieceIndex;
				game->PieceCount--;
				IsDestroyed = true;
				PickedPiece = true;
			}
			if (Mouse == RookPos)
			{
				game->PieceC.Pieces[PieceIndex] = new Rook(game, Pos, Color, Type_Queen);
				game->PieceC.Pieces[PieceIndex]->PieceIndex = PieceIndex;
				game->PieceCount--;
				IsDestroyed = true;
				PickedPiece = true;
			}
			if (Mouse == BishopPos)
			{
				game->PieceC.Pieces[PieceIndex] = new Bishop(game, Pos, Color, Type_Queen);
				game->PieceC.Pieces[PieceIndex]->PieceIndex = PieceIndex;
				game->PieceCount--;
				IsDestroyed = true;
				PickedPiece = true;
			}
			if (Mouse == KnightPos)
			{
				game->PieceC.Pieces[PieceIndex] = new Knight(game, Pos, Color, Type_Queen);
				game->PieceC.Pieces[PieceIndex]->PieceIndex = PieceIndex;
				game->PieceCount--;
				IsDestroyed = true;
				PickedPiece = true;
			}
			SDL_Delay(10);//Menuyu beklerken bilgisyara cok yuklenmesin diye loopu kisitliyoruz.
		}
	}


}
//Kale icin
void Rook::CalcMove(Game* game)
{
	Piece::CalcMove(game);

	//kalenin sag, sol, yukari ve asagisini kontrol ediyoruz. 
	//Eger PlaceMapta orasi bos ise true yapiyoruz. eger dolu ve bizim takimdan ise direk isaretlemeden cikiyoruz.
	// eger dolu ve karsi takimdansa isaretleyip cikiyoruz. boylelikle karsi takimin taslarini yiyebiliyoruz.
	int i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x, Pos.y - i);//up // Yukari
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x, Pos.y + i);//down // Asagi
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x - i, Pos.y);//left // Sol
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x + i, Pos.y);//right // Sag
		i++;
	}

}
//At icin
void Knight::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	// Eger gidebilecegi yer tahta icindeyse ve kendi takimindan tas yok ise isaretliyoruz.
	Set(MoveMap, Pos.x + 1, Pos.y - 2)
		Set(MoveMap, Pos.x - 1, Pos.y - 2)
		Set(MoveMap, Pos.x + 1, Pos.y + 2)
		Set(MoveMap, Pos.x - 1, Pos.y + 2)
		Set(MoveMap, Pos.x - 2, Pos.y + 1)
		Set(MoveMap, Pos.x - 2, Pos.y - 1)
		Set(MoveMap, Pos.x + 2, Pos.y + 1)
		Set(MoveMap, Pos.x + 2, Pos.y - 1)

}
//fil icin
void Bishop::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	//Kale ile ayni islem sadece yon degisiyor.
	int i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x - i, Pos.y - i)//up left
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x - i, Pos.y + i)//down left
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x + i, Pos.y - i)//up rignht
			i++;
	}
	i = 1;

	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x + i, Pos.y + i)//down right
			i++;
	}

}
//Vezir icin
void Queen::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	//Kale ve fildeki kodlarin birlesimi
	int i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x, Pos.y - i)//up 
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x, Pos.y + i)//down
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x - i, Pos.y)//left
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x + i, Pos.y)//right
			i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x - i, Pos.y - i);//up left
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x - i, Pos.y + i);//down left
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x + i, Pos.y - i);//up rignht
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x + i, Pos.y + i);//down right
		i++;
	}



}
//Sah icin
void King::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	//ilk olarak gidebilecegi yerleri eger kendi takimindan tas yok ve harita disinda degilse isaretliyoruz.
	Set(MoveMap, Pos.x - 1, Pos.y + 1);
	Set(MoveMap, Pos.x, Pos.y + 1);
	Set(MoveMap, Pos.x + 1, Pos.y + 1);

	Set(MoveMap, Pos.x - 1, Pos.y);
	Set(MoveMap, Pos.x + 1, Pos.y);


	Set(MoveMap, Pos.x - 1, Pos.y - 1);
	Set(MoveMap, Pos.x, Pos.y - 1);
	Set(MoveMap, Pos.x + 1, Pos.y - 1);

	//Daha sonra rengine gore eger karsi takimin danger mapinda gidebilicegi yer dogru ise movemapda false yapiyoruz.
	if (Color == Side_White)
	{

		RemoveSet(game->PieceC.WhiteDangerMap, Pos.y - 1, Pos.x + 1)
			RemoveSet(game->PieceC.WhiteDangerMap, Pos.y, Pos.x + 1)
			RemoveSet(game->PieceC.WhiteDangerMap, Pos.y + 1, Pos.x + 1)

			RemoveSet(game->PieceC.WhiteDangerMap, Pos.y - 1, Pos.x)
			RemoveSet(game->PieceC.WhiteDangerMap, Pos.y + 1, Pos.x)

			RemoveSet(game->PieceC.WhiteDangerMap, Pos.y - 1, Pos.x - 1)
			RemoveSet(game->PieceC.WhiteDangerMap, Pos.y, Pos.x - 1)
			RemoveSet(game->PieceC.WhiteDangerMap, Pos.y + 1, Pos.x - 1)

	}
	else
	{
		RemoveSet(game->PieceC.BlackDangerMap, Pos.y - 1, Pos.x + 1)
			RemoveSet(game->PieceC.BlackDangerMap, Pos.y, Pos.x + 1)
			RemoveSet(game->PieceC.BlackDangerMap, Pos.y + 1, Pos.x + 1)

			RemoveSet(game->PieceC.BlackDangerMap, Pos.y - 1, Pos.x)
			RemoveSet(game->PieceC.BlackDangerMap, Pos.y + 1, Pos.x)

			RemoveSet(game->PieceC.BlackDangerMap, Pos.y - 1, Pos.x - 1)
			RemoveSet(game->PieceC.BlackDangerMap, Pos.y, Pos.x - 1)
			RemoveSet(game->PieceC.BlackDangerMap, Pos.y + 1, Pos.x - 1)

	}
	//NOTE Checking for Castling.
	// Rok icin kontrol
	if (Bonus)//eger bonusu var yani haraket etmediyse.
	{
		if (Color == Side_White)
		{
			//Kisa rok icin
			//Burada Rok cok ozel bir durum oldugu icin makro kullanmadim.
			//Rok icin gidecegi yerler tehdit altinda degil ise, gidecegi yerlerde tas yok ve en sagdaki kaleninde bonusu var ise.
			if (!game->PieceC.WhiteDangerMap[Pos.y][Pos.x] &&
				!game->PieceC.WhiteDangerMap[Pos.y][Pos.x + 1] &&
				!game->PieceC.WhiteDangerMap[Pos.y][Pos.x + 2] &&
				game->PieceC.PlaceMap[Pos.y][Pos.x + 1].Color == Side_Empty &&
				game->PieceC.PlaceMap[Pos.y][Pos.x + 2].Color == Side_Empty &&
				game->PieceC.PlaceMap[Pos.y][Pos.x + 3].Color == Color &&
				game->PieceC.PlaceMap[Pos.y][Pos.x + 3].Type == Type_Rook &&
				game->PieceC.Pieces[game->PieceC.PlaceMap[Pos.y][Pos.x + 3].PieceID]->Bonus)

			{
				//Cast structini dolduruyoruz.
				MoveMap[Pos.y][Pos.x + 2] = true;
				Cast[0].IsValid = true;
				Cast[0].Pos.x = Pos.x + 2;
				Cast[0].Pos.y = Pos.y;
				Cast[0].RookIndex = game->PieceC.PlaceMap[Pos.y][Pos.x + 3].PieceID;


			}
			//Bu sefer sol icin bakiyoruz. Yani Uzun rok.
			if (!game->PieceC.WhiteDangerMap[Pos.y][Pos.x] &&
				!game->PieceC.WhiteDangerMap[Pos.y][Pos.x - 1] &&
				!game->PieceC.WhiteDangerMap[Pos.y][Pos.x - 2] &&
				!game->PieceC.WhiteDangerMap[Pos.y][Pos.x - 3] &&
				game->PieceC.PlaceMap[Pos.y][Pos.x - 1].Color == Side_Empty &&
				game->PieceC.PlaceMap[Pos.y][Pos.x - 2].Color == Side_Empty &&
				game->PieceC.PlaceMap[Pos.y][Pos.x - 3].Color == Side_Empty &&
				game->PieceC.PlaceMap[Pos.y][Pos.x - 4].Color == Color &&
				game->PieceC.PlaceMap[Pos.y][Pos.x - 4].Type == Type_Rook &&
				game->PieceC.Pieces[game->PieceC.PlaceMap[Pos.y][Pos.x - 4].PieceID]->Bonus)
			{
				MoveMap[Pos.y][Pos.x - 2] = true;
				Cast[1].IsValid = true;
				Cast[1].Pos.x = Pos.x - 2;
				Cast[1].Pos.y = Pos.y;
				Cast[1].RookIndex = game->PieceC.PlaceMap[Pos.y][Pos.x - 4].PieceID;

			}

		}
		if (Color == Side_Black)
		{
			if (!game->PieceC.BlackDangerMap[Pos.y][Pos.x] &&
				!game->PieceC.BlackDangerMap[Pos.y][Pos.x + 1] &&
				!game->PieceC.BlackDangerMap[Pos.y][Pos.x + 2] &&
				game->PieceC.PlaceMap[Pos.y][Pos.x + 1].Color == Side_Empty &&
				game->PieceC.PlaceMap[Pos.y][Pos.x + 2].Color == Side_Empty &&
				game->PieceC.PlaceMap[Pos.y][Pos.x + 3].Color == Color &&
				game->PieceC.PlaceMap[Pos.y][Pos.x + 3].Type == Type_Rook &&
				game->PieceC.Pieces[game->PieceC.PlaceMap[Pos.y][Pos.x + 3].PieceID]->Bonus)

			{
				MoveMap[Pos.y][Pos.x + 2] = true;
				Cast[0].IsValid = true;
				Cast[0].Pos.x = Pos.x + 2;
				Cast[0].Pos.y = Pos.y;
				Cast[0].RookIndex = game->PieceC.PlaceMap[Pos.y][Pos.x + 3].PieceID;


			}

			if (!game->PieceC.BlackDangerMap[Pos.y][Pos.x] &&
				!game->PieceC.BlackDangerMap[Pos.y][Pos.x - 1] &&
				!game->PieceC.BlackDangerMap[Pos.y][Pos.x - 2] &&
				!game->PieceC.BlackDangerMap[Pos.y][Pos.x - 3] &&
				game->PieceC.PlaceMap[Pos.y][Pos.x - 1].Color == Side_Empty &&
				game->PieceC.PlaceMap[Pos.y][Pos.x - 2].Color == Side_Empty &&
				game->PieceC.PlaceMap[Pos.y][Pos.x - 3].Color == Side_Empty &&
				game->PieceC.PlaceMap[Pos.y][Pos.x - 4].Color == Color &&
				game->PieceC.PlaceMap[Pos.y][Pos.x - 4].Type == Type_Rook &&
				game->PieceC.Pieces[game->PieceC.PlaceMap[Pos.y][Pos.x - 4].PieceID]->Bonus)
			{
				MoveMap[Pos.y][Pos.x - 2] = true;
				Cast[1].IsValid = true;
				Cast[1].Pos.x = Pos.x - 2;
				Cast[1].Pos.y = Pos.y;
				Cast[1].RookIndex = game->PieceC.PlaceMap[Pos.y][Pos.x - 4].PieceID;

			}


		}
	}


}

#pragma endregion

#pragma region CalcDanger

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
		DangerSet(DangerMap, Pos.x + 1, Pos.y - 1);//Front Right
		DangerSet(DangerMap, Pos.x - 1, Pos.y - 1);//Front Left


	}
	else
	{
		DangerSet(DangerMap, Pos.x + 1, Pos.y + 1);//Front Right
		DangerSet(DangerMap, Pos.x - 1, Pos.y + 1);//Front Left

	}

}
void Rook::CalcDanger(Game* game)
{
	Piece::CalcDanger(game);

	int i = 1;
	while (1)
	{

		DangerCheckSetBreak(DangerMap, Pos.x, Pos.y - i);//up // noktali virgullerin hic bir anlami yok bos satir yani empty instruduction olarak geciyor gozume guzel gozuksun diye koydum zaten release modda compiler onu optimize eder.
		i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x, Pos.y + i);//down
		i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x - i, Pos.y);//left
		i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x + i, Pos.y);//right
		i++;
	}

}
void Knight::CalcDanger(Game* game)
{
	Piece::CalcDanger(game);
	DangerSet(DangerMap, Pos.x + 1, Pos.y - 2)
		DangerSet(DangerMap, Pos.x - 1, Pos.y - 2)
		DangerSet(DangerMap, Pos.x + 1, Pos.y + 2)
		DangerSet(DangerMap, Pos.x - 1, Pos.y + 2)
		DangerSet(DangerMap, Pos.x - 2, Pos.y + 1)
		DangerSet(DangerMap, Pos.x - 2, Pos.y - 1)
		DangerSet(DangerMap, Pos.x + 2, Pos.y + 1)
		DangerSet(DangerMap, Pos.x + 2, Pos.y - 1)

}
void Bishop::CalcDanger(Game* game)
{
	Piece::CalcDanger(game);
	int i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x - i, Pos.y - i)//up left
			i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x - i, Pos.y + i)//down left
			i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x + i, Pos.y - i)//up rignht
			i++;
	}
	i = 1;

	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x + i, Pos.y + i)//down right
			i++;
	}

}
void Queen::CalcDanger(Game* game)
{
	Piece::CalcDanger(game);

	int i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x, Pos.y - i)//up 
			i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x, Pos.y + i)//down
			i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x - i, Pos.y)//left
			i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x + i, Pos.y)//right
			i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x - i, Pos.y - i);//up left
		i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x - i, Pos.y + i);//down left
		i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x + i, Pos.y - i);//up rignht
		i++;
	}
	i = 1;
	while (1)
	{
		DangerCheckSetBreak(DangerMap, Pos.x + i, Pos.y + i);//down right
		i++;
	}



}
void King::CalcDanger(Game* game)
{
	Piece::CalcDanger(game);
	DangerSet(DangerMap, Pos.x - 1, Pos.y + 1);
	DangerSet(DangerMap, Pos.x, Pos.y + 1);
	DangerSet(DangerMap, Pos.x + 1, Pos.y + 1);

	DangerSet(DangerMap, Pos.x - 1, Pos.y);
	DangerSet(DangerMap, Pos.x + 1, Pos.y);

	DangerSet(DangerMap, Pos.x - 1, Pos.y - 1);
	DangerSet(DangerMap, Pos.x, Pos.y - 1);
	DangerSet(DangerMap, Pos.x + 1, Pos.y - 1);
}

#pragma endregion


int MateDummy::Test(Game* game, int x, int y)
{
	//sah mati test etmek icin.

	if (game->PieceC.PlaceMap[y][x].Color == Side_Empty)//Eger test edilecek karede tas yoksa direk oraya tasi koyuyoruz.
	{
		Pos.y = y;
		Pos.x = x;
		return -1;
	}
	else // varsa tasi oraya koyuyoruz daha sonra orda olan tasin yok olma flagini true yapiyoruz. ve tasin indexini donuyoruz.
	{
		TilePos TestPos = { x,y };
		for (int i = 0; i < game->PieceCount; i++)
		{
			if (game->PieceC.Pieces[i]->IsDestroyed == true) continue;
			if (game->PieceC.Pieces[i]->Pos == TestPos)
			{
				game->PieceC.Pieces[i]->IsDestroyed = true;
				return i;
			}
		}
	}





}

void King::Check(Game* game)
{
	IsCheck = true; // sah oldugunu dogru yapiroruz.
	IsTesting = true;
	Mate(game);// mat var mi diye kotnrol ediyoruz.
	IsTesting = false;
}
//Sah Mat
void King::Mate(Game* game)
{
	bool CanMove = false;
	game->PieceC.CalcMoveMap(game);
	bool IsMate = true;
	//sahin gidebilecegi yer var mi diye bakiyoruz.
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{

			if (MoveMap[j][i] == true)
			{
				CanMove = true;
				IsMate = false;
			}



		}
	}

	//Sahin gidebilecegi yer yok ise bidahaki tur icin sah olmayan bir varyasyon ariyoruz.
	if (CanMove == false)
	{
		for (int a = 0; a < game->PieceCount; a++)
		{
			if (game->PieceC.Pieces[a]->Type == Type_Dummy) // Dummyi buluyoruz.
			{
				//dummyi uyandiriyoruz.
				game->PieceC.Pieces[a]->IsDestroyed = false;
				game->PieceC.Pieces[a]->Color = Color;
				MateDummy* dummy = (MateDummy*)game->PieceC.Pieces[a];

				if (Color == Side_White)
				{

					//MoveMaptaki tum yerleri test ediyoruz. Yan movemaptaki tum yerlere tek tek dummy koyuyoruz ve gene sah mi diye kontrol ediyoruz.
					//Eger sah olmayan bir varyasyon bulursak mat olmadigini anliyoruz.
					for (int j = 0; j < 8; j++)
					{
						for (int i = 0; i < 8; i++)
						{
							if (game->PieceC.WhiteMoveMap[j][i] == true)
							{
								int TestItem = dummy->Test(game, i, j);
								game->PieceC.CalcDangerMap(game, true);
								if (game->PieceC.WhiteDangerMap[Pos.y][Pos.x] == false)
								{
									IsMate = false;
								}
								if (TestItem != -1) // Eger movemaptaki koydugumuz yerde tas var ise yok olma flagini true yapmistik simdi false yapiyoruz.
								{
									game->PieceC.Pieces[TestItem]->IsDestroyed = false;
								}

							}

						}
					}
				}
				if (Color == Side_Black)
				{


					for (int j = 0; j < 8; j++)
					{
						for (int i = 0; i < 8; i++)
						{
							if (game->PieceC.BlackMoveMap[j][i] == true)
							{
								int TestItem = dummy->Test(game, i, j);
								game->PieceC.CalcDangerMap(game, true);
								if (game->PieceC.BlackDangerMap[Pos.y][Pos.x] == false)
								{
									IsMate = false;
								}
								if (TestItem != -1)
								{
									game->PieceC.Pieces[TestItem]->IsDestroyed = false;
								}

							}

						}
					}
				}
				//Dummyi gene uyutuyoruz.
				game->PieceC.Pieces[a]->IsDestroyed = true;
				dummy->Color = Side_Empty;
				break;
			}
		}
	}
	//Daha sonra bozdugumuz bir sey olabilir diye tum maplari yeniliyoruz.
	game->PieceC.CalcAll(game);

	if (IsMate)// Daha sonra cikan sonucu oyun durumuna esitliyoruz.
	{
		if (Color == Side_White)
		{
			game->Cond = Condution_WhiteMate;
			game->EndGame();

		}
		else
		{
			game->Cond = Condution_BlackMate;
			game->EndGame();
		}
		std::cout << "CheckMate" << std::endl;

	}
	else
	{
		if (Color == Side_White)
		{
			game->Cond = Condution_WhiteCheck;
		}
		else
		{
			game->Cond = Condution_BlackCheck;
		}

		std::cout << "Check" << std::endl;
	}


}

void PieceController::CalcAll(Game* game) // Her seyi tek bir fonksiyonda tutmak icin
{
	CalcPiecePositions(game);
	CalcMoveMap(game);
	CalcDangerMap(game, true);
}





Move Piece::Move(TilePos Pos2, Game* game)
{
	//	game->PieceC.CalcMoveMap(game);
	if (MoveMap[Pos2.y][Pos2.x])//Eger gidecegi yer tasin MoveMapinda true ise
	{
		if (game->PieceC.PlaceMap[Pos2.y][Pos2.x].Color != Side_Empty)//Eger gidecegi yer bos degil ise
		{
			for (int i = 0; i < game->PieceCount; i++)
			{
				if (game->PieceC.Pieces[i]->IsDestroyed == true)
					continue;

				if (i != PieceIndex && game->PieceC.Pieces[i]->Pos == Pos2)// gidecegi yerdeki tasi bul.
				{
					if (game->PieceC.Pieces[i]->Color != Color)//Eger senin takimindan degil ise yok et.
					{
						game->PieceC.Pieces[i]->Destroy();

					}
					else //senin takimindansa MoveMap hesaplamada bir sikinti vardir 
					{
						std::cout << "Move Map Error!!!" << std::endl;
						return Move_SameColor;
					}

				}

			}

		}



		TilePos Test = Pos;
		Pos = Pos2;
		Condution cond = game->Cond;
		//NOTE Imposible move stuff
		if (game->PieceC.CalcDangerMap(game, false) == Color)
		{
			Pos = Test;
			game->PieceC.CalcDangerMap(game, false);
			game->Cond = cond;
			return Move_Imposible;


		}

		//NOTE Castling Stuff
		//Rok icin
		if (Type == Type_King) // eger haraket eden sah ise
		{
			//Rok structlarini kontrol ediyoruz. Eger rok var ve gittigi yerde rokun belirttigi yer ise kalenin yerinide degistiriyoruz.
			King* king = (King*)this;
			if (king->Cast[0].IsValid &&
				king->Cast[0].Pos == Pos2)
			{
				game->PieceC.Pieces[king->Cast[0].RookIndex]->Pos.x = Pos2.x - 1;
				game->PieceC.Pieces[king->Cast[0].RookIndex]->Pos.y = Pos2.y;
			}
			if (king->Cast[1].IsValid &&
				king->Cast[1].Pos == Pos2)
			{
				game->PieceC.Pieces[king->Cast[1].RookIndex]->Pos.x = Pos2.x + 1;
				game->PieceC.Pieces[king->Cast[1].RookIndex]->Pos.y = Pos2.y;
			}
		}

		//NOTE capture while passing stuff.
		//Gecerken alma icin
		if (Type == Type_Pawn)//Eger haraket eden piyon ise
		{
			// gecerken alma strcutlarini kontrol ediyoruz eger biri dogru ve gittigimiz yerde onun istedigi yer ise arkasina gectigimiz tasi yok ediyoruz.
			Pawn* pawn = (Pawn*)this;
			if (pawn->CWP[0].IsValid == true && pawn->CWP[0].Pos == Pos2)
			{
				game->PieceC.Pieces[pawn->CWP[0].PawnIndex]->Destroy();
			}
			if (pawn->CWP[1].IsValid == true && pawn->CWP[1].Pos == Pos2)
			{
				game->PieceC.Pieces[pawn->CWP[1].PawnIndex]->Destroy();
			}

			if ((Pos2.y - Test.y == -2 || Pos2.y - Test.y == 2) && Bonus == true)//2 kare acildi ise 1 turluguna
			{
				pawn->CanCapturedWhilePassing = true;//gecerken almaya uygundur diye isaretliyoruz.
			}





		}
		Bonus = false;//Haraket eder ise bonusu kaldiriyoruz.
		game->Cond = Condution_Empty;
		game->PieceC.CalcAll(game);//Maplari hesapliyoruz.




	}
	else return Move_MoveMapIsNotTrue;

	return Move_Sucsess;




}

void Piece::Destroy() //bunu ilerde yok ederke farkli bir seyler yapmak istersem diye koydum.
{
	IsDestroyed = true;
}




//Takimlarin MoveMaplarini Hesaplama
void PieceController::CalcMoveMap(Game* game)
{
	//Ilk olarak tum haritalari temizliyoruz.
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
		{
			WhiteMoveMap[j][i] = false;
			BlackMoveMap[j][i] = false;
		}

	for (int a = 0; a < game->PieceCount; a++)
	{
		if (game->PieceC.Pieces[a]->IsDestroyed == true) //Eger yok olmadi ise
			continue;
		Pieces[a]->CalcMove(game); // Hareketini hesaplatiyoruz.

		if (Pieces[a]->Color == Side_White) // Daha sonra takimina gore Movemap ile or yapiyoruz boylelikle tum maplari birlestirmis oluyoruz.
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
	//Pat icin yani berabere
	bool IsDraw = true;

	if (game->Turn == Side_White) // Eger tur beyazda ise bidahki tur siyahtatir. Siyahin MoveMapindaki tum yerler false ise
	{
		for (int j = 0; j < 8; j++)
			for (int i = 0; i < 8; i++)
				if (BlackMoveMap[j][i] == true)
				{
					IsDraw = false;
				}

	}
	else
	{
		for (int j = 0; j < 8; j++)
			for (int i = 0; i < 8; i++)
				if (WhiteMoveMap[j][i] == true) // Siyah icinde ayni sey.
				{
					IsDraw = false;
				}
	}


	if (IsDraw) //Pattir
	{
		game->Cond = Condution_Draw;
		game->EndGame();
	}



}
Side PieceController::CalcDangerMap(Game* game, bool IsLogging)
{
	//MoveMap ile ayni matikta calisiyor ilk kismi.
	Side Result = Side_Empty;
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
		{
			WhiteDangerMap[j][i] = false;
			BlackDangerMap[j][i] = false;
		}

	for (int a = 0; a < game->PieceCount; a++)
	{
		if (game->PieceC.Pieces[a]->IsDestroyed == true)
			continue;
		Pieces[a]->CalcDanger(game);

		if (Pieces[a]->Color == Side_White)
		{
			for (int j = 0; j < 8; j++)
			{
				for (int i = 0; i < 8; i++)
				{
					BlackDangerMap[j][i] = BlackDangerMap[j][i] || Pieces[a]->DangerMap[j][i];
				}
			}
		}
		else
		{
			for (int j = 0; j < 8; j++)
			{
				for (int i = 0; i < 8; i++)
				{
					WhiteDangerMap[j][i] = WhiteDangerMap[j][i] || Pieces[a]->DangerMap[j][i];
				}
			}
		}



	}

	//Haritayi hesapladiktan sonra sah var mi diye konrol ediyoruz.
	//Eger sah var ise Sah yiyen tasin Check fonksiyonunu cagiriyoruz.
	//NOTE Cheking for is king Checked

	for (int a = 0; a < game->PieceCount; a++)
	{
		if (game->PieceC.Pieces[a]->IsDestroyed == true)
			continue;
		if (Pieces[a]->Type == Type_King)
		{
			King* test = (King*)Pieces[a];

			if (Pieces[a]->Color == Side_White)
			{
				if (WhiteDangerMap[Pieces[a]->Pos.y][Pieces[a]->Pos.x]) // Sah icin kontrol
				{

					Result = Side_White;
					game->Cond = Condution_WhiteCheck;
					if (test->IsTesting == false && IsLogging) // Burasi stack overflowu engellemek icin
					{
						test->Check(game);
					}


				}
				else if (test->IsTesting == false)
				{
					test->IsCheck = false;
				}

			}
			else if (Pieces[a]->Color == Side_Black)
			{
				if (BlackDangerMap[Pieces[a]->Pos.y][Pieces[a]->Pos.x])
				{
					Result = Side_Black;
					game->Cond = Condution_BlackCheck;

					if (test->IsTesting == false && IsLogging)
					{
						test->Check(game);

					}

				}
				else if (test->IsTesting == false)
				{
					test->IsCheck = false;
				}
			}

		}
	}

	return Result;
}
//Taslarin yerini hesaplamak icin.
void PieceController::CalcPiecePositions(Game* game)
{
	//ilk olarak temizliyoruz.
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
		{
			PlaceMap[j][i].Color = Side_Empty;
			PlaceMap[j][i].Type = Type_Empty;
			PlaceMap[j][i].PieceID = -1;


		}
	//Daha sonra tum taslar icin
	for (int i = 0; i < game->PieceCount; i++)
	{
		if (game->PieceC.Pieces[i]->IsDestroyed == true)//Eger yok olmadi ise
			continue;
		//Placemapi dolduruyoruz.
		TilePos Pos = Pieces[i]->Pos;

		PlaceMap[Pos.y][Pos.x].Color = Pieces[i]->Color;
		PlaceMap[Pos.y][Pos.x].Type = Pieces[i]->Type;
		PlaceMap[Pos.y][Pos.x].PieceID = Pieces[i]->PieceIndex;


	}
}

inline bool PieceController::CheckSetFonc(bool Map[8][8], int x, int y, Side color, bool IsMoveMap) // MoveMap ve DangerMap icin sorgular.
{
	if (IsMoveMap)//MoveMap icin
	{

		if (PlaceMap[y][x].Color == Side_Empty  //Bos ve sinirlar icerisinde ise direk isaretliyoruz.
			&& x >= 0 && x < 8 && y >= 0 && y < 8)
		{

			Map[y][x] = true;
			return true;
		}
		else if (x < 0 || x > 7 || y < 0 || y > 7) // Sinirlar icinde degil ise cikiyoruz.
			return false;
		else if (PlaceMap[y][x].Color != color) // Gidecegi yerdeki tas karsi takimda ise true yapip cikiyouruz
		{
			Map[y][x] = true;
			return false;
		}
		else // Degil ise direk cikiyoruz.
		{
			return false;
		}
	}
	else // DangerMap icin
	{
		//MoveMap ile yaklasik olarak ayni fakat bunda eger tehtit ettigimiz tas sah ise onu yok sayiyoruz cunku sahlarin arkasinida tehdit ederiz.
		if ((PlaceMap[y][x].Color == Side_Empty ||
			(PlaceMap[y][x].Type == Type_King && PlaceMap[y][x].Color != Side_Empty && PlaceMap[y][x].Color != color))
			&& x >= 0 && x < 8 && y >= 0 && y < 8)
		{

			Map[y][x] = true;
			return true;
		}
		else if (x < 0 || x > 7 || y < 0 || y > 7)
			return false;
		else
		{
			Map[y][x] = true;
			return false;
		}

	}
}





namespace Tools
{
	//Diskten texture yuklemek icin.
	//Bunu bir onceki oyunumdan aldim. Memory leake cok neden olmustu bu fonksiyon gecen oyunumda baya ugrasip cozmustum.
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
		// Oyundaki taslari olusturup yerlerine diziyoruz.
		// Hangi rengin asagida olcagini Game.h dosyasindaki ColorAtBottom ile secebilirsiniz.
		//Piyon
		for (int i = 0; i < 8; i++)
		{
			if (ColorAtBottom == Side_White)
			{
				Pawn* PieceW = new Pawn(game, TilePos(i, 1), Side_Black, Type_Pawn);
				Pawn* PieceB = new Pawn(game, TilePos(i, 6), Side_White, Type_Pawn);
			}
			else
			{
				Pawn* PieceW = new Pawn(game, TilePos(i, 6), Side_Black, Type_Pawn);
				Pawn* PieceB = new Pawn(game, TilePos(i, 1), Side_White, Type_Pawn);
			}

		}
		//Kale
		for (int i = 0; i < 2; i++)
		{
			if (ColorAtBottom == Side_White)
			{
				Rook* PieceW = new Rook(game, TilePos(i * 7, 0), Side_Black, Type_Rook);
				Rook* PieceB = new Rook(game, TilePos(i * 7, 7), Side_White, Type_Rook);
			}
			else
			{
				Rook* PieceW = new Rook(game, TilePos(i * 7, 7), Side_Black, Type_Rook);
				Rook* PieceB = new Rook(game, TilePos(i * 7, 0), Side_White, Type_Rook);
			}

		}
		//At
		for (int i = 0; i < 2; i++)
		{
			if (ColorAtBottom == Side_White)
			{
				Knight* PieceW = new Knight(game, TilePos(1 + i * 5, 0), Side_Black, Type_Knight);
				Knight* PieceB = new Knight(game, TilePos(1 + i * 5, 7), Side_White, Type_Knight);
			}
			else
			{
				Knight* PieceW = new Knight(game, TilePos(1 + i * 5, 7), Side_Black, Type_Knight);
				Knight* PieceB = new Knight(game, TilePos(1 + i * 5, 0), Side_White, Type_Knight);

			}

		}
		//Fil
		for (int i = 0; i < 2; i++)
		{
			if (ColorAtBottom == Side_White)
			{
				Bishop* PieceW = new Bishop(game, TilePos(2 + i * 3, 0), Side_Black, Type_Bishop);
				Bishop* PieceB = new Bishop(game, TilePos(2 + i * 3, 7), Side_White, Type_Bishop);
			}
			else
			{
				Bishop* PieceW = new Bishop(game, TilePos(2 + i * 3, 7), Side_Black, Type_Bishop);
				Bishop* PieceB = new Bishop(game, TilePos(2 + i * 3, 0), Side_White, Type_Bishop);
			}

		}
		//Vezir
		if (ColorAtBottom == Side_White)
		{
			Queen* QueenW = new Queen(game, TilePos(3, 0), Side_Black, Type_Queen);
			Queen* QueenB = new Queen(game, TilePos(3, 7), Side_White, Type_Queen);
		}
		else
		{
			Queen* QueenW = new Queen(game, TilePos(3, 7), Side_Black, Type_Queen);
			Queen* QueenB = new Queen(game, TilePos(3, 0), Side_White, Type_Queen);
		}
		//Sah
		if (ColorAtBottom == Side_White)
		{
			King* KingW = new King(game, TilePos(4, 0), Side_Black, Type_King);
			King* KingB = new King(game, TilePos(4, 7), Side_White, Type_King);


		}
		else
		{
			King* KingW = new King(game, TilePos(4, 7), Side_Black, Type_King);
			King* KingB = new King(game, TilePos(4, 0), Side_White, Type_King);
		}
		//Sah mat hesaplamada kullanmak icin kukla
		MateDummy* Dummy = new MateDummy(game, TilePos(-1, -1), Side_Empty, Type_Dummy);

		//Tum oyun haritalarini hesapliyoruz.
		game->PieceC.CalcAll(game);


	}




}
