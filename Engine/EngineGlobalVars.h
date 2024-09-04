#pragma once

#include <SFML/Graphics.hpp>

#include "Math/Vector/Vector.h"

namespace ENGINE {
    inline const int WindowWidth = 1440;
    inline const int WindowHeight = 720;
    inline const int WindowFPS = 180;
    inline const int WindowMinFPS = 10;
}

namespace GRID {
    inline const int Size = 32;
    inline const sf::Color Color = { 80, 80, 80, 100 };
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


