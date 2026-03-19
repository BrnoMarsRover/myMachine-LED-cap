#pragma once

enum class AppMode : uint8_t {
    DRAWING = 0,
    POLICE  = 1,
    ACCEL   = 2
};

extern AppMode g_mode;
