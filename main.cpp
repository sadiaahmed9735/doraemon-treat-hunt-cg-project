#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TOTAL_OBSTACLES 4
#define TOTAL_FIRES 3
#define TOTAL_STONES 3
#define TOTAL_PARTICLES 300

// World / character constants
const float GROUND_Y = -7.5f;          // charY when standing on the ground
const float CHAR_START_X = -9.5f;
const float CHAR_HITBOX_OFFSET_X = 4.75f; // charX -> hitbox left edge on screen
const float CHAR_HITBOX_SIZE = 0.9f;
const float SCROLL_RIGHT_X = -6.0f;    // charX where the world starts scrolling
const float SCROLL_LEFT_X = -5.0f;
const float MAX_CHAR_X = 3.2f;
const float MIN_CHAR_X = -12.5f;
const float SCREEN_LEFT = -9.5f;
const float SCREEN_RIGHT = 9.5f;
const float SCREEN_BOTTOM = -10.0f;
const float SCREEN_TOP = 10.0f;
const int   TOTAL_CAKES = 5;

// Doraemon() model coordinates (drawn at DORAEMON_SCALE, pivot CHAR_HITBOX_OFFSET_X)
const float DORAEMON_SCALE = 0.5f;
const float DORAEMON_HEAD_X = 5.46f;     // head top
const float DORAEMON_HEAD_TOP = 9.52f;
const float DORAEMON_BODY_LEFT = 3.6f;   // body box used for obstacle collision
const float DORAEMON_BODY_RIGHT = 7.5f;
const float DORAEMON_BODY_BOTTOM = 1.9f;
const float DORAEMON_BODY_TOP = 9.3f;

//Character Movement
float charX = CHAR_START_X;
float charY = GROUND_Y;
bool moving = false;
bool jumping = false;
int facing = 1;
float legAngle = 0.0f;
int legDir = 1;
float jumpSpeed = 0.1f;

float _moveSpeed = 0.2f;
float _legSwingSpeed = 2.0f;
float _legResetSpeed = 2.0f;
float _maxLegAngle = 3.0f;

// Gravity jump (+ double jump)
float velY = 0.0f;
int jumpsUsed = 0;
const float GRAVITY = 0.012f;
const float JUMP_VELOCITY = 0.36f;
const float DOUBLE_JUMP_VELOCITY = 0.30f;
const int   MAX_JUMPS = 2;

//Scene Movement
float worldX = 0.0f;
int sceneCount = 0;
int totalScenes = 4;
float sceneWidth = 23.0f;

// Doracakes
float doracakeX[5];
float doracakeY[5];
bool doracakeCollected[5] = { false, false, false, false, false };
int totalDoracakes = 5;
int doracakesCollected = 0;

// Hearts
float heartX[3];
float heartY[3];
bool heartLost[3] = { false, false, false };
int totalHearts = 3;
int lives = 3;
int totalLevels = 3;
int currentLevel = 0;

// Game end screen
bool showEndScreen = false;
bool gameWon = false;
bool showHome = true;

// Button positions
float buttonWidth = 3.6f;
float buttonHeight = 0.75f;
float nextLevelX = -1.8f, nextLevelY = -0.2f;
float mainMenuX = -1.8f, mainMenuY = -1.2f;
float restartX = -1.8f, restartY = -2.2f;

// Score
const int CAKE_POINTS = 10;
const int TIME_BONUS_SECONDS = 60;   // finish faster than this for a bonus
const int TIME_BONUS_PER_SECOND = 2;
const char* HIGHSCORE_FILE = "highscore.txt";
int score = 0;
int levelStartScore = 0;             // score is restored to this on Restart
int highScore = 0;
int lastTimeBonus = 0;
int levelFrames = 0;                 // 60 frames = 1 second
bool paused = false;

// Invincibility after a hit
const int INVINCIBLE_FRAMES = 45;
int invincibleTimer = 0;
float hitFlash = 0.0f;               // red screen flash alpha

// Take-copter gadget
const int COPTER_FRAMES = 300;       // ~5 seconds of flying
const float COPTER_LIFT = 0.15f;
const float COPTER_SINK = 0.05f;
const float COPTER_MAX_Y = 2.0f;
float copterX, copterY;
bool copterPicked = false;
int copterTimer = 0;
bool upHeld = false;
bool leftHeld = false;
bool rightHeld = false;
float propellerAngle = 0.0f;

// Mouse enemy (Doraemon is scared of mice)
const float MOUSE_SPEED = 0.06f;
const float MOUSE_SCARE_DIST = 2.5f;
const int SCARED_FRAMES = 45;
const float SCARED_SPEED_FACTOR = 1.6f;   // runs away faster than walking
const int SCARE_COOLDOWN_FRAMES = 180;  // after a scare Doraemon can walk past the mouse
float mouseX, mouseY;
int mouseDir = -1;
float mouseLeftBound, mouseRightBound;
int scaredTimer = 0;
int scareCooldown = 0;
int scaredDir = 1;

// Particles
struct Particle {
    float x, y;       // world coordinates (screen x = x + worldX)
    float vx, vy;
    float life;       // 1 -> 0
    float r, g, b;
};
Particle particles[TOTAL_PARTICLES];

// Position
float obstaclePosX[TOTAL_OBSTACLES];
float obstaclePosY[TOTAL_OBSTACLES];

// Active & level
bool obstacleEnabled[TOTAL_OBSTACLES];
int obstacleLevel[TOTAL_OBSTACLES];
bool obstacleHit[TOTAL_OBSTACLES] = {false};

// Movement
float obstacleMoveSpeedX[TOTAL_OBSTACLES];
int obstacleMoveDirX[TOTAL_OBSTACLES];  // 1 = right, -1 = left
float obstacleLeftBound[TOTAL_OBSTACLES];
float obstacleRightBound[TOTAL_OBSTACLES];

float obstacleMoveSpeedY[TOTAL_OBSTACLES];
int obstacleMoveDirY[TOTAL_OBSTACLES];  // 1 = up, -1 = down
float obstacleTopBound[TOTAL_OBSTACLES];
float obstacleBottomBound[TOTAL_OBSTACLES];
int obstacleType[TOTAL_OBSTACLES];


bool fireHit[TOTAL_FIRES];
float fireX[TOTAL_FIRES];
float fireY[TOTAL_FIRES];

bool  fireOn[TOTAL_FIRES];
float fireTimer[TOTAL_FIRES];

const float FIRE_ON_TIME  = 60.0f;   // frames (~1 sec)
const float FIRE_OFF_TIME = 60.0f;

// Hazards keep this far (x distance) from dorayaki and hearts
const float HAZARD_CLEARANCE = 3.0f;
const float OBSTACLE_SLIDE_RANGE = 2.5f;

// SignBoard (per obstacle) rotation
float signAngle[TOTAL_OBSTACLES];
float signRotateSpeed[TOTAL_OBSTACLES];

float stoneX[TOTAL_STONES];
float stoneY[TOTAL_STONES];
bool  stoneHit[TOTAL_STONES];

// drawStone() এর original size 2..7 range
const float STONE_SCALE = 0.18f;
const float STONE_W = 1.0f;   // width in world units
const float STONE_H = 1.0f;   // height in world


// ---------- Sound ----------
// Mac: built-in system sounds via afplay. Windows: sounds/<name>.wav via PlaySound.
void playSound(const char* name)
{
#ifdef __APPLE__
    const char* file = "Pop";
    if (strcmp(name, "collect") == 0) file = "Glass";
    else if (strcmp(name, "hit") == 0) file = "Basso";
    else if (strcmp(name, "levelup") == 0) file = "Hero";
    else if (strcmp(name, "gameover") == 0) file = "Sosumi";
    else if (strcmp(name, "gadget") == 0) file = "Submarine";
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "afplay /System/Library/Sounds/%s.aiff >/dev/null 2>&1 &", file);
    system(cmd);
#elif defined(_WIN32)
    char path[64];
    snprintf(path, sizeof(path), "sounds\\%s.wav", name);
    PlaySoundA(path, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
#else
    (void)name;
#endif
}

// ---------- High score ----------
void loadHighScore()
{
    FILE* f = fopen(HIGHSCORE_FILE, "r");
    if (!f) return;
    if (fscanf(f, "%d", &highScore) != 1) highScore = 0;
    fclose(f);
}

void saveHighScore()
{
    if (score <= highScore) return;
    highScore = score;
    FILE* f = fopen(HIGHSCORE_FILE, "w");
    if (!f) return;
    fprintf(f, "%d\n", highScore);
    fclose(f);
}

// ---------- Particles ----------
void clearParticles()
{
    for (int i = 0; i < TOTAL_PARTICLES; i++) particles[i].life = 0.0f;
}

// screenX/screenY are screen coordinates, stored in world space so they scroll with the level
void spawnParticles(float screenX, float screenY, int count, float r, float g, float b, float speed)
{
    for (int i = 0; i < TOTAL_PARTICLES && count > 0; i++) {
        if (particles[i].life > 0.0f) continue;
        float angle = (rand() / (float)RAND_MAX) * 2.0f * 3.1416f;
        float s = speed * (0.4f + 0.6f * rand() / (float)RAND_MAX);
        particles[i].x = screenX - worldX;
        particles[i].y = screenY;
        particles[i].vx = s * cos(angle);
        particles[i].vy = s * sin(angle);
        particles[i].life = 1.0f;
        particles[i].r = r; particles[i].g = g; particles[i].b = b;
        count--;
    }
}

void updateParticles()
{
    for (int i = 0; i < TOTAL_PARTICLES; i++) {
        if (particles[i].life <= 0.0f) continue;
        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        particles[i].vy -= 0.004f;
        particles[i].life -= 0.025f;
    }
}

void drawParticles()
{
    glPushAttrib(GL_LIGHTING_BIT | GL_POINT_BIT);
    glDisable(GL_LIGHTING);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < TOTAL_PARTICLES; i++) {
        if (particles[i].life <= 0.0f) continue;
        glColor4f(particles[i].r, particles[i].g, particles[i].b, particles[i].life);
        glVertex2f(particles[i].x + worldX, particles[i].y);
    }
    glEnd();
    glPopAttrib();
}

// ---------- Damage ----------
void takeHit()
{
    if (invincibleTimer > 0) return;
    lives--;
    invincibleTimer = INVINCIBLE_FRAMES;
    hitFlash = 0.45f;
    spawnParticles(charX + CHAR_HITBOX_OFFSET_X + 0.45f, charY + 3.0f, 25, 1.0f, 0.1f, 0.1f, 0.15f);
    playSound("hit");
}


void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void drawButton(float x, float y, float w, float h, const char* label)
{

    glColor3f(0.2f, 0.4f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y - h);
        glVertex2f(x, y - h);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y - h);
        glVertex2f(x, y - h);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(x + w / 4.0f, y - h / 1.5f, label);
}

//Doracake
void drawEllipse(float cx, float cy, float rx, float ry, float r, float g, float b)
{
    int segments = 100;
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for(int i = 0; i < segments; i++)
    {
        float theta = 2.0f * 3.1416f * i / segments;
        glVertex2f(cx + rx*cos(theta), cy + ry*sin(theta));
    }
    glEnd();

    glLineWidth(1);
    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < segments; i++)
    {
        float theta = 2.0f * 3.1416f * i / segments;
        glVertex2f(cx + rx*cos(theta), cy + ry*sin(theta));
    }
    glEnd();
}
void Doracake()
{
    float d1_cx = (-1.50f + 1.16f)/2;
    float d1_cy = (-0.72f + 0.89f)/2;
    float d1_rx = (1.53f - (-1.50f))/2;
    float d1_ry = (0.19f - (-0.72f))/2;
    drawEllipse(d1_cx, d1_cy, d1_rx, d1_ry, 0.55f, 0.3f, 0.2f);

    float c2_cx = (-1.68f + 1.32f)/2;
    float c2_cy = (-0.46f + 0.85f)/2;
    float c2_rx = (1.52f - (-1.68f))/2;
    float c2_ry = (0.43f - (-0.46f))/2;
    drawEllipse(c2_cx, c2_cy, c2_rx, c2_ry, 0.96f, 0.70f, 0.45f);

    float c_cx = (-1.88f + 1.59f)/2;
    float c_cy = (-0.14f + 0.76f)/2;
    float c_rx = (1.59f - (-1.88f))/2;
    float c_ry = (0.76f - (-0.14f))/2;
    drawEllipse(c_cx, c_cy, c_rx, c_ry, 0.90f, 0.64f, 0.38f);

    float d_cx = (-1.69f + 1.30f)/2;
    float d_cy = (0.28f + 0.6f)/2;
    float d_rx = (1.30f - (-1.69f))/2;
    float d_ry = (1.20f - 0.28f)/2;
    drawEllipse(d_cx, d_cy, d_rx, d_ry, 0.55f, 0.3f, 0.2f);
    glEnd();

    glFlush();
}

// Home screen layout (also used by mouseClick)
const float HOME_BTN_X = -2.6f;
const float HOME_BTN_W = 5.2f;
const float HOME_BTN_H = 1.1f;
const float HOME_BTN_TOP[3] = { 3.4f, 1.9f, 0.4f };

void Doraemon();
void drawTextCentered(float cx, float y, const char* text, void* font);

void fillCircle(float cx, float cy, float rx, float ry)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < 40; i++) {
        float a = 2.0f * 3.1416f * i / 40;
        glVertex2f(cx + rx * cos(a), cy + ry * sin(a));
    }
    glEnd();
}

void drawCloud(float x, float y, float s)
{
    glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
    fillCircle(x, y, 0.9f * s, 0.55f * s);
    fillCircle(x + 0.8f * s, y + 0.2f * s, 0.75f * s, 0.6f * s);
    fillCircle(x - 0.8f * s, y - 0.05f * s, 0.65f * s, 0.45f * s);
    fillCircle(x + 1.5f * s, y - 0.05f * s, 0.6f * s, 0.4f * s);
}

void drawRect(float x1, float y1, float x2, float y2)
{
    glBegin(GL_QUADS);
        glVertex2f(x1, y1); glVertex2f(x2, y1);
        glVertex2f(x2, y2); glVertex2f(x1, y2);
    glEnd();
}

void drawRectOutline(float x1, float y1, float x2, float y2, float width)
{
    glLineWidth(width);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x1, y1); glVertex2f(x2, y1);
        glVertex2f(x2, y2); glVertex2f(x1, y2);
    glEnd();
}

void drawHomeButton(int i, const char* label, const char* subtitle)
{
    float x1 = HOME_BTN_X, x2 = HOME_BTN_X + HOME_BTN_W;
    float y2 = HOME_BTN_TOP[i], y1 = y2 - HOME_BTN_H;

    glColor4f(0.0f, 0.0f, 0.0f, 0.25f);          // shadow
    drawRect(x1 + 0.12f, y1 - 0.15f, x2 + 0.12f, y2 - 0.15f);

    glColor3f(0.11f, 0.47f, 0.85f);              // Doraemon blue
    drawRect(x1, y1, x2, y2);
    glColor3f(0.25f, 0.6f, 0.95f);               // lighter top half
    drawRect(x1, y1 + HOME_BTN_H * 0.55f, x2, y2);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawRectOutline(x1, y1, x2, y2, 2.5f);

    drawTextCentered(0.0f, y2 - 0.5f, label, GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.85f, 0.93f, 1.0f);
    drawTextCentered(0.0f, y2 - 0.9f, subtitle, GLUT_BITMAP_HELVETICA_12);
}

void homeScreen()
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);       // levels leave lighting on, which washes out the menu colors
    glDisable(GL_DEPTH_TEST);

    // Sky gradient
    glBegin(GL_QUADS);
        glColor3f(0.25f, 0.6f, 0.95f);
        glVertex2f(SCREEN_LEFT, SCREEN_TOP); glVertex2f(SCREEN_RIGHT, SCREEN_TOP);
        glColor3f(0.7f, 0.9f, 1.0f);
        glVertex2f(SCREEN_RIGHT, -7.0f); glVertex2f(SCREEN_LEFT, -7.0f);
    glEnd();

    // Grass
    glColor3f(0.45f, 0.75f, 0.35f);
    drawRect(SCREEN_LEFT, SCREEN_BOTTOM, SCREEN_RIGHT, -7.0f);
    glColor3f(0.35f, 0.65f, 0.28f);
    drawRect(SCREEN_LEFT, -7.25f, SCREEN_RIGHT, -7.0f);

    drawCloud(-6.8f, 8.3f, 1.0f);
    drawCloud(6.3f, 8.7f, 0.8f);
    drawCloud(7.2f, 4.2f, 0.6f);
    drawCloud(-7.8f, 3.6f, 0.55f);

    // Title banner
    glColor4f(0.0f, 0.0f, 0.0f, 0.25f);
    drawRect(-5.85f, 5.15f, 6.15f, 7.35f);
    glColor3f(0.98f, 0.98f, 1.0f);               // white banner
    drawRect(-6.0f, 5.3f, 6.0f, 7.5f);
    glColor3f(0.11f, 0.47f, 0.85f);              // Doraemon blue border
    drawRectOutline(-6.0f, 5.3f, 6.0f, 7.5f, 4.0f);

    glColor3f(0.05f, 0.3f, 0.65f);
    drawTextCentered(0.0f, 6.5f, "DORAEMON'S TREAT HUNT", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.85f, 0.45f, 0.1f);
    drawTextCentered(0.0f, 5.75f, "Collect 5 dorayaki in every level!", GLUT_BITMAP_HELVETICA_18);

    // Level buttons
    drawHomeButton(0, "LEVEL 1", "Sunny Day");
    drawHomeButton(1, "LEVEL 2", "Sunset");
    drawHomeButton(2, "LEVEL 3", "Night");

    // High score pill
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "High Score: %d", highScore);
    glColor4f(1.0f, 1.0f, 1.0f, 0.75f);
    drawRect(-2.0f, -2.0f, 2.0f, -1.2f);
    glColor3f(0.9f, 0.6f, 0.1f);
    drawRectOutline(-2.0f, -2.0f, 2.0f, -1.2f, 2.0f);
    glColor3f(0.15f, 0.15f, 0.15f);
    drawTextCentered(0.0f, -1.75f, buffer, GLUT_BITMAP_HELVETICA_18);

    // Controls panel
    glColor4f(0.05f, 0.1f, 0.25f, 0.85f);
    drawRect(-4.6f, -6.6f, 4.6f, -2.7f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawRectOutline(-4.6f, -6.6f, 4.6f, -2.7f, 2.0f);
    glColor3f(1.0f, 0.85f, 0.2f);
    drawTextCentered(0.0f, -3.4f, "HOW TO PLAY", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTextCentered(0.0f, -4.3f, "Arrows : move & jump   (Up twice = double jump)", GLUT_BITMAP_HELVETICA_18);
    drawTextCentered(0.0f, -5.0f, "P : pause     Esc : quit     1 / 2 / 3 : start", GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.75f, 0.85f, 1.0f);
    drawTextCentered(0.0f, -5.8f, "Grab the Take-copter to fly.  Watch out for the mouse!", GLUT_BITMAP_HELVETICA_18);

    // Doraemon on the left
    glPushMatrix();
        glTranslatef(-10.6f, -8.1f, 0.0f);
        glScalef(0.7f, 0.7f, 1.0f);
        Doraemon();
    glPopMatrix();

    // Dorayaki on the right
    float cakePos[3][2] = { { 6.9f, -1.8f }, { 7.9f, -4.1f }, { 6.7f, -6.0f } };
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
            glTranslatef(cakePos[i][0], cakePos[i][1], 0.0f);
            glScalef(0.6f, 1.1f, 1.0f);
            Doracake();
        glPopMatrix();
    }

    glPopAttrib();

    glFlush();

    glutSwapBuffers();

}


// Width of a bitmap string in world units (window pixels -> gluOrtho2D units)
float textWidth(void* font, const char* text)
{
    int px = glutBitmapLength(font, (const unsigned char*)text);
    return px * (SCREEN_RIGHT - SCREEN_LEFT) / glutGet(GLUT_WINDOW_WIDTH);
}

void drawTextFont(float x, float y, const char* text, void* font)
{
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(font, text[i]);
}

void drawTextCentered(float cx, float y, const char* text, void* font)
{
    drawTextFont(cx - textWidth(font, text) / 2.0f, y, text, font);
}

void drawPanelButton(float x, float y, const char* label, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + buttonWidth, y);
        glVertex2f(x + buttonWidth, y + buttonHeight);
        glVertex2f(x, y + buttonHeight);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + buttonWidth, y);
        glVertex2f(x + buttonWidth, y + buttonHeight);
        glVertex2f(x, y + buttonHeight);
    glEnd();

    drawTextCentered(x + buttonWidth / 2.0f, y + buttonHeight / 2.0f - 0.15f, label, GLUT_BITMAP_HELVETICA_18);
}

