cmake_minimum_required(VERSION 3.26)
project(GAME)
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# haskell stuff
set(ghc_version 9.8.2)
set(ghc_rts_ver 1.0.2)
set(therac_sim_ver 0.1.0.0)
#set(CMAKE_FIND_DEBUG_MODE TRUE)
find_path(HSINCLUDES NAMES "HsFFI.h" PATH_SUFFIXES "lib/ghc-${ghc_version}/lib/x86_64-linux-ghc-${ghc_version}/rts-${ghc_rts_ver}/include")
file(COPY ${PROJECT_SOURCE_DIR}/hstherac25/dist-newstyle/build/x86_64-linux/ghc-${ghc_version}/hstherac25-${therac_sim_ver}/build/libHShstherac25-${therac_sim_ver}-inplace-ghc${ghc_version}.so DESTINATION ${CMAKE_BINARY_DIR})
#file(COPY $ENV{HOME}/.ghcup/ghc/${ghc_version}/lib/ghc-${ghc_version}/lib/x86_64-linux-ghc-${ghc_version}/libHSrts-${ghc_rts_ver}_thr-ghc${ghc_version}.so DESTINATION ${CMAKE_BINARY_DIR})
set(HSTHERAC ${PROJECT_SOURCE_DIR}/hstherac25/dist-newstyle/build/x86_64-linux/ghc-${ghc_version}/hstherac25-${therac_sim_ver}/build/libHShstherac25-${therac_sim_ver}-inplace-ghc${ghc_version}.so)
set(HSRTS HSrts-${ghc_rts_ver}_thr-ghc${ghc_version})
find_path(HSLDPATH NAMES "lib${HSRTS}.so" PATH_SUFFIXES "lib/ghc-${ghc_version}/lib/x86_64-linux-ghc-${ghc_version}/")
set(CMAKE_FIND_DEBUG_MODE FALSE)
#set(HSLDFLAGS "-L$ENV{HOME}/.ghcup/ghc/${ghc_version}/lib/ghc-${ghc_version}/lib/x86_64-linux-ghc-${ghc_version}/")
set(HSLDFLAGS "-L${HSLDPATH}")
#set(HSINCLUDES $ENV{HOME}/.ghcup/ghc/${ghc_version}/lib/ghc-${ghc_version}/lib/x86_64-linux-ghc-${ghc_version}/rts-${ghc_rts_ver}/include)

#siv3d
add_compile_definitions(BOOST_ASIO_HAS_STD_INVOKE_RESULT)
add_subdirectory(${PROJECT_SOURCE_DIR}/OpenSiv3D/Linux)
#set(Siv3D_DIR ${PROJECT_SOURCE_DIR}/OpenSiv3D/Linux/build)
#find_package(Siv3D)
#set_target_properties(libSiv3D PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/OpenSiv3D/Linux/build/libSiv3D.a")
#set_target_properties(libSiv3D PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/OpenSiv3D/Siv3D/include;${CMAKE_SOURCE_DIR}/OpenSiv3D/Siv3D/include/ThirdParty")

#entt
add_subdirectory(${PROJECT_SOURCE_DIR}/entt)

#ldtkloader
add_subdirectory(${PROJECT_SOURCE_DIR}/LDtkLoader)

#marl
set(MARL_DIR ${PROJECT_SOURCE_DIR}/marl)
add_subdirectory(${MARL_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Wpedantic -Wno-unused-parameter -fPIE -fPIC ${HSLDFLAGS} -fuse-ld=lld")
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/OpenSiv3D ${PROJECT_SOURCE_DIR}/OpenSiv3D/Siv3D ${PROJECT_SOURCE_DIR}/OpenSiv3D/Siv3D/include ${PROJECT_SOURCE_DIR}/OpenSiv3D/Siv3D/include/ThirdParty  ${PROJECT_SOURCE_DIR}/marl/include ${HSINCLUDES})

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
add_library(theracMinigame STATIC ${TSIMDIR}/TheracConsole.cpp ${TSIMDIR}/TextBox.cpp ${TSIMDIR}/TheracMinigame.cpp ${PLUGINDIR}/TheracSimulatorAdapter.cpp)
add_library(therac3DScene STATIC ${T3DDIR}/RadiationLab.cpp)
add_library(gqScene2D STATIC ${S2DDIR}/StageMap.cpp ${S2DDIR}/LayerParsing.cpp ${HOMESLUM}/TileDescriptions.cpp ${HOMESLUM}/HomeSlum.cpp)

add_executable(GAME main.cpp)
target_compile_features(GAME PRIVATE cxx_std_20 c_std_11)
target_link_libraries(theracMinigame PUBLIC Siv3D::Siv3D PUBLIC EnTT::EnTT)
target_link_libraries(therac3DScene PUBLIC Siv3D::Siv3D PUBLIC EnTT::EnTT)
target_link_libraries(gqScene2D PUBLIC Siv3D::Siv3D PUBLIC EnTT::EnTT)
target_link_libraries(GAME PUBLIC Siv3D::Siv3D PUBLIC EnTT::EnTT PUBLIC theracMinigame PUBLIC therac3DScene PUBLIC gqScene2D PRIVATE ${HSTHERAC} PRIVATE ffi PRIVATE ${HSRTS} PRIVATE marl PRIVATE LDtkLoader)
#target_link_libraries(GAME PUBLIC Siv3D::Siv3D PUBLIC EnTT::EnTT PUBLIC theracMinigame PUBLIC therac3DScene PUBLIC gqScene2D ffi HShstherac25 HSrts marl LDtkLoader)
