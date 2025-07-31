#include "iGraphics.h"
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define N_CLOUDS 3
#define N_PIPES 3
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 768

#define BIRD_WIDTH 50
#define BIRD_HEIGHT 50
#define PIPE_WIDTH 80
#define PIPE_GAP 220

#define GRAVITY 0.5f
#define JUMP_VELOCITY 10.0f
#define PIPE_SPEED 7

#define N_FRAMES 8

// Game state
Image cloudImages[N_CLOUDS];
float cloud_x[N_CLOUDS];
float cloud_y[N_CLOUDS] = {300, 350, 300};
Image lowerPipeImages[N_PIPES];
Image upperPipeImages[N_PIPES];
Image BG;
Image birdFrames[N_FRAMES];
int flyingFrame = 0;

int pipe_x[N_PIPES];
int pipe_gap_y[N_PIPES];

float bird_x = 200;
float bird_y = 400;
float bird_velocity = 0;

int score = 0;
bool gameOver = false;
bool scoreCounted = false;

int animTimer, physicsTimer;

void resetGame()
{
    bird_y = 400;
    bird_velocity = 0;

    for (int i = 0; i < N_PIPES; i++) {
        pipe_x[i] = SCREEN_WIDTH + i * (SCREEN_WIDTH / N_PIPES);  // Spread pipes across the screen
        pipe_gap_y[i] = 200 + rand() % 300;

        // Resize pipes according to new gap
        iResizeImage(&lowerPipeImages[i], PIPE_WIDTH, pipe_gap_y[i]);
        int topPipeHeight = SCREEN_HEIGHT - (pipe_gap_y[i] + PIPE_GAP);
        iResizeImage(&upperPipeImages[i], PIPE_WIDTH, topPipeHeight);
    }
    score = 0;
    gameOver = false;
    scoreCounted = false;
    iResumeTimer(physicsTimer);
}

void updateClouds()
{
    for (int i = 0; i < N_CLOUDS; i++)
    {
        cloud_x[i] -= 1;  // Move clouds to the left

        // Reset cloud position if it moves off-screen
        if (cloud_x[i] + 100 < 0)  // Assuming cloud width is 100
        {
            cloud_x[i] = SCREEN_WIDTH;
        }
    }
}

void iDraw()
{
    iClear();
    iShowLoadedImage(0, 0, &BG);

    // Draw clouds
    for (int i = 0; i < N_CLOUDS; i++)
    {
        iShowLoadedImage((int)cloud_x[i], (int)cloud_y[i], &cloudImages[i]);
    }

    // Bird
    iShowLoadedImage((int)bird_x, (int)bird_y, &birdFrames[flyingFrame]);

    // Pipes
    for (int i = 0; i < N_PIPES; i++) {
        // lower pipe
        iShowLoadedImage(pipe_x[i], 0, &lowerPipeImages[i]);

        // upper pipe
        iShowLoadedImage(pipe_x[i], pipe_gap_y[i] + PIPE_GAP, &upperPipeImages[i]);
    }

    // Score
    iSetColor(0, 0, 0);
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    iText(20, SCREEN_HEIGHT-120, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);

    // Game Over
    if (gameOver)
    {
        iSetColor(255, 0, 0);
        iText(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 2 - 30, "Press 'R' to Restart", GLUT_BITMAP_TIMES_ROMAN_24);
    }
}

void iKeyboard(unsigned char key)
{
    if (key == ' ')
    {
        if (!gameOver)
            bird_velocity = JUMP_VELOCITY;
    }
    else if (key == 'r' || key == 'R')
    {
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

void updateBirdAnimation()
{
    flyingFrame = (flyingFrame + 1) % N_FRAMES;
}

void updateGame()
{
    if (gameOver) return;

    // Gravity effect
    bird_velocity -= GRAVITY;
    bird_y += bird_velocity;

    for (int i = 0; i < N_PIPES; i++) {
        // Move pipe
        pipe_x[i] -= PIPE_SPEED;

        if (pipe_x[i] + PIPE_WIDTH < 0)
        {
            pipe_x[i] = SCREEN_WIDTH + PIPE_WIDTH;
            pipe_gap_y[i] = 200 + rand() % 300;

            iResizeImage(&lowerPipeImages[i], PIPE_WIDTH, pipe_gap_y[i]);
            int topPipeHeight = SCREEN_HEIGHT - (pipe_gap_y[i] + PIPE_GAP);
            iResizeImage(&upperPipeImages[i], PIPE_WIDTH, topPipeHeight);

            scoreCounted = false;
        }

        // Collision with pipe
        if (
            bird_x + BIRD_WIDTH > pipe_x[i] && bird_x < pipe_x[i] + PIPE_WIDTH &&
            (bird_y < pipe_gap_y[i] || bird_y + BIRD_HEIGHT > pipe_gap_y[i] + PIPE_GAP)
        )
        {
            gameOver = true;
            iPauseTimer(physicsTimer);
        }

        // Scoring
        if (!scoreCounted && pipe_x[i] + PIPE_WIDTH < bird_x)
        {
            score += 1;
            scoreCounted = true;
        }
    }

    // Collision with ground or sky
    if (bird_y < 0 || bird_y + BIRD_HEIGHT > SCREEN_HEIGHT)
    {
        gameOver = true;
        iPauseTimer(physicsTimer);
    }
}

int main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));
    glutInit(&argc, argv);

    // Load background
    iLoadImage(&BG, "b1.png");
    iScaleImage(&BG, 1.0);

    // Load bird animation
    char filenames[N_FRAMES][50] = {
        "tile000.png", "tile001.png", "tile002.png", "tile003.png",
        "tile004.png", "tile005.png", "tile006.png", "tile007.png"
    };

    for (int i = 0; i < N_FRAMES; i++)
    {
        iLoadImage(&birdFrames[i], filenames[i]);
        iResizeImage(&birdFrames[i], BIRD_WIDTH, BIRD_HEIGHT);
    }

    char cloudFilenames[N_CLOUDS][50] = {"cloud.png", "cloud.png", "cloud.png"};
    for (int i = 0; i < N_CLOUDS; i++)
    {
        iLoadImage(&cloudImages[i], cloudFilenames[i]);
        iResizeImage(&cloudImages[i], 300, 150);
        cloud_x[i] = SCREEN_WIDTH + i * 200;
    }

    // Load pipe images
    for (int i = 0; i < N_PIPES; i++) {
        iLoadImage(&lowerPipeImages[i], "l.png");
        iLoadImage(&upperPipeImages[i], "u.png");
    }

    // Initial pipe gap setup
    for (int i = 0; i < N_PIPES; i++) {
        pipe_gap_y[i] = 200 + rand() % 300;  // Set initial gap position
        iResizeImage(&lowerPipeImages[i], PIPE_WIDTH, pipe_gap_y[i]);
        int topPipeHeight = SCREEN_HEIGHT - (pipe_gap_y[i] + PIPE_GAP);
        iResizeImage(&upperPipeImages[i], PIPE_WIDTH, topPipeHeight);
    }

    iSetTimer(10, updateClouds);
    animTimer = iSetTimer(100, updateBirdAnimation);
    physicsTimer = iSetTimer(15, updateGame);

        resetGame();

    iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Bird - Gravity Based");
    return 0;
}