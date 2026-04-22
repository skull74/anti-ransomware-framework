#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

using namespace std;

const double ENTROPY_THRESHOLD = 7.5;


vector<string> CORE_OS_FILES = {
    "explorer.exe", "winlogon.exe", "dwm.exe", "sihost.exe",
    "svchost.exe", "csrss.exe", "smss.exe", "lsass.exe",
    "services.exe", "wininit.exe", "fontdrvhost.exe",
    "sgrmbroker.exe", "system", "registry", "taskhostw.exe",
    "conhost.exe", "cmd.exe", "python.exe", "py.exe", "pythonw.exe", 
    "pycharm64.exe", "new_guard.exe", "startmenuexperiencehost.exe", 
    "runtimebroker.exe", "searchapp.exe", "smartscreen.exe", 
    "securityhealthsystray.exe", "vboxtray.exe",
    "applicationframehost.exe", "ctfmon.exe"
};
vector<string> TRUSTED_WHITELIST = {
    "msmpeng.exe", "searchindexer.exe", "explorer.exe",
    "system", "svchost.exe", "pycharm64.exe", "python.exe", 
    "py.exe", "pythonw.exe", "new_guard.exe", "onedrive.exe"
};


double calculate_entropy(const string& file_path) {
    return 0.0;
}
string to_lower(const string& str) {
    string lower_str = str;
    transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    return lower_str;
}

int main() {
    cout << "Anti-Ransomware Framework Initialized.\n";
    return 0;
}
