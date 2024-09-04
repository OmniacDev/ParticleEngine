cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(ParticleEngine)

# Dependencies
set(BUILD_SHARED_LIBS OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

# Our Project

add_executable(${PROJECT_NAME} main.cpp
        Engine/EngineGlobalVars.h
        Physics/Particle.cpp
        Physics/Particle.h
        Engine/Math/Vector/Vector.h
        Engine/Math/Types.h
        Engine/Math/Viewport/Viewport.cpp
        Engine/Math/Viewport/Viewport.h
        Physics/Solver.cpp
        Physics/Solver.h
        Physics/QuadTree/QuadTree.h
        Engine/Math/Rect/Rect.h
        Engine/FreeList/FreeList.h
)

# Link Dependencies
target_link_libraries(${PROJECT_NAME} sfml-graphics)