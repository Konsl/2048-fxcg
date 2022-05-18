#pragma once
#include "vsdefs.h"

#include "Game.h"
#include "stdint.h"

#define DIRNAME (unsigned char*)"@2048"
#define SAVEFILENAME (unsigned char*)"Save"
#define BACKUPFILENAME (unsigned char*)"Backup"

enum class SaveFileVersion : uint32_t {
	VERSION_INVALID = 0,
	VERSION_1_0_0 = 1,
	VERSION_1_0_2 = 2
};

struct Version0100Data {
	unsigned int highScore;
	State state;
} __attribute__((packed));

struct Version0102Data {
	Version0100Data v0100;
	unsigned char undoCount;
	signed char undoIndex;
	State undoData[5];
} __attribute__((packed));

const uint32_t VERSION_FILE_SIZE_MAPPING[] = {
	0,									// VERSION_INVALID
	sizeof(Version0100Data),			// VERSION_1_0_0
	sizeof(Version0102Data),			// VERSION_1_0_2
};

class SaveFile
{
public:
	static bool TryLoad(Game& game);
	static void Save(Game& game);

	static void CreateBackup();

private:
	static bool TryLoadV0100(Game& game, Version0100Data* data);
	static bool TryLoadV0102(Game& game, Version0102Data* data);

public:
	static const SaveFileVersion CurrentVersion;
};
