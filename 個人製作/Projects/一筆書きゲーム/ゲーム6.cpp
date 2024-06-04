#include "DxLib.h"
#include <vector>
#include <time.h>

const int boardX = 10;
const int boardY = 9;
const int ClearCount = (boardX * boardY) - 1;

//�}�b�v�`�b�v
int mapchip[3];

enum Direction { UP, DOWN, LEFT, RIGHT };

//�Q�[���̏��
enum Status
{
    START, PLAY
};
//�Q�[���̏�Ԃ�ύX
int Sta = START;

class OneStrokeGame {
public:
    OneStrokeGame(int width, int height) : width(width), height(height), gameOver(false), gameClear(false),moveDelay(0) {
        OneStrokeGame::board = std::vector<std::vector<char>>(height, std::vector<char>(width, 0));
        srand(static_cast<unsigned>(time(nullptr)));
        spawnRandomPoint();
    }

    void drawBoard() const {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int cell = board[i][j];
                DrawGraph(j * 40, i * 40, mapchip[cell], TRUE);
            }
        }
    }

    void move(Direction direction) {

        if (!gameOver && !gameClear) {
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
                    if (ClearCount == count)
                    {
                        DrawFormatString(150, 185, GetColor(255, 255, 0), "CLEAR!");
                        //DrawFormatString(135, 150, GetColor(255, 255, 0), "%d", count);
                        gameClear = true;
                    }
                    else
                    {
                        DrawFormatString(135, 185, GetColor(0, 255, 0), "GAME OVER!");
                        //DrawFormatString(135, 150, GetColor(0, 255, 0), "%d", count);
                        gameOver = true;
                    }
                }
                else if (checkPoint()) {
                    spawnRandomPoint();
                }

                moveDelay = 15; //�ړ����x
            }
            else {
                moveDelay--;
            }
        }
    }

    bool isGameOver() const {
        return gameOver;
    }

    bool isGameClear() const
    {
        return gameClear;
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
    bool gameClear;
    int moveDelay;

    int count = 0;

    //player�̈ړ�
    void moveHead(int dy, int dx) {
        // �w�b�h�̈ړ��O�̈ʒu���ꎞ�I�ɕۑ�
        int prevY = head.first;
        int prevX = head.second;

        // �w�b�h���ړ�
        head.first += dy;
        head.second += dx;

        // Check boundaries
        if (head.first < 0) head.first = 0, count -= 1;
        if (head.first >= height) head.first = height - 1, count -= 1;
        if (head.second < 0) head.second = 0, count -= 1;
        if (head.second >= width) head.second = width - 1, count -= 1;

        // �w�b�h�����ɒʂ��Ă���ʒu�Ɉړ����悤�Ƃ����ꍇ�͈ړ����L�����Z��
        if (board[head.first][head.second] == 1) {
            head.first = prevY;
            head.second = prevX;
            return;
        }

        // �w�b�h���ړ�
        board[prevY][prevX] = 1;
        count++;

        // Clear previous tail
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (board[i][j] != 1 && board[i][j] != 2)
                {
                    board[i][j] = 0;
                }
            }
        }
    }

    bool checkCollision() const {
        return board[head.first][head.second] == 2;
    }

    bool checkPoint() const {
        return head == point;
    }

    void spawnRandomPoint() {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while (board[y][x] == 1 || board[y][x] == 2);

        point = std::make_pair(y, x);
        board[y][x] = 2;
    }
};


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // DxLib�̏�����
    if (DxLib_Init() == -1) {
        return -1;
    }

    // ��ʂ̑傫����ݒ�i��: 400�s�N�Z���A����: 360�s�N�Z���j
    SetGraphMode(400, 360, 32);

    // �E�B���h�E�̃^�C�g����ݒ�
    SetMainWindowText("My DxLib Window");

    //�}�b�v�`�b�v�̓ǂݍ���
    LoadDivGraph("img/mapchip.png", 3, 3, 1, 40, 40, mapchip);

    // �t�H���g�T�C�Y��ݒ�i�����ł�24�s�N�Z���j
    int fontSize = 24;
    ChangeFont("���C���I", DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
    SetFontSize(fontSize);


    // �摜�̓ǂݍ���
    //int backgroundImage = LoadGraph("img/backimg.png");
    int characterImage = LoadGraph("img/player2.png");

    // �Q�[���I�u�W�F�N�g�̐���
    //�{�[�h�̃}�X��
    OneStrokeGame game(boardX, boardY);

    // ���C�����[�v
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
        // �Q�[���̍X�V����
        if (!game.isGameOver() && !game.isGameClear()) {

            // �w�i�摜�̕`��
            //DrawGraph(0, 0, backgroundImage, FALSE);

            // �Q�[���I�u�W�F�N�g�̕`��
            game.drawBoard();

            if (Sta == START)
            {
                SetFontSize(fontSize);
                DrawString(60, 185, "STRAT PUSH             KEY", GetColor(0, 0, 0));
                DrawString(235, 185, "SPACE", GetColor(255, 255, 0));

                if (CheckHitKey(KEY_INPUT_SPACE))
                {
                    //�Q�[����ԕω�
                    Sta = PLAY;

                    //��ʂ̏�����
                    ClearDrawScreen();
                }

            }

            if (Sta == PLAY)
            {
                // �L�����N�^�[�摜�̕`��
                DrawGraph(game.GetHead().second * 40, game.GetHead().first * 40, characterImage, TRUE);


                Direction direction = static_cast<Direction>(-1);
                if (CheckHitKey(KEY_INPUT_UP) && direction != DOWN) direction = UP;
                else if (CheckHitKey(KEY_INPUT_DOWN) && direction != UP) direction = DOWN;
                else if (CheckHitKey(KEY_INPUT_LEFT) && direction != RIGHT) direction = LEFT;
                else if (CheckHitKey(KEY_INPUT_RIGHT) && direction != LEFT) direction = RIGHT;
                if (direction != -1) {
                    game.move(direction);
                }

            }

        }

        // ��ʂ��X�V
        ScreenFlip();

        //if (CheckHitKey(KEY_INPUT_RETURN) && (game.isGameOver() || game.isGameClear())) 
            if(CheckHitKey(KEY_INPUT_RETURN)){
            // Enter�L�[��������A�Q�[���I�[�o�[�܂��̓Q�[���N���A�̏�ԂȂ�START�ɖ߂�
            Sta = START;
            game = OneStrokeGame(boardX, boardY); // �Q�[�������Z�b�g
            ClearDrawScreen();
            }

    }

    while (CheckHitKeyAll() == 0)
    {
        // ���b�Z�[�W���[�v�ɑ��鏈��������
        if (ProcessMessage() == -1)
        {
            break;        // �G���[���N�����烋�[�v�𔲂���
        }
    }

    // DxLib�̏I��
    DxLib_End();

    return 0;
}