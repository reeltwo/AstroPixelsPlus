#include "DFRobotDFPlayerMini.h"
/***********************************************************
 *  MP3sound.c
 *  MarcDuino interface to play sounds from an MP3Trigger board
 *
 *  Created on: Sep 17, 2013
 *  Author: Marc Verdiell
 *  Copyright � 2013 Marc Verdiell, All Rights Reserved
 *
 ***********************************************************/
/***********************************************************
 *  On the MP3, there are a maximum of 255 sound files
 *  They must be named NNN-xxxx.mp3
 *  Where NNN = 001 to 255
 *  The numbering ranges are predetermined, 25 sounds per
 *  bank category
 *       Bank 1: gen sounds, numbered 001 to 025
 *       Bank 2: chat sounds, numbered 026 to 050
 *       Bank 3: happy sounds, numbered 051 to 075
 *       Bank 4: sad sounds, numbered 076 to 100
 *       Bank 5: whistle sounds, numbered 101 to 125
 *       Bank 6: scream sounds, numbered 126 to 150
 *       Bank 7: Leia sounds, numbered 151 to 175
 *       Bank 8: sing sounds (deprecated, not used by R2 Touch)
 *       Bank 9: mus sounds, numbered 176 to 181
 *
 *  The pre-made R2 sound library contains only a few non-copyrighted music sounds.
 *  Sound 202, 203 and 205 are "beeped music" place-holders, meant to be replaced with the
 *  original score of Star Wars, Empire March, and Cantina respectively
 *
 *  If you add your own sounds in Bank 9, make sure you update the last variable
 *  MP3_BANK9_SOUNDS below to reflect the total number of sounds
 *
 ***********************************************************/
#define MP3_MAX_BANKS                9  // nine banks
#define MP3_MAX_SOUNDS_PER_BANK     25  // no more than 25 sound in each
#define MP3_BANK_CUTOFF             4   // cutoff for banks that play "next" sound on $x

// for the random sounds, needs to know max sounds of first 5 banks
// only important for sounds below cutoff
#define MP3_BANK1_SOUNDS 19 // gen sounds, numbered 001 to 025
#define MP3_BANK2_SOUNDS 18 // chat sounds, numbered 026 to 050
#define MP3_BANK3_SOUNDS 7  // happy sounds, numbered 051 to 075
#define MP3_BANK4_SOUNDS 4  // sad sounds, numbered 076 to 100
#define MP3_BANK5_SOUNDS 3  // whistle sounds, numbered 101 to 125
// unless you change bank cutoff, these are ignored, so I set them to max
#define MP3_BANK6_SOUNDS MP3_MAX_SOUNDS_PER_BANK    // scream sounds, numbered 126 to 150
#define MP3_BANK7_SOUNDS MP3_MAX_SOUNDS_PER_BANK    // Leia sounds, numbered 151 to 175
#define MP3_BANK8_SOUNDS MP3_MAX_SOUNDS_PER_BANK    // sing sounds (deprecated, not used by R2 Touch)
#define MP3_BANK9_SOUNDS MP3_MAX_SOUNDS_PER_BANK // mus sounds, numbered 201 t0 225

// this defines where the startup sound is
#define MP3_EMPTY_SOUND 252 // workaround, used to stop sounds

// Defines for the Volume control
// DFPlayer Mini has a different set of values used.
#define DF_VOLUME_MID   15  // guessing mid volume 32 is right in-between...
#define DF_VOLUME_MIN   1   // doc says anything below 64 is inaudible, not true, 100 is. 82 is another good value
#define DF_VOLUME_MAX   30  // doc says max is 0
#define DF_VOLUME_OFF   0 // to turn it off... 255 gets a buzz.

// Defines for the SparkFun MP3 Trigger
#define MP3_VOLUME_MID  50  // guessing mid volume 32 is right in-between...
#define MP3_VOLUME_MIN  100 // doc says anything below 64 is inaudible, not true, 100 is. 82 is another good value
#define MP3_VOLUME_MAX  0   // doc says max is 0
#define MP3_VOLUME_OFF  254 // to turn it off... 255 gets a buzz.

