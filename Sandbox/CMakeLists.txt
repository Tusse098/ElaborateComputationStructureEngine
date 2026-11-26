# CMakeList.txt : CMake project for ElaborateComputationStructureEngine, include source and define
# project specific logic here.
#

# Add source to this project's executable.
#add_executable (Sandbox "Sandbox.cpp" "Sandbox.h")s

#if (CMAKE_VERSION VERSION_GREATER 3.12)
#  set_property(TARGET Sandbox PROPERTY CXX_STANDARD 20)
#endif()

# TODO: Add tests and install targets if needed.
find_package(SDL3 REQUIRED CONFIG)
add_executable(Sandbox
    src/main.cpp
 "src/core/systems/movement.h" "src/core/systems/movement.cpp" "src/core/systems/all.cpp" "src/core/systems/spawner.h" "src/core/systems/spawner.cpp" "src/core/systems/all.h" "src/core/systems/windowPlayground.h" "src/core/systems/windowPlayground.cpp" "src/core/rendering/blinking.cpp" "src/core/rendering/blinking.h" "src/core/rendering/mainRendering.h" "src/core/rendering/mainRendering.cpp")
target_link_libraries(Sandbox
    PRIVATE ECSPlatform
)

add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Sandbox>/assets)

target_include_directories(Sandbox PRIVATE include)
