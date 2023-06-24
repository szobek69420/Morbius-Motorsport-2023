#include "audiomanager.h"
#include <irrKlang/irrKlang.h>
#include <iostream>

static const size_t SOUND_COUNT = 13;

static const char* soundNames[SOUND_COUNT]={
	"_audio/bgmusic_1.mp3",
	"_audio/bgmusic_1_quiet.mp3",
	"_audio/bgmusic_2.mp3",
	"_audio/bgmusic_3.mp3",
	"_audio/bgmusic_4.mp3",
	"_audio/bgmusic_5.mp3",
	"_audio/block_break.mp3",
	"_audio/block_place.mp3",
	"_audio/gunshot.mp3",
	"_audio/jump.mp3",
	"_audio/god_mode_initiate.mp3",
	"_audio/god_mode_abort.mp3",
	"_audio/water.mp3"
};


AudioManager::AudioManager()
{
	using irrklang::createIrrKlangDevice;
	SE = createIrrKlangDevice();

	if (SE != NULL)
	{
		irrklang::ISound* temp;
		for (size_t i = 0; i < SOUND_COUNT; i++)
		{
			temp = SE->play2D(soundNames[i], false, false, true);
			temp->stop();
			temp->drop();
		}
	}

	sounds_started = gigachad::vector<irrklang::ISound*>();
}

//a destruktorban leallitja a meg jatszodo hangokat es torli a maradvanyokat
AudioManager::~AudioManager()
{
	int length = sounds_started.size();
	for (int i = 0; i < length; i++)
	{
		sounds_started[i]->stop();
		sounds_started[i]->drop();
	}

	sounds_started.clear();
	sounds_started.resize(0);
}

Sound AudioManager::PlaySFX(enum sounds effect, bool looped)
{
	if (SE == NULL)
		return Sound(NULL, AM_FAILED, this);

	sounds_started.push_back(SE->play2D(soundNames[(size_t)effect], looped, false, true));
	return Sound(*(--sounds_started.end()), effect, this);

#pragma region OLD_PLAYSFX_FUNCTION

	/*switch (effect)
	{
	case AM_MUSIC_1:
		sounds_started.push_back(SE->play2D("bgmusic_1.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_MUSIC_1, this);
		break;

	case AM_MUSIC_1_QUIET:
		sounds_started.push_back(SE->play2D("bgmusic_1_quiet.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_MUSIC_1_QUIET, this);
		break;

	case AM_MUSIC_2:
		sounds_started.push_back(SE->play2D("bgmusic_2.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_MUSIC_2, this);
		break;

	case AM_MUSIC_3:
		sounds_started.push_back(SE->play2D("bgmusic_3.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_MUSIC_3, this);
		break;

	case AM_MUSIC_4:
		sounds_started.push_back(SE->play2D("bgmusic_4.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_MUSIC_4, this);
		break;

	case AM_MUSIC_5:
		sounds_started.push_back(SE->play2D("bgmusic_5.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_MUSIC_5, this);
		break;

	case AM_GUNSHOT:
		sounds_started.push_back(SE->play2D("gunshot.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_GUNSHOT, this);
		break;

	case AM_BLOCK_BREAK:
		sounds_started.push_back(SE->play2D("block_break.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_BLOCK_BREAK, this);
		break;

	case AM_BLOCK_PLACE:
		sounds_started.push_back(SE->play2D("block_place.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_BLOCK_PLACE, this);
		break;

	case AM_JUMP:
		sounds_started.push_back(SE->play2D("jump.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_JUMP, this);
		break;

	case AM_GODMODE_INITIATE:
		sounds_started.push_back(SE->play2D("god_mode_initiate.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_GODMODE_INITIATE, this);
		break;

	case AM_GODMODE_ABORT:
		sounds_started.push_back(SE->play2D("god_mode_abort.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_GODMODE_ABORT, this);
		//std::cout << "sus abortusz\n";
		break;

	case AM_WATER:
		sounds_started.push_back(SE->play2D("water.mp3", looped, false, true));
		return Sound(*(--sounds_started.end()), AM_WATER, this);
		break;
	}*/

#pragma endregion
}