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
#include <wintrust.h>
#include <softpub.h>
#include <mscat.h>


#pragma comment(lib, "wintrust.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "psapi.lib")
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


bool VerifyEmbeddedSignature(LPCWSTR pwszSourceFile) {
    LONG lStatus;
    WINTRUST_FILE_INFO FileData;
    memset(&FileData, 0, sizeof(FileData));
    FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
    FileData.pcwszFilePath = pwszSourceFile;
    GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    WINTRUST_DATA WinTrustData;
    memset(&WinTrustData, 0, sizeof(WinTrustData));
    WinTrustData.cbStruct = sizeof(WinTrustData);
    WinTrustData.dwUIChoice = WTD_UI_NONE;
    WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE; 
    WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
    WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY;
    WinTrustData.pFile = &FileData;
    lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);
    WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);
    return (lStatus == ERROR_SUCCESS);
}
bool VerifySystemSignature(LPCWSTR pwszSourceFile) {
    if (VerifyEmbeddedSignature(pwszSourceFile)) return true;
    HCATADMIN hCatAdmin = NULL;
    if (!CryptCATAdminAcquireContext(&hCatAdmin, NULL, 0)) return false;
    HANDLE hFile = CreateFileW(pwszSourceFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) { CryptCATAdminReleaseContext(hCatAdmin, 0); return false; }
    DWORD cbHash = 0;
    CryptCATAdminCalcHashFromFileHandle(hFile, &cbHash, NULL, 0);
    if (cbHash == 0) { CloseHandle(hFile); CryptCATAdminReleaseContext(hCatAdmin, 0); return false; }
    vector<BYTE> pbHash(cbHash);
    if (!CryptCATAdminCalcHashFromFileHandle(hFile, &cbHash, pbHash.data(), 0)) { CloseHandle(hFile); CryptCATAdminReleaseContext(hCatAdmin, 0); return false; }
    CloseHandle(hFile);
    HCATINFO hCatInfo = CryptCATAdminEnumCatalogFromHash(hCatAdmin, pbHash.data(), cbHash, 0, NULL);
    if (!hCatInfo) { CryptCATAdminReleaseContext(hCatAdmin, 0); return false; }
    CATALOG_INFO catInfo = {0};
    catInfo.cbStruct = sizeof(CATALOG_INFO);
    CryptCATCatalogInfoFromContext(hCatInfo, &catInfo, 0);
    WINTRUST_CATALOG_INFO wtc = {0};
    wtc.cbStruct = sizeof(WINTRUST_CATALOG_INFO);
    wtc.pcwszCatalogFilePath = catInfo.wszCatalogFile;
    wtc.pcwszMemberFilePath = pwszSourceFile;
    wtc.pbCalculatedFileHash = pbHash.data();
    wtc.cbCalculatedFileHash = cbHash;
    WINTRUST_DATA WinTrustData = {0};
    WinTrustData.cbStruct = sizeof(WinTrustData);
    WinTrustData.dwUIChoice = WTD_UI_NONE;
    WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    WinTrustData.dwUnionChoice = WTD_CHOICE_CATALOG;
    WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY;
    WinTrustData.pCatalog = &wtc;
    GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    LONG lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);
    WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);
    CryptCATAdminReleaseCatalogContext(hCatAdmin, hCatInfo, 0);
    CryptCATAdminReleaseContext(hCatAdmin, 0);
    return (lStatus == ERROR_SUCCESS);
}
void execute_kill_switch(const string& attacked_file_path) {
    bool threat_killed = false;
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hProcessSnap, &pe32)) {
        do {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if (hProcess != NULL) {
                DWORD myPID = GetCurrentProcessId();
                if (pe32.th32ProcessID == myPID || pe32.th32ProcessID == 0 || pe32.th32ProcessID == 4) {
                    CloseHandle(hProcess);
                    continue; 
                }
                CloseHandle(hProcess);
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }
    CloseHandle(hProcessSnap);
    cout << "\n[>>>] INITIATING EDR LINEAGE & HEURISTIC TERMINATION...\n";

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
