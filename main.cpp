#include <Novice.h>
#include <string.h>
#include <math.h>

const char kWindowTitle[] = "LC1D_12_タカハシ";

enum GameShene {
    Title,
    Game,
    tutorial,
    BonusStage
};

struct Vector2 {
    float posX = 0.0f;
    float posY = 0.0f;
};

struct Box {
    Vector2 worldLeftTop{};
    Vector2 size{};
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    unsigned int color = WHITE;
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

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
    const float kMaxScroll = kStageWidth - 1280.0f;
    const float kBackgroundWidth = 1280.0f * 4.0f;

    float savedPlayerX = 0.0f;

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
    int batteryTexture =
        Novice::LoadTexture("./Resources/images/battery.png");

    int playerTexture =
        Novice::LoadTexture("./Resources/images/pengin2.png");

    int playerJumpTexture =
        Novice::LoadTexture("./Resources/images/jump.png");

    int universepengin =
        Novice::LoadTexture("./Resources/images/universepengin.png");

    int backgroundTexture =
        Novice::LoadTexture("./Resources/images/iceBackGround.png");

    int universeTexture =
        Novice::LoadTexture("./Resources/images/universe.png");

    int universe2Texture =
        Novice::LoadTexture("./Resources/images/universe2.png");

    int titleTexture1 =
        Novice::LoadTexture("./Resources/images/penginno.png");

    int titleTexture2 =
        Novice::LoadTexture("./Resources/images/harasuberi.png");

    int menuTextures[3] = {
        Novice::LoadTexture("./Resources/images/space.png"),
        Novice::LoadTexture("./Resources/images/tutorial.png"),
        Novice::LoadTexture("./Resources/images/credit.png")
    };

    int ruleTexture =
        Novice::LoadTexture("./Resources/images/ruleFormat.png");

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
    int kTitleHandle =
        Novice::LoadAudio("./Resources/bgm/title.MP3");

    int kTutorialHandle =
        Novice::LoadAudio("./Resources/bgm/tutorial.MP3");

    int titlePlayHandle = -1;

    if (kTitleHandle != -1) {
        titlePlayHandle =
            Novice::PlayAudio(kTitleHandle, true, 0.5f);
    }

    //==========================================
    // トランポリン
    //==========================================
    Vector2 trampolinePositions[] = {
        {500.0f, 500.0f},
        {1000.0f, 500.0f},
        {1500.0f, 500.0f},
        {2200.0f, 500.0f}
    };

    const int kTrampolineCount =
        sizeof(trampolinePositions) / sizeof(trampolinePositions[0]);

    const float kTrampolineBouncePower = 40.0f;
    const int kTrampolineWidth = 32;

    //==========================================
    // コイン
    //==========================================
    int coinMap[6][100] = {};

    const int kCoinMapHeight = 6;
    const int kCoinMapWidth = 100;
    const int kCoinMapSize = 64;
    const float kCoinSize = 64.0f;

    // コイン配置
    const int coinPositions[][2] = {
        {5,5}, {6,4}, {6,3}, {10,3},
        {17,3}, {18,3}, {26,3}, {27,4},
        {38,2}, {39,2}, {40,2},
        {50,2}, {52,2}, {60,1}
    };

    for (int i = 0;
        i < static_cast<int>(
            sizeof(coinPositions) / sizeof(coinPositions[0])
            ); i++) {

        int x = coinPositions[i][0];
        int y = coinPositions[i][1];

        coinMap[y][x] = 1;
    }

    const int kMaxCoinStageSets = 100;

    static bool coinCollectedByStage[kMaxCoinStageSets]
        [kCoinMapHeight][kCoinMapWidth] = {};

    int coinCount = 0;

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

