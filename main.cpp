#include <Novice.h>

const char kWindowTitle[] = "LC1D_12_タカハシ";

struct Vector2
{
	float posX = 0.0f;
	float posY = 0.0f;
};
struct Box
{
	Vector2 worldLeftTop{};
	Vector2 size{};
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	unsigned int color = WHITE;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };
	//==========================================
	//プレイヤーの変数
	//==========================================
	Box player{};
	player.worldLeftTop = { 10,500, };
	player.size = { 128.0f,32.0f };
	const float kMoveSpeed = 10.0f;
	const float kStageWidth = 1280.0f * 4.0f;
	const float kMaxScroll = kStageWidth - 1280.0f;
	//==========================================
	//プレイヤーのジャンプ処理の変数
	//==========================================
	float playerVelocityY = 0.0f;
	const float kGravity = 0.8f;
	const float kJumpPower = 15.0f;
	bool isJumping = false;

	//ジャンプ回数
	int jumpCount = 0;
	const int kMaxJumpCount = 3;
	//==========================================
	// ジャンプアニメーション処理変数
	//==========================================
	int jumpFrame = 0;
	int jumpAnimationTimer = 0;
	const int kJumpFrameCount = 4;
	//==========================================
	//背景用変数
	//=========================================
	Box backGrounds[4]{};
	for (int i = 0; i < 4; i++)
	{
		backGrounds[i].worldLeftTop.posX = 0.0f + 1280 * i;
		backGrounds[i].worldLeftTop.posY = 0.0f;
		backGrounds[i].size = { 1280.0f,720.f };
	}
	//==========================================
	//プレイヤー画像
	//==========================================
	int playerTexture = Novice::LoadTexture("./Resources/images/pengin2.png");
	//ジャンプ画像	
	int playerJumpTexture = Novice::LoadTexture("./Resources/images/jump.png");
	//==========================================
	//背景画像
	//==========================================
	int backgroundTexture[4];
	backgroundTexture[0] = Novice::LoadTexture("./Resources/images/iceBackGround.png");
	backgroundTexture[1] = Novice::LoadTexture("./Resources/images/iceBackGround.png");
	backgroundTexture[2] = Novice::LoadTexture("./Resources/images/iceBackGround.png");
	backgroundTexture[3] = Novice::LoadTexture("./Resources/images/iceBackGround.png");
	//==========================================
	//小物画像
	//==========================================
	int trampoline = 0; //トランポリン
	trampoline = Novice::LoadTexture("./Resources/images/trampoline.png");

	int coin = 0; //コイン
	coin = Novice::LoadTexture("./Resources/images/coin.png");

	//==========================================
	//効果音・BGM
	//==========================================
	int coinSound = Novice::LoadAudio("./Resources/sound/coin.MP3");
	int trampolineSound = Novice::LoadAudio("./Resources/sound/trampoline.MP3");
	int jumpSound = Novice::LoadAudio("./Resources/sound/jump.MP3");
	//==========================================
	// トランポリンの配置座標
	//==========================================
	Vector2 trampolinePositions[] =
	{
		{500.0f, 500.0f},
		{1000.0f,500.0f},
		{1500.0f,500.0f},
		{2200.0f,500.0f},
	};
	//==========================================
	// コインの配置座標(マップチップ)
	// コインマップ
	// 0 = 何もなし
	// 1 = コイン
	//==========================================
	int coinMap[6][100] =
	{
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,0,1},
		{0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	};
	//==========================================
	///トランポリン変数
	//==========================================
	const int kTrampolineCount =
		sizeof(trampolinePositions) / sizeof(trampolinePositions[0]);
	const float kTrampolineBouncePower = 25.0f;
	const int kTrampolineWidth = 32; //横のトランポリンのサイズ
	//==========================================
	//コイン変数
	//==========================================
	const int kCoinMapHeight = 6;
	const int kCoinMapWidth = 100;
	const int kCoinMapSize = 64;
	const float kCoinSize = 64.0f;
	//コイン取得記録変数
	int coinCount = 0;
	//==========================================
	//スクロール値
	//==========================================
	float scrollValue = 0.0f;
	//==========================================
	//スクロール開始位置
	//==========================================
	float scrollStartPositionX = 500.0f;
	//==========================================
	//座標変数
	//==========================================
	const float kGroundY = 500.0f;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		//============================
		//プレイヤー移動処理
		//============================
		player.worldLeftTop.posX += kMoveSpeed;
		if (keys[DIK_A])
		{
			player.worldLeftTop.posX -= 30;
		}
		//===========================
		//ジャンプ
		//===========================
		if (preKeys[DIK_SPACE] == 0 &&
			keys[DIK_SPACE] != 0 &&
			jumpCount < kMaxJumpCount)
		{
			Novice::PlayAudio(jumpSound, false, 0.8f);
			playerVelocityY = -kJumpPower;
			isJumping = true;
			jumpCount++;
		}
		if (isJumping)
		{
			// ジャンプアニメーション
			jumpAnimationTimer++;

			if (jumpAnimationTimer >= 10)
			{
				jumpAnimationTimer = 0;
				jumpFrame++;

				if (jumpFrame >= kJumpFrameCount)
				{
					jumpFrame = 0;
				}
			}
		}
		//===========================
		//重力
		//===========================
		if (isJumping)
		{
			playerVelocityY += kGravity;
			player.worldLeftTop.posY += playerVelocityY;
		}
		//===========================
		//トランポリン判定
		//===========================
		for (int i = 0; i < kTrampolineCount; i++)
		{
			float trampolineX = trampolinePositions[i].posX;
			float trampolineY = trampolinePositions[i].posY;

			// トランポリンの横幅
			float trampolineRight = trampolineX + kTrampolineWidth;

			// プレイヤーの足
			float playerBottom = player.worldLeftTop.posY + player.size.posY;

			// プレイヤーの右端
			float playerRight = player.worldLeftTop.posX + player.size.posX;

			// 落下中
			if (playerVelocityY > 0.0f)
			{
				// X方向で重なっている
				if (player.worldLeftTop.posX < trampolineRight && playerRight > trampolineX)
				{
					// トランポリンの上に到達した
					if (playerBottom >= trampolineY && playerBottom <= trampolineY + 20.0f)
					{
						// トランポリンの上に移動
						player.worldLeftTop.posY = trampolineY - player.size.posY;

						// 大きくジャンプ
						playerVelocityY = -kTrampolineBouncePower;
						Novice::PlayAudio(trampolineSound, false, 0.8f);
						isJumping = true;
					}
				}
			}
		}
		//===========================
		//地面判定
		//===========================
		if (player.worldLeftTop.posY >= kGroundY)
		{
			player.worldLeftTop.posY = kGroundY;
			playerVelocityY = 0.0f;
			isJumping = false;
			//ジャンプ回数リセット
			jumpCount = 0;
		}
		//===========================
		//コイン判定
		//===========================
		for (int y = 0; y < kCoinMapHeight; y++)
		{
			for (int x = 0; x < kCoinMapWidth; x++)
			{
				// 1ならコインがある
				if (coinMap[y][x] != 1)
				{
					continue;
				}

				// コインのワールド座標
				float coinX = static_cast<float>(x * kCoinMapSize);
				float coinY = static_cast<float>(y * kCoinMapSize);

				// コインの右端・下端
				float coinRight = coinX + kCoinSize;
				float coinBottom = coinY + kCoinSize;

				// プレイヤーの右端・下端
				float playerRight = player.worldLeftTop.posX + player.size.posX;

				float playerBottom = player.worldLeftTop.posY + player.size.posY;

				// プレイヤーとコインが重なった
				if (player.worldLeftTop.posX < coinRight &&
					playerRight > coinX &&
					player.worldLeftTop.posY < coinBottom &&
					playerBottom > coinY)
				{
					// コインを消す
					coinMap[y][x] = 0;

					// 取得枚数を増やす
					coinCount++;
					Novice::PlayAudio(coinSound, false, 0.8f);
				}
			}
		}
		//===========================
		//壁判定
		//===========================
		if (player.worldLeftTop.posX < 0.0f)
		{
			player.worldLeftTop.posX = 0.0f;
		}

		if (player.worldLeftTop.posX > kStageWidth - player.size.posX)
		{
			player.worldLeftTop.posX = kStageWidth - player.size.posX;
		}
		//===========================
		//スクロール処理
		//===========================
		scrollValue = player.worldLeftTop.posX + player.size.posX - scrollStartPositionX;

		if (scrollValue < 0.0f)
		{
			scrollValue = 0.0f;
		}

		if (scrollValue > kMaxScroll)
		{
			scrollValue = kMaxScroll;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//============================
		//画像描画処理
		//============================
		for (int i = 0; i < 4; i++)
		{
			//スクロール値の計算
			const float kBackGroudScreenLeftTopX =
				backGrounds[i].worldLeftTop.posX - scrollValue;
			Novice::DrawSprite(
				static_cast<int>(kBackGroudScreenLeftTopX),
				static_cast<int>(backGrounds[i].worldLeftTop.posY),
				backgroundTexture[i],
				1.0f,
				1.0f,
				0.0f,
				WHITE
			);
		}

		//===========================
		// トランポリン描画
		//===========================
		for (int i = 0; i < kTrampolineCount; i++)
		{
			int screenX =
				static_cast<int>(trampolinePositions[i].posX - scrollValue);

			int screenY =
				static_cast<int>(trampolinePositions[i].posY);

			Novice::DrawSprite(
				screenX,
				screenY,
				trampoline,
				1.0f,
				1.0f,
				0.0f,
				WHITE
			);
		}

		//===========================
		// コイン描画
		//===========================
		for (int y = 0; y < kCoinMapHeight; y++)
		{
			for (int x = 0; x < kCoinMapWidth; x++)
			{
				// コインがない場所
				if (coinMap[y][x] != 1)
				{
					continue;
				}

				float coinX = static_cast<float>(x * kCoinMapSize);
				float coinY = static_cast<float>(y * kCoinMapSize);

				int screenX = static_cast<int>(coinX - scrollValue);

				int screenY = static_cast<int>(coinY);

				Novice::DrawSprite(
					screenX,
					screenY,
					coin,
					1.0f,
					1.0f,
					0.0f,
					WHITE
				);
			}
		}
		//===========================
		// コイン枚数描画(デバック用)
		//===========================
		Novice::ScreenPrintf(
			20,
			20,
			"COIN : %d",
			coinCount
		);

		//===========================
		//プレイヤー描画処理
		//===========================
		//ジャンプ画像の描画
		if (isJumping)
		{
			Novice::DrawSpriteRect(
				static_cast<int>(player.worldLeftTop.posX - scrollValue),
				static_cast<int>(player.worldLeftTop.posY),
				jumpFrame * 128,
				0,
				128,
				128,
				playerJumpTexture,
				1.0f / 4.0f,
				1.0f,
				0.0f,
				WHITE
			);
		}
		//通常時の描画
		else
		{
			Novice::DrawSprite(
				static_cast<int>(player.worldLeftTop.posX - scrollValue),
				static_cast<int>(player.worldLeftTop.posY),
				playerTexture,
				1.0f,
				1.0f,
				0.0f,
				WHITE
			);
		}
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
