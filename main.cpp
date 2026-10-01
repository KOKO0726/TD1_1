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
	float radius;
	unsigned int color = WHITE;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Box player{};
	player.worldLeftTop = { 10,560, };
	player.size = { 32.0f,32.0f };
	const float kMoveSpeed = 15.0f;
	const float kStageWidth = 1280.0f * 4.0f;
	const float kMaxScroll = kStageWidth - 1280.0f;

	float playerVelocityY = 0.0f;
	const float kGravity = 0.8f;
	const float kJumpPower = 15.0f;
	const float kGroundY = 560.0f;
	bool isJumping = false;

	int backgroundTexture[4];
	backgroundTexture[0] = Novice::LoadTexture("./Resources/images/bg1.png");
	backgroundTexture[1] = Novice::LoadTexture("./Resources/images/bg2.png");
	backgroundTexture[2] = Novice::LoadTexture("./Resources/images/bg3.png");
	backgroundTexture[3] = Novice::LoadTexture("./Resources/images/bg4.png");

	Box backGrounds[4]{};
	for (int i = 0; i < 4; i++)
	{
		backGrounds[i].worldLeftTop.posX = 0.0f + 1280 * i;
		backGrounds[i].worldLeftTop.posY = 0.0f;
		backGrounds[i].size = { 1280.0f,720.f };
	}

	//スクロール値
	float scrollValue = 0.0f;
	//スクロール開始位置
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
	//プレイヤー移動処理
		if (keys[DIK_A])
		{
			player.worldLeftTop.posX -= kMoveSpeed;
		}

		if (keys[DIK_D])
		{
			player.worldLeftTop.posX += kMoveSpeed;
		}

		//ジャンプ
		if (keys[DIK_SPACE] && !isJumping)
		{
			playerVelocityY = -kJumpPower;
			isJumping = true;
		}

		//重力
		if (isJumping)
		{
			playerVelocityY += kGravity;
			player.worldLeftTop.posY += playerVelocityY;
		}

		//地面判定
		if (player.worldLeftTop.posY >= kGroundY)
		{
			player.worldLeftTop.posY = kGroundY;
			playerVelocityY = 0.0f;
			isJumping = false;
		}

		//壁判定
		if (player.worldLeftTop.posX < 0.0f)
		{
			player.worldLeftTop.posX = 0.0f;
		}

		if (player.worldLeftTop.posX > kStageWidth - player.size.posX)
		{
			player.worldLeftTop.posX = kStageWidth - player.size.posX;
		}

		//スクロール処理
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
		//画像描画処理
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
		//プレイヤー描画処理
		Novice::DrawBox(
			static_cast<int>(player.worldLeftTop.posX - scrollValue),
			static_cast<int>(player.worldLeftTop.posY),
			static_cast<int>(player.size.posX),
			static_cast<int>(player.size.posY),
			0.0f,
			static_cast<int>(player.color),
			kFillModeSolid
		);

		Novice::ScreenPrintf(20, 20, "WASD: Move Player on the enemy");
		Novice::ScreenPrintf(20, 40, "Arrow Keys: Move Scroll Start Line");
		Novice::ScreenPrintf(20, 100, "Scroll X (on World Axis):%f", scrollValue);
		Novice::ScreenPrintf(20, 120, "Scroll Start Line(on Screen Axis): 800");
		Novice::ScreenPrintf(20, 140, "player.pos.x:%f", player.worldLeftTop.posX);

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
