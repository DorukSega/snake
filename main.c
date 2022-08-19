#include <stdio.h>

#include "raylib.h"

enum Direction { left, right, top, down };

const int screenWidth = 760;
const int screenHeight = 760;
const int cellSize = 40;
const int snakeSize = 38;
float moveInterval = .3;
float nextMove = .3;

struct Part {
    struct Part* next;
    int x, y;  // x and y
    int dir;
};

struct {
    int size;
    struct Part* head;
} Snake;

struct {
    int x, y;  // x and y
} Food;

int partCalc(int cell) {
    return cellSize * cell + ((cellSize - snakeSize) / 2);
}

void recursiveErase(struct Part* node) {
    if (node == NULL)
        return;
    recursiveErase(node->next);
    MemFree(node);
}

void reset() {
    recursiveErase(Snake.head);
    Snake.head = MemAlloc(sizeof(struct Part));
    Snake.head->x = Snake.head->y = 9;
    Snake.head->dir = right;
    Snake.size = 1;
    Snake.head->next = NULL;
    Food.x = Food.y = 12;
    moveInterval = .3;
    nextMove = .3;
}

int enact(int dir) {
    switch (dir) {
        case left:
            return -1;
            break;
        case right:
            return 1;
            break;
        case top:
            return -2;
            break;
        case down:
            return 2;
            break;
        default:
            return 0;
            break;
    };
    return 0;
}

void recursiveMove(struct Part* node) {
    if (node == NULL)
        return;

    node->x += (enact(node->dir) % 2);
    node->y += (enact(node->dir) / 2);

    node->x %= screenWidth / cellSize;
    node->y %= screenHeight / cellSize;
    if (node->x < 0)
        node->x = screenWidth / cellSize;
    if (node->y < 0)
        node->y = screenHeight / cellSize;

    recursiveMove(node->next);
    if (node->next != NULL)
        node->next->dir = node->dir;
}

void grow(struct Part* node) {
    if (node->next == NULL) {
        node->next = MemAlloc(sizeof(struct Part));

        node->next->x = node->x - (enact(node->dir) % 2);
        node->next->y = node->y - (enact(node->dir) / 2);

        node->next->dir = node->dir;
        node->next->next = NULL;
        Snake.size++;
        return;
    }
    grow(node->next);
}

void recursiveDraw(struct Part* node) {
    if (node == NULL)
        return;

    DrawRectangle(partCalc(node->x), partCalc(node->y), snakeSize, snakeSize,
                  CLITERAL(Color){255, 179, 0, 255});
    recursiveDraw(node->next);
}

int recursiveCheck(struct Part* node, int X, int Y) {
    if (node == NULL)
        return 0;
    if (node->x == X && node->y == Y)
        return 1;
    return recursiveCheck(node->next, X, Y);
}

void moveFood() {
    Food.x = GetRandomValue(0, (screenWidth / cellSize) - 1);
    Food.y = GetRandomValue(0, (screenHeight / cellSize) - 1);
    if (recursiveCheck(Snake.head, Food.x, Food.y) == 1)
        moveFood();
}

int main() {
    InitWindow(screenWidth, screenHeight, "snake");

    SetTargetFPS(60);
    // HideCursor();
    reset();

    // Main game loop

    float moveInterval = .3;
    float nextMove = .3;
    while (!WindowShouldClose()) {
        // Update

        if (IsKeyDown(KEY_RIGHT) && Snake.head->dir != left)
            Snake.head->dir = right;
        if (IsKeyDown(KEY_LEFT) && Snake.head->dir != right)
            Snake.head->dir = left;
        if (IsKeyDown(KEY_UP) && Snake.head->dir != down)
            Snake.head->dir = top;
        if (IsKeyDown(KEY_DOWN) && Snake.head->dir != top)
            Snake.head->dir = down;

        if (IsKeyPressed(KEY_N))
            grow(Snake.head);
        if (IsKeyPressed(KEY_R))
            reset();

        if (GetTime() > nextMove) {
            recursiveMove(Snake.head);
            nextMove = GetTime() + moveInterval;
        }

        // TODO check snake collusion
        // TODO check food collusion
        if (Food.x == Snake.head->x && Food.y == Snake.head->y) {
            grow(Snake.head);
            moveFood();
            moveInterval -= .024 / Snake.size;
        }

        // Draw

        BeginDrawing();

        ClearBackground(CLITERAL(Color){17, 17, 17, 255});

        // for (int X = 0, Y = 0; Y <= screenHeight; X += cellSize) {
        //     if (X == screenWidth) {
        //         X = 0;
        //         Y += cellSize;
        //     }
        //     Rectangle per;
        //     per.height = per.width = 40;
        //     per.x = X;
        //     per.y = Y;
        //     DrawRectangleLinesEx(per, 1, CLITERAL(Color){157, 159, 179,
        //     125});
        // }

        DrawRectangle(partCalc(Food.x), partCalc(Food.y), snakeSize, snakeSize,
                      CLITERAL(Color){14, 153, 234, 255});
        recursiveDraw(Snake.head);

        // DrawFPS(10, 10);
        char text[100];
        sprintf(text, "Score: %d", Snake.size - 1);
        DrawText(text, 10, 10, 28, BLUE);
        EndDrawing();
    }

    CloseWindow();  // Close window and OpenGL context

    return 0;
}