void drawEndScreen() {
    if (!showEndScreen) return;

    const float panelL = -5.0f, panelR = 5.0f, panelB = -3.6f, panelT = 3.6f;

    // Panel background + border
    glColor4f(0.05f, 0.05f, 0.12f, 0.94f);
    glBegin(GL_QUADS);
        glVertex2f(panelL, panelB); glVertex2f(panelR, panelB);
        glVertex2f(panelR, panelT); glVertex2f(panelL, panelT);
    glEnd();
    glColor3f(1.0f, 0.85f, 0.2f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(panelL, panelB); glVertex2f(panelR, panelB);
        glVertex2f(panelR, panelT); glVertex2f(panelL, panelT);
    glEnd();

    // Title
    bool hasNextLevel = gameWon && currentLevel < totalLevels;
    if (gameWon) glColor3f(1.0f, 0.85f, 0.2f);
    else glColor3f(1.0f, 0.35f, 0.35f);
    if (hasNextLevel) drawTextCentered(0.0f, 2.7f, "LEVEL COMPLETE!", GLUT_BITMAP_TIMES_ROMAN_24);
    else if (gameWon) drawTextCentered(0.0f, 2.7f, "YOU WIN! ALL LEVELS DONE!", GLUT_BITMAP_TIMES_ROMAN_24);
    else drawTextCentered(0.0f, 2.7f, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

    // Score lines
    char buffer[64];
    glColor3f(1.0f, 1.0f, 1.0f);
    snprintf(buffer, sizeof(buffer), "Score: %d", score);
    drawTextCentered(0.0f, 1.9f, buffer, GLUT_BITMAP_HELVETICA_18);

    glColor3f(0.75f, 0.75f, 0.85f);
    if (gameWon) snprintf(buffer, sizeof(buffer), "Time bonus: +%d      High Score: %d", lastTimeBonus, highScore);
    else snprintf(buffer, sizeof(buffer), "High Score: %d", highScore);
    drawTextCentered(0.0f, 1.2f, buffer, GLUT_BITMAP_HELVETICA_18);

    // Buttons
    if (hasNextLevel) drawPanelButton(nextLevelX, nextLevelY, "Next Level", 0.2f, 0.6f, 0.3f);
    drawPanelButton(mainMenuX, mainMenuY, "Main Menu", 0.2f, 0.4f, 0.8f);
    drawPanelButton(restartX, restartY, "Restart", 0.2f, 0.4f, 0.8f);

    // Keyboard hint
    glColor3f(0.6f, 0.6f, 0.7f);
    if (hasNextLevel) drawTextCentered(0.0f, -3.2f, "Enter: Next    R: Restart    Esc: Menu", GLUT_BITMAP_HELVETICA_12);
    else drawTextCentered(0.0f, -3.2f, "Enter / R: Restart    Esc: Menu", GLUT_BITMAP_HELVETICA_12);
}

void drawSky1()
   {
        glDisable(GL_LIGHTING);

        glBegin(GL_QUADS);


        glColor3f(0.0f, 0.4f, 0.8f);
        glVertex2f(-7.0, 7.0);
        glVertex2f(7.0, 7.0);


        glColor3f(0.6f, 0.8f, 1.0f);
         glVertex2f(7.0f, 0.0f);
        glVertex2f(-7.0f, 0.0f);


        glColor3f(0.4f, 0.7f, 0.992f);
        glVertex2f(-7.0f, -7.0f);
        glVertex2f(7.0f, -7.0f);

        glEnd();

        glEnable(GL_LIGHTING);

   }
void drawSky2()
{   glDisable(GL_LIGHTING);
    // TOP QUAD
    glBegin(GL_QUADS);
    glColor3f(0.36f, 0.41f, 0.53f); // cool blue-gray
    glVertex2f(0.0f, 20.0f);
    glVertex2f(18.0f, 20.0f);
    glColor3f(0.47f, 0.45f, 0.55f); // upper-mid
    glVertex2f(18.0f, 18.0f);
    glVertex2f(0.0f, 18.0f);
   // glVertex2f(18.0f, 17.0f);
   // glVertex2f(0.0f, 17.0f);
    glEnd();

    // MIDDLE QUAD
    glBegin(GL_QUADS);
    glColor3f(0.47f, 0.45f, 0.55f); // upper-mid
   // glVertex2f(0.0f, 17.0f);
   // glVertex2f(18.0f, 17.0f);
    glVertex2f(0.0f, 18.0f);
    glVertex2f(18.0f, 18.0f);
    glColor3f(0.67f, 0.55f, 0.50f); // lower-mid
    glVertex2f(18.0f, 13.5f);
    glVertex2f(0.0f, 13.5f);
    glEnd();

    // BOTTOM QUAD
    glBegin(GL_QUADS);
    glColor3f(0.67f, 0.55f, 0.50f); // lower-mid
    glVertex2f(0.0f, 13.5f);
    glVertex2f(18.0f, 13.5f);
    glColor3f(0.93f, 0.58f, 0.38f); // wbody sunset
    glVertex2f(18.0f, 10.0f);
    glVertex2f(0.0f, 10.0f);
    glEnd();
    glFlush();
    glEnable(GL_LIGHTING);
}
void drawSky3()
{
    // TOP QUAD
    glBegin(GL_QUADS);
    glColor3f(0.02f, 0.02f, 0.10f); // dark blue
    glVertex2f(0.0f, 20.0f);
    glVertex2f(18.0f, 20.0f);
    glColor3f(0.05f, 0.05f, 0.2f); // upper-mid
    glVertex2f(18.0f, 18.0f);
    glVertex2f(0.0f, 18.0f);
   // glVertex2f(18.0f, 17.0f);
   // glVertex2f(0.0f, 17.0f);
    glEnd();

    // MIDDLE QUAD
    glBegin(GL_QUADS);
    glColor3f(0.05f, 0.05f, 0.2f); // dark navy
   // glVertex2f(0.0f, 17.0f);
   // glVertex2f(18.0f, 17.0f);
    glVertex2f(0.0f, 18.0f);
    glVertex2f(18.0f, 18.0f);
    glColor3f(0.1f, 0.08f, 0.3f); // lower-mid
    glVertex2f(18.0f, 13.5f);
    glVertex2f(0.0f, 13.5f);
    glEnd();

    // BOTTOM QUAD
    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.08f, 0.3f); // bluish purple
    glVertex2f(0.0f, 13.5f);
    glVertex2f(18.0f, 13.5f);
    glColor3f(0.2f, 0.12f, 0.4f); // faint purple
    glVertex2f(18.0f, 10.0f);
    glVertex2f(0.0f, 10.0f);
    glEnd();
    glFlush();
}


void Doraemon()
{
    //NOSE
    glLineWidth(3);
    glBegin(GL_POLYGON);

    for(int i = 0; i < 300; i++)
    {
        glColor3f(1.0, 0.0, 0.0);
        float pi = 3.1416f;
        float A = (i * 2 * pi) / 300.0f;
        float r = 0.33f;

        float x = 7.53 + r * cos(A);
        float y = 8.39 + r * sin(A);

        glVertex2f(x, y);
    }
    glEnd();

    glLineWidth(3);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);

    for(int i = 0; i < 300; i++)
    {
        float pi = 3.1416f;
        float A = (i * 2 * pi) / 300.0f;
        float r = 0.33f;

        float x = 7.53 + r * cos(A);
        float y = 8.39 + r * sin(A);

        glVertex2f(x, y);
    }


    glEnd();
    //HEAD
    glLineWidth(3);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 300; i++)
    {
        glColor3f(1.0, 1.0, 1.0); // white fill
        float pi = 3.1416;
        float A = (i * pi) / 300; // only half circle
        float r = 2.1;
        float x = 5.46 + r * cos(A);
        float y = 7.42 + r * sin(A);
        glVertex2f(x, y);
    }
    glEnd();



    // Black border only on the curved edge
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(int i = 20; i <= 90; i++)   // 300 steps → 20–90 mapped
    {
        float pi = 3.1416f;
        float A = (i * pi) / 300.0f;

        float r = 2.1f;
        float x = 5.46f + r * cos(A);
        float y = 7.42f + r * sin(A);

        glVertex2f(x, y);
    }
    glEnd();

    glLineWidth(3);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 300; i++)
    {
        glColor3f(0.6,0.8,0.9);
        // white fill
        float pi = 3.1416;
        float A = (i *2* pi) / 300; // only half circle
        float r = 0.86;
        float x = 4.88 + r * cos(A);
        float y = 6.3 + r * sin(A);
        glVertex2f(x, y);
    }
    glEnd();

    // Black border only on the curved edge

    glEnd();



    glBegin(GL_POLYGON);

    float angleOfRotationNew = 60.0f;
    float rotationAngleNew = angleOfRotationNew * (3.1416f / 180.0f);

    float txNew = 0.2f;
    float tyNew = -0.03f;

    float cxNew = 5.24f;
    float cyNew = 7.45f;

    float rNew = 2.1f;

    for (int i = 0; i < 300; i++) {

        glColor3f(0.6,0.8,0.9);

        float pi = 3.1416f;
        float theta = (i * pi) / 300.0f;

        float x = rNew * cos(theta);
        float y = rNew * sin(theta);

        // Rotate
        float x_rot = x * cos(rotationAngleNew) - y * sin(rotationAngleNew);
        float y_rot = x * sin(rotationAngleNew) + y * cos(rotationAngleNew);

        // Translate + center
        float x_final = x_rot + cxNew + txNew;
        float y_final = y_rot + cyNew + tyNew;

        glVertex2f(x_final, y_final);
    }

    glEnd();

    // Black border
    glLineWidth(3);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);

    for (int i = 0; i < 300; i++) {

        float pi = 3.1416f;
        float theta = (i * pi) / 300.0f;

        float x = rNew * cos(theta);
        float y = rNew * sin(theta);

        float x_rot = x * cos(rotationAngleNew) - y * sin(rotationAngleNew);
        float y_rot = x * sin(rotationAngleNew) + y * cos(rotationAngleNew);

        float x_final = x_rot + cxNew + txNew;
        float y_final = y_rot + cyNew + tyNew;

        glVertex2f(x_final, y_final);
    }

    glEnd();

    //HAND(ROUND)
    glLineWidth(3);
    glBegin(GL_POLYGON);
    for(int i=0;i<300;i++)
    {
        glColor3f(1.0,1.0,1.0);
        float pi=3.1416;
        float A=(i*2*pi)/300;
        float r=0.43;
        float x = 3.2+r * cos(A);
        float y =5.71+ r * sin(A);
        glVertex2f(x,y );
    }
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<500;i++)
    {
        glColor3f(0.0,0.0,0.0);
        float pi=3.1416;
        float A=(i*2*pi)/500;
        float r=0.43;
        float x = 3.2+r * cos(A);
        float y =5.71+ r * sin(A);
        glVertex2f(x,y );
    }
    glEnd();

    //hand(blue)
    glLineWidth(4.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(5.12f, 5.3f);
    glVertex2f(4.01f, 5.78f);
    glVertex2f(3.78f, 5.82f);
    glVertex2f(3.61f, 5.81f);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex2f(3.43f, 5.39f);
    glVertex2f(3.55f, 5.28f);
    glVertex2f(3.65f, 5.19f);
    glVertex2f(3.8f,  5.08f);
    glVertex2f(3.93f, 4.99f);
    glVertex2f(4.09f, 4.89f);
    glVertex2f(4.26f, 4.78f);
    glVertex2f(4.43f, 4.67f);
    glEnd();

    glColor3f(0.6f, 0.8f, 0.9f);
    glBegin(GL_POLYGON);
    glVertex2f(5.12f,5.3f);
    glVertex2f(4.01f,5.78f);
    glVertex2f(3.78f,5.82f);
    glVertex2f(3.61f,5.81f);
    glVertex2f(3.43f,5.39f);
    glVertex2f(3.55f,5.28f);
    glVertex2f(3.65f,5.19f);
    glVertex2f(3.8f,5.08f);
    glVertex2f(3.93f,4.99f);
    glVertex2f(4.09f,4.89f);
    glVertex2f(4.26f,4.78f);
    glVertex2f(4.43f,4.67f);
    glEnd();


    //HEADWHITE

    glBegin(GL_POLYGON);

    float angleOfRotation = 73.5f;
    float rotationAngle = angleOfRotation * (3.1416 / 180.0f);

    float tx = -0.134f;
    float ty = -0.272f;

    float cx = 6.7f;
    float cy = 6.99f;

    for (int i = 0; i < 300; i++) {

        glColor3f(1.0f, 1.0f, 1.0f);

        float pi = 3.1416f;
        float A = (i * pi) / 300.0f;
        float r = 1.95f;

        float x = r * cos(A);
        float y = r * sin(A);

        float x_rot = x * cos(rotationAngle) - y * sin(rotationAngle);
        float y_rot = x * sin(rotationAngle) + y * cos(rotationAngle);

        float x_final = x_rot + cx + tx;
        float y_final = y_rot + cy + ty;

        glVertex2f(x_final, y_final);
    }

    glEnd();

    glLineWidth(3);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);

    for (int i = 0; i < 300; i++) {

        float pi = 3.1416f;
        float A = (i * pi) / 300.0f;
        float r = 1.95f;

        float x = r * cos(A);
        float y = r * sin(A);

        float x_rot = x * cos(rotationAngle) - y * sin(rotationAngle);
        float y_rot = x * sin(rotationAngle) + y * cos(rotationAngle);

        float x_final = x_rot + cx + tx;
        float y_final = y_rot + cy + ty;

        glVertex2f(x_final, y_final);
    }

    glEnd();



    //nicher thot er line
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(6.99f, 5.75f);
    glVertex2f(6.98f,5.83f);
    glVertex2f(6.98f,5.83f);
    glVertex2f(7.08f,5.95f);
    glVertex2f(7.08f,5.95f);
    glVertex2f(7.18,6.05f);
    glVertex2f(7.18,6.05f);
    glVertex2f(7.28f,6.16f);
    glVertex2f(7.28f,6.16f);
    glVertex2f(7.30f,6.18f);
    glVertex2f(7.30f,6.18f);
    glVertex2f(7.31f,6.23f);

    glEnd();
    //nicher thot er khali jyga
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(7.28f, 6.16f);
    glVertex2f(7.03f,6.11f);
    glVertex2f(6.93f,6.12f);
    glVertex2f(6.21f,5.67f);
    // glVertex2f(6.97f,5.75f);
    //  glVertex2f(6.90f,5.82f);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(6.21f,5.67f);
    glVertex2f(6.99f,5.75f);
    glVertex2f(6.92f,5.82f);
    glVertex2f(7.30f,6.18f);

    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(7.24f, 6.27f);
    glVertex2f(7.07f,6.28f);
    glVertex2f(6.93f,6.12f);
    glVertex2f(7.03f,6.11f);
    glVertex2f(7.28f,6.16f);

    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(6.21f, 5.67f);
    glVertex2f(6.8f, 6.2f);
    glVertex2f(6.16f,6.94f);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(6.8f, 6.2f);
    glVertex2f(6.21f, 5.67f);
    glVertex2f(7.19f,6.28f);
    glEnd();

    glLineWidth(3);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 300; i++)
    {
        glColor3f(1.0, 1.0, 1.0); // white fill
        float pi = 3.1416;
        float A = (i * pi) / 300; // only half circle
        float r = 0.09;
        float x = 7.2 + r * cos(A);
        float y = 6.2 + r * sin(A);
        glVertex2f(x, y);
    }
    glEnd();
    glLineWidth(3);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i < 300; i++)
    {
        float pi = 3.1416;
        float A = (i * pi) / 300; // only half circle
        float r = 0.09;
        float x = 7.2 + r * cos(A);
        float y = 6.2 + r * sin(A);
        glVertex2f(x, y);
    }
    glEnd();

    //nicher thot er line
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(6.99f, 5.75f);
    glVertex2f(6.98f,5.83f);
    glVertex2f(6.98f,5.83f);
    glVertex2f(7.08f,5.95f);
    glVertex2f(7.08f,5.95f);
    glVertex2f(7.18,6.05f);
    glVertex2f(7.18,6.05f);
    glVertex2f(7.28f,6.16f);
    glVertex2f(7.28f,6.16f);
    glVertex2f(7.30f,6.18f);
    glVertex2f(7.30f,6.18f);
    glVertex2f(7.31f,6.23f);

    glEnd();
    //nicher thot er khali jyga
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(7.28f, 6.16f);
    glVertex2f(7.03f,6.11f);
    glVertex2f(6.93f,6.12f);
    glVertex2f(6.21f,5.67f);

    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(6.21f,5.67f);
    glVertex2f(6.99f,5.75f);
    glVertex2f(6.92f,5.82f);
    glVertex2f(7.30f,6.18f);

    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(7.24f, 6.27f);
    glVertex2f(7.07f,6.28f);
    glVertex2f(6.93f,6.12f);
    glVertex2f(7.03f,6.11f);
    glVertex2f(7.28f,6.16f);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(6.21f, 5.67f);
    glVertex2f(6.8f, 6.2f);
    glVertex2f(6.16f,6.94f);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(6.8f, 6.2f);
    glVertex2f(6.21f, 5.67f);
    glVertex2f(7.19f,6.28f);
    glEnd();


    glLineWidth(3);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 300; i++)
    {
        glColor3f(1.0, 1.0, 1.0); // white fill
        float pi = 3.1416;
        float A = (i * pi) / 300; // only half circle
        float r = 0.09;
        float x = 7.2 + r * cos(A);
        float y = 6.2 + r * sin(A);
        glVertex2f(x, y);
    }
    glEnd();
    glLineWidth(3);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i < 300; i++)
    {
        float pi = 3.1416;
        float A = (i * pi) / 300; // only half circle
        float r = 0.09;
        float x = 7.2 + r * cos(A);
        float y = 6.2 + r * sin(A);
        glVertex2f(x, y);
    }
    glEnd();


    //SMILE RED HALF CIRCLE

    glBegin(GL_POLYGON);

    float angleOfRotationH1 = 160.0;
    float rotationAngleH1 = angleOfRotationH1 * (3.1416f / 180.0f);

    float txH1 = -0.225f;
    float tyH1 = -0.10f;


    float cxH1 = 6.78f;
    float cyH1 = 7.39f;

    float rH1 = 0.76f;

    for (int i = 0; i < 300; i++) {

        glColor3f(1.0f, 0.0f, 0.0f);

        float pi = 3.1416f;
        float AH1 = (i * pi) / 300.0f;

        float xH1 = rH1 * cos(AH1);
        float yH1 = rH1 * sin(AH1);

        // Rotate
        float xH1_rot = xH1 * cos(rotationAngleH1) - yH1 * sin(rotationAngleH1);
        float yH1_rot = xH1 * sin(rotationAngleH1) + yH1 * cos(rotationAngleH1);

        // Translate + center
        float xH1_final = xH1_rot + cxH1 + txH1;
        float yH1_final = yH1_rot + cyH1 + tyH1;

        glVertex2f(xH1_final, yH1_final);
    }

    glEnd();
    //HALF RED BORDER
    glLineWidth(3);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);

    for (int i = 0; i < 300; i++) {

        float pi = 3.1416f;
        float AH1 = (i * pi) / 300.0f;

        float xH1 = rH1 * cos(AH1);
        float yH1 = rH1 * sin(AH1);

        float xH1_rot = xH1 * cos(rotationAngleH1) - yH1 * sin(rotationAngleH1);
        float yH1_rot = xH1 * sin(rotationAngleH1) + yH1 * cos(rotationAngleH1);

        float xH1_final = xH1_rot + cxH1 + txH1;
        float yH1_final = yH1_rot + cyH1 + tyH1;

        glVertex2f(xH1_final, yH1_final);
    }
    glEnd();
    //MUKHER lal hasir plane edge border
    float x_start = rH1 * cos(0);
    float y_start = rH1 * sin(0);
    float x_end = rH1 * cos(3.1416f);
    float y_end = rH1 * sin(3.1416f);

    // Rotate
    float x_start_rot = x_start * cos(rotationAngleH1) - y_start * sin(rotationAngleH1);
    float y_start_rot = x_start * sin(rotationAngleH1) + y_start * cos(rotationAngleH1);

    float x_end_rot = x_end * cos(rotationAngleH1) - y_end * sin(rotationAngleH1);
    float y_end_rot = x_end * sin(rotationAngleH1) + y_end * cos(rotationAngleH1);

    x_start_rot += cxH1 + txH1;
    y_start_rot += cyH1 + tyH1;

    x_end_rot += cxH1 + txH1;
    y_end_rot += cyH1 + tyH1;

    glBegin(GL_LINES);
    glVertex2f(x_start_rot, y_start_rot);
    glVertex2f(x_end_rot, y_end_rot);
    glEnd();

    float txT = 0.02f;
    float tyT = -0.15f;

    float angle = 5.0f;
    glPushMatrix();
    glTranslatef(txT, tyT, 0.0f);

    glTranslatef(cx, cy, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-cx, -cy, 0.0f);

    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(7.33f, 7.3f);
    glVertex2f(6.73f, 7.42f);
    glVertex2f(7.13f, 6.89f);
    glEnd();

    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(7.33f, 7.3f);
    glVertex2f(6.73f, 7.42f);
    glVertex2f(7.13f, 6.89f);
    glEnd();

    glPopMatrix();

    //UPPER THOT
    glLineWidth(3);
    glBegin(GL_POLYGON);

    float rotAngle2_deg = -75.0f;
    float rotAngle2_rad = rotAngle2_deg * (3.1416f / 180.0f);


    float transX2 = -0.05f;
    float transY2 = -.19f;


    float centerX2 = 7.61f;
    float centerY2 = 7.74f;

    for (int i = 0; i < 300; i++)
    {
        glColor3f(1.0, 1.0, 1.0);

        float pi = 3.1416f;
        float A = (i * pi) / 300.0f;
        float r = 0.33f;


        float ox = r * cos(A);
        float oy = r * sin(A);


        float rx = ox * cos(rotAngle2_rad) - oy * sin(rotAngle2_rad);
        float ry = ox * sin(rotAngle2_rad) + oy * cos(rotAngle2_rad);


        float finalX = rx + centerX2 + transX2;
        float finalY = ry + centerY2 + transY2;

        glVertex2f(finalX, finalY);
    }

    glEnd();
    //THOTER BORDER
    glLineWidth(3);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);

    for (int i = 0; i < 300; i++)
    {
        float pi = 3.1416f;
        float A = (i * pi) / 300.0f;
        float r = 0.33f;

        float ox = r * cos(A);
        float oy = r * sin(A);

        float rx = ox * cos(rotAngle2_rad) - oy * sin(rotAngle2_rad);
        float ry = ox * sin(rotAngle2_rad) + oy * cos(rotAngle2_rad);

        float finalX = rx + centerX2 + transX2;
        float finalY = ry + centerY2 + transY2;

        glVertex2f(finalX, finalY);
    }
    glEnd();
    glBegin(GL_POLYGON);//majhe khali chilo seta fill koreci
    glColor3f(1.0f, 1.0f, 1.0f);

    glVertex2f(6.66f, 7.55f);
    glVertex2f(6.73f, 7.27f);
    glVertex2f(7.07f, 7.25f);
    glVertex2f(7.3f, 7.22f);
    glVertex2f(7.7f, 7.26f);
    glVertex2f(7.75f, 7.72f);

    glEnd();

    //EYE(
    float I_x = 6.22, I_y = 9.29;
    float J_x = 7.08, J_y = 8.41;   // Point J (major axis reference)
    float K_x = 6.6, K_y = 8.55;  // Point K (minor axis reference)

    // major (vertical) & minor (horizontal)
    float a = sqrt(pow(J_x - I_x, 2) + pow(J_y - I_y, 2)); // semi-major
    float b = sqrt(pow(K_x - I_x, 2) + pow(K_y - I_y, 2)); // semi-minor

    float scale = 0.5;
    a *= scale;
    b *= scale;

    // ellipse center at I
    float centerX = I_x;
    float centerY = I_y;

    // TRANSLATION
    float tx1 = 0.325;
    float ty1 = -0.55;

    centerX += tx1;
    centerY += ty1;

    float angleDeg = 52.0;
    float phi = angleDeg * 3.1416 / 180.0;

    glColor3f(1.0, 1.0, 1.0);   // White fill
    glBegin(GL_POLYGON);

    for(int i = 0; i < 360; i++)
    {
        float theta = i * 3.1416 / 180.0;

        float x0 = b * cos(theta);
        float y0 = a * sin(theta);

        float xr = x0 * cos(phi) - y0 * sin(phi);
        float yr = x0 * sin(phi) + y0 * cos(phi);

        glVertex2f(centerX + xr, centerY + yr);
    }

    glEnd();

    // BLACK BORDER

    glColor3f(0, 0, 0);
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);

    for(int i = 0; i < 360; i++)
    {
        float theta = i * 3.1416 / 180.0;

        float x0 = b * cos(theta);
        float y0 = a * sin(theta);

        float xr = x0 * cos(phi) - y0 * sin(phi);
        float yr = x0 * sin(phi) + y0 * cos(phi);

        glVertex2f(centerX + xr, centerY + yr);
    }
    glEnd();

    //black part
    float O1_x = 6.83f, O1_y = 8.81f;  // ellipse center
    float P1_x = 7.09f, P1_y = 8.48f;  // point on major axis
    float Q1_x = 6.96f, Q1_y = 8.50f;  // point on minor axis

    float a2 = sqrt(pow(P1_x - O1_x, 2) + pow(P1_y - O1_y, 2));  // semi-major
    float b2 = sqrt(pow(Q1_x - O1_x, 2) + pow(Q1_y - O1_y, 2));  // semi-minor

    float scale2 = 0.5f;
    a2 *= scale2;
    b2 *= scale2;


    // Translation
    float tx2 = -.7f;
    float ty2 = 0.81f;
    centerX2 += tx2;
    centerY2 += ty2;

    float angleDeg2 = 30.0f;
    float phi2 = angleDeg2 * 3.1416f / 180.0f;

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);

    for(int i = 0; i < 360; i++)
    {
        float theta = i * 3.1416f / 180.0f;

        float x0 = b2 * cos(theta);
        float y0 = a2 * sin(theta);

        float xr = x0 * cos(phi2) - y0 * sin(phi2);
        float yr = x0 * sin(phi2) + y0 * cos(phi2);

        glVertex2f(centerX2 + xr, centerY2 + yr);
    }

    glEnd();
    //CHOKHER white part

    float R3_x = 6.97f, R3_y = 8.69f;  // ellipse center reference
    float S3_x = 7.07f, S3_y = 8.54f;  // point on major axis
    float T3_x = 7.02f, T3_y = 8.57f;  // point on minor axis

    float a3 = sqrt(pow(S3_x - R3_x, 2) + pow(S3_y - R3_y, 2));  // semi-major
    float b3 = sqrt(pow(T3_x - R3_x, 2) + pow(T3_y - R3_y, 2));  // semi-minor

    float scale3 = 0.5f;
    a3 *= scale3;
    b3 *= scale3;

    float centerX3 = R3_x;
    float centerY3 = R3_y;

    float tx3 = -0.0099f;
    float ty3 = -0.179f;
    centerX3 += tx3;
    centerY3 += ty3;

    float angleDeg3 = 25.0f;
    float phi3 = angleDeg3 * 3.1416f / 180.0f;

    glColor3f(1.0f, 1.0f, 1.0f);  // white fill
    glBegin(GL_POLYGON);

    for(int i = 0; i < 360; i++)
    {
        float theta = i * 3.1416f / 180.0f;

        float x0 = b3 * cos(theta);
        float y0 = a3 * sin(theta);

        // rotate
        float xr = x0 * cos(phi3) - y0 * sin(phi3);
        float yr = x0 * sin(phi3) + y0 * cos(phi3);

        // translate + center
        glVertex2f(centerX3 + xr, centerY3 + yr);
    }

    glEnd();

    //muuchstac
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(5.56f, 8.26f);
    glVertex2f(6.39f, 7.98f);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(6.3f, 7.82f);
    glVertex2f(5.16f, 7.8579f);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(6.27f, 7.63f);
    glVertex2f(5.22f, 7.37f);
    glEnd();


    //mouth
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(7.665f, 7.229f);//u
    glVertex2f(7.396f, 7.20f);//v
    glVertex2f(7.396f, 7.20f);
    glVertex2f(7.147f, 7.21f);//w
    glVertex2f(7.147f, 7.21f);
    glVertex2f(7.13f, 7.23f);//z
    glVertex2f(7.13, 7.23f);
    glVertex2f(7.11f, 7.25f);//A1
    glVertex2f(7.11f, 7.25f);
    glVertex2f(6.9f, 7.27f);
    glVertex2f(6.9f, 7.27f);
    glVertex2f(6.77, 7.28f);
    glVertex2f(6.77, 7.28f);
    glVertex2f(6.65f, 7.29f);
    glEnd();

    // RED SMILE FILL
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);

    //  Curve A (first)
    float x0_A = 6.20,    y0_A = 6.82;
    float x1_A = 6.67,    y1_A = 6.41;
    float x2_A = 7.18,    y2_A = 6.33;
    float txA  = -0.1f,   tyA  = -0.18f;

    for (float t = 0; t <= 1.0f; t += 0.01f) {
        float x = (1 - t)*(1 - t)*x0_A + 2*(1 - t)*t*x1_A + t*t*x2_A;
        float y = (1 - t)*(1 - t)*y0_A + 2*(1 - t)*t*y1_A + t*t*y2_A;
        glVertex2f(x + txA, y + tyA);
    }

    //Curve B (second)
    float x0_B = 7.12978f, y0_B = 6.89011f;
    float x1_B = 7.1489f,  y1_B = 6.60329f;
    float x2_B = 7.07242f, y2_B = 6.28461f;
    float txB  = 0.05f,    tyB  = -0.09f;

    for (float t = 0; t <= 1.0f; t += 0.01f) {
        float x = (1 - t)*(1 - t)*x0_B + 2*(1 - t)*t*x1_B + t*t*x2_B;
        float y = (1 - t)*(1 - t)*y0_B + 2*(1 - t)*t*y1_B + t*t*y2_B;
        glVertex2f(x + txB, y + tyB);
    }

    glEnd();


    // BLACK BORDER
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);

    // Curve A border
    for (float t = 0; t <= 1.0f; t += 0.01f) {
        float x = (1 - t)*(1 - t)*x0_A + 2*(1 - t)*t*x1_A + t*t*x2_A;
        float y = (1 - t)*(1 - t)*y0_A + 2*(1 - t)*t*y1_A + t*t*y2_A;
        glVertex2f(x + txA, y + tyA);
    }

    // Curve B border
    for (float t = 0; t <= 1.0f; t += 0.01f) {
        float x = (1 - t)*(1 - t)*x0_B + 2*(1 - t)*t*x1_B + t*t*x2_B;
        float y = (1 - t)*(1 - t)*y0_B + 2*(1 - t)*t*y1_B + t*t*y2_B;
        glVertex2f(x + txB, y + tyB);
    }

    glEnd();
    //pet er nil color fill

    // right leg color

    glColor3f(0.6f, 0.8f, 0.9f);
    glBegin(GL_POLYGON);


    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x =
            (1 - t)*(1 - t)*5.56f +
            2*(1 - t)*t*5.45f +
            t*t*5.60f;

        float y =
            (1 - t)*(1 - t)*3.42f +
            2*(1 - t)*t*3.02f +
            t*t*2.59f;

        glVertex2f(x, y);
    }

    for(float t = 1.0f; t >= 0.0f; t -= 0.02f)
    {
        float x =
            (1 - t)*(1 - t)*7.14f +
            2*(1 - t)*t*7.29f +
            t*t*6.75f;

        float y =
            (1 - t)*(1 - t)*5.09f +
            2*(1 - t)*t*3.93f +
            t*t*2.59f;

        glVertex2f(x, y);
    }

    glEnd();

    //pet er kichu jayga
    glColor3f(0.6,0.8,0.9);
    glBegin(GL_POLYGON);
    glVertex2f(4.43,4.77);
    glVertex2f(5.93,4.77);
    glVertex2f(5.93,3.27);
    glVertex2f(4.43,3.27);
    glEnd();

    // right leg color

    glColor3f(0.6f, 0.8f, 0.9f);
    glBegin(GL_POLYGON);


    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*5.56f +
        2*(1 - t)*t*5.45f +
        t*t*5.60f;

        float y =
        (1 - t)*(1 - t)*3.42f +
        2*(1 - t)*t*3.02f +
        t*t*2.59f;

        glVertex2f(x, y);
    }

    for(float t = 1.0f; t >= 0.0f; t -= 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*7.14f +
        2*(1 - t)*t*7.29f +
        t*t*6.75f;

        float y =
        (1 - t)*(1 - t)*5.09f +
        2*(1 - t)*t*3.93f +
        t*t*2.59f;

        glVertex2f(x, y);
    }

    glEnd();
    //pet er boro sada color fill
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POLYGON);

        // Curve 1
        for(float t = 0.0f; t <= 1.0f; t += 0.02f)
        {
            float x =
            (1 - t)*(1 - t)*5.73f +
            2*(1 - t)*t*5.37f +
            t*t*5.50f;

            float y =
            (1 - t)*(1 - t)*5.49f +
            2*(1 - t)*t*4.93f +
            t*t*4.89f;

            glVertex2f(x, y);
        }

        // Bottom straight line
        glVertex2f(5.50f, 4.16f);
        glVertex2f(6.75f, 4.93f);

        //Curve 2 (reverse direction)
        for(float t = 1.0f; t >= 0.0f; t -= 0.02f)
        {
            float x =
            (1 - t)*(1 - t)*7.14f +
            2*(1 - t)*t*7.29f +
            t*t*6.75f;

            float y =
            (1 - t)*(1 - t)*5.09f +
            2*(1 - t)*t*4.93f +
            t*t*4.89f;

            glVertex2f(x, y);
        }

        //Top straight line
        glVertex2f(7.14f, 5.09f);
        glVertex2f(5.73f, 5.49f);

        glEnd();

        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(7.1f, 4.6385f);
        glVertex2f(5.78f,4.81f)  ;
        glVertex2f(5.12f, 5.3f);
        glVertex2f(6.69f,5.48f)  ;
        glEnd();

        //bellly...
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(7.1f, 4.6385f);
        glVertex2f(5.78f,4.81f)  ;
        glVertex2f(5.12f, 5.3f);
        glVertex2f(6.69f,5.48f)  ;
        glEnd();

        glBegin(GL_POLYGON);

        float angleOfRotationBelly = 118.5f;
        float rotationAngleBelly = angleOfRotationBelly * (3.1416f / 180.0f);

        float txBelly = 0.37f;
        float tyBelly = -0.03f;

        float cxBelly = 6.16f;
        float cyBelly = 4.29f;

        float rBelly = 1.05f;

        for (int i = 0; i < 300; i++) {

            glColor3f(1.0,1.0,1.0);

            float pi = 3.1416f;
            float theta = (i * pi) / 300.0f;

            float xBelly = rBelly * cos(theta);
            float yBelly = rBelly * sin(theta);

            // Rotate
            float x_rotBelly = xBelly * cos(rotationAngleBelly) - yBelly * sin(rotationAngleBelly);
            float y_rotBelly = xBelly * sin(rotationAngleBelly) + yBelly * cos(rotationAngleBelly);

            // Translate + center
            float x_finalBelly = x_rotBelly + cxBelly + txBelly;
            float y_finalBelly = y_rotBelly + cyBelly + tyBelly;

            glVertex2f(x_finalBelly, y_finalBelly);
        }

        glEnd();

        // Black border
        glLineWidth(3);
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_STRIP);

        for (int i = 110; i <= 300; i++) {

            float pi = 3.1416f;
            float theta = (i * pi) / 300.0f;

            float xBelly = rBelly * cos(theta);
            float yBelly = rBelly * sin(theta);

            float x_rotBelly = xBelly * cos(rotationAngleBelly) - yBelly * sin(rotationAngleBelly);
            float y_rotBelly = xBelly * sin(rotationAngleBelly) + yBelly * cos(rotationAngleBelly);

            float x_finalBelly = x_rotBelly + cxBelly+ txBelly;
            float y_finalBelly = y_rotBelly + cyBelly + tyBelly;

            glVertex2f(x_finalBelly, y_finalBelly);
        }

        glEnd();


        glLineWidth(3);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 300; i++)
        {
            glColor3f(1.0, 1.0, 1.0); // white fill
            float pi = 3.1416;
            float A = (i * 2*pi) / 300; // only half circle
            float r = 0.75;
            float x = 6.38 + r * cos(A);
            float y = 4.04 + r * sin(A);
            glVertex2f(x, y);
        }
        glEnd();
        glLineWidth(3);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 300; i++)
        {
            glColor3f(1.0, 1.0, 1.0); // white fill
            float pi = 3.1416;
            float A = (i * 2*pi) / 300; // only half circle
            float r = 0.45;
            float x = 6.67 + r * cos(A);
            float y = 3.73 + r * sin(A);
            glVertex2f(x, y);
        }
        glEnd();



        glLineWidth(3.0f);
        glColor3f(0.0f, 0.0f, 0.0f);

        glBegin(GL_LINE_STRIP);

        for(float t = 0.0f; t <= 1.0f; t += 0.02f)
        {
            float x =
            (1 - t)*(1 - t)*5.73f +
            2*(1 - t)*t*5.37f +
            t*t*5.50f;

            float y =
            (1 - t)*(1 - t)*5.49f +
            2*(1 - t)*t*4.37f +
            t*t*4.16f;

            glVertex2f(x, y);
        }

        glEnd();



        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(7.14f,5.06f);
        glVertex2f(6.78f,5.08f)  ;
        glVertex2f(6.8f, 3.97f);
        glVertex2f(6.97f,3.97f)  ;
        glVertex2f(7.16f,4.06f);
        glEnd();

        //pocket

        glBegin(GL_POLYGON);

        float pi = 3.1416f;
        float rotationAnglePocket = 180.0f * pi / 180.0f;

        float cxPocket = 6.23f;
        float cyPocket = 4.24f;
        float rPocket  = 1.2;

        float txPocket = 0.8f;
        float tyPocket = 0.5f;

        glColor3f(1.0f, 1.0f, 1.0f);


        for (int i = 0; i <= 75; i++) {
            float theta = (i * pi) / 150.0f;

            float x = rPocket * cos(theta);
            float y = rPocket * sin(theta);

            float xr = x * cos(rotationAnglePocket) - y * sin(rotationAnglePocket);
            float yr = x * sin(rotationAnglePocket) + y * cos(rotationAnglePocket);

            glVertex2f(xr + cxPocket + txPocket,
                       yr + cyPocket + tyPocket);
        }

        // center point
        glVertex2f(cxPocket + txPocket, cyPocket + tyPocket);

        glEnd();

        glLineWidth(3);
        glColor3f(0,0,0);
        glBegin(GL_LINE_STRIP);

        for (int i = 0; i <= 75; i++) {
            float theta = (i * pi) / 150.0f;

            float x = rPocket * cos(theta);
            float y = rPocket * sin(theta);

            float xr = x * cos(rotationAnglePocket) - y * sin(rotationAnglePocket);
            float yr = x * sin(rotationAnglePocket) + y * cos(rotationAnglePocket);

            glVertex2f(xr + cxPocket + txPocket,
                       yr + cyPocket + tyPocket);
        }
        glEnd();

        glBegin(GL_LINES);

        // edge 1
        glVertex2f(cxPocket + txPocket, cyPocket + tyPocket);
        glVertex2f(cxPocket + txPocket + rPocket * cos(rotationAnglePocket),
                   cyPocket + tyPocket + rPocket * sin(rotationAnglePocket));

        glEnd();
        glLineWidth(3);
        glColor3f(0.0f,0.0f,0.0f);


        glBegin(GL_LINE_STRIP);
        glVertex2f(7.0f, 4.74f);
        glVertex2f(7.15f, 4.74f);


        glEnd();

        //pet er nil ektu baki seta
        glBegin(GL_POLYGON);
        glColor3f(0.6f, 0.8f, 0.9f);
        glVertex2f(4.43, 4.67f);
        glVertex2f(5.1f,5.43f)  ;
        glVertex2f(5.715f, 5.47f);
        glVertex2f(5.44f,4.59f)  ;
        glEnd();

    //belt er line
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(6.99f, 5.75f);
    glVertex2f(4.73325f,5.50063f)  ;
    glVertex2f(5.12f, 5.3f);
    glVertex2f(6.69f,5.48f)  ;
    glEnd();
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f,0.0f,0.0f);
    glVertex2f(6.99f, 5.75f);
    glVertex2f(4.73325f,5.50063f)  ;
    glVertex2f(5.12f, 5.3f);
    glVertex2f(6.69f,5.48f)  ;
    glEnd();

    //BELL
    float centerXB = 7.03f;
    float centerYB = 5.41f;
    float r = 0.35f;
   // float pi=3.1416;

    glColor3f(1.0f, 0.84f, 0.0f);   // golden color (R=1, G=0.84, B=0)
    glBegin(GL_POLYGON);
    for(int i = 0; i < 300; i++) {
        float B = (i * 2 * pi) / 300.0f;
        float x = centerXB + r * cos(B);
        float y = centerYB + r * sin(B);
        glVertex2f(x, y);
    }
    glEnd();

    glLineWidth(3);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 300; i++) {
        float B = (i * 2 * pi) / 300.0f;
        float x = centerXB + r * cos(B);
        float y = centerYB + r * sin(B);
        glVertex2f(x, y);
    }
    glEnd();

    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(6.69f, 5.48f);
    glVertex2f(7.13f, 5.74f);
    glEnd();
    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(6.68f, 5.37f);    // x, y
    glVertex2f(7.22f, 5.69f);    // x, y
    glEnd();
    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(7.35f, 5.53f);    // x, y
    glVertex2f(7.18f, 5.45f);    // x, y
    glEnd();
    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(7.18f, 5.45f);    // x, y
    glVertex2f(7.37f, 5.4f);    // x, y
    glEnd();

    //right leg


    //RIGHT LEG
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*7.14f +
        2*(1 - t)*t*7.29f +
        t*t*6.75f;

        float y =
        (1 - t)*(1 - t)*5.09f +
        2*(1 - t)*t*3.93f +
        t*t*2.59f;

        glVertex2f(x, y);
    }

    glEnd();
    //juta
    //colour
    glPushMatrix();
    glRotatef(legAngle, 0, 0, -1);
    glTranslatef(0.05f, 0.0f, 0.0f); // move to hip
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);

    //Right straight line
    glVertex2f(6.93f, 2.64f);
    glVertex2f(6.89f, 1.99f);

    //Lower Bezier
    for(float t = 1.0f; t >= 0.0f; t -= 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*6.89f +
        2*(1 - t)*t*5.78f +
        t*t*5.23f;

        float y =
        (1 - t)*(1 - t)*1.99f +
        2*(1 - t)*t*1.75f +
        t*t*2.23f;

        glVertex2f(x, y);
    }

    // Left straight line
    glVertex2f(5.23f, 2.23f);
    glVertex2f(5.54f, 2.5f);

    glEnd();
    //right circle
    glLineWidth(3);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 300; i++)
    {
        glColor3f(1.0, 1.0, 1.0); // white fill
        float pi = 3.1416;
        float A = (i *2* pi) / 300;
        float r = 0.34;
        float x = 6.84+ r * cos(A);
        float y = 2.35 + r * sin(A);
        glVertex2f(x, y);
    }
    glEnd();

    // Black border only on the curved edge
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(int i = 230; i <= 400; i++)   // 300 steps → 20–90 mapped
    {
        float pi = 3.1416f;
        float A = (i*2 * pi) / 300.0f;

        float r = 0.34f;
        float x = 6.84f + r * cos(A);
        float y = 2.35f + r * sin(A);

        glVertex2f(x, y);
    }
    glEnd();

    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*6.93 +
        2*(1 - t)*t*6.29f +
        t*t*5.54f;

        float y =
        (1 - t)*(1 - t)*2.69f +
        2*(1 - t)*t*2.51f +
        t*t*2.61f;

        glVertex2f(x, y);
    }
    glEnd();
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*6.89f +
        2*(1 - t)*t*5.78f +
        t*t*5.23f;

        float y =
        (1 - t)*(1 - t)*1.99f +
        2*(1 - t)*t*1.75f +
        t*t*2.23f;

        glVertex2f(x, y);
    }
    glEnd();
    //left circle
    glLineWidth(3);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 300; i++)
    {
        glColor3f(1.0, 1.0, 1.0); // white fill
        float pi = 3.1416;
        float A = (i *2* pi) / 300;
        float r = 0.27;
        float x = 5.5+ r * cos(A);
        float y = 2.35 + r * sin(A);
        glVertex2f(x, y);
    }
    glEnd();

    // Black border only on the curved edge
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(int i =60; i <= 230; i++)   // 300 steps → 20–90 mapped
    {
        float pi = 3.1416f;
        float A = (i*2 * pi) / 300.0f;

        float r = 0.27f;
        float x = 5.5f + r * cos(A);
        float y = 2.35f + r * sin(A);

        glVertex2f(x, y);
    }
    glEnd();
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*5.56f +
        2*(1 - t)*t*5.45f +
        t*t*5.60f;

        float y =
        (1 - t)*(1 - t)*3.42 +
        2*(1 - t)*t*3.02f +
        t*t*2.59f;

        glVertex2f(x, y);
    }
    glEnd();
    glPopMatrix();

    //left leg
    //left leg


    glLineWidth(4.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);
    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x =
        (1 - t) * (1 - t) * 4.43f +
        2 * (1 - t) * t * 4.08f +
        t * t * 4.55f;

        float y =
        (1 - t) * (1 - t) * 4.77f +
        2 * (1 - t) * t * 3.13f +
        t * t * 2.09f;

        glVertex2f(x, y);
    }
    glEnd();


    //pa colour
    glColor3f(0.6f, 0.8f, 0.9f);   // WHITE fill
    glBegin(GL_POLYGON);

    // Curve 1
    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*4.43f +
        2*(1 - t)*t*4.08f +
        t*t*4.55f;

        float y =
        (1 - t)*(1 - t)*4.77f +
        2*(1 - t)*t*3.13f +
        t*t*2.09f;

        glVertex2f(x, y);
    }

    // Bottom straight line
    glVertex2f(4.55f, 2.09f);
    glVertex2f(6.00f, 2.36f);

    // Curve 2 (reverse direction)
    for(float t = 1.0f; t >= 0.0f; t -= 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*6.20f +
        2*(1 - t)*t*5.95f +
        t*t*6.00f;

        float y =
        (1 - t)*(1 - t)*3.03f +
        2*(1 - t)*t*2.51f +
        t*t*2.36f;

        glVertex2f(x, y);
    }

    // Top straight line
    glVertex2f(6.20f, 3.03f);
    glVertex2f(4.43f, 4.77f);

    glEnd();
    //juta
    //colour juta

    glPushMatrix();
    glTranslatef(-0.05f, 0.0f, 0.0f);
    glRotatef(legAngle, 0, 0, 1);

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);

    // Right straight line
    glVertex2f(6.0f, 2.25);
    glVertex2f(6.2f, 1.73f);

    //  Lower Bezier curve
    for(float t=1.0f; t>=0.0f; t-=0.02f)
    {
        float x = (1-t)*(1-t)*4.3f + 2*(1-t)*t*5.26f + t*t*6.2f;

        float y = (1-t)*(1-t)*2.0f + 2*(1-t)*t*1.52f + t*t*1.73f;

        glVertex2f(x,y);
    }

    //Left straight line
    glVertex2f(4.3f, 1.95f);
    glVertex2f(4.49f, 2.56f);

    glEnd();


    //left side circle
    glLineWidth(3);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 300; i++)
    {
        glColor3f(1.0, 1.0, 1.0); // white fill
        float pi = 3.1416;
        float A = (i *2* pi) / 300;
        float r = 0.34;
        float x = 4.43 + r * cos(A);
        float y = 2.3 + r * sin(A);
        glVertex2f(x, y);
    }
    glEnd();

    // Black border only on the curved edge
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(int i = 67; i <= 230; i++)   // 300 steps → 20–90 mapped
    {
        float pi = 3.1416f;
        float A = (i*2 * pi) / 300.0f;

        float r = 0.34f;
        float x = 4.43f + r * cos(A);
        float y = 2.3f + r * sin(A);

        glVertex2f(x, y);
    }
    glEnd();
    //curve
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*4.49f +
        2*(1 - t)*t*5.32f +
        t*t*6.0f;

        float y =
        (1 - t)*(1 - t)*2.65f +
        2*(1 - t)*t*2.2f +
        t*t*2.4f;

        glVertex2f(x, y);
    }

    glEnd();

    //right side circle
    glLineWidth(3);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 300; i++)
    {
        glColor3f(1.0, 1.0, 1.0);
        float pi = 3.1416;
        float A = (i *2* pi) / 300;
        float r = 0.34;
        float x = 6.15 + r * cos(A);
        float y = 2.09 + r * sin(A);
        glVertex2f(x, y);
    }
    glEnd();

    // Black border only on the curved edge
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(int i = 225; i <= 410; i++)
    {
        float pi = 3.1416f;
        float A = (i*2 * pi) / 300.0f;

        float r = 0.34f;
        float x = 6.15f + r * cos(A);
        float y = 2.09f + r * sin(A);

        glVertex2f(x, y);
    }
    glEnd();
    //curve
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*4.3f +
        2*(1 - t)*t*5.26f +
        t*t*6.2f;

        float y =
        (1 - t)*(1 - t)*2.0f +
        2*(1 - t)*t*1.52f +
        t*t*1.73f;

        glVertex2f(x, y);
    }

    glEnd();

    //jutar uporer curve
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);

    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x =
        (1 - t)*(1 - t)*6.2f +
        2*(1 - t)*t*5.95f +
        t*t*6.0f;

        float y =
        (1 - t)*(1 - t)*3.03f +
        2*(1 - t)*t*2.51f +
        t*t*2.36f;

        glVertex2f(x, y);
    }

    glEnd();

    //tail
    glLineWidth(4.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(4.25, 3.64f);
    glVertex2f(4.00f, 3.72f);
    glEnd();

    //dim
    glLineWidth(3);
    glBegin(GL_POLYGON);

    for(int i = 0; i < 300; i++)
    {
        glColor3f(1.0, 0.0, 0.0);
        float pi = 3.1416f;
        float A = (i * 2 * pi) / 300.0f;
        float r = 0.25f;

        float x = 3.80 + r * cos(A);
        float y = 3.75 + r * sin(A);

        glVertex2f(x, y);
    }

    glEnd();

    glLineWidth(3);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);

    for(int i = 0; i < 300; i++)
    {
        float pi = 3.1416f;
        float A = (i * 2 * pi) / 300.0f;
        float r = 0.25f;

        float x = 3.80 + r * cos(A);
        float y = 3.750 + r * sin(A);

        glVertex2f(x, y);
    }

    glEnd();
    glPopMatrix();
    glFlush();
}

   //flower pot
