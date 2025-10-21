// WindowsProject4-Server.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WindowsProject4-Server.h"

#define MAX_LOADSTRING 100

#include <windows.h>
#include <windowsx.h>
#include <winsock.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "WS2_32.lib")

char szBuf[1024];
DWORD cbWritten;
static HWND hwndEdit;
char mess[2048];                            // Повідомлення клієнту
char* m_mess = mess;

#define SERV_PORT 5000                      // Порт сервера
#define WSA_ACCEPT (WM_USER+0)
#define WSA_NETEVENT (WM_USER+1)
//#define MAX_LOADSTRING 100

HWND hWindow;
WSADATA wsaData;                            // Відомості про конкретну реалізацію інтерфейсу Windows Sockets
WORD wVersionRequested = MAKEWORD(1, 1);    // Номер версії Windows Sockets
int err = 0;
SOCKET srv_socket = INVALID_SOCKET;         // Сокет сервера
int ClientNum = -1;                         // Номер останнього клієнта
const int max_sock_number = 3;              // Максимальна кількість сокетів клієнтів
SOCKET sock[max_sock_number];               // Сокети кліентів
SOCKADDR_IN sockaddr[3];                    // Адреси кліентів

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
    LoadStringW(hInstance, IDC_WINDOWSPROJECT4SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT4SERVER));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT4SERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT4SERVER);
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

   HWND hWnd = CreateWindowW(szWindowClass, L"Server", WS_OVERLAPPEDWINDOW,
      590, 300, 420, 260, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   hWindow = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// Функції сервера

void ServerStart(HWND hWnd)
{
    if (srv_socket != INVALID_SOCKET)
    {
        MessageBoxA(hWnd, "Server already launched. Socket created", "Info", MB_OK |
            MB_ICONINFORMATION);
        return;
    }
    // створюємо сокет сервера для роботи з потоком даних
    srv_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_socket == INVALID_SOCKET)
    {
        MessageBoxA(hWnd, "Socket creation error!!!", "Error", MB_OK |
            MB_ICONERROR);
        return;
    }
    // Встановлюємо адресу IP та номер порту
    SOCKADDR_IN srv_address;
    srv_address.sin_family = AF_INET;
    srv_address.sin_port = htons(SERV_PORT);
    srv_address.sin_addr.s_addr = INADDR_ANY; // використовуємо адресу за замовчуванням (тобто будь-яка)
	// Зв'язуємо IP-адресу з сокетом
	if (SOCKET_ERROR == bind(srv_socket, (LPSOCKADDR)&srv_address, sizeof(srv_address)))
	{
		closesocket(srv_socket);
		MessageBoxA(hWnd, "Port connection error", "Error", MB_OK |
			MB_ICONSTOP);
		return;
	}
    // Чекаємо на встановлення зв'язку
    if (listen(srv_socket, 4) == SOCKET_ERROR)
    {
        closesocket(srv_socket);
        MessageBoxA(hWnd, "Error in waiting for connection", "Error", MB_OK);
        return;
    }
    // при спробі з'єднання головне вікно отримає повідомлення WSA_ACCEPT
    int rc = WSAAsyncSelect(srv_socket, hWnd, WSA_ACCEPT, FD_ACCEPT);
    if (rc)
    {
        closesocket(srv_socket);
        MessageBoxA(hWnd, "WSAAsyncSelect error", "Error", MB_OK);
        return;
    }
    // Виводимо повідомлення про запуск сервера
    SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)"Server running");
    //    UpdateWindow(hwndEdit);
}

void ServerStop(HWND hWnd)
{
    // скасовуємо надходження повідомлень у головну функцію вікна при виникненні будь-яких подій, пов'язаних із системою Windows Sockets
    WSAAsyncSelect(srv_socket, hWnd, 0, 0);
    if (srv_socket != INVALID_SOCKET) // якщо сокет було створено, закриваємо його
    {
        srv_socket = INVALID_SOCKET;
        closesocket(srv_socket);
        WSACleanup();
        return;
    }
    // виводимо повідомлення про зупинення сервера
    SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)"Server stopped");
    // UpdateWindow(hwndEdit);
}

BOOL AcceptClient(int j) // Підключення клієнта
{
    int sockaddr_len = sizeof(sockaddr[j]);
    sock[j] = accept(srv_socket, (LPSOCKADDR)&sockaddr[j], (int FAR*) &sockaddr_len);
    if (sock[j] != INVALID_SOCKET)
        if (!WSAAsyncSelect(sock[j], hWindow, WSA_NETEVENT, FD_READ | FD_CLOSE))
            return TRUE;
    closesocket(sock[j]);
    return FALSE;
}

void DisconnectClient(int j) // Відключення клієнта
{
    WSAAsyncSelect(sock[j], hWindow, 0, 0);
    closesocket(sock[j]);
}

