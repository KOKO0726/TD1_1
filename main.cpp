#include <Novice.h>

const char kWindowTitle[] = "LC1D_12_タカハシ_コウノスケ";

struct Vector2
{
	float x = 0.0f;
	float y = 0.0f;
};
struct Box
{
	Vector2 worldLeftTop{};
	Vector2 size{};
	unsigned int color = WHITE;
};

struct Line
{
	Vector2 strat{};
	Vector2 end{};

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

	int backgroundTexture[4];
	backgroundTexture[0] = Novice::LoadTexture("./Resources/images/bg1.png");
	backgroundTexture[1] = Novice::LoadTexture("./Resources/images/bg2.png");
	backgroundTexture[2] = Novice::LoadTexture("./Resources/images/bg3.png");
	backgroundTexture[3] = Novice::LoadTexture("./Resources/images/bg4.png");

	Box backGrounds[4]{};
	for (int i = 0; i < 4; i++)
	{
		backGrounds[i].worldLeftTop.x = 0.0f + 1280 * i;
		backGrounds[i].worldLeftTop.y = 0.0f;
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
			player.worldLeftTop.x -= kMoveSpeed;
		}
		if (keys[DIK_D])
		{
			player.worldLeftTop.x += kMoveSpeed;
		}

		if (keys[DIK_LEFT])
		{
			scrollStartPositionX -= kMoveSpeed;
		}
		if (keys[DIK_RIGHT])
		{
			scrollStartPositionX += kMoveSpeed;
		}
		//壁判定
		if (player.worldLeftTop.x < 0.0f)
		{
			player.worldLeftTop.x = 0.0f;
		}

		if (player.worldLeftTop.x > kStageWidth - player.size.x)
		{
			player.worldLeftTop.x = kStageWidth - player.size.x;
		}

		//スクロール処理
		scrollValue = player.worldLeftTop.x + player.size.x - scrollStartPositionX;

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
				backGrounds[i].worldLeftTop.x - scrollValue;
			Novice::DrawSprite(
				static_cast<int>(kBackGroudScreenLeftTopX),
				static_cast<int>(backGrounds[i].worldLeftTop.y),
				backgroundTexture[i],
				1.0f,
				1.0f,
				0.0f,
				WHITE
			);
		}
		//プレイヤー描画処理
		Novice::DrawBox(
			static_cast<int>(player.worldLeftTop.x - scrollValue),
			static_cast<int>(player.worldLeftTop.y),
			static_cast<int>(player.size.x),
			static_cast<int>(player.size.y),
			0.0f,
			static_cast<int>(player.color),
			kFillModeSolid
		);

		Novice::DrawLine(
			static_cast<int>(scrollStartPositionX),
			0,
			static_cast<int>(scrollStartPositionX),
			800,
			RED
		);

		Novice::ScreenPrintf(20, 20, "WASD: Move Player on the enemy");
		Novice::ScreenPrintf(20, 40, "Arrow Keys: Move Scroll Start Line");
		Novice::ScreenPrintf(20, 100, "Scroll X (on World Axis):%f", scrollValue);
		Novice::ScreenPrintf(20, 120, "Scroll Start Line(on Screen Axis): 800");
		Novice::ScreenPrintf(20, 140, "player.pos.x:%f", player.worldLeftTop.x);

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
