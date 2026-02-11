#include <windows.h>
#include <commdlg.h>
#include <string>

// IDs for dialog items
#define IDC_WIDTH 1001
#define IDC_HEIGHT 1002
#define IDC_FILENAME 1003
#define IDC_OK 1004
#define IDC_CANCEL 1005

// Dialog procedure to handle user input
LRESULT CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static int* pData;
    switch (message) {
    case WM_CREATE:
        pData = (int*)((CREATESTRUCT*)lParam)->lpCreateParams;
        CreateWindowEx(0, "STATIC", "Width:", WS_CHILD | WS_VISIBLE, 10, 10, 50, 20, hDlg, NULL, GetModuleHandle(NULL), NULL);
        CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 70, 10, 100, 20, hDlg, (HMENU)IDC_WIDTH, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, "STATIC", "Height:", WS_CHILD | WS_VISIBLE, 10, 40, 50, 20, hDlg, NULL, GetModuleHandle(NULL), NULL);
        CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 70, 40, 100, 20, hDlg, (HMENU)IDC_HEIGHT, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, "STATIC", "Filename:", WS_CHILD | WS_VISIBLE, 10, 70, 50, 20, hDlg, NULL, GetModuleHandle(NULL), NULL);
        CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 70, 70, 100, 20, hDlg, (HMENU)IDC_FILENAME, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, "BUTTON", "OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 10, 100, 75, 25, hDlg, (HMENU)IDC_OK, GetModuleHandle(NULL), NULL);
        CreateWindowEx(0, "BUTTON", "Cancel", WS_CHILD | WS_VISIBLE, 95, 100, 75, 25, hDlg, (HMENU)IDC_CANCEL, GetModuleHandle(NULL), NULL);
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_OK: {
            char buffer[10];
            GetDlgItemText(hDlg, IDC_WIDTH, buffer, sizeof(buffer));
            pData[0] = atoi(buffer);
            GetDlgItemText(hDlg, IDC_HEIGHT, buffer, sizeof(buffer));
            pData[1] = atoi(buffer);
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        case IDC_CANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    case WM_CLOSE:
        EndDialog(hDlg, IDCANCEL);
        return TRUE;
    }
    return DefWindowProc(hDlg, message, wParam, lParam);
}

// Function to create and show the dialog
bool ShowResolutionDialog(HINSTANCE hInstance, HWND hWnd, int* data) {
    // Register the dialog class
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = DialogProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ResolutionDialog";
    RegisterClass(&wc);

    // Create the dialog window
    HWND hDlg = CreateWindowEx(
        0, "ResolutionDialog", "Enter Resolution",
        WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX),
        CW_USEDEFAULT, CW_USEDEFAULT, 200, 150,
        hWnd, NULL, hInstance, data);

    if (!hDlg) {
        MessageBox(hWnd, "Failed to create dialog", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // Show and run the dialog
    ShowWindow(hDlg, SW_SHOW);
    UpdateWindow(hDlg);

    MSG msg;
    BOOL ret;
    while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (ret == -1) {
            return false;
        }
        if (!IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam == IDOK;
}

// Function to show file open dialog
std::string GetFileName(HWND hWnd) {
    OPENFILENAME ofn;
    char szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    }
    return "";
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HWND hWnd = GetConsoleWindow();
    int data[2]; // data[0] = width, data[1] = height

    // Show dialog to get width and height
    if (!ShowResolutionDialog(hInstance, hWnd, data)) {
        MessageBox(hWnd, "Dialog canceled", "Info", MB_OK);
        return 0;
    }

    // Show file open dialog to get filename
    std::string filename = GetFileName(hWnd);
    if (filename.empty()) {
        MessageBox(hWnd, "No file selected", "Info", MB_OK);
        return 0;
    }

    // Use data[0], data[1], and filename for your 3D display setup
    // For now, just display the collected information
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "Width: %d\nHeight: %d\nFilename: %s", data[0], data[1], filename.c_str());
    MessageBox(hWnd, buffer, "Collected Information", MB_OK);

    // Your full-screen 3D display code goes here

    return 0;
}
