// i req u to open the "flappy bird for linux" executable file in linux for better output or run the game in linux using bash runner.sh, 
// play the game in full width...
#include"iGraphics.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>//isnot working windows
#include <cstdlib>// i have told them that i have made an experiment with csdtdlib 
#include <ctime>
#include <string>// for string commands in my terminal , creating a in game terminal is possible in c++ , dont need to use igraphics
#include <fstream>
#include <sstream>
#include <GL/glut.h>
#include <vector>// this 1
#include <algorithm>// they are also used to make a in-game terminal... this 2

#define N_CLOUDS 4
#define N_PIPES 8
#define N_COINS 10
#define N_BEAMS 3
#define N_GERMS 2
#define COIN_WIDTH 30
#define COIN_HEIGHT 30
#define COIN_SPEED 9
#define COIN_FRAMES 6
#define BIRD_WIDTH 50
#define BIRD_HEIGHT 50
#define PIPE_WIDTH 70
#define PIPE_GAP 250
#define GRAVITY 0.5f
#define JUMP_VELOCITY 10.0f
#define PIPE_SPEED 7
#define N_FRAMES 5
#define BEAM_WIDTH 20
#define BEAM_HEIGHT 10
#define BEAM_VELOCITY 12
#define GERM_WIDTH 40
#define N_PIPE_SPRITES 4
#define GERM_HEIGHT 40
#define GERM_SPEED 7
#define GERM_FRAMES 3
#define JUMP_COOLDOWN 200
#define FRAME_BUFFER_SIZE 5
#define SCORE_ANIM_DURATION 500 
#define GAME_STATE_LEVEL_SELECT 6
#define GAME_STATE_PAUSE 7

int elapsedTimeMs = 0;


//terminal btn
const int shellBtnX = 1230;  
const int shellBtnY = 840; 
const int shellBtnW = 200;
const int shellBtnH = 60;
const int NUM_BIRD_SPRITESHEETS = 3; 
Image birdFramesMultiple[NUM_BIRD_SPRITESHEETS][N_FRAMES];
int currentBirdSpriteSheet = 0; 

//bg

Image creditbg;


// UI positions and sizes
const int levelBtnW = 250, levelBtnH = 80;

const int levelStartX = (1980 - levelBtnW) / 2; 
const int levelStartY = 600;
const int levelSpacing = 30;

Mix_Music* music1 = nullptr;
Mix_Music* music2 = nullptr;
Mix_Music* currentMusic = nullptr;


const int volumeBarX = 300;
const int volumeBarY = 600;
const int volumeBarWidth = 300;
const int volumeBarHeight = 20;
const int plusIconX = volumeBarX + volumeBarWidth + 10;
const int plusIconY = volumeBarY - 5;
const int minusIconX = volumeBarX - 30;
const int minusIconY = volumeBarY - 5;
const int iconSize = 20;

//my terminal

bool terminalActive = false;
std::string terminalInput;
std::vector<std::string> terminalOutput;
const int maxTerminalLines = 10;
const int maxInputLength = 100; 
int terminalCursorBlinkTimer = 0;

const int easyX = 500-150, easyY = 300;
const int mediumX = 700-150, mediumY = 300;
const int hardX = 900-150, hardY = 300;
const int backBtnX = 125, backBtnY = 50;
const int backBtnW = 125, backBtnH = 50;
const int btnW = 200, btnH = 60;
const int playX = 140, playY = 270;
const int continueX = 430, continueY = 270;
const int helpX = 140, helpY = 145;
const int scoreX = 140, scoreY = 25;
const int exitX = 430, exitY = 25;
const int levelX = 430, levelY = 145;
const int helpContentHeight = 5000;
const int scoreContentHeight = 1600;
const int goBtnW = 200, goBtnH = 80;
const int goRestartX = 600-200, goRestartY = 300;
const int goHomeX = 850-200, goHomeY = 300;
const int goExitX = 1100-200, goExitY = 300;
const int goSaveX = 850-200, goSaveY = 420; 




Mix_Music* backgroundMusic = nullptr; 

// ================== ENUMS AND STRUCTS ==================
enum Difficulty { EASY, MEDIUM, HARD };


//load the game
enum GameState { MENU, GAME, HELP, CONTINUE, SCORE, LEVEL_SELECT, PAUSE, LOADING , SETTINGS, CREDIT};
GameState gameState = MENU;
int loadingProgress = 0;
int loadingTimer = -1;


//pipe cmnd

Image lowerPipeImages[N_PIPES];
Image upperPipeImages[N_PIPES];
Image pipeSpriteSetLower[N_PIPE_SPRITES];
Image pipeSpriteSetUpper[N_PIPE_SPRITES];


float loadingAngle = 0.0f;
int loadingDotCount = 0;


float bird_velocity_x = 0;

int currentPipeWidth = PIPE_WIDTH;

//settings

bool soundOn = true;
int soundVolume = MIX_MAX_VOLUME / 2; //  volume 50%
int newScreenWidth = 1800;
int newScreenHeight = 900;
bool editingWidth = false;
bool editingHeight = false;
std::string widthInput = std::to_string(newScreenWidth);
std::string heightInput = std::to_string(newScreenHeight);


struct HighScore {
    std::string name;
    int score;
      std::string level;
    std::string time;
};

struct JumpQueue {
    int timestamps[5];
    int front, rear, size;
    JumpQueue() : front(0), rear(-1), size(0) {
        for (int i = 0; i < 5; i++) timestamps[i] = 0;
    }
    void enqueue(int time) {
        if (size < 5) {
            rear = (rear + 1) % 5;
            timestamps[rear] = time;
            size++;
        }
    }
    int dequeue() {
        if (size == 0) return -1;
        int time = timestamps[front];
        front = (front + 1) % 5;
        size--;
        return time;
    }
    bool canJump(int currentTime) {
        while (size > 0 && currentTime - timestamps[front] > JUMP_COOLDOWN) {
            dequeue();
        }
        return size == 0;
    }
};

//updated loading

// now how the commands work?
void updateLoading() {
    loadingProgress += 5; 
    if (loadingProgress >= 100) {
        loadingProgress = 100;
        iPauseTimer(loadingTimer);
        loadingTimer = -1;
        gameState = MENU;
    }
    loadingAngle += 5.0f; 
    if (loadingAngle >= 360.0f) loadingAngle -= 360.0f;

    loadingDotCount = (loadingDotCount + 1) % 4;
}



struct ScoreAnimation {
    int score;
    float scale;
    int timer;
    bool active;
    ScoreAnimation() : score(0), scale(1.0f), timer(0), active(false) {}
    void start(int newScore) {
        score = newScore;
        scale = 1.5f;
        timer = SCORE_ANIM_DURATION;
        active = true;
    }
    void update() {
        if (!active) return;
        timer -= 16;
        if (timer <= 0) {
            active = false;
            scale = 1.0f;
        } else {
            scale = 1.0f + 0.5f * (timer / (float)SCORE_ANIM_DURATION);
        }
    }
};

// ================== GLOBAL VARIABLES ==================
Difficulty currentDifficulty = EASY;


// bg music for windows 

Mix_Chunk* backgroundMusicChunk = nullptr;
int backgroundMusicChannel = -1;
bool bgMusicPlaying = false;
int bgMusicVolume = MIX_MAX_VOLUME / 2;



bool isGamePaused = false;
bool isEnteringName = false;
bool gameOver = false;
bool gameOverSoundPlayed = false;
std::string playerName;
int nameCharIndex = 0;
HighScore highScores[5];
Image background, groundImage, gameOverImage, helpImage, scoreImage, scoreDisplayImage;
Image play, playHover, help, helpHover, quit, quitHover, cont, contHover, scoreBtn, scoreBtnHover, level, levelHover;
Image backBtn, backBtnHover;
bool useBackgroundImage = false; 
Image cloudImages[N_CLOUDS];

Image birdFrames[N_FRAMES];
Image coinFrames[N_COINS][COIN_FRAMES];
Image germFrames[GERM_FRAMES];

Image scoreWideImage;

Image settingsBtn, settingsBtnHover;




float cloud_x[N_CLOUDS], cloud_y[N_CLOUDS] = {300, 350, 300, 320};
float pipe_x[N_PIPES], pipe_gap_y[N_PIPES];
float coin_x[N_COINS], coin_y[N_COINS];
float germ_x[N_GERMS], germ_y[N_GERMS];
bool germ_active[N_GERMS] = {false};
float beam_x[N_BEAMS], beam_y[N_BEAMS];
bool beam_active[N_BEAMS] = {false};
bool scoreCountedPerPipe[N_PIPES] = {false};
float bird_x = 200, bird_y = 400, bird_velocity = 0;
int score = 0, flyingFrame = 0, germFrameIndex = 0;
int coinFrameIndex[N_COINS] = {0};
int frameBuffer[FRAME_BUFFER_SIZE] = {0};
int frameBufferIndex = 0;

float ground_x = 0;
float flashTimer = 0;
int animTimer, coinAnimTimer, germAnimTimer, physicsTimer, beamSpawnTimer;
int timers[] = { animTimer, coinAnimTimer, germAnimTimer, physicsTimer, beamSpawnTimer };
const int NUM_TIMERS = 5;
int mx = 0, my = 0;
JumpQueue jumpQueue;
ScoreAnimation scoreAnim;

    int screenWidth = 1800;
    int screenHeight = 900;


void resetGame();

 //my terminal
 void addterminaltext(const std::string& line) {
    terminalOutput.push_back(line);
    if ((int)terminalOutput.size() > maxTerminalLines) {
        terminalOutput.erase(terminalOutput.begin());
    }
}


void addHighScore(const std::string &name, int score);
void switchMusic(int musicNumber);


