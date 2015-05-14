#include "video.h"

using namespace std;

namespace video {

int screen_width   = 640;
int screen_height  = 666;
int screen_bpp     = 32;
bool fullscreen    = false;
bool noframe       = false;
bool videomemory   = true;
Uint32 video_options;

const int number_of_images = 10;

SDL_Surface* screen = NULL;
SDL_Surface* screen_backup = NULL;
TTF_Font* font = NULL;

SDL_Surface* LoadImage(string filename) {
  SDL_Surface* loaded_image = NULL;
  SDL_Surface* optimized_image = NULL;

  loaded_image = IMG_Load(filename.c_str());

  if (loaded_image != NULL) {
    optimized_image = SDL_DisplayFormatAlpha(loaded_image);
    SDL_FreeSurface(loaded_image);
    return optimized_image;
  } else {
    cerr << "[오류] " << filename << " 파일을 불러오지 못했습니다." << endl;
    return false;
  }
}

const bool UnloadImage(SDL_Surface* surface) {
  if (surface == NULL) {
      return false;
  } else {
    SDL_FreeSurface(surface);
    return true;
  }
}

const bool LoadFont() {
  font = TTF_OpenFont("./NanumGothicBold.ttf", 13);
  return true;
}

const bool InitializeVideo() {
  // 비디오 옵션 플래그 값 만들기
  if (videomemory) {
    if (fullscreen) {
      video_options = (SDL_HWSURFACE | SDL_FULLSCREEN);
    } else {
      if (noframe) {
        video_options = (SDL_HWSURFACE | SDL_NOFRAME);
      } else {
        video_options = (SDL_HWSURFACE);
      }
    }
  } else {
    if (fullscreen) {
      video_options = (SDL_SWSURFACE | SDL_FULLSCREEN);
    } else {
      if (noframe) {
        video_options = (SDL_SWSURFACE | SDL_NOFRAME);
      } else {
      video_options = (SDL_SWSURFACE);
      }
    }
  }
  screen = SDL_SetVideoMode(screen_width, screen_height, screen_bpp, video_options);
  if (screen == NULL) {
    cerr << "[오류] 비디오 모드 설정에 실패했습니다." << endl;
    return false;
  }
  TTF_Init();
  LoadFont();
  // 스크린 백업 서페이스
  screen_backup = SDL_CreateRGBSurface(SDL_SWSURFACE, screen_width, screen_height, screen_bpp, 0, 0, 0, 0);
  return true;
}

void CleanUpVideo() {
  SDL_FreeSurface(screen_backup);
  TTF_CloseFont(font);
  TTF_Quit();
}

void DrawImage(const Coord& offset, const int& width, const int& height, SDL_Surface* image, SDL_Rect image_rect) {
  SDL_Rect rect;
  rect.w = width;
  rect.h = height;
  SDL_BlitSurface(image, &image_rect, screen, &rect);
}

void DrawImage(SDL_Rect rect, SDL_Surface* image, SDL_Rect image_rect) {
  SDL_BlitSurface(image, &image_rect, screen, &rect);
}

void DrawBox(const SDL_Rect& rect, const SDL_Color& color) {
  SDL_Rect _rect = rect;
  Uint32 converted_color(SDL_MapRGB(screen->format, color.r, color.g, color.b));
  SDL_FillRect(screen, &_rect, converted_color);
}

void DrawBox(const Coord& offset, const int& width, const int& height, const SDL_Color& color) {
  SDL_Rect rect;
  rect.x = offset.x();
  rect.y = offset.y();
  rect.w = width;
  rect.h = height;
  Uint32 converted_color(SDL_MapRGB(screen->format, color.r, color.g, color.b));
  SDL_FillRect(screen, &rect, converted_color);
}

void DrawBorder(const SDL_Rect& rect, const int& thickness, const SDL_Color& color) {
  Uint32 converted_color(SDL_MapRGB(screen->format, color.r, color.g, color.b));
  SDL_Rect _rect;
  _rect.x = rect.x;
  _rect.y = rect.y;
  _rect.w = thickness;
  _rect.h = rect.h;
  SDL_FillRect(screen, &_rect, converted_color);
  _rect.w = rect.w;
  _rect.h = thickness;
  SDL_FillRect(screen, &_rect, converted_color);
  _rect.x = rect.x + rect.w - thickness;
  _rect.w = thickness;
  _rect.h = rect.h;
  SDL_FillRect(screen, &_rect, converted_color);
  _rect.x = rect.x;
  _rect.y = rect.y + rect.h - thickness;
  _rect.w = rect.w;
  _rect.h = thickness;
  SDL_FillRect(screen, &_rect, converted_color);
}
void DrawUTF8(const Coord& offset, const int& width,
                       const SDL_Color& bg_color, const string& text) {
  // 단어 입력 루틴
  vector<string> word_vector; // 단어 벡터
  string word;                   // 단어 버퍼
  char ch;
  auto text_it = text.begin();
  while(text_it != text.end()) {
    // char 가져옴
    ch = *text_it++;
    // 단어의 끝 :
    if (ch == '\n' || ch == ' ' || ch == '\t') {
      if (!word.empty()) {
        word_vector.push_back(word);
        word.clear();
      }
      word.push_back(ch);
      word_vector.push_back(word);
      word.clear();
    } else if (ch == '<') {
      if (!word.empty()) {
        word_vector.push_back(word);
        word.clear();
      }
      word.push_back(ch);
    } else if (ch == '>') {
      word.push_back(ch);
      word_vector.push_back(word);
      word.clear();
    }
    // 단어가 계속될 때 :
    else {
      word.push_back(ch);
    }
  }
  if (!word.empty())
    word_vector.push_back(word);

  // 단어 출력 루틴
  int marginHeight = TTF_FontHeight(font);    // 줄간격
  SDL_Color font_color = color_black;
  SDL_Surface* word_surface = NULL; // 개별 글자 렌더링 서페이스
  SDL_Rect rect;
  rect.x = offset.x();
  rect.y = offset.y();
  auto word_vector_it = word_vector.begin();
  while(word_vector_it != word_vector.end()) {
    // 태그 처리
    if ((*word_vector_it)[0] == '<') {
      if (word_vector_it->compare("<black>") == 0) {
        font_color = color_black;
        word_vector_it++;
        continue;
      } else if (word_vector_it->compare("<white>") == 0) {
        font_color = color_white;
        word_vector_it++;
        continue;
      } else if (word_vector_it->compare("<red>") == 0) {
        font_color = color_red;
        word_vector_it++;
        continue;
      } else if (word_vector_it->compare("<green>") == 0) {
        font_color = color_green;
        word_vector_it++;
        continue;
      } else if (word_vector_it->compare("<blue>") == 0) {
        font_color = color_blue;
        word_vector_it++;
        continue;
      } else {
      }
    }
    if ((*word_vector_it)[0] == '\n') {
      rect.x = offset.x();
      rect.y += marginHeight;
      word_vector_it++;
      continue;
    } else if ((*word_vector_it)[0] == ' ') {
      word_surface = TTF_RenderUTF8_Solid(font, " ", font_color);
      rect.x += word_surface->w;
      word_vector_it++;
      SDL_FreeSurface(word_surface);
      continue;
    } else {
      word_surface = TTF_RenderUTF8_Shaded(font, word_vector_it->c_str(), font_color, bg_color);
      if (rect.x >= width + offset.x()) {
        rect.x = offset.x();
        rect.y += marginHeight;
      }
      SDL_BlitSurface(word_surface, NULL, screen, &rect);
      rect.x += word_surface->w;
      word_vector_it++;
      SDL_FreeSurface(word_surface);
      continue;
    }
  }
}
void DrawGraph(SDL_Rect& rect,
               const SDL_Color& current_color, const SDL_Color& max_color,
               const int& current, const int& max) {
  DrawBox(rect, max_color);
  if (current < 0)
    return;
  DrawBox(Coord(rect.x, rect.y),
          rect.w*((double)current/(double)max),
          rect.h,
          current_color);
}

void BackUpScreen() {
  SDL_BlitSurface(screen, NULL, screen_backup, NULL);
}
void RestoreScreen() {
  SDL_BlitSurface(screen_backup, NULL, screen, NULL);
}
const bool Refresh() {
  if (SDL_Flip(screen)==-1)
    return false;
  else
    return true;
}

} // namespace video
