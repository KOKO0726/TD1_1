#include <Novice.h>
#include <string.h>
#include <math.h>

const char kWindowTitle[] = "LC1D_12_タカハシ";

enum GameShene
{
    Title,
    Game,
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

int WINAPI WinMain(
    _In_ HINSTANCE,
    _In_opt_ HINSTANCE,
    _In_ LPSTR,
    _In_ int)
{
    Novice::Initialize(kWindowTitle, 1280, 720);

    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    //==========================================
    // プレイヤーの変数
    //==========================================
    Box player{};
    player.worldLeftTop = { 10, 500 };
    player.size = { 128.0f, 32.0f };

    const float kMoveSpeed = 10.0f;
    const float kStageWidth = 1280.0f * 4.0f;
    const float kMaxScroll = kStageWidth - 1280.0f;

    //==========================================
    // ジャンプ処理
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
    // 背景
    //==========================================
    Box backGrounds[4]{};

    for (int i = 0; i < 4; i++)
    {
        backGrounds[i].worldLeftTop.posX =
            0.0f + 1280 * i;

        backGrounds[i].worldLeftTop.posY = 0.0f;
        backGrounds[i].size = { 1280.0f, 720.0f };
    }

    //==========================================
    // 画像読み込み
    //==========================================
    int playerTexture =
        Novice::LoadTexture(
            "./Resources/images/pengin2.png");

    int playerJumpTexture =
        Novice::LoadTexture(
            "./Resources/images/jump.png");

    int backgroundTexture[4];

    backgroundTexture[0] =
        Novice::LoadTexture(
            "./Resources/images/iceBackGround.png");

    backgroundTexture[1] =
        Novice::LoadTexture(
            "./Resources/images/iceBackGround.png");

    backgroundTexture[2] =
        Novice::LoadTexture(
            "./Resources/images/iceBackGround.png");

    backgroundTexture[3] =
        Novice::LoadTexture(
            "./Resources/images/iceBackGround.png");

    //==========================================
    // タイトル画像
    //==========================================
    int titleTexture1 =
        Novice::LoadTexture(
            "./Resources/images/penginno.png");

    int titleTexture2 =
        Novice::LoadTexture(
            "./Resources/images/harasuberi.png");

    int spaceTexture =
        Novice::LoadTexture(
            "./Resources/images/space.png");

    //==========================================
    // 小物画像
    //==========================================
    int trampoline =
        Novice::LoadTexture(
            "./Resources/images/trampoline.png");

    int coin =
        Novice::LoadTexture(
            "./Resources/images/coin.png");

    //==========================================
    // 効果音
    //==========================================
    int coinSound =
        Novice::LoadAudio(
            "./Resources/sound/coin.MP3");

    int trampolineSound =
        Novice::LoadAudio(
            "./Resources/sound/trampoline.MP3");

    int jumpSound =
        Novice::LoadAudio(
            "./Resources/sound/jump.MP3");

    //==========================================
    // トランポリン
    //==========================================
    Vector2 trampolinePositions[] =
    {
        {500.0f, 500.0f},
        {1000.0f, 500.0f},
        {1500.0f, 500.0f},
        {2200.0f, 500.0f},
    };

    //==========================================
    // コインマップ
    //==========================================
    int coinMap[6][100] =
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,0,1},
        {0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    };

    //==========================================
    // トランポリン変数
    //==========================================
    const int kTrampolineCount =
        sizeof(trampolinePositions) /
        sizeof(trampolinePositions[0]);

    const float kTrampolineBouncePower = 25.0f;
    const int kTrampolineWidth = 32;

    //==========================================
    // コイン変数
    //==========================================
    const int kCoinMapHeight = 6;
    const int kCoinMapWidth = 100;
    const int kCoinMapSize = 64;
    const float kCoinSize = 64.0f;

    int coinCount = 0;

    //==========================================
    // スクロール
    //==========================================
    float scrollValue = 0.0f;
    float scrollStartPositionX = 500.0f;

    const float kGroundY = 500.0f;

    //==========================================
    // タイトル変数
    //==========================================
    int scene = Title;

    int timer = 0;
    int fadeAlpha = 255;

    int gameFadeTimer = 0;

    float title1Y = -180.0f;
    float title2Y = -250.0f;

    float title1Velocity = 0.0f;
    float title2Velocity = 0.0f;

    float penguinX = 1400.0f;
    float penguinY = 480.0f;
    float penguinSpeed = 15.0f;

