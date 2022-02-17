// CLangScripter.h : Include file for standard system include files,
// or project specific include files.
#pragma once

#include <filesystem>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <functional>


class cppModule;
class cppBuilder;

class cppProject {
protected:
	std::vector<std::string> files;
	std::vector<std::filesystem::path> paths;
	std::string modulePath;
	std::string log;
	std::string _options;
	std::string _includes;
	cppModule* ref;
	cppBuilder* builder;
	void _add(const std::string& opt);
	void _remove(const std::string& opt);
public:		
	cppProject();
	virtual ~cppProject();


	/**
	 * \brief Add the path where we seek for files if the relative path passed
	 * \param path the absolute path
	 * \param parent_depth we add the path, then add parent path and so on, repeat this parent_depth count. It is useful if executable nested somewhere deeply into project folder
	 */
	void addSearhPath(const std::filesystem::path& path, int parent_depth = 0);

	/**
	 * \brief determines if path is local and tries to find the file in any folder defined previously with addSearhPath
	 * \param path relative or absolute path 
	 * \return the file path if the file exists, empty path othervice 
	 */
	std::filesystem::path findFile(const std::string& path);

	/// Compile the project (if need).
	void recompileIfNeed();

	/// Check if the code compiled successfully and available to run. There 
	bool valid();

 	/// Add the cpp file into the project, provide the full path
	cppProject& addFile(const std::string& path);

	/// Add the include file into the project
	cppProject& addIncludeFolder(const std::string& path);

	/// Add the text of cpp file to be compiled into the project
	cppProject& addSource(const std::string& cpp_text);

	/// Compile (in future) with the speed optimization
	cppProject& speedOptimization();

	/// Compile (in future) with the size optimization
	cppProject& sizeOptimization();

	/// Compile debug version (this is just option setting, not compilation itself)
	cppProject& debug();

	/// Compile release version (this is just option setting, not compilation itself)
	cppProject& release();

	/// Returns "" if the LLVM-CL installed correctly, othervice it returns the path to download, you may display the message to offer the download.
	std::string checkIfCompilerInstalled();

	/// Pass the function name and get the function tat may be called later (till the project is loaded).
	template <class F>
	std::function<F> bind(const std::string& functionName);

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
std::function<F> cppProject::bind(const std::string& functionName) {
	std::function<F> f1 = nullptr;
	if (!ref) {
		recompileIfNeed();
	}
	if (ref) {
		f1 = static_cast<F*>(ref->raw_ptr(functionName.c_str()));
	}
	return f1;
}

class cppModule {
public:
	virtual ~cppModule() {};
	virtual bool valid() const = 0;
	virtual bool loadModule(const char* path) = 0;
	virtual void unLoadModule() = 0;
	virtual void* raw_ptr(const char* functionName) = 0;
	static cppModule* create();
};

class cppBuilder {
	friend class cppProject;
public:

	virtual ~cppBuilder() {};

	/// returns the module extension (dll for windows)
	virtual const char* getModuleExtension() const = 0;

	/// compile the project
	virtual void compile(cppProject& project) = 0;

	/// converts relative path to file to the full path
	virtual std::string pathInHeap(const std::string& relative) = 0;

	/// returns LLVM-CL download path for the current OS
	virtual std::string downloadPath() = 0;

	/// returns true if the LLVM-CL exists in system and available to be used
	virtual bool valid() = 0;

	static cppBuilder* create();
};


/// implementation



#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))
#include <filesystem>
#include <fstream>

namespace md5 {
	inline void hash(const uint8_t* initial_msg, size_t initial_len, char* res) {

		uint32_t h0, h1, h2, h3;
		uint8_t* msg = NULL;

		static uint32_t r[] = {
			7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
			5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
			4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
			6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
		};

		static uint32_t k[] = {
			0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
			0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
			0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
			0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
			0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
			0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
			0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
			0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
			0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
			0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
			0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
			0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
			0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
			0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
			0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
			0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
		};

		h0 = 0x67452301;
		h1 = 0xefcdab89;
		h2 = 0x98badcfe;
		h3 = 0x10325476;

		size_t new_len = ((((initial_len + 8) / 64) + 1) * 64) - 8;

		msg = (uint8_t*)calloc(new_len + 64, 1);
		memcpy(msg, initial_msg, initial_len);
		msg[initial_len] = 128;
		size_t bits_len = 8 * initial_len;
		memcpy(msg + new_len, &bits_len, 4);

		for (int offset = 0; offset < new_len; offset += (512 / 8)) {
			uint32_t* w = (uint32_t*)(msg + offset);

			uint32_t a = h0;
			uint32_t b = h1;
			uint32_t c = h2;
			uint32_t d = h3;

			uint32_t i;
			for (i = 0; i < 64; i++) {
				uint32_t f, g;
				if (i < 16) {
					f = (b & c) | ((~b) & d);
					g = i;
				}
				else if (i < 32) {
					f = (d & b) | ((~d) & c);
					g = (5 * i + 1) % 16;
				}
				else if (i < 48) {
					f = b ^ c ^ d;
					g = (3 * i + 5) % 16;
				}
				else {
					f = c ^ (b | (~d));
					g = (7 * i) % 16;
				}
				uint32_t temp = d;
				d = c;
				c = b;
				b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
				a = temp;
			}

			h0 += a;
			h1 += b;
			h2 += c;
			h3 += d;
		}
		free(msg);
		uint8_t bytes[16];
		uint32_t* m5res = (uint32_t*)bytes;
		m5res[0] = h0;
		m5res[1] = h1;
		m5res[2] = h2;
		m5res[3] = h3;
		const char* chars16 = "0123456789abcdef";
		for (int i = 0, p = 0; i < 16; i++) {
			uint8_t b = bytes[i];
			res[p++] = chars16[b >> 4];
			res[p++] = chars16[b & 15];
		}
	}