void drawCircle(float cx, float cy, float cr,float r,float g,float b) {
    glBegin(GL_POLYGON);
    glColor3f(r,g,b);
    for (int i = 0; i < 360; i++) {

        float A = i * 2 * 3.1415926f / 360;
        float x = cr * cos(A) + cx;
        float y = cr * sin(A) + cy;
        glVertex2f(x, y);
    }
    glEnd();
    glBegin(GL_LINE_STRIP);
    glColor3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 360; i++) {

        float A = i * 2 * 3.1415926f / 360;
        float x = cr * cos(A) + cx;
        float y = cr * sin(A) + cy;
        glVertex2f(x, y);
    }
    glEnd();
}


void drawFlower()
{ glColor3f(0.6,0.2,0);
    glBegin(GL_POLYGON);
    glVertex2f(11,1.11);
    glVertex2f(11,0.57);
    glVertex2f(11.01,0.5555);
    glVertex2f(11.03,0.54);
    glVertex2f(11.93,0.54);
    glVertex2f(11.95,0.5555);
    glVertex2f(11,0.57);
    glVertex2f(11.96,0.57);
    glVertex2f(11.96,1.11);
    glEnd();

    glColor3f(0.8,0.4,0.0);
    glBegin(GL_POLYGON);
    glVertex2f(11,1.11);
    glVertex2f(10.97,1.12);
    glVertex2f(10.97302,1.11894);
    glVertex2f(10.95455,1.15131);
    glVertex2f(10.96995,1.4031);
    glVertex2f(11,1.4);
    glVertex2f(11.97808,1.41);
    glVertex2f(12,1.4);
    glVertex2f(12.01195,1.35994);
    glVertex2f(12.01041,1.14412);
    glVertex2f(11.98886,1.11894);
    glVertex2f(11.95826,1.11117);
    glEnd();

    //pata

    float x0 = 10.76139f, y0 = 1.8923f;
    float x1 = 11.15541f, y1 = 1.82755f;
    float x2 = 11.45708f, y2 = 1.42828f;

    glColor3f(0.0f, 0.0f, 0.0f); // curve color
    glLineWidth(2.0f);

    glBegin(GL_LINE_STRIP);
    for(float t = 0.0f; t <= 1.0f; t += 0.01f)
    {
        float x = (1-t)*(1-t)*x0
        + 2*(1-t)*t*x1
        + t*t*x2;

        float y = (1-t)*(1-t)*y0
        + 2*(1-t)*t*y1
        + t*t*y2;

        glVertex2f(x, y);
    }
    glEnd();
    float x10 = 10.76139f, y10 = 1.8923f;
    float x11 = 10.87683f, y11 = 1.59375f;
    float x12 = 11.07692f,  y12 = 1.4067f;

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);

    glBegin(GL_LINE_STRIP);
    for(float t = 0.0f; t <= 1.0f; t += 0.01f)
    {
        float x = (1-t)*(1-t)*x10
                + 2*(1-t)*t*x11
                + t*t*x12;

        float y = (1-t)*(1-t)*y10
                + 2*(1-t)*t*y11
                + t*t*y12;

        glVertex2f(x, y);
    }
    glEnd();
