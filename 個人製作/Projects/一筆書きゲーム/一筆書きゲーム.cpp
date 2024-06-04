#include "DxLib.h"
#include <vector>
#include <time.h>

#define KEY_CNT 256

const int boardX = 10;
const int boardY = 9;
const int ClearCount = (boardX * boardY) - 1;

//�}�b�v�`�b�v
int mapchip[3];

enum Direction { UP = 1, DOWN, LEFT, RIGHT };

//�Q�[���̏��
enum Status
{
    START, PLAY
};
//�Q�[���̏�Ԃ�ύX
int Sta = START;

class OneStrokeGame {
public:
    OneStrokeGame(int width, int height) : directionHistory(), width(width), height(height), gameOver(false), gameClear(false), Delete(false), countDecremented(false),moveDelay(0) {
        OneStrokeGame::board = std::vector<std::vector<char>>(height, std::vector<char>(width, 0));
        srand(static_cast<unsigned>(time(nullptr)));
        //head�̏����ʒu�������_���ɐݒ�
        head = std::make_pair(rand() % height, rand() % width);

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

                // ���݂̕�����ۑ�
                //prevDirection = direction;

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
                        DrawFormatString(150, 185, GetColor(0, 255, 0), "CLEAR!");
                        //DrawFormatString(135, 150, GetColor(255, 255, 0), "%d", count);
                        gameClear = true;
                    }
                    else
                    {
                        DrawFormatString(135, 185, GetColor(255, 255, 0), "GAME OVER!");
                        //DrawFormatString(135, 150, GetColor(0, 255, 0), "%d", count);
                        gameOver = true;
                    }
                }
                else if (checkPoint()) {
                    spawnRandomPoint();
                }

                moveDelay = 0; //�ړ����x
            }
            else {
                moveDelay--;
            }
        }

        if (board[head.first][head.second] != 1 && Delete ==false)
        {
        //�������q�X�g���[�ɒǉ�
        directionHistory.push_back(direction);
        }
    }

    //���O�̈ړ��������擾
    Direction getPreviousDirection(Direction direction)
    {
        //�q�X�g���[����łȂ��ꍇ�A�Ō�̗v�f���擾
        if (!directionHistory.empty() && Delete == false)
        {
            return directionHistory.back();
        }
        //�q�X�g���[����̏ꍇ�A�f�t�H���g�̒l��Ԃ�
        return UP;
    }

    int GetMoveDelay() const {
        return moveDelay;
    }

    void SetMoveDelay(int newDelay) {
        moveDelay = newDelay;
    }

    void returnprev(Direction direction)
    {
        if (!directionHistory.empty())
        {
            //�q�X�g���[��������O�̕������擾
            Direction prevDirection = directionHistory.back();
            directionHistory.pop_back();

        int prevY = head.first;
        int prevX = head.second;

        // ��x�ʂ����ʒu���N���A
        board[prevY][prevX] = 0;

        // �ۑ����Ă����������Ŗ߂�
        if (prevDirection == UP) {
            moveHead(1, 0);
        }
        else if (prevDirection == DOWN) {
            moveHead(-1, 0);
        }
        else if (prevDirection == LEFT) {
            moveHead(0, 1);
        }
        else if (prevDirection == RIGHT) {
            moveHead(0, -1);
        }
        count--;

        //moveDelay �̒l��ύX
        SetMoveDelay(0);
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

    char keybuf[KEY_CNT];		//�L�[�o�b�t�@ (���t���[��)
    char keybuf_old[KEY_CNT];	//1�O�̃L�[�o�b�t�@(�O�t���[��) 
    char key_down[KEY_CNT];		//�����ꂽ�u�� 
    char key_up[KEY_CNT];		//�����ꂽ�u��

    void Update_Key(void)		//�L�[�̓��͏�� 
    {
        int i;
        for (i = 0; i < KEY_CNT; i++)
        {
            keybuf_old[i] = keybuf[i];
        }		//1�O�̃L�[�̏�Ԃ�ۑ�

        GetHitKeyStateAll(keybuf);//�S�ẴL�[�̏�Ԃ��擾 

        for (i = 0; i < KEY_CNT; i++)
        {
            int key_xor = keybuf[i] ^ keybuf_old[i];//�O�t���[���ƌ��t���[����xor 
            key_down[i] = key_xor & keybuf[i];//�����ꂽ�u�� = (���t���[����key_xor��AND) 
            key_up[i] = key_xor & keybuf_old[i];//�����ꂽ�u�� = (�O�t���[����key_xor��AND) 
        }
    }

private:
    int width;
    int height;
    std::vector<std::vector<char>> board;
    std::pair<int, int> head{ height / 2, width / 2 };//player�̏����ʒu
    std::pair<int, int> point;//�S�[���̈ʒu
    bool gameOver;
    bool gameClear;
    int moveDelay;

    bool Delete;//�ړ����L�����Z�����ꂽ�Ƃ��Ƀf�[�^��ۑ����Ȃ�
    bool countDecremented; // count��1�x�������Z���邽�߂̃t���O

    int count = 0;

    std::vector<Direction> directionHistory; // �ړ������̃q�X�g���[

    //player�̈ړ�
    void moveHead(int dy, int dx) {
        // �w�b�h�̈ړ��O�̈ʒu���ꎞ�I�ɕۑ�
        int prevY = head.first;
        int prevX = head.second;

        // �w�b�h���ړ�
        head.first += dy;
        head.second += dx;

        // Check boundaries
        //���������
        if (head.first < 0)
        {
            head.first = 0;
            if (!countDecremented)
            {
                count + 0;
                countDecremented = true;
            }
        }
        //����������
        if (head.first >= height)
        {
            head.first = height - 1;
            if (!countDecremented)
            {
                count + 0;
                countDecremented = true;
            }
        }
        //����������
        if (head.second < 0)
        {
            head.second = 0;
            if (!countDecremented)
            {
                count + 0;
                countDecremented = true;
            }
        }
        //�E��������
        if (head.second >= width)
        {
            head.second = width - 1;
            if (!countDecremented)
            {
                count + 0;
                countDecremented = true;
            }
        }

        // R�L�[��������Ă���ꍇ�͏������X�L�b�v
        if (key_down[KEY_INPUT_R]) {
            return;
        }
        else
        {
            // �w�b�h�����ɒʂ��Ă���ʒu�Ɉړ����悤�Ƃ����ꍇ�͈ړ����L�����Z��
            if (board[head.first][head.second] == 1) {
                head.first = prevY;
                head.second = prevX;

                Delete = true;

                return;
            }
        }

        // �w�b�h���ړ�                 
        if (board[prevY][prevX] = 1)
        {
            Delete = false;//�ړ������ۑ�
            if (head.first != prevY || head.second != prevX)
            {
                count++;
            }
        }

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

    OneStrokeGame* onegame;

    // ��ʂ̑傫����ݒ�i��: 400�s�N�Z���A����: 360�s�N�Z���j
    SetGraphMode(400, 400, 32);

    // �E�B���h�E�̃^�C�g����ݒ�
    SetMainWindowText("My DxLib Window");

    //�}�b�v�`�b�v�̓ǂݍ���
    LoadDivGraph("img/mapchip.png", 3, 3, 1, 40, 40, mapchip);

    // �t�H���g�T�C�Y��ݒ�i�����ł�24�s�N�Z���j
    int fontSize = 20;
    ChangeFont("Gill Sans Ultra Bold", DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
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

                //���^�[��
                DrawString(20, 155, "RETURN PUSH         KEY", GetColor(255, 255, 255));
                DrawString(205, 155, "<R>", GetColor(0, 255, 0));

                //�X�^�[�g
                DrawString(20, 185, "STRAT PUSH                  KEY", GetColor(255, 255, 255));
                DrawString(180, 185, "<SPACE>", GetColor(0, 255, 0));

                //���Z�b�g
                DrawString(20, 215, "RETRAY PUSH                  KEY", GetColor(255, 255, 255));
                DrawString(205, 215, "<ENTER>", GetColor(0, 255, 0));

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
                //���^�[��
                DrawString(40, 370, "RETURN PUSH        KEY", GetColor(255, 255, 255));
                DrawString(220, 370, "<R>", GetColor(0, 255, 0));

                // �L�����N�^�[�摜�̕`��
                DrawGraph(game.GetHead().second * 40, game.GetHead().first * 40, characterImage, TRUE);

                game.Update_Key();

                Direction direction = static_cast<Direction>(-1);

                if (!game.key_down[KEY_INPUT_R])
                {
                    if ((game.key_down[KEY_INPUT_UP])) {
                        direction = UP;
                    }
                    else if ((game.key_down[KEY_INPUT_DOWN])) {
                        direction = DOWN;
                    }
                    else if ((game.key_down[KEY_INPUT_LEFT])) {
                        direction = LEFT;
                    }
                    else if ((game.key_down[KEY_INPUT_RIGHT])) {
                        direction = RIGHT;
                    }
                    if (direction != -1) {
                        game.move(direction);
                    }
                }


                if (game.key_down[KEY_INPUT_R]) {
                    game.returnprev(direction);
                    if (game.key_up[KEY_INPUT_R]) { break; }
                }

            }

        }

        // ��ʂ��X�V
        ScreenFlip();

        //if (CheckHitKey(KEY_INPUT_RETURN) && (game.isGameOver() || game.isGameClear())) 
        if (CheckHitKey(KEY_INPUT_RETURN)) {
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