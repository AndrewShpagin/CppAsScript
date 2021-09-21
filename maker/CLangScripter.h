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
	virtual const char* getModuleExtension() const = 0;
	virtual void compile(CLangProject& project) = 0;
	virtual std::string pathInHeap(const std::string& relative) = 0;
	
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
	bool valid();
	CLangProject& addFile(const std::string& path);
	CLangProject& addIncludeFolder(const std::string& path);
	
	CLangProject& addSource(const char* cpp_text);
	CLangProject& speedOptimization();
	CLangProject& sizeOptimization();
	CLangProject& debug();
	CLangProject& release();
	
	void recompileIfNeed();

	template <class F>
	std::function<F> bind(const char* functionName);
	
	std::string& compileLog();
	std::string& module();
	std::vector<std::string>& filesList();
	std::string& options();
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
