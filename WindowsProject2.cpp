// WindowsProject2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WindowsProject2.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
int wndWidth = CW_USEDEFAULT; 
int wndHeight = CW_USEDEFAULT;
HPEN penBlack = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
HBRUSH brMSRed = CreateSolidBrush(RGB(242, 80, 34));
HBRUSH brMSGreen = CreateSolidBrush(RGB(127, 186, 0));
HBRUSH brMSBlue = CreateSolidBrush(RGB(0, 164, 239));
HBRUSH brMSYellow = CreateSolidBrush(RGB(255, 185, 0));


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                DrawMSLogo(HDC hdc, int wndWidth, int wndHeight);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
        
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT2));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(WHITE_BRUSH);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, wndWidth, wndHeight, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SIZE:
        wndWidth = LOWORD(lParam);
        wndHeight = HIWORD(lParam);
        InvalidateRect(hWnd, NULL, true);
    break;   
    case WM_LBUTTONDOWN:
    {
        brMSRed = CreateHatchBrush(HS_CROSS, RGB(242, 80, 34));
        brMSGreen = CreateHatchBrush(HS_CROSS, RGB(127, 186, 0));
        brMSBlue = CreateHatchBrush(HS_CROSS, RGB(0, 164, 239));
        brMSYellow = CreateHatchBrush(HS_CROSS, RGB(255, 185, 0));

        InvalidateRect(hWnd, NULL, true);
    }
    break;   
    case WM_RBUTTONDOWN:
    {
        brMSRed = CreateSolidBrush(RGB(242, 80, 34));
        brMSGreen = CreateSolidBrush(RGB(127, 186, 0));
        brMSBlue = CreateSolidBrush(RGB(0, 164, 239));
        brMSYellow = CreateSolidBrush(RGB(255, 185, 0));

        InvalidateRect(hWnd, NULL, true);
    }
    break;
    case WM_PAINT:
    {
        if (true) {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...

            DrawMSLogo(hdc, wndWidth, wndHeight);

            EndPaint(hWnd, &ps);
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void DrawMSLogo(HDC hdc, int wndWidth, int wndHeight)
{
    SelectObject(hdc, penBlack);

    SelectObject(hdc, brMSRed);
    Rectangle(hdc, 0, 0, wndWidth / 2, wndHeight / 2);
    SelectObject(hdc, brMSGreen);
    Rectangle(hdc, wndWidth / 2, 0, wndWidth, wndHeight / 2);
    SelectObject(hdc, brMSBlue);
    Rectangle(hdc, 0, wndHeight / 2, wndWidth / 2, wndHeight);
    SelectObject(hdc, brMSYellow);
    Rectangle(hdc, wndWidth / 2, wndHeight / 2, wndWidth, wndHeight);
    
    return;
}
//BOOL DrawMSLogo(HDC hdc, int wndWidth, int wndHeight) 
//{
//    //HPEN pen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
//    //HPEN pen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
//    SelectObject(hdc, pen);
//
//    //HBRUSH br = CreateSolidBrush(RGB(255, 255, 255)); 
//    //SelectObject(hdc, br);
//
//    // RED (f25022) (242, 80, 34) brush
//    //br = (brushStyleFlag) ? CreateSolidBrush(RGB(242, 80, 34)) : CreateHatchBrush(HS_CROSS, RGB(242, 80, 34));
//    SelectObject(hdc, brMSRed);
//    Rectangle(hdc, 0, 0, wndWidth / 2, wndHeight / 2);
//    //if (!Rectangle(hdc, 0, 0, wndWidth / 2, wndHeight / 2))
//    //{
//    //    return 1;
//    //}
//    
//    // GREEN (7fba00) (127, 186, 0) brush
//    //br = (brushStyleFlag) ? CreateSolidBrush(RGB(127, 186, 0)) : CreateHatchBrush(HS_CROSS, RGB(127, 186, 0));
//    SelectObject(hdc, brMSGreen);
//    Rectangle(hdc, wndWidth / 2, 0, wndWidth, wndHeight / 2);
//    //if (!Rectangle(hdc, wndWidth / 2, 0, wndWidth, wndHeight / 2))
//    //{
//    //    return 1;
//    //}
//    
//    // BLUE (00a4ef) (0, 164, 239) brush
//    //br = (brushStyleFlag) ? CreateSolidBrush(RGB(0, 164, 239)) : CreateHatchBrush(HS_CROSS, RGB(0, 164, 239));
//    SelectObject(hdc, brMSBlue);
//    Rectangle(hdc, 0, wndHeight / 2, wndWidth / 2, wndHeight);
//    //if (!Rectangle(hdc, 0, wndHeight / 2, wndWidth / 2, wndHeight))
//    //{
//    //    return 1;
//    //}
//    
//    // YELLOW (ffb900) (255, 185, 0) brush
//    //br = (brushStyleFlag)? CreateSolidBrush(RGB(255, 185, 0)) : CreateHatchBrush(HS_CROSS, RGB(255, 185, 0));
//    SelectObject(hdc, brMSYellow);
//    Rectangle(hdc, wndWidth / 2, wndHeight / 2, wndWidth, wndHeight);
//    //if (!Rectangle(hdc, wndWidth / 2, wndHeight / 2, wndWidth, wndHeight))
//    //{
//    //    return 1;
//    //}
//
//    return 0;
//}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
