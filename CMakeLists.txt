cmake_minimum_required(VERSION 3.26)
project(GAME)
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# haskell stuff
set(ghc_version 9.8.1)
set(ghc_rts_ver 1.0.2)
set(therac_sim_ver 0.1.0.0)
file(COPY $ENV{HOME}/.ghcup/ghc/${ghc_version}/lib/ghc-${ghc_version}/lib/x86_64-linux-ghc-${ghc_version}/libHSrts-${ghc_rts_ver}_thr-ghc${ghc_version}.so DESTINATION ${CMAKE_BINARY_DIR})
set(HSTHERAC ${PROJECT_SOURCE_DIR}/hstherac25/dist-newstyle/build/x86_64-linux/ghc-${ghc_version}/hstherac25-${therac_sim_ver}/build/libHShstherac25-${therac_sim_ver}-inplace-ghc${ghc_version}.so)
set(HSRTS HSrts-${ghc_rts_ver}_thr-ghc${ghc_version})
set(HSLDFLAGS "-L$ENV{HOME}/.ghcup/ghc/${ghc_version}/lib/ghc-${ghc_version}/lib/x86_64-linux-ghc-${ghc_version}/")
set(HSINCLUDES $ENV{HOME}/.ghcup/ghc/${ghc_version}/lib/ghc-${ghc_version}/lib/x86_64-linux-ghc-${ghc_version}/rts-${ghc_rts_ver}/include)

#siv3d
## this is way easier than finding the 382 deps found by ldd
find_package(Siv3D)
#set_target_properties(libSiv3D PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/OpenSiv3D/Linux/build/libSiv3D.a")
#set_target_properties(libSiv3D PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/OpenSiv3D/Siv3D/include;${CMAKE_SOURCE_DIR}/OpenSiv3D/Siv3D/include/ThirdParty")

#entt
add_subdirectory(${PROJECT_SOURCE_DIR}/entt)

#ldtkloader
add_subdirectory(${PROJECT_SOURCE_DIR}/LDtkLoader)

#marl
set(MARL_DIR ${PROJECT_SOURCE_DIR}/marl)
add_subdirectory(${MARL_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Wpedantic -Wno-unused-parameter -fPIE -fPIC ${HSLDFLAGS}")
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/OpenSiv3D ${PROJECT_SOURCE_DIR}/OpenSiv3D/Siv3D ${PROJECT_SOURCE_DIR}/OpenSiv3D/Siv3D/include  ${PROJECT_SOURCE_DIR}/marl/include ${HSINCLUDES})

set(SRC_DIR src)
set(MIGADIR ${SRC_DIR}/Minigames)
set(TSIMDIR ${MIGADIR}/TheracSimulator/)
set(S2DDIR ${SRC_DIR}/Scene2D)
set(HOMESLUM ${S2DDIR}/Scenes/HomeSlum)
set(S3DDIR ${SRC_DIR}/Scene3D)
set(T3DDIR ${S3DDIR}/Scenes/RadiationLab)
set(PLUGINPATH Plugins)
set(PLUGINDIR ${SRC_DIR}/${PLUGINPATH})

configure_file(${PROJECT_SOURCE_DIR}/include/${PLUGINPATH}/TheracSimulatorAdapter.hpp.in ${PROJECT_SOURCE_DIR}/include/${PLUGINPATH}/TheracSimulatorAdapter.hpp)
#set(THCDIR ${PLUGINDIR})
add_library(theracMinigame SHARED ${TSIMDIR}/TheracConsole.cpp ${TSIMDIR}/TextBox.cpp ${TSIMDIR}/TheracMinigame.cpp ${PLUGINDIR}/TheracSimulatorAdapter.cpp)
add_library(therac3DScene SHARED ${T3DDIR}/RadiationLab.cpp)
add_library(gqScene2D SHARED ${S2DDIR}/StageMap.cpp ${S2DDIR}/LayerParsing.cpp ${HOMESLUM}/TileDescriptions.cpp ${HOMESLUM}/HomeSlum.cpp)

add_executable(GAME main.cpp)
target_link_libraries(theracMinigame Siv3D::Siv3D EnTT::EnTT)
target_link_libraries(therac3DScene Siv3D::Siv3D EnTT::EnTT)
target_link_libraries(gqScene2D Siv3D::Siv3D EnTT::EnTT)
target_link_libraries(GAME PUBLIC Siv3D::Siv3D PUBLIC EnTT::EnTT PUBLIC theracMinigame PUBLIC therac3DScene PUBLIC gqScene2D ${HSTHERAC} ffi ${HSRTS} marl LDtkLoader)
