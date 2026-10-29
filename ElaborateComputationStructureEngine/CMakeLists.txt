# CMakeList.txt : CMake project for ElaborateComputationStructureEngine, include source and define
# project specific logic here.
#



# Add source to this project's executable.
# add_executable (ElaborateComputationStructureEngine "ElaborateComputationStructureEngine.cpp" "ElaborateComputationStructureEngine.h")

add_library(ECSEngine STATIC
	src/core/world.cpp
)

target_include_directories(ECSEngine PUBLIC include)
target_link_directories(ECSEngine PUBLIC d3d11 dxgi d3dcompiler)

#if (CMAKE_VERSION VERSION_GREATER 3.12)
#  set_property(TARGET ElaborateComputationStructureEngine PROPERTY CXX_STANDARD 20)
#endif()

# TODO: Add tests and install targets if needed.
