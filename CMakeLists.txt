cmake_minimum_required(VERSION 3.11)
project(infiniteStatisticalMineSweeper)
set(CMAKE_CXX_STANDARD 20)


if (${SDL_DISPLAY})
  ADD_DEFINITIONS(-DSDL_DISPLAY=1)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
  find_package(SDL2 REQUIRED)
  find_package(SDL2_image REQUIRED)
  find_package(SDL2_ttf REQUIRED)
  include_directories(${SDL2_INCLUDE_DIRS})
else()
  ADD_DEFINITIONS(-DSDL_DISPLAY=0)
endif()

# SFML 2.5
#find_package(SFML 2.5 COMPONENTS graphics audio window system network QUIET)

# Build types
if (UNIX)
    add_compile_options(-Wall -Wextra -Og )
    SET(CMAKE_BUILD_TYPE DEBUG)
    #Install sfml if not already installed
    if (NOT ${SDL_DISPLAY})
      if (NOT SFML_FOUND)
        include(FetchContent)

        FetchContent_Declare(
          SFML
          URL https://www.sfml-dev.org/files/SFML-2.5.1-linux-gcc-64-bit.tar.gz
        )
        FetchContent_GetProperties(SFML)
        if (NOT sfml_POPULATED)
          set(FETCHCONTENT_QUIET NO)
          FetchContent_Populate(sfml)
          set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
          include_directories("${sfml_SOURCE_DIR}/include")
        endif()
      endif()
    endif()
endif()

if (WIN32)
    SET(CMAKE_BUILD_TYPE DEBUG)
    file(COPY ${CMAKE_CURRENT_LIST_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/..)
    # file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../Release)
    #Install sfml if not already installed
    if (NOT SFML_FOUND)
      include(FetchContent)

      FetchContent_Declare(
        SFML
        URL https://www.sfml-dev.org/files/SFML-2.5.1-windows-vc15-64-bit.zip
      )
      FetchContent_GetProperties(SFML)
      if (NOT sfml_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(sfml)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        include_directories("${sfml_SOURCE_DIR}/include")
      endif()
      file(GLOB SFML_LIB "${sfml_SOURCE_DIR}/bin/*.dll")
      file(COPY ${SFML_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/..)
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()



add_executable(${PROJECT_NAME}
    src/main.cpp
    src/network/ByteObject.cpp
    src/map/dificultyModulator.cpp
    src/map/map.cpp
    src/map/mySharedMutex.cpp
    src/map/sfml.cpp
    src/map/sdl2.cpp
)

if (UNIX)
  if (${SDL_DISPLAY})
    target_link_libraries(${PROJECT_NAME}
      SDL2::Main SDL2::Image SDL2::TTF
    )
  else()
    target_link_libraries(${PROJECT_NAME}
      ${sfml_SOURCE_DIR}/lib/libsfml-graphics.so
      ${sfml_SOURCE_DIR}/lib/libsfml-audio.so
      ${sfml_SOURCE_DIR}/lib/libsfml-window.so
      ${sfml_SOURCE_DIR}/lib/libsfml-system.so
      ${sfml_SOURCE_DIR}/lib/libsfml-network.so
    )

  endif()
  target_link_libraries(${PROJECT_NAME} pthread)
endif()

if (WIN32)
target_link_libraries(${PROJECT_NAME}
  ${sfml_SOURCE_DIR}/lib/sfml-graphics-d.lib
  ${sfml_SOURCE_DIR}/lib/sfml-audio-d.lib
  ${sfml_SOURCE_DIR}/lib/sfml-window-d.lib
  ${sfml_SOURCE_DIR}/lib/sfml-system-d.lib
  ${sfml_SOURCE_DIR}/lib/sfml-network-d.lib
)
endif()

# target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-window sfml-system sfml-network)
set(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)