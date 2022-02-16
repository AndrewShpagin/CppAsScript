// CLangScripter.h : Include file for standard system include files,
// or project specific include files.
#pragma once

#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <functional>

class LibReference {
public:
	virtual ~LibReference() {};
	virtual bool valid() const = 0;
	virtual bool loadModule(const char* path) = 0;
	virtual void unLoadModule() = 0;
	virtual void* raw_ptr(const char* functionName) = 0;
	static LibReference* create();
};

class CLangProject;

class CLangBuilder {
	friend class CLangProject;
public:
	
	virtual ~CLangBuilder() {};

	/// returns the module extension (dll for windows)
	virtual const char* getModuleExtension() const = 0;

	/// compile the project
	virtual void compile(CLangProject& project) = 0;

	/// converts relative path to file to the full path
	virtual std::string pathInHeap(const std::string& relative) = 0;

	/// returns LLVM-CL download path for the current OS
	virtual std::string downloadPath() = 0;

	/// returns true if the LLVM-CL exists in system and available to be used
	virtual bool valid() = 0;
	
	static CLangBuilder* create();
};

class CLangProject {
protected:
	std::vector<std::string> files;
	std::string modulePath;
	std::string log;
	std::string _options;
	std::string _includes;
	LibReference* ref;
	CLangBuilder* builder;
	void _add(const std::string& opt);
	void _remove(const std::string& opt);
public:		
	CLangProject();
	virtual ~CLangProject();

	/// Compile the project (if need).
	void recompileIfNeed();

	/// Check if the code compiled successfully and available to run. There 
	bool valid();

 	/// Add the cpp file into the project, provide the full path
	CLangProject& addFile(const std::string& path);

	/// Add the include file into the project
	CLangProject& addIncludeFolder(const std::string& path);

	/// Add the text of cpp file to be compiled into the project
	CLangProject& addSource(const char* cpp_text);

	/// Compile (in future) with the speed optimization
	CLangProject& speedOptimization();

	/// Compile (in future) with the size optimization
	CLangProject& sizeOptimization();

	/// Compile debug version (this is just option setting, not compilation itself)
	CLangProject& debug();

	/// Compile release version (this is just option setting, not compilation itself)
	CLangProject& release();

	/// Returns "" if the LLVM-CL installed correctly, othervice it returns the path to download, you may display the message to offer the download.
	std::string checkIfCompilerInstalled();

	/// Pass the function name and get the function tat may be called later (till the project is loaded).
	template <class F>
	std::function<F> bind(const char* functionName);

	/// returns the compile log
	std::string& compileLog();

	/// returns the path to the compiled module
	std::string& module();

	/// returns the list of files in the project
	std::vector<std::string>& filesList();

	/// returns the compiler options
	std::string& options();

	/// returns the list of includes
	std::string& includes();
};

template <class F>
std::function<F> CLangProject::bind(const char* functionName) {
	std::function<F> f1 = nullptr;
	if (!ref) {
		recompileIfNeed();
	}
	if (ref) {
		f1 = static_cast<F*>(ref->raw_ptr(functionName));
	}
	return f1;
}
