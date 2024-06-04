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
                DrawFormatString(j * 40, i * 40, GetColor(255, 255, 255), "%c", board[i][j]);
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
                    DrawFormatString(0, height * 40, GetColor(255, 0, 0), "Game Over!");
                    gameOver = true;
                }
                else if (checkPoint()) {
                    spawnRandomPoint();
                }

                moveDelay = 10; // �K�؂Ȓl�ɒ������Ă�������
            }
            else {
                moveDelay--;
            }
        }
    }

    bool isGameOver() const {
        return gameOver;
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
        board[head.first][head.second] = '*';
        head.first += dy;
        head.second += dx;

        // Check boundaries
        if (head.first < 0) head.first = height - 1;
        if (head.first >= height) head.first = 0;
        if (head.second < 0) head.second = width - 1;
        if (head.second >= width) head.second = 0;

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
    // DxLib�̏�����
    if (DxLib_Init() == -1) {
        return -1;
    }

    // �Q�[���I�u�W�F�N�g�̐���
    OneStrokeGame game(10, 10);

    // �L�[���͂̏�Ԃ�ۑ�����ϐ�
    Direction lastDirection = UP; // �C�ӂ̏����l

    // ���C�����[�v
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
        // �Q�[���̍X�V����
        if (!game.isGameOver()) {
            ClearDrawScreen();
            game.drawBoard();

            // �L�[���͂̎擾
            Direction direction = lastDirection; // �O��̓��͂������l�Ƃ���

            if (CheckHitKey(KEY_INPUT_UP) && direction != DOWN) direction = UP;
            else if (CheckHitKey(KEY_INPUT_DOWN) && direction != UP) direction = DOWN;
            else if (CheckHitKey(KEY_INPUT_LEFT) && direction != RIGHT) direction = LEFT;
            else if (CheckHitKey(KEY_INPUT_RIGHT) && direction != LEFT) direction = RIGHT;

            // �L�[���͂��ύX���ꂽ�ꍇ�A�������X�V
            if (direction != lastDirection) {
                game.move(direction);
                lastDirection = direction;
            }

            // �L�[�������ꂽ�ꍇ�AlastDirection �����Z�b�g
            if (!CheckHitKey(KEY_INPUT_UP) && !CheckHitKey(KEY_INPUT_DOWN) &&
                !CheckHitKey(KEY_INPUT_LEFT) && !CheckHitKey(KEY_INPUT_RIGHT)) {
                lastDirection = UP; // �C�ӂ̏����l
            }
        }

        // ��ʂ��X�V
        ScreenFlip();
    }

    // DxLib�̏I��
    DxLib_End();

    return 0;
}