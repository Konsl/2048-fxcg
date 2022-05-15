#pragma once
#include "Game.h"
#include "stdint.h"

#define DIRNAME (unsigned char*)"@2048"
#define SAVEFILENAME (unsigned char*)"Save"
#define BACKUPFILENAME (unsigned char*)"Backup"

enum class SaveFileVersion : uint32_t {
	VERSION_INVALID = 0,
	VERSION_1_0_0 = 1
};

const uint32_t VERSION_FILE_SIZE_MAPPING[] = {
	0, // VERSION_INVALID
	27, // VERSION_1_0_0
};

class SaveFile
{
public:
	static bool TryLoad(Game& game);
	static void Save(Game& game);

	static void CreateBackup();

private:
	static bool TryLoadV0100(Game& game, unsigned char* buf);

public:
	static const SaveFileVersion CurrentVersion = SaveFileVersion::VERSION_1_0_0;
};