// Process terminal commands
void processTerminalCommand(const std::string& cmd) {
    std::string command = cmd;
  
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

//src git adding custom in-bulid shell
      if (command.rfind("save/", 0) == 0) { 
        std::string name = cmd.substr(5); 
        if (!name.empty()) {
            addHighScore(name, score);
            addterminaltext("Score saved for player: " + name);
        } else {
            addterminaltext("Invalid save command. Usage: save/yourname");
        }
        return;
    }

    // from where i have got this idea? or make the help? lets explore


    if (command == "start") {
        gameState = GAME;
        resetGame();
        addterminaltext("Game started.");
    } 

     if (command == "home") {
        gameState = MENU;
        addterminaltext("Returned to main menu.");
        return;
    }
    if (command == "help") {
        gameState = HELP;
        addterminaltext("Help page opened.");
        return;
    }
    if (command == "credit") {
        gameState = CREDIT;
       addterminaltext("Credit page opened.");
        return;
    }
    
    else if (command == "reset scr") {
        if (gameState == SCORE) {
            gameState = MENU;
            addterminaltext("Score screen closed.");
        } else {
            addterminaltext("Score screen not open.");
        }
    } else if (command == "set") {
        gameState = SETTINGS;
        addterminaltext("Settings opened.");
    } else if (command == "s on") {
        soundOn = true;
        Mix_ResumeMusic();
        addterminaltext("Sound turned ON.");
    } else if (command == "s off") {
        soundOn = false;
        Mix_PauseMusic();
        addterminaltext("Sound turned OFF.");
    } else if (command == "ext") {
        addterminaltext("Exiting game...");
        exit(0);
    } 
    
    else if (command == "music 1") {
    switchMusic(1);
    addterminaltext("Switched to music 1.");
} else if (command == "music 2") {
    switchMusic(2);
    addterminaltext("Switched to music 2.");
}

else if (command == "name.hscr") {
        gameState = SCORE;
        addterminaltext("Showing high scores.");
    } else if (command.rfind("level ", 0) == 0) {
        if (command.length() >= 7) {
            char levelChar = command[6];
            if (levelChar == 'e') {
                currentDifficulty = EASY;
                resetGame();
                gameState = GAME;
                addterminaltext("Easy level started.");
            } else if (levelChar == 'm') {
                currentDifficulty = MEDIUM;
                resetGame();
                gameState = GAME;
                addterminaltext("Medium level started.");
            } else if (levelChar == 'h') {
                currentDifficulty = HARD;
                resetGame();
                gameState = GAME;
                addterminaltext("Hard level started.");
            } else {
                addterminaltext("Unknown level: " + std::string(1, levelChar));
            }
        } else {
            addterminaltext("Invalid level command.");
        }
    } 
    else if (command.rfind("bird ", 0) == 0) { 
        if (command.length() >= 6) {
            int sheetNum = command[5] - '1'; // expecting "bird 1", "bird 2", etc.
            if (sheetNum >= 0 && sheetNum < NUM_BIRD_SPRITESHEETS) {
                currentBirdSpriteSheet = sheetNum;
                addterminaltext("Bird sprite sheet changed to " + std::to_string(sheetNum + 1));
            } else {
                addterminaltext("Invalid bird sprite sheet number.");
            }
        } else {
            addterminaltext("Invalid bird command.");
        }
    }
    else {
        addterminaltext("Unknown command " + cmd);
    }
}


void iPlayBackgroundMusicWav(const char* filename) {
       if (backgroundMusicChunk) {
        Mix_HaltChannel(backgroundMusicChannel);
        Mix_FreeChunk(backgroundMusicChunk);
        backgroundMusicChunk = nullptr;
        backgroundMusicChannel = -1;
    }
    backgroundMusicChunk = Mix_LoadWAV(filename);
    if (!backgroundMusicChunk) {
        
        return;
    }
    backgroundMusicChannel = Mix_PlayChannel(-1, backgroundMusicChunk, -1); 
    Mix_Volume(backgroundMusicChannel, bgMusicVolume);
    bgMusicPlaying = true;
}

void iStopBackgroundMusicWav() {
    if (backgroundMusicChunk) {
        Mix_HaltChannel(backgroundMusicChannel);
        Mix_FreeChunk(backgroundMusicChunk);
        backgroundMusicChunk = nullptr;
        backgroundMusicChannel = -1;
    }
    bgMusicPlaying = false;
}

void iPauseBackgroundMusicWav() {
    if (bgMusicPlaying && backgroundMusicChannel != -1) {
        Mix_Pause(backgroundMusicChannel);
        bgMusicPlaying = false;
    }
}

void iResumeBackgroundMusicWav() {
    if (!bgMusicPlaying && backgroundMusicChannel != -1) {
        Mix_Resume(backgroundMusicChannel);
        bgMusicPlaying = true;
    }
}

void iSetBackgroundMusicVolume(int volume) {
    bgMusicVolume = volume;
    if (backgroundMusicChannel != -1) {
        Mix_Volume(backgroundMusicChannel, bgMusicVolume);
    }
}



    
// ================== STYLISH TEXT FUNCTION ==================
void drawStyledText(int x, int y, const char* text, void* font = GLUT_BITMAP_TIMES_ROMAN_24, bool isImportant = false) {
 
    void* renderFont = font;
    if (isImportant && scoreAnim.active && scoreAnim.scale > 1.2f) {
        renderFont = GLUT_BITMAP_TIMES_ROMAN_24; 
    } else if (isImportant) {
        renderFont = GLUT_BITMAP_HELVETICA_18; 
    }

    iSetColor(50, 50, 50);
    iText(x + 2, y - 2, text, renderFont);

    iSetColor(0, 0, 0);
    iText(x + 1, y, text, renderFont);
    iText(x - 1, y, text, renderFont);
    iText(x, y + 1, text, renderFont);
    iText(x, y - 1, text, renderFont);

    if (isImportant && scoreAnim.active) {
        int red = 255;
        int green = 255 * (scoreAnim.timer / (float)SCORE_ANIM_DURATION);
        int blue = 0;
        iSetColor(red, green, blue);
    } else {
        iSetColor(isImportant ? 255 : 255, isImportant ? 255 : 255, isImportant ? 0 : 255);
    }
    iText(x, y, text, renderFont);
}

// ================== HELPER FUNCTIONS ==================
bool loadImageWithCheck(Image &img, const char *filename, int width = -1, int height = -1) {
    if (!iLoadImage(&img, filename)) {
       
        return false;
    }
    if (width != -1 && height != -1) {
        iResizeImage(&img, width, height);
    }
    return true;
}

void pauseAllTimers() {
    for (int i = 0; i < NUM_TIMERS; i++) {
        if (timers[i] >= 0 && timers[i] < 10) { 
            iPauseTimer(timers[i]);
        }
    }
}

void resumeAllTimers() {
    for (int i = 0; i < NUM_TIMERS; i++) {
        if (timers[i] >= 0 && timers[i] < 10) {
            iResumeTimer(timers[i]);
        }
    }
}

void iPlaySound(const char *filename, bool loop = false) {
    Mix_Chunk *sound = Mix_LoadWAV(filename);
    if (!sound) {
      
        return;
    }
    Mix_PlayChannel(-1, sound, loop ? -1 : 0); 
}

void loadHighScores() {
    std::ifstream file("highscores.txt");
    if (!file.is_open()) {
        for (int i = 0; i < 5; i++) {
            highScores[i].name = "---";
            highScores[i].score = 0;
            highScores[i].level = "---";
            highScores[i].time = "00:00";
        }
        return;
    }
    for (int i = 0; i < 5; i++) {
        if (!(file >> highScores[i].name >> highScores[i].score >> highScores[i].level >> highScores[i].time)) {
            highScores[i].name = "---";
            highScores[i].score = 0;
            highScores[i].level = "---";
            highScores[i].time = "00:00";
        }
    }
    file.close();
}

void saveHighScores() {
    std::ofstream file("highscores.txt");
    if (!file.is_open()) return;
    for (int i = 0; i < 5; i++) {
        file << highScores[i].name << " " << highScores[i].score << " " << highScores[i].level << " " << highScores[i].time << "\n";
    }
    file.close();
}

void addHighScore(const std::string &name, int score) {
    int pos = 5;
    for (int i = 0; i < 5; i++) {
        if (score > highScores[i].score) {
            pos = i;
            break;
        }
    }
    if (pos == 5) return;
    for (int i = 4; i > pos; i--) {
        highScores[i] = highScores[i - 1];
    }
    int minutes = elapsedTimeMs / 60000;
    int seconds = (elapsedTimeMs / 1000) % 60;
    char timeStr[16];
    sprintf(timeStr, "%02d:%02d", minutes, seconds);
    std::string levelStr = (currentDifficulty == HARD) ? "Hard" : (currentDifficulty == MEDIUM) ? "Medium" : "Easy";

    highScores[pos].name = name.substr(0, 49);
    highScores[pos].score = score;
    highScores[pos].level = levelStr;
    highScores[pos].time = timeStr;
    saveHighScores();
}


void switchMusic(int musicNumber) {
    if (musicNumber == 1 && currentMusic != music1 && music1) {
        Mix_HaltMusic();
        currentMusic = music1;
        Mix_PlayMusic(currentMusic, -1);
    } else if (musicNumber == 2 && currentMusic != music2 && music2) {
        Mix_HaltMusic();
        currentMusic = music2;
        Mix_PlayMusic(currentMusic, -1);
    }
}





void scrollScreen(int &scrollY, int contentHeight, int delta) {
    scrollY += delta;
    int maxScroll = contentHeight - screenHeight;
    if (maxScroll < 0) maxScroll = 0;
    if (scrollY < 0) scrollY = 0;
    if (scrollY > maxScroll) scrollY = maxScroll;
}
// ================== GAME LOGIC ==================
void resetGame() {
    elapsedTimeMs = 0;
    bird_x = 200;
    bird_y = 400;
    bird_velocity = 0;
    bird_velocity_x = 0;
    score = 0;
    gameOver = false;
    gameOverSoundPlayed = false;
    isEnteringName = false;
    playerName = "";
    nameCharIndex = 0;
    flashTimer = 0;
    flyingFrame = 0;
    germFrameIndex = 0;
    scoreAnim.active = false;
    jumpQueue = JumpQueue();
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++) frameBuffer[i] = 0;
    frameBufferIndex = 0;


    currentPipeWidth = (currentDifficulty == HARD) ? 200 : PIPE_WIDTH; 

    if (currentDifficulty == HARD) {
        int pipeSpacing = 600;
        for (int i = 0; i < N_PIPES; i++) {
            pipe_x[i] = screenWidth + i * pipeSpacing;
            pipe_gap_y[i] = 150 + rand() % 200;
            int topPipeHeight = screenHeight - (pipe_gap_y[i] + PIPE_GAP);
            if (topPipeHeight < 0) topPipeHeight = 0;
            iResizeImage(&lowerPipeImages[i], currentPipeWidth, pipe_gap_y[i]);
            iResizeImage(&upperPipeImages[i], currentPipeWidth, topPipeHeight);
            scoreCountedPerPipe[i] = false;
        }
    } else {
        
        int totalGroups = N_PIPES / 4;
        int groupSpacing = screenWidth / 2;
        for (int group = 0; group < totalGroups; group++) {
            int baseX = screenWidth + group * groupSpacing;
            int baseGapY = 150 + rand() % 200;
            for (int i = 0; i < 4; i++) {
                int idx = group * 4 + i;
                if (currentDifficulty == EASY && i != 0) {
                    pipe_x[idx] = -1000;
                    scoreCountedPerPipe[idx] = true;
                    continue;
                }
                pipe_x[idx] = baseX + i * (currentPipeWidth + 10);
                pipe_gap_y[idx] = baseGapY + (i % 2) * 80;
                int topPipeHeight = screenHeight - (pipe_gap_y[idx] + PIPE_GAP);
                if (topPipeHeight < 0) topPipeHeight = 0;
                iResizeImage(&lowerPipeImages[idx], currentPipeWidth, pipe_gap_y[idx]);
                iResizeImage(&upperPipeImages[idx], currentPipeWidth, topPipeHeight);
                scoreCountedPerPipe[idx] = false;
            }
        }
    }
   
    for (int i = 0; i < N_COINS; i++) {
        coin_x[i] = screenWidth + i * 300;
        coin_y[i] = 100 + rand() % (screenHeight - 200);
        coinFrameIndex[i] = 0;
    }


    for (int i = 0; i < N_GERMS; i++) {
        germ_x[i] = screenWidth + i * 300;
        germ_y[i] = 100 + rand() % (screenHeight - 200);
        germ_active[i] = (currentDifficulty == HARD);
    }

    for (int i = 0; i < N_BEAMS; i++) {
        beam_active[i] = (currentDifficulty == HARD);
        if (beam_active[i]) {
            beam_x[i] = screenWidth + i * (BEAM_WIDTH + 50);
            beam_y[i] = 300 + rand() %200 ;
        }
    }

    resumeAllTimers();
}



