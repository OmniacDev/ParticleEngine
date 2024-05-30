#pragma once


#include "../Vector/Vector.h"

class Rect {
public:
    FVector2 Position;
    FVector2 Size;

    explicit Rect(const FVector2& Position = FVector2(0.f, 0.f), const FVector2& Size = FVector2(1.f, 1.f)) : Position(Position), Size(Size) {}

    [[nodiscard]] bool Contains(const Rect& R) const {
        return ((R.Position.X >= Position.X) && (R.Position.Y >= Position.Y)) && (((R.Position.X + R.Size.X) < (Position.X + Size.X)) && ((R.Position.Y + R.Size.Y) < (Position.Y + Size.Y)));
    }

    [[nodiscard]] bool Contains(const FVector2& P) const {
        return ((P.X >= Position.X) && (P.Y >= Position.Y)) && ((P.X < (Position.X + Size.X)) && (P.Y < (Position.Y + Size.Y)));
    }

    [[nodiscard]] bool Overlaps(const Rect& R) const {
        return (((Position.X < (R.Position.X + R.Size.X)) && (R.Position.X <= (Position.X + Size.X))) && ((Position.Y < (R.Position.Y + R.Size.Y)) && (R.Position.Y <= (Position.Y + Size.Y))));
    }
};
