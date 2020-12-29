message(STATUS "Building cyberpunkd_tests...")

add_library(catch_main STATIC catch_main.cpp)

add_executable(
  cyberpunkd_tests
  tests.cpp ${PROJECT_SOURCE_DIR}/src/file_utils.cpp
  ${PROJECT_SOURCE_DIR}/src/ocr.cpp ${PROJECT_SOURCE_DIR}/src/string_utils.cpp)

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
