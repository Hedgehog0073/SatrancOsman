#include "Game.h"
#include <iostream>



void Game::Init()
{
	//SDL kutuphanelerini init ediyorum.
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	//Pencere ve Renderer oluşturuyorum
	Window = SDL_CreateWindow("Chess by Osman v4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, NULL);
	Renderer = SDL_CreateRenderer(Window, -1, 0);

	//Mouse için focus windowu set ediyorum.
	SDL_SetWindowInputFocus(Window);
	//Arka Plan Rengini ayarlıyorum.
	SDL_SetRenderDrawColor(Renderer, 0xEA, 0xD7, 0xC6, 255);

	//Satranc tahtasını init ediyorum. Yani tum gereken textureları diskten bellege yukluyor.
	ChessBoard.Init(Renderer);
	//Satranç tahtasını render ediyorum.
	
   
	
	ChessBoard.DrawBoard(this);

	//Tasları olusturup tahtaya diziyorum.
	Tools::AlignPieces(this);

	//Renderlediğim şeyi görmek için windowa yansıtıyorum.
	SDL_RenderPresent(Renderer);
}

void Game::HandleEvents()
{
	/*Bu fonksiyon 3 aşamadan oluşuyor.
	1-Girdi al(Input)
	2-Tepki ver Hesapla(Calculate)
	3-Çiz(Render)
	oyun bitene kadar bu aşamaları tekrarlıyor.
	*/

	SDL_Event Event;//input için event structı

	while (Running)
	{

		Uint64 StartTime = SDL_GetPerformanceCounter(); // Oyunun FPS'ini yani saniye başına kare sayısını hesaplamak için.

		if (SDL_PollEvent(&Event))//PollEvent ile input alıyoruz.
		{
			int x, y;
			switch (Event.type)//Eventin tipine göre switch yapıyoruz.
			{
			case(SDL_QUIT)://Pencerenin Kapatma tuşuna basıldığında kapatmak için.
				Running = false;
				break;
			case(SDL_KEYDOWN)://Klavyede eğer bir tuşa basılırsa
				switch (Event.key.keysym.sym)
				{
				case(SDLK_ESCAPE):// Basılan tuş escape ise programı kapatıyoruz.
					Running = false;
					break;
				case(SDLK_m):
					PieceC.CalcMoveMap(this);
					break;
				}
				break;
			case(SDL_MOUSEBUTTONDOWN)://kullanıcı mouseye bastığında
				//Mousenin Pencerenin sol üstüne göre konumunu input ediyoruz.
				SDL_GetMouseState(&x, &y);
				// Kare boyunuta bölerek mousenin kare olarak konumunu buluyorum.
				x = x / RectSize;
				y = y / RectSize;
				//Daha sonra mousenin konumunda olan bir taş arıyorum ve bulduğumda onun indexini işaretliyorum.
				for (int i = 0; i < PieceCount; i++)
				{
					if (PieceC.Pieces[i]->IsDestroyed == true)
						continue;


					if (PieceC.Pieces[i]->Pos.x == x && PieceC.Pieces[i]->Pos.y == y && PieceC.Pieces[i]->Color == Turn)
					{
						PieceC.Pieces[i]->IsFloating = true;//Kullanıcının Mouse konumunda taşı renderlemek için
						HoldItemIndex = i;
					}
				}
				break;

			case(SDL_MOUSEBUTTONUP)://Kullanıcı Mouseyi basıp bıraktığında
				if (HoldItemIndex != -1)//Eğer bastığında tuttuğu taş var ise
				{

					PieceC.Pieces[HoldItemIndex]->IsFloating = false;// Artık mouse konumunda taşı renderlememek için.
					//Mouse konumunu alıyorum.
					SDL_GetMouseState(&x, &y);
					x = x / RectSize;
					y = y / RectSize;

					//Eğer yapılan hareket başarılı ise sırayı değiştiriyorum.
					if (PieceC.Pieces[HoldItemIndex]->Move(TilePos(x, y), this) == Move_Sucsess)
						if (Turn == Side_White)
							Turn = Side_Black;
						else
							Turn = Side_White;


					HoldItemIndex = -1;// İşaretlediğim taşın işaretini kaldırıyorum.

				}
				break;


			}

		}
		//Tüm input işlemleri tamamlandıktan sonra rendere başlıyorum
		Render();


		//Burası FPS'i sabit tutmak için.
		/*Örneğin oyunumun FPS'sini eğer 100 FPS olarak sabit tutmak istiyorsam:
		kod başladığında cycle sayısını almıştım zaten şimdi ise kod bittiği zaman cycle sayısını alıyorum.
		Daha sonra sondaki cycledan baştaki cycleyi çıkararak Delta cycleyi buluyorum.
		Daha sonra DeltaCycleyi İşlemci hızına bölerek baştan sona geçen saniyeyi buluyorum ve buda DeltaTime oluyor.
		Delta time oyunlarda oyunun işlemcinin hızına göre değilde zamana göre çalışması istendiğinde kullanılıyor.
		En sonda DeltaTimeyi 1000 ile çarparak ms birimine çeviriyorum.
		Oyunda istediğim FPS 100 olduğundan 1 saniyede 100 frame renderlemek istiyorsak ozaman 1 frame için
		1s/100frame kadar zaman harcalamalıyız saniyeyide ms'e çevirirsek 1000ms/100frame ozaman 1 frame için 10ms harcamamız
		lazım. bunun içinde oyunu 10-(geçen zaman) kadar uyutuyoruz.
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
		/*SDL_Log("Time elapsed for frame is %f", DeltaTime); // 1 Kare için geçen zaman ms olarak.
		SDL_Log("FPS: %f", 1000/DeltaTime);// 1 saniyede geçen kare
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
	//Bir önceki frameden kalan şeyleri ekrandan temizliyorum.
	SDL_RenderClear(Renderer);
	//Satranç Tahtasını çiziyorum.
	ChessBoard.DrawBoard(this);
	//Taşların hepsini konumlarında renderliyorum
	PieceC.RenderAll(this);
	//Renderde olan şeyleri ekrana yansıtıyorum.
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
	//taşı arrarye yerleştirmek için.
	PieceIndex = game->PieceCount++;
	game->PieceC.Pieces[PieceIndex] = this;


}
void PieceController::RenderAll(Game* game)
{
	int FloatingPieceIndex = -1;
	for (int i = 0; i < game->PieceCount; i++)
	{
		// Eğer taş yok olmuş ise renderlemiyorum.
		if (game->PieceC.Pieces[i]->IsDestroyed == true)
			continue;

		//Eğer taş kullanıcı tarafındantutuluyorsa burda renderlemiyorum.
		//Çünkü kullanıcının tuttuğu taşın diğer taşların üstünde gözükmesini istiyorum bunun içinde kullanıcının tuttuğu taşı
		// en son renderleyeceğim.
		if (Pieces[i]->IsFloating)
		{
			FloatingPieceIndex = i;
			continue;
		}
		//taşa Renderlenmesini söylüyorum.
		Pieces[i]->Render(game->Renderer);
	}
	//Daha öncede dediğim gibi taşın diğer taşların üstünde gözükmesi için en son renderliyorum.
	if (FloatingPieceIndex != -1)
	{
		Pieces[FloatingPieceIndex]->Render(game->Renderer);
	}


}



void Board::Init(SDL_Renderer* renderer)
{

	// Burada MoveMap ve DangerMap için kullandığım debug textureleri yüklüyorum.
	// Bunları kullanmak için Game.h dosyasındaki MOVEMAP veya DANGERMAP definelarını comment out yapın.
	Tools::CreateTexture(renderer, "Images/NewBlackRect.png", &Rects[0]);
	Tools::CreateTexture(renderer, "Images/NewWhiteRect.png", &Rects[1]);
	Tools::CreateTexture(renderer, "Images/WhiteExclamation.png", &Debugs[0]);
	Tools::CreateTexture(renderer, "Images/BlackExclamation.png", &Debugs[1]);
	Tools::CreateTexture(renderer, "Images/RedExclamation.png", &Debugs[2]);

	//Tahtanın harf ve yazıları burada SDL_TTF kullanabilirdim fakat 
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


	// Bildirimler için
	Tools::CreateTexture(renderer, "Images/LogWhite.png", &Sides[0]);
	Tools::CreateTexture(renderer, "Images/LogBlack.png", &Sides[1]);

	Tools::CreateTexture(renderer, "Images/LogCheck.png", &Condutions[0]);
	Tools::CreateTexture(renderer, "Images/LogMate.png", &Condutions[1]);

	//Kullanıcıya hareket ipucusu vermek için.

	Tools::CreateTexture(renderer, "Images/CanMove.png", &Float[0]);
	Tools::CreateTexture(renderer, "Images/MousePos.png", &Float[1]);


}

void Board::DrawBoard(Game* game)
{

	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			//Satranç tahtasını bir matris olarak düşünürsek eğer matrisnin x ve y bileşenlerinin toplamı çift ise beyaz
			//Tek ise siyah olmalı bunu tam tersinide yapabilirsiniz.
			SDL_Rect Rect{ i * RectSize,j * RectSize,RectSize,RectSize };
			SDL_RenderCopy(game->Renderer, this->Rects[(i + j) % 2], 0, &Rect);

			// MoveMap ve DangerMapi satranca yansitmak icin. Debug yaparken COOK yardimci oldu.
			// aktive etmek için Game.h dosyasındaki MOVEMAP  veya DangerMap definelerını comment out yapın.
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
	// Kullanıcıya ipucu vermek için.
	if (game->HoldItemIndex != -1)
	{
		//Kullanıcının tuttuğu taşın Movemapını oyuna yansıtıyoruz.
		for (int j = 0; j < 8; j++)
			for (int i = 0; i < 8; i++)
				if (game->PieceC.Pieces[game->HoldItemIndex]->MoveMap[j][i])
				{
					SDL_Rect Rect{ i * RectSize,j * RectSize,RectSize,RectSize };
					SDL_RenderCopy(game->Renderer, this->Float[0], 0, &Rect);
				}
		//Kullanıcının koyduğu taşın nereye gideceğini göstermek için Mouseposisyonunu tahtaya yansıtıyoruz.
		int x, y;
		SDL_GetMouseState(&x, &y);//Mouse input
		//Mousenin Piksel pozisyonundan kurtulup Kare konumunu alıyoruz.
		x = x / RectSize;
		y = y / RectSize;
		// Mousenin bulunduğu kare konumunda textureyi renderliyoruz.
		SDL_Rect Rect{ x * RectSize,y * RectSize,RectSize,RectSize };
		SDL_RenderCopy(game->Renderer, this->Float[1], 0, &Rect);
	}



	//Satranc tahtasının Harf ve Sayılarını yazdırıyoruz.
	for (int i = 0; i < 8; i++)
	{
		int j = 8;
		SDL_Rect Rect{ i * RectSize,j * RectSize,RectSize,RectSize };
		SDL_RenderCopy(game->Renderer, Letters[i], 0, &Rect);

		SDL_Rect RectNumbers{ j * RectSize,(7 - i) * RectSize,RectSize,RectSize };
		SDL_RenderCopy(game->Renderer, Numbers[i], 0, &RectNumbers);
	}


	//Eğer oyunda şah veya şahmat varsa sağ taraftaki log kısmına yazıdırıyoruz.
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
	// eğer taş şah ve tehtit altındaysa yani şah çekilmişse tehtit olarak şahın arkasındaki textureyi yüklüyorum.
	if (Type == Type_King)
	{
		King* king = (King*)this;//Şaha özel değişkenlere ulaşmak için şah olduğunu bildiğim taşı şaha cast ediyorum.
		if (king->IsCheck)//şah mı diye kontrol ediyorum.
		{
			SDL_Rect Rect{ Pos.x * RectSize,Pos.y * RectSize,RectSize,RectSize };
			SDL_RenderCopy(renderer, king->CheckTexture, 0, &Rect);

		}
	}
	//Eğer kullanıcı taşı tutuyorsa mousenin olduğu yerde render ediyorum.
	if (IsFloating)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		//burda mouse pozisyonunun eksenlerinden taş boyutunun yarısı çıkararak taşın mousenin tam taşın ortasında olmasını sağlıyorum.
		x -= RectSize / 2;
		y -= RectSize / 2;
		SDL_Rect Rect{ x,y ,RectSize,RectSize };
		SDL_RenderCopy(renderer, Texture, 0, &Rect);


	}
	else
	{
		//Eğer kullanıcı tarafından tutulmuyorsa bulunduğu yerde render ediyorum.
		SDL_Rect Rect{ Pos.x * RectSize,Pos.y * RectSize,RectSize,RectSize };
		SDL_RenderCopy(renderer, Texture, 0, &Rect);
	}


}




//consturctorlarda istenilen taş oluşturulup parametrede verilen Posisyonla kendi pozisyonu eşitleniyor.
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

//Bu bolgede her taşın kurallarına özel olarak haraket haritalarını hesaplıyoruz.
#pragma region CalcMove

void Piece::CalcMove(Game* game)
{
	//game->PieceC.CalcPiecePositions(game);

	//Her hesaplamadan önce bu fonksiyonu çağırarak haritaları temizliyoruz.
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
		{
			MoveMap[j][i] = false;
		}

}


//Piyon için
void Pawn::CalcMove(Game* game)
{
	Piece::CalcMove(game);




	//Bu taşın geçerken alma için uygun olup olmadığını ayarlıyoruz.
	if (CanCapturedWhilePassing == true && DontRepeat == true)
	{
		DontRepeat = false;
	}
	else if (CanCapturedWhilePassing == true)
	{
		CanCapturedWhilePassing = false;
	}


	if (Color == ColorAtBottom)//Alttaki taraftaki piyonlar için
	{
		if (Pos.y == 0)//eğer sona geldiyse promotion yap
		{
			Promotion(game);
		}
		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x + 1, Pos.y - 1);// Sağ üstte taş varsa true yap
		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x - 1, Pos.y - 1);// Sol üstte taş varsa true yap

		//Bu bolgede Gecerken almayi kontrol ediyoruz.
		//Eğer sağında taş varsa , karşı takımdansa ve türü piyonsa
#pragma region Capture While Passing
		//eğer sağında taş var,karşı takımdan ve piyonsa
		if (Pos.x + 1 < 8 &&
			game->PieceC.PlaceMap[Pos.y][Pos.x + 1].Color != Side_Empty &&
			game->PieceC.PlaceMap[Pos.y][Pos.x + 1].Color != Color &&
			game->PieceC.PlaceMap[Pos.y][Pos.x + 1].Type == Type_Pawn)
		{

			Pawn* TestPawn = (Pawn*)game->PieceC.Pieces[game->PieceC.PlaceMap[Pos.y][Pos.x + 1].PieceID];
			//Geçerken almaya uygun mu diye bak.
			if (TestPawn->CanCapturedWhilePassing)
			{
				//Uygunsa CWP'yi doldur.
				MoveMap[Pos.y - 1][Pos.x + 1] = true;
				CWP[0].IsValid = true;
				CWP[0].PawnIndex = TestPawn->PieceIndex;
				CWP[0].Pos = TilePos(Pos.x + 1, Pos.y - 1);
			}


		}
		//aynı islem ustte.
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


		//eğer üstünde taş yoksa true yap varsa return. 1 ilerinde taş varsa 2 ilerisine gidemez.
		CheckSetReturn(MoveMap, game->PieceC.PlaceMap, Pos.x, Pos.y - 1);
		if (Bonus)
		{
			CheckSetReturn(MoveMap, game->PieceC.PlaceMap, Pos.x, Pos.y - 2);//bonusun var ise ve 2 ilerinde taş yoksa true yap.
		}

	}
	else
	{
		//Sona geldiysen rütbe atla.
		if (Pos.y == 7)
		{
			Promotion(game);
		}

		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x + 1, Pos.y + 1);// Sağ altta taş varsa true yap
		ReverseCheckSet(MoveMap, game->PieceC.PlaceMap, Pos.x - 1, Pos.y + 1);// Sol altta taş varsa true yap
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

	//Kullanıcıya sağ tarafta menu diziyoruz. Ekstra bir texture yüklemek istemediğim için oyundaki olan textureleri yansıttım.
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
	while (!PickedPiece)//seçmediği sürece devam eden loop.
	{
		//Menuden bir şey seçtimi diye bakıyoruz.
		//eğer seçtiyse bu taşın olduğu indexi seçtiği türden oluşturduğumuz bir taş ile değiştiriyoruz.
		//PieceCount costructorda otomatik arttığı için bizde 1 tane düşürüyoruz.
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
			SDL_Delay(10);//Menuyü beklerken bilgisyara çok yüklenmesin diye loopu kısıtlıyoruz.
		}
	}


}
//Kale için
void Rook::CalcMove(Game* game)
{
	Piece::CalcMove(game);

	//kalenin sağ, sol, yukarı ve aşağısını kontrol ediyoruz. 
	//Eğer PlaceMapta orası boş ise true yapıyoruz. eğer dolu ve bizim takımdan ise direk işaretlemeden çıkıyoruz.
	// eğer dolu ve karşı takımdansa işaretleyip çıkıyoruz. böylelikle karşı takımın taşlarını yiyebiliyoruz.
	int i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x, Pos.y - i);//up // Yukarı
		i++;
	}
	i = 1;
	while (1)
	{
		CheckSetBreak(MoveMap, Pos.x, Pos.y + i);//down // Aşağı
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
		CheckSetBreak(MoveMap, Pos.x + i, Pos.y);//right // Sağ
		i++;
	}

}
//At için
void Knight::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	// Eğer gidebileceği yer tahta içindeyse ve kendi takımından taş yok ise işaretliyoruz.
	Set(MoveMap, Pos.x + 1, Pos.y - 2)
		Set(MoveMap, Pos.x - 1, Pos.y - 2)
		Set(MoveMap, Pos.x + 1, Pos.y + 2)
		Set(MoveMap, Pos.x - 1, Pos.y + 2)
		Set(MoveMap, Pos.x - 2, Pos.y + 1)
		Set(MoveMap, Pos.x - 2, Pos.y - 1)
		Set(MoveMap, Pos.x + 2, Pos.y + 1)
		Set(MoveMap, Pos.x + 2, Pos.y - 1)

}
//fil için
void Bishop::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	//Kale ile aynı işlem sadece yön değişiyor.
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
//Vezir için
void Queen::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	//Kale ve fildeki kodların birleşimi
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
//Şah için
void King::CalcMove(Game* game)
{
	Piece::CalcMove(game);
	//ilk olarak gidebileceği yerleri eğer kendi takımından taş yok ve harita dışında değilse işaretliyoruz.
	Set(MoveMap, Pos.x - 1, Pos.y + 1);
	Set(MoveMap, Pos.x, Pos.y + 1);
	Set(MoveMap, Pos.x + 1, Pos.y + 1);

	Set(MoveMap, Pos.x - 1, Pos.y);
	Set(MoveMap, Pos.x + 1, Pos.y);


	Set(MoveMap, Pos.x - 1, Pos.y - 1);
	Set(MoveMap, Pos.x, Pos.y - 1);
	Set(MoveMap, Pos.x + 1, Pos.y - 1);

	//Daha sonra rengine göre eğer karşı takımın danger mapında gidebiliceği yer doğru ise movemapda false yapıyoruz.
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
	// Rok için kontrol
	if (Bonus)//eğer bonusu var yani haraket etmediyse.
	{
		if (Color == Side_White)
		{
			//Kısa rok için
			//Burada Rok çok özel bir durum olduğu için makro kullanmadım.
			//Rok için gideceği yerler tehdit altında değil ise, gideceği yerlerde taş yok ve en sağdaki kaleninde bonusu var ise.
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
			//Bu sefer sol için bakıyoruz. Yani Uzun rok.
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
	//şah matı test etmek için.

	if (game->PieceC.PlaceMap[y][x].Color == Side_Empty)//Eğer test edilecek karede taş yoksa direk oraya taşı koyuyoruz.
	{
		Pos.y = y;
		Pos.x = x;
		return -1;
	}
	else // varsa taşı oraya koyuyoruz daha sonra orda olan taşın yok olma flagini true yapıyoruz. ve taşın indexini dönüyoruz.
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
	IsCheck = true; // şah olduğunu doğru yapıroruz.
	IsTesting = true;
	Mate(game);// mat var mı diye kotnrol ediyoruz.
	IsTesting = false;
}
//Şah Mat
void King::Mate(Game* game)
{
	bool CanMove = false;
	game->PieceC.CalcMoveMap(game);
	bool IsMate = true;
	//şahın gidebileceği yer var mı diye bakıyoruz.
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

	//Şahın gidebileceği yer yok ise bidahaki tur için şah olmayan bir varyasyon arıyoruz.
	if (CanMove == false)
	{
		for (int a = 0; a < game->PieceCount; a++)
		{
			if (game->PieceC.Pieces[a]->Type == Type_Dummy) // Dummyi buluyoruz.
			{
				//dummyi uyandırıyoruz.
				game->PieceC.Pieces[a]->IsDestroyed = false;
				game->PieceC.Pieces[a]->Color = Color;
				MateDummy* dummy = (MateDummy*)game->PieceC.Pieces[a];

				if (Color == Side_White)
				{

					//MoveMaptaki tüm yerleri test ediyoruz. Yan movemaptaki tüm yerlere tek tek dummy koyuyoruz ve gene şah mı diye kontrol ediyoruz.
					//Eğer şah olmayan bir varyasyon bulursak mat olmadığını anlıyoruz.
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
								if (TestItem != -1) // Eğer movemaptaki koyduğumuz yerde taş var ise yok olma flagini true yapmıştık şimdi false yapıyoruz.
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
	//Daha sonra bozduğumuz bir şey olabilir diye tüm mapları yeniliyoruz.
	game->PieceC.CalcAll(game);

	if (IsMate)// Daha sonra çıkan sonucu oyun durumuna eşitliyoruz.
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

void PieceController::CalcAll(Game* game) // Her şeyi tek bir fonksiyonda tutmak için
{
	CalcPiecePositions(game);
	CalcMoveMap(game);
	CalcDangerMap(game, true);
}





Move Piece::Move(TilePos Pos2, Game* game)
{
	//	game->PieceC.CalcMoveMap(game);
	if (MoveMap[Pos2.y][Pos2.x])//Eğer gideceği yer taşın MoveMapında true ise
	{
		if (game->PieceC.PlaceMap[Pos2.y][Pos2.x].Color != Side_Empty)//Eğer gideceği yer boş değil ise
		{
			for (int i = 0; i < game->PieceCount; i++)
			{
				if (game->PieceC.Pieces[i]->IsDestroyed == true)
					continue;

				if (i != PieceIndex && game->PieceC.Pieces[i]->Pos == Pos2)// gideceği yerdeki taşı bul.
				{
					if (game->PieceC.Pieces[i]->Color != Color)//Eğer senin takımından değil ise yok et.
					{
						game->PieceC.Pieces[i]->Destroy();

					}
					else //senin takımındansa MoveMap hesaplamada bir sıkıntı vardır 
					{
						std::cout << "Move Map Error!!!" << std::endl;
						return Move_SameColor;
					}

				}

			}

		}



		TilePos Test = Pos;
		Pos = Pos2;
		//NOTE Imposible move stuff
		if (game->PieceC.CalcDangerMap(game, false) == Color)
		{
			Pos = Test;
			game->PieceC.CalcDangerMap(game, false);
			return Move_Imposible;


		}
		//NOTE Castling Stuff
		//Rok için
		if (Type == Type_King) // eğer haraket eden şah ise
		{
			//Rok structlarını kontrol ediyoruz. Eğer rok var ve gittiği yerde rokun belirttiği yer ise kalenin yerinide değiştiriyoruz.
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
		//Geçerken alma için
		if (Type == Type_Pawn)//Eğer haraket eden piyon ise
		{
			// geçerken alma strcutlarını kontrol ediyoruz eğer biri doğru ve gittiğimiz yerde onun istediği yer ise arkasına geçtiğimiz taşı yok ediyoruz.
			Pawn* pawn = (Pawn*)this;
			if (pawn->CWP[0].IsValid == true && pawn->CWP[0].Pos == Pos2)
			{
				game->PieceC.Pieces[pawn->CWP[0].PawnIndex]->Destroy();
			}
			if (pawn->CWP[1].IsValid == true && pawn->CWP[1].Pos == Pos2)
			{
				game->PieceC.Pieces[pawn->CWP[1].PawnIndex]->Destroy();
			}

			if ((Pos2.y - Test.y == -2 || Pos2.y - Test.y == 2) && Bonus == true)//2 kare açıldı ise 1 turluğuna
			{
				pawn->CanCapturedWhilePassing = true;//geçerken almaya uygundur diye işaretliyoruz.
			}





		}
		Bonus = false;//Haraket eder ise bonusu kaldırıyoruz.
		game->Cond = Condution_Empty;
		game->PieceC.CalcAll(game);//Mapları hesaplıyoruz.




	}
	else return Move_MoveMapIsNotTrue;

	return Move_Sucsess;




}

void Piece::Destroy() //bunu ilerde yok ederke farklı bir şeyler yapmak istersem diye koydum.
{
	IsDestroyed = true;
}




//Takımların MoveMaplarını Hesaplama
void PieceController::CalcMoveMap(Game* game)
{
	//İlk olarak tüm haritaları temizliyoruz.
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
		{
			WhiteMoveMap[j][i] = false;
			BlackMoveMap[j][i] = false;
		}

	for (int a = 0; a < game->PieceCount; a++)
	{
		if (game->PieceC.Pieces[a]->IsDestroyed == true) //Eğer yok olmadı ise
			continue;
		Pieces[a]->CalcMove(game); // Hareketini hesaplatıyoruz.

		if (Pieces[a]->Color == Side_White) // Daha sonra takımına göre Movemap ile or yapıyoruz böylelikle tüm mapları birleştirmiş oluyoruz.
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
	//Pat için yani berabere
	bool IsDraw = true;

	if (game->Turn == Side_White) // Eğer tur beyazda ise bidahki tur siyahtatır. Siyahın MoveMapındaki tüm yerler false ise
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
				if (WhiteMoveMap[j][i] == true) // Siyah içinde aynı şey.
				{
					IsDraw = false;
				}
	}


	if (IsDraw) //Pattır
	{
		game->Cond = Condution_Draw;
		game->EndGame();
	}



}
Side PieceController::CalcDangerMap(Game* game, bool IsLogging)
{
	//MoveMap ile aynı matıkta çalışıyor ilk kısmı.
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

	//Haritayı hesapladıktan sonra şah var mı diye konrol ediyoruz.
	//Eğer şah var ise Şah yiyen taşın Check fonksiyonunu çağırıyoruz.
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
				if (WhiteDangerMap[Pieces[a]->Pos.y][Pieces[a]->Pos.x]) // Şah için kontrol
				{

					Result = Side_White;
					game->Cond = Condution_WhiteCheck;
					if (test->IsTesting == false && IsLogging) // Burası stack overflowu engellemek için
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
//Taşların yerini hesaplamak için.
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
	//Daha sonra tüm taşlar için
	for (int i = 0; i < game->PieceCount; i++)
	{
		if (game->PieceC.Pieces[i]->IsDestroyed == true)//Eğer yok olmadı ise
			continue;
		//Placemapı dolduruyoruz.
		TilePos Pos = Pieces[i]->Pos;

		PlaceMap[Pos.y][Pos.x].Color = Pieces[i]->Color;
		PlaceMap[Pos.y][Pos.x].Type = Pieces[i]->Type;
		PlaceMap[Pos.y][Pos.x].PieceID = Pieces[i]->PieceIndex;


	}
}

inline bool PieceController::CheckSetFonc(bool Map[8][8], int x, int y, Side color, bool IsMoveMap) // MoveMap ve DangerMap için sorgular.
{
	if (IsMoveMap)//MoveMap için
	{

		if (PlaceMap[y][x].Color == Side_Empty  //Boş ve sınırlar içerisinde ise direk işaretliyoruz.
			&& x >= 0 && x < 8 && y >= 0 && y < 8)
		{

			Map[y][x] = true;
			return true;
		}
		else if (x < 0 || x > 7 || y < 0 || y > 7) // Sınırlar içinde değil ise çıkıyoruz.
			return false;
		else if (PlaceMap[y][x].Color != color) // Gideceği yerdeki taş karşı takımda ise true yapıp çıkıyouruz
		{
			Map[y][x] = true;
			return false;
		}
		else // Değil ise direk çıkıyoruz.
		{
			return false;
		}
	}
	else // DangerMap için
	{
		//MoveMap ile yaklaşık olarak aynı fakat bunda eğer tehtit ettiğimiz taş şah ise onu yok sayıyoruz çünkü şahların arkasınıda tehdit ederiz.
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
	//Diskten texture yüklemek için.
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
		// Oyundaki taşları oluşturup yerlerine diziyoruz.
		// Hangi rengin aşağıda olcağını Game.h dosyasındaki ColorAtBottom ile seçebilirsiniz.
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
		//Şah
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
		//Şah mat hesaplamada kullanmak için kukla
		MateDummy* Dummy = new MateDummy(game, TilePos(-1, -1), Side_Empty, Type_Dummy);

		//Tüm oyun haritalarını hesaplıyoruz.
		game->PieceC.CalcAll(game);


	}




}