#define MP3_VOLUME_STEPS 20 // R2 Touch app has 20 steps from min to max

#define MP3_PLAY_CMD    't' // command to play sound file on the MP3 trigger
#define MP3_VOLUME_CMD  'v' // command to play sound file on the MP3 trigger
#define MP3_STOP_CMD    'O' // command to stop/play  - not used

#define MP3_MIN_RANDOM_PAUSE 600        // min wait on random sounds
#define MP3_MAX_RANDOM_PAUSE 10000      // max wait on random sounds
#define MP3_MAX_PAUSE_ON_RESUME 12      // default wait to resume random. Works for short sound. Set mp3_random_timer manually for long ones.

/////////////COMMAND VOCABULARY///////////
// Play sound command by bank/sound numbers
// $xyy
// x=bank number
// yy=sound number. If none, next sound is played in the bank
//
// Other commands
// $c
// where c is a command character
// R - random from 4 first banks
// O - sound off
// L - Leia message (bank 7 sound 1)
// C - Cantina music (bank 9 sound 5)
// c - Beep cantina (bank 9 sound 1)
// S - Scream (bank 6 sound 1)
// F - Faint/Short Circuit (bank 6 sound 3)
// D - Disco (bank 9 sound 6)
// s - stop sounds
// + - volume up
// - - volume down
// m - volume mid
// f - volume max
// p - volume min
// W - Star Wars music (bank 9 sound 2)
// M - Imperial March (bank 9 sound 3)
//
///////////////////////////////////////////////

class MarcSound
{
public:
    enum Module
    {
        kDisabled,
        kMP3Trigger,
        kDFMini,
        kHCR
    };

    void idle()
    {
        if (fModule != kDisabled)
        {
            uint32_t currentMillis = millis();
            if (fRandomEnabled && fNextRandomEvent && currentMillis >= fNextRandomEvent)
            {
                playRandom();
                fNextRandomEvent = millis() + random(fRandomMinDelay, fRandomMaxDelay);
            }
        }
    }

    void setRandomMin(uint32_t delay)
    {
        fRandomMinDelay = delay;
    }

    void setRandomMax(uint32_t delay)
    {
        fRandomMaxDelay = delay;
    }

    void playSound(uint8_t bank, uint8_t sound)
    {
        uint8_t filenum;

        if (bank > MP3_MAX_BANKS)
            return;
        if (bank != 0 && sound > MP3_MAX_SOUNDS_PER_BANK)
            return;

        // if bank=0 play the sound number provided
        if (bank == 0)
            filenum = sound;

        else if (sound != 0)
        {
            // calculate actual file number on the MP3 memory card
            filenum = (bank - 1) * MP3_MAX_SOUNDS_PER_BANK + sound;
            // also adjust last sound played index for the next sound command
            // make sure not to go past max sounds
            if (sound > fMaxSounds[bank])
                fBankIndexes[bank] = fMaxSounds[bank];
            else
                fBankIndexes[bank] = sound;
        }
        // sound "0", play first or next sound depending on bank
        else
        {
            if (bank <= MP3_BANK_CUTOFF)
            {
                // advance index, rewind to first sound if at end
                if ((++fBankIndexes[bank]) > fMaxSounds[bank])
                    fBankIndexes[bank] = 1;
                // we'll play the new indexed sound
                sound = fBankIndexes[bank];
            }
            else
            {
                // for banks that always play the first sound
                sound = 1;
            }
            filenum = (bank - 1) * MP3_MAX_SOUNDS_PER_BANK + sound;
        }

        switch (fModule)
        {
            case kDisabled:
                break;
            case kDFMini:
                fDFMini.play(filenum);
                break;
            case kMP3Trigger:
                // send a 't'nnn number where nnn=file number
                sendMP3(MP3_PLAY_CMD);
                sendMP3(filenum);
                break;
            case kHCR:
            {
                char buffer[10];
                snprintf(buffer, sizeof(buffer), "<CA%04d>", filenum);
                switch (bank)
                {
                    case kGenSounds:
                        sendHCR("<SS0>");
                        break;
                    case kChatSounds:
                        sendHCR("<MT><MT>");
                        break;
                    case kHappySounds:
                        if (filenum < fMaxSounds[bank] / 2)
                            sendHCR("<SH0>");
                        else
                            sendHCR("<SH1>");
                        break;
                    case kSadSounds:
                        if (filenum < fMaxSounds[bank] / 2)
                            sendHCR("<SS0>");
                        else
                            sendHCR("<SS1>");
                        break;
                    case kWhistleSounds:
                        sendHCR("<MT><MT>");
                        break;
                    case kScreamSounds:
                        switch (sound)
                        {
                            case 1:
                                sendHCR("<SC0>");
                                break;
                            case 2:
                                sendHCR("<SC1>");
                                break;
                            case 3:
                            default:
                                sendHCR("<SE>");
                                break;
                        }
                        break;
                    case 0:
                    case kLeiaSounds:
                    case kSingSounds:
                    case kMusicSounds:
                        sendHCR(buffer);
                        break;
                }
                break;
            }
        }
    }

