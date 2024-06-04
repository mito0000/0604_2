#include "DxLib.h"
#include <vector>
#include <time.h>

enum Direction { UP, DOWN, LEFT, RIGHT };

class OneStrokeGame {
public:
    OneStrokeGame(int width, int height) : width(width), height(height), gameOver(false), moveDelay(0) {
        OneStrokeGame::board = std::vector<std::vector<char>>(height, std::vector<char>(width, ' '));
        srand(static_cast<unsigned>(time(nullptr)));
        spawnRandomPoint();
    }

    void drawBoard() const {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                DrawFormatString(j * 40, i * 40, GetColor(0, 0, 0), "%c", board[i][j]);
            }
        }
    }



    void move(Direction direction) {
        if (!gameOver) {
            if (moveDelay <= 0) {
                switch (direction) {
                case UP:
                    moveHead(-1, 0);
                    break;
                case DOWN:
                    moveHead(1, 0);
                    break;
                case LEFT:
                    moveHead(0, -1);
                    break;
                case RIGHT:
                    moveHead(0, 1);
                    break;
                }

                if (checkCollision()) {
                    DrawFormatString(140, 200, GetColor(255, 0, 0), "Game Over!");
                    gameOver = true;
                }
                else if (checkPoint()) {
                    spawnRandomPoint();
                }

                moveDelay = 10; // 適切な値に調整してください
            }
            else {
                moveDelay--;
            }
        }
    }

    bool isGameOver() const {
        return gameOver;
    }

    std::pair<int, int> GetHead() const {
        return head;
    }

private:
    int width;
    int height;
    std::vector<std::vector<char>> board;
    std::pair<int, int> head{ height / 2, width / 2 };
    std::pair<int, int> point;
    bool gameOver;
    int moveDelay;

    void moveHead(int dy, int dx) {
        // ヘッドの移動前の位置を一時的に保存
        int prevY = head.first;
        int prevX = head.second;

        // ヘッドを移動
        head.first += dy;
        head.second += dx;

        // Check boundaries
        if (head.first < 0) head.first = 0;
        if (head.first >= height) head.first = height - 1;
        if (head.second < 0) head.second = 0;
        if (head.second >= width) head.second = width - 1;

        // ヘッドが * マークの位置に移動しようとした場合は移動をキャンセル
        if (board[head.first][head.second] == '*') {
            head.first = prevY;
            head.second = prevX;
            return;
        }

        // ヘッドを移動
        board[prevY][prevX] = '*';

        // Clear previous tail
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (board[i][j] != '*' && board[i][j] != 'o') {
                    board[i][j] = ' ';
                }
            }
        }
    }

    bool checkCollision() const {
        return board[head.first][head.second] == 'o';
    }

    bool checkPoint() const {
        return head == point;
    }

    void spawnRandomPoint() {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while (board[y][x] == '*' || board[y][x] == 'o');

        point = std::make_pair(y, x);
        board[y][x] = 'o';
    }
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // DxLibの初期化
    if (DxLib_Init() == -1) {
        return -1;
    }

    // 画面の大きさを設定（幅: 800ピクセル、高さ: 600ピクセル）
    SetGraphMode(400,400, 32);

    // ウィンドウのタイトルを設定
    SetMainWindowText("My DxLib Window");

    // フォントサイズを設定（ここでは24ピクセル）
    int fontSize = 24;
    ChangeFont("メイリオ", DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
    SetFontSize(fontSize);

    // 画像の読み込み
    int backgroundImage = LoadGraph("img/backimg.png");
    int characterImage = LoadGraph("img/player.png");

    // ゲームオブジェクトの生成
    OneStrokeGame game(10, 10);

    // メインループ
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
        // ゲームの更新処理
        if (!game.isGameOver()) {
            ClearDrawScreen();

            // 背景画像の描画
            DrawGraph(0, 0, backgroundImage, FALSE);

            // ゲームオブジェクトの描画
            game.drawBoard();

            // キャラクター画像の描画
            DrawGraph(game.GetHead().second * 40, game.GetHead().first * 40, characterImage, TRUE);

            Direction direction = static_cast<Direction>(-1);
            if (
                CheckHitKey(KEY_INPUT_UP) && direction != DOWN) direction = UP;
            else if (CheckHitKey(KEY_INPUT_DOWN) && direction != UP) direction = DOWN;
            else if (CheckHitKey(KEY_INPUT_LEFT) && direction != RIGHT) direction = LEFT;
            else if (CheckHitKey(KEY_INPUT_RIGHT) && direction != LEFT) direction = RIGHT;
            if (direction != -1) {
                game.move(direction);
            }
        }

        // 画面を更新
        ScreenFlip();
    }

    // DxLibの終了
    DxLib_End();

    return 0;
}
