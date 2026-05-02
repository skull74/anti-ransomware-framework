#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
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
    int wchars_num = MultiByteToWideChar(CP_UTF8, 0, file_path.c_str(), -1, NULL, 0);
    vector<wchar_t> wstr(wchars_num + 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, file_path.c_str(), -1, wstr.data(), wchars_num);
    HANDLE hFile = CreateFileW(wstr.data(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        char buffer[8192];
        DWORD bytesRead = 0;
        if (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
            CloseHandle(hFile);
            unordered_map<char, int> counts;
            for (size_t i = 0; i < bytesRead; ++i) counts[buffer[i]]++;
            double entropy = 0.0;
            for (auto const& pair : counts) {
                double p_x = (double)pair.second / bytesRead;
                entropy -= p_x * (log(p_x) / log(2.0));
            }
            return entropy;
        }
        CloseHandle(hFile);
    }
    HANDLE hFile = CreateFileW(wstr.data(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        char buffer[8192];
        DWORD bytesRead = 0;
        if (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
            CloseHandle(hFile);
            unordered_map<char, int> counts;
            for (size_t i = 0; i < bytesRead; ++i) counts[buffer[i]]++;
            double entropy = 0.0;
            for (auto const& pair : counts) {
                double p_x = (double)pair.second / bytesRead;
                entropy -= p_x * (log(p_x) / log(2.0));
            }
            return entropy;
        }
        CloseHandle(hFile);
    }
}

    size_t dot_pos = file_path.find_last_of(".");
    if (dot_pos == string::npos) return false;
    string ext = to_lower(file_path.substr(dot_pos));

    unordered_map<string, vector<unsigned char>> signatures = {
        {".jpg", {0xFF, 0xD8, 0xFF}}, {".png", {0x89, 0x50, 0x4E, 0x47}}, {".pdf", {0x25, 0x50, 0x44, 0x46}},
        {".zip", {0x50, 0x4B, 0x03, 0x04}}, {".docx", {0x50, 0x4B, 0x03, 0x04}}, {".xlsx", {0x50, 0x4B, 0x03, 0x04}}
    };
    if (signatures.find(ext) == signatures.end()) return false;
    return true;
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
