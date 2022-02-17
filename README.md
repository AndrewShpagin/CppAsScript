# CppAsScript
Using the compilable c++ as script, compiled at runtime using the clang-cl.<br>
Currently it works for Windows, but it will be crossplatform in future).

**This is one-header library, just include the header "cppAsScript.h" into your project and you may use c++ at native speed as the scripting language!**

## The problem it solves
You need to execute external scripts using your program API at the native c++ speed. Now it is possible! Declare the API, create script example as cpp file, use this project to complile that cpp and execute at runtime. It allows to create platform-independent plugins that are amost independent on API changes instead of old DLL-s method. It opens the possibility to users of the main program to create really fast and time-critical extensions. 

## Advantages
- The main goal is **"super easy to use"** + be able to setup and use all this on user's side, allow user to debug what he created. All what user needs to do - download and install LLVM using the provided link with all default settings. No additional setup required. And, generally LLVM download/setup may be done in automated way during the main program installation. In addition, **LLVM-CL has very permissive license** and you may just include it into the distributive of your package. This is a huge advantage over the Visual Studio.
- The real-time compiled sources may be **debugged** if need.
- **Very easy API creation**, you may export whole class with just a single directive. This is much more easy than API creation for any usiual scripting language. No need to call any registration functions.
- **One-header-only library!** No external dependencies!
- Perspective for the application to become the **semi-opensource**, when core is hidden but the most part of functionality is open in external c++ files. They are executed at the native speed, may be debugged. Making API is so easy that opening the most part of internal API is not a problem. Migration to the new version of the application, changes of the inner structures becomes easy, old scripts and pluging will be just recompiled on the fly.

## Pre-requisites
You need to install LLVM clang (it is relatively lightweight), not later than 12.0.1<br>
https://github.com/llvm/llvm-project/releases/tag/llvmorg-12.0.1 <br>

Fow example, for Windows it is only 186 MB:<br>
https://github.com/llvm/llvm-project/releases/download/llvmorg-12.0.1/LLVM-12.0.1-win64.exe <br>

No other dependencies, so even installing LLVM is not a problem at all on user's side, it may be done once during the main program installation. 

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
cppProject project;
std::string download = project.checkIfCompilerInstalled();
if(download.length()) {
	std::cout << "LLVM-CL not installed, please download and install at:\n" << download << "\n";
	return 1;
}
/// Set searh paths for files if they passed in relative form
/// 3 means that we add current folder of exe file and 3 parent folders
/// this is done because exe is usually deeply inside projects folder
project.addSearhPath(std::filesystem::current_path(), 3);

/// addFile adds the path to the file to be compiled and executed
/// addIncludeFolder adds the path to API reference include folder
/// debug states that debug config compiled, PDB file will be compiled as well
project.addFile("test/test.cpp").addIncludeFolder("api_test/").debug();

/// find and execute the main
auto f = project.bind<void()>("main");
if (f)f();

/// find and execute the test123
auto f1 = project.bind<void(int)>("test123");
if (f1)f1(23);
```
## Debugging
First, call .debug() for the project as stated in the example above.
You may easily debug the realtime compiled sources. Attach (for example, using Visual Studio) to the process that uses this library and runs the compiled functions. 
Load the text of the realtime compiled script, set breakpoint somewhere there. It just works!

## Licensing
Use it as you wish for any commercial or hobby purposes. Just mention somewhere that you use this library and place the link there.

