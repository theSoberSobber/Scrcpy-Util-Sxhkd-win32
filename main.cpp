#include <windows.h>
#include <iostream>
#include <string>
#include <winuser.h>

HWND g_hTextBox;
HWND g_hButton;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "ADB TCP/IP Connecter";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "ADB TCP/IP Connect",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 120,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    g_hTextBox = CreateWindowEx(
        0,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        10, 10, 260, 20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    g_hButton = CreateWindowEx(
        0,
        "BUTTON",
        "Connect",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 40, 100, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    SetFocus(g_hButton);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_COMMAND:
            if (reinterpret_cast<HWND>(lParam) == g_hButton) {
                int len = GetWindowTextLength(g_hTextBox) + 1;
                char* buffer = new char[len];
                GetWindowText(g_hTextBox, buffer, len);

                std::string ip = std::string(buffer);
                if(ip=="") ip = "192.168.1.29";

                std::string adbCommand = "adb connect " + ip + ":5555";
                FILE *pipe = _popen(adbCommand.c_str(), "r");
                if (pipe) {
                    char outputBuffer[128];
                    std::string output = "";
                    while (!feof(pipe)) {
                        if (fgets(outputBuffer, 128, pipe) != NULL)
                            output += outputBuffer;
                    }
                    _pclose(pipe);
                    MessageBox(NULL, output.c_str(), "Output", MB_OK | MB_ICONINFORMATION);
                }

                delete[] buffer;
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

