// CppAsScript.cpp : Defines the entry point for the application.
//

#include "maker/CLangScripter.h"
#include <filesystem>

int main()
{
	CLangProject PR;
	std::filesystem::path cur = std::filesystem::current_path().parent_path().parent_path().parent_path();
	
	std::filesystem::path test = cur;
	test.append("test/test.cpp");
	
	std::filesystem::path inc = cur;
	inc.append("api_test/");
	
	PR.addFile(test.string()).addIncludeFolder(inc.string()).debug();

	auto f = PR.bind<void()>("main");
	if (f)f();

	auto f1 = PR.bind<void(int)>("test123");
	if (f1)f1(23);

	return 0;
}
