#include "libs.h"
#include "video.h"
#include "Color.h"
#include "Coord.h"

using namespace std;
using namespace video;

enum CELL_STATE_TYPE {
  BLANK   = 0,
  MARKED  = 1,
  NOMARK  = 2,
  UNKNOWN = 3,
  OUT     = 4,
};

bool program_quit = false;
bool stop_game = true;
bool debug_mode = false;
bool edit_mode = false;
bool cheat_mode = false;

SDL_Rect frame_cellboard_rect;
const int frame_cellboard_width = 480;
const int frame_cellboard_height = 480;
const int frame_cellboard_x = 0;
const int frame_cellboard_y = 36;

SDL_Rect frame_horizontal_number_rect;
const int frame_horizontal_number_width = 180;
const int frame_horizontal_number_height = frame_cellboard_height;
const int frame_horizontal_number_x = frame_cellboard_width + frame_cellboard_x;
const int frame_horizontal_number_y = 0 + frame_cellboard_y;

SDL_Rect frame_vertical_number_rect;
const int frame_vertical_number_width = frame_cellboard_height;
const int frame_vertical_number_height = 110;
const int frame_vertical_number_x = 0 + frame_cellboard_x;
const int frame_vertical_number_y = frame_cellboard_height + frame_cellboard_y;

SDL_Rect frame_dialog_rect;
const int frame_dialog_width = 400;
const int frame_dialog_height = 300;
const int frame_dialog_x = 100;
const int frame_dialog_y = 150;

SDL_Rect mark_rect0, mark_rect1, mark_rect2, mark_rect3, mark_rect4;
SDL_Surface* mark_surface;

const int max_cells_x = 20;
const int max_cells_y = 20;
const int cell_width = 24;
const int cell_height = 24;

const int number_of_stages = 50;
int stage_state[number_of_stages];
int stage = 0;
int user_win_list[number_of_stages] = {0, };

char user_cells[max_cells_x][max_cells_y];
char answer_cells[number_of_stages][max_cells_x][max_cells_y];
string stage_name[number_of_stages];

int amount_of_cells_x;
int amount_of_cells_y;

string message0, message1;

bool title = true;


string GetStageTitle() {
  stringstream sstr;
  if (user_win_list[stage] == 1) {
    sstr << "그림" << stage + 1 << ": [ " << stage_name[stage] << " ] -- 완료";
  } else {
    sstr << "그림" << stage + 1 << ": [ " << stage_name[stage] << " ]";
  }
  return sstr.str();
}

void Title() {
  DrawBox(frame_dialog_rect, color_white);
  DrawBorder(frame_dialog_rect, 4, color_dkgray);
  Coord offset;
  offset.set_y(frame_dialog_y + 20);
  offset.set_x(frame_dialog_x + 100);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "<blue>< 도트홀릭 0.1 >");
  offset.set_y(offset.y() + 20);
  offset.set_x(frame_dialog_x + 200);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "<black>- 박연오(bakyeono@gmail.com)");
  offset.set_y(offset.y() + 40);
  offset.set_x(frame_dialog_x + 20);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "숫자에 맞게 칸에 색칠을 하는 놀이입니다.");
  offset.set_y(offset.y() + 20);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "<blue>\"1 1 1\"<black> : 그 줄에 점1개-빈칸들-점1개-빈칸들-점1개를 이어서 그림");
  offset.set_y(offset.y() + 20);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "<blue>\"5 1\"<black> : 그 줄에 점5개-빈칸들-점1개를 이어서 그림");
  offset.set_y(offset.y() + 20);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "<blue>\"2 3 2 1\"<black> : 점2개-빈칸들-점3개-빈칸들-점2개-빈칸들-점1개");
  offset.set_y(offset.y() + 20);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "점의 개수는 주어지지만, 빈칸의 개수는 요령껏 알아내셔야 합니다.");
  offset.set_y(offset.y() + 20);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "가로세로 숫자에 맞춰 색칠하다 보면 모든 칸을 알아낼 수 있어요!");
  offset.set_y(offset.y() + 40);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "<red>[좌우 화살표]<black> 키를 눌러 그림을 선택할 수 있습니다.");
  offset.set_y(offset.y() + 20);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "<red>[엔터]<black> 키를 눌러 시작합니다.");
  offset.set_y(offset.y() + 20);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "<red>[h]<black> 키를 누르면 도움말을 숨기거나 다시 드러냅니다.");
}

