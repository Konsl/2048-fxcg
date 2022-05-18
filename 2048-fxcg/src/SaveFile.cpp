#include "SaveFile.h"
#include "tiles.h"

#include <fxcg/file.h>
#include <alloca.h>
#include <string.h>

const SaveFileVersion SaveFile::CurrentVersion = SaveFileVersion::VERSION_1_0_2;

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
		return TryLoadV0100(game, (Version0100Data*)(buf + 4));
	case SaveFileVersion::VERSION_1_0_2:
		return TryLoadV0102(game, (Version0102Data*)(buf + 4));

	default:
		return false;
	}
}

void SaveFile::Save(Game& game)
{
	unsigned char buf[4 + sizeof(Version0102Data)] = { 0 };
	Version0102Data* data = (Version0102Data*)(buf + 4);
	memcpy(buf, &CurrentVersion, 4);

	// Version 01.00 fields
	data->v0100.highScore = game.highScore;
	memcpy(&data->v0100.state, &game.state, sizeof(State));

	// Version 01.02 fields
	data->undoCount = game.undoCount;
	data->undoIndex = game.undoIndex;

	memcpy(data->undoData, game.undoData, 5 * sizeof(State));

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

bool SaveFile::TryLoadV0100(Game& game, Version0100Data* data)
{
	game.highScore = data->highScore;
	memcpy(&game.state, &data->state, sizeof(State));

	game.freeCells = 0;
	for (unsigned char x = 0; x < 4; x++)
		for (unsigned char y = 0; y < 4; y++) {
			if (!game.state.grid[y * 4 + x])
				game.freeCells++;

			if (game.state.grid[y * 4 + x] > TILE_MAX)
				return false;
		}

	return true;
}

bool SaveFile::TryLoadV0102(Game& game, Version0102Data* data)
{
	if (!TryLoadV0100(game, &data->v0100))
		return false;

	game.undoCount = data->undoCount;
	game.undoIndex = data->undoIndex;

	for (unsigned char i = 0; i < 5; i++) {
		memcpy(&game.undoData[i], &data->undoData[i], sizeof(State));

		for (unsigned char x = 0; x < 4; x++)
			for (unsigned char y = 0; y < 4; y++)
				if (game.undoData[i].grid[y * 4 + x] > TILE_MAX)
					return false;
	}

	return true;
}
