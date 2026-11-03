add_executable(Bench main.cpp )
target_link_libraries(Bench PRIVATE ECSEngine)
target_include_directories(Bench PUBLIC include)