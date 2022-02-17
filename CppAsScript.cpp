// The example of the compiler usage

#include "cppAsScript.h"

int main()
{
	cppProject project;
	std::string download = project.checkIfCompilerInstalled();
	if(download.length()) {
		std::cout << "LLVM-CL not installed, please download and install at:\n" << download << "\n";
		return 1;
	}
	/// Set searh paths for files if they passed in relative form
	///	3 means that we add current folder of exe file and 3 parent folders
	///	this is done because exe is usually deeply inside projects folder
	project.addSearhPath(std::filesystem::current_path(), 3);

	/// addFile adds the path to the file to be compiled and executed
	///	addIncludeFolder adds the path to API reference include folder
	///	debug states that debug config compiled, PDB file will be compiled as well
	project.addFile("test/test.cpp").addIncludeFolder("api_test/").debug();

	/// find and execute the main
	auto f = project.bind<void()>("main");
	if (f)f();

	/// find and execute the test123
	auto f1 = project.bind<void(int)>("test123");
	if (f1)f1(23);

	return 0;
}
