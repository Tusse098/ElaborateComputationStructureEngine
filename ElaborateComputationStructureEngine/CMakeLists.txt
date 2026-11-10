# --- core: no SDL, ever ---------------------------------------------------
add_library(ECSEngine STATIC
    src/core/world.cpp
    src/core/entityCommandBuffer.cpp
    src/core/scheduler.cpp
    src/debugging/debug.cpp "include/ecs/platform/window.h")
target_include_directories(ECSEngine PUBLIC include)
target_compile_features(ECSEngine PUBLIC cxx_std_20)

# --- platform: window, events, time; the only target that sees SDL ---------
find_package(SDL3 CONFIG REQUIRED)

add_library(ECSPlatform STATIC
    src/platform/window.cpp "include/ecs/platform/window.h")
target_include_directories(ECSPlatform PUBLIC include)
target_link_libraries(ECSPlatform
    PUBLIC ECSEngine
    PUBLIC SDL3::SDL3)