//pata color
        glColor3f(0.0f, 0.45f, 0.0f);
        glBegin(GL_POLYGON);

        for(float t = 0.0f; t <= 1.0f; t += 0.02f)
        {
            float x = (1-t)*(1-t)*x0
                    + 2*(1-t)*t*x1
                    + t*t*x2;

            float y = (1-t)*(1-t)*y0
                    + 2*(1-t)*t*y1
                    + t*t*y2;

            glVertex2f(x, y);
        }

        for(float t = 1.0f; t >= 0.0f; t -= 0.02f)
        {
            float x = (1-t)*(1-t)*x10
                    + 2*(1-t)*t*x11
                    + t*t*x12;

            float y = (1-t)*(1-t)*y10
                    + 2*(1-t)*t*y11
                    + t*t*y12;

            glVertex2f(x, y);
        }

        glEnd();

    //pata right
    float x20 = 11.52173f, y20 = 1.41749f;
    float x21 = 11.8157f,  y21 = 1.82755f;
    float x22 = 12.24359f, y22 = 1.88151f;
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);

    glBegin(GL_LINE_STRIP);
    for(float t = 0.0f; t <= 1.0f; t += 0.01f)
    {
        float x = (1-t)*(1-t)*x20
                + 2*(1-t)*t*x21
                + t*t*x22;

        float y = (1-t)*(1-t)*y20
                + 2*(1-t)*t*y21
                + t*t*y22;

        glVertex2f(x, y);
    }
    glEnd();
    float x30 = 11.93576f, y30 = 1.41749f;
    float x31 = 12.10814f,  y31 = 1.55058f;
    float x32 = 12.24359f, y32 = 1.88151f;
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);

    glBegin(GL_LINE_STRIP);
    for(float t = 0.0f; t <= 1.0f; t += 0.01f)
    {
        float x = (1-t)*(1-t)*x30
                + 2*(1-t)*t*x31
                + t*t*x32;

        float y = (1-t)*(1-t)*y30
                + 2*(1-t)*t*y31
                + t*t*y32;

        glVertex2f(x, y);
    }
    glEnd();
    //pata color
    glColor3f(0.0f, 0.45f, 0.0f);
    glBegin(GL_POLYGON);

    for(float t = 0.0f; t <= 1.0f; t += 0.02f)
    {
        float x = (1-t)*(1-t)*x20
                + 2*(1-t)*t*x21
                + t*t*x22;

        float y = (1-t)*(1-t)*y20
                + 2*(1-t)*t*y21
                + t*t*y22;

        glVertex2f(x, y);
    }

    for(float t = 1.0f; t >= 0.0f; t -= 0.02f)
    {
        float x = (1-t)*(1-t)*x30
                + 2*(1-t)*t*x31
                + t*t*x32;

        float y = (1-t)*(1-t)*y30
                + 2*(1-t)*t*y31
                + t*t*y32;

        glVertex2f(x, y);
    }

    glEnd();

    glLineWidth(2.0);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(10.91223,1.7664);
    glVertex2f(11.35242,1.41749);

    glVertex2f(11.66333,1.41389);
    glVertex2f(12.1,1.8);
    glEnd();
    //LOMBA
    glLineWidth(2.0);
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.45f, 0.0f);
    glVertex2f(11.45708,1.42828);
    glVertex2f(11.46,1.81);

    glVertex2f(11.51,1.85);
    glVertex2f(11.52173,1.41749);
    glEnd();

    //full
    drawCircle(11.31,2.69,0.19771922640362516,0.8,0.4,1);
    drawCircle(11.15,2.44,0.17034343770221755,0.8,0.4,1);
    drawCircle(11.16,2.13,0.16801150074766016,0.8,0.4,1);
    drawCircle(11.39,1.96,0.1634418553389585,0.8,0.4,1);
    drawCircle(11.66,1.96,0.18765000067862558,0.8,0.4,1);
    drawCircle(11.86,2.17,0.17,0.8,0.4,1);
    drawCircle(11.86,2.49,0.18,0.8,0.4,1);
    drawCircle(11.61,2.72,0.19,0.8,0.4,1);

    drawCircle(11.49888,2.31652,0.28,1,0.84,0);
    glFlush();
}

void Heart()

{
    glColor3f(0,0,0);
    glLineWidth(4);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<100;i++)
    {
        float theta = 2.0f * 3.1416f * i / 100;
        glVertex2f(-0.18f + 0.2f*cos(theta), 0.2f + 0.2f*sin(theta));
    }
    glEnd();
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<100;i++)
    {
        float theta = 2.0f * 3.1416f * i / 100;
        glVertex2f(0.18f + 0.2f*cos(theta), 0.2f + 0.2f*sin(theta));
    }
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.38f, 0.15f);
    glVertex2f(0.0f, 0.3f);
    glVertex2f(0.38f, 0.15f);
    glVertex2f(0.0f, -0.4f);

    glEnd();

    glColor3f(1,0.3,0.4);

    glBegin(GL_POLYGON);

    for(int i=0;i<100;i++)

    {

        float theta = 2.0f * 3.1416f * i / 100;

        glVertex2f(-0.18f + 0.2f*cos(theta), 0.2f + 0.2f*sin(theta));

    }

    glEnd();

    glBegin(GL_POLYGON);

    for(int i=0;i<100;i++)

    {

        float theta = 2.0f * 3.1416f * i / 100;

        glVertex2f(0.18f + 0.2f*cos(theta), 0.2f + 0.2f*sin(theta));

    }

    glEnd();

    glBegin(GL_POLYGON);

    glVertex2f(-0.38f, 0.15f);

    glVertex2f(0.0f, 0.3f);

    glVertex2f(0.38f, 0.15f);

    glVertex2f(0.0f, -0.4f);

    glEnd();

    glFlush();

}
void electricPole()
{
    glPointSize(10.0f);

    //Borders
    glLineWidth(4);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_LOOP);
    float al = 0.03f;
    float bl = 0.03f;
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float x =  al * cos(angle);
        float y = 0.6 + bl * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    glBegin(GL_LINE_LOOP);
    float al1 = 0.03f;
    float bl1 = 0.03f;
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float x =  al1 * cos(angle);
        float y = -0.6 + bl1 * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-0.175f, 0.5f);
    glVertex2f(0.175f, 0.5f);
    glVertex2f(0.135f, 0.45f);
    glVertex2f(-0.135f, 0.45f);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-0.105f, 0.4f);
    glVertex2f(0.105f, 0.4f);
    glVertex2f(0.065f, 0.35f);
    glVertex2f(-0.065f, 0.35f);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.03f, 0.6f);
    glVertex2f(-0.03f, 0.6f);
    glVertex2f(-0.03f, -0.6f);
    glVertex2f(0.03f, -0.6f);
    glEnd();
    glFlush();


    glBegin(GL_QUADS);
    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex2f(0.03f, 0.6f);
    glVertex2f(-0.03f, 0.6f);
    glVertex2f(-0.03f, -0.6f);
    glVertex2f(0.03f, -0.6f);
    glEnd();
    glFlush();


    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_POLYGON);
    float a = 0.03f;
    float b = 0.03f;
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float x =  a * cos(angle);
        float y = 0.6 + b * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
    glFlush();

    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_POLYGON);
    float a1 = 0.03f;
    float b1 = 0.03f;
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float x =  a1 * cos(angle);
        float y = -0.6 + b1 * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
    glFlush();

    glLineWidth(0.25);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(1.0f, 0.47f);
    glVertex2f(0.135f, 0.47f);
    glEnd();
    glFlush();

    //WIRES
    glLineWidth(2);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINES);
    glVertex2f(-1.0f, 0.47f);
    glVertex2f(-0.135f, 0.47f);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(1.0f, 0.37f);
    glVertex2f(0.065f, 0.37f);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(-1.0f, 0.37f);
    glVertex2f(-0.065f, 0.37f);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(1.0f, 0.47f);
    glVertex2f(0.135f, 0.47f);
    glEnd();
    glFlush();

    glLineWidth(0.25);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-0.175f, 0.5f);
    glVertex2f(0.175f, 0.5f);
    glVertex2f(0.135f, 0.45f);
    glVertex2f(-0.135f, 0.45f);
    glEnd();
    //glFlush();

    glLineWidth(0.25);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-0.105f, 0.4f);
    glVertex2f(0.105f, 0.4f);
    glVertex2f(0.065f, 0.35f);
    glVertex2f(-0.065f, 0.35f);
    glEnd();
    //glFlush();

    glBegin(GL_QUADS);
    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex2f(-0.175f, 0.5f);
    glVertex2f(0.175f, 0.5f);
    glVertex2f(0.135f, 0.45f);
    glVertex2f(-0.135f, 0.45f);
    glEnd();
    glFlush();

    glBegin(GL_QUADS);
    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex2f(-0.105f, 0.4f);
    glVertex2f(0.105f, 0.4f);
    glVertex2f(0.065f, 0.35f);
    glVertex2f(-0.065f, 0.35f);
    glEnd();
    glFlush();
}

void drawHome()
{


    // RED  roof
    //SQUARE PART

    glColor3f(1.0f, 0.5f, 0.6f);
    glBegin(GL_POLYGON);
        glVertex2f(-3.1,3.07);
        glVertex2f(-3.1,3.20);
        glVertex2f(-0.05,4.55);
        glVertex2f(2.75,3.20);
        glVertex2f(2.75,3.07);
        glVertex2f(-0.05,4.5);
    glEnd();

    //SQUARE PART
    glColor3f(1.0f, 0.5f, 0.6f);
    glBegin(GL_POLYGON);

        glVertex2f(-0.20,4.60);
        glVertex2f(0.10,4.60);
        glVertex2f(0.10,4.3);
        glVertex2f(-0.20,4.3);
    glEnd();


// WHITE PART
   glColor3f(1.0f, 1.0f, 1.0f);
   glBegin(GL_POLYGON);
    glVertex2f(-2.58f, 3.11f);
    glVertex2f(-0.05f, 4.21f);
    glVertex2f(2.24f, 3.14f);
    glVertex2f(2.75f, 3.07f);
     glVertex2f(-0.05f, 4.4f);
    glVertex2f(-3.1f, 3.07f);
     glVertex2f(-2.56f, 2.89f);



glEnd();


    // 2nd floor body
    glColor3f(1.0f, 0.95f, 0.83f);
    glBegin(GL_POLYGON);
        glVertex2f(-2.58f, 0.54f);
         glVertex2f(-2.58f, 3.11f);
          glVertex2f(-0.05f, 4.21f);
          glVertex2f(2.24f, 3.11f);
           glVertex2f(2.24f, 1.19f);

    glEnd();
     glColor3f(1.0f, 0.75f, 0.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-2.58f, 0.54f);
         glVertex2f(-2.58f, 3.11f);
          glVertex2f(-0.05f, 4.21f);
          glVertex2f(2.24f, 3.11f);
           glVertex2f(2.24f, 1.19f);

    glEnd();


    // 1st window brown part
    glColor3f(0.76f, 0.60f, 0.42f);
    glBegin(GL_POLYGON);
        glVertex2f(-1.71,3.14);
        glVertex2f(-1.71,1.71);
        glVertex2f(-0.65,1.71);
        glVertex2f(-0.65,3.14);
    glEnd();

    //  window borders
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.5);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-1.71,3.14);
        glVertex2f(-1.71,1.71);
        glVertex2f(-0.65,1.71);
        glVertex2f(-0.65,3.14);
    glEnd();

    glBegin(GL_LINE_LOOP);
        glVertex2f(-1.60,2.99);
        glVertex2f(-1.6,1.86);
        glVertex2f(-0.76,1.86);
        glVertex2f(-0.76,2.99);
    glEnd();

    glLineWidth(1.0);
   //brown window lines
    glBegin(GL_LINES);
        glVertex2f(-1.6,2.70); glVertex2f(-0.87,2.70);
        glVertex2f(-1.6,2.85); glVertex2f(-0.79,2.85);
        glVertex2f(-1.6,2.00); glVertex2f(-0.8,2.00);
        glVertex2f(-1.6,2.23); glVertex2f(-1.27,2.23);
        glVertex2f(-1.6,2.57); glVertex2f(-0.98,2.57);
        glVertex2f(-1.6,2.46); glVertex2f(-0.77,2.46);
        glVertex2f(-1.6,2.33); glVertex2f(-0.8,2.33);
        glVertex2f(-1.6,2.12); glVertex2f(-0.9,2.12);
    glEnd();

    // big window blue part
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.65,3.14);
        glVertex2f(1.40,3.14);
        glVertex2f(1.40,1.71);
        glVertex2f(-0.65,1.71);
    glEnd();

    glColor3f(0.5f, 0.8f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.49,2.96);
        glVertex2f(1.22,2.96);
        glVertex2f(1.22,1.86);
        glVertex2f(-0.49,1.86);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(0.32,2.96);
        glVertex2f(0.45,2.96);
        glVertex2f(0.45,1.86);
        glVertex2f(0.32,1.86);
    glEnd();

    glColor3f(0,0,0);
    glLineWidth(1.5);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.65,3.14);
        glVertex2f(1.40,3.14);
        glVertex2f(1.40,1.71);
        glVertex2f(-0.65,1.71);
    glEnd();

    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.49,2.96);
        glVertex2f(1.22,2.96);
        glVertex2f(1.22,1.86);
        glVertex2f(-0.49,1.86);
    glEnd();

    glBegin(GL_LINE_LOOP);
        glVertex2f(0.32,2.96);
        glVertex2f(0.45,2.96);
        glVertex2f(0.45,1.86);
        glVertex2f(0.32,1.86);
    glEnd();

    // white strip
    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(-2,3.2);
        glVertex2f(-2,3.08);
        glVertex2f(1.59,3.08);
        glVertex2f(1.59,3.2);
    glEnd();

    // red top
    glColor3f(1.0f, 0.5f, 0.6f);
    glBegin(GL_POLYGON);
        glVertex2f(-2.0f, 3.2f);
        glVertex2f(-1.72f, 3.37f);
        glVertex2f(-0.4f, 3.37f);
        glVertex2f(1.27f, 3.37f);
        glVertex2f(1.59f, 3.2f);
    glEnd();

     // 1ST FLOOR  roof
     //red strip
     glColor3f(1.0f, 0.5f, 0.6f);
    glBegin(GL_POLYGON);
        glVertex2f(-3.20f, 0.5f);
        glVertex2f(-3.20f, 0.7f);
        glVertex2f(0.79f, 1.75f);
        glVertex2f(5.10f, 0.45f);
        glVertex2f(5.10f, 0.25f);
    glEnd();

//white strip
glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(-2.83f,0.4f);
        glVertex2f(0.79f,1.35f);
        glVertex2f(4.69,0.25);
        glVertex2f(5.10f,0.25f);
        glVertex2f(0.79f,1.55f);
        glVertex2f(-3.20f,0.5f);
          glVertex2f(-2.56f,0.0f);


    glEnd();


    // 1ST FLOOR body
    glColor3f(1.0f, 0.95f, 0.83f);
    glBegin(GL_POLYGON);
        glVertex2f(-2.83f,-2.72f);
        glVertex2f(-2.83f,0.4f);
        glVertex2f(0.79f,1.35f);
        glVertex2f(4.69f,0.25f);
        glVertex2f(4.69f,-2.72f);
    glEnd();

     glColor3f(1.0f, 0.75f, 0.0f);
 glLineWidth(1.0);
    glBegin(GL_LINE_LOOP);

        glVertex2f(-2.83f,-2.72f);
        glVertex2f(-2.83F,0.4f);
        glVertex2f(0.79f,1.35f);
        glVertex2f(4.69f,0.25f);
        glVertex2f(4.69f,-2.72f);
    glEnd();

// home road
 glColor3f(0.5f, 0.4f, 0.6f);

    glBegin(GL_POLYGON);
        glVertex2f(2.25f,-2.72f);
        glVertex2f(4.55f,-2.72f);
        glVertex2f(6.0f,-3.0f);
        glVertex2f(2.14f,-3.0f);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.5);
    glBegin(GL_LINE_LOOP);
        glVertex2f(2.25f,-2.72f);
        glVertex2f(4.55f,-2.72f);
        glVertex2f(6.0f,-3.0f);
        glVertex2f(2.14f,-3.0f);

    glEnd();

    //2ND WINDOW BROWN PART
     glColor3f(0.76f, 0.60f, 0.42f);
    glBegin(GL_POLYGON);
        glVertex2f(-2.45,0.0);
        glVertex2f(-1.1,0.0);
        glVertex2f(-1.1,-2);
        glVertex2f(-2.45,-2);
    glEnd();
    // BORDER
    glColor3f(0,0,0);
    glLineWidth(1.5);
     glBegin(GL_LINE_LOOP);
       glVertex2f(-2.45,0.0);
        glVertex2f(-1.1,0.0);
        glVertex2f(-1.1,-2);
        glVertex2f(-2.45,-2);
    glEnd();
   // INSIDE BORDER
    glColor3f(0,0,0);
    glLineWidth(1.5);
     glBegin(GL_LINE_LOOP);
       glVertex2f(-2.28, -0.20);
        glVertex2f(-1.25, -0.20);
        glVertex2f(-1.25, -1.91);
        glVertex2f(-2.28, -1.91);
    glEnd();
    // BROWN WINDOW LINES
       glLineWidth(1.0);
   //brown window lines
    glBegin(GL_LINES);
        glVertex2f(-2.28, -0.35); glVertex2f(-1.25,-0.35);
         glVertex2f(-2.28, -0.50);glVertex2f(-1.50, -0.5);
         glVertex2f(-2.28, -0.65);glVertex2f(-1.80, -0.65);
         glVertex2f(-2.28, -0.80);glVertex2f(-1.40, -0.80);
         glVertex2f(-2.28, -0.95); glVertex2f(-1.25,-0.95);
         glVertex2f(-2.28, -1.10); glVertex2f(-1.60,-1.10);
         glVertex2f(-2.28, -1.25); glVertex2f(-1.25,-1.25);
         glVertex2f(-2.28, -1.40); glVertex2f(-1.30,-1.40);
    glEnd();
     //BLUE WIMDOW PART
      glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);

        glVertex2f(-1.1,0.0);
        glVertex2f(1.35, 0.0);
         glVertex2f(1.35, -2);
        glVertex2f(-1.1,-2);

    glEnd();
    glColor3f(0,0,0);
    glLineWidth(1.5);
     glBegin(GL_LINE_LOOP);
        glVertex2f(-1.1,0.0);
        glVertex2f(1.35, 0.0);
         glVertex2f(1.35, -2);
        glVertex2f(-1.1,-2);

    glEnd();
  //WINDOW SKYBLUE PART
     glColor3f(0.5f, 0.8f, 1.0f);
     glBegin(GL_POLYGON);
       glVertex2f(-0.90, -0.20);
        glVertex2f(1.15, -0.20);
        glVertex2f(1.15,-1.91);
        glVertex2f(-0.90,-1.91);
    glEnd();

   glColor3f(0,0,0);
    glLineWidth(1.5);
     glBegin(GL_LINE_LOOP);
     glVertex2f(-0.90, -0.20);
        glVertex2f(1.15, -0.20);
        glVertex2f(1.15,-1.91);
        glVertex2f(-0.90,-1.91);
    glEnd();
     glColor3f(0.0f, 0.0f, 1.0f);
     glBegin(GL_POLYGON);
       glVertex2f(0.0, -0.20);
        glVertex2f(0.2, -0.20);
        glVertex2f(0.2,-1.91);
        glVertex2f(0.0,-1.91);
    glEnd();

