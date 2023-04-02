#include <graphx.h>
#include <keypadc.h>
#include <stdbool.h>
#include <sys/util.h>

int goalTop = 90;
int goalBottom = 150;

typedef struct
{
    int x;
    int y;
} v2;

typedef struct 
{
    v2 pos;
    int width;
    int height;
    int vel;
    int score;
} paddle_t;

typedef struct
{
    v2 pos;
    int size;
    v2 vel;
} ball_t;

kb_key_t key;

// Init
void initPaddles(paddle_t *paddle1, paddle_t *paddle2);
void initBall(ball_t *ball);

// Update
void handleMovement(paddle_t *paddle1, paddle_t *paddle2, ball_t *ball);
bool handleCollisions(paddle_t *paddle, ball_t *ball);

// Draw
void drawFrame(paddle_t *paddle1, paddle_t *paddle2, ball_t *ball);

int main(void)
{
    static paddle_t l_paddle;
    static paddle_t r_paddle;
    static ball_t ball;

    initPaddles(&l_paddle, &r_paddle);
    initBall(&ball);

    gfx_Begin();
    do {
        kb_Scan();
        gfx_SetDrawBuffer();
        gfx_ZeroScreen();

        // Update
        handleMovement(&l_paddle, &r_paddle, &ball);

        // Draw
        drawFrame(&l_paddle, &r_paddle, &ball);
        gfx_SwapDraw();

    } while (kb_Data[1] != kb_2nd);
    gfx_End();

    return 0;
}

void initPaddles(paddle_t *paddle1, paddle_t *paddle2)
{
    paddle1->pos.x = 30;
    paddle1->pos.y = GFX_LCD_HEIGHT / 2 - 10;
    paddle1->width = 5;
    paddle1->height = 30;
    paddle1->vel = 2;
    paddle1->score = 0;

    paddle2->pos.x = GFX_LCD_WIDTH - 30;
    paddle2->pos.y = GFX_LCD_HEIGHT / 2 - 10;
    paddle2->width = 5;
    paddle2->height = 30;
    paddle2->vel = 2;
    paddle2->score = 0;
}

void initBall(ball_t *ball)
{
    ball->pos.x = GFX_LCD_WIDTH / 2;
    ball->pos.y = randInt(1, GFX_LCD_HEIGHT / 2);
    ball->size = 5;
    ball->vel.x = 2;
    ball->vel.y = randInt(1, 3);
}

void handleMovement(paddle_t *paddle1, paddle_t *paddle2, ball_t *ball)
{
    // Left Paddle
    if ((kb_Data[7] & kb_Up) && (paddle1->pos.y > 0))
    {
        paddle1->pos.y -= paddle1->vel;
    }
    else if ((kb_Data[7] & kb_Down) && (paddle1->pos.y + paddle1->height < GFX_LCD_HEIGHT))
    {
        paddle1->pos.y += paddle1->vel;
    }

    // Ball Movement
    ball->pos.x += ball->vel.x;
    ball->pos.y += ball->vel.y;

    // Collisions with Paddles/Side Walls
    bool leftHit = gfx_CheckRectangleHotspot(paddle1->pos.x, paddle1->pos.y, paddle1->width, paddle1->height, ball->pos.x, ball->pos.y, ball->size, ball->size);
    bool rightHit = gfx_CheckRectangleHotspot(paddle2->pos.x, paddle2->pos.y, paddle2->width, paddle2->height, ball->pos.x, ball->pos.y, ball->size, ball->size);
    bool hitLeftWall = ball->pos.x < 0 && (ball->pos.y < goalTop || ball->pos.y + ball->size > goalBottom);
    bool hitRightWall = ball->pos.x + ball->size > GFX_LCD_WIDTH && (ball->pos.y < goalTop || ball->pos.y + ball->size > goalBottom);
    
    if (leftHit || rightHit || hitLeftWall || hitRightWall)
    {
        ball->vel.x *= -1;
    }

    // Collisions with Top/Bottom Walls
    if (ball->pos.y < 0 || ball->pos.y + ball->size > GFX_LCD_HEIGHT)
    {
        ball->vel.y *= -1;
    }

    // Points Scored (Ball is in Goal)
    if (ball->pos.x < 0 && !hitLeftWall) {
        paddle2->score++;
        ball->pos.x = GFX_LCD_WIDTH / 2;
        ball->pos.y = randInt(1, GFX_LCD_HEIGHT / 2);
        ball->vel.x *= -1;
    }

    if (ball->pos.x + ball->size > GFX_LCD_WIDTH && !hitRightWall) {
        paddle1->score++;
        ball->pos.x = GFX_LCD_WIDTH / 2;
        ball->pos.y = randInt(1, GFX_LCD_HEIGHT / 2);
        ball->vel.x *= -1;
    }

    // Right Paddle
    paddle2->pos.y += paddle2->vel;

    if (paddle2->pos.y < goalTop / 2 || paddle2->pos.y + paddle2->height > goalBottom + (GFX_LCD_HEIGHT - goalBottom) / 2)
    {
        paddle2->vel *= -1;
    }
    
}

void drawFrame(paddle_t *paddle1, paddle_t *paddle2, ball_t *ball) 
{
    // Set to White
    gfx_SetColor(255);

    // Draw Background
    gfx_HorizLine(0, 0, GFX_LCD_WIDTH);
    gfx_HorizLine(0, GFX_LCD_HEIGHT - 1, GFX_LCD_WIDTH);
    gfx_VertLine(GFX_LCD_WIDTH / 2, 0, GFX_LCD_HEIGHT);

    // Draw Goals
    gfx_VertLine(0, 0, goalTop);
    gfx_VertLine(0, goalBottom, GFX_LCD_HEIGHT);
    gfx_VertLine(GFX_LCD_WIDTH - 1, 0, goalTop);
    gfx_VertLine(GFX_LCD_WIDTH - 1, goalBottom, GFX_LCD_HEIGHT);

    // Set Text To White
    gfx_SetTextBGColor(0);
    gfx_SetTextFGColor(255);
    gfx_SetTextTransparentColor(0);

    // Draw Score
    gfx_SetTextXY(100, 25);
    gfx_PrintInt(paddle1->score, 1);
    gfx_SetTextXY(GFX_LCD_WIDTH - 100, 25);
    gfx_PrintInt(paddle2->score, 1);

    // Draw Paddles
    gfx_FillRectangle(paddle1->pos.x, paddle1->pos.y, paddle1->width, paddle1->height);
    gfx_FillRectangle(paddle2->pos.x, paddle2->pos.y, paddle2->width, paddle2->height);

    // Draw Ball
    gfx_FillRectangle(ball->pos.x, ball->pos.y, ball->size, ball->size);
}