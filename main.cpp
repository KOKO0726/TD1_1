#include <Novice.h>
#include <string.h>
#include <math.h>

const char kWindowTitle[] = "LC1D_12_タカハシ";

enum GameShene
{
	Title,
	Game,
	tutorial,
	BonusStage,
	Clear
};

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

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{

	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//==========================================
	// プレイヤー
	//==========================================
	Box player{};
	player.worldLeftTop = { 10.0f, 500.0f };
	player.size = { 128.0f, 32.0f };

	const float kMoveSpeed = 10.0f;
	const float kStageWidth = 1280.0f * 4.0f;
	//const float kMaxScroll = kStageWidth - 1280.0f;
	const float kBackgroundWidth = 1280.0f * 4.0f;

	float savedPlayerX = 0.0f;
	//==========================================
	// ステージセット変数
	//==========================================
	int stageSet =static_cast<int>(player.worldLeftTop.posX / kStageWidth);
	//==========================================
	// 宇宙ステージ
	//==========================================
	int universeFrame = 0;
	int universeAnimationTimer = 0;
	const int kUniverseFrameCount = 4;

	int batteryFrame = 0;
	int batteryTimer = 0;
	const int kBatteryFrameCount = 6;

	bool isTransitioningToBonus = false;
	int bonusTransitionTimer = 0;
	const int kMaxTransitionTime = 60;

	bool isTransitioningFromBonus = false;
	int bonusExitTransitionTimer = 0;

	// ボーナスステージ用 マップチップ設定 (充電・ダメージアイテム用)
	const int kBonusMapHeight = 10;
	const int kBonusMapWidth = 100;
	const int kBonusTileSize = 64;
	int bonusMap[kBonusMapHeight][kBonusMapWidth] = {};

	// 背景ループごとの獲得フラグ (各ループ・各マス用)
	const int kMaxBonusLoops = 10;
	bool bonusItemCollected[kMaxBonusLoops][kBonusMapHeight][kBonusMapWidth] = {};

	//==========================================
	// ジャンプ
	//==========================================
	float playerVelocityY = 0.0f;
	const float kGravity = 0.8f;
	const float kJumpPower = 15.0f;
	bool isJumping = false;

	int jumpCount = 0;
	const int kMaxJumpCount = 3;

	int jumpFrame = 0;
	int jumpAnimationTimer = 0;
	const int kJumpFrameCount = 4;

	//==========================================
	// 画像
	//==========================================
	// バッテリー
	int batteryTexture =
		Novice::LoadTexture("./Resources/images/battery.png");
	// ダメージバッテリー
	int damagebatteryTexture =
		Novice::LoadTexture("./Resources/images/damagebattery.png");
	// 電池
	int chargingTexture =
		Novice::LoadTexture("./Resources/images/charging.png");
	// プレイヤー
	int playerTexture =
		Novice::LoadTexture("./Resources/images/pengin2.png");
	// ジャンプアニメーション
	int playerJumpTexture =
		Novice::LoadTexture("./Resources/images/jump.png");
	// 宇宙ペンギン
	int universepengin =
		Novice::LoadTexture("./Resources/images/universepengin.png");
	// 宇宙ペンギン2 (バッテリー0時)
	int universepengin2Texture =
		Novice::LoadTexture("./Resources/images/universepengin2.png");
	// 背景
	int backgroundTexture =
		Novice::LoadTexture("./Resources/images/iceBackGround.png");
	// 宇宙ステージ1
	int universeTexture =
		Novice::LoadTexture("./Resources/images/universe.png");
	// 宇宙ステージ2
	int universe2Texture =
		Novice::LoadTexture("./Resources/images/universe2.png");
	// タイトル文字1
	int titleTexture1 =
		Novice::LoadTexture("./Resources/images/penginno.png");
	// タイトル文字2
	int titleTexture2 =
		Novice::LoadTexture("./Resources/images/harasuberi.png");
	// メニュー選択
	int menuTextures[3] = {
		Novice::LoadTexture("./Resources/images/space.png"),
		Novice::LoadTexture("./Resources/images/tutorial.png"),
		Novice::LoadTexture("./Resources/images/credit.png")
	};
	// ルール画面
	int ruleTexture =
		Novice::LoadTexture("./Resources/images/ruleFormat.png");
	// 選択矢印
	int selectArrowTexture =
		Novice::LoadTexture("./Resources/images/command.png");
	// PUSH SPACE
	int pushSpaceTexture =
		Novice::LoadTexture("./Resources/images/pushSpace.png");
	// トランポリン
	int trampoline =
		Novice::LoadTexture("./Resources/images/trampoline.png");
	// コイン
	int coin =
		Novice::LoadTexture("./Resources/images/coin.png");
	//看板
	int signboard =
		Novice::LoadTexture("./Resources/images/signboard.png");
	int goalHouseTexture = Novice::LoadTexture("./Resources/images/goalSnowHouse.png");
	int goalFlagTexture = Novice::LoadTexture("./Resources/images/goalFlag.png");
	int clearTexture = Novice::LoadTexture("./Resources/images/kuria.png");

	//==========================================
	// 効果音
	//==========================================
	int fireSound =
		Novice::LoadAudio("./Resources/sound/fire.MP3");

	// ★fireの再生ハンドル
	int firePlayHandle = -1;

	int coinSound =
		Novice::LoadAudio("./Resources/sound/coin.MP3");

	int trampolineSound =
		Novice::LoadAudio("./Resources/sound/trampoline.MP3");

	int jumpSound =
		Novice::LoadAudio("./Resources/sound/jump.MP3");

	int cursorSound =
		Novice::LoadAudio("./Resources/sound/cursor.MP3");

	int pushSound =
		Novice::LoadAudio("./Resources/sound/push.MP3");

	//==========================================
	// BGM
	//==========================================
	//タイトル
	int kTitleHandle =
		Novice::LoadAudio("./Resources/bgm/title.MP3");
	//チュートリアル
	int kTutorialHandle =
		Novice::LoadAudio("./Resources/bgm/tutorial.MP3");
	//ステージ
	int kStageHandle =
		Novice::LoadAudio("./Resources/bgm/stage.MP3");
	//ボーナスステージ
	int kBonusHandle =
		Novice::LoadAudio("./Resources/bgm/bonus.MP3");

	// BGMの再生ハンドル
	int titlePlayHandle = -1;
	int stagePlayHandle = -1;
	int bonusPlayHandle = -1;

	if (kTitleHandle != -1) {
		titlePlayHandle =
			Novice::PlayAudio(kTitleHandle, true, 0.5f);
	}

	//==========================================
	// トランポリン
	//==========================================
	Vector2 trampolinePositions[] = {
		{ 3000.0f, 500.0f },
		{ 5000.0f, 500.0f }
	};

	const int kTrampolineCount =
		sizeof(trampolinePositions) / sizeof(trampolinePositions[0]);

	const float kTrampolineBouncePower = 40.0f;
	const int kTrampolineWidth = 32;
	//==========================================
	// 看板配置
	//==========================================
	Vector2 signboardPositions[] =
	{
		{ 350.0f, 500.0f },
		{ 850.0f, 500.0f },
		{ 1350.0f, 500.0f },
		{ 2050.0f, 500.0f },
	};
	//==========================================
	// コイン
	//==========================================
	const int kCoinMapHeight = 6;
	const int kCoinMapWidth = 100;
	const int kCoinMapSize = 64;
	const float kCoinSize = 64.0f;

	// ステージのコイン配置マップ（1: コインあり, 0: コインなし）
	int coinMap[kCoinMapHeight][kCoinMapWidth] = 
	{
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
	};
	//==========================================
	// ボーナスステージ専用コインマップ
	//==========================================
	const int kBonusCoinMapHeight = 11;
	const int kBonusCoinMapWidth = 100;
	const int kBonusCoinMapSize = 64;

	const int kMaxCoinStageSets = 100;

	// 1ループは2560px、1マスは64pxなので40マス
	const int kBonusCoinLoopTileCount = 40;
	const float kBonusCoinLoopWidth = 2560.0f;

	// ループごとにコインの取得状態を管理
	bool bonusCoinCollected[kMaxBonusLoops]
		[kBonusCoinMapHeight][kBonusCoinMapWidth] = {};

	static bool coinCollectedByStage[kMaxCoinStageSets]
		[kCoinMapHeight][kCoinMapWidth] = {};
	int bonusCoinMap[kBonusCoinMapHeight][kBonusCoinMapWidth] =
	{
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }
	};
	// リトライ時にコインを復元するため、初期配置を保存
	int initialCoinMap[kCoinMapHeight][kCoinMapWidth] = {};
	memcpy(initialCoinMap, coinMap, sizeof(coinMap));

	// ゴール（200x200画像を等倍表示）
	const float kGoalX = 30000.0f; // かまくら・旗・ゴール判定を200px手前へ
	const float kGoalY = 350.0f;
	bool isGameClear = false;
	int goalTimer = 0;
	int goalFlagFrame = 0;
	int clearTimer = 0;
	const int kGoalWaitFrames = 180;
	const int kClearShowFrames = 420;
	int coinCount = 0;

	// 看板の配置数
	const int kSignboardCount =
		sizeof(signboardPositions) / sizeof(signboardPositions[0]);
	// 看板の幅（画像サイズに合わせて調整）
	const int kSignboardWidth = 64;

	//==========================================
	// スクロール
	//==========================================
	float scrollValue = 0.0f;
	const float scrollStartPositionX = 500.0f;
	const float kGroundY = 500.0f;

	//==========================================
	// シーン
	//==========================================
	int scene = Title;

	int timer = 0;
	int gameFadeTimer = 0;
	int selectedMenu = 0;

	bool isRuleOpen = false;

	bool isTitleFading = false;
	int titleFadeTimer = 0;
	const int kTitleFadeDuration = 90;

	//==========================================
	// タイトルアニメーション
	//==========================================
	float title1Y = -180.0f;
	float title2Y = -250.0f;

	float title1Velocity = 0.0f;
	float title2Velocity = 0.0f;

	float penguinX = -200.0f;
	float penguinY = 380.0f;
	const float penguinSpeed = 8.0f;

	//==========================================
	// メインループ
	//==========================================
	while (Novice::ProcessMessage() == 0) {

		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		switch (scene) {

			//==========================================
			// タイトル
			//==========================================
		case Title: {

			timer++;

			// タイトル1の落下
			if (timer > 60) {

				if (title1Y < 20.0f ||
					title1Velocity != 0.0f) {

					title1Velocity += 0.7f;
					title1Y += title1Velocity;

					if (title1Y >= 20.0f) {

						title1Y = 20.0f;
						title1Velocity *= -0.4f;

						if (fabsf(title1Velocity) < 1.5f) {
							title1Velocity = 0.0f;
						}
					}
				}
			}

			// タイトル2の落下
			if (timer > 85) {

				if (title2Y < 150.0f ||
					title2Velocity != 0.0f) {

					title2Velocity += 0.7f;
					title2Y += title2Velocity;

					if (title2Y >= 150.0f) {

						title2Y = 150.0f;
						title2Velocity *= -0.4f;

						if (fabsf(title2Velocity) < 1.5f) {
							title2Velocity = 0.0f;
						}
					}
				}
			}

			// ペンギン移動
			if (timer > 180) {

				penguinX += penguinSpeed;

				if (penguinX > 1280.0f) {
					penguinX = -200.0f;
				}
			}

			if (timer > 300) {
				penguinY =
					380.0f + sinf(timer * 0.07f) * 5.0f;
			}

			//==========================================
			// メニュー操作
			//==========================================
			if (timer > 365 && !isTitleFading) {

				if (isRuleOpen) {

					if (preKeys[DIK_SPACE] == 0 &&
						keys[DIK_SPACE] != 0) {

						isRuleOpen = false;
					}

				}
				else {

					// 左
					if ((preKeys[DIK_LEFT] == 0 &&
						keys[DIK_LEFT] != 0) ||
						(preKeys[DIK_A] == 0 &&
							keys[DIK_A] != 0)) {

						selectedMenu = (selectedMenu + 2) % 3;

						if (cursorSound != -1) {
							Novice::PlayAudio(
								cursorSound, false, 0.8f
							);
						}
					}

					// 右
					if ((preKeys[DIK_RIGHT] == 0 &&
						keys[DIK_RIGHT] != 0) ||
						(preKeys[DIK_D] == 0 &&
							keys[DIK_D] != 0)) {

						selectedMenu = (selectedMenu + 1) % 3;

						if (cursorSound != -1) {
							Novice::PlayAudio(
								cursorSound, false, 0.8f
							);
						}
					}

					// 決定
					if (preKeys[DIK_SPACE] == 0 &&
						keys[DIK_SPACE] != 0) {

						if (selectedMenu == 2) {

							isRuleOpen = true;

						}
						else {

							if (pushSound != -1) {
								Novice::PlayAudio(
									pushSound, false, 0.8f
								);
							}

							isTitleFading = true;
							titleFadeTimer = 0;
						}
					}
				}
			}

			//==========================================
			// タイトルフェードアウト
			//==========================================
			if (isTitleFading) {

				titleFadeTimer++;

				if (titleFadeTimer >= kTitleFadeDuration) {

					if (titlePlayHandle != -1) {
						Novice::StopAudio(titlePlayHandle);
						titlePlayHandle = -1;
					}

					if (selectedMenu == 0) {
						scene = Game;

						// ステージBGMを再生
						if (kStageHandle != -1) {
							stagePlayHandle =
								Novice::PlayAudio(kStageHandle, true, 1.0f);
						}
					}
					else if (selectedMenu == 1) {

						scene = tutorial;

						if (kTutorialHandle != -1) {
							Novice::PlayAudio(
								kTutorialHandle, true, 0.5f
							);
						}
					}

					gameFadeTimer = 0;
					isTitleFading = false;
				}
			}

			//==========================================
			// 背景
			//==========================================
			if (backgroundTexture != -1) {
				Novice::DrawSprite(
					0, 0,
					backgroundTexture,
					1.0f, 1.0f, 0.0f, WHITE
				);
			}

			// タイトル文字
			if (timer > 60 && titleTexture1 != -1) {
				Novice::DrawSprite(
					430,
					static_cast<int>(title1Y),
					titleTexture1,
					1.5f, 1.5f, 0.0f, WHITE
				);
			}

			if (timer > 85 && titleTexture2 != -1) {
				Novice::DrawSprite(
					400,
					static_cast<int>(title2Y),
					titleTexture2,
					2.0f, 2.0f, 0.0f, WHITE
				);
			}

			// ペンギン
			if (timer > 180 && playerTexture != -1) {
				Novice::DrawSprite(
					static_cast<int>(penguinX),
					static_cast<int>(penguinY),
					playerTexture,
					1.5f, 1.5f, 0.0f, WHITE
				);
			}

			//==========================================
			// メニュー
			//==========================================
			if (timer > 300) {

				const int menuX[3] = { 185, 515, 845 };
				const int menuY = 485;

				for (int i = 0; i < 3; i++) {

					int elapsed = timer - 301 - i * 9;

					if (elapsed < 0 || menuTextures[i] == -1) {
						continue;
					}

					float offsetY = 0.0f;

					if (elapsed < 18) {

						float t =
							static_cast<float>(elapsed) / 18.0f;

						float eased =
							1.0f - (1.0f - t) * (1.0f - t);

						offsetY = 180.0f - 200.0f * eased;

					}
					else if (elapsed < 28) {

						float t =
							static_cast<float>(elapsed - 18) / 10.0f;

						offsetY = -20.0f + 20.0f * t;
					}

					Novice::DrawSprite(
						menuX[i],
						menuY + static_cast<int>(offsetY),
						menuTextures[i],
						1.0f, 1.0f, 0.0f, WHITE
					);
				}

				if (timer > 365 && selectArrowTexture != -1) {
					Novice::DrawSprite(
						menuX[selectedMenu] + 109,
						menuY - 38,
						selectArrowTexture,
						1.0f, 1.0f, 0.0f, WHITE
					);
				}
			}

			// PUSH SPACE
			if (timer > 365 && pushSpaceTexture != -1) {

				if ((timer / 30) % 2 == 0) {
					Novice::DrawSprite(
						500, 585,
						pushSpaceTexture,
						2.0f, 2.0f, 0.0f, WHITE
					);
				}
			}

			// タイトル開始フェードイン
			if (timer <= 60) {

				int alpha = 255 - timer * 255 / 60;

				if (alpha < 0) {
					alpha = 0;
				}

				Novice::DrawBox(
					0, 0, 1280, 720,
					0.0f,
					static_cast<unsigned int>(alpha),
					kFillModeSolid
				);
			}

			// 決定後フェードアウト
			if (isTitleFading) {

				int alpha =
					titleFadeTimer * 255 / kTitleFadeDuration;

				if (alpha > 255) {
					alpha = 255;
				}

				Novice::DrawBox(
					0, 0, 1280, 720,
					0.0f,
					static_cast<unsigned int>(alpha),
					kFillModeSolid
				);
			}

			//==========================================
			// 基本操作画面
			//==========================================
			if (isRuleOpen) {

				if (ruleTexture != -1) {
					Novice::DrawSprite(
						0, 0,
						ruleTexture,
						1.0f, 1.0f, 0.0f, WHITE
					);
				}

				if (playerTexture != -1) {
					Novice::DrawSprite(
						700, 150,
						playerTexture,
						1.5f, 1.5f, 0.0f, WHITE
					);
				}

				if (coin != -1) {
					Novice::DrawSprite(
						870, 175,
						coin,
						1.5f, 1.5f, 0.0f, WHITE
					);
				}

				if (trampoline != -1) {
					Novice::DrawSprite(
						850, 400,
						trampoline,
						1.5f, 1.5f, 0.0f, WHITE
					);
				}

				if (playerTexture != -1) {
					Novice::DrawSprite(
						850, 340,
						playerTexture,
						1.5f, 1.5f, 0.0f, WHITE
					);
				}
			}

			break;
		}

		 //==========================================
		 // 通常ゲーム
		 //==========================================

		case Game: {
			//==========================================
			// 現在のステージセットを更新
			//==========================================
			stageSet = static_cast<int>(player.worldLeftTop.posX / kStageWidth);

			//==========================================
			// 更新処理
			//==========================================
			if (!isTransitioningToBonus && !isGameClear) {
				player.worldLeftTop.posX += kMoveSpeed;

				if (keys[DIK_A]) {
					player.worldLeftTop.posX -= 30.0f;//デバック用
				}

				if (preKeys[DIK_SPACE] == 0 &&
					keys[DIK_SPACE] != 0 &&
					jumpCount < kMaxJumpCount) {

					playerVelocityY = -kJumpPower;
					isJumping = true;
					jumpCount++;

					if (jumpSound != -1) {
						Novice::PlayAudio(
							jumpSound, false, 0.8f
						);

					}
				}
			}

			// ジャンプアニメーション
			if (isJumping) {

				jumpAnimationTimer++;

				if (jumpAnimationTimer >= 10) {

					jumpAnimationTimer = 0;
					jumpFrame =
						(jumpFrame + 1) % kJumpFrameCount;
				}
			}

			

			// 重力
			if (isJumping) {

				playerVelocityY += kGravity;
				player.worldLeftTop.posY += playerVelocityY;
			}

			//==========================================
			// トランポリン判定
			//==========================================
			for (int i = 0; i < kTrampolineCount && !isGameClear; i++) {

				float trampolineX =
					trampolinePositions[i].posX;

				float trampolineY =
					trampolinePositions[i].posY;

				float playerRight =
					player.worldLeftTop.posX + player.size.posX;

				float playerBottom =
					player.worldLeftTop.posY + player.size.posY;

				if (playerVelocityY > 0.0f &&
					player.worldLeftTop.posX <
					trampolineX + kTrampolineWidth &&
					playerRight > trampolineX &&
					playerBottom >= trampolineY &&
					playerBottom <= trampolineY + 20.0f) {

					player.worldLeftTop.posY =
						trampolineY - player.size.posY;

					playerVelocityY = -kTrampolineBouncePower;
					isJumping = true;

					if (trampolineSound != -1) {
						Novice::PlayAudio(
							trampolineSound, false, 0.8f
						);
					}
				}
			}

			//==========================================
			// 宇宙へ移動
			//==========================================
			if (player.worldLeftTop.posY < -100.0f &&
				!isTransitioningToBonus && !isGameClear) {

				isTransitioningToBonus = true;
				bonusTransitionTimer = 0;
				savedPlayerX = player.worldLeftTop.posX;
			}

			// 地面判定
			if (player.worldLeftTop.posY >= kGroundY) {

				player.worldLeftTop.posY = kGroundY;
				playerVelocityY = 0.0f;
				isJumping = false;
				jumpCount = 0;
			}

			//==========================================
			// コイン判定
			//==========================================
			int collisionStageSet =
				static_cast<int>(
					player.worldLeftTop.posX / kStageWidth
					);
			int coinCollisionStageSet =
				static_cast<int>(
					player.worldLeftTop.posX / kStageWidth
					);

			for (int set = coinCollisionStageSet - 1;
				set <= collisionStageSet + 1; set++) {

				if (set < 0 || set >= kMaxCoinStageSets) {
					continue;
				}

				for (int y = 0; y < kCoinMapHeight; y++) {
					for (int x = 0; x < kCoinMapWidth; x++) {

						if (coinMap[y][x] != 1 ||
							coinCollectedByStage[set][y][x]) {
							continue;
						}

						float coinX =
							x * kCoinMapSize + set * kStageWidth;

						float coinY =
							static_cast<float>(y * kCoinMapSize);

						float playerRight =
							player.worldLeftTop.posX +
							player.size.posX;

						float playerBottom =
							player.worldLeftTop.posY +
							player.size.posY;

						if (player.worldLeftTop.posX <
							coinX + kCoinSize &&
							playerRight > coinX &&
							player.worldLeftTop.posY <
							coinY + kCoinSize &&
							playerBottom > coinY) {

							coinCollectedByStage[set][y][x] = true;
							coinCount++;

							if (coinSound != -1) {
								Novice::PlayAudio(
									coinSound, false, 0.2f
								);
							}
						}
					}
				}
			}

			// ゴール到達：ペンギンがかまくらに触れたら停止
			if (!isGameClear && !isTransitioningToBonus &&
				player.worldLeftTop.posX + player.size.posX > kGoalX &&
				player.worldLeftTop.posX < kGoalX + 200.0f &&
				player.worldLeftTop.posY + player.size.posY > kGoalY) {
				isGameClear = true;
				isJumping = false;
				playerVelocityY = 0.0f;
				goalTimer = 0;
				goalFlagFrame = 0;
			}
			if (isGameClear) {
				goalTimer++;
				if (goalTimer % 8 == 0 && goalFlagFrame < 3) goalFlagFrame++;
				if (goalTimer >= kGoalWaitFrames) {
					scene = Clear;
					clearTimer = 0;
					if (stagePlayHandle != -1) {
						Novice::StopAudio(stagePlayHandle);
						stagePlayHandle = -1;
					}
				}
			}

			// 画面端
			if (player.worldLeftTop.posX < 0.0f) {
				player.worldLeftTop.posX = 0.0f;
			}

			// ゴールより先に進めないようにする
			if (player.worldLeftTop.posX >
				kGoalX + 200.0f - player.size.posX) {

				player.worldLeftTop.posX =
					kGoalX + 200.0f - player.size.posX;
			}

			//==========================================
			// スクロール
			//==========================================
			scrollValue =
				player.worldLeftTop.posX +
				player.size.posX - scrollStartPositionX;

			if (scrollValue < 0.0f) {
				scrollValue = 0.0f;
			}

			float loopScroll =
				fmodf(scrollValue, kBackgroundWidth);

			// 背景
			for (int i = 0; i < 8; i++) {

				int textureIndex = i % 4;

				float backgroundX =
					1280.0f * i - loopScroll;

				if (textureIndex >= 0 &&
					backgroundTexture != -1) {

					Novice::DrawSprite(
						static_cast<int>(backgroundX),
						0,
						backgroundTexture,
						1.0f, 1.0f, 0.0f, WHITE
					);
				}
			}

				static_cast<int>(
					player.worldLeftTop.posX / kStageWidth
					);

			// 背景
			for (int i = 0; i < 4; i++) {

				if (backgroundTexture != -1) {
					Novice::DrawSprite(
						static_cast<int>(1280.0f * i - scrollValue),
						0,
						backgroundTexture,
						1.0f, 1.0f, 0.0f, WHITE
					);
				}
			}
			
			// トランポリン描画
			for (int i = 0; i < kTrampolineCount; i++) {

				if (trampoline != -1) {
					Novice::DrawSprite(
						static_cast<int>(
							trampolinePositions[i].posX - scrollValue
							),
						static_cast<int>(
							trampolinePositions[i].posY
							),
						trampoline,
						1.0f, 1.0f, 0.0f, WHITE
					);
				}
			}

			// コイン描画
			for (int set = stageSet - 1;
				set <= stageSet + 1; set++) {

				if (set < 0 || set >= kMaxCoinStageSets) {
					continue;
				}

				for (int y = 0; y < kCoinMapHeight; y++) {
					for (int x = 0; x < kCoinMapWidth; x++) {

						if (coinMap[y][x] != 1 ||
							coinCollectedByStage[set][y][x]) {
							continue;
						}

						float coinX =
							x * kCoinMapSize +
							set * kStageWidth;

						int screenX =
							static_cast<int>(coinX - scrollValue);

						if (screenX + kCoinSize < 0 ||
							screenX > 1280) {
							continue;
						}

						if (coin != -1) {
							Novice::DrawSprite(
								screenX,
								y * kCoinMapSize,
								coin,
								1.0f, 1.0f, 0.0f, WHITE
							);
						}
					}
				}
			}

			// かまくらと旗（旗は192x48、48x48の4コマ）
			if (goalHouseTexture != -1) {
				Novice::DrawSprite(static_cast<int>(kGoalX - scrollValue),
					static_cast<int>(kGoalY), goalHouseTexture, 1.0f, 1.0f, 0.0f, WHITE);
			}
			if (goalFlagTexture != -1) {
				Novice::DrawSpriteRect(static_cast<int>(kGoalX + 120.0f - scrollValue),
					static_cast<int>(kGoalY + 28.0f), goalFlagFrame * 48, 0,
					48, 48, goalFlagTexture, 0.375f, 1.5f, 0.0f, WHITE);
			}

			// プレイヤー
			if (isJumping && playerJumpTexture != -1) {

				Novice::DrawSpriteRect(
					static_cast<int>(
						player.worldLeftTop.posX - scrollValue
						),
					static_cast<int>(player.worldLeftTop.posY),
					jumpFrame * 128,
					0,
					128, 128,
					playerJumpTexture,
					0.25f, 1.0f,
					0.0f, WHITE
				);

			}
			else if (playerTexture != -1) {

				Novice::DrawSprite(
					static_cast<int>(
						player.worldLeftTop.posX - scrollValue
						),
					static_cast<int>(player.worldLeftTop.posY),
					playerTexture,
					1.0f, 1.0f, 0.0f, WHITE
				);
			}

			Novice::ScreenPrintf(
				20, 20, "COIN : %d", coinCount
			);

			//==========================================
			// 宇宙へのフェードアウト
			//==========================================
			if (isTransitioningToBonus) {

				bonusTransitionTimer++;

				int alpha =
					bonusTransitionTimer * 255 / kMaxTransitionTime;

				if (alpha > 255) {
					alpha = 255;
				}

				Novice::DrawBox(
					0, 0, 1280, 720,
					0.0f,
					static_cast<unsigned int>(alpha),
					kFillModeSolid
				);

				if (bonusTransitionTimer >= kMaxTransitionTime) {

					scene = BonusStage;
					isTransitioningToBonus = false;
					bonusTransitionTimer = 0;

					batteryFrame = 0;
					batteryTimer = 0;

					isTransitioningFromBonus = false;
					bonusExitTransitionTimer = 0;

					player.worldLeftTop.posY = 200.0f;
					playerVelocityY = 0.0f;

					// 充電・ダメージアイテムの初期化
					for (int y = 0; y < kBonusMapHeight; y++) {
						for (int x = 0; x < kBonusMapWidth; x++) {
							bonusMap[y][x] = 0;
							// ボーナスステージのコイン取得状態をリセット
							memset(bonusCoinCollected, 0, sizeof(bonusCoinCollected));
						}
					}
					// 1つのループパターン（40マス＝2560px）内での配置
					bonusMap[3][20] = 1; // 充電電池 (回復) : 1個
					bonusMap[2][10] = 2; // ダメージ電池 (ダメージ) : 1個目
					bonusMap[4][30] = 2; // ダメージ電池 (ダメージ) : 2個目

					// 獲得フラグのリセット
					for (int i = 0; i < kMaxBonusLoops; i++) {
						for (int y = 0; y < kBonusMapHeight; y++) {
							for (int x = 0; x < kBonusMapWidth; x++) {
								bonusItemCollected[i][y][x] = false;
							}
						}
					}

					//==================================
					// ★宇宙に入ったらfireを常時再生
					//==================================
					if (firePlayHandle != -1) {
						Novice::StopAudio(firePlayHandle);
						firePlayHandle = -1;
					}

					// ステージBGMを停止
					if (stagePlayHandle != -1) {
						Novice::StopAudio(stagePlayHandle);
						stagePlayHandle = -1;
					}

					// ボーナスステージのBGMを再生
					if (kBonusHandle != -1) {
						bonusPlayHandle =
							Novice::PlayAudio(kBonusHandle, true, 0.5f);
					}

					// ロケットの噴射音を再生
					if (fireSound != -1) {
						firePlayHandle =
							Novice::PlayAudio(fireSound, true, 1.0f);
					}
				}
			}
		}
				 break;

		 //==========================================
		 // 宇宙ボーナスステージ
		 //==========================================
		case BonusStage: {

			//==========================================
			// バッテリー更新
			//==========================================
			if (batteryFrame < kBatteryFrameCount - 1) {

				batteryTimer++;

				if (batteryTimer >= 180) {
					batteryTimer = 0;
					batteryFrame++;
				}
			}

			bool isOutOfFuel =
				batteryFrame >= kBatteryFrameCount - 1;

			//==========================================
			// ★燃料切れでfire音を停止
			//==========================================
			if (isOutOfFuel && firePlayHandle != -1) {

				Novice::StopAudio(firePlayHandle);
				firePlayHandle = -1;
			}

			// 自動前進
			player.worldLeftTop.posX += kMoveSpeed * 0.5f;

			//==========================================
			// SPACEで浮上（音は鳴らさない）
			//==========================================
			if (!isOutOfFuel) {

				if (preKeys[DIK_SPACE] == 0 &&
					keys[DIK_SPACE] != 0) {

					playerVelocityY = -6.0f;
				}
			}

			// 重力
			const float kSpaceGravity = 0.25f;

			playerVelocityY += kSpaceGravity;

			if (!isOutOfFuel) {

				if (playerVelocityY > 4.0f) {
					playerVelocityY = 4.0f;
				}

			}
			else {

				if (playerVelocityY > 12.0f) {
					playerVelocityY = 12.0f;
				}
			}

			//==========================================
			// ボーナスステージのコイン判定
			//==========================================
			for (int loop = 0; loop < kMaxBonusLoops; loop++) {
				for (int y = 0; y < kBonusCoinMapHeight; y++) {
					for (int x = 0; x < kBonusCoinLoopTileCount; x++) {

						// コインがなければスキップ
						if (bonusCoinMap[y][x] != 1) {
							continue;
						}

						// このループのコインを取得済みならスキップ
						if (bonusCoinCollected[loop][y][x]) {
							continue;
						}

						// コインのワールド座標
						float coinX =
							x * kBonusCoinMapSize
							+ loop * kBonusCoinLoopWidth;

						int coinY =
							y * kBonusCoinMapSize;

						// プレイヤーの座標
						float playerLeft = player.worldLeftTop.posX;
						float playerRight =
							player.worldLeftTop.posX + player.size.posX;
						float playerTop = player.worldLeftTop.posY;
						float playerBottom =
							player.worldLeftTop.posY + player.size.posY;

						// 当たり判定
						if (playerLeft < coinX + kBonusCoinMapSize &&
							playerRight > coinX &&
							playerTop < coinY + kBonusCoinMapSize &&
							playerBottom > coinY) {

							// このループのコインだけ取得済みにする
							bonusCoinCollected[loop][y][x] = true;

							coinCount++;

							if (coinSound != -1) {
								Novice::PlayAudio(
									coinSound, false, 0.1f
								);
							}
						}
					}
				}
			}

			player.worldLeftTop.posY += playerVelocityY;

			// 天井判定
			if (player.worldLeftTop.posY < 0.0f) {

				player.worldLeftTop.posY = 0.0f;
				playerVelocityY = 0.0f;
			}

			// 背景ループ幅 (2560px)
			const float kLoopWidth = 2560.0f;

			//==========================================
			// 充電・ダメージアイテムとの当たり判定（拾うと消滅）
			//==========================================
			for (int y = 0; y < kBonusMapHeight; y++) {
				for (int x = 0; x < kBonusMapWidth; x++) {

					if (bonusMap[y][x] == 0) {
						continue;
					}

					float baseItemX = static_cast<float>(x * kBonusTileSize);
					float itemY = static_cast<float>(y * kBonusTileSize);

					for (int i = 0; i < kMaxBonusLoops; i++) {

						// 既に獲得済みなら判定を行わない
						if (bonusItemCollected[i][y][x]) {
							continue;
						}

						float itemX = baseItemX + i * kLoopWidth;

						if (player.worldLeftTop.posX < itemX + kBonusTileSize &&
							player.worldLeftTop.posX + player.size.posX > itemX &&
							player.worldLeftTop.posY < itemY + kBonusTileSize &&
							player.worldLeftTop.posY + player.size.posY > itemY) {

							// アイテム獲得フラグを立てて消す
							bonusItemCollected[i][y][x] = true;

							if (bonusMap[y][x] == 1) {
								// 充電電池: バッテリー1個分回復
								if (batteryFrame > 0) {
									batteryFrame--;
									batteryTimer = 0; // タイマーリセット
								}
							}
							else if (bonusMap[y][x] == 2) {
								// ダメージ電池: バッテリー2個分減少
								batteryFrame += 2;
								if (batteryFrame >= kBatteryFrameCount - 1) {
									batteryFrame = kBatteryFrameCount - 1; // 燃料切れの上限固定
								}
								batteryTimer = 0;
							}
						}
					}
				}
			}

			//==========================================
			// 宇宙ペンギンアニメーション
			//==========================================
			universeAnimationTimer++;

			if (universeAnimationTimer >= 8) {

				universeAnimationTimer = 0;

				universeFrame =
					(universeFrame + 1) % kUniverseFrameCount;
			}

			//==========================================
			// 宇宙背景スクロール
			//==========================================
			scrollValue =
				player.worldLeftTop.posX +
				player.size.posX - scrollStartPositionX;

			if (scrollValue < 0.0f) {
				scrollValue = 0.0f;
			}

			float loopScroll =
				fmodf(scrollValue, kLoopWidth);

			float universe1X = -loopScroll;

			if (universe1X <= -1280.0f) {
				universe1X += kLoopWidth;
			}

			float universe2X = 1280.0f - loopScroll;

			if (universe2X <= -1280.0f) {
				universe2X += kLoopWidth;
			}

			// 宇宙背景1
			if (universeTexture != -1) {

				Novice::DrawSprite(
					static_cast<int>(universe1X),
					0,
					universeTexture,
					1.0f, 1.0f, 0.0f, WHITE
				);
			}

			// 宇宙背景2
			if (universe2Texture != -1) {

				Novice::DrawSprite(
					static_cast<int>(universe2X),
					0,
					universe2Texture,
					1.0f, 1.0f, 0.0f, WHITE
				);
			}

			//==========================================
			// 充電・ダメージアイテムの描画 (未獲得の物のみ描画)
			//==========================================
			for (int y = 0; y < kBonusMapHeight; y++) {
				for (int x = 0; x < kBonusMapWidth; x++) {

					int itemType = bonusMap[y][x];
					if (itemType == 0) {
						continue;
					}

					int targetTexture = -1;
					if (itemType == 1) {
						targetTexture = chargingTexture;
					}
					else if (itemType == 2) {
						targetTexture = damagebatteryTexture;
					}

					if (targetTexture != -1) {
						float baseItemX = static_cast<float>(x * kBonusTileSize);
						int drawY = y * kBonusTileSize;

						for (int i = 0; i < kMaxBonusLoops; i++) {

							// 獲得済みなら描画しない
							if (bonusItemCollected[i][y][x]) {
								continue;
							}

							int drawX = static_cast<int>(baseItemX + i * kLoopWidth - scrollValue);

							if (drawX + kBonusTileSize >= 0 && drawX <= 1280) {
								Novice::DrawSprite(
									drawX,
									drawY,
									targetTexture,
									1.0f, 1.0f, 0.0f, WHITE
								);
							}
						}
					}
				}
			}

			//==========================================
			// 宇宙ペンギン描画 (バッテリー0時はuniversepengin2Textureを表示)
			//==========================================
			int drawUniversePenginTex = isOutOfFuel ? universepengin2Texture : universepengin;

			if (drawUniversePenginTex != -1) {

				Novice::DrawSpriteRect(
					static_cast<int>(
						player.worldLeftTop.posX - scrollValue
						),
					static_cast<int>(player.worldLeftTop.posY),
					universeFrame * 128,
					0,
					128, 128,
					drawUniversePenginTex,
					1.0f / static_cast<float>(kUniverseFrameCount),
					1.0f,
					0.0f, WHITE
				);
			}
			//==========================================
			// ボーナスステージのコイン描画
			//==========================================
			for (int loop = 0; loop < kMaxBonusLoops; loop++) {
				for (int y = 0; y < kBonusCoinMapHeight; y++) {
					for (int x = 0; x < kBonusCoinLoopTileCount; x++) {

						if (bonusCoinMap[y][x] != 1 || coin == -1) {
							continue;
						}

						// 取得済みなら描画しない
						if (bonusCoinCollected[loop][y][x]) {
							continue;
						}

						// 当たり判定と同じワールド座標を使う
						float worldX =
							x * kBonusCoinMapSize
							+ loop * kBonusCoinLoopWidth;

						int worldY =
							y * kBonusCoinMapSize;

						// ワールド座標から画面座標へ変換
						int screenX =
							static_cast<int>(worldX - scrollValue);

						int screenY =
							static_cast<int>(worldY);

						// 画面内のコインだけ描画
						if (screenX + kBonusCoinMapSize >= 0 &&
							screenX <= 1280) {

							Novice::DrawSprite(
								screenX,
								screenY,
								coin,
								1.0f, 1.0f, 0.0f, WHITE
							);
						}
					}
				}
			}
			//==========================================
			// バッテリー描画
			//==========================================
			if (batteryTexture != -1) {

				const int kBatteryWidth = 256;
				const int kBatteryHeight = 256;
				const int kBatteryImageWidth =
					kBatteryWidth * kBatteryFrameCount;

				float scale = 0.5f;

				Novice::DrawSpriteRect(
					20, 20,
					batteryFrame * kBatteryWidth,
					0,
					kBatteryWidth,
					kBatteryHeight,
					batteryTexture,
					(static_cast<float>(kBatteryWidth) /
						static_cast<float>(kBatteryImageWidth)) * scale,
					scale,
					0.0f, WHITE
				);
			}

			//==========================================
			// 画面外に落ちたら戻る
			//==========================================
			if (player.worldLeftTop.posY >= 720.0f &&
				!isTransitioningFromBonus) {

				isTransitioningFromBonus = true;
				bonusExitTransitionTimer = 0;
			}

			// 宇宙開始時フェードイン
			if (bonusTransitionTimer < kMaxTransitionTime) {

				bonusTransitionTimer++;

				int alpha =
					255 -
					bonusTransitionTimer * 255 / kMaxTransitionTime;

				if (alpha < 0) {
					alpha = 0;
				}

				Novice::DrawBox(
					0, 0, 1280, 720,
					0.0f,
					static_cast<unsigned int>(alpha),
					kFillModeSolid
				);
			}

			//==========================================
			// 宇宙終了時フェードアウト
			//==========================================
			if (isTransitioningFromBonus) {

				bonusExitTransitionTimer++;

				int alpha =
					bonusExitTransitionTimer * 255 / 30;

				if (alpha > 255) {
					alpha = 255;
				}

				Novice::DrawBox(
					0, 0, 1280, 720,
					0.0f,
					static_cast<unsigned int>(alpha),
					kFillModeSolid
				);

				if (bonusExitTransitionTimer >= 30) {

					// 通常ステージに戻る時
					// 噴射音を停止
					if (firePlayHandle != -1) {
						Novice::StopAudio(firePlayHandle);
						firePlayHandle = -1;
					}

					// ボーナスステージのBGMを停止
					if (bonusPlayHandle != -1) {
						Novice::StopAudio(bonusPlayHandle);
						bonusPlayHandle = -1;
					}

					// 通常ステージのBGMを再生
					if (kStageHandle != -1) {
						stagePlayHandle =
							Novice::PlayAudio(kStageHandle, true, 1.0f);
					}

					scene = Game;
					isTransitioningFromBonus = false;
					gameFadeTimer = 0;

					player.worldLeftTop.posX =
						savedPlayerX - 200.0f;

					if (player.worldLeftTop.posX < 0.0f) {
						player.worldLeftTop.posX = 0.0f;
					}

					player.worldLeftTop.posY = kGroundY;
					playerVelocityY = 0.0f;

					isJumping = false;
					jumpCount = 0;
				}
			}

			break;
		}

		//==========================================
		// クリア
		//==========================================
		case Clear: {
			clearTimer++;
			if (clearTexture != -1) {
				Novice::DrawSprite(0, 0, clearTexture, 1.0f, 1.0f, 0.0f, WHITE);
			}
			else {
				Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x222033FF, kFillModeSolid);
				Novice::ScreenPrintf(560, 100, "STAGE CLEAR!!");
			}
			if (clearTimer >= kClearShowFrames) {
				scene = Title;
				timer = 0;
				selectedMenu = 0;
				isRuleOpen = false;
				isTitleFading = false;
				titleFadeTimer = 0;
				title1Y = -180.0f;
				title2Y = -250.0f;
				title1Velocity = 0.0f;
				title2Velocity = 0.0f;
				penguinX = -200.0f;
				penguinY = 380.0f;
				player.worldLeftTop = { 10.0f, 500.0f };
				playerVelocityY = 0.0f;
				isJumping = false;
				jumpCount = 0;
				jumpFrame = 0;
				jumpAnimationTimer = 0;
				scrollValue = 0.0f;
				gameFadeTimer = 0;
				isGameClear = false;
				goalTimer = 0;
				goalFlagFrame = 0;
				coinCount = 0;
				memcpy(coinMap, initialCoinMap, sizeof(coinMap));
				memset(coinCollectedByStage, 0, sizeof(coinCollectedByStage));
				memset(bonusItemCollected, 0, sizeof(bonusItemCollected));
				isTransitioningToBonus = false;
				isTransitioningFromBonus = false;
				bonusTransitionTimer = 0;
				bonusExitTransitionTimer = 0;
				batteryFrame = 0;
				batteryTimer = 0;
				if (firePlayHandle != -1) { Novice::StopAudio(firePlayHandle); firePlayHandle = -1; }
				if (bonusPlayHandle != -1) { Novice::StopAudio(bonusPlayHandle); bonusPlayHandle = -1; }
				if (stagePlayHandle != -1) { Novice::StopAudio(stagePlayHandle); stagePlayHandle = -1; }
				if (kTitleHandle != -1) titlePlayHandle = Novice::PlayAudio(kTitleHandle, true, 0.5f);
			}
			break;
		}


		//==========================================
		// チュートリアル
		//==========================================

		case tutorial: {
			//==========================================
			// 現在のステージセットを更新
			//==========================================
			stageSet = static_cast<int>(player.worldLeftTop.posX / kStageWidth);
			//==========================================
			// 更新処理
			// =========================================
			
			// 自動前進
			player.worldLeftTop.posX += kMoveSpeed;

			if (keys[DIK_A]) {
				player.worldLeftTop.posX -= 30.0f;
			}

			// ジャンプ
			if (preKeys[DIK_SPACE] == 0 &&
				keys[DIK_SPACE] != 0 &&
				jumpCount < kMaxJumpCount) {

				playerVelocityY = -kJumpPower;
				isJumping = true;
				jumpCount++;

				if (jumpSound != -1) {
					Novice::PlayAudio(
						jumpSound, false, 0.8f
					);
				}
			}

			// ジャンプアニメーション
			if (isJumping) {

				jumpAnimationTimer++;

				if (jumpAnimationTimer >= 10) {

					jumpAnimationTimer = 0;
					jumpFrame =
						(jumpFrame + 1) % kJumpFrameCount;
				}
			}

			float previousPlayerBottom =
				player.worldLeftTop.posY + player.size.posY;

			// 重力
			if (isJumping) {

				playerVelocityY += kGravity;
				player.worldLeftTop.posY += playerVelocityY;
			}

			//==========================================
			// トランポリン判定
			//==========================================
			int collisionStageSet =
				static_cast<int>(
					player.worldLeftTop.posX / kStageWidth
					);

			for (int set = collisionStageSet - 1;
				set <= collisionStageSet + 1; set++) {

				for (int i = 0; i < kTrampolineCount; i++) {

					float trampolineX =
						trampolinePositions[i].posX +
						set * kStageWidth;

					float trampolineY =
						trampolinePositions[i].posY;

					float playerRight =
						player.worldLeftTop.posX +
						player.size.posX;

					float playerBottom =
						player.worldLeftTop.posY +
						player.size.posY;

					bool isOverlapX =
						player.worldLeftTop.posX <
						trampolineX + kTrampolineWidth &&
						playerRight > trampolineX;

					bool isLanding =
						previousPlayerBottom <= trampolineY &&
						playerBottom >= trampolineY;

					if (playerVelocityY > 0.0f &&
						isOverlapX && isLanding) {

						player.worldLeftTop.posY =
							trampolineY - player.size.posY;

						playerVelocityY =
							-kTrampolineBouncePower;

						isJumping = true;

						if (trampolineSound != -1) {
							Novice::PlayAudio(
								trampolineSound, false, 0.8f
							);
						}
					}
				}
			}

			// 地面判定
			if (player.worldLeftTop.posY >= kGroundY) {

				player.worldLeftTop.posY = kGroundY;
				playerVelocityY = 0.0f;
				isJumping = false;
				jumpCount = 0;
			}

			//==========================================
			// コイン判定
			//==========================================
			int coinCollisionStageSet =
				static_cast<int>(
					player.worldLeftTop.posX / kStageWidth
					);

			for (int set = coinCollisionStageSet - 1;
				set <= collisionStageSet + 1; set++) {

				if (set < 0 || set >= kMaxCoinStageSets) {
					continue;
				}

				for (int y = 0; y < kCoinMapHeight; y++) {
					for (int x = 0; x < kCoinMapWidth; x++) {

						if (coinMap[y][x] != 1 ||
							coinCollectedByStage[set][y][x]) {
							continue;
						}

						float coinX =
							x * kCoinMapSize + set * kStageWidth;

						float coinY =
							static_cast<float>(y * kCoinMapSize);

						float playerRight =
							player.worldLeftTop.posX +
							player.size.posX;

						float playerBottom =
							player.worldLeftTop.posY +
							player.size.posY;

						if (player.worldLeftTop.posX <
							coinX + kCoinSize &&
							playerRight > coinX &&
							player.worldLeftTop.posY <
							coinY + kCoinSize &&
							playerBottom > coinY) {

							coinCollectedByStage[set][y][x] = true;
							coinCount++;

							if (coinSound != -1) {
								Novice::PlayAudio(
									coinSound, false, 0.8f
								);
							}
						}
					}
				}
			}

			//==========================================
			// スクロール
			//==========================================
			scrollValue =
				player.worldLeftTop.posX +
				player.size.posX - scrollStartPositionX;

			if (scrollValue < 0.0f) {
				scrollValue = 0.0f;
			}

			float loopScroll =
				fmodf(scrollValue, kBackgroundWidth);

			// 背景
			for (int i = 0; i < 8; i++) {

				int textureIndex = i % 4;

				float backgroundX =
					1280.0f * i - loopScroll;

				if (textureIndex >= 0 &&
					backgroundTexture != -1) {

					Novice::DrawSprite(
						static_cast<int>(backgroundX),
						0,
						backgroundTexture,
						1.0f, 1.0f, 0.0f, WHITE
					);
				}
			}

			//================================
			//チュートリアル描画処理
			//================================

			// トランポリン描画
			for (int set = stageSet - 1;
				set <= stageSet + 1; set++) {

				for (int i = 0; i < kTrampolineCount; i++) {

					float trampolineWorldX =
						trampolinePositions[i].posX +
						set * kStageWidth;

					int screenX =
						static_cast<int>(
							trampolineWorldX - scrollValue
							);

					if (screenX + kTrampolineWidth < 0 ||
						screenX > 1280) {
						continue;
					}

					if (trampoline != -1) {
						Novice::DrawSprite(
							screenX,
							static_cast<int>(
								trampolinePositions[i].posY
								),
							trampoline,
							1.0f, 1.0f, 0.0f, WHITE
						);
					}
				}
			}

			//===========================
			// 看板描画
			//===========================

			// 前後のステージの看板も描画する
			for (int set = stageSet - 1;
				set <= stageSet + 1; set++)
			{
				for (int i = 0; i < kSignboardCount; i++)
				{
					// 看板のワールド座標
					float signboardWorldX =
						signboardPositions[i].posX
						+ set * kStageWidth;

					float signboardWorldY =
						signboardPositions[i].posY;

					// ワールド座標から画面座標へ変換
					int screenX = static_cast<int>(
						signboardWorldX - scrollValue
						);

					int screenY = static_cast<int>(
						signboardWorldY
						);

					// 画面外なら描画しない
					if (screenX + kSignboardWidth < 0 ||
						screenX > 1280)
					{
						continue;
					}

					// 看板を描画
					if (signboard != -1)
					{
						Novice::DrawSprite(
							screenX,
							screenY,
							signboard,
							1.0f,
							1.0f,
							0.0f,
							WHITE
						);
					}
				}
			}

			// コイン描画
			for (int set = stageSet - 1;
				set <= stageSet + 1; set++) {

				if (set < 0 || set >= kMaxCoinStageSets) {
					continue;
				}

				for (int y = 0; y < kCoinMapHeight; y++) {
					for (int x = 0; x < kCoinMapWidth; x++) {

						if (coinMap[y][x] != 1 ||
							coinCollectedByStage[set][y][x]) {
							continue;
						}

						float coinX =
							x * kCoinMapSize +
							set * kStageWidth;

						int screenX =
							static_cast<int>(coinX - scrollValue);

						if (screenX + kCoinSize < 0 ||
							screenX > 1280) {
							continue;
						}

						if (coin != -1) {
							Novice::DrawSprite(
								screenX,
								y * kCoinMapSize,
								coin,
								1.0f, 1.0f, 0.0f, WHITE
							);
						}
					}
				}
			}

			// プレイヤー
			if (isJumping && playerJumpTexture != -1) {

				Novice::DrawSpriteRect(
					static_cast<int>(
						player.worldLeftTop.posX - scrollValue
						),
					static_cast<int>(player.worldLeftTop.posY),
					jumpFrame * 128,
					0,
					128, 128,
					playerJumpTexture,
					0.25f, 1.0f,
					0.0f, WHITE
				);

			}
			else if (playerTexture != -1) {

				Novice::DrawSprite(
					static_cast<int>(
						player.worldLeftTop.posX - scrollValue
						),
					static_cast<int>(player.worldLeftTop.posY),
					playerTexture,
					1.0f, 1.0f, 0.0f, WHITE
				);
			}

			Novice::ScreenPrintf(
				20, 20, "COIN : %d", coinCount
			);

			break;
		}
		}

		//==========================================
		// シーン切り替えフェードイン
		//==========================================
		if (scene != Title &&
			gameFadeTimer < 60 &&
			!isTransitioningToBonus &&
			scene != BonusStage &&
			scene != Clear) {

			int alpha =
				255 - gameFadeTimer * 255 / 60;

			Novice::DrawBox(
				0, 0, 1280, 720,
				0.0f,
				static_cast<unsigned int>(alpha),
				kFillModeSolid
			);

			gameFadeTimer++;
		}

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 &&
			keys[DIK_ESCAPE] != 0 &&
			scene != Title) {
			break;
		}
	}
	Novice::Finalize();
	return 0;
}