    //==========================================
    // メインループ
    //==========================================
    while (Novice::ProcessMessage() == 0)
    {
        Novice::BeginFrame();

        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        switch (scene)
        {
            //==========================================
            // タイトル画面
            //==========================================
        case Title:
        {
            timer++;

            // 背景フェードイン
            if (timer <= 60)
            {
                fadeAlpha =
                    255 - timer * 255 / 60;

                if (fadeAlpha < 0)
                {
                    fadeAlpha = 0;
                }
            }

            // タイトル1 落下・バウンド
            if (timer > 60)
            {
                if (title1Y < 100.0f ||
                    title1Velocity != 0.0f)
                {
                    title1Velocity += 0.7f;
                    title1Y += title1Velocity;

                    if (title1Y >= 100.0f)
                    {
                        title1Y = 100.0f;
                        title1Velocity *= -0.4f;

                        if (fabsf(title1Velocity) < 1.5f)
                        {
                            title1Velocity = 0.0f;
                        }
                    }
                }
            }

            // タイトル2 落下・バウンド
            if (timer > 85)
            {
                if (title2Y < 230.0f ||
                    title2Velocity != 0.0f)
                {
                    title2Velocity += 0.7f;
                    title2Y += title2Velocity;

                    if (title2Y >= 230.0f)
                    {
                        title2Y = 230.0f;
                        title2Velocity *= -0.4f;

                        if (fabsf(title2Velocity) < 1.5f)
                        {
                            title2Velocity = 0.0f;
                        }
                    }
                }
            }

            // ペンギンが右から登場
            if (timer > 180)
            {
                if (penguinX > 520.0f)
                {
                    penguinX -= penguinSpeed;

                    if (penguinX < 900.0f)
                    {
                        penguinSpeed *= 0.98f;
                    }

                    if (penguinSpeed < 5.0f)
                    {
                        penguinSpeed = 5.0f;
                    }

                    if (penguinX <= 520.0f)
                    {
                        penguinX = 520.0f;
                    }
                }
            }

            // ペンギン待機アニメーション
            if (timer > 300)
            {
                penguinY =
                    480.0f + sinf(timer * 0.07f) * 5.0f;

                // SPACEでゲーム開始
                if (preKeys[DIK_SPACE] == 0 &&
                    keys[DIK_SPACE] != 0)
                {
                    scene = Game;
                    gameFadeTimer = 0;
                }
            }

            // Rキーでタイトル再生
            if (preKeys[DIK_R] == 0 &&
                keys[DIK_R] != 0)
            {
                timer = 0;
                fadeAlpha = 255;

                title1Y = -180.0f;
                title2Y = -250.0f;

                title1Velocity = 0.0f;
                title2Velocity = 0.0f;

                penguinX = 1400.0f;
                penguinY = 480.0f;
                penguinSpeed = 15.0f;
            }

            //==========================================
            // タイトル描画
            //==========================================

            // 背景
            if (backgroundTexture[0] != -1)
            {
                Novice::DrawSprite(
                    0,
                    0,
                    backgroundTexture[0],
                    1.0f,
                    1.0f,
                    0.0f,
                    WHITE
                );
            }

            // タイトル1
            if (timer > 60 && titleTexture1 != -1)
            {
                Novice::DrawSprite(
                    430,
                    static_cast<int>(title1Y),
                    titleTexture1,
                    1.5f,
                    1.5f,
                    0.0f,
                    WHITE
                );
            }

            // タイトル2
            if (timer > 85 && titleTexture2 != -1)
            {
                Novice::DrawSprite(
                    400,
                    static_cast<int>(title2Y),
                    titleTexture2,
                    2.0f,
                    2.0f,
                    0.0f,
                    WHITE
                );
            }

            // ペンギン
            if (timer > 180 && playerTexture != -1)
            {
                Novice::DrawSprite(
                    static_cast<int>(penguinX),
                    static_cast<int>(penguinY),
                    playerTexture,
                    1.5f,
                    1.5f,
                    0.0f,
                    WHITE
                );
            }

            // SPACE画像の点滅
            if (timer > 300 && spaceTexture != -1)
            {
                if ((timer / 30) % 2 == 0)
                {
                    Novice::DrawSprite(
                        470,
                        620,
                        spaceTexture,
                        0.3f,
                        0.3f,
                        0.0f,
                        WHITE
                    );
                }
            }

            // タイトルのフェードイン
            if (timer <= 60)
            {
                Novice::DrawBox(
                    0,
                    0,
                    1280,
                    720,
                    0.0f,
                    static_cast<unsigned int>(fadeAlpha),
                    kFillModeSolid
                );
            }

            break;
        }

        //==========================================
        // ゲーム画面
        //==========================================
        case Game:
        {
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

            break;
        }

        } // switch終了

        //==========================================
        // ゲーム画面への1秒フェードイン
        //==========================================
        if (scene == Game && gameFadeTimer < 60)
        {
            int alpha =
                255 - gameFadeTimer * 255 / 60;

            Novice::DrawBox(
                0,
                0,
                1280,
                720,
                0.0f,
                static_cast<unsigned int>(alpha),
                kFillModeSolid
            );

            gameFadeTimer++;
        }

        if (scene == Title)
        {
            gameFadeTimer = 0;
        }

        // フレームの終了
        Novice::EndFrame();

        // ESCキーが押されたらループを抜ける
        if (preKeys[DIK_ESCAPE] == 0 &&
            keys[DIK_ESCAPE] != 0)
        {
            break;
        }

    } // while終了

    Novice::Finalize();

    return 0;
}