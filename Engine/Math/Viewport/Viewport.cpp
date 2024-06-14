#include "Viewport.h"

void VIEWPORT::TranslateViewport(IVector2 V) {
    VIEWPORT::Offset += V;
}

IVector2 VIEWPORT::WorldToViewport(IVector2 V) {
    return (V * VIEWPORT::AxisDirections) + VIEWPORT::Offset;
}

IVector2 VIEWPORT::ViewportToWorld(IVector2 V) {
    return (V - VIEWPORT::Offset) * VIEWPORT::AxisDirections;
}

Rect VIEWPORT::WorldToViewport(const Rect& R) {
    const IVector2 ViewportPos = VIEWPORT::WorldToViewport(IVector2((int)R.Position.X, (int)R.Position.Y));

    return {FVector2 ((float)ViewportPos.X, (float)ViewportPos.Y), R.Size};
}

Rect VIEWPORT::ViewportToWorld(const Rect& R) {
    const IVector2 WorldPos = VIEWPORT::ViewportToWorld(IVector2((int)R.Position.X, (int)R.Position.Y));

    return {FVector2 ((float)WorldPos.X, (float)WorldPos.Y), R.Size};
}
