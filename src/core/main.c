#include "defs.h"
#include "flecs.h"
#include "state.h"
#include "window.h"
#include <math.h>
#include <raylib.h>

ecs_world_t* world;

v2* mouse;
Font globalFont;

const i32 screenWidth = 2560;
const i32 screenHeight = 1440;

f64 plotFunction(f64 x) {
    return x * cos(x);
    // extra
}

f64 crazy(f64 x) {
    return sin(pow(2, 2 * x) - pow(pow(2, x), 2)) - cos(x);
    // extra
}

bool looseEquals(f64 a, f64 b) { return fabs(a - b) < 0.3; }

bool circ(f64 x, f64 y) {
    return looseEquals(x * x + y * y, 10);
    // extra
}

void drawGrid(v2 origin, f64 scaleX, f64 scaleY) {
    const Color cl = GRUV_DARK3;
    const Color centerLineColor = GRUV_LIGHT1;

    const u32 stepX = scaleX; // Step size for x-axis, based on scaleX
    const u32 stepY = scaleY; // Step size for y-axis, based on scaleY

    const i32 centerX = origin.x;
    const i32 centerY = origin.y;

    // Draw vertical grid lines and x-axis labels
    for (i32 i = centerX % stepX; i <= screenWidth; i += stepX) {
        Color lineColor = (i == centerX) ? centerLineColor : cl;
        DrawLineEx((v2){i, 0}, (v2){i, screenHeight}, 1, lineColor);

        // Draw x-axis numbers next to the y-axis
        if (i != centerX) { // Avoid overlapping numbers at origin
            f64 worldX = (i - origin.x) / scaleX;
            DrawTextEx(globalFont, TextFormat("%.1f", worldX),
                       (v2){i + 2, centerY + 5}, 16, 0, GRUV_LIGHT2);
        }
    }

    // Draw horizontal grid lines and y-axis labels
    for (i32 i = centerY % stepY; i <= screenHeight; i += stepY) {
        Color lineColor = (i == centerY) ? centerLineColor : cl;
        DrawLineEx((v2){0, i}, (v2){screenWidth, i}, 1, lineColor);

        // Draw y-axis numbers next to the x-axis
        if (i != centerY) { // Avoid overlapping numbers at origin
            f64 worldY = (origin.y - i) / scaleY;
            DrawTextEx(globalFont, TextFormat("%.1f", worldY),
                       (v2){centerX + 5, i + 2}, 16, 0, GRUV_LIGHT2);
        }
    }

    // Draw labels for the origin (0,0) at the intersection
    DrawTextEx(globalFont, "0", (v2){centerX + 5, centerY + 5}, 16, 0, GRUV_LIGHT2);
}

void drawFunction(f64 (*func)(f64), f64 scaleX, f64 scaleY, v2 origin,
                  Color graphColor) {
    for (i32 screenX = 0; screenX < screenWidth; ++screenX) {
        f64 worldX1 = (screenX - origin.x) / scaleX;
        f64 worldX2 = (screenX + 1 - origin.x) / scaleX;

        f64 worldY1 = func(worldX1);
        f64 worldY2 = func(worldX2);

        f64 screenY1 = origin.y - worldY1 * scaleY;
        f64 screenY2 = origin.y - worldY2 * scaleY;

        DrawLineEx((v2){screenX, screenY1}, (v2){screenX + 1, screenY2}, 10,
                   graphColor);
    }
}

void drawEquation(bool (*func)(f64, f64), f64 scaleX, f64 scaleY, v2 origin,
                  Color graphColor) {
    for (i32 screenX = 0; screenX < screenWidth; ++screenX) {
        f64 worldX = (screenX - origin.x) / scaleX;

        for (i32 screenY = 0; screenY < screenHeight; ++screenY) {
            f64 worldY = (origin.y - screenY) / scaleY;

            if (func(worldX, worldY)) {
                // If the boolean function returns true, draw a point or pixel
                DrawPixel(screenX, screenY, graphColor);
            }
        }
    }
}

f64 lerp(f64 a, f64 b, f64 t) { return a + t * (b - a); }

int main(void) {
    setWindowFlags();
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    globalFont = LoadFontEx("assets/fonts/spaceMono.ttf", 512, 0, 0);
    world = ecs_init();
    mouse = malloc(sizeof(v2));

    f64 scaleX = 100.0f;
    f64 scaleY = 100.0f;
    v2 origin = {screenWidth / 2.0f, screenHeight / 2.0f};
    v2 mouseDragStart = {0, 0};
    const f64 scrollMult = 3;

    while (!WindowShouldClose()) {
        f64 scale = getWindowScale();
        *mouse = getScreenMousePos(mouse, scale, screenWidth, screenHeight);

        BeginTextureMode(target);
        ClearBackground(GRUV_DARK0);

        if (IsMouseButtonPressed(0)) {
            mouseDragStart = *mouse;
        }

        if (IsMouseButtonDown(0)) {
            origin.x -= mouseDragStart.x - mouse->x;
            origin.y -= mouseDragStart.y - mouse->y;
            mouseDragStart = *mouse;
        }

        const f64 minScale = 0.1f;

        scaleX += GetMouseWheelMove() * scrollMult;
        scaleY += GetMouseWheelMove() * scrollMult;

        scaleX = MAX(scaleX, minScale);
        scaleY = MAX(scaleY, minScale);

        ecs_progress(world, GetFrameTime());
        drawGrid(origin, scaleX, scaleY);
        drawFunction(crazy, scaleX, scaleY, origin, Fade(GRUV_BLUE, 0.3f));
        drawFunction(plotFunction, scaleX, scaleY, origin, Fade(GRUV_GREEN, 0.3f));
        drawEquation(circ, scaleX, scaleY, origin, Fade(GRUV_RED, 0.3f));

        EndTextureMode();
        BeginDrawing();
        ClearBackground(GRUV_DARK0);
        drawScaledWindow(target, screenWidth, screenHeight, scale);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
