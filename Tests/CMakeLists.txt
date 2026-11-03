find_package(Catch2 3 REQUIRED)

add_executable(ecs_tests
    entity_tests.cpp
    world_tests.cpp)
target_link_libraries(ecs_tests PRIVATE
    ECSEngine   # whatever your engine target is called
    Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(ecs_tests)