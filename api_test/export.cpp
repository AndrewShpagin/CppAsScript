#include <cstdio>

#define APICALL __declspec(dllexport)

#include "export.h"

APICALL void test(const char* res) {
	printf("%s", res);
}

void testClass::testfn(const char* x) {
	printf("%s", x);
}
