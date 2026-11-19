#include <ecs/core/world.h>
#include <ecs/core/pool.h>
#include <cstdio>
#include <core/components.h>
#include <ecs/debugging/debug.h>
#include <ecs/core/entity.h>
#include <ecs/core/scheduler.h>
#include <chrono>
#include "./core/systems/all.h"
#include <ecs/platform/window.h>
#include <ecs/platform/debugging/debug.h>





using Clock = std::chrono::steady_clock;
auto seconds = [](auto d) { return std::chrono::duration<float>(d).count(); };

#define TOTALTICKS 1000
int main() {
    ecs::World w;
    ecs::Scheduler scheduler;
    ecs::platform::addWindowSystems(scheduler);
    game::addGameplaySystems(scheduler);

    ecs::Entity e = w.create();
    ecs::platform::WindowDesc windowDesc = w.add<ecs::platform::WindowDesc>(e, ecs::platform::WindowDesc {
            .title = "ECSE",
            .x = 200,
            .y = 150,
            .w = 640,
            .h = 480,
        });
 
    ecs::platform::debug::printWindowDesc(windowDesc);

    //ecs::platform::applyWindows(w, 1);

    /*for (int tick = 0; tick < 1000; ++tick) {
        scheduler.tick(w, 1.0f / 60);
        assert(ecs::debug::validate(w));
    }*/

    //ecs::debug::printSummary(w);
    // expect: Position 12, Velocity 12, Lifetime 12, Frozen 4
}
//#define SDL_MAIN_USE_CALLBACKS 1
//#include <SDL3/SDL.h>
//#include <SDL3/SDL_main.h>
//#include <vector>
//
//#define AMOUNT_OF_WINDOWS 5
//
///* We will use this renderer to draw into this window every frame. */
//static std::vector<SDL_Window*> windows;
//static std::vector<SDL_Renderer*> renderers;
//static std::vector<SDL_Texture*> textures;
//
//
///* This function runs once at startup. */
//SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
//{
//    SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");
//
//    if (!SDL_Init(SDL_INIT_VIDEO)) {
//        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
//        return SDL_APP_FAILURE;
//    }
//
//    SDL_Surface* sprite = SDL_LoadBMP("assets/test.bmp");   // SDL 3.4+; see note below
//    if (!sprite) {
//        SDL_Log("Couldn't load sprite: %s", SDL_GetError());
//        return SDL_APP_FAILURE;
//    }
//
//    for (int i = 0; i < AMOUNT_OF_WINDOWS; i++) {
//
//        SDL_Window* window = NULL;
//        SDL_Renderer* renderer = NULL;
//        //SDL_CreateWindow();
//        //SDL_CreateRenderer();
//        if (!SDL_CreateWindowAndRenderer("", 640, 480, SDL_WINDOW_BORDERLESS | SDL_WINDOW_TRANSPARENT, &window, &renderer)) {
//            SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
//            return SDL_APP_FAILURE;
//        }
//
//        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, sprite);
//        if (!texture) {
//            SDL_Log("Couldn't create texture: %s", SDL_GetError());
//            return SDL_APP_FAILURE;
//        }
//        textures.push_back(texture);
//
//        SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);
//        SDL_SetWindowPosition(window, 640 *i, ((480*i)%1080));
//        windows.push_back(window);
//        renderers.push_back(renderer);
//
//    }
//
//    SDL_DestroySurface(sprite);
//
//    return SDL_APP_CONTINUE;  /* carry on with the program! */
//}
//
///* This function runs when a new event (mouse input, keypresses, etc) occurs. */
//SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
//{
//    if (event->type == SDL_EVENT_QUIT) {
//        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
//    }
//
//    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
//        SDL_Window* closed = SDL_GetWindowFromID(event->window.windowID);
//        for (size_t i = 0; i < windows.size(); i++) {
//            if (windows[i] == closed) {
//                // Close
//                // Remove from windows
//                // remove from renderers
//                SDL_DestroyWindow(windows[i]);
//                SDL_DestroyRenderer(renderers[i]);
//                renderers.erase(renderers.begin() + i);
//                windows.erase(windows.begin() + i);
//                break;
//            }
//        }
//
//    }
//
//    return SDL_APP_CONTINUE;  /* carry on with the program! */
//}
//
///* This function runs once per frame, and is the heart of the program. */
//SDL_AppResult SDL_AppIterate(void* appstate)
//{
//    //const double now = ((double)SDL_GetTicks()) / 1000.0;  /* convert from milliseconds to seconds. */
//    ///* choose the color for the frame we will draw. The sine wave trick makes it fade between colors smoothly. */
//
//    //const float red = (float)(0.5 + 0.5 * SDL_sin(now));
//    //const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
//    //const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
//
//    for (size_t i = 0; i < renderers.size(); i++) {
//        //SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */
//
//        ///* clear the window to the draw color. */
//        //SDL_RenderClear(renderer);
//
//        ///* put the newly-cleared rendering on the screen. */
//        //SDL_RenderPresent(renderer);
//
//        SDL_Renderer* renderer = renderers[i];
//        SDL_Texture* texture = textures[i];
//
//        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);   // fully transparent
//        SDL_RenderClear(renderer);
//
//        SDL_FRect dst = { 100.0f, 100.0f, 64.0f, 64.0f };
//        SDL_RenderTexture(renderer, texture, NULL, &dst);
//
//        SDL_RenderPresent(renderer);
//    }
//
//    return SDL_APP_CONTINUE;  /* carry on with the program! */
//}
//
///* This function runs once at shutdown. */
//void SDL_AppQuit(void* appstate, SDL_AppResult result)
//{
//    /* SDL will clean up the window/renderer for us. */
//
//}
//
