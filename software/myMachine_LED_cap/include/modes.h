#pragma once

enum class AppMode : uint8_t {
    DRAWING = 0,
    SOS     = 1,
    BLINKR  = 2
};

// Nálady pro DRAWING mód – každá odpovídá barvě LED pásku
enum class Mood : uint8_t {
    SMUTNY     = 0,   // modrá
    VESELY     = 1,   // žlutá
    NASTVANY   = 2,   // červená
    PREKVAPENY = 3,   // oranžová
    VYDESENY   = 4,   // zelená
    NADSENY    = 5,   // fialová
    MOOD_COUNT = 6
};

extern AppMode g_mode;
extern Mood    g_mood;
