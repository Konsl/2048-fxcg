#include "Game.h"

#include <string.h>
#include <fxcg/file.h>

#include "renderer.h"
#include "rng.h"
#include "tiles.h"
#include "graphic_functions.h"
#include "resources.h"
#include "SaveFile.h"

Game::Game() : mergeTable(0), undoCount(0), undoIndex(0), valid(false), scoreTextWidth(0), scoreTextValid(false), highScoreTextWidth(0), highScoreTextValid(false)
{
	ClearUndoData();

	if (!TryLoad()) {
		SaveFile::CreateBackup();

		freeCells = 16;
		state.score = highScore = 0;
		state.won = state.lost = state.keepPlaying = false;

		for (unsigned char x = 0; x < 4; x++)
			for (unsigned char y = 0; y < 4; y++) {
				unsigned char index = y * 4 + x;

				state.grid[index] = TILE_0;
				renderedGrid[index] = 0;
			}

		ClearUndoData();

		AddRandomTile();
		AddRandomTile();
	}
}

unsigned char* Game::GetGrid()
{
	return state.grid;
}

unsigned char Game::GetFreeCells()
{
	return freeCells;
}

void Game::Render()
{
	for (unsigned char x = 0; x < 4; x++)
		for (unsigned char y = 0; y < 4; y++) {
			unsigned char index = y * 4 + x;

			if (!valid || (state.grid[index] != renderedGrid[index])) {
				renderedGrid[index] = state.grid[index];

				RenderTile(x, y, state.grid[index]);
			}
		}
	valid = true;

	if (!scoreTextValid) {
		FillRectangle(48 - scoreTextWidth / 2, 74, scoreTextWidth, SCORE_FONT_HEIGHT, COLOR_FIELD);

		unsigned int val = state.score;

		unsigned char buf[10] = { 0 };
		unsigned char i = 9;

		do {
			buf[i--] = val % 10;
			val /= 10;
		} while (val > 0);

		i++;

		unsigned int textWidth = GetScoreTextWidth(buf + i, 10 - i);
		RenderScoreText(48 - textWidth / 2, 74, buf + i, 10 - i);

		scoreTextWidth = textWidth;
		scoreTextValid = true;
	}

	if (!highScoreTextValid) {
		FillRectangle(48 - highScoreTextWidth / 2, 107, highScoreTextWidth, SCORE_FONT_HEIGHT, COLOR_FIELD);

		unsigned int val = highScore;

		unsigned char buf[10] = { 0 };
		unsigned char i = 9;

		do {
			buf[i--] = val % 10;
			val /= 10;
		} while (val > 0);

		i++;

		unsigned int textWidth = GetScoreTextWidth(buf + i, 10 - i);
		RenderScoreText(48 - textWidth / 2, 107, buf + i, 10 - i);

		highScoreTextWidth = textWidth;
		highScoreTextValid = true;
	}
}

void Game::Invalidate()
{
	valid = false;
}

void Game::InvalidateScore()
{
	scoreTextValid = false;
}

void Game::InvalidateHighScore()
{
	highScoreTextValid = false;
}

bool Game::ShowWinDialog()
{
	return state.won && !state.keepPlaying;
}

bool Game::ShowLoseDialog()
{
	return state.lost;
}

void Game::Restart()
{
	ClearMergeTable();
	InvalidateScore();
	InvalidateHighScore();

	for (unsigned char x = 0; x < 4; x++)
		for (unsigned char y = 0; y < 4; y++)
			state.grid[y * 4 + x] = 0;

	freeCells = 16;
	state.score = 0;
	state.won = state.lost = state.keepPlaying = false;

	AddRandomTile();
	AddRandomTile();
}

void Game::KeepPlaying()
{
	state.keepPlaying = true;
}

bool Game::CanUndo()
{
	return undoCount > 0;
}

bool Game::Undo()
{
	if (!CanUndo())
		return false;

	memcpy(&state, &undoData[undoIndex], sizeof(State));

	freeCells = 0;
	for (unsigned char x = 0; x < 4; x++)
		for (unsigned char y = 0; y < 4; y++)
			if (!state.grid[y * 4 + x])
				freeCells++;

	undoCount--;
	undoIndex = (undoIndex + 4) % 5;

	InvalidateScore();

	return true;
}

void Game::CreateUndoPoint()
{
	if (undoCount < 5)
		undoCount++;

	undoIndex = (undoIndex + 1) % 5;

	memcpy(&undoData[undoIndex], &state, sizeof(State));
}

void Game::ClearUndoData()
{
	undoCount = 0;
	undoIndex = 0;

	for (unsigned char i = 0; i < 5; i++) {
		for (unsigned char j = 0; j < 16; j++)
			undoData[i].grid[j] = TILE_0;

		undoData[i].score = 0;
		undoData[i].won = undoData[i].lost = undoData[i].keepPlaying = false;
	}
}

