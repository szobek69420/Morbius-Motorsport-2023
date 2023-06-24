#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H
#include <irrKlang/irrKlang.h>
#include <vector>
#include "my_vector.hpp"

//a lejatszhato hangok nevet tartalmazo enumerator
enum sounds {
	AM_MUSIC_1,
	AM_MUSIC_1_QUIET,
	AM_MUSIC_2,
	AM_MUSIC_3,
	AM_MUSIC_4,
	AM_MUSIC_5,
	AM_BLOCK_BREAK,
	AM_BLOCK_PLACE,
	AM_GUNSHOT,
	AM_JUMP,
	AM_GODMODE_INITIATE,
	AM_GODMODE_ABORT,
	AM_WATER,


	AM_FAILED
};

class AudioManager;
class Sound;


class AudioManager {
private :
	//az irrklang keretrendszer mukodesehez szukseges osztalyra mutato pointer
	irrklang::ISoundEngine* SE;
	//az ebben az AudioManager peldanyon keresztul lejatszott hangok listaja (hogy kesobb ki tudjam oket kapcsolni)
	gigachad::vector<irrklang::ISound*> sounds_started;
public:
	AudioManager();
	~AudioManager();
	Sound PlaySFX(enum sounds effect, bool looped = false);
};

class Sound {

	friend AudioManager;

private:
	irrklang::ISound* pSound;//az a hangeffekt, ami hozza tartozik
	enum sounds sound_t;//milyen hangeffekt volt az
	AudioManager* audioManager;//melyik audiomanager jatszotta le (a replayhez)

private:
	Sound(irrklang::ISound* _pSound, enum sounds _sound_t, AudioManager* _audioManager)
		: pSound(_pSound), sound_t(_sound_t), audioManager(_audioManager) {}

public:
	~Sound() {}

	bool isFinished() { if (sound_t == AM_FAILED) return true; else return pSound->isFinished(); }
	void Replay() { if(sound_t!=AM_FAILED) pSound = (audioManager->PlaySFX(sound_t)).pSound; }
	void Stop() { if(sound_t!=AM_FAILED) pSound->stop(); }
};

#endif