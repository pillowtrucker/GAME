cmake_minimum_required(VERSION 3.26)
project(GAME)

# this is way easier than finding the 382 deps found by ldd
find_package(Siv3D)

#target_compile_features(Siv3DTest PRIVATE cxx_std_20)
#add_library(libSiv3D STATIC IMPORTED)
#set_target_properties(libSiv3D PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/OpenSiv3D/Linux/build/libSiv3D.a")
#set_target_properties(libSiv3D PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/OpenSiv3D/Siv3D/include;${CMAKE_SOURCE_DIR}/OpenSiv3D/Siv3D/include/ThirdParty")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Wpedantic -Wno-unused-parameter")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(GAME main.cpp)
target_link_libraries(GAME PUBLIC Siv3D::Siv3D)
#target_link_libraries(GAME libSiv3D)
