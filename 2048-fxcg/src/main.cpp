#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/rtc.h>
#include <fxcg/system.h>

#include "resources.h"
#include "graphic_functions.h"
#include "screen.h"
#include "renderer.h"
#include "rng.h"
#include "Game.h"

Game game;

void OnQuit() {
	game.Save();
}

int main() {
	init_xorshift128plus();

	Bdisp_EnableColor(1);
	Bdisp_AllClr_VRAM();
	DrawFrame(COLOR_BG);

	RenderBackground();

	game = Game();
	bool left, up, right, down, moved = true;

	SetQuitHandler(OnQuit);

	int k;
	while (true) {
		if (moved) {
			game.GetAvailableMoves(&left, &up, &right, &down);
			game.Render();

			if (game.ShowLoseDialog()) {
				RenderBoardOverlay(COLOR_LOSE_DIALOG, ALPHA_LOSE_DIALOG);
				CopySpriteAlpha(GAME_OVER_TEXTURE, GAME_OVER_TEXTURE_ALPHA, RECT(GAME_OVER));

				RenderRoundedRect2Alpha(RECT(TRY_AGAIN_EXE_BUTTON), COLOR_DIALOG_BUTTON);
				VRAM_CopySprite(TRY_AGAIN_EXE_TEXTURE, RECT(TRY_AGAIN_EXE));

				k = KEY_CTRL_NOP;
				while(k != KEY_CTRL_EXE)
					GetKey(&k);

				RenderBoard();

				game.Invalidate();
				game.Restart();

				game.GetAvailableMoves(&left, &up, &right, &down);
				game.Render();
			}
			else if (game.ShowWinDialog()) {
				RenderBoardOverlay(COLOR_WIN_DIALOG, ALPHA_WIN_DIALOG);

				CopySpriteAlpha(YOU_WIN_TEXTURE, YOU_WIN_TEXTURE_ALPHA, RECT(YOU_WIN));

				RenderRoundedRect2Alpha(RECT(KEEP_GOING_EXE_BUTTON), COLOR_DIALOG_BUTTON);
				RenderRoundedRect2Alpha(RECT(TRY_AGAIN_EXIT_BUTTON), COLOR_DIALOG_BUTTON);

				VRAM_CopySprite(KEEP_GOING_EXE_TEXTURE, RECT(KEEP_GOING_EXE));
				VRAM_CopySprite(TRY_AGAIN_EXIT_TEXTURE, RECT(TRY_AGAIN_EXIT));

				k = KEY_CTRL_NOP;
				while (k != KEY_CTRL_EXE && k != KEY_CTRL_EXIT)
					GetKey(&k);

				RenderBoard();

				game.Invalidate();

				if (k == KEY_CTRL_EXIT) {
					game.Restart();
					game.GetAvailableMoves(&left, &up, &right, &down);
				}
				else
					game.KeepPlaying();

				game.Render();
			}

			moved = false;
		}

		GetKey(&k);

		if (k == KEY_CTRL_LEFT && left) { game.MoveLeft(); moved = true; }
		if (k == KEY_CTRL_UP && up) { game.MoveUp(); moved = true; }
		if (k == KEY_CTRL_RIGHT && right) { game.MoveRight(); moved = true; }
		if (k == KEY_CTRL_DOWN && down) { game.MoveDown(); moved = true; }
	}

	return 0;
}