    void playRandom()
    {
        if (fModule == kHCR)
        {
            sendHCR("<MT><MT>");
            return;
        }
        uint8_t num;
        // Plays a random sound from the first 5 banks only
        num = random(1, MP3_BANK1_SOUNDS + MP3_BANK2_SOUNDS + MP3_BANK3_SOUNDS + MP3_BANK4_SOUNDS + MP3_BANK5_SOUNDS);
        if(num <= MP3_BANK1_SOUNDS)
        {
            playSound(1, num);
            return;
        }
        num -= MP3_BANK1_SOUNDS;
        if (num <= MP3_BANK2_SOUNDS)
        {
            playSound(2, num);
            return;
        }
        num -= MP3_BANK2_SOUNDS;
        if (num <= MP3_BANK3_SOUNDS)
        {
            playSound(3, num);
            return;
        }
        num -= MP3_BANK3_SOUNDS;
        if (num <= MP3_BANK4_SOUNDS)
        {
            playSound(4, num);
            return;
        }
        num -= MP3_BANK4_SOUNDS;
        if (num <= MP3_BANK5_SOUNDS)
        {
            playSound(5, num);
            return;
        }
    }

    void playStartSound()
    {
        if (fStartupSound != -1)
            playSound(0, fStartupSound);
    }

    void stop()
    {
        switch (fModule)
        {
            case kDisabled:
                break;
            case kDFMini:
                fDFMini.stop();
                break;
            case kMP3Trigger:
                playSound(0, MP3_EMPTY_SOUND);
                break;
            case kHCR:
                sendHCR("<PSG>");
                break;
        }
    }

    inline void startRandom()
    {
        startRandomInSeconds(1);
    }

    void startRandomInSeconds(uint32_t seconds)
    {
        fRandomEnabled = true;
        fNextRandomEvent = millis() + seconds * 1000L;
    }

    void stopRandom()
    {
        fRandomEnabled = true;
    }

    void suspendRandom()
    {
        fRandomEnabledSaved = fRandomEnabled;
        fRandomEnabled = false;
    }

    void resumeRandomInSeconds(uint32_t seconds)
    {
        fRandomEnabled = fRandomEnabledSaved;
        if (fRandomEnabled)
            fNextRandomEvent = millis() + seconds * 1000L;
    }

    inline void resumeRandom()
    {
        resumeRandomInSeconds(MP3_MAX_PAUSE_ON_RESUME);
    }

    void volumeMid()
    {
        setVolume(0.5);
    }

    void volumeOff()
    {
        setVolume(0.0);
    }

