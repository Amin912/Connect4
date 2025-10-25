#include "Paint.h"

#include "Theme.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>

namespace paint {

namespace {

// Colour with straight (not premultiplied) alpha, channels in 0..1.
struct Color {
    float r, g, b, a;
};

Color toColor(SDL_Color c)
{
    return {c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f};
}

Color mix(Color x, Color y, float t)
{
    return {x.r + (y.r - x.r) * t, x.g + (y.g - x.g) * t, x.b + (y.b - x.b) * t,
            x.a + (y.a - x.a) * t};
}

// Positive amounts move towards white, negative towards black.
Color shade(Color c, float amount)
{
    const Color target = amount > 0 ? Color{1, 1, 1, c.a} : Color{0, 0, 0, c.a};
    return mix(c, target, std::abs(amount));
}

float smoothstep(float edge0, float edge1, float x)
{
    const float t = std::clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
    return t * t * (3 - 2 * t);
}

// How much of a pixel lies inside a shape, given the signed distance from the
// pixel's centre to the shape's edge in pixels (negative means inside).
float coverage(float distance)
{
    return std::clamp(0.5f - distance, 0.f, 1.f);
}

// Signed distance from (x, y) to a rounded rectangle centred on the origin.
float roundedRectDistance(float x, float y, float halfWidth, float halfHeight, float corner)
{
    const float qx = std::abs(x) - halfWidth + corner;
    const float qy = std::abs(y) - halfHeight + corner;
    const float outside = std::hypot(std::max(qx, 0.f), std::max(qy, 0.f));
    const float inside = std::min(std::max(qx, qy), 0.f);
    return outside + inside - corner;
}

// Fills a width x height surface by calling shader(x, y) for the centre of
// every pixel.
template <typename Shader>
SurfacePtr paintSurface(int width, int height, Shader shader)
{
    SurfacePtr surface(SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32));
    if (!surface) {
        throw std::runtime_error(std::string("Could not create a surface: ") + SDL_GetError());
    }
    for (int y = 0; y < height; ++y) {
        auto* row = static_cast<Uint8*>(surface->pixels) + y * surface->pitch;
        for (int x = 0; x < width; ++x) {
            const Color c = shader(x + 0.5f, y + 0.5f);
            Uint8* px = row + x * 4;
            px[0] = static_cast<Uint8>(std::lround(std::clamp(c.r, 0.f, 1.f) * 255));
            px[1] = static_cast<Uint8>(std::lround(std::clamp(c.g, 0.f, 1.f) * 255));
            px[2] = static_cast<Uint8>(std::lround(std::clamp(c.b, 0.f, 1.f) * 255));
            px[3] = static_cast<Uint8>(std::lround(std::clamp(c.a, 0.f, 1.f) * 255));
        }
    }
    return surface;
}

int pixels(float units, float scale)
{
    return static_cast<int>(std::ceil(units * scale));
}

// Centre of the board hole nearest to (x, y), in pixels.
void nearestHole(float x, float y, float scale, float& holeX, float& holeY)
{
    const float cell = theme::Cell * scale;
    const float pad = theme::BoardPadding * scale;
    const int col = std::clamp(static_cast<int>((x - pad) / cell), 0, connect4::Board::Cols - 1);
    const int row = std::clamp(static_cast<int>((y - pad) / cell), 0, connect4::Board::Rows - 1);
    holeX = pad + (col + 0.5f) * cell;
    holeY = pad + (row + 0.5f) * cell;
}

}  // namespace

SurfacePtr piece(float radius, SDL_Color color, float scale)
{
    const float r = radius * scale;
    const int size = pixels(2 * radius, scale) + 2;
    const float centre = size / 2.f;
    const Color base = toColor(color);

    return paintSurface(size, size, [&](float x, float y) {
        const float dx = x - centre;
        const float dy = y - centre;
        const float dist = std::hypot(dx, dy);
        const float alpha = coverage(dist - r);
        if (alpha <= 0) {
            return Color{0, 0, 0, 0};
        }

        // Lit from above: lighter at the top, darker at the bottom.
        Color c = mix(shade(base, 0.14f), shade(base, -0.16f), (dy / r + 1) / 2);

        // A recessed centre with a thin bright lip, like a game counter.
        const float inner = 0.64f * r;
        c = mix(c, shade(c, -0.08f), coverage(dist - inner));
        const float lip = 1 - smoothstep(0, 1.6f * scale, std::abs(dist - inner));
        c = mix(c, shade(base, 0.3f), lip * 0.55f * (1 - (dy / r + 1) / 2));

        // Darker rim and a soft highlight.
        c = mix(c, shade(base, -0.3f), smoothstep(-3 * scale, 0, dist - r) * 0.6f);
        const float shine = 1 - smoothstep(0, 0.55f * r, std::hypot(dx + 0.3f * r, dy + 0.42f * r));
        c = mix(c, Color{1, 1, 1, 1}, shine * 0.2f);

        c.a = alpha;
        return c;
    });
}

