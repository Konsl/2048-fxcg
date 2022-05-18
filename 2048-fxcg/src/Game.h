#pragma once
#include "vsdefs.h"

struct State
{
	unsigned int score;
	unsigned char grid[16];
	bool won;
	bool lost;
	bool keepPlaying;
} __attribute__((packed));

class Game
{
public:
	Game();

	unsigned char* GetGrid();
	unsigned char GetFreeCells();

	void Render();
	void Invalidate();
	void InvalidateScore();
	void InvalidateHighScore();

	bool ShowWinDialog();
	bool ShowLoseDialog();

	void Restart();
	void KeepPlaying();

	bool CanUndo();
	bool Undo();
	void CreateUndoPoint();
	void ClearUndoData();

	void AddRandomTile();
	void GetAvailableMoves(bool* left, bool* up, bool* right, bool* down);

	void MoveLeft();
	void MoveUp();
	void MoveRight();
	void MoveDown();

	void Move(unsigned char src, unsigned char dest);

	void IncrementScore(unsigned int amount);

	void ClearMergeTable();
	bool IsMerged(unsigned char index);
	void SetMerged(unsigned char index);

	void Save();
	bool TryLoad();

private:
	State state;
	unsigned short mergeTable;

	State undoData[5];
	unsigned char undoCount;
	signed char undoIndex;

	unsigned char renderedGrid[16];
	bool valid;

	unsigned int scoreTextWidth;
	bool scoreTextValid;
	unsigned int highScoreTextWidth;
	bool highScoreTextValid;

	unsigned char freeCells;

	unsigned int highScore;

	friend class SaveFile;
};