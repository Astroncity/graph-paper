#include "defs.h"
#include "flecs.h"
#include "state.h"
#include "window.h"
#include <math.h>
#include <raylib.h>

ecs_world_t* world;

v2* mouse;
f32 time;
Font globalFont;

const u32 screenWidth = 1920;
const u32 screenHeight = 1080;

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

void drawGrid(v2 origin) {
    const Color cl = GRUV_DARK3;
    const Color centerLineColor = GRUV_LIGHT1;

    const u32 step = (screenWidth < screenHeight ? screenWidth : screenHeight) / 10;

    const i32 centerX = origin.x;
    const i32 centerY = origin.y;

    for (u32 i = 0; i <= screenWidth; i += step) {
        Color lineColor = (i >= centerX - step / 2 && i <= centerX + step / 2)
                              ? centerLineColor
                              : cl;
        DrawLineEx((v2){i, 0}, (v2){i, screenHeight}, 1, lineColor);
    }

    for (u32 i = 0; i <= screenHeight; i += step) {
        Color lineColor = (i >= centerY - step / 2 && i <= centerY + step / 2)
                              ? centerLineColor
                              : cl;
        DrawLineEx((v2){0, i}, (v2){screenWidth, i}, 1, lineColor);
    }
}

void drawFunction(f64 (*func)(f64), f32 scaleX, f32 scaleY, v2 origin,
                  Color graphColor) {
    for (u32 screenX = 0; screenX < screenWidth; ++screenX) {
        f64 worldX1 = (screenX - origin.x) / scaleX;
        f64 worldX2 = (screenX + 1 - origin.x) / scaleX;

        f64 worldY1 = func(worldX1);
        f64 worldY2 = func(worldX2);

        f32 screenY1 = origin.y - worldY1 * scaleY;
        f32 screenY2 = origin.y - worldY2 * scaleY;

        DrawLineEx((v2){screenX, screenY1}, (v2){screenX + 1, screenY2}, 10,
                   graphColor);
    }
}

void drawEquation(bool (*func)(f64, f64), f32 scaleX, f32 scaleY, v2 origin,
                  Color graphColor) {
    for (u32 screenX = 0; screenX < screenWidth; ++screenX) {
        f64 worldX = (screenX - origin.x) / scaleX;

        for (u32 screenY = 0; screenY < screenHeight; ++screenY) {
            f64 worldY = (origin.y - screenY) / scaleY;

            if (func(worldX, worldY)) {
                // If the boolean function returns true, draw a point or pixel
                DrawPixel(screenX, screenY, graphColor);
            }
        }
    }
}
int main(void) {
    setWindowFlags();
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    globalFont = LoadFontEx("assets/fonts/spaceMono.ttf", 512, 0, 0);
    world = ecs_init();
    mouse = malloc(sizeof(v2));

    const f32 scaleX = 100.0f;
    const f32 scaleY = 100.0f;
    v2 origin = {screenWidth / 2.0f, screenHeight / 2.0f};

    while (!WindowShouldClose()) {
        f32 scale = getWindowScale();
        *mouse = getScreenMousePos(mouse, scale, screenWidth, screenHeight);

        BeginTextureMode(target);
        ClearBackground(GRUV_DARK0);

        if (IsKeyPressed(KEY_LEFT)) {
            origin.x -= 100;
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            origin.x += 100;
        }
        if (IsKeyPressed(KEY_UP)) {
            origin.y -= 100;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            origin.y += 100;
        }

        ecs_progress(world, GetFrameTime());
        drawGrid(origin);
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
