#pragma once

#include "raylib.h"

#include "Math/Vector/Vector.h"

namespace ENGINE {
    inline const int WindowWidth = 1440;
    inline const int WindowHeight = 720;
    inline const int WindowFPS = 180;
    inline const int WindowMinFPS = 10;

    inline const auto ConfigFlags = FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE; // FLAG_WINDOW_ALWAYS_RUN | FLAG_WINDOW_UNDECORATED
}

namespace GRID {
    inline const int Size = 32;
    inline const Color Color = { 80, 80, 80, 100 };
    inline const IVector2 Origin(0, 0);
    // inline const IVector2 Amount((int)(ENGINE::WindowWidth / GRID::Size) + 2, (int)(ENGINE::WindowHeight / GRID::Size) + 2);
    inline const IVector2 Amount(32, 18);
}

namespace BOUNDS {
    inline const float X_POS = ((float)GRID::Amount.X / 2.0f) * (float)GRID::Size;
    inline const float X_NEG = ((float)GRID::Amount.X / 2.0f) * -(float)GRID::Size;
    inline const float Y_POS = ((float)GRID::Amount.Y / 2.0f) * (float)GRID::Size;
    inline const float Y_NEG = ((float)GRID::Amount.Y / 2.0f) * -(float)GRID::Size;
}


