cmake_minimum_required(VERSION 3.15...3.19)

# +---------------+
# | Project setup |
# +---------------+

project(
  cyberpunkd
  VERSION 1.1.0
  DESCRIPTION "A Cyberpunk 2077 hacking mini-game solution finder"
  HOMEPAGE_URL "https://github.com/rufus-stone/cyberpunkd"
  LANGUAGES CXX)

# Create our executable
add_executable(
  ${PROJECT_NAME}
  src/main.cpp
  src/core/notifier.cpp
  src/core/ocr.cpp
  src/core/puzzler.cpp
  src/core/watcher.cpp
  src/game/point.cpp
  src/game/route.cpp
  src/game/goal.cpp
  src/game/state.cpp
  src/gui/gui.cpp
  src/gui/menubar.cpp
  src/gui/config_bar.cpp
  src/gui/about.cpp
  src/gui/demo.cpp
  src/utils/file_utils.cpp
  src/utils/string_utils.cpp
  src/utils/uuid.cpp)

# We're using C++17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Set compiler warnings
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -W4 -WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra
                                                 -Wold-style-cast -Wpedantic)
endif()

# clang-tidy
find_program(CLANGTIDY clang-tidy)
if(CLANGTIDY)
  set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY})
else()
  message(WARNING "clang-tidy not found")
endif()

# +--------------+
# | Dependencies |
# +--------------+

# Conan script
include(${PROJECT_SOURCE_DIR}/cmake/conan.cmake)

# Pull in the conan dependencies
conan_cmake_run(
  REQUIRES
  libarchive/3.4.3
  libwebp/1.1.0
  tesseract/4.1.1
  opencv/4.5.0
  spdlog/1.8.2
  docopt.cpp/0.6.3
  imgui-sfml/2.1@bincrafters/stable
  BASIC_SETUP
  CMAKE_TARGETS
  BUILD
  missing)

# Add the include directory to the target
target_include_directories(${PROJECT_NAME}
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Link libraries to executable
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE CONAN_PKG::spdlog CONAN_PKG::docopt.cpp CONAN_PKG::opencv
          CONAN_PKG::tesseract CONAN_PKG::libarchive CONAN_PKG::imgui-sfml)

# +--------------------+
# | Post-build actions |
# +--------------------+

# Copy the tesseract trained data to the build directory after building is
# complete. This came from https://github.com/tesseract-ocr/tessdata
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tessdata
          $<TARGET_FILE_DIR:${PROJECT_NAME}>/tessdata)

# +-------+
# | Tests |
# +-------+

include(CTest)
message("BUILD_TESTING: ${BUILD_TESTING}")
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