            if (!isTransitioningToBonus) {

                player.worldLeftTop.posX += kMoveSpeed;

                if (keys[DIK_A]) {
                    player.worldLeftTop.posX -= 30.0f;
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

            // 重力
            if (isJumping) {
                playerVelocityY += kGravity;
                player.worldLeftTop.posY += playerVelocityY;
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

            // トランポリン
            for (int i = 0; i < kTrampolineCount; i++) {

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
                !isTransitioningToBonus) {

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

            // コイン判定
            for (int y = 0; y < kCoinMapHeight; y++) {
                for (int x = 0; x < kCoinMapWidth; x++) {

                    if (coinMap[y][x] != 1) {
                        continue;
                    }

                    float coinX = static_cast<float>(x * kCoinMapSize);
                    float coinY = static_cast<float>(y * kCoinMapSize);

                    if (player.worldLeftTop.posX < coinX + kCoinSize &&
                        player.worldLeftTop.posX + player.size.posX > coinX &&
                        player.worldLeftTop.posY < coinY + kCoinSize &&
                        player.worldLeftTop.posY + player.size.posY > coinY) {

                        coinMap[y][x] = 0;
                        coinCount++;

                        if (coinSound != -1) {
                            Novice::PlayAudio(
                                coinSound, false, 0.8f
                            );
                        }
                    }
                }
            }

            // 画面端
            if (player.worldLeftTop.posX < 0.0f) {
                player.worldLeftTop.posX = 0.0f;
            }

            if (player.worldLeftTop.posX >
                kStageWidth - player.size.posX) {

                player.worldLeftTop.posX =
                    kStageWidth - player.size.posX;
            }

            // スクロール
            scrollValue =
                player.worldLeftTop.posX +
                player.size.posX - scrollStartPositionX;

            if (scrollValue < 0.0f) {
                scrollValue = 0.0f;
            }

            if (scrollValue > kMaxScroll) {
                scrollValue = kMaxScroll;
            }

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
            for (int y = 0; y < kCoinMapHeight; y++) {
                for (int x = 0; x < kCoinMapWidth; x++) {

                    if (coinMap[y][x] == 1 && coin != -1) {
                        Novice::DrawSprite(
                            static_cast<int>(
                                x * kCoinMapSize - scrollValue
                                ),
                            y * kCoinMapSize,
                            coin,
                            1.0f, 1.0f, 0.0f, WHITE
                        );
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

                    //==================================
                    // ★宇宙に入ったらfireを常時再生
                    //==================================
                    if (firePlayHandle != -1) {
                        Novice::StopAudio(firePlayHandle);
                        firePlayHandle = -1;
                    }

                    if (fireSound != -1) {
                        firePlayHandle =
                            Novice::PlayAudio(
                                fireSound,
                                true,
                                0.2f
                            );
                    }
                }
            }

            break;
        }

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

            player.worldLeftTop.posY += playerVelocityY;

            // 天井判定
            if (player.worldLeftTop.posY < 0.0f) {

                player.worldLeftTop.posY = 0.0f;
                playerVelocityY = 0.0f;
            }

            // 燃料がある間の地面判定
            if (!isOutOfFuel &&
                player.worldLeftTop.posY >= kGroundY) {

                player.worldLeftTop.posY = kGroundY;
                playerVelocityY = 0.0f;
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

            const float kLoopWidth = 2560.0f;

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
            // 宇宙ペンギン描画
            //==========================================
            if (universepengin != -1) {

                Novice::DrawSpriteRect(
                    static_cast<int>(
                        player.worldLeftTop.posX - scrollValue
                        ),
                    static_cast<int>(player.worldLeftTop.posY),
                    universeFrame * 128,
                    0,
                    128, 128,
                    universepengin,
                    1.0f / static_cast<float>(kUniverseFrameCount),
                    1.0f,
                    0.0f, WHITE
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

                    // ★通常ステージに戻る時も停止
                    if (firePlayHandle != -1) {

                        Novice::StopAudio(firePlayHandle);
                        firePlayHandle = -1;
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
                       // チュートリアル
                       //==========================================
        case tutorial: {

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
                set <= coinCollisionStageSet + 1; set++) {

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

            int stageSet =
                static_cast<int>(
                    player.worldLeftTop.posX / kStageWidth
                    );

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
            scene != BonusStage) {

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

    // 終了時fireを停止
    if (firePlayHandle != -1) {
        Novice::StopAudio(firePlayHandle);
    }

    Novice::Finalize();
    return 0;
}