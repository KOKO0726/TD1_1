#include <Novice.h>
#include <string.h>
#include <math.h>

const char kWindowTitle[] = "LC1D_12_タカハシ";

enum GameShene
{
    Title,
    Game,
    tutorial,
    BonusStage, // ボーナスステージ
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
    player.worldLeftTop = { 10, 500 };
    player.size = { 128.0f, 32.0f };

    const float kMoveSpeed = 10.0f;
    const float kStageWidth = 1280.0f * 4.0f;
    const float kMaxScroll = kStageWidth - 1280.0f;
    const float kBackgroundWidth = 1280.0f * 4.0f;

    //==========================================
    // 宇宙ボーナスステージ用アニメーション変数
    //==========================================
    int universeFrame = 0;
    int universeAnimationTimer = 0;
    const int kUniverseFrameCount = 4;

    //==========================================
    // 宇宙ボーナス遷移用変数
    //==========================================
    bool isTransitioningToBonus = false;
    int bonusTransitionTimer = 0;
    const int kMaxTransitionTime = 60;

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
    // 背景
    //==========================================
    Box backGrounds[4]{};

    for (int i = 0; i < 4; i++)
    {
        backGrounds[i].worldLeftTop.posX = 1280.0f * i;
        backGrounds[i].worldLeftTop.posY = 0.0f;
        backGrounds[i].size = { 1280.0f, 720.0f };
    }

    //==========================================
    // 画像読み込み
    //==========================================
    int playerTexture =
        Novice::LoadTexture("./Resources/images/pengin2.png");

    int playerJumpTexture =
        Novice::LoadTexture("./Resources/images/jump.png");

    int universepengin = Novice::LoadTexture("./Resources/images/universepengin.png");

    int backgroundTexture[4] = {};

    for (int i = 0; i < 4; i++)
    {
        backgroundTexture[i] =
            Novice::LoadTexture("./Resources/images/iceBackGround.png");
    }

    // 宇宙画像
    int universeTexture = Novice::LoadTexture("./Resources/images/universe.png");
    int universe2Texture = Novice::LoadTexture("./Resources/images/universe2.png");

    int titleTexture1 =
        Novice::LoadTexture("./Resources/images/penginno.png");

    int titleTexture2 =
        Novice::LoadTexture("./Resources/images/harasuberi.png");

    int menuTextures[3] =
    {
        Novice::LoadTexture("./Resources/images/space.png"),
        Novice::LoadTexture("./Resources/images/tutorial.png"),
        Novice::LoadTexture("./Resources/images/credit.png")
    };

    int selectArrowTexture =
        Novice::LoadTexture("./Resources/images/command.png");

    int pushSpaceTexture =
        Novice::LoadTexture("./Resources/images/pushSpace.png");

    int trampoline =
        Novice::LoadTexture("./Resources/images/trampoline.png");

    int coin =
        Novice::LoadTexture("./Resources/images/coin.png");

    //==========================================
    // 効果音
    //==========================================
    int fireSound =
        Novice::LoadAudio("./Resources/sound/fire.MP3");

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
    int kTitleHandle =
        Novice::LoadAudio("./Resources/bgm/title.MP3");

    int kTutorialHandle =
        Novice::LoadAudio("./Resources/bgm/tutorial.MP3");

    int titlePlayHandle = -1;
    int tutorialPlayHandle = -1;

    if (kTitleHandle != -1)
    {
        titlePlayHandle =
            Novice::PlayAudio(kTitleHandle, true, 0.5f);
    }

    //==========================================
    // トランポリン配置
    //==========================================
    Vector2 trampolinePositions[] =
    {
        {500.0f, 500.0f},
        {1000.0f, 500.0f},
        {1500.0f, 500.0f},
        {2200.0f, 500.0f},
    };