	inline std::string hash(const uint8_t* initial_msg, size_t initial_len) {
		char c[33];
		hash(initial_msg, initial_len, c);
		c[32] = 0;
		return std::string(c);
	}

	inline std::string hash(const std::string& s) {
		return hash(reinterpret_cast<const uint8_t*>(s.c_str()), s.length());
	}

	inline std::string file_hash(const std::string& path) {
		size_t sz = std::filesystem::file_size(path);
		std::ifstream f(path, std::ios::binary);
		if (f.is_open()) {
			char* u = new char[sz];
			f.read(u, sz);
			f.close();
			return hash(reinterpret_cast<const uint8_t*>(u), sz);
		}
		return "";
	}
}


inline static std::string _replace(const std::string& str, const std::string& sub, const std::string& mod) {
	std::string tmp(str);
	auto fnd = tmp.find(sub);
	if (fnd != -1)tmp.replace(fnd, sub.length(), mod);
	return tmp;
}

inline void cppProject::_add(const std::string& opt) {
	_remove(opt);
	_options += " " + opt;
	_options = _replace(_options, "  ", " ");
}

inline void cppProject::_remove(const std::string& opt) {
	_options = _replace(_options, opt, "");
	_options = _replace(_options, "  ", " ");
}

inline cppProject::cppProject() {
	builder = cppBuilder::create();
	ref = nullptr;
	_options = "/std:c++latest /LD";
}

inline cppProject::~cppProject() {
	if (builder)delete(builder);
	builder = nullptr;
	if (ref)delete(ref);
	ref = nullptr;
}

inline void cppProject::addSearhPath(const std::filesystem::path& path, int parent_depth) {
	paths.push_back(path);
	if(parent_depth && path.has_parent_path()) {
		addSearhPath(path.parent_path(), parent_depth - 1);
	}
}

inline std::filesystem::path cppProject::findFile(const std::string& path) {
	std::filesystem::path p = path;
	if(p.is_absolute()) {
		if (exists(p))return p;
	} else {
		for(auto p:paths) {
			auto pt = p;
			pt.append(path);
			if (exists(pt))return pt;
		}
	}
	p.clear();
	return p;
}

inline bool cppProject::valid() {
	return ref != nullptr;
}

inline std::string& cppProject::compileLog() {
	return log;
}

inline std::string& cppProject::module() {
	return modulePath;
}

inline std::vector<std::string>& cppProject::filesList() {
	return files;
}

inline std::string& cppProject::options() {
	return _options;
}

inline std::string& cppProject::includes() {
	return _includes;
}

inline cppProject& cppProject::addFile(const std::string& path) {
	auto fpath = findFile(path);
	if (!is_empty(fpath))filesList().push_back(fpath.string());
	return *this;
}

inline cppProject& cppProject::addIncludeFolder(const std::string& path) {
	auto fpath = findFile(path);
	if(exists(fpath)) includes() += std::string(" /I ") + "\"" + path + "\"";
	return *this;
}

inline cppProject& cppProject::addSource(const std::string& cpp_text) {
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

inline cppProject& cppProject::speedOptimization() {
	_add("/Ot");
	return *this;
}

inline cppProject& cppProject::sizeOptimization() {
	_add("/Os");
	return *this;
}

inline cppProject& cppProject::debug() {
	_remove("/LD");
	_add("/LDd");
	_add("-fuse-ld=lld -Z7");
	return *this;
}

inline cppProject& cppProject::release() {
	_remove("/LDd");
	_add("/LD");
	return *this;
}

inline std::string cppProject::checkIfCompilerInstalled() {
	if (builder) {
		if (!builder->valid()) {
			return builder->downloadPath();
		}
	}
	return "";
}

inline void cppProject::recompileIfNeed() {
	if (builder) {
		builder->compile(*this);
		if (ref)delete(ref);
		ref = cppModule::create();
		ref->loadModule(module().c_str());
	}
}

#ifdef _WIN32

#include <Windows.h>
#include <shlobj_core.h>

inline std::filesystem::path _getexepath() {
#ifdef _WIN32
	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, path, MAX_PATH);
	return path;
#else
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
#endif
}

struct funcRef {
	funcRef() {
		address = nullptr;
	}
	std::string function;
	FARPROC address;
};

class LibReferenceWin : public cppModule {
	HMODULE h;
	std::vector<funcRef> refs;
public:
	~LibReferenceWin() override {
		LibReferenceWin::unLoadModule();
	}
	LibReferenceWin() {
		h = nullptr;
	}
	bool valid() const override {
		return h != nullptr;
	}
	bool loadModule(const char* path) override {
		if (h)unLoadModule();
		h = LoadLibrary(path);
		return h != nullptr;
	}
	void unLoadModule() override {
		if (h) {
			FreeLibrary(h);
			refs.clear();
		}
		h = nullptr;
	}
	void* raw_ptr(const char* functionName) override {
		if (h) {
			for (auto i = refs.begin(); i != refs.end(); i++) {
				if (i->function == functionName) {
					return (void*)i->address;
				}
			}
			FARPROC f = GetProcAddress(h, functionName);
			if (f) {
				funcRef fn;
				fn.function = functionName;
				fn.address = f;
				return f;
			}
		}
		return nullptr;
	}
};

inline std::string exec(const char* cmd) {
	std::string strResult;
	HANDLE hPipeRead, hPipeWrite;

	SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
	saAttr.bInheritHandle = TRUE; // Pipe handles are inherited by child process.
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe to get results from child's stdout.
	if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0))
		return strResult;

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = hPipeWrite;
	si.hStdError = hPipeWrite;
	si.wShowWindow = SW_HIDE; // Prevents cmd window from flashing.
							  // Requires STARTF_USESHOWWINDOW in dwFlags.

	PROCESS_INFORMATION pi = { 0 };

	BOOL fSuccess = CreateProcess(NULL, (LPSTR)cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!fSuccess)
	{
		CloseHandle(hPipeWrite);
		CloseHandle(hPipeRead);
		return strResult;
	}

	bool bProcessEnded = false;
	for (; !bProcessEnded;)
	{
		// Give some timeslice (50 ms), so we won't waste 100% CPU.
		bProcessEnded = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;

		// Even if process exited - we continue reading, if
		// there is some data available over pipe.
		for (;;)
		{
			char buf[1024];
			DWORD dwRead = 0;
			DWORD dwAvail = 0;

			if (!::PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwAvail, NULL))
				break;

			if (!dwAvail) // No data available, return
				break;

			if (!::ReadFile(hPipeRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
				// Error, the child process might ended
				break;

			buf[dwRead] = 0;
			strResult += buf;
		}
	} //for

	CloseHandle(hPipeWrite);
	CloseHandle(hPipeRead);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return strResult;
}