void updateGround() {
    ground_x -= PIPE_SPEED;
    if (ground_x <= -screenWidth) ground_x = 0;
}

void updateClouds() {
    for (int i = 0; i < N_CLOUDS; i++) {
        cloud_x[i] -= 2;
        if (cloud_x[i] + 300 < 0) cloud_x[i] = screenWidth;
    }
}

void updateCoins() {
    for (int i = 0; i < N_COINS; i++) {
        coin_x[i] -= COIN_SPEED;
        if (coin_x[i] + COIN_WIDTH < 0) {
            coin_x[i] = screenWidth;
            coin_y[i] = 100 + rand() % (screenHeight - 200);
        }
        if (!gameOver && bird_x + BIRD_WIDTH > coin_x[i] && bird_x < coin_x[i] + COIN_WIDTH &&
            bird_y + BIRD_HEIGHT > coin_y[i] && bird_y < coin_y[i] + COIN_HEIGHT) {
            score += 1;
            scoreAnim.start(score);
            coin_x[i] = screenWidth;
            coin_y[i] = 100 + rand() % (screenHeight - 200);
            iPlaySound("coin_collect.wav");
        }
    }
}

void updateCoinAnimation() {
    for (int i = 0; i < N_COINS; i++) {
        coinFrameIndex[i] = (coinFrameIndex[i] + 1) % COIN_FRAMES;
    }
}

void updateBirdAnimation() {
    if (gameOver || isGamePaused) return;
    flyingFrame = (flyingFrame + 1) % N_FRAMES;
    frameBuffer[frameBufferIndex] = flyingFrame;
    frameBufferIndex = (frameBufferIndex + 1) % FRAME_BUFFER_SIZE;
}

void updateGerms() {
    if (currentDifficulty != HARD) return;
    for (int i = 0; i < N_GERMS; i++) {
        if (!germ_active[i]) continue;
        germ_x[i] -= GERM_SPEED;
        if (germ_x[i] + GERM_WIDTH < 0) {
            germ_x[i] = screenWidth;
            germ_y[i] = 100 + rand() % (screenHeight - 200);
        }
        if (!gameOver && bird_x + BIRD_WIDTH > germ_x[i] && bird_x < germ_x[i] + GERM_WIDTH &&
            bird_y + BIRD_HEIGHT > germ_y[i] && bird_y < germ_y[i] + GERM_HEIGHT) {
            gameOver = true;
            flashTimer = 30;
            pauseAllTimers();
            if (!gameOverSoundPlayed) {
                iPlaySound("game_over.wav");
                gameOverSoundPlayed = true;
            }
            germ_active[i] = false;
        }
    }
}

void updateGermAnimation() {
    germFrameIndex = (germFrameIndex + 1) % GERM_FRAMES;
}


void updateBeams() {
    if (currentDifficulty != HARD) return;

    static float groupBeamY = 0; 

    bool resetGroup = false;
    for (int i = 0; i < N_BEAMS; i++) {
        if (!beam_active[i]) continue;
        beam_x[i] -= BEAM_VELOCITY;
        if (beam_x[i] + BEAM_WIDTH < 0) {
            resetGroup = true;
        }
    }

    if (resetGroup) {
        
        groupBeamY = 400 + rand() % 200;
       
        for (int i = 0; i < N_BEAMS; i++) {
            beam_x[i] = screenWidth + i * (BEAM_WIDTH + 50);
            beam_y[i] = groupBeamY;
        }
    }


    for (int i = 0; i < N_BEAMS; i++) {
        if (!beam_active[i]) continue;
        if (!gameOver && bird_x + BIRD_WIDTH > beam_x[i] && bird_x < beam_x[i] + BEAM_WIDTH &&
            bird_y + BIRD_HEIGHT > beam_y[i] && bird_y < beam_y[i] + BEAM_HEIGHT) {
            gameOver = true;
            flashTimer = 30;
            pauseAllTimers();
            if (!gameOverSoundPlayed) {
                iPlaySound("game_over.wav");
                gameOverSoundPlayed = true;
            }
        }
    }
}




// pipe coded

void updatePipes() {
    if (currentDifficulty == HARD) {
        for (int i = 0; i < N_PIPES; i++) {
            pipe_x[i] -= PIPE_SPEED;
            if (pipe_x[i] + currentPipeWidth < -250) {
                pipe_x[i] = screenWidth + (N_PIPES - 1) * 600;

               
                int minGap = 180; 
                int maxGap = 320; 
                int gapSize = minGap + rand() % (maxGap - minGap + 1);

              
                int middleGapY = (screenHeight - gapSize) / 2;

                pipe_gap_y[i] = middleGapY;

                int topPipeHeight = screenHeight - (pipe_gap_y[i] + gapSize);
                if (topPipeHeight < 0) topPipeHeight = 0;

                iResizeImage(&lowerPipeImages[i], currentPipeWidth, pipe_gap_y[i]);
                iResizeImage(&upperPipeImages[i], currentPipeWidth, topPipeHeight);

                scoreCountedPerPipe[i] = false;
            }

            if (!gameOver && bird_x + BIRD_WIDTH > pipe_x[i] && bird_x < pipe_x[i] + currentPipeWidth &&
                (bird_y < pipe_gap_y[i] || bird_y + BIRD_HEIGHT > pipe_gap_y[i] + PIPE_GAP)) {
                gameOver = true;
                flashTimer = 30;
                pauseAllTimers();
                if (!gameOverSoundPlayed) {
                    iPlaySound("game_over.wav");
                    gameOverSoundPlayed = true;
                }
            }

            if (!scoreCountedPerPipe[i] && pipe_x[i] + currentPipeWidth < bird_x) {
                score++;
                scoreAnim.start(score);
                scoreCountedPerPipe[i] = true;
            }
        }
    } else {
        int totalGroups = N_PIPES / 4;
        for (int group = 0; group < totalGroups; group++) {
            bool groupReset = false;
            for (int i = 0; i < 4; i++) {
                int idx = group * 4 + i;
                if (idx < 0 || idx >= N_PIPES) continue;

                if (currentDifficulty == EASY && i != 0) continue;

                pipe_x[idx] -= PIPE_SPEED;
                if (pipe_x[idx] + currentPipeWidth < -250) groupReset = true;

                if (!gameOver && bird_x + BIRD_WIDTH > pipe_x[idx] && bird_x < pipe_x[idx] + currentPipeWidth &&
                    (bird_y < pipe_gap_y[idx] || bird_y + BIRD_HEIGHT > pipe_gap_y[idx] + PIPE_GAP)) {
                    gameOver = true;
                    flashTimer = 30;
                    pauseAllTimers();
                    if (!gameOverSoundPlayed) {
                        iPlaySound("game_over.wav");
                        gameOverSoundPlayed = true;
                    }
                }

                if (!scoreCountedPerPipe[idx] && pipe_x[idx] + currentPipeWidth < bird_x) {
                    score++;
                    scoreAnim.start(score);
                    scoreCountedPerPipe[idx] = true;
                }
            }
            if (groupReset) {
                int baseX = screenWidth;
                int baseGapY = 150 + rand() % 200;
                for (int i = 0; i < 4; i++) {
                    int idx = group * 4 + i;
                    if (idx < 0 || idx >= N_PIPES) continue;

                    if (currentDifficulty == EASY && i != 0) {
                        pipe_x[idx] = -1000;
                        scoreCountedPerPipe[idx] = true;
                        continue;
                    }
                    pipe_x[idx] = baseX + i * (currentPipeWidth + 10);
                    pipe_gap_y[idx] = baseGapY + (i % 2) * 80;
                    int topPipeHeight = screenHeight - (pipe_gap_y[idx] + PIPE_GAP);
                    if (topPipeHeight < 0) topPipeHeight = 0;

                    iResizeImage(&lowerPipeImages[idx], currentPipeWidth, pipe_gap_y[idx]);
                    iResizeImage(&upperPipeImages[idx], currentPipeWidth, topPipeHeight);
                    scoreCountedPerPipe[idx] = false;
                }
            }
        }
    }
}

void updateGame() {
    if (gameOver || isGamePaused || terminalActive) return;

    elapsedTimeMs += 16;

    scoreAnim.update();

    bird_velocity -= GRAVITY;
    if (bird_velocity < -2 * JUMP_VELOCITY) bird_velocity = -2 * JUMP_VELOCITY;
    bird_y += bird_velocity;

    bird_x += bird_velocity_x;
   

    if (bird_y < 0) {
        bird_y = 0;
        gameOver = true;
        flashTimer = 30;
        pauseAllTimers();
        if (!gameOverSoundPlayed) {
            iPlaySound("game_over.wav");
            gameOverSoundPlayed = true;
        }
    }
    if (bird_y + BIRD_HEIGHT > screenHeight) {
        bird_y = screenHeight - BIRD_HEIGHT;
        bird_velocity = 0;
    }
    if (bird_x < 0) bird_x = 0;
    if (bird_x + BIRD_WIDTH > screenWidth) bird_x = screenWidth - BIRD_WIDTH;

    updatePipes();
    updateClouds();
    updateCoins();
    updateBeams();
    updateGerms();
    updateGround();

    if (flashTimer > 0) flashTimer--;
}


void drawRoundedGradientRect(int x, int y, int width, int height, int radius) {
   
    for (int i = 0; i < height; i++) {
        float t = i / (float)height;
        int r = (int)((1 - t) * 0 + t * 139);  
        int g = 0;
        int b = (int)((1 - t) * 139 + t * 0);  
        iSetColor(r, g, b);
        iLine(x + radius, y + i, x + width - radius, y + i);
    }

  
    iSetColor(0, 0, 139); 
    iFilledRectangle(x, y + radius, radius, height - 2 * radius);
    iSetColor(139, 0, 0);
    iFilledRectangle(x + width - radius, y + radius, radius, height - 2 * radius);

    
    iSetColor(0, 0, 139); 
    iFilledCircle(x + radius, y + radius, radius);
    iFilledCircle(x + radius, y + height - radius, radius);

    iSetColor(139, 0, 0); 
    iFilledCircle(x + width - radius, y + radius, radius);
    iFilledCircle(x + width - radius, y + height - radius, radius);
}


