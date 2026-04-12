#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

const double ENTROPY_THRESHOLD = 7.5;

vector<string> TRUSTED_WHITELIST = {
    "msmpeng.exe", "searchindexer.exe", "explorer.exe",
    "system", "svchost.exe", "pycharm64.exe", "python.exe", 
    "py.exe", "pythonw.exe", "new_guard.exe", "onedrive.exe"
};

string to_lower(const string& str) {
    string lower_str = str;
    transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    return lower_str;
}

int main() {
    cout << "Anti-Ransomware Framework Initialized.\n";
    return 0;
}
