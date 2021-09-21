# CppAsScript
Using the compilable c++ as script, compiled at runtime using the clang-cl.<br>
Currently it works for Windows, but it will be crossplatform in future).

## The problem it solves
You need to execute external scripts using your program API at the native c++ speed. Now it is possible! Declare the API, create scripе example as cpp file, use thes project to complile that cpp and execute at runtime.

## Pre-requisites
You need to install LLVM clang (it is relatively lightweight), not later that 12.0.1<br>
https://github.com/llvm/llvm-project/releases/tag/llvmorg-12.0.1 <br>

Fow example, for Windows it is inly 186 MB:<br>
https://github.com/llvm/llvm-project/releases/download/llvmorg-12.0.1/LLVM-12.0.1-win64.exe <br>

## Using
1. Include the files from the **maker** folder to your project.
2. Create the API for your program. In this example look [export.h](api_test/export.h), [export.cpp](api_test/export.cpp), we export class and function
```cpp
APICALL void test(const char* res);
class APICALL testClass {
public:
	void testfn(const char* x);
};
```
3. Now make some example that uses this API. Look [test/test.cpp](test/test.cpp) as the example. This file contains functions to be executed.
4. Use the **CLangProject** to execute functions. You need to pass the path to include files folder that contains the API reference header.
```cpp
CLangProject PR;
/// the path to project, we walk up because thet exe placed somehere deeply
std::filesystem::path cur = std::filesystem::current_path().parent_path().parent_path().parent_path();

/// the path to the file to be compiled and executed
std::filesystem::path test = cur;
test.append("test/test.cpp");

/// the path to API reference include folder
std::filesystem::path inc = cur;
inc.append("api_test/");
	
/// set project settings, pass the file to execute
PR.addFile(test.string()).addIncludeFolder(inc.string()).debug();

/// find and execute the main
auto f = PR.bind<void()>("main");
if (f)f();

/// find and execute the test123
auto f1 = PR.bind<void(int)>("test123");
if (f1)f1(23);
```