    void volumeMax()
    {
        setVolume(1.0);
    }

    void volumeMin()
    {
        setVolume(0.01);
    }

    void volumeUp()
    {
        setVolume(fVolume + (1.0 / MP3_VOLUME_STEPS));
    }

    void volumeDown()
    {
        setVolume(fVolume - (1.0 / MP3_VOLUME_STEPS));
    }

    void setVolume(float volume)
    {
        // clamp value to 0.0 - 1.0
        volume = std::min<float>(volume, 1.0);
        volume = std::max<float>(volume, 0.0);
        fVolume = volume;
        switch (fModule)
        {
            case kDisabled:
                break;
            case kDFMini:
                fDFMini.volume(ceil(volume * DF_VOLUME_MAX));
                break;
            case kMP3Trigger:
                sendMP3(MP3_VOLUME_CMD);
                sendMP3(MP3_VOLUME_MIN - volume * MP3_VOLUME_MIN);
                break;
            case kHCR:
            {
                char buffer[10];
                snprintf(buffer, sizeof(buffer), "<PVV%d>", int(volume * 100));
                sendHCR(buffer);
                break;
            }
        }
    }

    bool begin(Module module, Stream& stream, int startupSound = -1)
    {
        fModule = kDisabled;
        fStartupSound = startupSound;
        switch (module)
        {
            case kDisabled:
                fStream = nullptr;
                break;
            case kDFMini:
                fStream = &stream;
                if (!fDFMini.begin(stream))
                {
                    DEBUG_PRINTLN("Unable to begin:");
                    DEBUG_PRINTLN("1.Please recheck the connection!");
                    DEBUG_PRINTLN("2.Please insert the SD card!");
                    DEBUG_PRINTLN("2.Reboot!");
                    return false;
                }
                fDFMini.EQ(DFPLAYER_EQ_NORMAL);
                break;
            case kMP3Trigger:
                fStream = &stream;
                break;
            case kHCR:
                fStream = &stream;
                break;
        }
        for (uint8_t i=0; i < SizeOfArray(fBankIndexes); i++)
        {
            fBankIndexes[i] = 0;
        }
        fModule = module;
        return true;
    }

