#include "audio.h"
// #include "AudioFileSourceFunction.h" // Disabled due to compatibility issues
// #include "AudioGeneratorMIDI.h" // Disabled due to compatibility issues
// #include "AudioGeneratorWAV.h" // Disabled due to compatibility issues
// #include "AudioGeneratorAAC.h" // Disabled due to compatibility issues
// #include "AudioGeneratorFLAC.h" // Disabled due to compatibility issues
// #include "AudioOutputI2SNoDAC.h" // Disabled due to compatibility issues
#include "core/mykeyboard.h"
// #include <ESP8266Audio.h> // Disabled due to compatibility issues
// #include <ESP8266SAM.h> // Disabled due to compatibility issues

// Audio functionality disabled for ESP32-3248S035R compatibility

#if defined(HAS_NS4168_SPKR)

bool playAudioFile(FS *fs, String filepath) {
    // Audio functionality disabled for ESP32-3248S035R compatibility
    return false;
}

bool playAudioRTTTLString(String song) {
    // Audio functionality disabled for ESP32-3248S035R compatibility
    return false;
}

bool tts(String text) {
    // Audio functionality disabled for ESP32-3248S035R compatibility
    return false;
}

bool isAudioFile(String filepath) {

    return filepath.endsWith(".opus") || filepath.endsWith(".rtttl") || filepath.endsWith(".wav") ||
           filepath.endsWith(".mod") || filepath.endsWith(".mp3");
}

void playTone(unsigned int frequency, unsigned long duration, short waveType) {
    // Audio functionality disabled for ESP32-3248S035R compatibility
}

#endif

void _tone(unsigned int frequency, unsigned long duration) {
    if (!bruceConfig.soundEnabled) return;

#if defined(BUZZ_PIN)
    tone(BUZZ_PIN, frequency, duration);
#elif defined(HAS_NS4168_SPKR)
    //  alt. implementation using the speaker
    playTone(frequency, duration, 0);
#endif
}
