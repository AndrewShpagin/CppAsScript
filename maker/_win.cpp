#include "CLangScripter.h"
#include <Windows.h>
#include <shlobj_core.h>
#include "md5.h"
#include <filesystem>

static std::filesystem::path _getexepath() {
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

class LibReferenceWin : public LibReference {
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
		if(h) {
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

std::string exec( const char* cmd) {
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

class CLangBuilderWin : public CLangBuilder {
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
		std::filesystem::remove_all(tp);
		create_directories(tp);
	}
	~CLangBuilderWin() override {
		
	}
	const char* getModuleExtension() const override {
		return "dll";
	}
	void compile(CLangProject& project) override {
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
			std::cout << "Compilation result: " << project.compileLog() << "\n";
		} else {
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

LibReference* LibReference::create() {
	return new LibReferenceWin;
}

CLangBuilder* CLangBuilder::create() {
	return new CLangBuilderWin;
}
