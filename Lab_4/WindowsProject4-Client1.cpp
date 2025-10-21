// WindowsProject4-Client1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WindowsProject4-Client1.h"

#define MAX_LOADSTRING 100

// Клієнт-серверні include, постійні та змінні
#include <winsock.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "WS2_32.lib")

char szBuf[1024];
DWORD cbWritten;
int flag = 0;
static HWND hwndEdit;
char mess[2048];                            // Повідомлення від сервера
char* m_mess = mess;
#define SERV_PORT 5000                      // Порт сервера
#define WSA_NETEVENT (WM_USER+1)
WSADATA wsaData;                            // Відомості про конкретнуреалізацію інтерфейсу Windows Sockets
WORD wVersionRequested = MAKEWORD(1, 1);    // Номер версії Windows Sockets
int err = 0;
SOCKET cln_socket = INVALID_SOCKET;         // Сокет сервера
static PHOSTENT phe;
SOCKADDR_IN dest_sin;                       // Адреса сервера
char szHostName[128] = "localhost";         // Ім'я хоста

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
    LoadStringW(hInstance, IDC_WINDOWSPROJECT4CLIENT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT4CLIENT1));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT4CLIENT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT4CLIENT1);
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

   HWND hWnd = CreateWindowW(szWindowClass, L"Client 1", WS_OVERLAPPEDWINDOW,
      1000, 200, 420, 260, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// Функції клієнта

BOOL SetConnection(HWND hWnd)
{
    cln_socket = socket(AF_INET, SOCK_STREAM, 0); // створюємо сокет
    if (cln_socket == INVALID_SOCKET)
    {
        MessageBoxA(hWnd, "Socket error", "Error", MB_OK | MB_ICONSTOP);
        return FALSE;
    }
    phe = gethostbyname(szHostName); // Визначаємо адресу вузла
    if (phe == NULL)
    {
        closesocket(cln_socket);
        MessageBoxA(hWnd, "Host address is undefined", "Error", MB_OK | MB_ICONSTOP);
        return FALSE;
    }
    dest_sin.sin_family = AF_INET;     // Задаємо тип адреси
    dest_sin.sin_port = htons(SERV_PORT);     // Встановлюємо номер порту
    // Копіюємо адресу вузла
    memcpy((char FAR*) & (dest_sin.sin_addr), phe->h_addr, phe->h_length);
	// Встановлюємо з'єднання
	if (connect(cln_socket, (PSOCKADDR)&dest_sin, sizeof(dest_sin)) == SOCKET_ERROR)
	{
		closesocket(cln_socket);
		MessageBoxA(hWnd, "Connection error", "Error", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
    // при спробі з'єднання головне вікно отримає повідомлення WSA_ACCEPT
    if (WSAAsyncSelect(cln_socket, hWnd, WSA_NETEVENT, FD_READ | FD_CLOSE))
    {
        MessageBoxA(hWnd, "WSAAsyncSelect error", "Error", MB_OK);
        return FALSE;
    }
    // Виводимо повідомлення про встановлення з'єднання з вузлом
    SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)"Connection is set!");
    return TRUE;
}

void SendMsg(HWND hWnd) // повідомлення від клієнта 1
{
    RECT rect;
    GetWindowRect(hWnd, &rect);

    char touchscreen[100];
    int digitizer = GetSystemMetrics(SM_DIGITIZER); // SM_MAXIMUMTOUCHES
    if (digitizer != 0) 
        strcpy_s(touchscreen, _countof(touchscreen), "Digitizer connected");
    else
        strcpy_s(touchscreen, _countof(touchscreen), "Digitizer not connected");

    char mouseWheel[100];
    int wheelPresent = GetSystemMetrics(SM_MOUSEWHEELPRESENT);
    if (wheelPresent != 0) // частоти подій від миші немає, ця метрика визначає наявність колеса прокрутки
        strcpy_s(mouseWheel, _countof(mouseWheel), "Mouse wheel exists");
    else
        strcpy_s(mouseWheel, _countof(mouseWheel), "Mouse wheel doesn't exist");

    HDC hdc = GetDC(NULL); // Контекст для DPI

    sprintf_s(szBuf, "%s. \r\n%s. \r\nDPI horisontally: %lu. \r\n ", 
        touchscreen, mouseWheel, GetDeviceCaps(hdc, LOGPIXELSX));
    if (send(cln_socket, szBuf, strlen(szBuf), 0) != SOCKET_ERROR)
    {
        sprintf_s(m_mess, 100, "\r\nData to Server:\r\n%s", szBuf);
        SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
    }
    else
    {
        sprintf_s(m_mess, 100, "Client %d: Error in message sending.\r\n ", 1);
        SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
    }
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
		hwndEdit = CreateWindow( // Створюємо доч. вікно для виведення даних від процесів
			TEXT("EDIT"), NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
			0, 0, 400, 200, hWnd, NULL, hInst, NULL);
		// WSAStartup дозволяє програмі або DLL вказувати потрібну версію Windows Sockets 
        // і отримувати деталі конкретної реалізації Windows Sockets
		err = WSAStartup(wVersionRequested, &wsaData);
		if (err) {
			MessageBoxA(hWnd, "WSAStartup Error", "ERROR", MB_OK | MB_ICONSTOP);
			return FALSE;
		}
		sprintf_s(mess, "Using %s \r\nStatus: %s\r\n ",
			wsaData.szDescription, wsaData.szSystemStatus);
		SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)mess);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_CLIENT_SETCONNECTION:
                SetConnection(hWnd);
                break;
            case ID_CLIENT_SENDMSG:
                SendMsg(hWnd);
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
    case WSA_NETEVENT:
        // якщо на сокеті виконується передача даних - приймаємо та відображаємо їх
        if (WSAGETSELECTEVENT(lParam) == FD_READ)
        {
            int rc = recv(cln_socket, szBuf, sizeof(szBuf), 0);
            if (rc)
            {
                szBuf[rc] = '\0';
                sprintf_s(mess, /*"%s \r\n */"Data from server: %s\r\n ", /*mess,*/
                    szBuf);
                SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
            }
        }
        // якщо з'єднання завершено, виводимо повідомлення про це
        if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
            MessageBoxA(hWnd, "Server is closed", "Server", MB_OK);
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