void PrepareMarks() {
  mark_surface = LoadImage("./mark.png");
  mark_rect0.w = cell_width;
  mark_rect0.h = cell_height;
  mark_rect0.x = 0 * cell_width;
  mark_rect0.y = 0;
  mark_rect1.w = cell_width;
  mark_rect1.h = cell_height;
  mark_rect1.x = 1 * cell_width;
  mark_rect1.y = 0;
  mark_rect2.w = cell_width;
  mark_rect2.h = cell_height;
  mark_rect2.x = 2 * cell_width;
  mark_rect2.y = 0;
  mark_rect3.w = cell_width;
  mark_rect3.h = cell_height;
  mark_rect3.x = 3 * cell_width;
  mark_rect3.y = 0;
  mark_rect4.w = cell_width;
  mark_rect4.h = cell_height;
  mark_rect4.x = 4 * cell_width;
  mark_rect4.y = 0;
}

void ClearBoards(int x, int y) {
  for (int x = 0; x < max_cells_x; ++x) {
    for (int y = 0; y < max_cells_y; ++y) {
      if (answer_cells[stage][x][y] == OUT) {
        user_cells[x][y] = OUT;
      } else {
        user_cells[x][y] = BLANK;
      }
    }
  }
}

Coord GetCellByPoint(int x, int y) {
  Coord pos;
  pos.set_x((x - frame_cellboard_x) / cell_width);
  pos.set_y((y - frame_cellboard_y) / cell_height);
  return pos;
}

bool CompareCell(int x, int y) {
  if (user_cells[x][y] == OUT)
    return true;
  if (user_cells[x][y] == UNKNOWN)
    return false;
  if (user_cells[x][y] == MARKED && answer_cells[stage][x][y] == MARKED)
    return true;
  if (user_cells[x][y] == BLANK && answer_cells[stage][x][y] == BLANK)
    return true;
  if (user_cells[x][y] == NOMARK && answer_cells[stage][x][y] == BLANK)
    return true;
  return false;
}

bool CompareBoards() {
  for (int x = 0; x < max_cells_x; ++x) {
    for (int y = 0; y < max_cells_y; ++y) {
      if (! CompareCell(x, y))
        return false;
    }
  }
  return true;
}

string GetHorizontalNumbers(int y) {
  stringstream str;
  int count = 0;
  for (int x = 0; x < max_cells_x; ++x) {
    if (answer_cells[stage][x][y] == MARKED) {
      ++count;
    } else {
      if (0 < count) {
        str << count << "  ";
        count = 0;
      }
    }
  }
  if (count != 0) {
    str << count << ' ';
  }
  return str.str();
}

string GetVerticalNumbers(int x) {
  stringstream str;
  int count = 0;
  for (int y = 0; y < max_cells_y; ++y) {
    if (answer_cells[stage][x][y] == MARKED) {
      ++count;
    } else {
      if (0 < count) {
        str << count << '\n';
        count = 0;
      }
    }
  }
  if (count != 0) {
    str << count << '\n';
  }
  return str.str();
}

void PrepareFrames() {
  frame_cellboard_rect.w = frame_cellboard_width;
  frame_cellboard_rect.h = frame_cellboard_height;
  frame_cellboard_rect.x = frame_cellboard_x;
  frame_cellboard_rect.y = frame_cellboard_y;
  frame_horizontal_number_rect.w = frame_horizontal_number_width;
  frame_horizontal_number_rect.h = frame_horizontal_number_height;
  frame_horizontal_number_rect.x = frame_horizontal_number_x;
  frame_horizontal_number_rect.y = frame_horizontal_number_y;
  frame_vertical_number_rect.w = frame_vertical_number_width;
  frame_vertical_number_rect.h = frame_vertical_number_height;
  frame_vertical_number_rect.x = frame_vertical_number_x;
  frame_vertical_number_rect.y = frame_vertical_number_y;
  frame_dialog_rect.w = frame_dialog_width;
  frame_dialog_rect.h = frame_dialog_height;
  frame_dialog_rect.x = frame_dialog_x;
  frame_dialog_rect.y = frame_dialog_y;
}

void PrepareBoards() {
  ClearBoards(20, 20);
}

SDL_Rect GetCellRect(int x, int y) {
  SDL_Rect rect;
  rect.x = frame_cellboard_x + (cell_width * x);
  rect.y = frame_cellboard_y + (cell_height * y);
  rect.w = cell_width;
  rect.h = cell_height;
  return rect;
}

SDL_Rect GetCellRect(const Coord& pos) {
  return GetCellRect(pos.x(), pos.y());
}

void DrawCells(bool show_answer) {
  for (int y = 0; y < max_cells_y; ++y) {
    for (int x = 0; x < max_cells_x; ++x) {
      switch ((show_answer) ? answer_cells[stage][x][y] : user_cells[x][y]) {
        case BLANK:
          DrawImage(GetCellRect(x, y), mark_surface, mark_rect0);
          break;
        case MARKED:
          DrawImage(GetCellRect(x, y), mark_surface, mark_rect1);
          break;
        case NOMARK:
          DrawImage(GetCellRect(x, y), mark_surface, mark_rect2);
          break;
        case UNKNOWN:
          DrawImage(GetCellRect(x, y), mark_surface, mark_rect3);
          break;
        case OUT:
          DrawImage(GetCellRect(x, y), mark_surface, mark_rect4);
          break;
        default:
          break;
      }
    }
  }
}

