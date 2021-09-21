#include <export.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>

testClass tc;
EXPORT
int main() {
    std::cout << "c++ version:" << __cplusplus << std::endl;
    std::string s = "Element of array\n";
    std::vector<std::string> v;
    v.push_back(s);
    // call the function of the class from the script
    tc.testfn(v[0].c_str());
    std::cout << "Wow, it works!\n";
    printf("printf works as well!\n");
    return 0;
}
EXPORT
void test123(int x){
    std::cout << x << "\n";
}
