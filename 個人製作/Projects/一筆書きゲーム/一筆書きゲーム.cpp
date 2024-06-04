#include "DxLib.h"
#include <vector>
#include <time.h>

#define KEY_CNT 256

const int boardX = 10;
const int boardY = 9;
const int ClearCount = (boardX * boardY) - 1;

//マップチップ
int mapchip[3];

enum Direction { UP = 1, DOWN, LEFT, RIGHT };

//ゲームの状態
enum Status
{
    START, PLAY
};
//ゲームの状態を変更
int Sta = START;

class OneStrokeGame {
public:
    OneStrokeGame(int width, int height) : directionHistory(), width(width), height(height), gameOver(false), gameClear(false), Delete(false), countDecremented(false),moveDelay(0) {
        OneStrokeGame::board = std::vector<std::vector<char>>(height, std::vector<char>(width, 0));
        srand(static_cast<unsigned>(time(nullptr)));
        //headの初期位置をランダムに設定
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

                // 現在の方向を保存
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

                moveDelay = 0; //移動速度
            }
            else {
                moveDelay--;
            }
        }

        if (board[head.first][head.second] != 1 && Delete ==false)
        {
        //方向をヒストリーに追加
        directionHistory.push_back(direction);
        }
    }

    //一手前の移動方向を取得
    Direction getPreviousDirection(Direction direction)
    {
        //ヒストリーが空でない場合、最後の要素を取得
        if (!directionHistory.empty() && Delete == false)
        {
            return directionHistory.back();
        }
        //ヒストリーが空の場合、デフォルトの値を返す
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
            //ヒストリー内から一手前の方向を取得
            Direction prevDirection = directionHistory.back();
            directionHistory.pop_back();

        int prevY = head.first;
        int prevX = head.second;

        // 一度通った位置をクリア
        board[prevY][prevX] = 0;

        // 保存しておいた方向で戻す
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

        //moveDelay の値を変更
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

    char keybuf[KEY_CNT];		//キーバッファ (現フレーム)
    char keybuf_old[KEY_CNT];	//1つ前のキーバッファ(前フレーム) 
    char key_down[KEY_CNT];		//押された瞬間 
    char key_up[KEY_CNT];		//離された瞬間

    void Update_Key(void)		//キーの入力状態 
    {
        int i;
        for (i = 0; i < KEY_CNT; i++)
        {
            keybuf_old[i] = keybuf[i];
        }		//1つ前のキーの状態を保存

        GetHitKeyStateAll(keybuf);//全てのキーの状態を取得 

        for (i = 0; i < KEY_CNT; i++)
        {
            int key_xor = keybuf[i] ^ keybuf_old[i];//前フレームと現フレームのxor 
            key_down[i] = key_xor & keybuf[i];//押された瞬間 = (現フレームとkey_xorのAND) 
            key_up[i] = key_xor & keybuf_old[i];//離された瞬間 = (前フレームとkey_xorのAND) 
        }
    }

private:
    int width;
    int height;
    std::vector<std::vector<char>> board;
    std::pair<int, int> head{ height / 2, width / 2 };//playerの初期位置
    std::pair<int, int> point;//ゴールの位置
    bool gameOver;
    bool gameClear;
    int moveDelay;

    bool Delete;//移動をキャンセルされたときにデータを保存しない
    bool countDecremented; // countを1度だけ減算するためのフラグ

    int count = 0;

    std::vector<Direction> directionHistory; // 移動方向のヒストリー

    //playerの移動
    void moveHead(int dy, int dx) {
        // ヘッドの移動前の位置を一時的に保存
        int prevY = head.first;
        int prevX = head.second;

        // ヘッドを移動
        head.first += dy;
        head.second += dx;

        // Check boundaries
        //上方向制限
        if (head.first < 0)
        {
            head.first = 0;
            if (!countDecremented)
            {
                count + 0;
                countDecremented = true;
            }
        }
        //下方向制限
        if (head.first >= height)
        {
            head.first = height - 1;
            if (!countDecremented)
            {
                count + 0;
                countDecremented = true;
            }
        }
        //左方向制限
        if (head.second < 0)
        {
            head.second = 0;
            if (!countDecremented)
            {
                count + 0;
                countDecremented = true;
            }
        }
        //右方向制限
        if (head.second >= width)
        {
            head.second = width - 1;
            if (!countDecremented)
            {
                count + 0;
                countDecremented = true;
            }
        }

        // Rキーが押されている場合は処理をスキップ
        if (key_down[KEY_INPUT_R]) {
            return;
        }
        else
        {
            // ヘッドが既に通っている位置に移動しようとした場合は移動をキャンセル
            if (board[head.first][head.second] == 1) {
                head.first = prevY;
                head.second = prevX;

                Delete = true;

                return;
            }
        }

        // ヘッドを移動                 
        if (board[prevY][prevX] = 1)
        {
            Delete = false;//移動方向保存
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
    // DxLibの初期化
    if (DxLib_Init() == -1) {
        return -1;
    }

    OneStrokeGame* onegame;

    // 画面の大きさを設定（幅: 400ピクセル、高さ: 360ピクセル）
    SetGraphMode(400, 400, 32);

    // ウィンドウのタイトルを設定
    SetMainWindowText("My DxLib Window");

    //マップチップの読み込み
    LoadDivGraph("img/mapchip.png", 3, 3, 1, 40, 40, mapchip);

    // フォントサイズを設定（ここでは24ピクセル）
    int fontSize = 20;
    ChangeFont("Gill Sans Ultra Bold", DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
    SetFontSize(fontSize);


    // 画像の読み込み
    //int backgroundImage = LoadGraph("img/backimg.png");
    int characterImage = LoadGraph("img/player2.png");

    // ゲームオブジェクトの生成
    //ボードのマス数
    OneStrokeGame game(boardX, boardY);

    // メインループ
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
        // ゲームの更新処理
        if (!game.isGameOver() && !game.isGameClear()) {

            // 背景画像の描画
            //DrawGraph(0, 0, backgroundImage, FALSE);

            // ゲームオブジェクトの描画
            game.drawBoard();

            if (Sta == START)
            {
                SetFontSize(fontSize);

                //リターン
                DrawString(20, 155, "RETURN PUSH         KEY", GetColor(255, 255, 255));
                DrawString(205, 155, "<R>", GetColor(0, 255, 0));

                //スタート
                DrawString(20, 185, "STRAT PUSH                  KEY", GetColor(255, 255, 255));
                DrawString(180, 185, "<SPACE>", GetColor(0, 255, 0));

                //リセット
                DrawString(20, 215, "RETRAY PUSH                  KEY", GetColor(255, 255, 255));
                DrawString(205, 215, "<ENTER>", GetColor(0, 255, 0));

                if (CheckHitKey(KEY_INPUT_SPACE))
                {
                    //ゲーム状態変化
                    Sta = PLAY;

                    //画面の初期化
                    ClearDrawScreen();
                }

            }

            if (Sta == PLAY)
            {
                //リターン
                DrawString(40, 370, "RETURN PUSH        KEY", GetColor(255, 255, 255));
                DrawString(220, 370, "<R>", GetColor(0, 255, 0));

                // キャラクター画像の描画
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

        // 画面を更新
        ScreenFlip();

        //if (CheckHitKey(KEY_INPUT_RETURN) && (game.isGameOver() || game.isGameClear())) 
        if (CheckHitKey(KEY_INPUT_RETURN)) {
            // Enterキーが押され、ゲームオーバーまたはゲームクリアの状態ならSTARTに戻る
            Sta = START;
            game = OneStrokeGame(boardX, boardY); // ゲームをリセット
            ClearDrawScreen();
        }

    }

    while (CheckHitKeyAll() == 0)
    {
        // メッセージループに代わる処理をする
        if (ProcessMessage() == -1)
        {
            break;        // エラーが起きたらループを抜ける
        }
    }

    // DxLibの終了
    DxLib_End();

    return 0;
}