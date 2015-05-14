#ifndef VIDEO_H
#define VIDEO_H

#include "libs.h"
#include "Coord.h"
#include "Color.h"
#include "libs.h"

using namespace std;

namespace video {
extern int screen_width;
extern int screen_height;
extern int screen_bpp;
extern bool fullscreen;
extern bool noframe;
extern bool videomemory;

extern SDL_Surface* screen;
extern SDL_Surface* screen_backup;
extern Uint32 video_options;
extern TTF_Font* font;

SDL_Surface* LoadImage(string filename);
const bool UnloadImage(SDL_Surface* surface);
const bool LoadFont();

const bool InitializeVideo();
void CleanUpVideo();

void DrawImage(const Coord& offset, const int& width, const int& height, SDL_Surface* image, SDL_Rect image_rect);
void DrawImage(SDL_Rect rect, SDL_Surface* image, SDL_Rect image_rect);
void DrawBox(const SDL_Rect& rect, const SDL_Color& color);
void DrawBox(const Coord& offset, const int& width, const int& height, const SDL_Color& color);
void DrawBorder(const SDL_Rect& rect, const int& thickness, const SDL_Color& color);
void DrawGraph(SDL_Rect& rect, const SDL_Color& current_color, const SDL_Color& max_color, const int& current, const int& max);
void DrawUTF8(const Coord& offset, const int& width, const SDL_Color& bg_color, const string& text);
void BackUpScreen();
void RestoreScreen();
const bool Refresh();
} // namespace video

#endif // VIDEO_H

