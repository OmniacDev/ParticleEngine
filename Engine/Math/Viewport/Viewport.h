#pragma once

#include "../Vector/Vector.h"
#include "../../EngineGlobalVars.h"

namespace VIEWPORT {

    inline const IVector2 Zero(ENGINE::WindowWidth / 2, ENGINE::WindowHeight / 2);
    inline IVector2 Offset = Zero;

    inline const IVector2 AxisDirections(1, -1);
    inline const float Scaling = 1;

    void TranslateViewport(IVector2 V);

    IVector2 WorldToViewport(IVector2 V);
    IVector2 ViewportToWorld(IVector2 V);
}