void DrawNumbers() {
  for (int x = 0; x < max_cells_x; ++x) {
    DrawUTF8(Coord(frame_vertical_number_x + x * cell_width + 6, frame_vertical_number_y),
             20, color_white,
             GetVerticalNumbers(x));
  }
  for (int y = 0; y < max_cells_y; ++y) {
    DrawUTF8(Coord(frame_horizontal_number_x + 4, frame_horizontal_number_y + y * cell_height + 2),
             frame_horizontal_number_width, color_white,
             GetHorizontalNumbers(y));
  }
}

void DrawBoard() {
  if (cheat_mode || stop_game && user_win_list[stage] == 1) {
    DrawCells(true);
  } else {
    DrawCells(false);
  }
  DrawBorder(frame_cellboard_rect, 2, color_dkgray);
  DrawNumbers();
}


bool InitializeEverything() {
  if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
    cerr << "[오류] SDL 초기화에 실패했습니다." << endl;
    return false;
  }
  SDL_WM_SetCaption("도트홀릭 0.1", NULL);
  if (!InitializeVideo()) {
    cerr << "[오류] 그래픽 초기화에 실패했습니다." << endl;
    return false;
  }
  const int key_delay = 160;
  const int key_interval = 90;
  SDL_EnableKeyRepeat(key_delay, key_interval);
  SDL_EnableUNICODE(SDL_ENABLE);
  return true;
}

bool CleanUpEverything() {
  CleanUpVideo();
  SDL_Quit();
  return true;
}

void DrawBackground() {
  DrawBox(Coord(0,0), screen_width, screen_height, color_white);
}

void Draw() {
  DrawBackground();
  DrawUTF8(Coord(0,0), screen_width, color_white, message0);
  DrawUTF8(Coord(0,18), screen_width, color_white, message1);
  DrawBoard();
  if (title)
    Title();
  Refresh();
}

bool IsValidCell(int x, int y) {
  if (0 <= x && x < max_cells_x && 0 <= y && y < max_cells_y) {
    return true;
  } else {
    return false;
  }
}

void MarkCell(int x, int y) {
  if (! IsValidCell(x, y))
    return;
  switch (user_cells[x][y]) {
    case BLANK:
    case UNKNOWN:
    case NOMARK:
      user_cells[x][y] = MARKED;
      break;
    case MARKED:
    case OUT:
    default:
      break;
  }
}

void NoMarkCell(int x, int y) {
  if (! IsValidCell(x, y))
    return;
  switch (user_cells[x][y]) {
    case BLANK:
      user_cells[x][y] = NOMARK;
      break;
    case NOMARK:
      user_cells[x][y] = UNKNOWN;
      break;
    case MARKED:
    case UNKNOWN:
      user_cells[x][y] = BLANK;
      break;
    case OUT:
    default:
      break;
  }
}

void OrganizeStage() {
  for (int y = 0; y < max_cells_y; ++y) {
    for (int x = 0; x < max_cells_x; ++x) {
      switch (user_cells[x][y]) {
        case BLANK:
        case MARKED:
        case OUT:
          break;
        case NOMARK:
        case UNKNOWN:
          user_cells[x][y] = BLANK;
          break;
        default:
          break;
      }
    }
  }
}

void SaveScore() {
  ofstream f;
  f.open("./score");
  f << number_of_stages << endl;
  for (int i = 0; i < number_of_stages; ++i) {
    f << user_win_list[i] << endl;
  }
  f.close();
}

void LoadScore() {
  ifstream f;
  for (int i = 0; i < number_of_stages; ++i) {
    user_win_list[i] = 0;
  }
  int this_end;
  f.open("./score");
  f >> this_end;
  for (int i = 0; i < this_end; ++i) {
    f >> user_win_list[i];
  }
  f.close();
}

void SaveStage() {
  ofstream f;
  f.open("./map_temp");
  f << "제목없음" << endl;
  f << max_cells_x << " " << max_cells_y << " " << endl;
  OrganizeStage();
  for (int y = 0; y < max_cells_y; ++y) {
    for (int x = 0; x < max_cells_x; ++x) {
      f << (int) user_cells[x][y] << " ";
    }
    f << endl;
  }
  f.close();
}

void LoadStage() {
  int n;
  int max_x, max_y;
  int adapter;
  ifstream f;
  f.open("./map");
  for (int i = 0; i < number_of_stages; ++i) {
    f >> n;
    f >> stage_name[i];
    f >> max_x >> max_y;
    for (int y = 0; y < max_cells_y; ++y) {
      for (int x = 0; x < max_cells_x; ++x) {
        f >> adapter;
        answer_cells[i][x][y] = adapter;
      }
    }
  }
  f.close();
  f.open("./mapname");
  for (int i = 0; i < number_of_stages; ++i) {
    f >> stage_name[i];
  }
  f.close();
}