void WndProc_OnWSAAccept(HWND hWnd, LPARAM lParam)
{
    // при помилці скасовуємо надходження повідомлень до головного вікна програми
    if (WSAGETSELECTERROR(lParam)) {
        MessageBoxA(hWnd, "Accept error", "Error", MB_OK);
        WSAAsyncSelect(srv_socket, hWnd, 0, 0);
        return;
    }
    if (ClientNum == max_sock_number - 1) {
        MessageBoxA(hWnd, "Clients amount > 3\r\n", "Connect is prohibited!", MB_OK);
        return;
    }
    ClientNum++;
    if (!AcceptClient(ClientNum)) {
        MessageBoxA(hWnd, "Error connecting with client", "Error", MB_OK);
        return;
    }
    // додаємо клієнта
    sprintf_s(szBuf, "Added Client %i\r\nAddress: IP=%s Port=%u\r\n \0", ClientNum + 1,
        inet_ntoa(sockaddr[ClientNum].sin_addr), htons(sockaddr[ClientNum].sin_port));
    SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)szBuf);
}

void WndProc_OnWSANetEvent(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    char szTemp[256], szMess[256];
    int Number;
    // дізнаємося від якого клієнта надійшло повідомлення, => Number
    for (int i = 0; i < max_sock_number; i++) {
        if (sock[i] == (SOCKET)wParam) {
            Number = i;
            break;
        }
    }
    // якщо на сокеті виконується передача даних, приймаємо та відображаємо їх
    if (WSAGETSELECTEVENT(lParam) == FD_READ)
    {
        int rc = recv((SOCKET)wParam, szTemp, sizeof(szTemp), 0);
        if (rc) {
            szTemp[rc] = '\0';
            sprintf_s(m_mess, 200, "Client %i data:\r\n%s", Number + 1, szTemp);
            SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
        }
    }
    // якщо з'єднання завершено - виводимо повідомлення про це
    if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
    {
        WSAAsyncSelect(sock[Number], hWindow, 0, 0);
        closesocket(sock[Number]);
        sprintf_s(szTemp, 30, "Client %i finished task", Number + 1);
        SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)szTemp);
    }
}
void SendToClient(int j)
{
    if (j > ClientNum) return;
    cbWritten = SendMessageA(hwndEdit, WM_GETTEXTLENGTH, 0, 0);
    SendMessageA(hwndEdit, WM_GETTEXT, (WPARAM)cbWritten, (LPARAM)szBuf);
    szBuf[cbWritten] = '\0';
    if (send(sock[j], szBuf, strlen(szBuf), 0) != SOCKET_ERROR)
    {
        sprintf_s(mess, "Data to Client %d: %s\r\n ", j + 1, szBuf);
        SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
    }
    else
        SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)"Error in message sending.\r\n");
}
void ClientOff(HWND hWnd, int j)
{
    if (j > ClientNum) return;
    sprintf_s(szBuf, "Disconnect Client %i ?", j + 1);
    if (IDYES == MessageBoxA(hWnd, szBuf, "Question", MB_YESNO | MB_ICONQUESTION))
    {
        sprintf_s(szBuf, "Client %i is disconnected", j + 1);
        if (send(sock[j], szBuf, strlen(szBuf), 0) != SOCKET_ERROR)
        {
            SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)szBuf);
            DisconnectClient(j);
        }
        else
            SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)"Disconnect error\r\n");
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
        hwndEdit = CreateWindow( // Створюємо віконце для виведення даних від процесів
            TEXT("EDIT"), NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
            0, 0, 400, 200, hWnd, NULL, hInst, NULL);
        // Початкова інформайція про тип та статус підключення
        err = WSAStartup(wVersionRequested, &wsaData);
        if (err) {
            MessageBoxA(hWnd, "WSAStartup Error", "ERROR", MB_OK | MB_ICONSTOP);
            return FALSE;
        }
        sprintf_s(m_mess, 100, "Using %s \r\nStatus: %s\r\n",
            wsaData.szDescription, wsaData.szSystemStatus);
        SendMessageA(hwndEdit, WM_SETTEXT, 0, (LPARAM)m_mess);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_SERVER_START:
                ServerStart(hWnd);
                break;
            case ID_SERVER_STOP:
                ServerStop(hWnd);
                break;
            case ID_SENDTOCLIENT_1:
                SendToClient(0);
                break;
            case ID_SENDTOCLIENT_2:
                SendToClient(1);
                break;
            case ID_SENDTOCLIENT_3:
                SendToClient(2);
                break;
            case ID_CLIENTOFF_1:
                ClientOff(hWnd, 0);
                break;
            case ID_CLIENTOFF_2:
                ClientOff(hWnd, 1);
                break;
            case ID_CLIENTOFF_3:
                ClientOff(hWnd, 2);
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
        WSACleanup(); // визволяємо задіяні мережеві та й не тільки ресурси
        PostQuitMessage(0);
        break;
    case WSA_ACCEPT: // витягує перше підключення в черзі очікуваних підключень до сокету
        WndProc_OnWSAAccept(hWnd, lParam);
        break;
    case WSA_NETEVENT: //  викликає запис та вилучення мережевих дій та помилок через WSAEnumNetworkEvents
		WndProc_OnWSANetEvent(hWnd, wParam, lParam);
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
