// WindowsProject5-Server.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WindowsProject5-Server.h"

#define MAX_LOADSTRING 100

static HWND hwndEdit;
char mess[2048];
char* m_mess = mess;
#include <stdlib.h>
#include <stdio.h>
#include <string>
HANDLE hMailslot;
LPCWSTR MailslotName = L"\\\\.\\mailslot\\lab7"; // ім’я скриньки
//LPCSTR MailslotName = "\\\\.\\mailslot\\lab7"; // ім’я скриньки
DWORD cbRead;                       // кількість прийнятих байтів даних
char szBuf[512];                    // буфер
#define IDT_TIMER 1001              // таймер читача поштової скрині

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_WINDOWSPROJECT5SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT5SERVER));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT5SERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT5SERVER);
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
      CW_USEDEFAULT, 0, 440, 540, nullptr, nullptr, hInstance, nullptr);

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
    case WM_CREATE:
        hwndEdit = CreateWindow(TEXT("EDIT"), TEXT("Lab work - Mailbox.\r\n"),
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE |
            ES_AUTOVSCROLL, 20, 10, 400, 460, hWnd, NULL, hInst, NULL);
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case IDT_TIMER:
            DWORD cbMessage, cMessage, cbRead;
            BOOL fResult = GetMailslotInfo(hMailslot, NULL, &cbMessage, &cMessage, NULL);
            if (fResult && cMessage != 0)
            {
                if (!ReadFile(hMailslot, szBuf, 512, &cbRead, NULL))
                {
                    sprintf_s(mess, "%s\r\nReadFile failed. Error: %d\r\n", m_mess, GetLastError());
                    SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
                    break;
                }
                sprintf_s(mess, "%s\r\n%s\r\n", m_mess, szBuf);
                SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
            }
            break;
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_MAILSLOT_CREATE:
                sprintf_s(mess, "Mailbox\r\n");
                SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
                // створення та запуск поштової скрині
                hMailslot = CreateMailslot(MailslotName, 0, MAILSLOT_WAIT_FOREVER, NULL);
                if (hMailslot == INVALID_HANDLE_VALUE)
                {
                    sprintf_s(mess, "%s \r\n Mail Server Error: %d \r\n ", m_mess,
                        GetLastError());
                    SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
                    break;
                }
                sprintf_s(mess, "%s\r\nServer launched\r\n", m_mess);
                SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
                SetTimer(hWnd, IDT_TIMER, 1000, (TIMERPROC)NULL); //запуск таймеру
                break;
            case ID_MAILSLOT_CLOSE:
                //закриття поштової скрині
                if (CloseHandle(hMailslot))
                {
                    sprintf_s(mess, "%s\r\nServer closed\r\n", m_mess);
                    SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
                    KillTimer(hWnd, IDT_TIMER); //зупинка таймеру
                }
                else
                {
                    sprintf_s(mess, "%s \r\n Mail Server Error: %d \r\n ", m_mess,
                        GetLastError());
                    SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
                }
				break;
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
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