glColor3f(0,0,0);
    glLineWidth(1.5);
     glBegin(GL_LINE_LOOP);
     glVertex2f(0.0, -0.20);
        glVertex2f(0.2, -0.20);
        glVertex2f(0.2,-1.91);
        glVertex2f(0.0,-1.91);
    glEnd();

    //white strip
     glColor3f(1.0f, 1.0f, 1.0f);
     glBegin(GL_POLYGON);
       glVertex2f(-2.68, 0.1);
        glVertex2f(1.35, 0.10);
        glVertex2f(1.35,-0.1);
        glVertex2f(-2.68,-0.1);
    glEnd();
//red top
     glColor3f(1.0f, 0.5f, 0.6f);
glBegin(GL_POLYGON);
    glVertex2f(-2.68,  0.1);
    glVertex2f( -2.0, 0.3);
    glVertex2f( 1.53,0.3);
    glVertex2f(1.35, 0.1);
glEnd();
//window another part
 glColor3f(1.0f, 1.0f, 1.0f);
glBegin(GL_POLYGON);
    glVertex2f(1.53, 0.3);
    glVertex2f( 1.35, 0.1);
    glVertex2f( 1.35,-0.1);
    glVertex2f(1.53, 0.1);
glEnd();



    // door
    glColor3f(0.5f, 0.2f, 0.03f);
    glBegin(GL_POLYGON);
        glVertex2f(2.25f,-2.72f);
        glVertex2f(4.35f,-2.72f);
        glVertex2f(4.35f,0.0f);
        glVertex2f(2.25f,0.0f);
    glEnd();
    glColor3f(0,0,0);
    glBegin(GL_LINE_STRIP);

        glVertex2f(4.35f,-2.72f);
        glVertex2f(4.35f,0.0f);
        glVertex2f(2.25f,0.0f);
         glVertex2f(2.25f,-2.72f);
    glEnd();

    // border
    glColor3f(0,0,0);
    glBegin(GL_LINE_STRIP);
        glVertex2f(2.45f, -2.72f);
        glVertex2f(2.45f, -0.2f);
        glVertex2f(4.15f, -0.2f);
        glVertex2f(4.15f, -2.72f);

    glEnd();


    glEnd();
 // DOOR SIDE SKYBLUE PART
    glColor3f(0.5f, 0.6f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(3.75f,0.0f);
        glVertex2f(4.15f,0.0f);
        glVertex2f(4.15f,-2.72f);
        glVertex2f(3.75f,-2.72f);
    glEnd();
    // INSIDE PART
     glColor3f(0.4f, 0.0f, 0.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(3.85,-0.7f);
        glVertex2f(4.05f,-0.7f);
        glVertex2f(4.05f,-1.4f);
        glVertex2f(3.85f,-1.4f);
    glEnd();
     glColor3f(0.4f, 0.0f, 0.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(3.85,-1.5f);
        glVertex2f(4.05f,-1.5f);
        glVertex2f(4.05f,-2.2f);
        glVertex2f(3.85f,-2.2f);
    glEnd();
     glBegin(GL_LINE_LOOP);
        glVertex2f(3.85,-2.3f);
        glVertex2f(4.05f,-2.3f);
        glVertex2f(4.05f,-2.9f);
        glVertex2f(3.85f,-2.9f);
    glEnd();
    //SIDE PART BORDER
 glColor3f(0,0,0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(3.75f,-2.72f);
        glVertex2f(3.75f,0.0f);
        glVertex2f(4.15f,0.0f);
        glVertex2f(4.15f,-2.72f);

    glEnd();
    // DOOR TOP SKYBLUE PART
    glColor3f(0.5f, 0.6f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(2.45f,0.0f);
        glVertex2f(4.15f,0.0f);
        glVertex2f(4.15f,-0.4f);
        glVertex2f(2.45f,-0.4f);
    glEnd();
    // TOP SKY BLUE INSIDE PART
    glColor3f(0.4f, 0.0f, 0.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(2.55f,-0.1f);
        glVertex2f(3.25f,-0.1f);
        glVertex2f(3.25f,-0.3f);
        glVertex2f(2.55f,-0.3f);
    glEnd();


     glColor3f(0.4f, 0.0f, 0.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(3.35f,-0.1f);
        glVertex2f(4.05f,-0.1f);
        glVertex2f(4.05f,-0.3f);
        glVertex2f(3.35f,-0.3f);
    glEnd();


    //border part
  glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(2.45f,-0.4f);
        glVertex2f(2.45f,0.0f);
        glVertex2f(4.15f,0.0f);
        glVertex2f(4.15f,-0.4f);

    glEnd();
   //border
    glBegin(GL_LINES);
    glVertex2f(2.45f,-0.4f);
    glVertex2f(3.75f,-0.4f);
      glEnd();
      //door white strip
     glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(2.05f,0.1f);
        glVertex2f(4.55f,0.1f);
        glVertex2f(4.55f,-0.1f);
        glVertex2f(2.05f,-0.1f);
    glEnd();
    // door REDtop part
         glColor3f(1.0f, 0.5f, 0.6f);
    glBegin(GL_POLYGON);
        glVertex2f(2.05f,0.1f);
        glVertex2f(2.45f,0.3f);
        glVertex2f(4.15f,0.3f);
        glVertex2f(4.55f,0.1f);

    glEnd();
    //DOOR MIDDLE BROWN PART
       glColor3f(0.5f, 0.2f, 0.03f);
    glBegin(GL_POLYGON);
        glVertex2f(2.45f,-0.4f);
        glVertex2f(4.15f,-0.4f);
        glVertex2f(4.15f,-0.6f);
        glVertex2f(2.45f,-0.6f);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
         glVertex2f(2.45f,-0.4f);
        glVertex2f(4.15f,-0.4f);
        glVertex2f(4.15f,-0.6f);
        glVertex2f(2.45f,-0.6f);

    glEnd();
    //DOOR SIDE BORDER
     glBegin(GL_LINES);
    glVertex2f(3.55f,-0.6f);
    glVertex2f(3.55f,-2.72f);
      glEnd();
    //DOOR INSIDE QUAD
    glColor3f(0.5f, 0.6f, 1.0f);
    glBegin(GL_POLYGON);
       glVertex2f(2.75f,-0.8f);
        glVertex2f(3.25f,-0.8f);
        glVertex2f(3.25f,-1.5f);
        glVertex2f(2.75f,-1.5f);


    glEnd();
     glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
         glVertex2f(2.75f,-0.8f);
        glVertex2f(3.25f,-0.8f);
        glVertex2f(3.25f,-1.5f);
        glVertex2f(2.75f,-1.5f);

    glEnd();
     glColor3f(0.4f, 0.0f, 0.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(2.85f,-0.9f);
        glVertex2f(3.15f,-0.9f);
        glVertex2f(3.15f,-1.4f);
        glVertex2f(2.85f,-1.4f);
    glEnd();
    //DOOR  LOCK
    glColor3f(0.5f, 0.6f, 1.0f);
    glBegin(GL_POLYGON);
       glVertex2f(3.33f,-1.6f);
        glVertex2f(3.45f,-1.6f);
        glVertex2f(3.45f,-1.9f);
        glVertex2f(3.33f,-1.9f);
glEnd();
glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
          glVertex2f(3.33f,-1.6f);
        glVertex2f(3.45f,-1.6f);
        glVertex2f(3.45f,-1.9f);
        glVertex2f(3.33f,-1.9f);
    glEnd();
//CIRCLE PART
glBegin(GL_POLYGON);
for( int i=0;i<200;i++)
    {
glColor3f(0.5f,0.6f,1.0f);
float pi= 3.1416;
float A =(i*2*pi)/200;
float r = 0.07;
float x= 3.39+r* cos(A);
float y= -1.72+r* sin(A);
glVertex2f(x,y);
    }
glEnd();

glLineWidth(1.5);

glBegin(GL_LINE_LOOP);

for( int i=0;i<100;i++)
    {
glColor3f(0.0f,0.0f,0.0f);
float pi= 3.1416;
float A =(i*2*pi)/100;
float r = 0.07;
float x= 3.39+r* cos(A);
float y= -1.72+r* sin(A);
glVertex2f(x,y);
    }
glEnd();


// main gate part
//1ST LEFT PART
float startX = 2.14f;
float width  = 0.09f;
float gap    = 0.15f;

for(int i = 0; i < 3; i++)
{
    float x1 = startX + i * gap;     // left
    float x2 = x1 + width;           // right
    float y1 = -1.6f;                // top
    float y2 = -2.9f;                // bottom

    // Filled polygon (body)
    glColor3f(0.0f, 0.5f, 1.0f);

    glBegin(GL_POLYGON);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();

    // Border (outline)
    glLineWidth(2.0f);          // border thickness
    glColor3f(0.0f, 0.0f, 0.0f); // border color black

    glBegin(GL_LINE_LOOP);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();
}



//LEFT PART
glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_POLYGON);

        glVertex2f(2.14f, -1.6f);
        glVertex2f(2.47f, -1.5f);
         glVertex2f(2.47f, -1.6f);
         glVertex2f(2.14f, -1.7f);

    glEnd();
    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(2.14f, -1.6f);
        glVertex2f(2.47f, -1.5f);
         glVertex2f(2.47f, -1.6f);
         glVertex2f(2.14f, -1.7f);
glEnd();
glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_POLYGON);

        glVertex2f(2.14f, -2.9f);
        glVertex2f(2.47f, -2.8f);
         glVertex2f(2.47f, -2.9f);
         glVertex2f(2.14f, -3.0f);

    glEnd();
    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(2.14f, -2.9f);
        glVertex2f(2.47f, -2.8f);
         glVertex2f(2.47f, -2.9f);
         glVertex2f(2.14f, -3.0f);
glEnd();

    glBegin(GL_POLYGON);
for( int i=0;i<200;i++)
    {
glColor3f(0.0f,0.5f,1.0f);
float pi= 3.1416;
float A =(i*2*pi)/200;
float r = 0.07;
float x= 2.47+r* cos(A);
float y= -1.55+r* sin(A);
glVertex2f(x,y);
    }
glEnd();

glLineWidth(1.5);

glBegin(GL_LINE_LOOP);

for( int i=0;i<100;i++)
    {
glColor3f(0.0f,0.0f,0.0f);
float pi= 3.1416;
float A =(i*2*pi)/100;
float r = 0.07;
float x= 2.47+r* cos(A);
float y= -1.55+r* sin(A);
glVertex2f(x,y);
    }
glEnd();

 glBegin(GL_POLYGON);
for( int i=0;i<200;i++)
    {
glColor3f(0.0f,0.5f,1.0f);
float pi= 3.1416;
float A =(i*2*pi)/200;
float r = 0.07;
float x= 2.47+r* cos(A);
float y= -2.85+r* sin(A);
glVertex2f(x,y);
    }
glEnd();

glLineWidth(1.5);

glBegin(GL_LINE_LOOP);

for( int i=0;i<100;i++)
    {
glColor3f(0.0f,0.0f,0.0f);
float pi= 3.1416;
float A =(i*2*pi)/100;
float r = 0.07;
float x= 2.47+r* cos(A);
float y= -2.85+r* sin(A);
glVertex2f(x,y);
    }
glEnd();

//2ND GATE PART
 startX = 4.46f;
 width  = 0.10f;
 gap    = 0.20f;

for(int i = 0; i < 6; i++)
{
    float x1 = startX - i * gap;     // left
    float x2 = x1 + width;           // right
    float y1 = -1.7f;                // top
    float y2 = -2.9f;                // bottom

    // Filled polygon (body)
    glColor3f(0.0f, 0.5f, 1.0f);

    glBegin(GL_POLYGON);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();

    // Border (outline)
    glLineWidth(2.0f);          // border thickness
    glColor3f(0.0f, 0.0f, 0.0f); // border color black

    glBegin(GL_LINE_LOOP);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();
}

//1ST PART
//BLUE PART
glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(3.52f, -1.6f);
        glVertex2f(4.56f, -1.6f);
        glVertex2f(4.56f, -1.7f);
        glVertex2f(3.52f, -1.7f);
    glEnd();
    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(3.52f, -1.6f);
        glVertex2f(4.56f, -1.6f);
        glVertex2f(4.56f, -1.7f);
        glVertex2f(3.52f, -1.7f);
    glEnd();

glBegin(GL_POLYGON);
for( int i=0;i<200;i++)
    {
glColor3f(0.0f,0.5f,1.0f);
float pi= 3.1416;
float A =(i*2*pi)/200;
float r = 0.07;
float x= 3.52+r* cos(A);
float y= -1.65+r* sin(A);
glVertex2f(x,y);
    }
glEnd();

glLineWidth(1.5);

glBegin(GL_LINE_LOOP);

for( int i=0;i<100;i++)
    {
glColor3f(0.0f,0.0f,0.0f);
float pi= 3.1416;
float A =(i*2*pi)/100;
float r = 0.07;
float x= 3.52+r* cos(A);
float y= -1.65+r* sin(A);
glVertex2f(x,y);
    }
glEnd();

//2ND PART
//BLUE PART
glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(3.52f, -2.9f);
        glVertex2f(4.56f, -2.9f);
        glVertex2f(4.56f, -3.0f);
        glVertex2f(3.52f, -3.0f);
    glEnd();
    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(3.52f, -2.9f);
        glVertex2f(4.56f, -2.9f);
        glVertex2f(4.56f, -3.0f);
        glVertex2f(3.52f, -3.0f);
    glEnd();

glBegin(GL_POLYGON);
for( int i=0;i<200;i++)
    {
glColor3f(0.0f,0.5f,1.0f);
float pi= 3.1416;
float A =(i*2*pi)/200;
float r = 0.07;
float x= 3.52+r* cos(A);
float y= -2.95+r* sin(A);
glVertex2f(x,y);
    }
glEnd();

glLineWidth(1.5);

glBegin(GL_LINE_LOOP);

for( int i=0;i<100;i++)
    {
glColor3f(0.0f,0.0f,0.0f);
float pi= 3.1416;
float A =(i*2*pi)/100;
float r = 0.07;
float x= 3.52+r* cos(A);
float y= -2.95+r* sin(A);
glVertex2f(x,y);
    }
glEnd();

    // first rectangle (wall)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_POLYGON);
        glVertex2f(-6.34f, -3.0f);
        glVertex2f(2.14f, -3.0f);
        glVertex2f(2.14f, -1.44f);
        glVertex2f(-6.34f, -1.44f);
    glEnd();

    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-6.34f, -3.0f);
        glVertex2f(2.14f, -3.0f);
        glVertex2f(2.14f, -1.44f);
        glVertex2f(-6.34f, -1.44f);
    glEnd();


    float y1 = -3.0f, y2 = -1.44f;
    float start_x1 = -6.36f, x_spacing1 = 0.5f;
    int num_vlines1 = 18;

    glLineWidth(0.5);
    glBegin(GL_LINES);
        for(int i=0;i<num_vlines1;i++){
            float x = start_x1 + i * x_spacing1;
            glVertex2f(x, y1);
            glVertex2f(x, y2);
        }
    glEnd();

    float h_start_y1 = -1.44f;
    float h_start_x1 = -6.36f, h_end_x1 = 2.14f;
    float h_spacing1 = (-1.44f - (-3.0f)) / 6;
    int num_hlines1 = 7;

    glBegin(GL_LINES);
        for(int i=0;i<num_hlines1;i++){
            float y = h_start_y1 - i * h_spacing1;
            glVertex2f(h_start_x1, y);
            glVertex2f(h_end_x1, y);
        }
    glEnd();

    // second rectangle
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_POLYGON);
        glVertex2f(4.56f, -3.0f);
        glVertex2f(4.56f, -1.44f);
        glVertex2f(6.76f, -1.44f);
        glVertex2f(6.76f, -3.0f);
    glEnd();

    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(4.56f, -3.0f);
        glVertex2f(4.56f, -1.44f);
        glVertex2f(6.76f, -1.44f);
        glVertex2f(6.76f, -3.0f);
    glEnd();

    float start_x2 = 4.56f, x_spacing2 = 0.5f;
    int num_vlines2 = 5;

    glBegin(GL_LINES);
        for(int i=0;i<num_vlines2;i++){
            float x = start_x2 + i * x_spacing2;
            glVertex2f(x, y1);
            glVertex2f(x, y2);
        }
    glEnd();

    float h_start_x2 = 4.56f, h_end_x2 = 6.76f;
    float h_spacing2 = (-1.44f - (-3.0f)) / 6;
    int num_hlines2 = 7;

    glBegin(GL_LINES);
        for(int i=0;i<num_hlines2;i++){
            float y = -1.44f - i * h_spacing2;
            glVertex2f(h_start_x2, y);
            glVertex2f(h_end_x2, y);
        }
    glEnd();
    glFlush();
}

void drawEllipse1(float cx, float cy, float rx, float ry)
{
    int segments = 100;
    glColor3f(0.639f, 0.784f, 0.886f);
    //glColor3f(0.45f, 0.75f, 1.0f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < segments; i++)
    {
        float theta = 2.0f * 3.1416f * i / segments;
        glVertex2f(cx + rx*cos(theta), cy + ry*sin(theta));
    }
    glEnd();

    glColor3f(0.353,0.549,0.631);
   // glColor3f(0.0f, 0.3f, 0.7f);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < segments; i++)
    {
        float theta = 2.0f * 3.1416f * i / segments;
        glVertex2f(cx + rx*cos(theta), cy + ry*sin(theta));
    }
    glEnd();
}

void waterPuddle(){
    float e_cx=(-0.66+2.35)/2;
    float e_cy=(0.6+1.58)/2;
    float e_rx=(2.35-(-0.66))/2;
    float e_ry=(1.58-0.6)/2;
    drawEllipse1(e_cx, e_cy, e_rx, e_ry);

    float f_cx=(-0.22+2.0)/2;
    float f_cy=(0.85+1.43)/2;
    float f_rx=(2-(-0.22))/2;
    float f_ry=(1.43-0.85)/2;
    drawEllipse1(f_cx, f_cy, f_rx, f_ry);

    float g_cx=(2.34+3.02)/2;
    float g_cy=(0.5+0.69)/2;
    float g_rx=(3.02-2.34)/2;
    float g_ry=(0.69-0.5)/2;
    drawEllipse1(g_cx, g_cy, g_rx, g_ry);

    float h_cx=(1.2+2.39)/2;
    float h_cy=(0.16+0.62)/2;
    float h_rx=(2.39-1.2)/2;
    float h_ry=(0.62-0.16)/2;
    drawEllipse1(h_cx, h_cy, h_rx, h_ry);

    glFlush();

}

void drawDoraemonShadow(float x, float y, float scale)
{
    glPushMatrix();
    glTranslatef(x, y - 0.05f, 0.0f);
    glScalef(scale, scale * 0.2f, 1.0f);

    glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
    int segments = 50;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = cos(angle);
        float y = sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    glPopMatrix();
}

void drawLampPost()
{
    //glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);
    glPointSize(10.0f);
    //glDisable(GL_LIGHTING);
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(0.1f, 0.2f, 0.2f);
    glVertex2f(-0.1f, 0.6f);
    glVertex2f(-0.13f, 0.6f);
    glVertex2f(-0.13f, -0.6f);
    glVertex2f(-0.1f, -0.6f);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(0.1f, 0.2f, 0.2f);
    glVertex2f(-0.13f, 0.6f);
    glVertex2f(-0.1f, 0.63f);
    glVertex2f(0.1f, 0.6f);
    glVertex2f(0.13f, 0.63f);
    glEnd();

    int layers = 20;

    for (int i = 0; i < layers; i++) {
        float t = (float)i / layers;

        float alpha = 0.15f - t * 0.12f;
        if (alpha < 0.01f) continue;

        float spread = t * 0.15f;

        glBegin(GL_TRIANGLES);
        glColor4f(0.7f, 0.7f, 0.7f, alpha);

        glVertex2f(0.103f, 0.632f);
        glVertex2f(-0.55f - spread, -1.5f);
        glVertex2f(0.55f + spread, -1.5f);

        glEnd();
    }


    float radius = 0.03f;
    int segments = 200;
    float cx = 0.1f;
    float cy = 0.62f;

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.9f, 0.9f, 0.9f);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = cx + radius * cos(angle);
        float y = cy + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();




    glFlush();

}

void drawLampPostWithLighting()
{
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    drawLampPost();

    GLfloat lightPos[]     = { 0.0f, 0.6f, 1.5f, 1.0f };
    GLfloat lightAmbient[] = { 0.12f, 0.12f, 0.12f, 1.0f };
    GLfloat lightDiffuse[] = { 0.9f, 0.9f, 0.7f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    glPopAttrib();
}

void Tree1()
{
    //glPointSize(5.0f);
    //glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.1, 0.2);
    glVertex2f(-0.1, 0.2);
    glVertex2f(-0.1, -0.9);
    glVertex2f(0.1, -0.9);
    glEnd();

    glColor3f(0.5f, 0.3f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(0.1, 0.2);
    glVertex2f(-0.1, 0.2);
    glVertex2f(-0.1, -0.9);
    glVertex2f(0.1, -0.9);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.6,-0.45);
    glVertex2f(0.0,0.2);
    glVertex2f(-0.6,-0.45);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex2f(0.5,-0.3);
    glVertex2f(0.0,0.4);
    glVertex2f(-0.5,-0.3);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex2f(0.4,-0.1);
    glVertex2f(0.0,0.6);
    glVertex2f(-0.4,-0.1);

    glBegin(GL_LINE_LOOP);
    glVertex2f(0.3,0.1);
    glVertex2f(0.0,0.6);
    glVertex2f(-0.3,0.1);
    glEnd();

    glColor3f(0.2f, 0.6f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.6,-0.45);
    glVertex2f(0.0,0.2);
    glVertex2f(-0.6,-0.45);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(0.5,-0.3);
    glVertex2f(0.0,0.4);
    glVertex2f(-0.5,-0.3);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(0.4,-0.1);
    glVertex2f(0.0,0.6);
    glVertex2f(-0.4,-0.1);

    glBegin(GL_TRIANGLES);
    glVertex2f(0.3,0.1);
    glVertex2f(0.0,0.6);
    glVertex2f(-0.3,0.1);
    glEnd();

    glFlush();
}

void drawwall()
{
// first rectangle (wall)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_POLYGON);
        glVertex2f(-6.34f, -3.0f);
        glVertex2f(2.14f, -3.0f);
        glVertex2f(2.14f, -1.44f);
        glVertex2f(-6.34f, -1.44f);
    glEnd();

    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-6.34f, -3.0f);
        glVertex2f(2.14f, -3.0f);
        glVertex2f(2.14f, -1.44f);
        glVertex2f(-6.34f, -1.44f);
    glEnd();


    float y1 = -3.0f, y2 = -1.44f;
    float start_x1 = -6.36f, x_spacing1 = 0.5f;
    int num_vlines1 = 18;

    glLineWidth(0.5);
    glBegin(GL_LINES);
        for(int i=0;i<num_vlines1;i++){
            float x = start_x1 + i * x_spacing1;
            glVertex2f(x, y1);
            glVertex2f(x, y2);
        }
    glEnd();

    float h_start_y1 = -1.44f;
    float h_start_x1 = -6.36f, h_end_x1 = 2.14f;
    float h_spacing1 = (-1.44f - (-3.0f)) / 6;
    int num_hlines1 = 7;

    glBegin(GL_LINES);
        for(int i=0;i<num_hlines1;i++){
            float y = h_start_y1 - i * h_spacing1;
            glVertex2f(h_start_x1, y);
            glVertex2f(h_end_x1, y);
        }
    glEnd();

    glFlush();
}
void drawBox(){
        glColor3f(0.7f, 0.4f, 0.2f);

//polygon1
            glBegin(GL_POLYGON);

                   glVertex2f(-3.32, -1.96);
                   glVertex2f(-3.32, 3.06);
                   glVertex2f(0.44, 2.13);
                   glVertex2f(0.44f, -2.99f);

             glEnd();

//polygon2
            glBegin(GL_POLYGON);
                   glVertex2f(0.44f, -2.99f);
                   glVertex2f(0.44, 2.13);
                   glVertex2f(4.0, 3.59);
                   glVertex2f(4.0f, -1.49f);

             glEnd();
//polygon3
            glBegin(GL_POLYGON);

                   glVertex2f(-3.32, 3.06);
                   glVertex2f(0.44, 2.13);
                   glVertex2f(4.0, 3.59);
                   glVertex2f(0.34f, 4.44f);

             glEnd();

//border
             glColor3f(0.0f, 0.0f, 0.0f);
             glLineWidth(2.0);
             glBegin(GL_LINE_LOOP);

                   glVertex2f(-3.32, -1.96);
                   glVertex2f(-3.32, 3.06);
                   glVertex2f(0.44, 2.13);
                   glVertex2f(0.44f, -2.99f);

             glEnd();

            glColor3f(0.0f, 0.0f, 0.0f);
             glBegin(GL_LINE_LOOP);
                   glVertex2f(0.44f, -2.99f);
                   glVertex2f(0.44, 2.13);
                   glVertex2f(4.0, 3.59);
                   glVertex2f(4.0f, -1.49f);

             glEnd();

             glBegin(GL_LINE_LOOP);
                     glVertex2f(-3.32, 3.06);
                   glVertex2f(0.44, 2.13);
                   glVertex2f(4.0, 3.59);
                   glVertex2f(0.34f, 4.44f);

             glEnd();

             glBegin(GL_LINES);
             glVertex2f(-3.32, 2.41);
             glVertex2f(0.44, 1.42);
             glEnd();

             glBegin(GL_LINES);
             glVertex2f(0.44, 1.42);
             glVertex2f(4.0, 2.95);
             glEnd();

             glBegin(GL_LINES);
             glVertex2f(0.44, -2.32);
             glVertex2f(4.0, -0.82);
             glEnd();

             glBegin(GL_LINES);
             glVertex2f(-3.32, -1.29);
             glVertex2f(0.44, -2.32);
             glEnd();
              glColor3f(0.7f, 0.5f, 0.3f);
              glBegin(GL_POLYGON);
                     glVertex2f(-2.65, 3.10);
                   glVertex2f(0.32, 4.23);
                   glVertex2f(3.37, 3.52);
                   glVertex2f(0.39, 2.38);

             glEnd();
             glColor3f(0.0f, 0.0f, 0.0f);
              glBegin(GL_LINE_LOOP);
                     glVertex2f(-2.65, 3.10);
                   glVertex2f(0.32, 4.23);
                   glVertex2f(3.37, 3.52);
                   glVertex2f(0.39, 2.38);

             glEnd();

            glBegin(GL_LINES);
             glVertex2f(-2.65, 3.10);
               glVertex2f(3.37, 3.52);
             glEnd();
             glBegin(GL_LINES);
             glVertex2f(0.32, 4.23);
              glVertex2f(0.39, 2.38);
             glEnd();
}

void wallScene()
{
    glPushMatrix();
        glTranslatef(4.0f, 3.0f, 0.0f);
        glScalef(6.0f, 6.9f, 1.0f);
        Tree1();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-3.0f, 3.5f, 0.0f);
        glScalef(6.5f, 7.9f, 1.0f);
        Tree1();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 3.0f, 0.0f);
        glScalef(6.0f, 6.9f, 1.0f);
        Tree1();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-7.0f, 2.5f, 0.0f);
        glScalef(5.5f, 6.9f, 1.0f);
        Tree1();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(4.0f, 3.0f, 0.0f);
        glScalef(2.5f, 2.5f, 1.6f);
        drawwall();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-5.5f, -0.5f, 0.0f);
        glScalef(7.0f, 7.0f, 1.0f);
        electricPole();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(8.5f, -0.5f, 0.0f);
        glScalef(7.0f, 7.0f, 1.0f);
        electricPole();
    glPopMatrix();

     glPushMatrix();
        glTranslatef(2.0f, -5.f, 0.0f);
        glScalef(0.250f, 0.250f, 1.0f);
        //drawBox();
    glPopMatrix();

}

