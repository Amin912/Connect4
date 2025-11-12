#include "Game.h"

#include "Grid.h"
#include "Menu.h"
#include "Paint.h"
#include "Text.h"
#include "Theme.h"

#include <exception>
#include <string>

namespace {

void fail(const std::string& message)
{
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Four in a Row", message.c_str(), nullptr);
}

std::string basePath()
{
    std::string path;
    if (char* base = SDL_GetBasePath()) {
        path = base;
        SDL_free(base);
    }
    return path;
}

// Fonts are looked up next to the executable, not in the current working
// directory, so the game also starts when launched from somewhere else.
std::string fontDirectory()
{
    return basePath() + "assets/fonts/";
}

}  // namespace

Game::Game() = default;

Game::~Game()
{
    // Textures belong to the renderer, so they go first.
    grid.reset();
    menu.reset();
    text.reset();
    sprites.reset();
    if (handCursor) {
        SDL_FreeCursor(handCursor);
    }
    if (arrowCursor) {
        SDL_FreeCursor(arrowCursor);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

bool Game::init(const char* title)
{
    // Size the window in display points rather than pixels, so it is not tiny
    // on a high-DPI screen, and render at the full pixel resolution.
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "1");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fail(std::string("Could not start SDL: ") + SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              theme::WindowWidth, theme::WindowHeight, SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        fail(std::string("Could not create the window: ") + SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, 0);
    }
    if (!renderer) {
        fail(std::string("Could not create the renderer: ") + SDL_GetError());
        return false;
    }

    // Everything is laid out in window units; scale them up to pixels.
    int pixelWidth = 0;
    int pixelHeight = 0;
    SDL_GetRendererOutputSize(renderer, &pixelWidth, &pixelHeight);
    const float scale = static_cast<float>(pixelWidth) / theme::WindowWidth;
    SDL_RenderSetScale(renderer, scale, scale);

    arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

    try {
        sprites = std::make_unique<Sprites>(renderer, scale);
        text = std::make_unique<Text>(renderer, fontDirectory(), scale);
        menu = std::make_unique<Menu>(renderer, *sprites, *text);
        grid = std::make_unique<Grid>(renderer, *sprites, *text);

        const SurfacePtr icon = paint::piece(28, theme::HumanColor, 1);
        SDL_SetWindowIcon(window, icon.get());
    } catch (const std::exception& e) {
        fail(e.what());
        return false;
    }

    isActivated = true;
    return true;
}

void Game::event()
{
    // Handle every event that arrived since the last frame.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isActivated = false;
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F12) {
            screenshotRequested = true;
        } else if (screen == Screen::Menu) {
            switch (menu->event(event)) {
            case Menu::Action::Play:
                grid->start(menu->settings());
                screen = Screen::Playing;
                break;
            case Menu::Action::Quit:
                isActivated = false;
                break;
            case Menu::Action::None:
                break;
            }
        } else if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) ||
                   grid->event(event) == Grid::Action::Menu) {
            screen = Screen::Menu;
        }
    }
}

void Game::update(float dt)
{
    if (screen == Screen::Menu) {
        menu->update(dt);
    } else {
        grid->update(dt);
    }
}

void Game::drawBackground() const
{
    const float w = theme::WindowWidth;
    const float h = theme::WindowHeight;
    const SDL_Vertex corners[4] = {
        {{0, 0}, theme::BackgroundTop, {0, 0}},
        {{w, 0}, theme::BackgroundTop, {0, 0}},
        {{w, h}, theme::BackgroundBottom, {0, 0}},
        {{0, h}, theme::BackgroundBottom, {0, 0}},
    };
    const int triangles[6] = {0, 1, 2, 0, 2, 3};
    SDL_RenderGeometry(renderer, nullptr, corners, 4, triangles, 6);

    // Faint coloured light in two corners, in the players' colours.
    SDL_Texture* glow = sprites->glow.get();
    SDL_SetTextureAlphaMod(glow, 34);
    SDL_SetTextureColorMod(glow, theme::HumanColor.r, theme::HumanColor.g, theme::HumanColor.b);
    Texture::drawCentered(renderer, glow, 60, 40, sprites->scale);
    SDL_SetTextureColorMod(glow, theme::AiColor.r, theme::AiColor.g, theme::AiColor.b);
    Texture::drawCentered(renderer, glow, w - 40, h - 20, sprites->scale);
    SDL_SetTextureColorMod(glow, 255, 255, 255);
    SDL_SetTextureAlphaMod(glow, 255);
}

void Game::render()
{
    drawBackground();
    bool overButton = false;
    if (screen == Screen::Menu) {
        overButton = menu->render();
    } else {
        overButton = grid->render();
    }
    SDL_SetCursor(overButton ? handCursor : arrowCursor);
    if (screenshotRequested) {
        saveScreenshot();
        screenshotRequested = false;
    }
    SDL_RenderPresent(renderer);
}

// Saves the frame just drawn as a BMP next to the executable, at full pixel
// resolution.
void Game::saveScreenshot() const
{
    int width = 0;
    int height = 0;
    SDL_GetRendererOutputSize(renderer, &width, &height);
    const SurfacePtr shot(SDL_CreateRGBSurfaceWithFormat(0, width, height, 24, SDL_PIXELFORMAT_BGR24));
    if (!shot || SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_BGR24, shot->pixels, shot->pitch) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Screenshot failed: %s", SDL_GetError());
        return;
    }
    const std::string path = basePath() + "screenshot-" + std::to_string(SDL_GetTicks()) + ".bmp";
    if (SDL_SaveBMP(shot.get(), path.c_str()) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Screenshot failed: %s", SDL_GetError());
    }
}