// ================== DRAW FUNCTION ==================
void iDraw() {
    iClear();
    std::string scoreText;
    int startY;
     

  if (gameState == LOADING) {
    iSetColor(20, 20, 40);
    iFilledRectangle(0, 0, screenWidth, screenHeight);

  
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2 + 50;
    int radius = 40;
    int numSegments = 12;
    for (int i = 0; i < numSegments; i++) {
        float angle = (360.0f / numSegments) * i + loadingAngle;
        float rad = angle * 3.14159f / 180.0f;
        int x = centerX + (int)(radius * cos(rad));
        int y = centerY + (int)(radius * sin(rad));
        int brightness = 100 + (155 * (i + 1)) / numSegments; // fading effect
        iSetColor(brightness, brightness, 255);
        iFilledCircle(x, y, 6);
    }

    int barWidth = 800;
    int barHeight = 15;
    int barX = centerX - barWidth / 2;
    int barY = centerY - 400;


    iSetColor(50, 50, 100);
    iRectangle(barX, barY, barWidth, barHeight);


    for (int i = 0; i < (barWidth * loadingProgress / 100); i++) {
        int red = 50 + (205 * i) / barWidth;
        int green = 50 + (205 * i) / barWidth;
        int blue = 150 + (105 * i) / barWidth;
        iSetColor(red, green, blue);
        iLine(barX + i, barY + 1, barX + i, barY + barHeight - 1);
    }

    

    return;
}


    if (gameState != MENU) {
        iShowLoadedImage(backBtnX, backBtnY, (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) ? &backBtnHover : &backBtn);
    }

    switch (gameState) {
    case MENU: {
        iShowLoadedImage(0, 0, &background);
        iShowLoadedImage(playX, playY, (mx >= playX && mx <= playX + btnW && my >= playY && my <= playY + btnH) ? &playHover : &play);
        iShowLoadedImage(helpX, helpY, (mx >= helpX && mx <= helpX + btnW && my >= helpY && my <= helpY + btnH) ? &helpHover : &help);
        iShowLoadedImage(exitX, exitY, (mx >= exitX && mx <= exitX + btnW && my >= exitY && my <= exitY + btnH) ? &quitHover : &quit);
        iShowLoadedImage(continueX, continueY, (mx >= continueX && mx <= continueX + btnW && my >= continueY && my <= continueY + btnH) ? &contHover : &cont);
        iShowLoadedImage(scoreX, scoreY, (mx >= scoreX && mx <= scoreX + btnW && my >= scoreY && my <= scoreY + btnH) ? &scoreBtnHover : &scoreBtn);
        iShowLoadedImage(levelX, levelY, (mx >= levelX && mx <= levelX + btnW && my >= levelY && my <= levelY + btnH) ? &levelHover : &level);
    
   
        
        int settingsX = screenWidth - 250;
    int settingsY = screenHeight - 250; 
    iShowLoadedImage(settingsX, settingsY,
        (mx >= settingsX && mx <= settingsX + 200 && my >= settingsY && my <= settingsY + 100) ?
        &settingsBtnHover : &settingsBtn);

       int creditW = 120;  
    int creditH = 60;   
    int creditX = settingsX - creditW - 40; 
    int creditY = settingsY+10;

    // credit btn 
    bool creditHovered = (mx >= creditX && mx <= creditX + creditW && my >= creditY && my <= creditY + creditH);
    for (int h = 0; h < creditH; h++) {
        float t = h / (float)creditH;
        int r = (int)((1 - t) * 70 + t * 180);
        int g = 0;
        int b = (int)((1 - t) * 180 + t * 70);
        iSetColor(r, g, b);
        iLine(creditX, creditY + h, creditX + creditW, creditY + h);
    }
    if (creditHovered) {
        iSetColor(100, 0, 200);
    } else {
        iSetColor(70, 0, 180);
    }
    iFilledCircle(creditX, creditY + creditH / 2, creditH / 2);
    iFilledCircle(creditX + creditW, creditY + creditH / 2, creditH / 2);

    int creditTextX = creditX + creditW / 2 - (6 * 6); 
    int creditTextY = creditY + creditH / 2 + 5;        
    drawStyledText(creditTextX, creditTextY, "Credit", GLUT_BITMAP_HELVETICA_12, creditHovered);


// draw shell
 bool shellHovered = (mx >= shellBtnX && mx <= shellBtnX + shellBtnW && my >= shellBtnY && my <= shellBtnY + shellBtnH);
    for (int h = 0; h < shellBtnH; h++) {
        float t = h / (float)shellBtnH;
        int r = (int)((1 - t) * 70 + t * 180);
        int g = 0;
        int b = (int)((1 - t) * 180 + t * 70);
        iSetColor(r, g, b);
        iLine(shellBtnX, shellBtnY + h, shellBtnX + shellBtnW, shellBtnY + h);
    }
    if (shellHovered) {
        iSetColor(100, 0, 200);
    } else {
        iSetColor(70, 0, 180);
    }
    iFilledCircle(shellBtnX, shellBtnY + shellBtnH / 2, shellBtnH / 2);
    iFilledCircle(shellBtnX + shellBtnW, shellBtnY + shellBtnH / 2, shellBtnH / 2);
    iSetColor(255, 255, 255);
    iRectangle(shellBtnX, shellBtnY, shellBtnW, shellBtnH);

    int textX = shellBtnX + shellBtnW / 2 - (5 * 10);
    int textY = shellBtnY + shellBtnH / 2 + 7;
    drawStyledText(textX, textY, "Shell", GLUT_BITMAP_HELVETICA_18, shellHovered);
        


        break;
    }



  

    case SETTINGS: {

    iSetColor(20, 20, 20);
    iFilledRectangle(0, 0, screenWidth, screenHeight);

   
    int panelX = screenWidth / 2 - 450;
    int panelY = screenHeight / 2 - 250;
    int panelWidth = 900;
    int panelHeight = 500;
    int panelRadius = 40;
    drawRoundedGradientRect(panelX, panelY, panelWidth, panelHeight, panelRadius);

 
    drawStyledText(panelX + panelWidth / 2 - 70, panelY + panelHeight - 60, "Settings", GLUT_BITMAP_TIMES_ROMAN_24, true);

    drawStyledText(panelX + 80, panelY + panelHeight - 140, "Sound:", GLUT_BITMAP_HELVETICA_18);
    std::string soundStatus = soundOn ? "ON" : "OFF";
    iSetColor(soundOn ? 0 : 150, soundOn ? 255 : 0, 0);
    drawStyledText(panelX + 230, panelY + panelHeight - 140, soundStatus.c_str(), GLUT_BITMAP_HELVETICA_18);

    // Volume label
    drawStyledText(panelX + 80, panelY + panelHeight - 200, "Volume:", GLUT_BITMAP_HELVETICA_18);

 
    int volumeBarX = panelX + 230;
    int volumeBarY = panelY + panelHeight - 210;
    int volumeBarWidth = 300;
    int volumeBarHeight = 25;
    iSetColor(100, 100, 100);
    iFilledRectangle(volumeBarX, volumeBarY, volumeBarWidth, volumeBarHeight);

    // Volume 
    int volWidth = (soundVolume * volumeBarWidth) / MIX_MAX_VOLUME;
    iSetColor(0, 255, 0);
    iFilledRectangle(volumeBarX, volumeBarY, volWidth, volumeBarHeight);

   
    int iconSize = 30;
    int minusIconX = volumeBarX - 50;
    int minusIconY = volumeBarY - 3;
    iSetColor(200, 0, 0);
    iFilledRectangle(minusIconX, minusIconY, iconSize, iconSize);
    iSetColor(255, 255, 255);
    iLine(minusIconX + 6, minusIconY + iconSize / 2, minusIconX + iconSize - 6, minusIconY + iconSize / 2);

   
    int plusIconX = volumeBarX + volumeBarWidth + 20;
    int plusIconY = volumeBarY - 3;
    iSetColor(0, 200, 0);
    iFilledRectangle(plusIconX, plusIconY, iconSize, iconSize);
    iSetColor(255, 255, 255);
    iLine(plusIconX + 6, plusIconY + iconSize / 2, plusIconX + iconSize - 6, plusIconY + iconSize / 2);
    iLine(plusIconX + iconSize / 2, plusIconY + 6, plusIconX + iconSize / 2, plusIconY + iconSize - 6);

    drawStyledText(panelX + 80, panelY + panelHeight - 270, "Screen Width:", GLUT_BITMAP_HELVETICA_18);
    iSetColor(editingWidth ? 255 : 200, editingWidth ? 255 : 255, editingWidth ? 0 : 200);
    drawStyledText(panelX + 280, panelY + panelHeight - 270, widthInput.c_str(), GLUT_BITMAP_HELVETICA_18);

  
    drawStyledText(panelX + 80, panelY + panelHeight - 320, "Screen Height:", GLUT_BITMAP_HELVETICA_18);
    iSetColor(editingHeight ? 255 : 200, editingHeight ? 255 : 255, editingHeight ? 0 : 200);
    drawStyledText(panelX + 280, panelY + panelHeight - 320, heightInput.c_str(), GLUT_BITMAP_HELVETICA_18);

    drawStyledText(panelX + 80, panelY + 40, "Click values to edit. Enter to apply. Backspace to delete.", GLUT_BITMAP_HELVETICA_12);
    drawStyledText(panelX + 80, panelY + 10, "Press 'B' or Back button to return to Menu.", GLUT_BITMAP_HELVETICA_12);

    // Back button
    iShowLoadedImage(backBtnX, backBtnY,
        (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) ?
        &backBtnHover : &backBtn);

    break;
}

case LEVEL_SELECT: {
 
    iShowLoadedImage(0, 0, &background);

  
    int btnWidth = levelBtnW;
    int btnHeight = levelBtnH;
    int btnXs[3] = {easyX, mediumX, hardX};
    int btnYs[3] = {easyY, mediumY, hardY};
    const char* btnLabels[3] = {"Easy", "Medium", "Hard"};

   
    for (int i = 0; i < 3; i++) {
        int btnX = btnXs[i];
        int btnY = btnYs[i];

        // hover effect
        bool isHovered = (mx >= btnX && mx <= btnX + btnWidth && my >= btnY && my <= btnY + btnHeight);

        for (int h = 0; h < btnHeight; h++) {
            float t = h / (float)btnHeight;
            int r = (int)((1 - t) * 70 + t * 180);
            int g = 0;
            int b = (int)((1 - t) * 180 + t * 70);
            iSetColor(r, g, b);
            iLine(btnX, btnY + h, btnX + btnWidth, btnY + h);
        }

       
        if (isHovered) {
            iSetColor(100, 0, 200); 
        } else {
            iSetColor(70, 0, 180);
        }
        iFilledCircle(btnX, btnY + btnHeight / 2, btnHeight / 2);
        iFilledCircle(btnX + btnWidth, btnY + btnHeight / 2, btnHeight / 2);

     
        iSetColor(255, 255, 255);
        iRectangle(btnX, btnY, btnWidth, btnHeight);

       
        int textX = btnX + btnWidth / 2 - (strlen(btnLabels[i]) * 10); 
        int textY = btnY + btnHeight / 2 + 7;
        drawStyledText(textX, textY, btnLabels[i], GLUT_BITMAP_HELVETICA_18, isHovered);
    }

  
    iShowLoadedImage(backBtnX, backBtnY,
        (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) ?
        &backBtnHover : &backBtn);

  
    drawStyledText(50, screenHeight - 50, "Press 'H' to return to Home", GLUT_BITMAP_HELVETICA_12);
    break;
}



    case GAME: {

          int minutes = elapsedTimeMs / 60000;
        int seconds = (elapsedTimeMs / 1000) % 60;
        char timeStr[16];
        sprintf(timeStr, "%02d:%02d", minutes, seconds);
        std::string levelStr = (currentDifficulty == HARD) ? "Hard" : (currentDifficulty == MEDIUM) ? "Medium" : "Easy";
        std::string scoreText = "Score: " + levelStr + " " + std::to_string(score) + " Time: " + timeStr;
        drawStyledText(250, screenHeight - 250, scoreText.c_str(), GLUT_BITMAP_TIMES_ROMAN_24, true);




        iSetColor(135, 206, 235);
        iFilledRectangle(0, 0, screenWidth, screenHeight);
        for (int i = 0; i < N_CLOUDS; i++) {
            iShowLoadedImage((int)cloud_x[i], (int)cloud_y[i], &cloudImages[i]);
        }
        for (int i = 0; i < N_BEAMS; i++) {
            if (beam_active[i]) {
                int centerX = (int)beam_x[i] + BEAM_WIDTH / 2;
                int centerY = (int)beam_y[i] + BEAM_HEIGHT / 2;
                int radius = BEAM_WIDTH / 2;
                for (int r = radius; r > 0; r--) {
                    int red = 255;
                    int green = (int)(100 * (float)r / radius);
                    int blue = (int)(100 * (float)r / radius);
                    iSetColor(red, green, blue);
                    iFilledCircle(centerX, centerY, r);
                }
            }
        }
        for (int i = 0; i < N_COINS; i++) {
            iShowLoadedImage((int)coin_x[i], (int)coin_y[i], &coinFrames[i][coinFrameIndex[i]]);
        }

        
       for (int i = 0; i < N_PIPES; i++) {
    if (currentDifficulty == EASY && i % 4 != 0) continue;

    iShowLoadedImage(pipe_x[i], 0, &lowerPipeImages[i]);
    iShowLoadedImage(pipe_x[i], pipe_gap_y[i] + PIPE_GAP, &upperPipeImages[i]);
}


        if (!gameOver || (int)flashTimer % 10 < 5) {
            int currentFrame = frameBuffer[(frameBufferIndex - 1 + FRAME_BUFFER_SIZE) % FRAME_BUFFER_SIZE];
            if (currentFrame >= 0 && currentFrame < N_FRAMES) {
             iShowLoadedImage((int)bird_x, (int)bird_y, &birdFramesMultiple[currentBirdSpriteSheet][currentFrame]);
            } else {
                iShowLoadedImage((int)bird_x, (int)bird_y, &birdFramesMultiple[currentBirdSpriteSheet][0]);
            }
        }
        for (int i = 0; i < N_GERMS; i++) {
            if (germ_active[i]) {
                iShowLoadedImage((int)germ_x[i], (int)germ_y[i], &germFrames[germFrameIndex]);
            }
        }
        iShowLoadedImage((int)ground_x, 0, &groundImage);
        iShowLoadedImage((int)ground_x + screenWidth, 0, &groundImage);
       std::string scoreStr = "Score: " + std::to_string(score);
    std::string modeStr = "Mode: " + std::string(currentDifficulty == HARD ? "Hard" : currentDifficulty == MEDIUM ? "Medium" : "Easy");

    drawStyledText(50, screenHeight - 250, scoreStr.c_str(), GLUT_BITMAP_TIMES_ROMAN_24, true);
    drawStyledText(50, screenHeight - 290, modeStr.c_str(), GLUT_BITMAP_TIMES_ROMAN_24, true);

     



if (isEnteringName) {
    
    int boxWidth = 1000;  
    int boxHeight = 250; 
    int boxX = (screenWidth - boxWidth) / 2;
    int boxY = (screenHeight - boxHeight) / 2;

   
    for (int i = 0; i < boxHeight; i++) {
        float t = i / (float)boxHeight;
        int r = (int)((1 - t) * 0 + t * 139);    
        int g = 0;
        int b = (int)((1 - t) * 139 + t * 0);    
        iSetColor(r, g, b);
        iLine(boxX, boxY + i, boxX + boxWidth, boxY + i);
    }

 
    iSetColor(255, 255, 255);
    iRectangle(boxX, boxY, boxWidth, boxHeight);

    drawStyledText(boxX + 40, boxY + boxHeight - 60, "Game Over! Enter your name:", GLUT_BITMAP_TIMES_ROMAN_24, true);
    drawStyledText(boxX + 40, boxY + boxHeight / 2 - 10, playerName.c_str(), GLUT_BITMAP_HELVETICA_18);

    static int blinkTimer = 0;
    blinkTimer = (blinkTimer + 1) % 60;
    if (blinkTimer < 30) {
        int charWidth = 18; 
        int cursorX = boxX + 40 + (int)(playerName.length() * charWidth);
        int cursorY = boxY + boxHeight / 2 - 10;
        iSetColor(255, 255, 255);
        iLine(cursorX, cursorY, cursorX, cursorY + 30); 
    }

    drawStyledText(boxX + 40, boxY + 20, "Press Enter to submit, Backspace to delete", GLUT_BITMAP_HELVETICA_12);
} 
     
     
       else if (gameOver) {
        
        iResizeImage(&gameOverImage, 600, 300);
    iShowLoadedImage(screenWidth / 2 - 400, screenHeight / 2-100 , &gameOverImage);

      int rectX = screenWidth / 2 - 400;
    int rectY = screenHeight / 2 - 200;
    int rectWidth = 800;
    int rectHeight = 400;
    int borderRadius = 30;
    drawRoundedGradientRect(rectX, rectY, rectWidth, rectHeight, borderRadius);

std::string modeStr = (currentDifficulty == HARD) ? "Hard" : (currentDifficulty == MEDIUM) ? "Medium" : "Easy";
std::string yourScoreText = "Your Score: " + std::to_string(score) + " (" + modeStr + ")";


iSetColor(150, 0, 0); 
int x = rectX + 50;
int y = rectY + rectHeight - 80;
iText(x - 1, y, yourScoreText.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
iText(x + 1, y, yourScoreText.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
iText(x, y - 1, yourScoreText.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
iText(x, y + 1, yourScoreText.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);


iSetColor(255, 255, 255);
iText(x, y, yourScoreText.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
   
    int highestScore = 0;
    for (int i = 0; i < 5; i++) {
        if (highScores[i].level == modeStr && highScores[i].score > highestScore) {
            highestScore = highScores[i].score;
        }
    }
    std::string highestScoreText = "Highest Score in " + modeStr + ": " + std::to_string(highestScore);
    drawStyledText(rectX + 50, rectY + rectHeight - 140, highestScoreText.c_str(), GLUT_BITMAP_HELVETICA_18);

     struct Button {
            int x, y, w, h;
            const char* label;
        };
        int xOffset = 50; 

        Button buttons[4] = {
            {goRestartX - xOffset, goRestartY, goBtnW, goBtnH, "RESTART"},
            {goHomeX - xOffset, goHomeY, goBtnW, goBtnH, "HOME"},
            {goExitX - xOffset, goExitY, goBtnW, goBtnH, "EXIT"},
            {goSaveX - xOffset, goSaveY, goBtnW, goBtnH, "SAVE"}
        };

        for (int i = 0; i < 4; i++) {
            int btnX = buttons[i].x;
            int btnY = buttons[i].y;
            int btnW = buttons[i].w;
            int btnH = buttons[i].h;
            bool isHovered = (mx >= btnX && mx <= btnX + btnW && my >= btnY && my <= btnY + btnH);

            for (int h = 0; h < btnH; h++) {
                float t = h / (float)btnH;
                int r = (int)((1 - t) * 70 + t * 180);
                int g = 0;
                int b = (int)((1 - t) * 180 + t * 70);
                iSetColor(r, g, b);
                iLine(btnX, btnY + h, btnX + btnW, btnY + h);
            }

            if (isHovered) {
                iSetColor(100, 0, 200);
            } else {
                iSetColor(70, 0, 180);
            }
            iFilledCircle(btnX, btnY + btnH / 2, btnH / 2);
            iFilledCircle(btnX + btnW, btnY + btnH / 2, btnH / 2);

            iSetColor(255, 255, 255);
            iRectangle(btnX, btnY, btnW, btnH);

            int textLen = strlen(buttons[i].label);
            int baseFontWidth = 10;

            if (isHovered) {
                int textX = btnX + btnW / 2 - (textLen * baseFontWidth);
                int textY = btnY + btnH / 2 + 7;

                for (int c = 0; c < textLen; c++) {
                    float t = c / (float)(textLen - 1);
                    int r = (int)((1 - t) * 139 + t * 75);
                    int g = 0;
                    int b = (int)((1 - t) * 0 + t * 130);
                    iSetColor(r, g, b);
                    char ch[2] = { buttons[i].label[c], '\0' };
                    iText(textX + c * (baseFontWidth + 2), textY, ch, GLUT_BITMAP_TIMES_ROMAN_24);
                }
            } else {
                int textX = btnX + btnW / 2 - (textLen * baseFontWidth);
                int textY = btnY + btnH / 2 + 7;
                drawStyledText(textX, textY, buttons[i].label, GLUT_BITMAP_HELVETICA_18, false);
            }
        }
    }

      iShowLoadedImage(backBtnX, backBtnY,
        (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) ?
        &backBtnHover : &backBtn);
        break;
    }
    case HELP: {
                iShowLoadedImage(0, 0, &helpImage);
       
        iShowLoadedImage(backBtnX, backBtnY,
            (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) ?
            &backBtnHover : &backBtn);
        break;

    }
    case CONTINUE: {
        drawStyledText(300, 300, "Continue Screen. Press 'H' to return.", GLUT_BITMAP_HELVETICA_18);
        break;
    }
    case SCORE: {
    iShowLoadedImage(0, 0, &scoreWideImage);

    int startY = screenHeight - 350; 

    int rectX = 300;       
    int rectY = startY - 320; 
    int rectWidth = 800; 
    int rectHeight = 360;  
    int borderRadius = 30; 

    drawRoundedGradientRect(rectX, rectY, rectWidth, rectHeight, borderRadius);

 
    drawStyledText(600, startY + 80, "High Scores:", GLUT_BITMAP_TIMES_ROMAN_24, true);

 
    drawStyledText(600-200, startY, "Rank", GLUT_BITMAP_HELVETICA_18);
    drawStyledText(700-200, startY, "Name", GLUT_BITMAP_HELVETICA_18);
    drawStyledText(950-200, startY, "Score", GLUT_BITMAP_HELVETICA_18);
    drawStyledText(1080-200, startY, "Level", GLUT_BITMAP_HELVETICA_18);
    drawStyledText(1220-200, startY, "Time", GLUT_BITMAP_HELVETICA_18);

    for (int i = 0; i < 5; i++) {
        int yPos = startY - (i + 1) * 50;
        std::string name = highScores[i].name;
        if (name.length() > 25) name = name.substr(0, 25);
        else name += std::string(25 - name.length(), ' ');

        std::string scoreStr = std::to_string(highScores[i].score);
        std::string levelStr = highScores[i].level;
        std::string timeStr = highScores[i].time;

        drawStyledText(600-200, yPos, std::to_string(i + 1).c_str(), GLUT_BITMAP_HELVETICA_18);
        drawStyledText(700-200, yPos, name.c_str(), GLUT_BITMAP_HELVETICA_18);
        drawStyledText(950-200, yPos, scoreStr.c_str(), GLUT_BITMAP_HELVETICA_18);
        drawStyledText(1080-200, yPos, levelStr.c_str(), GLUT_BITMAP_HELVETICA_18);
        drawStyledText(1220-200, yPos, timeStr.c_str(), GLUT_BITMAP_HELVETICA_18);
    }

       iShowLoadedImage(backBtnX, backBtnY,
            (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) ?
            &backBtnHover : &backBtn);
    break;
}

    case PAUSE: {

         iSetColor(135, 206, 235);
            iFilledRectangle(0, 0, screenWidth, screenHeight);
            for (int i = 0; i < N_CLOUDS; i++) {
                iShowLoadedImage((int)cloud_x[i], (int)cloud_y[i], &cloudImages[i]);
            }
            for (int i = 0; i < N_BEAMS; i++) {
                if (beam_active[i]) {
                    int centerX = (int)beam_x[i] + BEAM_WIDTH / 2;
                    int centerY = (int)beam_y[i] + BEAM_HEIGHT / 2;
                    int radius = BEAM_WIDTH / 2;
                    for (int r = radius; r > 0; r--) {
                        int red = 255;
                        int green = (int)(100 * (float)r / radius);
                        int blue = (int)(100 * (float)r / radius);
                        iSetColor(red, green, blue);
                        iFilledCircle(centerX, centerY, r);
                    }
                }
            }
            for (int i = 0; i < N_COINS; i++) {
                iShowLoadedImage((int)coin_x[i], (int)coin_y[i], &coinFrames[i][coinFrameIndex[i]]);
            }
            for (int i = 0; i < N_PIPES; i++) {
                if (currentDifficulty == EASY && i % 4 != 0) continue;
                iShowLoadedImage(pipe_x[i], 0, &lowerPipeImages[i]);
                iShowLoadedImage(pipe_x[i], pipe_gap_y[i] + PIPE_GAP, &upperPipeImages[i]);
            }
            if (!gameOver || (int)flashTimer % 10 < 5) {
                int currentFrame = frameBuffer[(frameBufferIndex - 1 + FRAME_BUFFER_SIZE) % FRAME_BUFFER_SIZE];
                if (currentFrame >= 0 && currentFrame < N_FRAMES) {
                    iShowLoadedImage((int)bird_x, (int)bird_y, &birdFramesMultiple[currentBirdSpriteSheet][currentFrame]);
                } else {
                    iShowLoadedImage((int)bird_x, (int)bird_y, &birdFramesMultiple[currentBirdSpriteSheet][0]);
                }
            }
            for (int i = 0; i < N_GERMS; i++) {
                if (germ_active[i]) {
                    iShowLoadedImage((int)germ_x[i], (int)germ_y[i], &germFrames[germFrameIndex]);
                }
            }
            iShowLoadedImage((int)ground_x, 0, &groundImage);
            iShowLoadedImage((int)ground_x + screenWidth, 0, &groundImage);
        

            std::string scoreStr = "Score: " + std::to_string(score);
            std::string modeStr = "Mode: " + std::string(currentDifficulty == HARD ? "Hard" : currentDifficulty == MEDIUM ? "Medium" : "Easy");
            drawStyledText(50, screenHeight - 250, scoreStr.c_str(), GLUT_BITMAP_TIMES_ROMAN_24, true);
            drawStyledText(50, screenHeight - 290, modeStr.c_str(), GLUT_BITMAP_TIMES_ROMAN_24, true);


    int panelX = screenWidth / 2 - 350;
    int panelY = screenHeight / 2 - 200;
    int panelWidth = 700;
    int panelHeight = 400;
    int panelRadius = 40;
    drawRoundedGradientRect(panelX, panelY, panelWidth, panelHeight, panelRadius);


    drawStyledText(panelX + panelWidth / 2 - 60, panelY + panelHeight - 80, "Paused", GLUT_BITMAP_TIMES_ROMAN_24, true);


    struct Button {
        int x, y, w, h;
        const char* label;
    };

    int btnWidth = 200;
    int btnHeight = 70;
    int btnSpacing = 40;
    int startX = panelX + (panelWidth - btnWidth) / 2;
    int startY = panelY + panelHeight - 160;

    Button buttons[3] = {
        {startX, startY, btnWidth, btnHeight, "Resume"},
        {startX, startY - (btnHeight + btnSpacing), btnWidth, btnHeight, "Restart"},
        {startX, startY - 2 * (btnHeight + btnSpacing), btnWidth, btnHeight, "Exit"}
    };

    for (int i = 0; i < 3; i++) {
        int btnX = buttons[i].x;
        int btnY = buttons[i].y;
        int btnW = buttons[i].w;
        int btnH = buttons[i].h;
        bool isHovered = (mx >= btnX && mx <= btnX + btnW && my >= btnY && my <= btnY + btnH);


        for (int h = 0; h < btnH; h++) {
            float t = h / (float)btnH;
            int r = (int)((1 - t) * 70 + t * 180);
            int g = 0;
            int b = (int)((1 - t) * 180 + t * 70);
            iSetColor(r, g, b);
            iLine(btnX, btnY + h, btnX + btnW, btnY + h);
        }

        
        if (isHovered) {
            iSetColor(100, 0, 200);
        } else {
            iSetColor(70, 0, 180);
        }
        iFilledCircle(btnX, btnY + btnH / 2, btnH / 2);
        iFilledCircle(btnX + btnW, btnY + btnH / 2, btnH / 2);

    
        iSetColor(255, 255, 255);
        iRectangle(btnX, btnY, btnW, btnH);


        int textLen = strlen(buttons[i].label);
        int baseFontWidth = 10; 
        int textX = btnX + btnW / 2 - (textLen * baseFontWidth);
        int textY = btnY + btnH / 2 + 7;
        drawStyledText(textX, textY, buttons[i].label, GLUT_BITMAP_HELVETICA_18, isHovered);
    }

    break;
}
    }

    if (gameState ==GAME) {
        int minutes = elapsedTimeMs / 60000;
        int seconds = (elapsedTimeMs / 1000) % 60;
        char timeStr[16];
        sprintf(timeStr, "Time: %02d:%02d", minutes, seconds);
        drawStyledText(screenWidth - 200, screenHeight - 250, timeStr, GLUT_BITMAP_TIMES_ROMAN_24, true);
    }





     if (terminalActive) {
        int termX = 50;
        int termY = 100;
        int lineHeight = 20;
        int width = screenWidth - 100;
        int height = lineHeight * (maxTerminalLines + 2);

        

        
        iSetColor(0, 0, 0);
        iFilledRectangle(termX - 10, termY - 10, width + 20, height + 20);

     
    int closeBtnSize = 20;
    int closeBtnX = termX + width + 5;
    int closeBtnY = termY + height + 5;
    iSetColor(200, 0, 0);
    iFilledRectangle(closeBtnX, closeBtnY, closeBtnSize, closeBtnSize);
    iSetColor(255, 255, 255);
    iLine(closeBtnX + 4, closeBtnY + 4, closeBtnX + closeBtnSize - 4, closeBtnY + closeBtnSize - 4);
    iLine(closeBtnX + 4, closeBtnY + closeBtnSize - 4, closeBtnX + closeBtnSize - 4, closeBtnY + 4);

       
        iSetColor(0, 255, 0);
        for (int i = 0; i < (int)terminalOutput.size(); i++) {
            iText(termX, termY + height - (i + 2) * lineHeight, terminalOutput[i].c_str(), GLUT_BITMAP_8_BY_13);
        }

       
        std::string prompt = "> " + terminalInput + ((glutGet(GLUT_ELAPSED_TIME) / 500) % 2 ? "_" : " ");
        iText(termX, termY + height - lineHeight, prompt.c_str(), GLUT_BITMAP_8_BY_13);
    }
if (gameState == CREDIT) {
    
iShowLoadedImage(0, 0, &creditbg);   

    
    int panelX = screenWidth / 2 - 600;
    int panelY = screenHeight / 2 - 300;
    int panelWidth = 1200;
    int panelHeight = 600;
    int panelRadius = 40;
    drawRoundedGradientRect(panelX, panelY, panelWidth, panelHeight, panelRadius);


    drawStyledText(panelX + 50, panelY + panelHeight - 100, "Special Thanks to Our Honorable Advisor:", GLUT_BITMAP_TIMES_ROMAN_24, true);
    drawStyledText(panelX + 50, panelY + panelHeight - 170, "Dr. Md. Mostofa Akbar.", GLUT_BITMAP_TIMES_ROMAN_24, true);
    drawStyledText(panelX + 50, panelY + panelHeight - 220, "Professor, Dept. of CSE, BUET.", GLUT_BITMAP_HELVETICA_18);

 
    int startY = panelY + panelHeight - 320;
    drawStyledText(panelX + 50, startY, "Team Partners :", GLUT_BITMAP_HELVETICA_18);
    drawStyledText(panelX + 50, startY - 50, "Naimul Islam (2405062)", GLUT_BITMAP_HELVETICA_18);
    drawStyledText(panelX + 50, startY - 100, "Al Nahian Alif (2405067)", GLUT_BITMAP_HELVETICA_18);
    drawStyledText(panelX + 50, startY - 150, "CSE-24, BUET", GLUT_BITMAP_HELVETICA_18);


    iShowLoadedImage(backBtnX, backBtnY,
        (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) ?
        &backBtnHover : &backBtn);
}

    
}


// ================== EVENT HANDLERS ==================
void iMouseMove(int mx_, int my_) {
    mx = mx_;
    my = my_;
}

void iMouse(int button, int state, int mx, int my) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
      iPlaySound("mouse_click.wav");

    
    if (terminalActive) {
        int termX = 50;
        int termY = 100;
        int lineHeight = 20;
        int width = screenWidth - 100;
        int height = lineHeight * (maxTerminalLines + 2);

        int closeBtnSize = 20;
        int closeBtnX = termX + width + 5;
        int closeBtnY = termY + height + 5;

        if (mx >= closeBtnX && mx <= closeBtnX + closeBtnSize &&
            my >= closeBtnY && my <= closeBtnY + closeBtnSize) {
            terminalActive = false;
            addterminaltext("Terminal closed.");
            return;
        }
    }

   
    if (gameOver) {
        if (mx >= goRestartX && mx <= goRestartX + goBtnW && my >= goRestartY && my <= goRestartY + goBtnH) {
            resetGame();
            gameState = GAME;
            return;
        }
        if (mx >= goHomeX && mx <= goHomeX + goBtnW && my >= goHomeY && my <= goHomeY + goBtnH) {
            gameState = MENU;
            pauseAllTimers();
            return;
        }
        if (mx >= goExitX && mx <= goExitX + goBtnW && my >= goExitY && my <= goExitY + goBtnH) {
            exit(0);
        }
        if (mx >= goSaveX && mx <= goSaveX + goBtnW && my >= goSaveY && my <= goSaveY + goBtnH) {
            isEnteringName = true;
            playerName = "";
            nameCharIndex = 0;
            return;
        }
    }

   
    if (gameState != MENU) {
        if (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) {
            gameState = MENU;
            pauseAllTimers();
            return;
        }
    }

    
    if (gameState == CREDIT) {
        if (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) {
            gameState = MENU;
            return;
        }
    }


    if (gameState == HELP) {
        if (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) {
            gameState = MENU;
            return;
        }
    }


    if (gameState != MENU && mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) {
        gameState = MENU;
        if (gameState == GAME || gameState == PAUSE) pauseAllTimers();
        return;
    }

   
    if (gameState == MENU) {
        int settingsX = screenWidth - 250;
        int settingsY = screenHeight - 250;

        int creditW = 120;
        int creditH = 60;

       int creditX = settingsX - creditW - 40;
    int creditY = settingsY+10;
        

        
        if (mx >= creditX && mx <= creditX + creditW && my >= creditY && my <= creditY + creditH) {
            gameState = CREDIT;
            return;
        }

        if (mx >= settingsX && mx <= settingsX + 200 && my >= settingsY && my <= settingsY + 100) {
            gameState = SETTINGS;
            return;
        }

        if (mx >= shellBtnX && mx <= shellBtnX + shellBtnW && my >= shellBtnY && my <= shellBtnY + shellBtnH) {
            terminalActive = !terminalActive;
            if (terminalActive) {
                terminalInput.clear();
                terminalOutput.clear();
                addterminaltext("Terminal opened. Type commands.");
            }
            return;
        }

        // Play button
        if (mx >= playX && mx <= playX + btnW && my >= playY && my <= playY + btnH) {
            gameState = GAME;
            resetGame();
            return;
        }

        // Help button
        if (mx >= helpX && mx <= helpX + btnW && my >= helpY && my <= helpY + btnH) {
            gameState = HELP;
            return;
        }

        // Exit button
        if (mx >= exitX && mx <= exitX + btnW && my >= exitY && my <= exitY + btnH) {
            exit(0);
        }

        // Continue button
        if (mx >= continueX && mx <= continueX + btnW && my >= continueY && my <= continueY + btnH) {
            if (isGamePaused) {
                isGamePaused = false;
                resumeAllTimers();
            }
            gameState = GAME;
            return;
        }

        // Score button
        if (mx >= scoreX && mx <= scoreX + btnW && my >= scoreY && my <= scoreY + btnH) {
            gameState = SCORE;
            return;
        }

        // Level select button
        if (mx >= levelX && mx <= levelX + btnW && my >= levelY && my <= levelY + btnH) {
            gameState = LEVEL_SELECT;
            return;
        }
    }

   
    if (gameState == LEVEL_SELECT) {
        if (mx >= easyX && mx <= easyX + levelBtnW && my >= easyY && my <= easyY + levelBtnH) {
            currentDifficulty = EASY;
            gameState = GAME;
            resetGame();
            iPlaySound("button_click.wav");
            return;
        }
        if (mx >= mediumX && mx <= mediumX + levelBtnW && my >= mediumY && my <= mediumY + levelBtnH) {
            currentDifficulty = MEDIUM;
            gameState = GAME;
            resetGame();
            iPlaySound("button_click.wav");
            return;
        }
        if (mx >= hardX && mx <= hardX + levelBtnW && my >= hardY && my <= hardY + levelBtnH) {
            currentDifficulty = HARD;
            gameState = GAME;
            resetGame();
            iPlaySound("button_click.wav");
            return;
        }
        if (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) {
            gameState = MENU;
            iPlaySound("button_click.wav");
            return;
        }
    }

    // Settings state buttons
    if (gameState == SETTINGS) {
        if (mx >= backBtnX && mx <= backBtnX + backBtnW && my >= backBtnY && my <= backBtnY + backBtnH) {
            gameState = MENU;
            return;
        }

        int panelX = screenWidth / 2 - 450;
        int panelY = screenHeight / 2 - 250;
        int volumeBarX = panelX + 230;
        int volumeBarY = panelY + 500 - 210;
        int volumeBarWidth = 300;
        int iconSize = 30;
        int minusIconX = volumeBarX - 50;
        int minusIconY = volumeBarY - 3;
        int plusIconX = volumeBarX + volumeBarWidth + 20;
        int plusIconY = volumeBarY - 3;

        if (mx >= panelX + 230 && mx <= panelX + 270 && my >= panelY + 500 - 160 && my <= panelY + 500 - 130) {
            soundOn = !soundOn;
            if (soundOn) {Mix_ResumeMusic();  iResumeBackgroundMusicWav();}
            else {Mix_PauseMusic();    iPauseBackgroundMusicWav();}
            return;
        }
bgMusicVolume += 8;
        // Volume minus
        if (mx >= minusIconX && mx <= minusIconX + iconSize && my >= minusIconY && my <= minusIconY + iconSize) {
            soundVolume -= 8;
            if (soundVolume < 0) soundVolume = 0;
            Mix_VolumeMusic(soundVolume);

            if (bgMusicVolume < 0) bgMusicVolume = 0;
             bgMusicVolume -= 8;
iSetBackgroundMusicVolume(bgMusicVolume);
            soundOn = (soundVolume > 0);
            return;
        }

        // Volume plus
        if (mx >= plusIconX && mx <= plusIconX + iconSize && my >= plusIconY && my <= plusIconY + iconSize) {
            soundVolume += 8;
            if (soundVolume > MIX_MAX_VOLUME) soundVolume = MIX_MAX_VOLUME;
            Mix_VolumeMusic(soundVolume);

            if (bgMusicVolume > MIX_MAX_VOLUME) bgMusicVolume = MIX_MAX_VOLUME;
            bgMusicVolume += 8;
iSetBackgroundMusicVolume(bgMusicVolume);

            soundOn = (soundVolume > 0);
            return;
        }



        

     
        if (mx >= panelX + 280 && mx <= panelX + 380 && my >= panelY + 500 - 270 && my <= panelY + 500 - 230) {
            editingWidth = true;
            editingHeight = false;
            return;
        }

     
        if (mx >= panelX + 280 && mx <= panelX + 380 && my >= panelY + 500 - 320 && my <= panelY + 500 - 280) {
            editingHeight = true;
            editingWidth = false;
            return;
        }
    }

    // Pausebutton
    if (gameState == PAUSE) {
        int panelX = screenWidth / 2 - 350;
        int panelY = screenHeight / 2 - 200;
        int panelWidth = 700;
        int panelHeight = 400;
        int btnWidth = 200;
        int btnHeight = 70;
        int btnSpacing = 40;
        int startX = panelX + (panelWidth - btnWidth) / 2;
        int startY = panelY + panelHeight - 160;

        struct Button {
            int x, y, w, h;
            const char* label;
        };

        Button buttons[3] = {
            {startX, startY, btnWidth, btnHeight, "Resume"},
            {startX, startY - (btnHeight + btnSpacing), btnWidth, btnHeight, "Restart"},
            {startX, startY - 2 * (btnHeight + btnSpacing), btnWidth, btnHeight, "Exit"}
        };

        for (int i = 0; i < 3; i++) {
            if (mx >= buttons[i].x && mx <= buttons[i].x + buttons[i].w &&
                my >= buttons[i].y && my <= buttons[i].y + buttons[i].h) {
                if (strcmp(buttons[i].label, "Resume") == 0) {
                    isGamePaused = false;
                    gameState = GAME;
                    resumeAllTimers();
                } else if (strcmp(buttons[i].label, "Restart") == 0) {
                    resetGame();
                    gameState = GAME;
                    resumeAllTimers();
                } else if (strcmp(buttons[i].label, "Exit") == 0) {
                    isGamePaused = false;
                    gameState = MENU;
                       pauseAllTimers(); 
                }
                return;
            }
        }
    }
}

void iKeyboard(unsigned char key) {

      if (terminalActive) {
        if (key == 13) {
            if (!terminalInput.empty() && terminalInput.length() <= maxInputLength) {
                processTerminalCommand(terminalInput);
                terminalInput.clear();
            }
        } else if (key == 8) { 
            if (!terminalInput.empty()) {
                terminalInput.pop_back();
            }
        } else if (key == 27) { 
            terminalActive = false;
            addterminaltext("Terminal closed.");
        } else if (key >= 32 && key <= 126 && terminalInput.length() < maxInputLength) {
            terminalInput += key;
        }
        return; 
    }

    else {
       
        if (key == 'c' || key == 'C') {
            terminalActive = true;
            terminalInput.clear();
            terminalOutput.clear();
            addterminaltext("Terminal opened. Type commands.");
            return;
        }
    }


    if (gameState == SETTINGS) {
        if (editingWidth) {
            if (key == 13) { 
                try {
                    int val = std::stoi(widthInput);
                    if (val >= 800 && val <= 3840) {
                        newScreenWidth = val;
                        screenWidth = val;
                        glutReshapeWindow(screenWidth, screenHeight);
                    }
                } catch (...) {}
                editingWidth = false;
            } else if (key == 8 && !widthInput.empty()) {
                widthInput.pop_back();
            } else if (key >= '0' && key <= '9') {
                widthInput += key;
            }
            return;
        }
        if (editingHeight) {
            if (key == 13) { 
                try {
                    int val = std::stoi(heightInput);
                    if (val >= 600 && val <= 2160) {
                        newScreenHeight = val;
                        screenHeight = val;
                        glutReshapeWindow(screenWidth, screenHeight);
                    }
                } catch (...) {}
                editingHeight = false;
            } else if (key == 8 && !heightInput.empty()) {
                heightInput.pop_back();
            } else if (key >= '0' && key <= '9') {
                heightInput += key;
            }
            return;
        }

        if (key == 's' || key == 'S') {
            soundOn = !soundOn;
            if (soundOn) Mix_ResumeMusic();
            else Mix_PauseMusic();
        }
        if (key == '+' || key == '=') {
            soundVolume += 8;
            if (soundVolume > MIX_MAX_VOLUME) soundVolume = MIX_MAX_VOLUME;
            Mix_VolumeMusic(soundVolume);
        }
        if (key == '-' || key == '_') {
            soundVolume -= 8;
            if (soundVolume < 0) soundVolume = 0;
            Mix_VolumeMusic(soundVolume);
        }
        if (key == 'b' || key == 'B') {
            gameState = MENU;
        }
        return;
    }

    if (isEnteringName) {
        if (key == 13) { 
            isEnteringName = false;
            addHighScore(playerName, score);
            gameState = SCORE;
            playerName = "";
            nameCharIndex = 0;
        } else if (key == 8 && nameCharIndex > 0) { 
            playerName.pop_back();
            nameCharIndex--;
        } else if (nameCharIndex < 49 && key >= 32 && key <= 126) {
            playerName += key;
            nameCharIndex++;
        }
        return;
    }

    if (gameState == GAME || gameState == PAUSE) {
        if (key == ' ') {
            if (gameState == PAUSE || isGamePaused ) {
                isGamePaused = false;
                gameState = GAME;
                resumeAllTimers();  
            }
            if (gameState == GAME && !gameOver) {
                int currentTime = glutGet(GLUT_ELAPSED_TIME);
                if (jumpQueue.canJump(currentTime)) {
                    jumpQueue.enqueue(currentTime);
                    bird_velocity = JUMP_VELOCITY;
                    iPlaySound("flappy_jump.wav");
                }
            }
        } else if (key == 'p' && !gameOver) {
            if (isGamePaused) {
                isGamePaused = false;
                gameState = GAME;
                resumeAllTimers();
            } else {
                isGamePaused = true;
                gameState = PAUSE;
                pauseAllTimers();
            }
        }
        else if (key == 27 && gameState == GAME && !gameOver) {  // esc press and functions
            isGamePaused = true;
            gameState = PAUSE;
            pauseAllTimers();
        } 
         else if (key == 'b' && !gameOver) {
            isGamePaused = true;
            pauseAllTimers();
            gameState = MENU;
        } else if (key == 'r' || key == 'R') {
            resetGame();
        } else if (key == 13 && gameOver) {
            isEnteringName = true;
            playerName = "";
            nameCharIndex = 0;
        } else if (key == 27 && gameOver) {
            gameState = MENU;
            pauseAllTimers();
        }
        return;
    }

    if (key == 'h' || key == 'b') {
        gameState = MENU;
        if (gameState == GAME || gameState == PAUSE) pauseAllTimers();
    }
}
 void iSpecialKeyboardUp(int key, int x, int y) {
    if (gameState == GAME && !gameOver && !isGamePaused) {
        if (key == GLUT_KEY_RIGHT) {
            bird_velocity_x = 0.0f;
            printf("Right arrow released: bird_velocity_x = %.2f\n", bird_velocity_x);
        }
    }
}


void iSpecialKeyboard(unsigned char key) {
   
}

   
void specialKeyboardHandler(int key, int x, int y) {
    if (gameState == GAME && !gameOver && !isGamePaused) {
        switch (key) {
            case GLUT_KEY_UP:
                if (jumpQueue.canJump(glutGet(GLUT_ELAPSED_TIME))) {
                    jumpQueue.enqueue(glutGet(GLUT_ELAPSED_TIME));
                    bird_velocity = JUMP_VELOCITY;
                    iPlaySound("flappy_jump.wav");
                    printf("Up arrow pressed: bird_velocity = %.2f\n", bird_velocity);
                }
                break;
            case GLUT_KEY_DOWN:
                bird_velocity = -JUMP_VELOCITY;
                printf("Down arrow pressed: bird_velocity = %.2f\n", bird_velocity);
                break;
            case GLUT_KEY_RIGHT:
                bird_velocity_x = 5.0f;
                printf("Right arrow pressed: bird_velocity_x = %.2f\n", bird_velocity_x);
                break;
            default:
                break;
        }
    } 
    printf("Game state: %d, gameOver: %d, isGamePaused: %d\n", gameState, gameOver, isGamePaused);
}


void iSpecialKeyboard(int key, int x, int y) {
    if (gameState == GAME && !gameOver && !isGamePaused) {
        switch (key) {
            case GLUT_KEY_UP:
                if (jumpQueue.canJump(glutGet(GLUT_ELAPSED_TIME))) {
                    jumpQueue.enqueue(glutGet(GLUT_ELAPSED_TIME));
                    bird_velocity = JUMP_VELOCITY;
                    iPlaySound("flappy_jump.wav");
                    printf("Up arrow pressed: bird_velocity = %.2f\n", bird_velocity);
                }
                break;
            case GLUT_KEY_DOWN:
                bird_velocity = -JUMP_VELOCITY;
                printf("Down arrow pressed: bird_velocity = %.2f\n", bird_velocity);
                break;
            case GLUT_KEY_RIGHT:
                bird_velocity_x = 5.0f;
                printf("Right arrow pressed: bird_velocity_x = %.2f\n", bird_velocity_x);
                break;
            default:
                break;
        }
    } 
    printf("Game state: %d, gameOver: %d, isGamePaused: %d\n", gameState, gameOver, isGamePaused);
}


void iMouseDrag(int mx, int my) {
    
}
void iMouseWheel(int dir, int mx, int my) {}

// ================== CLEANUP FUNCTION ==================
void iCleanup() {
    iFreeImage(&background);
    iFreeImage(&groundImage);
    iFreeImage(&gameOverImage);
    iFreeImage(&helpImage);
    iFreeImage(&scoreImage);
    iFreeImage(&scoreDisplayImage);
    iFreeImage(&play);
    iFreeImage(&playHover);
    iFreeImage(&help);
    iFreeImage(&helpHover);
    iFreeImage(&quit);
    iFreeImage(&quitHover);
    iFreeImage(&cont);
    iFreeImage(&contHover);
    iFreeImage(&scoreBtn);
    iFreeImage(&scoreBtnHover);
    iFreeImage(&level);
    iFreeImage(&levelHover);
    iFreeImage(&backBtn);
    iFreeImage(&backBtnHover);
    iFreeImage(&scoreWideImage);

    iStopBackgroundMusicWav();


    for (int i = 0; i < N_FRAMES; i++) iFreeImage(&birdFrames[i]);
    for (int i = 0; i < N_COINS; i++) {
        for (int f = 0; f < COIN_FRAMES; f++) iFreeImage(&coinFrames[i][f]);
    }
    for (int i = 0; i < N_CLOUDS; i++) iFreeImage(&cloudImages[i]);
    for (int i = 0; i < N_PIPES; i++) {
        iFreeImage(&lowerPipeImages[i]);
        iFreeImage(&upperPipeImages[i]);
    }
    for (int i = 0; i < GERM_FRAMES; i++) iFreeImage(&germFrames[i]);

    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
    }
}

// ================== MAIN FUNCTION ==================
int main(int argc, char *argv[]) {


    //  SDL_mixer , for linux not windows
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! Error: %s\n", SDL_GetError());
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! Error: %s\n", Mix_GetError());
        return 1;
    }
    
   

    srand((unsigned)time(NULL));
    glutInit(&argc, argv);

    glutSpecialFunc(specialKeyboardHandler);
    
    glutSpecialUpFunc(iSpecialKeyboardUp);

    terminalActive = false;
    terminalInput.clear();
    terminalOutput.clear();

  
gameState = LOADING;
loadingProgress = 0;
loadingTimer = iSetTimer(100, updateLoading);

music1 = Mix_LoadMUS("music1.mp3");
if (!music1) printf("");

music2 = Mix_LoadMUS("music2.mp3");
if (!music2) printf("");

// Start playing default music
currentMusic = music1;
if (currentMusic) Mix_PlayMusic(currentMusic, -1);
iPlayBackgroundMusicWav("background.wav");


     screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
     int windowWidth = screenWidth ;
    int windowHeight = screenHeight;

     // Load MP3 music
    backgroundMusic = Mix_LoadMUS("background.mp3");
    if (!backgroundMusic) {
        printf("");
      
    } else {
        Mix_PlayMusic(backgroundMusic, -1);
    }

    if (!loadImageWithCheck(background, "jjkrealm.png",  windowWidth, windowHeight)) exit(1);
    if (!loadImageWithCheck(groundImage, "ground.png",  windowWidth, 50)) exit(1);
    if (!loadImageWithCheck(gameOverImage, "gameoverpage.png", 800, 600)) exit(1);
    if (!loadImageWithCheck(helpImage, "hpage.png", screenWidth, screenHeight)) exit(1);
    if (!loadImageWithCheck(scoreImage, "scorebg.png", 1040, 780)) exit(1);
    if (!loadImageWithCheck(scoreDisplayImage, "scr.png", 300, 100)) exit(1);
    if (!loadImageWithCheck(play, "newgame.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(playHover, "newgamehover.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(help, "help.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(helpHover, "helphover.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(quit, "quit.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(quitHover, "exithover.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(cont, "continue.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(contHover, "continuehover.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(scoreBtn, "score.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(scoreBtnHover, "scorehover.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(level, "level.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(levelHover, "levelhover.png", 200, 100)) exit(1);
    if (!loadImageWithCheck(backBtn, "back.png", backBtnW, backBtnH)) exit(1);
    if (!loadImageWithCheck(backBtnHover, "back_hover.png", backBtnW, backBtnH)) exit(1);
  if (!loadImageWithCheck(scoreWideImage, "jjkrealm.png", screenWidth, screenHeight)) exit(1);
  if (!loadImageWithCheck(creditbg, "creditbg.jpg", screenWidth, screenHeight)) exit(1);

  if (!loadImageWithCheck(settingsBtn, "settings_hover.png", 80, 80)) exit(1);
if (!loadImageWithCheck(settingsBtnHover, "settings.png", 80, 80)) exit(1);

for (int sheet = 0; sheet < NUM_BIRD_SPRITESHEETS; sheet++) {
        for (int i = 0; i < N_FRAMES; i++) {
            std::string filename = "bird" + std::to_string(sheet + 1) + " (" + std::to_string(i + 1) + ").png";
            if (!loadImageWithCheck(birdFramesMultiple[sheet][i], filename.c_str(), BIRD_WIDTH + 10, BIRD_HEIGHT + 10)) {
              
                exit(1);
            }
        }
    }
  
    for (int i = 0; i < N_COINS; i++) {
        for (int f = 0; f < COIN_FRAMES; f++) {
            std::string filename = "coin (" + std::to_string(f + 1) + ").png";
            if (!loadImageWithCheck(coinFrames[i][f], filename.c_str(), COIN_WIDTH, COIN_HEIGHT)) exit(1);
        }
    }
    for (int i = 0; i < N_CLOUDS; i++) {
        if (!loadImageWithCheck(cloudImages[i], "cloud.png", 300, 150)) exit(1);
        cloud_x[i] = screenWidth + i * 400;
    }
    for (int i = 0; i < N_PIPES; i++) {
        if (!loadImageWithCheck(lowerPipeImages[i], "lowerpipe.png")) exit(1);
        if (!loadImageWithCheck(upperPipeImages[i], "upperpipe.png")) exit(1);
    }
    for (int i = 0; i < GERM_FRAMES; i++) {
        std::string filename = "kit (" + std::to_string(i + 1) + ").png";
        if (!loadImageWithCheck(germFrames[i], filename.c_str(), GERM_WIDTH, GERM_HEIGHT)) exit(1);
    }

    loadHighScores();
    resetGame();
    

    animTimer = iSetTimer(60, updateBirdAnimation);
    coinAnimTimer = iSetTimer(16, updateCoinAnimation);
    germAnimTimer = iSetTimer(16, updateGermAnimation);
    physicsTimer = iSetTimer(16, updateGame);
    beamSpawnTimer = iSetTimer(3000, updateBeams);
    timers[0] = animTimer;
    timers[1] = coinAnimTimer;
    timers[2] = germAnimTimer;
    timers[3] = physicsTimer;
    timers[4] = beamSpawnTimer;
    pauseAllTimers();

        iInitialize(windowWidth, windowHeight, "Flappy Bird");
        glutSpecialFunc(iSpecialKeyboard);
    atexit(iCleanup);
    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}