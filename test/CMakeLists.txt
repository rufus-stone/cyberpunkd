message(STATUS "Building cyberpunkd_tests...")

add_library(catch_main STATIC catch_main.cpp)

add_executable(
  cyberpunkd_tests
  tests.cpp
  ${PROJECT_SOURCE_DIR}/src/utils/file_utils.cpp
  ${PROJECT_SOURCE_DIR}/src/utils/string_utils.cpp
  ${PROJECT_SOURCE_DIR}/src/utils/uuid.cpp
  ${PROJECT_SOURCE_DIR}/src/ocr.cpp
  ${PROJECT_SOURCE_DIR}/src/game/point.cpp
  ${PROJECT_SOURCE_DIR}/src/game/route.cpp
  ${PROJECT_SOURCE_DIR}/src/game/goal.cpp
  ${PROJECT_SOURCE_DIR}/src/game/state.cpp
  ${PROJECT_SOURCE_DIR}/src/game/puzzler.cpp)

target_compile_features(cyberpunkd_tests PRIVATE cxx_std_17)

# Add the include directory to the target
target_include_directories(cyberpunkd_tests
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Link libraries to executable
target_link_libraries(
  cyberpunkd_tests
  PRIVATE CONAN_PKG::spdlog CONAN_PKG::docopt.cpp CONAN_PKG::opencv
          CONAN_PKG::tesseract CONAN_PKG::libarchive)

message("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")

add_custom_command(
  TARGET cyberpunkd_tests
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tessdata
          $<TARGET_FILE_DIR:cyberpunkd_tests>/tessdata)
