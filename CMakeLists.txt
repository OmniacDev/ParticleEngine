cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(ParticleEngine LANGUAGES CXX)

# Dependencies
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(BUILD_SHARED_LIBS OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(ImGui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.89.9
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)

set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml
        GIT_TAG 2.6.x
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(ImGui-SFML)

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
        Engine/Shaders/Shaders.h
)

# Link Dependencies
target_link_libraries(${PROJECT_NAME} sfml-graphics ImGui-SFML::ImGui-SFML)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Copy Resources to binary directory
add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy Resources"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
        VERBATIM)

if(WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
            VERBATIM)
endif()