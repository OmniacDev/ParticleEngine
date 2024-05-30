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