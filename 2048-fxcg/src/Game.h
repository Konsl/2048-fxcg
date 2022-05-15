#pragma once

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
	unsigned char grid[16];
	unsigned short mergeTable;

	unsigned char renderedGrid[16];
	bool valid;

	unsigned int scoreTextWidth;
	bool scoreTextValid;
	unsigned int highScoreTextWidth;
	bool highScoreTextValid;

	unsigned char freeCells;

	unsigned int score;
	unsigned int highScore;

	bool won;
	bool lost;
	bool keepPlaying;

	friend class SaveFile;
};