# CMakeList.txt : CMake project for CppAsScript, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

project ("CppAsScript")

# Add source to this project's executable.
add_executable (CppAsScript "CppAsScript.cpp" "CppAsScript.h" "maker/_win.cpp" "maker/CLangScripter.cpp" "api_test/export.cpp")

# TODO: Add tests and install targets if needed.
