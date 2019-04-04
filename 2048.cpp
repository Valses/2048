#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <curses.h>

#define OFFSET1 12
#define OFFSET2 1
#define N 4
#define W 5

#define TARGET 64

#define S_FAIL 0
#define S_WIN 1
#define S_NORMAL 2
#define S_QUIT 3


using namespace std;

class Game2048{

private:
    int data[N][N];
    int status;
public:
    Game2048(){
        restart();
    }

    int getStatus(){
        return status;
    }

    void processInput(){
        char ch = getch();
        if(ch >= 'a' && ch <= 'z')
            ch -= 32;

        if (ch == 'Q') {
            status = S_QUIT;
        }
        else if (ch == 'R') {
            restart();
        }

        if (status == S_NORMAL){
            bool update = false;
            if (ch == 'A'){
                update = moveLeft();
            }
            else if (ch == 'W'){
                rotate();
                update = moveLeft();
                rotate();
                rotate();
                rotate();
            }
            else if (ch == 'D'){
                rotate();
                rotate();
                update = moveLeft();
                rotate();
                rotate();
            }
            else if (ch == 'S'){
                rotate();
                rotate();
                rotate();
                update = moveLeft();
                rotate();
            }
            if(update){
                randNew();
                if (isOver()) {
                    status = S_FAIL;
                }
            }
        }
    }

    void draw(){
        //绘制游戏界面
        clear();
        mvprintw(0,20,"GAME 2048");
        for(int i = 0; i <= N; i++){
            for(int j = 0; j <= N; j++){
                drawItem(i*2+OFFSET2, j*W+OFFSET1, '+');
                if(i != N)
                    drawItem(i*2+1+OFFSET2, j*W+OFFSET1, '|');
                for(int k = 1; j != N && k < W; k++)
                    drawItem(i*2+OFFSET2, j*W+OFFSET1+k, '-');
                if(i != N && j != N){
                    drawNum(i*2+1+OFFSET2, (j+1)*W+OFFSET1-1, data[i][j]);
                }
            }
        }
        //文字
        mvprintw(2*N+2+OFFSET2, 1, "W(up),S(down),A(left),D(rigth),R(restart),Q(quit)");
        mvprintw(2*N+3+OFFSET2, 20, "Good Luck!");

        if(status == S_WIN){
            mvprintw(2*N+3+OFFSET2, 0, "*****************************************************");
            mvprintw(2*N+4+OFFSET2, 10, "YOU WIN! PRESS 'R' TO RESTART!");
            mvprintw(2*N+5+OFFSET2, 0, "*****************************************************");
        }
        if(status == S_FAIL){
            mvprintw(2*N+3+OFFSET2, 0, "*****************************************************");
            mvprintw(2*N+4+OFFSET2, 10, "YOU FAIL! PRESS 'R' TO RESTART!");
            mvprintw(2*N+5+OFFSET2, 0, "*****************************************************");
        }

    }
    void testData(){
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                data[i][j] = 0;
            }
        }
    }

private:
    bool moveLeft(){
        int temp[N][N] = {0};
        //移动
        for (int i = 0; i < N; ++i){
            int currentPos = 0;
            int lastValue = 0;
            for (int j = 0; j < N; ++j) {
                temp[i][j] = data[i][j];

                if(data[i][j] == 0) {
                    continue;
                }
                if(lastValue == 0) {
                    lastValue = data[i][j];
                }
                else{
                    if(lastValue == data[i][j]) {
                        data[i][currentPos] = lastValue * 2;
                        lastValue = 0;
                        if (data[i][currentPos] == TARGET) {
                            status = S_WIN;
                        }

                    }
                    else{
                        data[i][currentPos] = lastValue;
                        lastValue = data[i][j];
                    }
                    ++currentPos;
                }
                data[i][j] = 0;
            }

            if(lastValue != 0) {
               data[i][currentPos] = lastValue;
           }
        }
        //判断变化
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if(temp[i][j] != data[i][j])
                    return true;
            }
        }
        return false;
    }
    //其他方向的移动，对矩阵进行 旋转
    void rotate(){
        int temp[N][N] = {0};
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                temp[i][j] = data[j][N-1-i];
            }
        }
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                data[i][j] = temp[i][j];
            }
        }
    }
    void drawItem(int row, int col, char c){
        move(row, col);
        addch(c);
    }
    void drawNum(int row, int col, int num){
        //row,col为数字尾的位置
        while(num > 0){
            drawItem(row, col, num % 10 + '0');
            num /= 10;
            --col;
        }
    }

    bool isOver(){
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if(data[i][j] == 0)
                    return false;
                if(i < (N-1) && data[i][j] == data[i+1][j])
                    return false;
                if(j < (N-1) && data[i][j] == data[i][j+1])
                    return false;
            }
        }
        return true;
    }

    void restart(){
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                data[i][j] = 0;
            }
        }
        randNew();
        randNew();
        status = S_NORMAL;
    }

    bool randNew(){
        vector<int> empty;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (data[i][j] == 0) {
                    empty.push_back(i * N + j);
                }
            }
        }
        if (empty.size() == 0) {
            return false;
        }
        // 随机找个空位置
        int value = empty[rand() % empty.size()];
        // 10%的概率产生4
        data[value / N][value % N] = rand() % 10 == 1 ? 4 : 2;
        return true;
    }
};




void initialize(){
    //ncurses初始化
    initscr();
    //不用回车直接交互
    cbreak();
    //按键不显示
    noecho();
    //隐藏光标
    curs_set(0);
    //随机数
    srand(time(NULL));
}
void shutdown(){
    endwin();
}

int main(void){
    initialize();
    Game2048 game;

    do{
        game.draw();
        game.processInput();
    }while(game.getStatus() != S_QUIT);

    shutdown();
    return 0;

}