void houseScene()
{

        glPushMatrix();
        glScalef(1.5f,1.5f,1.0f);
        drawHome();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(8.5f, -0.5f, 0.0f);
        glScalef(7.0f, 7.0f, 1.0f);
        electricPole();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-5.5f, -0.5f, 0.0f);
        glScalef(7.0f, 7.0f, 1.0f);
        electricPole();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-8.0f, -2.650f, 0.0f);
        glScalef(.7f, .70f, 1.0f);
        drawFlower();
        glPopMatrix();



}

void placeStones()
{
    float minDistance = 5.0f;

    float margin = 2.0f;
    float minLocalX = -9.0f + margin;
    float maxLocalX =  9.0f - margin;

    for (int i = 0; i < TOTAL_STONES; i++) {
        bool valid = false;

        while (!valid) {
            int sceneIndex = rand() % totalScenes;
            float localX = minLocalX + (rand() / (float)RAND_MAX) * (maxLocalX - minLocalX);
            float candidateX = sceneIndex * sceneWidth + localX;

            valid = true;
            for (int j = 0; j < i; j++) {
                if (fabs(candidateX - stoneX[j]) < minDistance) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                stoneX[i] = candidateX;
                stoneY[i] = -7.5f;     // ground
                stoneHit[i] = false;
            }
        }
    }
}

void drawStone()
{


// FRONT POLYGON[1]

    glColor3f(1.0f, 0.6f, 0.2f);
            glBegin(GL_POLYGON);
                glVertex2f(-1.11f, 2.25f);
                glVertex2f(-1.11f, 4.09f);
                glVertex2f(-0.63f, 4.64f);
                glVertex2f(-0.54f, 5.56f);
                glVertex2f(0.39f, 6.06f);
                glVertex2f(0.85f, 6.98f);
                glVertex2f(1.5f, 7.30f);
                glVertex2f(1.8f,5.27f);
                glVertex2f(2.40f, 5.03f);
                glVertex2f(2.40f, 2.77f);
    glEnd();

    // 1ST B0TTOM[2]

           glColor3f(0.8f, 0.4f, 0.2f);
            glBegin(GL_POLYGON);
                glVertex2f(-2.3f, 2.41f);
                 glVertex2f(-2.1f, 3.85f);
                glVertex2f(-1.11f, 4.09f);
                 glVertex2f(-1.11f, 2.25f);

             glEnd();



             // B0TTOM UP[3]
               glColor3f(1.0f, 0.6f, 0.2f);
            glBegin(GL_POLYGON);

                 glVertex2f(-2.1f, 3.85f);
                 glVertex2f(-1.41f, 4.32f);
                 glVertex2f(-0.63f, 4.64f);
                 glVertex2f(-1.11f, 4.09f);


             glEnd();
            glColor3f(0.7f, 0.4f, 0.2f);
            glBegin(GL_LINE_LOOP);

                 glVertex2f(-2.1f, 3.85f);
                 glVertex2f(-1.41f, 4.32f);
                 glVertex2f(-0.63f, 4.64f);
                 glVertex2f(-1.11f, 4.09f);


             glEnd();

     // B0TT0M UP UP[4]
             glColor3f(0.7f, 0.4f, 0.2f);
            glBegin(GL_POLYGON);

                 glVertex2f(-1.41f, 4.32f);
                 glVertex2f(-1.35f, 5.72f);
                  glVertex2f(-0.54f, 5.56f);
                  glVertex2f(-0.63f, 4.64f);

             glEnd();

// BOTTOM UP 3[5]
          glColor3f(1.0f, 0.6f, 0.2f);
            glBegin(GL_POLYGON);


                 glVertex2f(-1.35f, 5.72f);
                  glVertex2f(-0.49f, 6.17);
                   glVertex2f(0.39f, 6.06f);
                    glVertex2f(-0.54f, 5.56f);

             glEnd();
             glLineWidth(2.0);
             glColor3f(0.7f, 0.4f, 0.2f);
            glBegin(GL_LINE_STRIP);


                 glVertex2f(-1.35f, 5.72f);
                  glVertex2f(-0.49f, 6.17);
                   glVertex2f(0.39f, 6.06f);
                    glVertex2f(-0.54f, 5.56f);

             glEnd();
//POLYGON[6]
               glColor3f(0.8f, 0.5f, 0.2f);
            glBegin(GL_POLYGON);


                  glVertex2f(-0.49f, 6.17);
                  glVertex2f(0.14f, 7.15);
                  glVertex2f(0.85f, 6.98f);
                  glVertex2f(0.39f, 6.06f);


             glEnd();
             /* glColor3f(0.8f, 0.4f, 0.2f);
            glBegin(GL_LINE_LOOP);


                  glVertex2f(-0.49f, 6.17);
                  glVertex2f(0.14f, 7.15);
                  glVertex2f(0.85f, 6.98f);
                  glVertex2f(0.39f, 6.06f);


             glEnd();*/

    //POLY[7[7]
     glColor3f(1.0f, 0.6f, 0.2f);
            glBegin(GL_POLYGON);


                   glVertex2f(0.14f, 7.15);
                  glVertex2f(0.80f, 7.7);
                  glVertex2f(1.6f, 7.7f);
                  glVertex2f(1.5f, 7.30f);
                   glVertex2f(0.85f, 6.98f);

             glEnd();
             //
//POLY[8]
     glColor3f(0.7f, 0.4f, 0.2f);
            glBegin(GL_POLYGON);

                  glVertex2f(1.6f, 7.7f);
                  glVertex2f(2.0f, 5.6);
                  glVertex2f(1.8f, 5.27f);
                  glVertex2f(1.5f, 7.30f);

             glEnd();

    //POLY[9]
    glColor3f(1.0f, 0.6f, 0.2f);
            glBegin(GL_POLYGON);

                  glVertex2f(2.0f, 5.6);
                  glVertex2f(3.0f, 5.40f);
                  glVertex2f(2.40f, 5.03f);
                  glVertex2f(1.8f, 5.27f);


             glEnd();
     glColor3f(1.0f, 0.6f, 0.2f);
            glBegin(GL_LINE_LOOP);

                  glVertex2f(2.0f, 5.6);
                  glVertex2f(3.0f, 5.40f);
                  glVertex2f(2.40f, 5.03f);
                  glVertex2f(1.8f, 5.27f);


             glEnd();
    //POLY[10]
    glColor3f(0.7f, 0.4f, 0.2f);
            glBegin(GL_POLYGON);


                  glVertex2f(3.0f, 5.40f);
                   glVertex2f(3.30f, 3.80f);

                   glVertex2f(2.40f, 2.77f);
                   glVertex2f(2.40f, 5.03f);

             glEnd();
     glColor3f(1.0f, 0.6f, 0.2f);
            glBegin(GL_LINE_LOOP);


                  glVertex2f(3.0f, 5.40f);
                   glVertex2f(3.30f, 3.80f);

                   glVertex2f(2.40f, 2.77f);
                   glVertex2f(2.40f, 5.03f);

             glEnd();
             glLineWidth(3.0);
     glColor3f(0.7f, 0.4f, 0.0f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(-1.11f, 2.25f);
                glVertex2f(-1.11f, 4.09f);
                glVertex2f(-0.63f, 4.64f);
                glVertex2f(-0.54f, 5.56f);
                glVertex2f(0.39f, 6.06f);
                glVertex2f(0.85f, 6.98f);
                glVertex2f(1.5f, 7.30f);
                glVertex2f(1.8f,5.27f);
                glVertex2f(2.40f, 5.03f);
                glVertex2f(2.40f, 2.77f);
    glEnd();

             glLineWidth(2.0);
         glColor3f(0.9f, 0.5f, 0.2f);
            glBegin(GL_LINES);
           glVertex2f(1.8f,5.27f);
            glVertex2f(1.7f,4.20f);
             glEnd();

              glLineWidth(2.0);
         glColor3f(0.9f, 0.5f, 0.2f);
            glBegin(GL_LINES);
           glVertex2f(-0.63f, 4.64f);
            glVertex2f(-0.50f,3.90f);
             glEnd();

             glLineWidth(2.0);
         glColor3f(0.9f, 0.5f, 0.2f);
            glBegin(GL_LINES);
          glVertex2f(0.39f, 6.06f);
            glVertex2f(0.40f,4.0);
             glEnd();
               glBegin(GL_POLYGON);
            for( int i=0;i<200;i++)
                {
            glColor3f(0.9f,0.5f,0.0f);
            float pi= 3.1416;
            float A =(i*2*pi)/200;
            float r = 0.1;
            float x= 2.40+r* cos(A);
            float y= 3.60+r* sin(A);
            glVertex2f(x,y);
                }
            glEnd();
             glBegin(GL_POLYGON);
            for( int i=0;i<200;i++)
                {
            glColor3f(0.9f,0.5f,0.0f);
            float pi= 3.1416;
            float A =(i*2*pi)/200;
            float r = 0.1;
            float x= 1.40+r* cos(A);
            float y= 7.60+r* sin(A);
            glVertex2f(x,y);
                }
            glEnd();
             glBegin(GL_POLYGON);
            for( int i=0;i<200;i++)
                {
            glColor3f(0.9f,0.5f,0.0f);
            float pi= 3.1416;
            float A =(i*2*pi)/200;
            float r = 0.1;
            float x= 0.20+r* cos(A);
            float y= 4.60+r* sin(A);
            glVertex2f(x,y);
                }
            glEnd();
             glBegin(GL_POLYGON);
            for( int i=0;i<200;i++)
                {
            glColor3f(0.9f,0.5f,0.0f);
            float pi= 3.1416;
            float A =(i*2*pi)/200;
            float r = 0.1;
            float x= 1.70+r* cos(A);
            float y= 4.20+r* sin(A);
            glVertex2f(x,y);
                }
            glEnd();

             glBegin(GL_POLYGON);
            for( int i=0;i<200;i++)
                {
            glColor3f(0.9f,0.5f,0.0f);
            float pi= 3.1416;
            float A =(i*2*pi)/200;
            float r = 0.1;
            float x= 1.20+r* cos(A);
            float y= 5.80+r* sin(A);
            glVertex2f(x,y);
                }
            glEnd();
             //SMALL STONES1
                float dx = 1.0f;
            float dy = 0.0f;
            glColor3f(0.94f, 0.5f, 0.0f);
            glBegin(GL_POLYGON);

            // 1
            for(float t=0.0f; t<=1.0f; t+=0.01f){
                float x = (1-t)*(1-t)*-1.29f + 2*(1-t)*t*-1.26f + t*t*-0.55f;
                float y = (1-t)*(1-t)* 2.36f + 2*(1-t)*t* 3.31f + t*t* 3.95f;
                glVertex2f(x + dx, y - dy);
            }

            // 2
            for(float t=0.0f; t<=1.0f; t+=0.01f){
                float x = (1-t)*(1-t)*-0.55f + 2*(1-t)*t*0.1f + t*t*0.3f;
                float y = (1-t)*(1-t)* 3.95f + 2*(1-t)*t*3.58f + t*t*3.26f;
                glVertex2f(x + dx, y - dy);
            }

            // 3
            for(float t=0.0f; t<=1.0f; t+=0.02f){
                float x = (1-t)*(1-t)*0.3f + 2*(1-t)*t*0.5f + t*t*0.4f;
                float y = (1-t)*(1-t)*3.26f + 2*(1-t)*t*2.80f + t*t*2.30f;
                glVertex2f(x + dx, y - dy);
            }

            // 4
            for(float t=1.0f; t>=0.0f; t-=0.01f){
                float x = (1-t)*(1-t)*0.0f + 2*(1-t)*t*0.1f + t*t*0.4f;
                float y = (1-t)*(1-t)*2.20f + 2*(1-t)*t*2.04f + t*t*2.30f;
                glVertex2f(x + dx, y - dy);
            }

            // 5
            for(float t=1.0f; t>=0.0f; t-=0.01f){
                float x = (1-t)*(1-t)*-1.29f + 2*(1-t)*t*-0.10f + t*t*0.0f;
                float y = (1-t)*(1-t)* 2.36f + 2*(1-t)*t* 2.00f + t*t*2.20f;
                glVertex2f(x + dx, y - dy);
            }

            glEnd();
            // BORDER (Outline)
             dx = 1.0f;
             dy = 0.0f;

            glLineWidth(2.5f);
            glColor3f(0.8f, 0.4f, 0.0f);

            // 1
            glBegin(GL_LINE_STRIP);
            for(float t=0.0f; t<=1.0f; t+=0.01f){
                float x = (1-t)*(1-t)*-1.29f + 2*(1-t)*t*-1.26f + t*t*-0.55f;
                float y = (1-t)*(1-t)* 2.36f + 2*(1-t)*t* 3.31f + t*t* 3.95f;
                glVertex2f(x + dx, y - dy);
            }
            glEnd();

            // 2
            glBegin(GL_LINE_STRIP);
            for(float t=0.0f; t<=1.0f; t+=0.01f){
                float x = (1-t)*(1-t)*-0.55f + 2*(1-t)*t*0.1f + t*t*0.3f;
                float y = (1-t)*(1-t)* 3.95f + 2*(1-t)*t*3.58f + t*t*3.26f;
                glVertex2f(x + dx, y - dy);
            }
            glEnd();

            // 3
            glBegin(GL_LINE_STRIP);
            for(float t=0.0f; t<=1.0f; t+=0.02f){
                float x = (1-t)*(1-t)*0.3f + 2*(1-t)*t*0.5f + t*t*0.4f;
                float y = (1-t)*(1-t)*3.26f + 2*(1-t)*t*2.80f + t*t*2.30f;
                glVertex2f(x + dx, y - dy);
            }
            glEnd();

            // 5
            glBegin(GL_LINE_STRIP);
            for(float t=0.0f; t<=1.0f; t+=0.01f){
                float x = (1-t)*(1-t)*0.0f + 2*(1-t)*t*0.1f + t*t*0.4f;
                float y = (1-t)*(1-t)*2.20f + 2*(1-t)*t*2.04f + t*t*2.30f;
                glVertex2f(x + dx, y - dy);
            }
            glEnd();

            // 4
            glBegin(GL_LINE_STRIP);
            for(float t=0.0f; t<=1.0f; t+=0.01f){
                float x = (1-t)*(1-t)*-1.29f + 2*(1-t)*t*-0.10f + t*t*0.0f;
                float y = (1-t)*(1-t)* 2.36f + 2*(1-t)*t* 2.00f + t*t*2.20f;
                glVertex2f(x + dx, y - dy);
            }
            glEnd();


                // SMALL STONE 2
                glColor3f(0.94f, 0.5f, 0.0f);

                glBegin(GL_POLYGON);



            for(float t=0.0f; t<=1.0f; t+=0.01f){
                float x = (1-t)*(1-t)*-1.29f + 2*(1-t)*t*-1.26f + t*t*-0.55f;
                float y = (1-t)*(1-t)* 2.36f + 2*(1-t)*t* 3.31f + t*t* 3.95f;
                glVertex2f(x,y);
            }


            for(float t=0.0f; t<=1.0f; t+=0.01f){
                float x = (1-t)*(1-t)*-0.55f + 2*(1-t)*t*0.1f + t*t*0.3f;
                float y = (1-t)*(1-t)* 3.95f + 2*(1-t)*t*3.58f + t*t*3.26f;
                glVertex2f(x,y);
            }


            for(float t=0.0f; t<=1.0f; t+=0.02f){
                float x = (1-t)*(1-t)*0.3f + 2*(1-t)*t*0.5f + t*t*0.4f;
                float y = (1-t)*(1-t)*3.26f + 2*(1-t)*t*2.80f + t*t*2.30f;
                glVertex2f(x,y);
            }



            for(float t=1.0f; t>=0.0f; t-=0.01f){
                float x = (1-t)*(1-t)*0.0f + 2*(1-t)*t*0.1f + t*t*0.4f;
                float y = (1-t)*(1-t)*2.20f + 2*(1-t)*t*2.04f + t*t*2.30f;
                glVertex2f(x,y);
            }

            for(float t=1.0f; t>=0.0f; t-=0.01f){
                float x = (1-t)*(1-t)*-1.29f + 2*(1-t)*t*-0.10f + t*t*0.0f;
                float y = (1-t)*(1-t)* 2.36f + 2*(1-t)*t* 2.00f + t*t*2.20f;
                glVertex2f(x,y);
            }

            glEnd();
            //SMALL STONE BORDER
                glLineWidth(3.0);
                glColor3f(0.7f, 0.4f, 0.0f);
                glBegin(GL_LINE_STRIP);

                for(float t = 0.0f; t <= 1.0f; t += 0.01f)
                {
                    float x =(1 - t)*(1 - t)*-1.29f +2*(1 - t)*t*-1.26f +t*t*-0.55f;
                    float y =(1 - t)*(1 - t)*2.36f +2*(1 - t)*t*3.31f +t*t*3.95f;
                    glVertex2f(x, y);
                }
                glEnd();

                 glLineWidth(3.0);
                glColor3f(0.8f, 0.4f, 0.0f);
                glBegin(GL_LINE_STRIP);

                for(float t = 0.0f; t <= 1.0f; t += 0.01f)
                {
                    float x =(1 - t)*(1 - t)*-0.55f +2*(1 - t)*t*0.1f +t*t*0.3f;
                    float y =(1 - t)*(1 - t)*3.95f +2*(1 - t)*t*3.58f +t*t*3.26f;
                    glVertex2f(x, y);
                }

                glEnd();
                 glLineWidth(3.0);
                glColor3f(0.8f, 0.4f, 0.0f);
                glBegin(GL_LINE_STRIP);

                for(float t = 0.0f; t <= 1.0f; t += 0.02f)
                {
                    float x =
                    (1 - t)*(1 - t)*0.3f +2*(1 - t)*t*0.5f +t*t*0.4f;
                    float y =(1 - t)*(1 - t)*3.26f +2*(1 - t)*t*2.80f +t*t*2.30f;
                    glVertex2f(x, y);
                }
                 glEnd();
                 glLineWidth(3.0);
                glColor3f(0.7f, 0.4f, 0.0f);
                glBegin(GL_LINE_STRIP);

                for(float t = 0.0f; t <= 1.0f; t += 0.01f)
                {
                    float x =(1 - t)*(1 - t)*-1.29f +2*(1 - t)*t*-0.10 +t*t*0.0f;
                    float y =(1 - t)*(1 - t)*2.36f +2*(1 - t)*t*2.00f +t*t*2.20f;
                    glVertex2f(x, y);
                }
                glEnd();

                 glLineWidth(3.0);
                glColor3f(0.7f, 0.4f, 0.0f);
                glBegin(GL_LINE_STRIP);

                for(float t = 0.0f; t <= 1.0f; t += 0.01f)
                {
                    float x =(1 - t)*(1 - t)*0.0+2*(1 - t)*t*0.1 +t*t*0.4f;
                    float y =(1 - t)*(1 - t)*2.20f +2*(1 - t)*t*2.04f +t*t*2.30f;
                    glVertex2f(x, y);
                }
                glEnd();

}

void drawStones()
{
    for (int i = 0; i < TOTAL_STONES; i++) {
        float drawX = stoneX[i] + worldX;
        float drawY = stoneY[i];

        glPushMatrix();
        glTranslatef(drawX, drawY, 0.0f);


        glScalef(STONE_SCALE*3.2, STONE_SCALE*3.2, 1.0f);
        glTranslatef(0.0f, -2.25, 0.0f);

        drawStone();
        glPopMatrix();
    }
  }
void checkStones()
{
    float charScreenX = charX + CHAR_HITBOX_OFFSET_X;
    float charScreenY = charY + 0.1f;

    for (int i = 0; i < TOTAL_STONES; i++) {
        float sx = stoneX[i] + worldX;
        float sy = stoneY[i];

        // simple AABB hitbox
        bool hitX = charScreenX < sx + STONE_W &&
                    charScreenX + CHAR_HITBOX_SIZE > sx - STONE_W;

        bool hitY = charScreenY < sy + STONE_H &&
                    charScreenY + CHAR_HITBOX_SIZE > sy;

        if (hitX && hitY) {
            if (!stoneHit[i]) {   // একবারই life কমবে
                takeHit();
                stoneHit[i] = true;
            }
        } else {
            stoneHit[i] = false;
        }
    }
}

void signBoard()
{
    float cx = 0.0f;
    float cy = 0.0f;   // IMPORTANT: center for rotation
    float r = 1.5f;

    // Board fill
    glColor3f(0.9f, 0.75f, 0.45f);
    glBegin(GL_POLYGON);
    for(int i = 0; i <= 360; i++)
    {
        float a = i * 3.1416f / 180.0f;
        glVertex2f(cx + r * cos(a), cy + r * sin(a));
    }
    glEnd();

    // Inner panel
    glColor3f(0.98f, 0.90f, 0.75f);
    glBegin(GL_POLYGON);
    for(int i = 0; i <= 360; i++)
    {
        float a = i * 3.1416f / 180.0f;
        glVertex2f(cx + (r - 0.3f) * cos(a), cy + (r - 0.3f) * sin(a));
    }
    glEnd();

    glLineWidth(3.0f);
    glColor3f(0.3f, 0.15f, 0.05f);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i <= 360; i++)
    {
        float a = i * 3.1416f / 180.0f;
        glVertex2f(cx + r * cos(a), cy + r * sin(a));
    }
    glEnd();

    glColor3f(0.2f, 0.2f, 0.2f);
    for(float x = -r+0.2f; x <= r-0.2f; x += 2*r-0.4f)
    {
        glBegin(GL_POLYGON);
        for(int i = 0; i < 360; i++)
        {
            float a = i * 3.1416f / 180.0f;
            glVertex2f(x + 0.06f * cos(a), cy + 0.05f + 0.06f * sin(a));
        }
        glEnd();
    }
}
void drawRotatingSignBoard(float x, float y, int i)
{
    glPushMatrix();

    glTranslatef(x, y, 0.0f);                 // obstacle position
    glRotatef(signAngle[i], 0.0f, 0.0f, 1.0f); // rotate around center

    signBoard(); // draw the object

    glPopMatrix();
}

