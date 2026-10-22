#include <Novice.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

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

// コイン取得時のパーティクル
struct Particle
{
	Vector2 position;
	Vector2 velocity;
	int timer;
	int lifeTime;
	float radius;
	bool isActive;
	unsigned int color = 0xFFFF00FF; // 初期色：黄色
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{

	Novice::Initialize(kWindowTitle, 1280, 720);

	// 乱数の初期化
	srand(static_cast<unsigned int>(time(NULL)));

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//==========================================
	// プレイヤー
	//==========================================
	Box player{};
	player.worldLeftTop = { 10.0f, 500.0f };
	player.size = { 80.0f, 80.0f };
	const float kPlayerDrawSize = 128.0f;
	const float kPlayerOffsetX = (kPlayerDrawSize - player.size.posX) * 0.5f;
	const float kPlayerOffsetY = (kPlayerDrawSize - player.size.posY) * 0.5f;

	const float kMoveSpeed = 10.0f;
	const float kStageWidth = 1280.0f * 4.0f;
	const float kBackgroundWidth = 1280.0f * 4.0f;

	float savedPlayerX = 0.0f;

	// ライフと無敵時間（エネミー当たり判定用）
	int playerLife = 3;
	bool isInvincible = false;
	int invincibleTimer = 0;
	const int kInvincibleDuration = 120; // ダメージ後約2秒間の無敵時間

	//==========================================
	// ステージセット変数
	//==========================================
	int stageSet = static_cast<int>(player.worldLeftTop.posX / kStageWidth);
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

	bool isSpeedBoostActive = false;
	bool hasSpeedBoostTriggered = false;
	int speedBoostTimer = 0;

	// ボーナスステージ用 マップチップ設定 (充電・ダメージアイテム用)
	const int kBonusMapHeight = 10;
	const int kBonusMapWidth = 100;
	const int kBonusTileSize = 128; // マップチップ間隔
	const float kBonusItemHitSize = 80.0f;
	int bonusMap[kBonusMapHeight][kBonusMapWidth] = {};

	// 背景ループごとの獲得フラグ (各ループ・各マス用)
	const int kMaxBonusLoops = 10;
	bool bonusItemCollected[kMaxBonusLoops][kBonusMapHeight][kBonusMapWidth] = {};

	//==========================================
	// チュートリアル用ガイド変数
	//==========================================
	bool isTutorialPaused = false;
	bool isStartGuideDone = false;       // 開始時の空中ジャンプガイド完了フラグ
	bool isTrampolineGuideDone = false;  // トランポリン手前ガイド完了フラグ
	int tutorialPauseType = 0;           // 0: 開始時空中ポーズ, 1: トランポリン前ポーズ
	int tutorialStep = 1;                // 開始時ポーズの段数(1~3)
	bool isWaitingForPeak = false;       // ジャンプ頂点到達待ちフラグ

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
	int enemyTexture =
		Novice::LoadTexture("./Resources/images/enemy.png");
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
	// 宇宙ペンギン3 (コイン300枚以上・スピードアップ時)
	int universepengin3Texture =
		Novice::LoadTexture("./Resources/images/speed.png");
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
	int menuTextures[4] = {
		Novice::LoadTexture("./Resources/images/space.png"),
		Novice::LoadTexture("./Resources/images/tutorial.png"),
		Novice::LoadTexture("./Resources/images/credit.png"),
		Novice::LoadTexture("./Resources/images/endGame.png")
	};
	// ルール画面
	int ruleTexture =
		Novice::LoadTexture("./Resources/images/ruleFormat.png");
	// 選択矢印
	int selectArrowTexture =
		Novice::LoadTexture("./Resources/images/underCommand.png");
	int pauseTexture = Novice::LoadTexture("./Resources/images/pushEscape.png");
	int leftCommandTexture = Novice::LoadTexture("./Resources/images/Leftcommand.png");
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
	//星
	int starTexture =
		Novice::LoadTexture("./Resources/images/star.png");
	int noStarTexture =
		Novice::LoadTexture("./Resources/images/noStar.png");
	//星をすべて取った時の文字
	int perfectTexture =
		Novice::LoadTexture("./Resources/images/perfect.png");
	// クリア画面の条件文字・数字画像
	int clearLabelTextures[4] = {
		Novice::LoadTexture("./Resources/images/goal.png"),
		Novice::LoadTexture("./Resources/images/goBonus.png"),
		Novice::LoadTexture("./Resources/images/formatCoin.png"),
		Novice::LoadTexture("./Resources/images/noDamage.png")
	};
	int countTextures[10] = {};
	for (int i = 0; i < 10; i++) {
		char path[128];
		sprintf_s(path, sizeof(path), "./Resources/images/count/count%d.png", i);
		countTextures[i] = Novice::LoadTexture(path);
	}
	int slashTexture = Novice::LoadTexture("./Resources/images/slash.png");
	//ライフの画像
	int playerLifeTexture =
		Novice::LoadTexture("./Resources/images/life.png");
	//チュートリアル説明用画像
	int tutorialTextTexture =
		Novice::LoadTexture("./Resources/images/tutorialText.png");
	int tutorialTextTexture2 =
		Novice::LoadTexture("./Resources/images/tutorialText2.png");
	int tutorialTextTexture3 =
		Novice::LoadTexture("./Resources/images/tutorialText3.png");
	int tutorialTextTexture4 =
		Novice::LoadTexture("./Resources/images/tutorialText4.png");

	int secretHandle[3] = {};
	secretHandle[0] = Novice::LoadTexture("./Resources/images/wireFish1.png");
	secretHandle[1] = Novice::LoadTexture("./Resources/images/wireFish2.png");
	secretHandle[2] = Novice::LoadTexture("./Resources/images/wireFish3.png");
	bool isTimerSecret = false;
	float secretX = 0.0f;
	int secretAnimationTimer = 0;
	int secretCurrentFrame = 0;
	//==========================================
	// 敵（シャチ）の設定
	//==========================================
	struct Enemy
	{
		Vector2 position;
		Vector2 size;
		float speed;
		bool isActive;
		bool hasTrampoline; // トランポリンを乗せているかのフラグ
	};

	Enemy enemy{};
	enemy.size = { 256.0f, 128.0f }; // サイズ256x128
	enemy.speed = 8.0f; // 移動速度
	enemy.position = { 1280.0f + 200.0f, 420.0f };
	enemy.isActive = true;
	enemy.hasTrampoline = (rand() % 2 == 0); // 50%の確率でトランポリン付き

	//==========================================
	// 効果音
	//==========================================
	int fireSound =
		Novice::LoadAudio("./Resources/sound/fire.MP3");

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
	const int kTrampolineWidth = 100;
	const float kTrampolineOffsetY = 60.0f;
	//==========================================
	// 看板配置
	//==========================================
	Vector2 signboardPositions[] =
	{
		{ 2900.0f, 500.0f },
		{ 4900.0f, 500.0f },
	};
	//==========================================
	// コイン
	//==========================================
	const int kCoinMapHeight = 6;
	const int kCoinMapWidth = 100;
	const int kCoinMapSize = 64;
	const float kCoinSize = 48.0f;

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
	// コインパーティクル
	//==========================================
	const int kMaxParticles = 40;
	Particle particles[kMaxParticles]{};

	const int kParticleLifeTime = 40;

	// コイン取得時の光の輪
	bool isCoinRingActive = false;
	float coinRingX = 0.0f;
	float coinRingY = 0.0f;
	float coinRingRadius = 0.0f;
	int coinRingTimer = 0;

	// 光の輪の設定
	const int kCoinRingLifeTime = 12;
	const float kCoinRingMaxRadius = 60.0f;
	//==========================================
	// ボーナスステージ専用コインマップ
	//==========================================
	const int kBonusCoinMapHeight = 11;
	const int kBonusCoinMapWidth = 100;
	const int kBonusCoinMapSize = 64;
	const int kMaxCoinStageSets = 100;
	// ★コインの当たり判定サイズ (64x64に対して48x48に調整)
	const float kBonusCoinHitSize = 48.0f;

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
	//星の管理変数
	bool hasVisitedBonus = false;
	bool hasTakenDamage = false;

	bool starConditions[4] = {};
	int starCount = 0;

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
	bool isPaused = false;
	bool waitSpaceRelease = false;
	bool isPauseRuleOpen = false;
	int pauseSelectedMenu = 0;

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

	bool shouldExit = false;

	// 右上のコイン枚数を count0.png ～ count9.png で描画
	auto DrawCoinCount = [&]() {
		char coinDigits[32];
		sprintf_s(coinDigits, sizeof(coinDigits), "%d", coinCount);
		const int digitSpacing = 24;
		const int digitY = 25;
		for (int i = 0; coinDigits[i] != '\0'; i++) {
			int digit = coinDigits[i] - '0';
			if (digit >= 0 && digit <= 9 && countTextures[digit] != -1) {
				Novice::DrawSprite(1130 + i * digitSpacing, digitY,
					countTextures[digit], 0.10f, 0.10f, 0.0f, WHITE);
			}
		}
		};

	//==========================================
	// メインループ
	//==========================================
	while (Novice::ProcessMessage() == 0 && !shouldExit) {

		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// ESCでポーズ。上下で選択、SPACEで決定
		if (scene == Game || scene == BonusStage || scene == tutorial) {
			if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
				if (isPauseRuleOpen) isPauseRuleOpen = false;
				else { isPaused = !isPaused; pauseSelectedMenu = 0; }
			}
			if (isPaused && !isPauseRuleOpen) {
				if ((preKeys[DIK_UP] == 0 && keys[DIK_UP] != 0) ||
					(preKeys[DIK_W] == 0 && keys[DIK_W] != 0)) {
					pauseSelectedMenu = (pauseSelectedMenu + 2) % 3;
					if (cursorSound != -1) Novice::PlayAudio(cursorSound, false, 0.8f);
				}
				if ((preKeys[DIK_DOWN] == 0 && keys[DIK_DOWN] != 0) ||
					(preKeys[DIK_S] == 0 && keys[DIK_S] != 0)) {
					pauseSelectedMenu = (pauseSelectedMenu + 1) % 3;
					if (cursorSound != -1) Novice::PlayAudio(cursorSound, false, 0.8f);
				}
				if (preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE] != 0) {
					if (pushSound != -1) Novice::PlayAudio(pushSound, false, 0.8f);
					if (pauseSelectedMenu == 0) { isPaused = false; waitSpaceRelease = true; }
					else if (pauseSelectedMenu == 1) isPauseRuleOpen = true;
					else {
						isPaused = false;
						isPauseRuleOpen = false;
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
						hasVisitedBonus = false;
						hasTakenDamage = false;
						starCount = 0;
						playerLife = 3;
						isInvincible = false;
						invincibleTimer = 0;
						isTutorialPaused = false;
						isStartGuideDone = false;
						isTrampolineGuideDone = false;

						// タイトル戻り時にスピードアップ用のフラグをリセット
						isSpeedBoostActive = false;
						hasSpeedBoostTriggered = false;
						speedBoostTimer = 0;

						memset(starConditions, 0, sizeof(starConditions));
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
				}
			}
		}

		if (waitSpaceRelease && keys[DIK_SPACE] == 0) {
			waitSpaceRelease = false;
		}

		if (!isPaused) {
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

				// --- シークレット画像の出現・移動処理を追加 ---
				if (timer > 1800)
				{
					isTimerSecret = true;
					secretX += 7.0f;
				}

				// 画面右端に消えたらリセットする場合（任意）
				if (secretX > 1280.0f) {
					isTimerSecret = false;
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

							selectedMenu = (selectedMenu + 3) % 4;

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

							selectedMenu = (selectedMenu + 1) % 4;

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

						if (selectedMenu == 3) {
							shouldExit = true;
						}
						else if (selectedMenu == 0) {
							scene = Game;

							// ライフと無敵時間を初期化
							playerLife = 3;
							isInvincible = false;
							invincibleTimer = 0;

							// 通常ゲーム開始時の位置セット
							player.worldLeftTop = { 10.0f, 500.0f };
							enemy.position = { 1280.0f + 200.0f, 420.0f };
							enemy.isActive = true;
							enemy.hasTrampoline = (rand() % 2 == 0); // スタート時のシャチの種類をランダム決定

							// ステージBGMを再生
							if (kStageHandle != -1) {
								stagePlayHandle =
									Novice::PlayAudio(kStageHandle, true, 1.0f);
							}
						}
						else if (selectedMenu == 1) {

							scene = tutorial;

							// チュートリアル用に初期化（最初は地面で停止してSPACE押し待ち）
							player.worldLeftTop = { 10.0f, kGroundY + kPlayerOffsetY };
							playerVelocityY = 0.0f;
							isJumping = false;
							jumpCount = 0;

							enemy.position = { 1000.0f, 420.0f };
							enemy.isActive = false;
							enemy.hasTrampoline = true; // チュートリアルはトランポリン付きで固定

							// 最初は画面停止（ポーズ）でプレイヤーのSPACE開始入力を待つ
							isTutorialPaused = true;
							isStartGuideDone = false;
							isTrampolineGuideDone = false;
							tutorialPauseType = 0; // 0: 開始時ガイド
							tutorialStep = 1;
							isWaitingForPeak = false;

							// タイトル画面決定時のSPACE押しっぱなし判定を防ぐためキー入力を上書き
							preKeys[DIK_SPACE] = 1;

							if (kTutorialHandle != -1) {
								Novice::PlayAudio(
									kTutorialHandle, true, 0.5f
								);
							}
						}

						if (!shouldExit) {
							gameFadeTimer = 0;
							isTitleFading = false;
						}
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

					const int menuX[4] = { 100, 390, 680, 970 };
					const int menuY = 485;

					for (int i = 0; i < 4; i++) {

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
							0.8f, 0.8f, 0.0f, WHITE
						);
					}

					if (timer > 365 && selectArrowTexture != -1) {
						Novice::DrawSprite(
							menuX[selectedMenu] + 87,
							menuY - 38,
							selectArrowTexture,
							1.0f, 1.0f, 0.0f, WHITE
						);
					}

				}

				//シークレット
				if (isTimerSecret && secretHandle[secretCurrentFrame] != -1)
				{
					Novice::DrawSprite(
						static_cast<int>(secretX),
						300, // Y座標（画面の中央付近）
						secretHandle[secretCurrentFrame],
						1.0f,
						1.0f,
						0.0f,
						WHITE
					);
				}
				// アニメーション更新
				if (isTimerSecret) {
					secretAnimationTimer++;
					if (secretAnimationTimer >= 10) {
						secretAnimationTimer = 0;
						secretCurrentFrame++;
						if (secretCurrentFrame >= 3) {
							secretCurrentFrame = 0;
						}
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
				// 無敵時間の処理
				//==========================================
				if (isInvincible) {
					invincibleTimer++;
					if (invincibleTimer >= kInvincibleDuration) {
						isInvincible = false;
						invincibleTimer = 0;
					}
				}

				float previousPlayerBottom =
					player.worldLeftTop.posY + player.size.posY;

				//==========================================
				// 更新処理
				//==========================================
				if (!isTransitioningToBonus && !isGameClear) {
					player.worldLeftTop.posX += kMoveSpeed;

					if (keys[DIK_A]) {
						player.worldLeftTop.posX -= 30.0f;//デバック用
					}

					if (!waitSpaceRelease && preKeys[DIK_SPACE] == 0 &&
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

				//==========================================
				// スクロール計算（敵の更新で使うため先行計算）
				//==========================================
				scrollValue =
					player.worldLeftTop.posX +
					player.size.posX - scrollStartPositionX;

				if (scrollValue < 0.0f) {
					scrollValue = 0.0f;
				}

				//==========================================
				// シャチ（敵）の更新処理
				//==========================================
				if (enemy.isActive && !isGameClear) {
					// 右から左へ移動
					enemy.position.posX -= enemy.speed;

					// 画像の右端が完全に画面の左外側（scrollValue）を出たら、画面右外側へループ再出現
					if (enemy.position.posX + enemy.size.posX < scrollValue) {
						enemy.position.posX = scrollValue + 1280.0f + 200.0f;
						enemy.position.posY = 420.0f;
						enemy.hasTrampoline = (rand() % 2 == 0); // 50%の確率でトランポリン付きにランダム変更
					}

					//==========================================
					// エネミー（シャチ）との当たり判定
					//==========================================
					if (!isInvincible && !isTransitioningToBonus && !isGameClear) {
						float playerLeft = player.worldLeftTop.posX;
						float playerRight = player.worldLeftTop.posX + player.size.posX;
						float playerTop = player.worldLeftTop.posY;
						float playerBottom = player.worldLeftTop.posY + player.size.posY;

						float enemyLeft = enemy.position.posX;
						float enemyRight = enemy.position.posX + enemy.size.posX;
						float enemyTop = enemy.position.posY;
						float enemyBottom = enemy.position.posY + enemy.size.posY;

						// 矩形同士のオーバーラップ判定
						if (playerLeft < enemyRight &&
							playerRight > enemyLeft &&
							playerTop < enemyBottom &&
							playerBottom > enemyTop) {

							// ライフを1つ減らす
							if (playerLife > 0) {
								playerLife--;
							}

							hasTakenDamage = true; // 星の獲得条件用フラグ

							// 無敵開始
							isInvincible = true;
							invincibleTimer = 0;
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
						trampolinePositions[i].posY + kTrampolineOffsetY;

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

						// トランポリンを踏んだ場所から粒を飛ばす
						float particleCenterX =
							trampolineX + kTrampolineWidth / 2.0f;

						float particleCenterY = trampolineY;

						int spawnedCount = 0;

						for (int p = 0; p < kMaxParticles; p++)
						{
							if (!particles[p].isActive)
							{
								// 左右に広がりながら上に飛ぶ
								float angle =
									3.1415926f +
									(3.1415926f / 7.0f) * spawnedCount;

								particles[p].position = {
									particleCenterX,
									particleCenterY
								};

								particles[p].velocity = {
									cosf(angle) * 6.0f,
									sinf(angle) * 6.0f
								};

								particles[p].timer = 0;
								particles[p].lifeTime = 18;
								particles[p].radius = 6.0f;
								particles[p].color = 0xA0A0A0FF; // 灰色
								particles[p].isActive = true;

								spawnedCount++;

								if (spawnedCount >= 8)
								{
									break;
								}
							}
						}

						if (trampolineSound != -1) {
							Novice::PlayAudio(
								trampolineSound, false, 0.8f
							);
						}
					}
				}

				// ★通常ゲームでの「シャチの上のトランポリン」当たり判定 (hasTrampoline が true の時のみ判定)
				if (enemy.isActive && enemy.hasTrampoline && !isGameClear) {
					float enemyTrampolineX = enemy.position.posX;
					float enemyTrampolineY = enemy.position.posY - 60.0f + kTrampolineOffsetY;

					float playerRight = player.worldLeftTop.posX + player.size.posX;
					float playerBottom = player.worldLeftTop.posY + player.size.posY;

					bool isOverlapX = player.worldLeftTop.posX < enemyTrampolineX + kTrampolineWidth && playerRight > enemyTrampolineX;
					bool isLanding = previousPlayerBottom <= enemyTrampolineY && playerBottom >= enemyTrampolineY;

					if (playerVelocityY > 0.0f && isOverlapX && isLanding) {
						player.worldLeftTop.posY = enemyTrampolineY - player.size.posY;
						playerVelocityY = -kTrampolineBouncePower;
						isJumping = true;

						// パーティカル発生
						float particleCenterX = enemyTrampolineX + kTrampolineWidth / 2.0f;
						float particleCenterY = enemyTrampolineY;
						int spawnedCount = 0;

						for (int p = 0; p < kMaxParticles; p++) {
							if (!particles[p].isActive) {
								float angle = 3.1415926f + (3.1415926f / 7.0f) * spawnedCount;
								particles[p].position = { particleCenterX, particleCenterY };
								particles[p].velocity = { cosf(angle) * 6.0f, sinf(angle) * 6.0f };
								particles[p].timer = 0;
								particles[p].lifeTime = 18;
								particles[p].radius = 6.0f;
								particles[p].color = 0xA0A0A0FF;
								particles[p].isActive = true;

								spawnedCount++;
								if (spawnedCount >= 8) break;
							}
						}

						if (trampolineSound != -1) {
							Novice::PlayAudio(trampolineSound, false, 0.8f);
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

				// 地面判定 (足元に合わせて調整)
				if (player.worldLeftTop.posY >= kGroundY + kPlayerOffsetY) {

					player.worldLeftTop.posY = kGroundY + kPlayerOffsetY;
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

							// ★コイン(64x64)の判定を中央寄り(48x48)にずらして配置
							float coinX =
								x * kCoinMapSize + set * kStageWidth + 8.0f;

							float coinY =
								static_cast<float>(y * kCoinMapSize) + 8.0f;

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

								// コインを取った場所から黄色い粒を発生
								for (int i = 0; i < kMaxParticles; i++)
								{
									if (!particles[i].isActive)
									{
										float coinCenterX = coinX + kCoinSize / 2.0f;
										float coinCenterY = coinY + kCoinSize / 2.0f;

										float angle =
											(6.2831853f / kMaxParticles) * i;

										particles[i].position = {
											coinCenterX,
											coinCenterY
										};

										// 粒子の飛ぶ速さを上げる
										particles[i].velocity.posX = cosf(angle) * 8.0f;
										particles[i].velocity.posY = sinf(angle) * 8.0f;

										particles[i].timer = 0;
										particles[i].lifeTime = kParticleLifeTime;
										particles[i].radius = 5.0f;
										particles[i].color = 0xFFFF00FF; // コイン用は黄色に戻す
										particles[i].isActive = true;

										break;
									}
								}

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
					// ゴール時に4つの星の条件を確定
					starConditions[0] = true;
					starConditions[1] = hasVisitedBonus;
					starConditions[2] = coinCount >= 1000;
					starConditions[3] = !hasTakenDamage;
					starCount = 0;
					for (int i = 0; i < 4; i++) {
						if (starConditions[i]) starCount++;
					}
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
				float loopScroll =
					fmodf(scrollValue, kBackgroundWidth);

				//=================================
				// パーティクル更新
				//=================================
				for (int i = 0; i < kMaxParticles; i++)
				{
					if (particles[i].isActive)
					{
						particles[i].position.posX +=
							particles[i].velocity.posX;

						particles[i].position.posY +=
							particles[i].velocity.posY;

						// 少しずつ減速
						particles[i].velocity.posX *= 0.95f;
						particles[i].velocity.posY *= 0.95f;

						// 少しずつ小さくする
						particles[i].radius *= 0.96f;

						particles[i].timer++;

						if (particles[i].timer >= particles[i].lifeTime)
						{
							particles[i].isActive = false;
						}
					}
				}

				//==========================================
				//描画処理
				//==========================================

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

				// 黄色いコイン用パーティクル描画
				for (int i = 0; i < kMaxParticles; i++)
				{
					if (particles[i].isActive)
					{
						float screenX =
							particles[i].position.posX - scrollValue;
						float screenY =
							particles[i].position.posY;

						// 外側
						Novice::DrawEllipse(
							(int)screenX,
							(int)screenY,
							(int)(particles[i].radius * 2.0f),
							(int)(particles[i].radius * 2.0f),
							0,
							0xFFAA00FF,
							kFillModeSolid
						);

						// 内側
						Novice::DrawEllipse(
							(int)screenX,
							(int)screenY,
							(int)particles[i].radius,
							(int)particles[i].radius,
							0,
							particles[i].color,
							kFillModeSolid
						);
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

				// シャチ（敵）の描画
				if (enemy.isActive && enemyTexture != -1) {
					Novice::DrawSprite(
						static_cast<int>(enemy.position.posX - scrollValue),
						static_cast<int>(enemy.position.posY),
						enemyTexture,
						1.0f, 1.0f, 0.0f, WHITE
					);

					// ★hasTrampoline が true の時だけ真上にトランポリンを描画
					if (enemy.hasTrampoline && trampoline != -1) {
						Novice::DrawSprite(
							static_cast<int>(enemy.position.posX - scrollValue),
							static_cast<int>(enemy.position.posY - 60.0f),
							trampoline,
							1.0f, 1.0f, 0.0f, WHITE
						);
					}

					// ★【デバッグ用】シャチ本体の当たり判定枠を赤線で表示
					Novice::DrawBox(
						static_cast<int>(enemy.position.posX - scrollValue),
						static_cast<int>(enemy.position.posY),
						static_cast<int>(enemy.size.posX),
						static_cast<int>(enemy.size.posY),
						0.0f,
						0xFF0000FF, // 赤色 (RGBA)
						kFillModeWireFrame
					);

					// ★【デバッグ用】トランポリン判定ゾーンを赤線で表示（hasTrampolineがtrueの時のみ）
					if (enemy.hasTrampoline) {
						Novice::DrawBox(
							static_cast<int>(enemy.position.posX - scrollValue),
							static_cast<int>(enemy.position.posY - 60.0f + kTrampolineOffsetY),
							kTrampolineWidth,
							20, // 跳ね返り高さの判定幅
							0.0f,
							0xFF0000FF, // 赤色 (RGBA)
							kFillModeWireFrame
						);
					}
				}

				// ★プレイヤー描画（無敵時間中の点滅処理を考慮）
				bool shouldDrawPlayer = true;
				if (isInvincible && (invincibleTimer / 4) % 2 == 0) {
					shouldDrawPlayer = false; // 無敵時間は4フレームごとに点滅表示
				}

				if (shouldDrawPlayer) {
					if (isJumping && playerJumpTexture != -1) {

						Novice::DrawSpriteRect(
							static_cast<int>(
								player.worldLeftTop.posX - kPlayerOffsetX - scrollValue
								),
							static_cast<int>(player.worldLeftTop.posY - kPlayerOffsetY),
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
								player.worldLeftTop.posX - kPlayerOffsetX - scrollValue
								),
							static_cast<int>(player.worldLeftTop.posY - kPlayerOffsetY),
							playerTexture,
							1.0f, 1.0f, 0.0f, WHITE
						);
					}
				}

				// コイン画像と獲得枚数を右上に表示
				if (coin != -1) {
					Novice::DrawSprite(1080, 15, coin, 0.5f, 0.5f, 0.0f, WHITE);
				}
				DrawCoinCount();

				//==========================================
				// ライフ描画（残数に応じて描画）
				//==========================================
				if (playerLifeTexture != -1) {
					for (int i = 0; i < playerLife; i++) {
						Novice::DrawSprite(
							20 + i * 60, 20,  // 描画位置
							playerLifeTexture,
							0.5f, 0.5f,       // 画像の大きさ
							0.0f, WHITE
						);
					}
				}

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
						hasVisitedBonus = true;
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

				// 自動前進（コイン300枚以上で4秒間のみ超高速化）
				float currentBonusSpeed = kMoveSpeed;

				if (coinCount >= 300 && !hasSpeedBoostTriggered && !isSpeedBoostActive) {
					isSpeedBoostActive = true;
					hasSpeedBoostTriggered = true; // 1回限りに固定
					speedBoostTimer = 0;
				}

				if (isSpeedBoostActive) {
					currentBonusSpeed = kMoveSpeed * 2.5f; // スピード調整
					speedBoostTimer++;
					if (speedBoostTimer >= 240) { // 240フレーム（約4秒）経ったら終了
						isSpeedBoostActive = false;
					}
				}

				player.worldLeftTop.posX += currentBonusSpeed * 0.5f;

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

							// ★コイン(64x64)の判定を中心寄り(48x48)に配置
							float coinX =
								x * kBonusCoinMapSize
								+ loop * kBonusCoinLoopWidth + 8.0f;

							float coinY =
								static_cast<float>(y * kBonusCoinMapSize) + 8.0f;

							// プレイヤーの座標
							float playerLeft = player.worldLeftTop.posX;
							float playerRight =
								player.worldLeftTop.posX + player.size.posX;
							float playerTop = player.worldLeftTop.posY;
							float playerBottom =
								player.worldLeftTop.posY + player.size.posY;

							float hitSize = kBonusCoinHitSize;

							// コイン矩形との当たり判定
							float hitLeft = coinX;
							float hitTop = coinY;

							if (playerLeft < hitLeft + hitSize &&
								playerRight > hitLeft &&
								playerTop < hitTop + hitSize &&
								playerBottom > hitTop) {

								// このループのコインだけ取得済みにする
								bonusCoinCollected[loop][y][x] = true;

								coinCount++;
								// コインを取った場所から黄色い粒を発生
								for (int i = 0; i < kMaxParticles; i++)
								{
									if (!particles[i].isActive)
									{
										float coinCenterX = coinX + kCoinSize / 2.0f;
										float coinCenterY = coinY + kCoinSize / 2.0f;

										float angle =
											(6.2831853f / kMaxParticles) * i;

										particles[i].position = {
											coinCenterX,
											coinCenterY
										};

										particles[i].velocity = {
											cosf(angle) * 5.0f,
											sinf(angle) * 5.0f
										};

										particles[i].timer = 0;
										particles[i].lifeTime = kParticleLifeTime;
										particles[i].radius = 5.0f;
										particles[i].color = 0xFFFF00FF; // コイン用は黄色に戻す
										particles[i].isActive = true;

										break;
									}
								}
								// 光の輪を発生させる
								isCoinRingActive = true;

								coinRingX = coinX + kCoinSize / 2.0f;
								coinRingY = coinY + kCoinSize / 2.0f;

								coinRingRadius = 5.0f;
								coinRingTimer = 0;

								if (coinSound != -1) {
									Novice::PlayAudio(
										coinSound, false, 0.1f
									);
								}
							}
						}
					}
				}

				//=================================
				// パーティクル更新
				//=================================
				for (int i = 0; i < kMaxParticles; i++)
				{
					if (particles[i].isActive)
					{
						particles[i].position.posX +=
							particles[i].velocity.posX;

						particles[i].position.posY +=
							particles[i].velocity.posY;

						// 少しずつ減速
						particles[i].velocity.posX *= 0.95f;
						particles[i].velocity.posY *= 0.95f;

						// 少しずつ小さくする
						particles[i].radius *= 0.96f;

						particles[i].timer++;

						if (particles[i].timer >= particles[i].lifeTime)
						{
							particles[i].isActive = false;
						}
					}
				}
				// 光の輪の更新
				if (isCoinRingActive)
				{
					coinRingTimer++;

					// 0.0f から 1.0f まで変化する
					float progress =
						(float)coinRingTimer / kCoinRingLifeTime;

					// 時間とともに輪を大きくする
					coinRingRadius = kCoinRingMaxRadius * progress;

					// 12フレーム経過したら消す
					if (coinRingTimer >= kCoinRingLifeTime)
					{
						isCoinRingActive = false;
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

						// ★電池(128x128)の判定を中央寄り(80x80)に計算
						float baseItemX = static_cast<float>(x * kBonusTileSize) + 24.0f;
						float itemY = static_cast<float>(y * kBonusTileSize) + 24.0f;

						for (int i = 0; i < kMaxBonusLoops; i++) {

							// 既に獲得済みなら判定を行わない
							if (bonusItemCollected[i][y][x]) {
								continue;
							}

							float itemX = baseItemX + i * kLoopWidth;

							if (player.worldLeftTop.posX < itemX + kBonusItemHitSize &&
								player.worldLeftTop.posX + player.size.posX > itemX &&
								player.worldLeftTop.posY < itemY + kBonusItemHitSize &&
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
									hasTakenDamage = true;
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
				// 宇宙ペンギン描画 (見た目オフセットを補正して描画)
				//==========================================
				int drawUniversePenginTex = universepengin;

				if (isSpeedBoostActive) {
					drawUniversePenginTex = universepengin3Texture;
				}
				else if (isOutOfFuel) {
					drawUniversePenginTex = universepengin2Texture;
				}

				if (drawUniversePenginTex != -1) {

					Novice::DrawSpriteRect(
						static_cast<int>(
							player.worldLeftTop.posX - kPlayerOffsetX - scrollValue
							),
						static_cast<int>(player.worldLeftTop.posY - kPlayerOffsetY),
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

				// 黄色いコイン用パーティクル描画
				for (int i = 0; i < kMaxParticles; i++)
				{
					if (particles[i].isActive)
					{
						float screenX =
							particles[i].position.posX - scrollValue;
						float screenY =
							particles[i].position.posY;

						// 外側
						Novice::DrawEllipse(
							(int)screenX,
							(int)screenY,
							(int)(particles[i].radius * 2.0f),
							(int)(particles[i].radius * 2.0f),
							0,
							0xFFAA00FF,
							kFillModeSolid
						);

						// 内側
						Novice::DrawEllipse(
							(int)screenX,
							(int)screenY,
							(int)particles[i].radius,
							(int)particles[i].radius,
							0,
							particles[i].color,
							kFillModeSolid
						);
					}
				}
				// コイン取得時の大きな光の輪
				if (isCoinRingActive)
				{
					float progress =
						(float)coinRingTimer / kCoinRingLifeTime;

					int alpha = (int)(255.0f * (1.0f - progress));

					unsigned int ringColor =
						0xFFFF80FF & 0xFFFFFF00;

					ringColor |= (unsigned int)alpha;

					Novice::DrawEllipse(
						(int)(coinRingX - scrollValue),
						(int)coinRingY,
						(int)coinRingRadius,
						(int)coinRingRadius,
						0.0f,
						ringColor,
						kFillModeWireFrame
					);
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

						player.worldLeftTop.posY = kGroundY + kPlayerOffsetY;
						playerVelocityY = 0.0f;

						isJumping = false;
						jumpCount = 0;
					}
				}

				// コイン画像と獲得枚数を右上に表示
				if (coin != -1) {
					Novice::DrawSprite(1080, 15, coin, 0.5f, 0.5f, 0.0f, WHITE);
				}
				DrawCoinCount();
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
				// クリア画面にも獲得コインを表示
				if (coin != -1) {
					Novice::DrawSprite(1080, 15, coin, 0.5f, 0.5f, 0.0f, WHITE);
				}
				DrawCoinCount();
				// 星が20フレームごとに1個ずつ飛び出す演出
				const int kStarInterval = 20;
				const int kStarSize = 180; // star.png / noStar.png の画像サイズ
				const float kStarFinalScale = 0.9f;
				for (int i = 0; i < 4; i++) {
					int starElapsed = clearTimer - (i + 1) * kStarInterval;
					if (starElapsed < 0) continue;

					int texture = starConditions[i] ? starTexture : noStarTexture;
					if (texture == -1) continue;

					// 0～7フレーム: 大きく登場 / 8～15フレーム: 元の大きさに戻る
					float scale = kStarFinalScale;
					if (starElapsed < 8) {
						scale = kStarFinalScale * (0.3f + 1.0f * starElapsed / 8.0f);
					}
					else if (starElapsed < 16) {
						scale = kStarFinalScale * (1.3f - 0.3f * (starElapsed - 8) / 8.0f);
					}

					// 拡大・縮小しても星の中心位置が動かないようにする
					int centerX = 325 + i * 210;
					int centerY = 480;
					int drawX = centerX - static_cast<int>(kStarSize * scale * 0.5f);
					int drawY = centerY - static_cast<int>(kStarSize * scale * 0.5f);
					Novice::DrawSprite(drawX, drawY, texture, scale, scale, 0.0f, WHITE);

					// 星の下に条件画像を表示
					if (clearLabelTextures[i] != -1) {
						Novice::DrawSprite(centerX - 83, 565, clearLabelTextures[i],
							0.65f, 0.65f, 0.0f, WHITE);
					}
				}
				// 獲得星数を数字画像で表示
				if (clearTimer >= 4 * kStarInterval + 16) {
					const float digitScale = 0.38f;
					if (countTextures[starCount] != -1)
						Novice::DrawSprite(559, 650, countTextures[starCount], digitScale, digitScale, 0.0f, WHITE);
					if (slashTexture != -1)
						Novice::DrawSprite(609, 650, slashTexture, digitScale, digitScale, 0.0f, WHITE);
					if (countTextures[4] != -1)
						Novice::DrawSprite(690, 650, countTextures[4], digitScale, digitScale, 0.0f, WHITE);
				}
				// 4つの星をすべて獲得したときだけPERFECTを表示
				// 4つ目の星の登場アニメーションが終わってから表示
				if (starCount == 4 && perfectTexture != -1 && clearTimer >= 110) {
					int perfectElapsed = clearTimer - 110;
					float perfectScale = 1.0f;
					if (perfectElapsed < 10) {
						perfectScale = 0.2f + 1.1f * perfectElapsed / 10.0f;
					}
					else if (perfectElapsed < 20) {
						perfectScale = 1.3f - 0.3f * (perfectElapsed - 10) / 10.0f;
					}
					// perfect.png は400x200。上部中央に表示
					const int perfectWidth = 400;
					const int perfectHeight = 200;
					int perfectX = 640 - static_cast<int>(perfectWidth * perfectScale / 2.0f);
					int perfectY = 300 - static_cast<int>(perfectHeight * perfectScale / 2.0f);
					Novice::DrawSprite(perfectX, perfectY, perfectTexture,
						perfectScale, perfectScale, 0.0f, WHITE);
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
					hasVisitedBonus = false;
					hasTakenDamage = false;
					starCount = 0;

					// タイトル戻り時にスピードアップ用のフラグをリセット
					isSpeedBoostActive = false;
					hasSpeedBoostTriggered = false;
					speedBoostTimer = 0;

					memset(starConditions, 0, sizeof(starConditions));
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
				// チュートリアル用の弱めのトランポリン跳ね返り力
				const float kTutorialBouncePower = 25.0f;

				//==========================================
				// 現在のステージセットを更新
				//==========================================
				stageSet = static_cast<int>(player.worldLeftTop.posX / kStageWidth);

				// ジャンプ中、最高点付近（Y速度が-2.0f以上）になったら一時停止を発動
				if (isWaitingForPeak && !isStartGuideDone) {
					if (playerVelocityY >= -2.0f) {
						isTutorialPaused = true;
						tutorialPauseType = 0;
						isWaitingForPeak = false;
					}
				}

				// トランポリン接近時のチェック（まだガイドを行っていない場合）
				if (!isTrampolineGuideDone && !isTutorialPaused && isStartGuideDone) {
					float firstTrampolineX = trampolinePositions[0].posX;
					// 最初のトランポリン手前300pxまで接近したら一時停止
					if (player.worldLeftTop.posX >= firstTrampolineX - 300.0f) {
						isTutorialPaused = true;
						tutorialPauseType = 1; // 1: トランポリン前ポーズ
					}
				}

				// ガイドポーズ中のキー操作（SPACEで解除して次のジャンプ）
				if (isTutorialPaused) {
					if (preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE] != 0) {
						if (tutorialPauseType == 0) {
							// 【開始時空中ジャンプガイド】
							if (tutorialStep <= 3) {
								playerVelocityY = -kJumpPower;
								isJumping = true;
								jumpCount = tutorialStep;
								isTutorialPaused = false; // 一旦動かして最高点まで飛ばす
								isWaitingForPeak = true;

								if (jumpSound != -1) {
									Novice::PlayAudio(jumpSound, false, 0.8f);
								}

								if (tutorialStep == 3) {
									// 3回目のジャンプを出したらガイド全工程完了
									isStartGuideDone = true;
									isWaitingForPeak = false;
								}

								tutorialStep++;
							}
						}
						else if (tutorialPauseType == 1) {
							// 【トランポリン前ガイド】
							isTutorialPaused = false;
							isTrampolineGuideDone = true;

							// そのままジャンプ開始
							playerVelocityY = -kJumpPower;
							isJumping = true;
							jumpCount = 1;

							if (jumpSound != -1) {
								Novice::PlayAudio(jumpSound, false, 0.8f);
							}
						}
					}
				}

				//==========================================
				// 更新処理（非ポーズ時のみ実行）
				// =========================================
				if (!isTutorialPaused) {

					// 自動前進
					player.worldLeftTop.posX += kMoveSpeed;

					if (keys[DIK_A]) {
						player.worldLeftTop.posX -= 30.0f;
					}

					//==========================================
					// スクロール計算（敵の更新で使うため先行計算）
					//==========================================
					scrollValue =
						player.worldLeftTop.posX +
						player.size.posX - scrollStartPositionX;

					if (scrollValue < 0.0f) {
						scrollValue = 0.0f;
					}

					// シャチ（敵）の更新処理
					if (enemy.isActive) {
						enemy.position.posX -= enemy.speed;

						// 画像の右端が完全に画面の左外側（scrollValue）を出たら再出現
						if (enemy.position.posX + enemy.size.posX < scrollValue) {
							enemy.position.posX = scrollValue + 1280.0f + 200.0f;
							enemy.position.posY = 420.0f;
							enemy.hasTrampoline = true;
						}
					}

					// ジャンプ（開始ガイド後から1つ目のトランポリン前までは自由ジャンプを禁止）
					if (preKeys[DIK_SPACE] == 0 &&
						keys[DIK_SPACE] != 0 &&
						jumpCount < kMaxJumpCount &&
						!isWaitingForPeak &&
						(!isStartGuideDone || isTrampolineGuideDone)) {

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
					// トランポリン判定（チュートリアル用調整）
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
								trampolinePositions[i].posY + kTrampolineOffsetY;

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
									-kTutorialBouncePower; // 弱めの跳ね返り力を適用

								// トランポリンを踏んだ場所から粒を飛ばす
								float particleCenterX =
									trampolineX + kTrampolineWidth / 2.0f;

								float particleCenterY = trampolineY;

								int spawnedCount = 0;

								for (int p = 0; p < kMaxParticles; p++)
								{
									if (!particles[p].isActive)
									{
										// 左右に広がりながら上に飛ぶ
										float angle =
											3.1415926f +
											(3.1415926f / 7.0f) * spawnedCount;

										particles[p].position = {
											particleCenterX,
											particleCenterY
										};

										particles[p].velocity = {
											cosf(angle) * 6.0f,
											sinf(angle) * 6.0f
										};

										particles[p].timer = 0;
										particles[p].lifeTime = 18;
										particles[p].radius = 6.0f;
										particles[p].color = 0xA0A0A0FF; // 灰色
										particles[p].isActive = true;

										spawnedCount++;

										if (spawnedCount >= 8)
										{
											break;
										}
									}
								}

								isJumping = true;
								jumpCount = 0; // トランポリンで空中ジャンプ回数をリセット

								if (trampolineSound != -1) {
									Novice::PlayAudio(
										trampolineSound, false, 0.8f
									);
								}
							}
						}
					}

					// 地面判定 (足元に合わせて調整)
					if (player.worldLeftTop.posY >= kGroundY + kPlayerOffsetY) {

						player.worldLeftTop.posY = kGroundY + kPlayerOffsetY;
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

								// ★コイン(64x64)の判定を中央寄り(48x48)にずらして配置
								float coinX =
									x * kCoinMapSize + set * kStageWidth + 8.0f;

								float coinY =
									static_cast<float>(y * kCoinMapSize) + 8.0f;

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

									// コインを取った場所から黄色い粒を発生
									for (int i = 0; i < kMaxParticles; i++)
									{
										if (!particles[i].isActive)
										{
											float coinCenterX = coinX + kCoinSize / 2.0f;
											float coinCenterY = coinY + kCoinSize / 2.0f;

											float angle =
												(6.2831853f / kMaxParticles) * i;

											particles[i].position = {
												coinCenterX,
												coinCenterY
											};

											particles[i].velocity = {
												cosf(angle) * 5.0f,
												sinf(angle) * 5.0f
											};

											particles[i].timer = 0;
											particles[i].lifeTime = kParticleLifeTime;
											particles[i].radius = 5.0f;
											particles[i].isActive = true;

											break;
										}
									}

									if (coinSound != -1) {
										Novice::PlayAudio(
											coinSound, false, 0.8f
										);
									}
								}
							}
						}
					}

					//=================================
					// パーティクル更新
					//=================================
					for (int i = 0; i < kMaxParticles; i++)
					{
						if (particles[i].isActive)
						{
							particles[i].position.posX +=
								particles[i].velocity.posX;

							particles[i].position.posY +=
								particles[i].velocity.posY;

							// 少しずつ減速
							particles[i].velocity.posX *= 0.95f;
							particles[i].velocity.posY *= 0.95f;

							// 少しずつ小さくする
							particles[i].radius *= 0.96f;

							particles[i].timer++;

							if (particles[i].timer >= particles[i].lifeTime)
							{
								particles[i].isActive = false;
							}
						}
					}
				}

				//==========================================
				// スクロール計算（描画用）
				//==========================================
				scrollValue =
					player.worldLeftTop.posX +
					player.size.posX - scrollStartPositionX;

				if (scrollValue < 0.0f) {
					scrollValue = 0.0f;
				}

				float loopScroll =
					fmodf(scrollValue, kBackgroundWidth);

				//================================
				//チュートリアル描画処理
				//================================

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

				// シャチ（敵）の描画
				if (enemy.isActive && enemyTexture != -1) {
					Novice::DrawSprite(
						static_cast<int>(enemy.position.posX - scrollValue),
						static_cast<int>(enemy.position.posY),
						enemyTexture,
						1.0f, 1.0f, 0.0f, WHITE
					);

					// ★チュートリアルでも hasTrampoline が true なら描画
					if (enemy.hasTrampoline && trampoline != -1) {
						Novice::DrawSprite(
							static_cast<int>(enemy.position.posX - scrollValue),
							static_cast<int>(enemy.position.posY - 60.0f),
							trampoline,
							1.0f, 1.0f, 0.0f, WHITE
						);
					}

					// ★【デバッグ用】チュートリアル中のシャチ本体の当たり判定枠を赤線で表示
					Novice::DrawBox(
						static_cast<int>(enemy.position.posX - scrollValue),
						static_cast<int>(enemy.position.posY),
						static_cast<int>(enemy.size.posX),
						static_cast<int>(enemy.size.posY),
						0.0f,
						0xFF0000FF, // 赤色 (RGBA)
						kFillModeWireFrame
					);

					// ★【デバッグ用】チュートリアル中のトランポリン判定ゾーンを赤線で表示
					if (enemy.hasTrampoline) {
						Novice::DrawBox(
							static_cast<int>(enemy.position.posX - scrollValue),
							static_cast<int>(enemy.position.posY - 60.0f + kTrampolineOffsetY),
							kTrampolineWidth,
							20, // 跳ね返り高さの判定幅
							0.0f,
							0xFF0000FF, // 赤色 (RGBA)
							kFillModeWireFrame
						);
					}
				}

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

				// 黄色いコイン用パーティクル描画
				for (int i = 0; i < kMaxParticles; i++)
				{
					if (particles[i].isActive)
					{
						float screenX =
							particles[i].position.posX - scrollValue;
						float screenY =
							particles[i].position.posY;

						// 外側の光：パーティクルに設定された色
						Novice::DrawEllipse(
							(int)screenX,
							(int)screenY,
							(int)(particles[i].radius * 2.0f),
							(int)(particles[i].radius * 2.0f),
							0,
							0xFFAA00FF,
							kFillModeSolid
						);

						// 内側の光：同じ色
						Novice::DrawEllipse(
							(int)screenX,
							(int)screenY,
							(int)particles[i].radius,
							(int)particles[i].radius,
							0,
							particles[i].color,
							kFillModeSolid
						);
					}
				}

				// プレイヤー描画
				if (isJumping && playerJumpTexture != -1) {

					Novice::DrawSpriteRect(
						static_cast<int>(
							player.worldLeftTop.posX - kPlayerOffsetX - scrollValue
							),
						static_cast<int>(player.worldLeftTop.posY - kPlayerOffsetY),
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
							player.worldLeftTop.posX - kPlayerOffsetX - scrollValue
							),
						static_cast<int>(player.worldLeftTop.posY - kPlayerOffsetY),
						playerTexture,
						1.0f, 1.0f, 0.0f, WHITE
					);
				}

				// コイン画像と獲得枚数を右上に表示
				if (coin != -1) {
					Novice::DrawSprite(1080, 15, coin, 0.5f, 0.5f, 0.0f, WHITE);
				}
				DrawCoinCount();

				//==========================================
				// チュートリアルガイド表示（ポーズ中）
				//==========================================
				if (isTutorialPaused) {
					// 画面暗転オーバーレイ
					Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x00000088, kFillModeSolid);

					if (tutorialPauseType == 0) {
						// 【開始時空中ジャンプガイド】
						if (tutorialStep == 1) {
							Novice::DrawSprite(460, 310, tutorialTextTexture, 1.0f, 1.0f, 0.0f, WHITE);
						}
						else if (tutorialStep == 2) {
							Novice::DrawSprite(460, 310, tutorialTextTexture2, 1.0f, 1.0f, 0.0f, WHITE);
						}
						else if (tutorialStep == 3) {
							Novice::DrawSprite(460, 310, tutorialTextTexture3, 1.0f, 1.0f, 0.0f, WHITE);
						}
					}
					else if (tutorialPauseType == 1) {
						// 【トランポリン前ガイド】
						Novice::DrawSprite(460, 310, tutorialTextTexture4, 1.0f, 1.0f, 0.0f, WHITE);
					}
				}

				break;
			}
			}

		} // ポーズ中はシーン更新を停止

		if (isPaused) {
			int texture = isPauseRuleOpen ? ruleTexture : pauseTexture;
			if (texture != -1) Novice::DrawSprite(0, 0, texture, 1.0f, 1.0f, 0.0f, WHITE);
			else Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x030C42FF, kFillModeSolid);
			if (!isPauseRuleOpen && leftCommandTexture != -1) {
				const int arrowY[3] = { 70, 330, 555 };
				Novice::DrawSprite(950, arrowY[pauseSelectedMenu], leftCommandTexture, 1.0f, 1.0f, 0.0f, WHITE);
			}
		}

		//==========================================
		// シーン切り替えフェードイン
		//==========================================
		if (!isPaused && scene != Title &&
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
			//シークレット
			isTimerSecret = false;
			secretX = -100.0f;
		}

		Novice::EndFrame();

	}
	Novice::Finalize();
	return 0;
}