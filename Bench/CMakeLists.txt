add_executable(Bench main.cpp "incluide/oop.h" "incluide/components.h" "Part1/setupAdd.h")
target_link_libraries(Bench PRIVATE ECSEngine)