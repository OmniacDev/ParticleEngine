#pragma once

#include "../Vector/Vector.h"

namespace RECT_PROF {
    inline int OVERLAP_TESTS = 0;
    inline int CONTAIN_TESTS = 0;
}

class Rect {
public:
    FVector2 Position;
    FVector2 Size;

    Rect(const FVector2& Position, const FVector2& Size) : Position(Position), Size(Size) {}
    Rect() : Position(0.f, 0.f), Size(0.f, 0.f) {};

    [[nodiscard]] bool Contains(const Rect& R) const {
        RECT_PROF::CONTAIN_TESTS++;
        return ((R.Position.X >= Position.X) && (R.Position.Y >= Position.Y)) && (((R.Position.X + R.Size.X) <= (Position.X + Size.X)) && ((R.Position.Y + R.Size.Y) <= (Position.Y + Size.Y)));
    }

    [[nodiscard]] bool Contains(const FVector2& P) const {
        RECT_PROF::CONTAIN_TESTS++;
        return ((P.X >= Position.X) && (P.Y >= Position.Y)) && ((P.X <= (Position.X + Size.X)) && (P.Y <= (Position.Y + Size.Y)));
    }

    [[nodiscard]] bool Overlaps(const Rect& R) const {
        RECT_PROF::OVERLAP_TESTS++;
        return (((Position.X <= (R.Position.X + R.Size.X)) && (R.Position.X <= (Position.X + Size.X))) && ((Position.Y <= (R.Position.Y + R.Size.Y)) && (R.Position.Y <= (Position.Y + Size.Y))));
    }

    static Rect Combine(const Rect& One, const Rect& Two) {
        const float PosX = One.Position.X < Two.Position.X ? One.Position.X : Two.Position.X;
        const float PosY = One.Position.Y < Two.Position.Y ? One.Position.Y : Two.Position.Y;

        const float SizeX = (One.Position.X + One.Size.X > Two.Position.X + Two.Size.X ? One.Position.X + One.Size.X : Two.Position.X + Two.Size.X) - PosX;
        const float SizeY = (One.Position.Y + One.Size.Y > Two.Position.Y + Two.Size.Y ? One.Position.Y + One.Size.Y : Two.Position.Y + Two.Size.Y) - PosY;

        const FVector2 CombinedPosition (PosX, PosY);
        const FVector2 CombinedSize (SizeX, SizeY);

        return {CombinedPosition, CombinedSize};
    }

    static sf::RectangleShape DrawRect(const Rect& R, const sf::Color& Colour, const bool Full = false) {
        sf::RectangleShape rect(sf::Vector2(R.Size.X, R.Size.Y));
        rect.setPosition(R.Position.X, R.Position.Y);
        if (Full) {
            rect.setFillColor({Colour.r, Colour.g, Colour.b, Colour.a});
        }
        else {
            rect.setFillColor({Colour.r, Colour.g, Colour.b, 127});
            rect.setOutlineThickness(-1.0f);
            rect.setOutlineColor({Colour.r, Colour.g, Colour.b, 255});
        }
        return rect;
    }

    static FVector2 MidPoint(const Rect& R) {
        return FVector2 (R.Position + (R.Size/2));
    }
};


