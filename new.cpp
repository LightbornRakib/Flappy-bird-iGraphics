#include "iGraphics.h"
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define N_CLOUDS 4
#define N_PIPES 8
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 768

#define N_COINS 5
#define COIN_WIDTH 30
#define COIN_HEIGHT 30
#define COIN_SPEED 7
#define COIN_FRAMES 6

#define BIRD_WIDTH 50
#define BIRD_HEIGHT 50
#define PIPE_WIDTH 80
#define PIPE_GAP 250

#define GRAVITY 0.5f
#define JUMP_VELOCITY 10.0f
#define PIPE_SPEED 7

#define N_FRAMES 8

Image cloudImages[N_CLOUDS];
float cloud_x[N_CLOUDS];
float cloud_y[N_CLOUDS] = {300, 350, 300, 320};

Image lowerPipeImages[N_PIPES];
Image upperPipeImages[N_PIPES];

Image BG;
Image birdFrames[N_FRAMES];
int flyingFrame = 0;

Image coinFrames[N_COINS][COIN_FRAMES];
float coin_x[N_COINS];
float coin_y[N_COINS];
int coinFrameIndex[N_COINS] = {0};

int pipe_x[N_PIPES];
int pipe_gap_y[N_PIPES];

float bird_x = 200;
float bird_y = 400;
float bird_velocity = 0;

int score = 0;
bool gameOver = false;
bool scoreCountedPerPipe[N_PIPES] = { false };

int animTimer, physicsTimer, coinAnimTimer;

// ----------------------------------------

void resetGame()
{
    bird_y = 400;
    bird_velocity = 0;

    int groupSpacing = SCREEN_WIDTH / 2;

    for (int group = 0; group < N_PIPES / 4; group++) {
        int baseX = SCREEN_WIDTH + group * groupSpacing;
        int baseGapY = 150 + rand() % 200;

        for (int i = 0; i < 4; i++) {
            int idx = group * 4 + i;

            pipe_x[idx] = baseX + i * (PIPE_WIDTH + 10);
            pipe_gap_y[idx] = baseGapY + (i % 2) * 80;

            int topPipeHeight = SCREEN_HEIGHT - (pipe_gap_y[idx] + PIPE_GAP);
            if (topPipeHeight < 0) topPipeHeight = 0;

            iResizeImage(&lowerPipeImages[idx], PIPE_WIDTH, pipe_gap_y[idx]);
            iResizeImage(&upperPipeImages[idx], PIPE_WIDTH, topPipeHeight);

            scoreCountedPerPipe[idx] = false;
        }
    }

    for (int i = 0; i < N_COINS; i++) {
        coin_x[i] = SCREEN_WIDTH + i * 300;
        coin_y[i] = 100 + rand() % (SCREEN_HEIGHT - 200);
    }

    score = 0;
    gameOver = false;
    iResumeTimer(physicsTimer);
}

// ----------------------------------------

void updateClouds()
{
    for (int i = 0; i < N_CLOUDS; i++) {
        cloud_x[i] -= 1;
        if (cloud_x[i] + 300 < 0) {
            cloud_x[i] = SCREEN_WIDTH;
        }
    }
}

void updateCoins()
{
    for (int i = 0; i < N_COINS; i++) {
        coin_x[i] -= COIN_SPEED;
        if (coin_x[i] + COIN_WIDTH < 0) {
            coin_x[i] = SCREEN_WIDTH;
            coin_y[i] = 100 + rand() % (SCREEN_HEIGHT - 200);
        }
    }
}

void updateCoinAnimation()
{
    for (int i = 0; i < N_COINS; i++) {
        coinFrameIndex[i] = (coinFrameIndex[i] + 1) % COIN_FRAMES;
    }
}

void updateBirdAnimation()
{
    flyingFrame = (flyingFrame + 1) % N_FRAMES;
}

// ----------------------------------------

