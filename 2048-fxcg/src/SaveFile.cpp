#include "SaveFile.h"
#include "tiles.h"

#include <fxcg/file.h>
#include <alloca.h>
void* __builtin_alloca(unsigned int size);
#include <string.h>

bool SaveFile::TryLoad(Game& game)
{
	int fileSize;
	if (MCSGetDlen2(DIRNAME, SAVEFILENAME, &fileSize)) return false;
	if (fileSize < 4) return false;

	unsigned char* buf = (unsigned char*)alloca(fileSize);
	if (MCSGetData1(0, fileSize, buf)) return false;

	SaveFileVersion version = SaveFileVersion::VERSION_INVALID;
	memcpy(&version, buf, 4);
	if (version == SaveFileVersion::VERSION_INVALID || version > CurrentVersion) return false;

	uint32_t dataSize = VERSION_FILE_SIZE_MAPPING[(uint32_t)version];
	if ((uint32_t)fileSize < dataSize + 4) return false;

	switch (version) {
	case SaveFileVersion::VERSION_1_0_0:
		return TryLoadV0100(game, buf + 4);

	default:
		return false;
	}
}

void SaveFile::Save(Game& game)
{
	unsigned char buf[4 + 27];
	SaveFileVersion version = SaveFileVersion::VERSION_1_0_0;
	memcpy(buf, &version, 4);

	memcpy(buf + 4, &game.highScore, 4);
	memcpy(buf + 8, &game.score, 4);
	memcpy(buf + 12, game.grid, 16);
	memcpy(buf + 28, &game.won, 1);
	memcpy(buf + 29, &game.lost, 1);
	memcpy(buf + 30, &game.keepPlaying, 1);

	if (MCS_CreateDirectory(DIRNAME))
		MCSDelVar2(DIRNAME, SAVEFILENAME);
	MCSPutVar2(DIRNAME, SAVEFILENAME, sizeof buf, buf);
}

void SaveFile::CreateBackup()
{
	int fileSize;
	if (MCSGetDlen2(DIRNAME, SAVEFILENAME, &fileSize)) return;

	unsigned char* buf = (unsigned char*)alloca(fileSize);
	if (MCSGetData1(0, fileSize, buf)) return;
	
	if (MCS_CreateDirectory(DIRNAME))
		MCSDelVar2(DIRNAME, BACKUPFILENAME);
	MCSPutVar2(DIRNAME, BACKUPFILENAME, fileSize, buf);
}

bool SaveFile::TryLoadV0100(Game& game, unsigned char* buf)
{
	memcpy(&game.highScore, buf, 4);
	memcpy(&game.score, buf + 4, 4);
	memcpy(game.grid, buf + 8, 16);
	memcpy(&game.won, buf + 24, 1);
	memcpy(&game.lost, buf + 25, 1);
	memcpy(&game.keepPlaying, buf + 26, 1);

	game.freeCells = 0;
	for (unsigned char x = 0; x < 4; x++)
		for (unsigned char y = 0; y < 4; y++) {
			if (!game.grid[y * 4 + x])
				game.freeCells++;

			if (game.grid[y * 4 + x] > TILE_MAX)
				return false;
		}

	return true;
}
