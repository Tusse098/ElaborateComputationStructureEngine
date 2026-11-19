# --- core: no SDL, ever ---------------------------------------------------
add_library(ECSEngine STATIC
    src/core/world.cpp
    src/core/entityCommandBuffer.cpp
    src/core/scheduler.cpp
    src/debugging/debug.cpp)
target_include_directories(ECSEngine PUBLIC include)
target_compile_features(ECSEngine PUBLIC cxx_std_20)

# --- platform: window, events, time; the only target that sees SDL ---------
find_package(SDL3 CONFIG REQUIRED)

add_library(ECSPlatform STATIC
    src/platform/osWindow.h              # listed for the IDE only
    include/ecs/platform/window.h 
    src/platform/window.cpp
    src/platform/debugging/debug.cpp
    include/ecs/platform/debugging/debug.h)
target_include_directories(ECSPlatform
    PUBLIC  include
    PRIVATE src)
target_link_libraries(ECSPlatform
    PUBLIC  ECSEngine
    PRIVATE SDL3::SDL3)