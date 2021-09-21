// CppAsScript.cpp : Defines the entry point for the application.
//

#include "maker/CLangScripter.h"
#include <filesystem>

int main()
{
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

	return 0;
}