SurfacePtr boardFront(float scale)
{
    const int width = pixels(theme::BoardWidth, scale);
    const int height = pixels(theme::BoardHeight, scale);
    const Color base = toColor(theme::BoardFront);
    const float holeRadius = theme::HoleRadius * scale;

    return paintSurface(width, height, [&](float x, float y) {
        const float edge = roundedRectDistance(x - width / 2.f, y - height / 2.f, width / 2.f,
                                               height / 2.f, theme::BoardCorner * scale);
        float holeX = 0;
        float holeY = 0;
        nearestHole(x, y, scale, holeX, holeY);
        const float toHole = std::hypot(x - holeX, y - holeY) - holeRadius;

        const float alpha = coverage(edge) * coverage(-toHole);
        if (alpha <= 0) {
            return Color{0, 0, 0, 0};
        }

        Color c = mix(shade(base, 0.1f), shade(base, -0.1f), y / height);

        // Bevel around each hole: shadowed at the top, catching light below.
        const float side = std::clamp((y - holeY) / holeRadius, -1.f, 1.f);
        const float bevel = 1 - smoothstep(0, 4 * scale, toHole);
        c = mix(c, shade(c, side > 0 ? 0.22f : -0.3f), bevel * std::abs(side) * 0.8f);

        // Light catches the top edge of the board.
        const float rim = 1 - smoothstep(0, 2.5f * scale, -edge);
        c = mix(c, shade(base, 0.25f), rim * (y < height / 2.f ? 0.7f : 0.15f));

        c.a = alpha;
        return c;
    });
}

SurfacePtr boardBack(float scale)
{
    const int width = pixels(theme::BoardWidth, scale);
    const int height = pixels(theme::BoardHeight, scale);
    const Color base = toColor(theme::BoardBack);
    const float holeRadius = theme::HoleRadius * scale;

    return paintSurface(width, height, [&](float x, float y) {
        const float edge = roundedRectDistance(x - width / 2.f, y - height / 2.f, width / 2.f,
                                               height / 2.f, theme::BoardCorner * scale);
        float holeX = 0;
        float holeY = 0;
        nearestHole(x, y, scale, holeX, holeY);
        // Each empty hole is darker towards the top, as if in shadow.
        const float side = (y - holeY) / holeRadius;
        Color c = shade(base, -0.35f * (1 - smoothstep(-1, 0.4f, side)));
        c.a = coverage(edge);
        return c;
    });
}

SurfacePtr boardShadow(float blur, float scale)
{
    const int width = pixels(theme::BoardWidth + 2 * blur, scale);
    const int height = pixels(theme::BoardHeight + 2 * blur, scale);
    const float soft = blur * scale;

    return paintSurface(width, height, [&](float x, float y) {
        const float edge = roundedRectDistance(x - width / 2.f, y - height / 2.f,
                                               theme::BoardWidth * scale / 2,
                                               theme::BoardHeight * scale / 2,
                                               theme::BoardCorner * scale);
        const float alpha = 1 - smoothstep(-soft * 0.6f, soft, edge);
        return Color{0, 0, 0, 0.55f * alpha * alpha};
    });
}

SurfacePtr ring(float radius, float thickness, float scale)
{
    const int size = pixels(2 * (radius + thickness), scale) + 2;
    const float centre = size / 2.f;
    return paintSurface(size, size, [&](float x, float y) {
        const float dist = std::hypot(x - centre, y - centre);
        return Color{1, 1, 1, coverage(std::abs(dist - radius * scale) - thickness * scale / 2)};
    });
}

SurfacePtr roundedRect(float width, float height, float corner, float scale)
{
    const int w = pixels(width, scale);
    const int h = pixels(height, scale);
    return paintSurface(w, h, [&](float x, float y) {
        const float edge = roundedRectDistance(x - w / 2.f, y - h / 2.f, w / 2.f, h / 2.f, corner * scale);
        return Color{1, 1, 1, coverage(edge)};
    });
}

SurfacePtr glow(float radius, float scale)
{
    const int size = pixels(2 * radius, scale);
    const float centre = size / 2.f;
    return paintSurface(size, size, [&](float x, float y) {
        const float fade = 1 - smoothstep(0, radius * scale, std::hypot(x - centre, y - centre));
        return Color{1, 1, 1, fade * fade};
    });
}

}  // namespace paint

Sprites::Sprites(SDL_Renderer* renderer, float scale)
    : scale(scale),
      humanPiece(Texture::fromSurface(renderer, paint::piece(theme::PieceRadius, theme::HumanColor, scale).get())),
      aiPiece(Texture::fromSurface(renderer, paint::piece(theme::PieceRadius, theme::AiColor, scale).get())),
      boardFront(Texture::fromSurface(renderer, paint::boardFront(scale).get())),
      boardBack(Texture::fromSurface(renderer, paint::boardBack(scale).get())),
      boardShadow(Texture::fromSurface(renderer, paint::boardShadow(ShadowBlur, scale).get())),
      ring(Texture::fromSurface(renderer, paint::ring(RingRadius, 3, scale).get())),
      glow(Texture::fromSurface(renderer, paint::glow(GlowRadius, scale).get())),
      humanDot(Texture::fromSurface(renderer, paint::piece(7, theme::HumanColor, scale).get())),
      aiDot(Texture::fromSurface(renderer, paint::piece(7, theme::AiColor, scale).get())),
      columnHighlight(Texture::fromSurface(
          renderer, paint::roundedRect(theme::Cell - 8, theme::BoardHeight - 20, (theme::Cell - 8) / 2, scale).get()))
{
}