void updateGame()
{
    if (gameOver) return;

    bird_velocity -= GRAVITY;
    if (bird_velocity < -2 * JUMP_VELOCITY) bird_velocity = -2 * JUMP_VELOCITY;
    bird_y += bird_velocity;

    if (bird_y < 0) {
        bird_y = 0;
        gameOver = true;
        iPauseTimer(physicsTimer);
    }

    if (bird_y + BIRD_HEIGHT > SCREEN_HEIGHT) {
        bird_y = SCREEN_HEIGHT - BIRD_HEIGHT;
    }

    for (int group = 0; group < N_PIPES / 4; group++) {
        bool groupReset = false;
        for (int i = 0; i < 4; i++) {
            int idx = group * 4 + i;
            pipe_x[idx] -= PIPE_SPEED;
            if (pipe_x[idx] + PIPE_WIDTH < -250)
                groupReset = true;
        }

        if (groupReset) {
            int baseX = SCREEN_WIDTH;
            int baseGapY = 150 + rand() % 200;

            for (int i = 0; i < 4; i++) {
                int idx = group * 4 + i;

                pipe_x[idx] = baseX + i * (PIPE_WIDTH + 10);
                pipe_gap_y[idx] = baseGapY + (i % 2) * 80;

                int topPipeHeight = SCREEN_HEIGHT - (pipe_gap_y[idx] + PIPE_GAP);
                if (topPipeHeight < 0) topPipeHeight = 0;

                iResizeImage(&lowerPipeImages[idx], PIPE_WIDTH, pipe_gap_y[idx]);
                iResizeImage(&upperPipeImages[idx], PIPE_WIDTH, topPipeHeight);

                scoreCountedPerPipe[idx] = false;
            }
        }
    }

    for (int i = 0; i < N_PIPES; i++) {
        if (bird_x + BIRD_WIDTH > pipe_x[i] && bird_x < pipe_x[i] + PIPE_WIDTH &&
            (bird_y < pipe_gap_y[i] || bird_y + BIRD_HEIGHT > pipe_gap_y[i] + PIPE_GAP)) {
            gameOver = true;
        }

        if (!scoreCountedPerPipe[i] && pipe_x[i] + PIPE_WIDTH < bird_x) {
            score++;
            scoreCountedPerPipe[i] = true;
        }
    }

    for (int i = 0; i < N_COINS; i++) {
        if (bird_x + BIRD_WIDTH > coin_x[i] && bird_x < coin_x[i] + COIN_WIDTH &&
            bird_y + BIRD_HEIGHT > coin_y[i] && bird_y < coin_y[i] + COIN_HEIGHT) {
            score += 5;
            coin_x[i] = SCREEN_WIDTH;
            coin_y[i] = 100 + rand() % (SCREEN_HEIGHT - 200);
        }
    }
}

// ----------------------------------------

void iDraw()
{
    iClear();
    iShowLoadedImage(0, 0, &BG);

    for (int i = 0; i < N_CLOUDS; i++) {
        iShowLoadedImage((int)cloud_x[i], (int)cloud_y[i], &cloudImages[i]);
    }

    for (int i = 0; i < N_COINS; i++) {
        iShowLoadedImage((int)coin_x[i], (int)coin_y[i], &coinFrames[i][coinFrameIndex[i]]);
    }

    iShowLoadedImage((int)bird_x, (int)bird_y, &birdFrames[flyingFrame]);

    for (int i = 0; i < N_PIPES; i++) {
        iShowLoadedImage(pipe_x[i], 0, &lowerPipeImages[i]);
        iShowLoadedImage(pipe_x[i], pipe_gap_y[i] + PIPE_GAP, &upperPipeImages[i]);
    }

    iSetColor(0, 0, 0);
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    iText(20, SCREEN_HEIGHT - 140, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);

    if (gameOver) {
        iSetColor(255, 0, 0);
        iText(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 2 - 30, "Press 'R' to Restart", GLUT_BITMAP_TIMES_ROMAN_24);
    }
}

// ----------------------------------------

void iKeyboard(unsigned char key)
{
    if (key == ' ') {
        if (!gameOver) bird_velocity = JUMP_VELOCITY;
    }
    else if (key == 'r' || key == 'R') {
        resetGame();
    }
}

void iSpecialKeyboard(unsigned char key)
{
    if (key == GLUT_KEY_END)
        exit(0);
}

void iMouseMove(int mx, int my) {}
void iMouseDrag(int mx, int my) {}
void iMouse(int button, int state, int mx, int my) {}
void iMouseWheel(int dir, int mx, int my) {}

// ----------------------------------------

int main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));
    glutInit(&argc, argv);

    iLoadImage(&BG, "background.jpg");
    iScaleImage(&BG, 2.0);

    for (int i = 0; i < N_FRAMES; i++) {
        char filename[50];
        sprintf(filename, "tile00%d.png", i);
        iLoadImage(&birdFrames[i], filename);
        iResizeImage(&birdFrames[i], BIRD_WIDTH, BIRD_HEIGHT);
    }

    for (int i = 0; i < N_COINS; i++) {
        for (int f = 0; f < COIN_FRAMES; f++) {
            char filename[50];
            sprintf(filename, "coin (%d).png", f + 1);
            iLoadImage(&coinFrames[i][f], filename);
            iResizeImage(&coinFrames[i][f], COIN_WIDTH, COIN_HEIGHT);
        }
    }

    for (int i = 0; i < N_CLOUDS; i++) {
        iLoadImage(&cloudImages[i], "cloud.png");
        iResizeImage(&cloudImages[i], 300, 150);
        cloud_x[i] = SCREEN_WIDTH + i * 400;
    }

    for (int i = 0; i < N_PIPES; i++) {
        iLoadImage(&lowerPipeImages[i], "l.png");
        iLoadImage(&upperPipeImages[i], "u.png");
    }

    resetGame();

    iSetTimer(10, updateClouds);
    animTimer = iSetTimer(100, updateBirdAnimation);
    physicsTimer = iSetTimer(15, updateGame);
    iSetTimer(10, updateCoins);
    coinAnimTimer = iSetTimer(100, updateCoinAnimation);

    iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Bird - Clean Version");
    return 0;
}