    bool handleCommand(const char* cmd, bool skipStart = false)
    {
        ////////////////////////////////////////////////
        // Play sound command by bank/sound numbers
        // $xyy
        // x=bank number
        // yy=sound number. If none, next sound is played in the bank
        //
        // Other commands
        // $c
        // where c is a command character
        // R - random from 4 first banks
        // O - sound off
        // L - Leia message (bank 7 sound 1)
        // C - Cantina music (bank 9 sound 5)
        // c - Beep cantina (bank 9 sound 1)
        // S - Scream (bank 6 sound 1)
        // F - Faint/Short Circuit (bank 6 sound 3)
        // D - Disco (bank 9 sound 6)
        // s - stop sounds
        // + - volume up
        // - - volume down
        // m - volume mid
        // f - volume max
        // p - volume min
        // W - Star Wars music (bank 9 sound 2)
        // M - Imperial March (bank 9 sound 3)
        //
        ///////////////////////////////////////////////
        if (skipStart)
            cmd--;
        uint8_t len = strlen(cmd);
        // check the start character
        if (!skipStart && cmd[0] != '$')
            return false;

        // should have between 2 and 4 characters
        if (len<2 || len>4)
        {
            return false;
        }

        char cmdch = cmd[1];

        // if the command character is a digit, this is a sound play command
        if (isdigit(cmdch))
        {
            stopRandom(); // any manual sound command stops random automatically
            uint8_t bank = (uint8_t)cmdch - '0';
            uint8_t sound = 0;
            if (len > 2)
            {
                sound = atoi(cmd + 2);
            }
            playSound(bank, sound);
            return true;
        }

        // the command is a character
        switch(cmdch)
        {
            case 'R':   // R - random from 4 first banks
                startRandom(); // keep firing random sounds
                break;
            case 'O':   // O - sound off
                stopRandom();
                volumeOff();
                break;
            case 'L':   // L - Leia message (bank 7 sound 1)
                suspendRandom();
                playSound(7,1);
                resumeRandomInSeconds(44); // 34s + 10s extra long delay
                break;
            case 'C':   // C - Cantina music (bank 9 sound 5)
                suspendRandom();
                playSound(8,5);
                resumeRandomInSeconds(56); // extra long delay
                break;
            case 'c':   // c - Beep cantina (bank 9 sound 1)
                suspendRandom();
                playSound(8,1);
                resumeRandomInSeconds(27); // extra long delay
                break;
            case 'S':   // S - Scream (bank 6 sound 1)
                suspendRandom();
                playSound(6,1);
                resumeRandom();
                break;
            case 'F':   // F - Faint/Short Circuit (bank 6 sound 3)
                suspendRandom();
                playSound(6,3);
                resumeRandom();
                break;
            case 'D':   // D - Disco (bank 9 sound 6)
                suspendRandom();
                playSound(8,6);
                resumeRandomInSeconds(40); // 6:26 +10s min extra long delay
                break;
            case 's':   // s - stop sounds
                stopRandom();
                stop();
                break;
            case '+':   // + - volume up
                volumeUp();
                break;
            case '-':   // - - volume down
                volumeDown();
                break;
            case 'm':   // m - volume mid
                volumeMid();
                break;
            case 'f':   // f - volume max
                volumeMax();
                break;
            case 'p':   // p - volume min
                volumeMin();
                break;
            case 'W':   // W - Star Wars music (bank 9 sound 2)
                stopRandom();      // so long, just stop random
                playSound(8,2);
                break;
            case 'M':   // M - Imperial March (bank 9 sound 3)
                stopRandom();      // so long, just stop random
                playSound(8,3);
                break;
            default:
                return false;
        }
        return true;
    }

private:
    DFRobotDFPlayerMini fDFMini;
    Stream* fStream = nullptr;
    float fVolume = 0.5;
    Module fModule = kDisabled;
    bool fRandomEnabled = false;
    bool fRandomEnabledSaved = false;
    uint32_t fNextRandomEvent = 0;
    uint32_t fRandomMinDelay = 600;
    uint32_t fRandomMaxDelay = 10000;
    int fStartupSound = -1;
    // global variables, current indexes to banks
    uint8_t fBankIndexes[MP3_MAX_BANKS];
    const uint8_t fMaxSounds[MP3_MAX_BANKS] =
    {
        MP3_BANK1_SOUNDS,
        MP3_BANK2_SOUNDS,
        MP3_BANK3_SOUNDS,
        MP3_BANK4_SOUNDS,
        MP3_BANK5_SOUNDS,
        MP3_BANK6_SOUNDS,
        MP3_BANK7_SOUNDS,
        MP3_BANK8_SOUNDS,
        MP3_BANK9_SOUNDS
    };
    enum
    {
        kGenSounds = 1,
        kChatSounds = 2,
        kHappySounds = 3,
        kSadSounds = 4,
        kWhistleSounds = 5,
        kScreamSounds = 6,
        kLeiaSounds = 7,
        kSingSounds = 8,
        kMusicSounds = 9
    };

    void sendMP3(uint8_t cmd)
    {
        printf("MP3:0x%02X\n", cmd);
        if (fStream != nullptr)
            fStream->write(cmd);
    }

    void sendHCR(const char* cmd)
    {
        printf("HCR: %s\n", cmd);
        if (fStream != nullptr)
            fStream->print(cmd);
    }
};

MarcSound sMarcSound;

MARCDUINO_ACTION(SoundCommand, $, ({
    const char* cmd = Marcduino::getCommand();
    if (sMarcSound.handleCommand(cmd, true))
    {
        printf("BAD SND CMD: $%s\n", cmd);
    }
}))