void WaitKeyPress() {
  SDL_Event event;
  while (1) {
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT) {
      program_quit = true;
      stop_game = true;
      break;
    }
    if (event.type == SDL_KEYDOWN) {
      break;
    }
  }
}

void ShowWinDialog() {
  DrawBox(frame_dialog_rect, color_white);
  DrawBorder(frame_dialog_rect, 4, color_dkgray);
  Coord offset;
  offset.set_y(frame_dialog_y + 150);
  offset.set_x(frame_dialog_x + 100);
  offset.set_y(offset.y() + 20);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "<red>유리카!");
  offset.set_y(offset.y() + 20);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           stage_name[stage]);
  offset.set_y(offset.y() + 20);
  DrawUTF8(offset, frame_dialog_width - 40, color_white,
           "아무 키나 누르십시오.");
  Refresh();
  WaitKeyPress();
}

void CheckWin() {
  if (CompareBoards()) {
    user_win_list[stage] = 1;
    ShowWinDialog();
    stop_game = true;
  }
}

void ControlMouseEvent(SDL_Event& event) {
  Coord target_cell;
  target_cell = GetCellByPoint(event.button.x, event.button.y);
  if (event.button.button == SDL_BUTTON_LEFT) {
    MarkCell(target_cell.x(), target_cell.y());
  }
  if (event.button.button == SDL_BUTTON_RIGHT) {
    NoMarkCell(target_cell.x(), target_cell.y());
  }
  if (! edit_mode) {
    CheckWin();
  }
}

void Play() {
  SDL_Event event;
  message1 = "게임 중 ...";
  Draw();
  while (!stop_game) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        program_quit = true;
        stop_game = true;
      }
      if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEMOTION) {
        ControlMouseEvent(event);
        Draw();
      }
      if (event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
        case SDLK_ESCAPE:
          stop_game = true;
          title = true;
          break;

        case SDLK_F1:
          break;

        case SDLK_F3:
          if (debug_mode) {
            if (cheat_mode) {
              cheat_mode = false;
              message0 = "답 보기 중지";
            } else {
              cheat_mode = true;
              message0 = "답 보기 시작";
            }
            Draw();
          }
          break;

        case SDLK_F4:
          if (debug_mode) {
            if (edit_mode) {
              edit_mode = false;
              message0 = "플레이 모드";
            } else {
              edit_mode = true;
              message0 = "에디트 모드";
            }
            Draw();
          }
          break;

        case SDLK_F5:
          if (debug_mode) {
            SaveStage();
            message0 = "스테이지가 저장되었습니다.";
            Draw();
          }
          break;

        case SDLK_F6:
          if (debug_mode) {
            PrepareBoards();
            message0 = "스테이지를 초기화 합니다.";
            Draw();
          }
          break;

        case SDLK_h:
          if (title) {
            title = false;
          } else {
            title = true;
          }
          Draw();
          break;

        default:
          break;
        }
      }
    }
  }
}

void MainMenu() {
  SDL_Event event;
  message0 = GetStageTitle();
  message1 = "게임을 시작하려면 <red>[엔터]<black> 키를 누르세요.";
  PrepareBoards();
  Draw();
  while (!program_quit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        program_quit = true;
      }
      if (event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
        case SDLK_ESCAPE:
          program_quit = true;
          break;

        case SDLK_LEFT:
          --stage;
          if (stage < 0)
            stage = 0;
          message0 = GetStageTitle();
          PrepareBoards();
          Draw();
          break;

        case SDLK_RIGHT:
          ++stage;
          if (number_of_stages <= stage)
            stage = number_of_stages - 1;
          message0 = GetStageTitle();
          PrepareBoards();
          Draw();
          break;

        case SDLK_RETURN:
          title = false;
          stop_game = false;
          Play();
          message1 = "게임을 시작하려면 <red>[엔터]<black> 키를 누르세요.";
          Draw();
          break;

        case SDLK_h:
          if (title) {
            title = false;
          } else {
            title = true;
          }
          Draw();
          break;

        default:
          break;
        }
      }
    }
  }
}

int main(int argc, char** args) {
  if ((2 <= argc) && (strcmp(args[1], "bakyeono"))) {
      debug_mode = true;
  }

  if (!InitializeEverything())
    return 1;
  if (!(Refresh())) {
    cerr << "[오류] 화면 갱신에 실패했습니다." << endl;
    return 1;
  }
  PrepareFrames();
  PrepareMarks();
  LoadStage();
  LoadScore();
  PrepareBoards();
  MainMenu();
  SaveScore();
  CleanUpEverything();
  return 0;
}
