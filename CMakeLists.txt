cmake_minimum_required(VERSION 3.15...3.18)

# Set up our project
project(
  cyberpunkd
  VERSION 0.1.0
  DESCRIPTION "A Cyberpunk 2077 hacking mini-game solution finder"
  HOMEPAGE_URL "https://github.com/rufus-stone/cyberpunkd"
  LANGUAGES CXX)

# Create our executable
add_executable(${PROJECT_NAME} src/main.cpp src/file_utils.cpp)

# We're using C++17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Set some compiler options/warnings
target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra
                                               -Wold-style-cast -Wpedantic)

# Conan scripts
include(${PROJECT_SOURCE_DIR}/cmake/conan.cmake)

# Pull in the conan dependencies
conan_cmake_run(
  REQUIRES
  spdlog/1.8.2
  docopt.cpp/0.6.3
  opencv/4.5.0
  tesseract/4.1.1
  BASIC_SETUP
  CMAKE_TARGETS
  BUILD
  missing)

# Add the include directory and external directory to the target
target_include_directories(${PROJECT_NAME}
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Link libraries to executable
target_link_libraries(
  ${PROJECT_NAME} PRIVATE CONAN_PKG::spdlog CONAN_PKG::docopt.cpp
                          CONAN_PKG::opencv)

# include(CTest) # This adds the BUILD_TESTING option (ON by default)
# message("BUILD_TESTING: ${BUILD_TESTING}") if(BUILD_TESTING) enable_testing()
# add_subdirectory(test ${PROJECT_BUILD_DIR}) endif()
message("OpenCV library status:")
message("          config: ${OpenCV_DIR}")
message("         version: ${OpenCV_VERSION}")
message("       libraries: ${OpenCV_LIBS}")
message("    include path: ${OpenCV_INCLUDE_DIRS}")
message("      source dir: ${OpenCV_SOURCE_DIR}")
