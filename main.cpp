#include <conio.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
#include <windows.h>

#pragma comment(lib, "Winmm.lib")
using namespace std;

unsigned char shuffle(unsigned char lower, unsigned char upper)
{
    return rand() % (upper + 1 - lower) + lower;
}

unsigned char snake[4] = {186, 186, 205, 205}, //119 29
    maxX = 116, maxY = 27, minX = 50, minY = 2,
              c = 0, giftX = shuffle(minX, maxX), giftY = shuffle(minY, maxY),
              tankX = shuffle(minX, maxX), tankY = shuffle(minY, maxY), tankDirect = shuffle(0, 3),
              bulletX, bulletY, bulletDirect = 4, rangeUFO;
int preXUFO, preYUFO;
const int xUFO[40] = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0, -1, -2, -3, -4, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
          yUFO[40] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0, -1, -2, -3, -4, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4};
bool matrix[121][31], shot = 0, availUFO = 0;
vector<unsigned char> snakeX, snakeY;

long max(long a, long b)
{
    if (a > b)
        return a;
    return b;
}

void gotoxy(unsigned char x, unsigned char y)
{
    static HANDLE h = NULL;
    if (!h)
        h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD c = {x, y};
    SetConsoleCursorPosition(h, c);
}

void createGift()
{
    char preGiftX = giftX, preGiftY = giftY;
    while (matrix[giftX][giftY] || (giftX == preGiftX && giftY == preGiftY))
    {
        giftX = shuffle(minX, maxX);
        giftY = shuffle(minY, maxY);
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
    gotoxy(giftX, giftY);
    printf("%c", 254);
}

void bulletMove()
{
    if (bulletDirect == 4)
        return;
    gotoxy(bulletX, bulletY);
    printf(" ");
    matrix[bulletX][bulletY] = 0;
    unsigned char x = bulletX, y = bulletY;
    if (bulletDirect == 1)
        --y;
    if (bulletDirect == 2)
        --x;
    if (bulletDirect == 3)
        ++x;
    if (bulletDirect == 0)
        ++y;
    if (x >= minX && x <= maxX && y >= minY && y <= maxY)
    {
        bulletX = x;
        bulletY = y;
        gotoxy(x, y);
        printf("%%");
        if (matrix[x][y])
            if (!(availUFO && x == preXUFO && y == preYUFO))
            {
                shot = 1;
                return;
            }
        matrix[x][y] = 1;
        if (x == giftX && y == giftY)
            createGift();
    }
    else
    {
        bulletDirect = 4;
        gotoxy(bulletX, bulletY);
        printf(" ");
        matrix[bulletX][bulletY] = 0;
    }
}

void shoot()
{
    PlaySound("sound\\smb3_fireball.wav", NULL, SND_ASYNC);
    bulletX = tankX, bulletY = tankY;
    if (tankDirect == 1)
        --bulletY;
    if (tankDirect == 2)
        --bulletX;
    if (tankDirect == 3)
        ++bulletX;
    if (tankDirect == 0)
        ++bulletY;
    if (bulletX < minX || bulletX > maxX || bulletY < minY || bulletY > maxY)
        return;
    bulletDirect = tankDirect;
    gotoxy(bulletX, bulletY);
    printf("%%");
    if (matrix[bulletX][bulletY])
    {
        shot = 1;
        return;
    }
    matrix[bulletX][bulletY] = 1;
    if (bulletX == giftX && bulletY == giftY)
        createGift();
}

void UFOMove()
{
    if (availUFO == 0)
        return;
    rangeUFO = (rangeUFO + 1) % 40;
    if (preXUFO >= minX && preXUFO <= maxX && preYUFO >= minY && preYUFO <= maxY)
    {
        gotoxy(preXUFO, preYUFO);
        printf(" ");
        matrix[preXUFO][preYUFO] = 0;
    }
    int x = tankX + xUFO[rangeUFO], y = tankY + yUFO[rangeUFO];
    preXUFO = x;
    preYUFO = y;
    if (x >= minX && x <= maxX && y >= minY && y <= maxY)
    {
        gotoxy(x, y);
        printf("@");
        if (matrix[x][y])
            if (!(bulletDirect != 4 && bulletX == x && bulletY == y))
            {
                shot = 1;
                return;
            }
        matrix[x][y] = 1;
        if (x == giftX && y == giftY)
            createGift();
    }
}

void launchUFO()
{
    if (availUFO)
    {
        availUFO = 0;
        PlaySound("sound\\smb3_break_brick_block.wav", NULL, SND_ASYNC);
        if (preXUFO >= minX && preXUFO <= maxX && preYUFO <= maxY && preYUFO >= minY)
        {
            gotoxy(preXUFO, preYUFO);
            printf(" ");
            matrix[preXUFO][preYUFO] = 0;
        }
    }
    else
    {
        availUFO = 1;
        rangeUFO = shuffle(0, 39);
        PlaySound("sound\\smb_kick.wav", NULL, SND_ASYNC);
        preXUFO = tankX + xUFO[(rangeUFO + 1) % 40];
        preYUFO = tankY + yUFO[(rangeUFO + 1) % 40];
        UFOMove();
    }
}

void tankMove()
{
    unsigned char x = tankX, y = tankY, direct = shuffle(0, 9);
    if (bulletDirect != 4)
        direct = shuffle(0, 3);
    if (direct < 5)
    {
        if (direct == 1)
            --y;
        else if (direct == 2)
            --x;
        else if (direct == 3)
            ++x;
        else if (direct == 0)
            ++y;
        else
        {
            launchUFO();
            return;
        }
        tankDirect = direct;
        if (x >= minX && x <= maxX && y >= minY && y <= maxY)
        {
            gotoxy(tankX, tankY);
            printf(" ");
            matrix[tankX][tankY] = 0;
            tankX = x;
            tankY = y;
            gotoxy(x, y);
            printf("%c", 219);
            if (matrix[x][y])
            {
                shot = 1;
                return;
            }
            matrix[x][y] = 1;
            if (x == giftX && y == giftY)
                createGift();
        }
    }
    else
        shoot();
}

void gameOver(long long x)
{
    PlaySound("sound\\smb_mariodie.wav", NULL, SND_FILENAME);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    --x;
    ifstream file;
    long long highScore = 0;
    file.open("highscore.dat");
    if (!(file >> highScore))
        highScore = 0;
    file.close();
    gotoxy(0, 0);
    int temp = highScore;
    if (x > highScore)
    {
        PlaySound("sound\\end.wav", NULL, SND_ASYNC);
        printf("YOU'VE JUST BROKEN THE RECORD!!!\nOld highscore: %ld\nNEW ", highScore);
        highScore = x;
        ofstream file;
        file.open("highscore.dat");
        file << highScore;
        file.close();
    }
    else
        PlaySound("sound\\smb_gameover.wav", NULL, SND_ASYNC);
    printf("HIGHSCORE: %ld\n", highScore);
    printf("Game over!\nYou scored %ld point", x);
    if (x > 1)
        printf("s");
    printf(".\n");
    if (x == temp)
        printf("YOU'VE JUST REACHED THE HIGHSCORE!!!");
    printf("\nPress ESC to exit.");
    unsigned char c = 10;
    while (c != 27)
        if (kbhit)
            c = getch();
    system("cls");
    printf("Press 1 to Replay.\nPress 2 to Exit.\n");
}

void testArr()
{
    unsigned char x, y;
    for (x = minX; x <= maxX; x++)
        for (y = minY; y <= maxY; y++)
            if (matrix[x][y])
            {
                gotoxy(x, y);
                printf("%c", 219);
            }
}

void testGift()
{
    while (1)
    {
        char c = 0;
        while (c != 27)
            if (kbhit)
                c = getch();
        createGift();
    }
}

void playGame()
{
    system("cls");
    srand(time(NULL));
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
    for (unsigned char i = minX; i < maxX + 1; i++)
        for (unsigned char j = minY; j < maxY + 1; j++)
            matrix[i][j] = 0;
    for (unsigned char i = minX - 1; i < maxX + 2; i++)
    {
        gotoxy(i, minY - 1);
        printf("%c", 219);
        gotoxy(i, maxY + 1);
        printf("%c", 219);
        matrix[i][minY - 1] = 1;
        matrix[i][maxY + 1] = 1;
    }
    for (unsigned char i = minY - 1; i < maxY + 2; i++)
    {
        gotoxy(minX - 1, i);
        printf("%c", 219);
        gotoxy(maxX + 1, i);
        printf("%c", 219);
        matrix[minX - 1][i] = 1;
        matrix[maxX + 1][i] = 1;
    }
    c = 0;
    snakeX.clear();
    snakeY.clear();
    giftX = shuffle(minX, maxX);
    giftY = shuffle(minY, maxY);
    tankX = shuffle(minX, maxX);
    tankY = shuffle(minY, maxY);
    tankDirect = shuffle(0, 3);
    bulletDirect = 4;
    availUFO = 0;
    shot = 0;
    snakeX.push_back(shuffle(minX + 5, maxX - 5));
    snakeY.push_back(shuffle(minY + 5, maxY - 5));
    unsigned char x = snakeX.back(), y = snakeY.back();
    matrix[x][y] = 1;
    gotoxy(x, y);
    char Huong = shuffle(0, 3), preHuong = Huong, prec;
    switch (Huong)
    {
    case 1:
        prec = 72;
        break;
    case 2:
        prec = 75;
        break;
    case 3:
        prec = 77;
        break;
    case 0:
        prec = 80;
        break;
    }
    printf("%c", snake[Huong]);
    while (matrix[tankX][tankY])
    {
        tankX = shuffle(minX, maxX);
        tankY = shuffle(minY, maxY);
    }
    matrix[tankX][tankY] = 1;
    gotoxy(tankX, tankY);
    printf("%c", 219);
    createGift();
    gotoxy(snakeX[0], snakeY[0]);
    PlaySound("sound\\contra_title.wav", NULL, SND_FILENAME);
    //testGift();
    do
    {
        gotoxy(0, 0);
        if (kbhit())
        {
            c = getch();
            if (c == 224)
            {
                c = getch();
                if (!((c - prec) * (c + prec - 152)))
                    continue;
                prec = c;
                if (c == 72)
                    Huong = 1; //y=y-1;
                if (c == 75)
                    Huong = 2; //x=x-1;
                if (c == 77)
                    Huong = 3; //x=x+1;
                if (c == 80)
                    Huong = 0; //y=y+1;
            }
        }
        Sleep(max(127 - snakeX.size() * 3, 50));
        //testArr();
        switch (Huong)
        {
        case 1:
            --y;
            break;
        case 2:
            --x;
            break;
        case 3:
            ++x;
            break;
        case 0:
            ++y;
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        UFOMove();
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
        bulletMove();
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        tankMove();
        if (shot)
        {
            gameOver(snakeX.size());
            return;
        }
        unsigned char direction;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        if ((preHuong - Huong))
        {
            switch (preHuong)
            {
            case 0:
                if (Huong == 2)
                    direction = 188;
                else if (Huong == 3)
                    direction = 200;
                break;
            case 1:
                if (Huong == 2)
                    direction = 187;
                else if (Huong == 3)
                    direction = 201;
                break;
            case 2:
                if (Huong == 1)
                    direction = 200;
                else if (Huong == 0)
                    direction = 201;
                break;
            default:
                if (Huong == 1)
                    direction = 188;
                else if (Huong == 0)
                    direction = 187;
            }
            unsigned char u = snakeX.size() - 1;
            gotoxy(snakeX[u], snakeY[u]);
            printf("%c", direction);
        }
        gotoxy(x, y);
        printf("%c", snake[Huong]);
        preHuong = Huong;
        snakeX.push_back(x);
        snakeY.push_back(y);
        if (x != giftX || y != giftY)
        {
            gotoxy(snakeX.front(), snakeY.front());
            printf(" ");
            matrix[snakeX.front()][snakeY.front()] = 0;
            snakeX.erase(snakeX.begin());
            snakeY.erase(snakeY.begin());
        }
        else
        {
            PlaySound("sound\\smb3_coin.wav", NULL, SND_ASYNC);
            createGift();
        }
        if (matrix[x][y])
        {
            gameOver(snakeX.size());
            return;
        }
        matrix[x][y] = 1;
    } while (c != 27);
    gameOver(snakeX.size());
}

int main()
{
    playGame();
    char c = 0;
    while (c != '2')
    {
        if (kbhit())
            c = getch();
        if (c == '1')
        {
            playGame();
            c = 0;
        }
    }
    return 0;
}