// Distance from x to the nearest dorayaki or heart, so hazards are not placed on top of them
float distanceToCollectible(float x)
{
    float best = 1000.0f;
    for (int i = 0; i < TOTAL_CAKES; i++) best = fmin(best, fabs(x - doracakeX[i]));
    for (int i = 0; i < totalHearts; i++) best = fmin(best, fabs(x - heartX[i]));
    return best;
}

void placeManholeFire()
{
    float minX = -7.0f;
    float maxX =  7.0f;

    for (int i = 0; i < TOTAL_FIRES; i++) {
        int attempts = 0;
        do {
            int sceneIndex = rand() % totalScenes;
            float localX = minX + (rand() / (float)RAND_MAX) * (maxX - minX);
            fireX[i] = sceneIndex * sceneWidth + localX;
            attempts++;
        } while (distanceToCollectible(fireX[i]) < HAZARD_CLEARANCE && attempts < 1000);

        fireY[i] = -6.5f;          // ground  manhole position

        fireOn[i] = false;
        fireTimer[i] = 0.0f;
        fireHit[i] = false;
    }
}
void updateManholeFire()
{
    for (int i = 0; i < TOTAL_FIRES; i++) {
        fireTimer[i]++;
        if (fireOn[i]) {
            if (fireTimer[i] > FIRE_ON_TIME) {
                fireOn[i] = false;
                fireTimer[i] = 0.0f;
                fireHit[i] = false;
            }
        }else {
            if (fireTimer[i] > FIRE_OFF_TIME) {
                fireOn[i] = true;
                fireTimer[i] = 0.0f;
            }
        }
    }
}
void drawManholeFire()
{
    for (int i = 0; i < TOTAL_FIRES; i++) {

        float drawX = fireX[i] + worldX;
        float drawY = fireY[i];

        // ---- Manhole (static) ----
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_POLYGON);
        for (int a = 0; a < 360; a++) {
            float rad = a * 3.1416f / 180.0f;
            glVertex2f(drawX + 0.6f * cos(rad), drawY + 0.15f * sin(rad));
        }
        glEnd();

        // ---Fire (only when ON) ----
        if (fireOn[i]) {
            float flameH = 1.2f + 0.2f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.01f);

            glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 0.4f, 0.0f);
            glVertex2f(drawX - 0.3f, drawY);
            glVertex2f(drawX + 0.3f, drawY);
            glColor3f(1.0f, 0.9f, 0.0f);
            glVertex2f(drawX, drawY + flameH);
            glEnd();
        }
    }
}
void checkManholeFire()
{
    float charScreenX = charX + CHAR_HITBOX_OFFSET_X;
    float charScreenY = charY + 0.1f;

    for (int i = 0; i < TOTAL_FIRES; i++) {
        if (!fireOn[i]) {
            fireHit[i] = false;
            continue;
        }

        float fx = fireX[i] + worldX;
        float fy = fireY[i];

        bool hitX = charScreenX < fx + 0.6f &&
                    charScreenX + CHAR_HITBOX_SIZE > fx - 0.6f;

        bool hitY = charScreenY < fy + 1.3f &&
                    charScreenY + CHAR_HITBOX_SIZE >= fy - 0.05f;

        if (hitX && hitY) {
            if (!fireHit[i]) {
                takeHit();
                fireHit[i] = true;
            }
        } else {
            fireHit[i] = false;
        }
    }
}

void drawBall(float x, float y) {
    int numSegments = 40;
float radius = 1.0f;
glBegin(GL_TRIANGLE_FAN);

// --- Center color (darker pink) ---
glColor3f(0.9f, 0.3f, 0.6f);
glVertex2f(x, y);   // center

for (int i = 0; i <= numSegments; i++) {
    float angle = 2.0f * 3.14159f * i / numSegments;
    float px = x + cos(angle) * radius;
    float py = y + sin(angle) * radius;

    // --- Edge color (lighter pink) ---
    glColor3f(1.0f, 0.7f, 0.85f);
    glVertex2f(px, py);
}

glEnd();
}

void drawMouse(float x, float y, float scale = 1.0f) {
    glBegin(GL_LINE_LOOP);
    glLineWidth(3);
    glColor3f(0.0f, 0.0f, 0.0f); // outline
    glVertex2f(x + (-0.4f * scale), y + (0.0f * scale));
    glVertex2f(x + (-0.04f * scale), y + (0.12f * scale));
    glVertex2f(x + (-0.05f * scale), y + (-0.15f * scale));
    glEnd();

    drawCircle(x + 0.06999f * scale, y + -0.02158f * scale, 0.177f * scale, 0.75f, 0.75f, 0.78f); // body

    glBegin(GL_POLYGON);
    glColor3f(0.75f, 0.75f, 0.78f);
    glVertex2f(x + (-0.4f * scale), y + (0.0f * scale));
    glVertex2f(x + (-0.04f * scale), y + (0.12f * scale));
    glVertex2f(x + (-0.05f * scale), y + (-0.15f * scale));
    glEnd();

    drawCircle(x + -0.09946f * scale, y + 0.15048f * scale, 0.112f * scale, 0.75f, 0.75f, 0.78f); // ear
    drawCircle(x + -0.10468f * scale, y + 0.15569f * scale, 0.055f * scale, 0.95f, 0.7f, 0.75f);  // inner ear
    drawCircle(x + -0.4f * scale, y + 0.0f * scale, 0.02f * scale, 0.0f, 0.0f, 0.0f);              // nose
    drawCircle(x + -0.2f * scale, y + 0.02f * scale, 0.02f * scale, 0.0f, 0.0f, 0.0f);            // eye

    // Tail
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(x + 0.24f * scale, y + -0.01f * scale);
        glVertex2f(x + 0.4f * scale, y + 0.06f * scale);
    glEnd();

    // Whiskers
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(x + -0.05f * scale, y + -0.01f * scale); glVertex2f(x + -0.15f * scale, y + -0.01f * scale);
        glVertex2f(x + -0.05f * scale, y + -0.03f * scale); glVertex2f(x + -0.15f * scale, y + -0.03f * scale);
    glEnd();

    glFlush();
}

void drawSpikyFruit(float x, float y, float scale = 1.0f) {
    float radius = 0.2f * scale;
    int spikes = 20;

    drawCircle(x, y, radius, 0.0f, 0.0f, 0.0f); // center

    for (int i = 0; i < spikes; i++) {
        float angle = 2.0f * 3.1415926f * i / spikes;
        float spikeLength = radius * 0.5f;

        glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);

        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
        glVertex2f(x + radius * cos(angle + 0.1f), y + radius * sin(angle + 0.1f));
        glVertex2f(x + (radius + spikeLength) * cos(angle + 0.05f), y + (radius + spikeLength) * sin(angle + 0.05f));

        glEnd();
    }
    glFlush();
}

void placeCollectibles() {
    float minDistance = 4.0f;
    float minX = 0.0f;
    float maxX = 12.0f;

    // Dora cakes
    for (int i = 0; i < 5; i++) {
        bool valid = false;
        while (!valid) {
            int sceneIndex = rand() % totalScenes;
            float localX = minX + (rand() / (float)RAND_MAX) * (maxX - minX);
            float candidateX = sceneIndex * sceneWidth + localX;

            valid = true;
            for (int j = 0; j < i; j++) {
                if (fabs(candidateX - doracakeX[j]) < minDistance) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                doracakeX[i] = candidateX;
                doracakeY[i] = -6.0f;
                doracakeCollected[i] = false;
            }
        }
    }

    // Hearts
    for (int i = 0; i < 3; i++) {
        bool valid = false;
        while (!valid) {
            int sceneIndex = rand() % totalScenes;
            float localX = minX + (rand() / (float)RAND_MAX) * (maxX - minX);
            float candidateX = sceneIndex * sceneWidth + localX;

            valid = true;
            for (int j = 0; j < i; j++) {
                if (fabs(candidateX - heartX[j]) < minDistance) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                heartX[i] = candidateX;
                heartY[i] = -6.0f + 0.5f;
                heartLost[i] = false;
            }
        }
    }

    doracakesCollected = 0;
    lives = totalHearts;
}

void checkCollectibles() {
    float collectRadius = 2.0f;

    float charScreenX = charX + CHAR_HITBOX_OFFSET_X;
    float charScreenY = charY + 0.1f;

    // Dora cakes
    for (int i = 0; i < 5; i++) {
        if (!doracakeCollected[i]) {
            float cakeScreenX = doracakeX[i] + worldX;
            float cakeScreenY = doracakeY[i];

            float dx = charScreenX - cakeScreenX;
            float dy = charScreenY - cakeScreenY;

            if (dx * dx + dy * dy < collectRadius * collectRadius) {
                doracakeCollected[i] = true;
                doracakesCollected++;
                score += CAKE_POINTS;
                spawnParticles(cakeScreenX, cakeScreenY + 0.5f, 30, 1.0f, 0.9f, 0.2f, 0.12f);
                playSound("collect");
            }
        }
    }

    // Hearts
    for (int i = 0; i < 3; i++) {
        if (!heartLost[i]) {
            float heartScreenX = heartX[i] + worldX;
            float heartScreenY = heartY[i];

            float dx = charScreenX - heartScreenX;
            float dy = charScreenY - heartScreenY;

            if (dx * dx + dy * dy < collectRadius * collectRadius) {
                heartLost[i] = true;
                if (lives < totalHearts) {
                    lives++;
                }
                spawnParticles(heartScreenX, heartScreenY + 0.5f, 20, 1.0f, 0.5f, 0.7f, 0.1f);
            }
        }
    }

    if (doracakesCollected >= TOTAL_CAKES) {
        int seconds = levelFrames / 60;
        lastTimeBonus = (seconds < TIME_BONUS_SECONDS) ? (TIME_BONUS_SECONDS - seconds) * TIME_BONUS_PER_SECOND : 0;
        score += lastTimeBonus;
        saveHighScore();
        showEndScreen = true;
        gameWon = true;
        moving = false;
        jumping = false;
        playSound("levelup");
    }

}

void placeObstacles() {
    float minDistance = 6.0f;
    float minX = -3.0f;
    float maxX = 9.0f;

    for (int i = 0; i < TOTAL_OBSTACLES; i++) {
        bool valid = false;
        int attempts = 0;

        while (!valid) {
            int sceneIndex = rand() % totalScenes;
            float localX = minX + (rand() / (float)RAND_MAX) * (maxX - minX);
            float candidateX = sceneIndex * sceneWidth + localX;
            bool slides = (rand() % 3 == 0);
            float range = slides ? OBSTACLE_SLIDE_RANGE : 0.0f;
            attempts++;

            // keep clear of dorayaki and hearts (give up after many tries so placement always finishes)
            valid = attempts > 1000 || distanceToCollectible(candidateX) >= HAZARD_CLEARANCE + range;

            for (int j = 0; j < i && valid; j++) {
                if (fabs(candidateX - obstaclePosX[j]) < minDistance) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                obstaclePosX[i] = candidateX;
                obstaclePosY[i] = -6.0f;

                obstacleLevel[i] = currentLevel;
                obstacleEnabled[i] = true;
                obstacleType[i] = rand() % 3;

                // Horizontal movement (Left–Right), only around its own spot
                if (slides) {
                    obstacleMoveSpeedX[i] = 0.03f;
                    obstacleMoveDirX[i] = 1;
                    obstacleLeftBound[i]  = candidateX - range;
                    obstacleRightBound[i] = candidateX + range;
                } else {
                    obstacleMoveSpeedX[i] = 0.0f;
                    obstacleMoveDirX[i] = 0;
                    obstacleLeftBound[i]  = 0;
                    obstacleRightBound[i] = 0;
                }

                // Vertical movement (Up–Down or falling)
                int verticalType = rand() % 3;
                if (verticalType == 0) {
                    obstacleMoveSpeedY[i] = 0.02f;
                    obstacleMoveDirY[i] = 1;
                    obstacleTopBound[i] = -4.0f;
                    obstacleBottomBound[i] = -7.0f;
                } else if (verticalType == 1) {
                    obstacleMoveSpeedY[i] = 0.04f;
                    obstacleMoveDirY[i] = -1;
                    obstacleTopBound[i] = -1.0f;      // falls from above down to the ground
                    obstacleBottomBound[i] = -6.0f;
                    obstaclePosY[i] = obstacleTopBound[i];
                } else { // static
                    obstacleMoveSpeedY[i] = 0.0f;
                    obstacleMoveDirY[i] = 0;
                    obstacleTopBound[i] = 0;
                    obstacleBottomBound[i] = 0;
                }
                signAngle[i] = 0.0f;
                signRotateSpeed[i] = 3.0f;
            }
        }
    }
}

void updateObstacles() {
    for (int i = 0; i < TOTAL_OBSTACLES; i++) {
        if (!obstacleEnabled[i]) continue;
        if (obstacleLevel[i] != currentLevel) continue;

        obstaclePosX[i] += obstacleMoveSpeedX[i] * obstacleMoveDirX[i];
        if (obstacleMoveSpeedX[i] > 0) {
            if (obstaclePosX[i] >= obstacleRightBound[i])
                obstacleMoveDirX[i] = -1;
            if (obstaclePosX[i] <= obstacleLeftBound[i])
                obstacleMoveDirX[i] = 1;
        }

        obstaclePosY[i] += obstacleMoveSpeedY[i] * obstacleMoveDirY[i];
        if (obstacleMoveSpeedY[i] > 0) {
            if (obstacleMoveDirY[i] == 1 && obstaclePosY[i] >= obstacleTopBound[i])
                obstacleMoveDirY[i] = -1;
            if (obstacleMoveDirY[i] == -1 && obstaclePosY[i] <= obstacleBottomBound[i])
                obstacleMoveDirY[i] = 1;
        }
        signAngle[i] += signRotateSpeed[i];
        if (signAngle[i] >= 360.0f)
            signAngle[i] -= 360.0f;
    }
}

// Half width / half height of an obstacle's drawing, centered on obstaclePos
void getObstacleHalfSize(int i, float& halfW, float& halfH, float& offsetY)
{
    offsetY = 0.0f;
    if (obstacleLevel[i] == 2) {             // rotating sign board, r = 1.5
        halfW = 1.3f; halfH = 1.3f;
    } else if (obstacleType[i] == 0) {       // ball, r = 1.0
        halfW = 0.9f; halfH = 0.9f;
    } else if (obstacleType[i] == 1) {       // mouse at scale 3
        halfW = 1.1f; halfH = 0.6f; offsetY = 0.15f;
    } else {                                 // spiky fruit at scale 3
        halfW = 0.8f; halfH = 0.8f;
    }
}

void checkObstacles() {
    // Doraemon's body box on screen (mirrored when facing left)
    float bodyA = charX + CHAR_HITBOX_OFFSET_X + facing * DORAEMON_SCALE * (DORAEMON_BODY_LEFT - CHAR_HITBOX_OFFSET_X);
    float bodyB = charX + CHAR_HITBOX_OFFSET_X + facing * DORAEMON_SCALE * (DORAEMON_BODY_RIGHT - CHAR_HITBOX_OFFSET_X);
    float bodyLeft = fmin(bodyA, bodyB);
    float bodyRight = fmax(bodyA, bodyB);
    float bodyBottom = charY + DORAEMON_SCALE * DORAEMON_BODY_BOTTOM;
    float bodyTop = charY + DORAEMON_SCALE * DORAEMON_BODY_TOP;

    for (int i = 0; i < TOTAL_OBSTACLES; i++) {
        if (!obstacleEnabled[i]) continue;
        if (obstacleLevel[i] != currentLevel) continue;

        float halfW, halfH, offsetY;
        getObstacleHalfSize(i, halfW, halfH, offsetY);
        float obsX = obstaclePosX[i] + worldX;
        float obsY = obstaclePosY[i] + offsetY;

        bool hitX = bodyLeft < obsX + halfW && bodyRight > obsX - halfW;
        bool hitY = bodyBottom < obsY + halfH && bodyTop > obsY - halfH;

        if (hitX && hitY) {
            if (!obstacleHit[i]) {
                takeHit();
                obstacleHit[i] = true;
            }
        } else {
            obstacleHit[i] = false;
        }
    }
}

// ---------- Take-copter gadget ----------
// Drawn around (0,0): stem going down, blades on top. Model units.
void drawTakeCopter(float angle)
{
    // blade width changes with the angle so it looks like it is spinning
    float bladeW = 0.75f * fabs(cos(angle * 3.1416f / 180.0f)) + 0.12f;

    glColor3f(0.95f, 0.75f, 0.1f);   // stem
    glBegin(GL_QUADS);
        glVertex2f(-0.07f, -0.5f); glVertex2f(0.07f, -0.5f);
        glVertex2f(0.07f, 0.05f);  glVertex2f(-0.07f, 0.05f);
    glEnd();

    glColor3f(0.95f, 0.75f, 0.1f);   // blades
    glBegin(GL_QUADS);
        glVertex2f(-bladeW, 0.05f); glVertex2f(bladeW, 0.05f);
        glVertex2f(bladeW, 0.2f);   glVertex2f(-bladeW, 0.2f);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-bladeW, 0.05f); glVertex2f(bladeW, 0.05f);
        glVertex2f(bladeW, 0.2f);   glVertex2f(-bladeW, 0.2f);
    glEnd();

    glColor3f(0.85f, 0.2f, 0.2f);    // hub
    glBegin(GL_POLYGON);
    for (int i = 0; i < 20; i++) {
        float a = 2.0f * 3.1416f * i / 20;
        glVertex2f(0.12f * cos(a), 0.12f + 0.12f * sin(a));
    }
    glEnd();

    glColor3f(0.95f, 0.75f, 0.1f);   // base that sits on the head
    glBegin(GL_POLYGON);
    for (int i = 0; i < 20; i++) {
        float a = 3.1416f * i / 19;
        glVertex2f(0.25f * cos(a), -0.55f + 0.15f * sin(a));
    }
    glEnd();
}

void placeGadget()
{
    int sceneIndex = 1 + rand() % (totalScenes - 1);
    copterX = sceneIndex * sceneWidth + 2.0f + (rand() / (float)RAND_MAX) * 8.0f;
    copterY = -6.0f;
    copterPicked = false;
    copterTimer = 0;
}

void checkGadget()
{
    if (copterPicked) return;
    float dx = (charX + CHAR_HITBOX_OFFSET_X + CHAR_HITBOX_SIZE / 2) - (copterX + worldX);
    float dy = (charY + CHAR_HITBOX_SIZE / 2) - copterY;
    if (dx * dx + dy * dy < 1.5f * 1.5f) {
        copterPicked = true;
        copterTimer = COPTER_FRAMES;
        jumping = true;
        spawnParticles(copterX + worldX, copterY + 0.5f, 30, 0.3f, 0.8f, 1.0f, 0.12f);
        playSound("gadget");
    }
}

void drawGadgetItem()
{
    if (copterPicked) return;
    float bob = 0.2f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.004f);
    glPushMatrix();
    glTranslatef(copterX + worldX, copterY + 1.0f + bob, 0.0f);
    glScalef(1.4f, 1.4f, 1.0f);
    drawTakeCopter(propellerAngle);
    glPopMatrix();

    // label so players know what the item is
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.95f, 0.4f);
    drawTextCentered(copterX + worldX, copterY + 1.6f + bob, "Take-copter", GLUT_BITMAP_HELVETICA_18);
    glPopAttrib();
}

// ---------- Mouse enemy ----------
void placeMouseEnemy()
{
    int sceneIndex = 1 + rand() % (totalScenes - 1);
    float center = sceneIndex * sceneWidth + 4.0f;
    mouseLeftBound = center - 5.0f;
    mouseRightBound = center + 5.0f;
    mouseX = center;
    mouseY = -6.8f;
    mouseDir = -1;
    scaredTimer = 0;
    scareCooldown = 0;
}

