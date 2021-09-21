// CLangScripter.cpp : Defines the entry point for the application.
//

#include "CLangScripter.h"

#include "md5.h"

static std::string _replace(const std::string& str, const std::string& sub, const std::string& mod) {
	std::string tmp(str);
	auto fnd = tmp.find(sub);
	if (fnd != -1)tmp.replace(fnd, sub.length(), mod);
	return tmp;
}

void CLangProject::_add(const std::string& opt) {
	_remove(opt);
	_options += " " + opt;
	_replace(_options, "  ", " ");
}

void CLangProject::_remove(const std::string& opt) {
	_replace(_options, opt, "");
	_replace(_options, "  ", " ");
}

CLangProject::CLangProject() {
	builder = CLangBuilder::create();
	ref = nullptr;
	_options = "/std:c++latest /LD";
}

CLangProject::~CLangProject() {
	if (builder)delete(builder);
	builder = nullptr;
	if (ref)delete(ref);
	ref = nullptr;
}

bool CLangProject::valid() {
	return ref != nullptr;
}

std::string& CLangProject::compileLog() {
	return log;
}

std::string& CLangProject::module() {
	return modulePath;
}

std::vector<std::string>& CLangProject::filesList() {
	return files;
}

std::string& CLangProject::options() {
	return _options;
}

std::string& CLangProject::includes() {
	return _includes;
}

CLangProject& CLangProject::addFile(const std::string& path) {
	filesList().push_back(path);
	return *this;
}

CLangProject& CLangProject::addIncludeFolder(const std::string& path) {
	includes() += " /I " + path;
	return *this;
}

CLangProject& CLangProject::addSource(const char* cpp_text) {
	std::string fn = "temp_" + md5::hash(cpp_text) + ".cpp";
	std::string res = builder->pathInHeap(fn);
	std::ofstream f(res);
	if (f.is_open()) {
		f << cpp_text;
		f.close();
	}
	filesList().push_back(res);
	return *this;
}

CLangProject& CLangProject::speedOptimization() {
	_add("/Ot");
	return *this;
}

CLangProject& CLangProject::sizeOptimization() {
	_add("/Os");
	return *this;
}

CLangProject& CLangProject::debug() {
	_remove("/LD");
	_add("/LDd");
	return *this;
}

CLangProject& CLangProject::release() {
	_remove("/LDd");
	_add("/LD");
	return *this;
}

void CLangProject::recompileIfNeed() {
	if(builder) {
		builder->compile(*this);
		if (ref)delete(ref);
		ref = LibReference::create();
		ref->loadModule(module().c_str());
	}
}