class CLangBuilderWin : public cppBuilder {
	std::string clangPath;
	std::string tempPath;
public:
	CLangBuilderWin() {
		WCHAR pf[MAX_PATH];
		SHGetSpecialFolderPathW(0, pf, CSIDL_PROGRAM_FILES, FALSE);
		std::filesystem::path p = pf;
		p.append("LLVM/bin/clang-cl.exe");
		if (exists(p)) {
			clangPath = p.string();
		}
		std::filesystem::path tp = std::filesystem::temp_directory_path();
		std::string enm = "cpp_scripting_" + md5::hash(_getexepath().string());
		tp.append(enm);
		tempPath = tp.string();
		//std::filesystem::remove_all(tp);
		create_directories(tp);
	}
	~CLangBuilderWin() override {

	}
	const char* getModuleExtension() const override {
		return "dll";
	}
	void compile(cppProject& project) override {
		std::filesystem::path p = _getexepath();
		p.replace_extension("lib");
		std::string libpath = p.string();
		std::string opt = clangPath;
		opt += " ";
		std::string cumulate = project.options();
		for (int i = 0; i < project.filesList().size(); i++) {
			opt += "\"" + project.filesList()[i] + "\" ";
			cumulate += md5::file_hash(project.filesList()[i]);
		}
		cumulate += project.options() + libpath;

		opt += " \"" + libpath + "\" ";

		std::string md5s = md5::hash(cumulate);
		project.module() = tempPath + "/clang_temp_" + md5s + "." + getModuleExtension();
		if (!std::filesystem::exists(project.module())) {
#ifdef _WIN64
			opt += " -m64";
#endif			
			opt += " /D APICALL=\"__declspec(dllimport)\" ";
			opt += "/D EXPORT=\"extern \\\"C\\\" __declspec(dllexport)\" ";
			opt += project.includes();
			opt += " " + project.options() + " -o \"" + project.module() + "\" ";
			project.compileLog() = exec(opt.c_str());
			// std::cout << opt << "\n";
			std::cout << "Compilation result: " << project.compileLog() << "\n";
		}
		else {
			std::cout << "The compiled module is up to date.\n";
		}
	}

	std::string pathInHeap(const std::string& relative) {
		std::filesystem::path p = tempPath;
		p.append(relative);
		return p.string();
	}

	virtual std::string downloadPath() {
		return "https://github.com/llvm/llvm-project/releases/download/llvmorg-13.0.1/LLVM-13.0.1-win64.exe";
	}

	virtual bool valid() {
		return clangPath.length() > 0;
	}
};

inline cppModule* cppModule::create() {
	return new LibReferenceWin;
}

inline cppBuilder* cppBuilder::create() {
	return new CLangBuilderWin;
}


#endif