void updateMouseEnemy()
{
    mouseX += MOUSE_SPEED * mouseDir;
    if (mouseX <= mouseLeftBound) mouseDir = 1;
    if (mouseX >= mouseRightBound) mouseDir = -1;

    if (scareCooldown > 0) scareCooldown--;
    if (scaredTimer > 0 || scareCooldown > 0) return;

    float charCenterX = charX + CHAR_HITBOX_OFFSET_X + CHAR_HITBOX_SIZE / 2;
    float mouseScreenX = mouseX + worldX;
    bool nearGround = charY < GROUND_Y + 2.0f;   // jumping high over the mouse is safe

    if (nearGround && fabs(charCenterX - mouseScreenX) < MOUSE_SCARE_DIST) {
        scaredTimer = SCARED_FRAMES;
        scareCooldown = SCARED_FRAMES + SCARE_COOLDOWN_FRAMES;
        scaredDir = (charCenterX < mouseScreenX) ? -1 : 1;
        playSound("scared");
    }
}

void drawMouseEnemy()
{
    glPushMatrix();
    glTranslatef(mouseX + worldX, mouseY, 0.0f);
    glScalef((float)-mouseDir, 1.0f, 1.0f);   // drawMouse() faces left
    drawMouse(0.0f, 0.0f, 2.5f);
    glPopMatrix();
}

// ---------- Level setup ----------
// Puts character, world, collectibles and every hazard back to the start of currentLevel.
void resetLevel()
{
    charX = CHAR_START_X;
    charY = GROUND_Y;
    velY = 0.0f;
    jumpsUsed = 0;
    moving = false;
    jumping = false;
    leftHeld = rightHeld = upHeld = false;
    facing = 1;
    legAngle = 0.0f;
    worldX = 0.0f;
    sceneCount = 0;

    showEndScreen = false;
    gameWon = false;
    paused = false;
    invincibleTimer = 0;
    hitFlash = 0.0f;
    levelFrames = 0;
    lastTimeBonus = 0;

    placeCollectibles();

    for (int i = 0; i < TOTAL_OBSTACLES; i++) {
        obstacleEnabled[i] = false;
        obstacleHit[i] = false;
    }
    if (currentLevel == 1) {
        placeStones();
    } else {
        placeObstacles();
        if (currentLevel == 2) placeManholeFire();
    }

    placeGadget();
    placeMouseEnemy();
    clearParticles();
}

void startLevel(int level)
{
    currentLevel = level;
    showHome = false;
    levelStartScore = score;
    resetLevel();
}

void restartLevel()
{
    score = levelStartScore;
    resetLevel();
}

void goToMainMenu()
{
    saveHighScore();
    score = 0;
    showEndScreen = false;
    paused = false;
    showHome = true;
}

// ---------- Player ----------
void drawPlayer()
{
    bool blinkHidden = invincibleTimer > 0 && (invincibleTimer / 4) % 2 == 0;
    if (!blinkHidden) {
        glPushMatrix();
        glTranslatef(charX, charY, 0.0f);
        glTranslatef(CHAR_HITBOX_OFFSET_X, 0.0f, 0.0f);
        glScalef((float)facing * DORAEMON_SCALE, DORAEMON_SCALE, 1.0f);
        glTranslatef(-CHAR_HITBOX_OFFSET_X, 0.0f, 0.0f);
        Doraemon();
        if (copterTimer > 0) {
            glTranslatef(DORAEMON_HEAD_X, DORAEMON_HEAD_TOP + 1.0f, 0.0f);
            glScalef(1.7f, 1.7f, 1.0f);
            drawTakeCopter(propellerAngle);
        }
        glPopMatrix();
    }

    if (scaredTimer > 0) {
        float headX = charX + CHAR_HITBOX_OFFSET_X + facing * DORAEMON_SCALE * (DORAEMON_HEAD_X - CHAR_HITBOX_OFFSET_X);
        float headY = charY + DORAEMON_SCALE * DORAEMON_HEAD_TOP;
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        float shake = 0.08f * sin(scaredTimer * 1.5f);
        glColor3f(1.0f, 0.1f, 0.1f);
        drawTextCentered(headX + shake, headY + 0.4f, "!!", GLUT_BITMAP_TIMES_ROMAN_24);
        glPopAttrib();
    }
}

// ---------- HUD / overlays ----------
float levelProgress()
{
    float levelLength = totalScenes * sceneWidth + (MAX_CHAR_X - CHAR_START_X);
    float p = ((charX - CHAR_START_X) - worldX) / levelLength;
    if (p < 0.0f) p = 0.0f;
    if (p > 1.0f) p = 1.0f;
    return p;
}

void drawHUD() {
    char buffer[96];

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f);
    sprintf(buffer, "Cakes: %d / %d", doracakesCollected, TOTAL_CAKES);
    drawText(-9.2f, 8.5f, buffer);

    sprintf(buffer, "Level %d", currentLevel);
    drawText(-0.8f, 9.2f, buffer);
    sprintf(buffer, "Time: %d s", levelFrames / 60);
    drawText(-0.9f, 8.5f, buffer);

    sprintf(buffer, "Score: %d", score);
    drawText(5.5f, 9.2f, buffer);
    sprintf(buffer, "High: %d", highScore > score ? highScore : score);
    drawText(5.5f, 8.5f, buffer);

    if (copterTimer > 0) {
        glColor3f(1.0f, 0.85f, 0.2f);
        sprintf(buffer, "Flying!  Hold UP to rise, release to glide down  (%d s)", copterTimer / 60 + 1);
        drawTextCentered(0.0f, 6.9f, buffer, GLUT_BITMAP_HELVETICA_18);
    }

    // Progress bar
    float barX = -3.0f, barY = 7.8f, barW = 6.0f, barH = 0.25f;
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(barX, barY); glVertex2f(barX + barW, barY);
        glVertex2f(barX + barW, barY + barH); glVertex2f(barX, barY + barH);
    glEnd();
    glColor3f(0.3f, 0.8f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(barX, barY); glVertex2f(barX + barW * levelProgress(), barY);
        glVertex2f(barX + barW * levelProgress(), barY + barH); glVertex2f(barX, barY + barH);
    glEnd();

    glPopAttrib();

    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(-8.9f + i * 0.8f, 7.5f, 0.0f);
        glScalef(0.7f, 0.7f, 1.0f);

        if (i < lives) {
            glColor3f(0.99f, 0.85f, 0.88f);
            Heart();
        }

        glPopMatrix();
    }
}

void drawFullScreenQuad(float r, float g, float b, float a)
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
        glVertex2f(SCREEN_LEFT, SCREEN_BOTTOM); glVertex2f(SCREEN_RIGHT, SCREEN_BOTTOM);
        glVertex2f(SCREEN_RIGHT, SCREEN_TOP);   glVertex2f(SCREEN_LEFT, SCREEN_TOP);
    glEnd();
    glPopAttrib();
}

// Everything drawn on top of a level: particles, hit flash, HUD, pause and end screen
void drawOverlays()
{
    drawParticles();
    if (hitFlash > 0.0f) drawFullScreenQuad(1.0f, 0.0f, 0.0f, hitFlash);
    drawHUD();

    if (paused && !showEndScreen) {
        drawFullScreenQuad(0.0f, 0.0f, 0.0f, 0.4f);
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);

        const float panelL = -3.6f, panelR = 3.6f, panelB = -1.4f, panelT = 1.6f;
        glColor4f(0.05f, 0.05f, 0.12f, 0.94f);
        glBegin(GL_QUADS);
            glVertex2f(panelL, panelB); glVertex2f(panelR, panelB);
            glVertex2f(panelR, panelT); glVertex2f(panelL, panelT);
        glEnd();
        glColor3f(1.0f, 0.85f, 0.2f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(panelL, panelB); glVertex2f(panelR, panelB);
            glVertex2f(panelR, panelT); glVertex2f(panelL, panelT);
        glEnd();

        drawTextCentered(0.0f, 0.6f, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawTextCentered(0.0f, -0.2f, "P : Resume", GLUT_BITMAP_HELVETICA_18);
        glColor3f(0.75f, 0.75f, 0.85f);
        drawTextCentered(0.0f, -0.9f, "Esc : Main Menu", GLUT_BITMAP_HELVETICA_18);
        glPopAttrib();
    }

    if (showEndScreen) {
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        drawEndScreen();
        glPopAttrib();
    }
}

void drawLighting2()
{
      //LIGHTING

         glEnable(GL_DEPTH_TEST);

        glEnable(GL_LIGHTING);
        glDisable(GL_LIGHT0);   // Level 1 light, otherwise both lights add up after switching levels
        glEnable(GL_LIGHT1);
        glEnable(GL_COLOR_MATERIAL);

        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        glShadeModel(GL_SMOOTH);

        GLfloat lightPos[]     = { 8.0f, 14.0f, 8.0f, 1.0f };
        GLfloat lightAmbient[] = { 0.10f, 0.08f, 0.07f, 1.0f };
        GLfloat lightDiffuse[] = { 0.60f, 0.45f, 0.30f, 1.0f };

        glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT1, GL_AMBIENT,  lightAmbient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE,  lightDiffuse);
}

void drawLighting1()
{
    glEnable(GL_LIGHTING);
    glDisable(GL_LIGHT1);   // Level 2 light, otherwise both lights add up after switching levels
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    GLfloat lightPos[]     = { 0.30f, 0.30f, 0.30f, 0.0f };   // sun position
    GLfloat lightAmbient[] = { 0.55f, 0.55f, 0.55f, 1.0f };   // day ambient
    GLfloat lightDiffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };  // bright sunlight

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

void Level1()
{
    currentLevel = 1;
    drawLighting1();
     glPushMatrix();
       glTranslatef(0.0f, -2.5f, 0.0f);
       glScalef(1.8f, 1.8f, 2.0f);
       drawSky1();
       glPopMatrix();


       for (int i = 0; i <= totalScenes; i++)
       {
           glPushMatrix();
           glTranslatef(worldX + i * sceneWidth, 0.0f, 0.0f);

           if (i % 2 == 0)
               houseScene();
           else
               wallScene();

           glPopMatrix();
       }

    drawPlayer();

       drawGadgetItem();
       drawMouseEnemy();

       for (int i = 0; i < 5; i++) {
           if (!doracakeCollected[i]) {
               glPushMatrix();
               glTranslatef(doracakeX[i] + worldX, doracakeY[i], 0.0f);
               glScalef(0.5f,1.2f, 0.0f);
               Doracake();
               glPopMatrix();
           }
       }

       // Hearts
       for (int i = 0; i < 3; i++) {
           if (!heartLost[i]) {
               glPushMatrix();
               glTranslatef(heartX[i] + worldX, heartY[i], 0.0f);
               glScalef(1.5f,1.2f, 0.0f);
               Heart();
               glPopMatrix();
           }
       }

       drawStones();
       drawOverlays();

       glutSwapBuffers();

}

void Level2()
{
    currentLevel = 2;
    drawLighting2();
    glPushMatrix();
    glTranslatef(-9.5f, -17.2f, 0.0f);
    glScalef(1.2f, 1.5f, 1.0f);
    glNormal3f(0.0f, 0.0f, 1.0f);
    drawSky2();
    glPopMatrix();


    for (int i = 0; i <= totalScenes; i++)
    {
        glPushMatrix();
        glTranslatef(worldX + i * sceneWidth, 0.0f, 0.0f);

        if (i % 2 == 0)
           houseScene();
            else
            wallScene();

            glPopMatrix();
        }

    drawPlayer();

    drawGadgetItem();
    drawMouseEnemy();

    for (int i = 0; i < TOTAL_OBSTACLES; i++) {
        if (!obstacleEnabled[i]) continue;
        if (obstacleLevel[i] != currentLevel) continue;

        float drawX = obstaclePosX[i] + worldX;
        float drawY = obstaclePosY[i];

        drawRotatingSignBoard(drawX, drawY, i);

    }


        for (int i = 0; i < 5; i++) {
            if (!doracakeCollected[i]) {
                glPushMatrix();
                glTranslatef(doracakeX[i] + worldX, doracakeY[i], 0.0f);
                glScalef(0.5f,1.2f, 0.0f);
                Doracake();
                glPopMatrix();
            }
        }

        // Hearts
        for (int i = 0; i < 3; i++) {
            if (!heartLost[i]) {
                glPushMatrix();
                glTranslatef(heartX[i] + worldX, heartY[i], 0.0f);
                glScalef(1.5f,1.2f, 0.0f);
                Heart();
                glPopMatrix();
            }
        }


        drawManholeFire();
        drawOverlays();

        glutSwapBuffers();

}

void Level3()
{
    currentLevel = 3;
    glPushAttrib(GL_LIGHTING_BIT);
        glDisable(GL_LIGHTING);
       glPushMatrix();
        glTranslatef(-9.5f, -17.2f, 0.0f);
        glScalef(1.2f, 1.5f, 1.0f);
        drawSky3();
        glPopMatrix();

        for (int i = 0; i <= totalScenes; i++)
        {
            glPushMatrix();
            glTranslatef(worldX + i * sceneWidth, 0.0f, 0.0f);

            if (i % 2 == 0)
                houseScene();
            else
                wallScene();

            glPopMatrix();
        }

    drawPlayer();

        drawGadgetItem();
        drawMouseEnemy();
        for (int i = 0; i < TOTAL_OBSTACLES; i++) {
    if (!obstacleEnabled[i]) continue;
    if (obstacleLevel[i] != currentLevel) continue;

    float drawX = obstaclePosX[i] + worldX;
    float drawY = obstaclePosY[i];

    // Draw based on obstacle type
    if (obstacleType[i] == 0) {
        drawBall(drawX, drawY);           // Ball
    }
    else if (obstacleType[i] == 1) {
        drawMouse(drawX, drawY, 3.0f);    // Mouse
    }
    else if (obstacleType[i] == 2) {
        drawSpikyFruit(drawX, drawY, 3.0f); // Spiky Fruit
    }
}


        for (int i = 0; i < 5; i++) {
            if (!doracakeCollected[i]) {
                glPushMatrix();
                glTranslatef(doracakeX[i] + worldX, doracakeY[i], 0.0f);
                glScalef(0.5f,1.2f, 0.0f);
                Doracake();
                glPopMatrix();
            }
        }

        // Hearts
        for (int i = 0; i < 3; i++) {
            if (!heartLost[i]) {
                glPushMatrix();
                glTranslatef(heartX[i] + worldX, heartY[i], 0.0f);
                glScalef(1.5f,1.2f, 0.0f);
                Heart();
                glPopMatrix();
            }
        }

        glPushMatrix();
        glColor4f(0.0f, 0.0f, 0.2f, 0.5f);
        glBegin(GL_QUADS);
        glVertex2f(-9.5f, -10.0f);
        glVertex2f(9.5f, -10.0f);
        glVertex2f(9.5f, 10.0f);
        glVertex2f(-9.5f, 10.0f);
        glEnd();
        glPopMatrix();

        for (int i = 0; i <= totalScenes; i++) {
        glPushMatrix();
        glTranslatef(worldX + i * sceneWidth, 0.0f, 0.0f);
        if (i % 2 == 0)
        {
            glTranslatef(3.5f, -4.2f, 0.0f);
            glScalef(8.2f, 6.5f, -1.0f);
            drawLampPost();
        } else {
            glTranslatef(3.5f, -4.2f, 0.0f);
            glScalef(8.2f, 8.5f, 1.0f);
            drawLampPost();
        }
        glPopMatrix();
        }

        drawOverlays();
        glPopAttrib();

        glutSwapBuffers();

}


void updateMovementFromKeys() {
    if (scaredTimer > 0) return;          // running away from the mouse, ignore input
    if (rightHeld && !leftHeld) facing = 1;
    if (leftHeld && !rightHeld) facing = -1;
    moving = leftHeld || rightHeld;
}

void tryJump() {
    if (copterTimer > 0) return;          // flying: UP lifts instead
    if (jumpsUsed >= MAX_JUMPS) return;
    velY = (jumpsUsed == 0) ? JUMP_VELOCITY : DOUBLE_JUMP_VELOCITY;
    jumping = true;
    jumpsUsed++;
}

void specialKeyPress(int key, int x, int y) {
    if (showHome || showEndScreen) return;
    switch(key) {
        case GLUT_KEY_RIGHT:
            rightHeld = true;
            break;
        case GLUT_KEY_LEFT:
            leftHeld = true;
            break;
        case GLUT_KEY_UP:
            upHeld = true;
            if (!paused) tryJump();
            break;
    }
    updateMovementFromKeys();
}

void specialKeyRelease(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_RIGHT:
            rightHeld = false;
            break;
        case GLUT_KEY_LEFT:
            leftHeld = false;
            break;
        case GLUT_KEY_UP:
            upHeld = false;
            break;
    }
    if (!showHome && !showEndScreen) updateMovementFromKeys();
}

void goToNextLevel() {
    if (currentLevel < totalLevels) startLevel(currentLevel + 1);
}

void keyPress(unsigned char key, int x, int y) {
    const unsigned char KEY_ENTER = 13, KEY_ESC = 27;

    if (showHome) {
        if (key == '1' || key == KEY_ENTER) { score = 0; startLevel(1); }
        else if (key == '2') { score = 0; startLevel(2); }
        else if (key == '3') { score = 0; startLevel(3); }
        else if (key == KEY_ESC) exit(0);
        return;
    }

    if (showEndScreen) {
        if (key == KEY_ENTER) {
            if (gameWon && currentLevel < totalLevels) goToNextLevel();
            else restartLevel();
        }
        else if (key == 'r' || key == 'R') restartLevel();
        else if (key == KEY_ESC) goToMainMenu();
        return;
    }

    if (key == 'p' || key == 'P') paused = !paused;
    else if (key == KEY_ESC) goToMainMenu();
}


void mouseClick(int button, int state, int x, int y)
{
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);

    // window pixels -> gluOrtho2D(-9.5, 9.5, -10, 10) coordinates
    float fx = ((float)x / winW) * (SCREEN_RIGHT - SCREEN_LEFT) + SCREEN_LEFT;
    float fy = SCREEN_TOP - ((float)y / winH) * (SCREEN_TOP - SCREEN_BOTTOM);

    if (showHome)
    {
        for (int i = 0; i < 3; i++) {
            if (fx >= HOME_BTN_X && fx <= HOME_BTN_X + HOME_BTN_W &&
                fy <= HOME_BTN_TOP[i] && fy >= HOME_BTN_TOP[i] - HOME_BTN_H) {
                score = 0;
                startLevel(i + 1);
                return;
            }
        }
    }

    if (showEndScreen)
    {
        // Next Level
        if (gameWon && currentLevel < totalLevels &&
            fx >= nextLevelX && fx <= nextLevelX + buttonWidth &&
            fy >= nextLevelY && fy <= nextLevelY + buttonHeight)
        {
            goToNextLevel();
            return;
        }

        // Main Menu
        if (fx >= mainMenuX && fx <= mainMenuX + buttonWidth &&
            fy >= mainMenuY && fy <= mainMenuY + buttonHeight)
        {
            goToMainMenu();
            return;
        }

        if (fx >= restartX && fx <= restartX + buttonWidth &&
            fy >= restartY && fy <= restartY + buttonHeight)
        {
            restartLevel();
            return;
        }
    }
}
void update(int value)
{
    // Nothing moves on the menu, end screen or while paused
    if (showHome || showEndScreen || paused) {
        glutPostRedisplay();
        glutTimerFunc(16, update, 0);
        return;
    }

    levelFrames++;
    if (invincibleTimer > 0) invincibleTimer--;
    if (hitFlash > 0.0f) hitFlash -= 0.03f;
    propellerAngle += 25.0f;
    if (propellerAngle >= 360.0f) propellerAngle -= 360.0f;

    // Scared of the mouse: run the other way for a while
    if (scaredTimer > 0) {
        scaredTimer--;
        moving = true;
        facing = scaredDir;
        if (scaredTimer == 0) updateMovementFromKeys();
    }

    if(moving && jumping)
    {
        legAngle = 0.0f;
    }

    if (moving)
    {
        legAngle += legDir * _legSwingSpeed;
        if (legAngle > _maxLegAngle || legAngle < -_maxLegAngle)
            legDir *= -1;
    }
    else
    {
        if (legAngle > 0.0f)
            legAngle -= _legResetSpeed;
        else if (legAngle < 0.0f)
            legAngle += _legResetSpeed;

        if (legAngle < 1.0f && legAngle > -1.0f)
            legAngle = 0.0f;
    }

    // Vertical movement: Take-copter flight or gravity
    if (copterTimer > 0)
    {
        copterTimer--;
        velY = upHeld ? COPTER_LIFT : -COPTER_SINK;
        charY += velY;
        if (charY > COPTER_MAX_Y) charY = COPTER_MAX_Y;
        if (copterTimer == 0) {
            velY = 0.0f;
            jumping = true;
            jumpsUsed = MAX_JUMPS;   // fall down, no mid-air jump
        }
    }
    else if (jumping || charY > GROUND_Y)
    {
        velY -= GRAVITY;
        charY += velY;
    }

    if (charY <= GROUND_Y)
    {
        charY = GROUND_Y;
        velY = 0.0f;
        jumping = false;
        jumpsUsed = 0;
    }

    float moveSpeed = (scaredTimer > 0) ? _moveSpeed * SCARED_SPEED_FACTOR : _moveSpeed;
    if (moving)
    {
        if (facing == 1)
        {
            if (sceneCount < totalScenes && charX >= SCROLL_RIGHT_X)
            {
                charX = SCROLL_RIGHT_X;
                worldX -= moveSpeed;

                if (worldX <= -(sceneCount + 1) * sceneWidth)
                    sceneCount++;
            }
            else
                charX += moveSpeed;
        }

        else if (facing == -1)
        {
            if (sceneCount > 0 && charX <= SCROLL_LEFT_X)
            {
                charX = SCROLL_LEFT_X;
                worldX += moveSpeed;

                if (worldX >= -(sceneCount - 1) * sceneWidth)
                    sceneCount--;
            }
             else
                charX -= moveSpeed;
        }
    }

    if (sceneCount >= totalScenes)
    {
        if (charX > MAX_CHAR_X) charX = MAX_CHAR_X;
    }
    if (sceneCount <= 0)
    {
        if (charX < MIN_CHAR_X) charX = MIN_CHAR_X;
    }

    updateParticles();
    updateMouseEnemy();
    checkCollectibles();
    checkGadget();

    // Only check the hazards that belong to this level
    if (currentLevel == 1) {
        checkStones();
    } else {
        updateObstacles();
        checkObstacles();
        if (currentLevel == 2) {
            updateManholeFire();
            checkManholeFire();
        }
    }

    if (lives <= 0 && !showEndScreen) {
        showEndScreen = true;
        gameWon = false;
        moving = false;
        jumping = false;
        saveHighScore();
        playSound("gameover");
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}


void display() {
    if (showHome) {
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    } else {
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    }
    //printf("showHome=%d  currentLevel=%d\n", showHome, currentLevel);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

     if (showHome) {
        homeScreen();
    }
    else {
        if (currentLevel == 1) Level1();
        else if (currentLevel == 2) Level2();
        else if (currentLevel == 3) Level3();
    }



}


void init()
{
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.8, 0.8, 0.8, 1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-9.50, 9.50, -10.0, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1500, 1000);
    glutCreateWindow("Doraemon's Treat Hunt");
    init();
    srand((unsigned)time(NULL));
    loadHighScore();
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeyPress);
    glutSpecialUpFunc(specialKeyRelease);
    glutKeyboardFunc(keyPress);
    glutIgnoreKeyRepeat(1);
    glutMouseFunc(mouseClick);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}


