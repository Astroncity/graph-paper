#include "flecs.h"
#include "render.h"
#include "state.h"
#include "transform.h"
#include "uiFramework.h"
#include "window.h"
#include <raylib.h>
#include <stdio.h>

ecs_world_t* world;

v2* mouse;
f32 time;
Font globalFont;

/* const u32 screenWidth = 480; */
/* const u32 screenHeight = 270; */

const u32 screenWidth = 640;
const u32 screenHeight = 360;

int main(void) {
    setWindowFlags();
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    globalFont = LoadFontEx("assets/fonts/spaceMono.ttf", 512, 0, 0);
    world = ecs_init();
    mouse = malloc(sizeof(v2));

    ECS_IMPORT(world, TransformModule);
    ECS_IMPORT(world, RendererModule);
    ECS_IMPORT(world, UIModule);

    while (!WindowShouldClose()) {
        f32 scale = getWindowScale();
        *mouse = getScreenMousePos(mouse, scale, screenWidth, screenHeight);

        BeginTextureMode(target);
        ClearBackground(BLACK);

        ecs_progress(world, GetFrameTime());

        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledWindow(target, screenWidth, screenHeight, scale);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