void Game::AddRandomTile()
{
	if (freeCells) {
		unsigned char d = (unsigned char)(xorshift128plus() % freeCells);

		for (unsigned char i = 0; i < 16; i++)
			if (!state.grid[i]) {
				if (d)
					d--;
				else {
					unsigned char type = (unsigned char)(xorshift128plus() % 10) == 0 ? TILE_4 : TILE_2;
					state.grid[i] = type;
					freeCells--;

					break;
				}
			}
	}
}

void Game::GetAvailableMoves(bool* left, bool* up, bool* right, bool* down)
{
	*left = *up = *right = *down = false;

	for (unsigned char x = 0; x < 4; x++)
		for (unsigned char y = 0; y < 4; y++) {
			unsigned char v = state.grid[y * 4 + x];

			if (v) {
				if (left && x > 0) {
					unsigned char vLeft = state.grid[y * 4 + (x - 1)];

					if (!vLeft || vLeft == v)
						*left = true;
				}
				if (up && y > 0) {
					unsigned char vTop = state.grid[(y - 1) * 4 + x];

					if (!vTop || vTop == v)
						*up = true;
				}
				if (right && x < 3) {
					unsigned char vRight = state.grid[y * 4 + (x + 1)];

					if (!vRight || vRight == v)
						*right = true;
				}
				if (down && y < 3) {
					unsigned char vBottom = state.grid[(y + 1) * 4 + x];

					if (!vBottom || vBottom == v)
						*down = true;
				}
			}
		}

	if (*left == false && *up == false && *right == false && *down == false)
		state.lost = true;
}

void Game::MoveLeft()
{
	CreateUndoPoint();
	ClearMergeTable();

	for (unsigned char x = 0; x < 4; x++)
		for (unsigned char y = 0; y < 4; y++) {
			unsigned char v = state.grid[y * 4 + x];

			if (v) {
				unsigned char move = 0;
				for (; move < x; move++) {
					unsigned char other = y * 4 + (x - move - 1);
					unsigned char otherV = state.grid[other];

					if (otherV && (otherV != v || IsMerged(other)))
						break;
				}

				Move(y * 4 + x, y * 4 + (x - move));
			}
		}

	AddRandomTile();
}

void Game::MoveUp()
{
	CreateUndoPoint();
	ClearMergeTable();

	for (unsigned char x = 0; x < 4; x++)
		for (unsigned char y = 0; y < 4; y++) {
			unsigned char v = state.grid[y * 4 + x];

			if (v) {
				unsigned char move = 0;
				for (; move < y; move++) {
					unsigned char other = (y - move - 1) * 4 + x;
					unsigned char otherV = state.grid[other];

					if (otherV && (otherV != v || IsMerged(other)))
						break;
				}

				Move(y * 4 + x, (y - move) * 4 + x);
			}
		}

	AddRandomTile();
}

void Game::MoveRight()
{
	CreateUndoPoint();
	ClearMergeTable();

	for (signed char x = 3; x >= 0; x--)
		for (unsigned char y = 0; y < 4; y++) {
			unsigned char v = state.grid[y * 4 + x];

			if (v) {
				unsigned char move = 0;
				for (; move < 3 - x; move++) {
					unsigned char other = y * 4 + (x + move + 1);
					unsigned char otherV = state.grid[other];

					if (otherV && (otherV != v || IsMerged(other)))
						break;
				}

				Move(y * 4 + x, y * 4 + (x + move));
			}
		}

	AddRandomTile();
}

void Game::MoveDown()
{
	CreateUndoPoint();
	ClearMergeTable();

	for (unsigned char x = 0; x < 4; x++)
		for (signed char y = 3; y >= 0; y--) {
			unsigned char v = state.grid[y * 4 + x];

			if (v) {
				unsigned char move = 0;
				for (; move < 3 - y; move++) {
					unsigned char other = (y + move + 1) * 4 + x;
					unsigned char otherV = state.grid[other];

					if (otherV && (otherV != v || IsMerged(other)))
						break;
				}

				Move(y * 4 + x, (y + move) * 4 + x);
			}
		}

	AddRandomTile();
}

void Game::Move(unsigned char src, unsigned char dest)
{
	if (src != dest) {
		if (state.grid[src] == state.grid[dest]) {
			state.grid[dest] = state.grid[src] + 1;

			SetMerged(dest);
			freeCells++;

			IncrementScore(1 << state.grid[dest]);

			if (state.grid[dest] == TILE_2048)
				state.won = true;
		}
		else
			state.grid[dest] = state.grid[src];

		state.grid[src] = TILE_0;
	}
}

void Game::IncrementScore(unsigned int amount)
{
	state.score += amount;
	InvalidateScore();

	if (state.score > highScore) {
		highScore = state.score;
		InvalidateHighScore();
	}
}

void Game::ClearMergeTable()
{
	mergeTable = 0;
}

bool Game::IsMerged(unsigned char index)
{
	return mergeTable & (1 << index);
}

void Game::SetMerged(unsigned char index)
{
	mergeTable |= (1 << index);
}

void Game::Save()
{
	SaveFile::Save(*this);
}

bool Game::TryLoad()
{
	return SaveFile::TryLoad(*this);
}