    //==========================================
    // コイン配置
    //==========================================
    int coinMap[6][100] =
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,0,1},
        {0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    };

    const int kTrampolineCount =
        sizeof(trampolinePositions) / sizeof(trampolinePositions[0]);

    // 宇宙まで跳べるようにバウンド力を大きめに調整
    const float kTrampolineBouncePower = 40.0f;
    const int kTrampolineWidth = 32;

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
    // シーン
    //==========================================
    int scene = Title;

    int timer = 0;
    int fadeAlpha = 255;
    int gameFadeTimer = 0;

    int selectedMenu = 0;

    //==========================================
    // タイトル決定後のフェードアウト
    //==========================================
    bool isTitleFading = false;
    int titleFadeTimer = 0;
    const int kTitleFadeDuration = 90;

    //==========================================
    // タイトル文字
    //==========================================
    float title1Y = -180.0f;
    float title2Y = -250.0f;

    float title1Velocity = 0.0f;
    float title2Velocity = 0.0f;

    //==========================================
    // タイトルペンギン
    //==========================================
    float penguinX = -200.0f;
    float penguinY = 380.0f;
    float penguinSpeed = 8.0f;

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
        case Title:
        {
            timer++;

            //==========================================
            // タイトル開始時のフェードイン更新
            //==========================================
            if (timer <= 60)
            {
                fadeAlpha = 255 - timer * 255 / 60;

                if (fadeAlpha < 0)
                {
                    fadeAlpha = 0;
                }
            }

            //==========================================
            // 「ペンギンの」落下・バウンド
            //==========================================
            if (timer > 60)
            {
                if (title1Y < 20.0f || title1Velocity != 0.0f)
                {
                    title1Velocity += 0.7f;
                    title1Y += title1Velocity;

                    if (title1Y >= 20.0f)
                    {
                        title1Y = 20.0f;
                        title1Velocity *= -0.4f;

                        if (fabsf(title1Velocity) < 1.5f)
                        {
                            title1Velocity = 0.0f;
                        }
                    }
                }
            }

            //==========================================
            // 「はらすべり」落下・バウンド
            //==========================================
            if (timer > 85)
            {
                if (title2Y < 150.0f || title2Velocity != 0.0f)
                {
                    title2Velocity += 0.7f;
                    title2Y += title2Velocity;

                    if (title2Y >= 150.0f)
                    {
                        title2Y = 150.0f;
                        title2Velocity *= -0.4f;

                        if (fabsf(title2Velocity) < 1.5f)
                        {
                            title2Velocity = 0.0f;
                        }
                    }
                }
            }

            //==========================================
            // ペンギン移動
            //==========================================
            if (timer > 180)
            {
                penguinX += penguinSpeed;

                if (penguinX > 1280.0f)
                {
                    penguinX = -200.0f;
                }
            }

            if (timer > 300)
            {
                penguinY =
                    380.0f + sinf(timer * 0.07f) * 5.0f;
            }

            //==========================================
            // メニュー操作＋効果音
            //==========================================
            if (timer > 365 && !isTitleFading)
            {
                // 左へ移動
                if ((preKeys[DIK_LEFT] == 0 && keys[DIK_LEFT] != 0) ||
                    (preKeys[DIK_A] == 0 && keys[DIK_A] != 0))
                {
                    selectedMenu = (selectedMenu + 2) % 3;

                    if (cursorSound != -1)
                    {
                        Novice::PlayAudio(cursorSound, false, 0.8f);
                    }
                }

                // 右へ移動
                if ((preKeys[DIK_RIGHT] == 0 && keys[DIK_RIGHT] != 0) ||
                    (preKeys[DIK_D] == 0 && keys[DIK_D] != 0))
                {
                    selectedMenu = (selectedMenu + 1) % 3;

                    if (cursorSound != -1)
                    {
                        Novice::PlayAudio(cursorSound, false, 0.8f);
                    }
                }

                // SPACEで決定
                if (preKeys[DIK_SPACE] == 0 &&
                    keys[DIK_SPACE] != 0)
                {
                    // 遷移先があるメニューだけ決定
                    if (selectedMenu == 0 || selectedMenu == 1)
                    {
                        if (pushSound != -1)
                        {
                            Novice::PlayAudio(pushSound, false, 0.8f);
                        }

                        isTitleFading = true;
                        titleFadeTimer = 0;
                    }
                }
            }

            //==========================================
            // 決定後のフェードアウト更新
            //==========================================
            if (isTitleFading)
            {
                titleFadeTimer++;

                // 真っ暗になった次のフレームで遷移
                if (titleFadeTimer > kTitleFadeDuration)
                {
                    if (titlePlayHandle != -1)
                    {
                        Novice::StopAudio(titlePlayHandle);
                    }

                    if (selectedMenu == 0)
                    {
                        scene = Game;
                    }
                    else if (selectedMenu == 1)
                    {
                        if (kTutorialHandle != -1)
                        {
                            tutorialPlayHandle =
                                Novice::PlayAudio(
                                    kTutorialHandle,
                                    true,
                                    0.5f
                                );
                        }

                        scene = tutorial;
                    }

                    gameFadeTimer = 0;
                    isTitleFading = false;
                }
            }

            //==========================================
            // Rキーでタイトル演出を再生
            //==========================================
            if (!isTitleFading &&
                preKeys[DIK_R] == 0 &&
                keys[DIK_R] != 0)
            {
                timer = 0;
                fadeAlpha = 255;

                title1Y = -180.0f;
                title2Y = -250.0f;

                title1Velocity = 0.0f;
                title2Velocity = 0.0f;

                penguinX = -200.0f;
                penguinY = 380.0f;
                penguinSpeed = 8.0f;

                selectedMenu = 0;
            }

            //==========================================
            // 背景
            //==========================================
            if (backgroundTexture[0] != -1)
            {
                Novice::DrawSprite(
                    0, 0,
                    backgroundTexture[0],
                    1.0f, 1.0f, 0.0f, WHITE
                );
            }

            //==========================================
            // タイトル1
            //==========================================
            if (timer > 60 && titleTexture1 != -1)
            {
                Novice::DrawSprite(
                    430,
                    static_cast<int>(title1Y),
                    titleTexture1,
                    1.5f, 1.5f, 0.0f, WHITE
                );
            }

            //==========================================
            // タイトル2
            //==========================================
            if (timer > 85 && titleTexture2 != -1)
            {
                Novice::DrawSprite(
                    400,
                    static_cast<int>(title2Y),
                    titleTexture2,
                    2.0f, 2.0f, 0.0f, WHITE
                );
            }

            //==========================================
            // ペンギン
            //==========================================
            if (timer > 180 && playerTexture != -1)
            {
                Novice::DrawSprite(
                    static_cast<int>(penguinX),
                    static_cast<int>(penguinY),
                    playerTexture,
                    1.5f, 1.5f, 0.0f, WHITE
                );
            }

            //==========================================
            // メニュー登場アニメーション
            //==========================================
            if (timer > 300)
            {
                const int menuX[3] = { 185, 515, 845 };
                const int menuY = 485;

                for (int i = 0; i < 3; i++)
                {
                    int elapsed = timer - 301 - i * 9;

                    if (elapsed < 0 || menuTextures[i] == -1)
                    {
                        continue;
                    }

                    float offsetY = 0.0f;

                    if (elapsed < 18)
                    {
                        float t =
                            static_cast<float>(elapsed) / 18.0f;

                        float eased =
                            1.0f - (1.0f - t) * (1.0f - t);

                        offsetY = 180.0f - 200.0f * eased;
                    }
                    else if (elapsed < 28)
                    {
                        float t =
                            static_cast<float>(elapsed - 18) / 10.0f;

                        offsetY = -20.0f + 20.0f * t;
                    }
                    else if (elapsed < 35)
                    {
                        float t =
                            static_cast<float>(elapsed - 28) / 7.0f;

                        offsetY =
                            -7.0f * sinf(t * 3.14159265f);
                    }

                    Novice::DrawSprite(
                        menuX[i],
                        menuY + static_cast<int>(offsetY),
                        menuTextures[i],
                        1.0f, 1.0f, 0.0f, WHITE
                    );
                }

                // 選択矢印
                if (timer > 365 && selectArrowTexture != -1)
                {
                    Novice::DrawSprite(
                        menuX[selectedMenu] + 109,
                        menuY - 38,
                        selectArrowTexture,
                        1.0f, 1.0f, 0.0f, WHITE
                    );
                }
            }

            //==========================================
            // PUSH SPACE
            //==========================================
            if (timer > 365 && pushSpaceTexture != -1)
            {
                if ((timer / 30) % 2 == 0)
                {
                    Novice::DrawSprite(
                        500,
                        585,
                        pushSpaceTexture,
                        2.0f,
                        2.0f,
                        0.0f,
                        WHITE
                    );
                }
            }

            //==========================================
            // タイトル開始時のフェードイン
            //==========================================
            if (timer <= 60)
            {
                unsigned int fadeColor =
                    static_cast<unsigned int>(fadeAlpha);

                Novice::DrawBox(
                    0, 0, 1280, 720,
                    0.0f,
                    fadeColor,
                    kFillModeSolid
                );
            }

            //==========================================
            // 決定後のフェードアウト描画
            //==========================================
            if (isTitleFading)
            {
                int alpha =
                    titleFadeTimer * 255 / kTitleFadeDuration;

                if (alpha > 255)
                {
                    alpha = 255;
                }

                unsigned int fadeColor =
                    static_cast<unsigned int>(alpha);

                Novice::DrawBox(
                    0,
                    0,
                    1280,
                    720,
                    0.0f,
                    fadeColor,
                    kFillModeSolid
                );
            }

            break;
        }

        //==========================================
        // ゲーム処理
        //==========================================
        case Game:
        {
            if (!isTransitioningToBonus)
            {
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
            }

            if (isJumping)
            {
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

                float trampolineRight = trampolineX + kTrampolineWidth;
                float playerBottom = player.worldLeftTop.posY + player.size.posY;
                float playerRight = player.worldLeftTop.posX + player.size.posX;

                if (playerVelocityY > 0.0f)
                {
                    if (player.worldLeftTop.posX < trampolineRight && playerRight > trampolineX)
                    {
                        if (playerBottom >= trampolineY && playerBottom <= trampolineY + 20.0f)
                        {
                            player.worldLeftTop.posY = trampolineY - player.size.posY;
                            playerVelocityY = -kTrampolineBouncePower;
                            Novice::PlayAudio(trampolineSound, false, 0.8f);
                            isJumping = true;
                        }
                    }
                }
            }

            // 画面上外飛び出しチェック（宇宙へ遷移）
            if (player.worldLeftTop.posY < -100.0f && !isTransitioningToBonus)
            {
                isTransitioningToBonus = true;
                bonusTransitionTimer = 0;
            }

            //===========================
            //地面判定
            //===========================
            if (player.worldLeftTop.posY >= kGroundY)
            {
                player.worldLeftTop.posY = kGroundY;
                playerVelocityY = 0.0f;
                isJumping = false;
                jumpCount = 0;
            }

            //===========================
            //コイン判定
            //===========================
            for (int y = 0; y < kCoinMapHeight; y++)
            {
                for (int x = 0; x < kCoinMapWidth; x++)
                {
                    if (coinMap[y][x] != 1)
                    {
                        continue;
                    }

                    float coinX = static_cast<float>(x * kCoinMapSize);
                    float coinY = static_cast<float>(y * kCoinMapSize);

                    float coinRight = coinX + kCoinSize;
                    float coinBottom = coinY + kCoinSize;

                    float playerRight = player.worldLeftTop.posX + player.size.posX;
                    float playerBottom = player.worldLeftTop.posY + player.size.posY;

                    if (player.worldLeftTop.posX < coinRight &&
                        playerRight > coinX &&
                        player.worldLeftTop.posY < coinBottom &&
                        playerBottom > coinY)
                    {
                        coinMap[y][x] = 0;
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

            //============================
            //背景描画
            //============================
            for (int i = 0; i < 4; i++)
            {
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
            //トランポリン描画
            //===========================
            for (int i = 0; i < kTrampolineCount; i++)
            {
                int screenX = static_cast<int>(trampolinePositions[i].posX - scrollValue);
                int screenY = static_cast<int>(trampolinePositions[i].posY);

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
            //コイン描画
            //===========================
            for (int y = 0; y < kCoinMapHeight; y++)
            {
                for (int x = 0; x < kCoinMapWidth; x++)
                {
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
            //コイン枚数描画
            //===========================
            Novice::ScreenPrintf(
                20,
                20,
                "COIN : %d",
                coinCount
            );

            //===========================
            //プレイヤー描画
            //===========================
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

            // 宇宙ボーナスステージ遷移暗転処理
            if (isTransitioningToBonus)
            {
                bonusTransitionTimer++;
                int alpha = (bonusTransitionTimer * 255) / kMaxTransitionTime;
                if (alpha > 255)
                {
                    alpha = 255;
                }

                Novice::DrawBox(
                    0,
                    0,
                    1280,
                    720,
                    0.0f,
                    static_cast<unsigned int>(alpha),
                    kFillModeSolid
                );

                if (bonusTransitionTimer >= kMaxTransitionTime)
                {
                    scene = BonusStage;
                    isTransitioningToBonus = false;
                    bonusTransitionTimer = 0;

                    player.worldLeftTop.posY = 200.0f;
                    playerVelocityY = 0.0f;
                }
            }

            break;
        }

        //==========================================
        // 宇宙ボーナスステージ
        //==========================================
        case BonusStage:
        {
            // 1. 自動前進
            player.worldLeftTop.posX += kMoveSpeed * 0.5f;

            // 2. 水中風操作：SPACEキーを押すと宇宙服の炎を噴射してふわっと浮上＋効果音再生
            if (preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE] != 0)
            {
                playerVelocityY = -6.0f;
                if (fireSound != -1)
                {
                    Novice::PlayAudio(fireSound, false, 0.2f);
                }
            }

            // 3. 水中風の緩やかな重力
            const float kSpaceGravity = 0.25f;
            playerVelocityY += kSpaceGravity;

            // 落下速度の上限を制限
            if (playerVelocityY > 4.0f)
            {
                playerVelocityY = 4.0f;
            }

            player.worldLeftTop.posY += playerVelocityY;

            // 画面上下の移動制限
            if (player.worldLeftTop.posY < 0.0f)
            {
                player.worldLeftTop.posY = 0.0f;
                playerVelocityY = 0.0f;
            }
            if (player.worldLeftTop.posY >= kGroundY)
            {
                player.worldLeftTop.posY = kGroundY;
                playerVelocityY = 0.0f;
            }

            // 4. 連番アニメーション処理（コマ送り）
            universeAnimationTimer++;
            if (universeAnimationTimer >= 8)
            {
                universeAnimationTimer = 0;
                universeFrame = (universeFrame + 1) % kUniverseFrameCount;
            }

            // 5. スクロール処理
            scrollValue = player.worldLeftTop.posX + player.size.posX - scrollStartPositionX;
            if (scrollValue < 0.0f) scrollValue = 0.0f;

            // 背景2枚分の総幅 (1280 * 2 = 2560)
            const float kLoopWidth = 2560.0f;

            // スクロール値を 0〜2560 の範囲に正規化
            float loopScroll = fmodf(scrollValue, kLoopWidth);

            // 2枚の画像を交互に循環させて連続ループ処理
            float universe1X = 0.0f - loopScroll;
            if (universe1X <= -1280.0f) universe1X += kLoopWidth;

            float universe2X = 1280.0f - loopScroll;
            if (universe2X <= -1280.0f) universe2X += kLoopWidth;

            // 宇宙背景の描画
            if (universeTexture != -1)
            {
                Novice::DrawSprite(static_cast<int>(universe1X), 0, universeTexture, 1.0f, 1.0f, 0.0f, WHITE);
            }
            if (universe2Texture != -1)
            {
                Novice::DrawSprite(static_cast<int>(universe2X), 0, universe2Texture, 1.0f, 1.0f, 0.0f, WHITE);
            }

            // 6. 宇宙ペンギン描画（連番画像を切り抜いて表示）
            if (universepengin != -1)
            {
                Novice::DrawSpriteRect(
                    static_cast<int>(player.worldLeftTop.posX - scrollValue),
                    static_cast<int>(player.worldLeftTop.posY),
                    universeFrame * 128, // 切り抜き開始X位置
                    0,                   // 切り抜き開始Y位置
                    128,                 // 1コマの横幅
                    128,                 // 1コマの高さ
                    universepengin,
                    1.0f / static_cast<float>(kUniverseFrameCount),
                    1.0f,
                    0.0f,
                    WHITE
                );
            }

            // フェードイン演出
            if (bonusTransitionTimer < kMaxTransitionTime)
            {
                bonusTransitionTimer++;
                int alpha = 255 - (bonusTransitionTimer * 255) / kMaxTransitionTime;
                if (alpha < 0) alpha = 0;

                Novice::DrawBox(
                    0,
                    0,
                    1280,
                    720,
                    0.0f,
                    static_cast<unsigned int>(alpha),
                    kFillModeSolid
                );
            }

            Novice::ScreenPrintf(20, 20, "BONUS STAGE!");
            break;
        }

        //==========================================
        // チュートリアル処理
        //==========================================
        case tutorial:
        {
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

                float trampolineRight = trampolineX + kTrampolineWidth;
                float playerBottom = player.worldLeftTop.posY + player.size.posY;
                float playerRight = player.worldLeftTop.posX + player.size.posX;

                if (playerVelocityY > 0.0f)
                {
                    if (player.worldLeftTop.posX < trampolineRight && playerRight > trampolineX)
                    {
                        if (playerBottom >= trampolineY && playerBottom <= trampolineY + 20.0f)
                        {
                            player.worldLeftTop.posY = trampolineY - player.size.posY;
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
                jumpCount = 0;
            }

            //===========================
            //コイン判定
            //===========================
            for (int y = 0; y < kCoinMapHeight; y++)
            {
                for (int x = 0; x < kCoinMapWidth; x++)
                {
                    if (coinMap[y][x] != 1)
                    {
                        continue;
                    }

                    float coinX = static_cast<float>(x * kCoinMapSize);
                    float coinY = static_cast<float>(y * kCoinMapSize);

                    float coinRight = coinX + kCoinSize;
                    float coinBottom = coinY + kCoinSize;

                    float playerRight = player.worldLeftTop.posX + player.size.posX;
                    float playerBottom = player.worldLeftTop.posY + player.size.posY;

                    if (player.worldLeftTop.posX < coinRight &&
                        playerRight > coinX &&
                        player.worldLeftTop.posY < coinBottom &&
                        playerBottom > coinY)
                    {
                        coinMap[y][x] = 0;
                        coinCount++;
                        Novice::PlayAudio(coinSound, false, 0.8f);
                    }
                }
            }

            //===========================
            //スクロール処理
            //===========================
            scrollValue = player.worldLeftTop.posX + player.size.posX - scrollStartPositionX;

            if (scrollValue >= kBackgroundWidth)
            {
                scrollValue -= kBackgroundWidth;
            }

            //============================================
            //チュートリアル画面描画
            //============================================
            const float kBackgroundSetWidth = 1280.0f * 4.0f;

            int backgroundSet =
                static_cast<int>(
                    player.worldLeftTop.posX / kBackgroundSetWidth
                    );

            for (int set = 0; set < 2; set++)
            {
                float currentSetX =
                    static_cast<float>(backgroundSet + set) *
                    kBackgroundSetWidth;

                for (int i = 0; i < 4; i++)
                {
                    float backgroundWorldX =
                        currentSetX + 1280.0f * i;

                    float screenX =
                        backgroundWorldX -
                        player.worldLeftTop.posX +
                        scrollStartPositionX;

                    Novice::DrawSprite(
                        static_cast<int>(screenX),
                        0,
                        backgroundTexture[i],
                        1.0f,
                        1.0f,
                        0.0f,
                        WHITE
                    );
                }
            }

            int stageSet =
                static_cast<int>(player.worldLeftTop.posX / kStageWidth);

            for (int i = 0; i < kTrampolineCount; i++)
            {
                float trampolineWorldX =
                    trampolinePositions[i].posX +
                    stageSet * kStageWidth;

                int screenX =
                    static_cast<int>(
                        trampolineWorldX -
                        player.worldLeftTop.posX +
                        scrollStartPositionX
                        );

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

            for (int y = 0; y < kCoinMapHeight; y++)
            {
                for (int x = 0; x < kCoinMapWidth; x++)
                {
                    if (coinMap[y][x] != 1)
                    {
                        continue;
                    }

                    float coinX = static_cast<float>(x * kCoinMapSize) + stageSet * kStageWidth;
                    float coinY = static_cast<float>(y * kCoinMapSize);

                    int screenX = static_cast<int>(coinX - player.worldLeftTop.posX + scrollStartPositionX);
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

            Novice::ScreenPrintf(
                20,
                20,
                "COIN : %d",
                coinCount
            );
            break;
        }
        }

        //==========================================
        // シーン切り替えフェードイン
        //==========================================
        if (scene != Title && gameFadeTimer < 60 && !isTransitioningToBonus && scene != BonusStage)
        {
            int alpha = 255 - gameFadeTimer * 255 / 60;

            unsigned int fadeColor =
                static_cast<unsigned int>(alpha);

            Novice::DrawBox(
                0, 0, 1280, 720,
                0.0f,
                fadeColor,
                kFillModeSolid
            );

            gameFadeTimer++;
        }

        Novice::EndFrame();

        if (preKeys[DIK_ESCAPE] == 0 &&
            keys[DIK_ESCAPE] != 0)
        {
            break;
        }
    }

    Novice::Finalize();
    return 0;
}