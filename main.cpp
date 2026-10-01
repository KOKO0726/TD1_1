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
	player.worldLeftTop = { 10,560, };
	player.size = { 32.0f,32.0f };
	const float kMoveSpeed = 10.0f;
	const float kStageWidth = 1280.0f * 4.0f;
	const float kMaxScroll = kStageWidth - 1280.0f;
	//==========================================
	//プレイヤーのジャンプ処理の変数
	//==========================================
	float playerVelocityY = 0.0f;
	const float kGravity = 0.8f;
	const float kJumpPower = 15.0f;
	const float kGroundY = 560.0f;
	bool isJumping = false;
	//==========================================
	//背景画像
	//==========================================
	int backgroundTexture[4];
	backgroundTexture[0] = Novice::LoadTexture("./Resources/images/bg1.png");
	backgroundTexture[1] = Novice::LoadTexture("./Resources/images/bg2.png");
	backgroundTexture[2] = Novice::LoadTexture("./Resources/images/bg3.png");
	backgroundTexture[3] = Novice::LoadTexture("./Resources/images/bg4.png");
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
	//小物画像
	//==========================================
	int trampoline = 0; //トランポリン
	trampoline = Novice::LoadTexture("./Resources/images/trampoline.png");

	int coin = 0; //コイン
	coin = Novice::LoadTexture("./Resources/images/coin.png");
	//==========================================
	//小物用画像
	//==========================================
	///トランポリン変数
	const int kTrampolineWidth = 25; //横のトランポリンのサイズ
	const int kTrampolineHeight = 25; //縦のトランポリンのサイズ

	const int kTrampolineSize = 32; //トランポリンのタイルサイズ

	///コイン変数
	const int kCoinWidth = 25; //横コインのサイズ
	const int kCoinHeight = 25; //縦のコインサイズ

	const int kCoinSize = 32; //コインのタイルサイズ
	//==========================================
	//スクロール値
	//==========================================
	float scrollValue = 0.0f;
	//==========================================
	//スクロール開始位置
	//==========================================
	float scrollStartPositionX = 800.0f;

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
		//===========================
		//ジャンプ
		//===========================
		if (keys[DIK_SPACE] && !isJumping)
		{
			playerVelocityY = -kJumpPower;
			isJumping = true;
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
		//地面判定
		//===========================
		if (player.worldLeftTop.posY >= kGroundY)
		{
			player.worldLeftTop.posY = kGroundY;
			playerVelocityY = 0.0f;
			isJumping = false;
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
		//プレイヤー描画処理
		//===========================
		Novice::DrawBox(
			static_cast<int>(player.worldLeftTop.posX - scrollValue),
			static_cast<int>(player.worldLeftTop.posY),
			static_cast<int>(player.size.posX),
			static_cast<int>(player.size.posY),
			0.0f,
			static_cast<int>(player.color),
			kFillModeSolid
		);

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
