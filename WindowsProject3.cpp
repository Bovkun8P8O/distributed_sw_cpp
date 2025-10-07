// WindowsProject3.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WindowsProject3.h"
#include <vector>
#include <iostream>
#include <stdio.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                        // current instance
WCHAR szTitle[MAX_LOADSTRING];                          // the title bar text
WCHAR szWindowClass[MAX_LOADSTRING];                    // the main window class name
HWND hWnd;                                              // window handler

int wndWidth = 1000;                                    // розміри вікна (для правильного ділення областей)
int wndHeight = 700;

const int iSTR_SIZE = 100;

// більше потоків - частіше оновлення ресурсів/блоків
char sz0_AndNaturalsStr[iSTR_SIZE];                     // рядок потоку 1
char szFibonacciStr[iSTR_SIZE];                         // рядок потоку 2
char szThreadCodeStr[iSTR_SIZE];                        // рядок потоку 3 (для коду потоку)
char szThreadCallsCountersStr[iSTR_SIZE];               // рядок потоку 4

uint64_t i0_Naturals = 0;                               // число потоку 1
uint64_t iFibonacci_n = 0;                              // число потоку 2
uint64_t iFibonacci_n_2 = 0;                            // число потоку 2
uint64_t iFibonacci_n_1 = 1;                            // число потоку 2
int iSeed = time(NULL);                                 // ключ для ГПВЧ для потоку 3 

int iThread1Counter = 0;                                // лічильники виклику потоків 1-3 для потоку 4
int iThread2Counter = 0;                                // (якщо в потоку 4 не буде іншого доступу до кількості виклику інших)
int iThread3Counter = 0;

std::vector<HANDLE> vhThreads;                          // дескриптори потоків (вектор для зручнішої динаміки)
std::vector<DWORD> vdwThreadIDs;                        // ідентифікатори потоків

                                                        // прямокутники для потоків
static RECT rt1 = { 0, 0, wndWidth, wndHeight / 4 };
static RECT rt2 = { 0, rt1.bottom, wndWidth, rt1.bottom + wndHeight / 4 };
static RECT rt3 = { 0, rt2.bottom, wndWidth, rt2.bottom + wndHeight / 4 };
static RECT rt4 = { 0, rt3.bottom, wndWidth, rt3.bottom + wndHeight / 4 };

static RECT rt3_rand;                                   // прямокутник випадкового кольору та розміру (в межах rt3)
HBRUSH hRandBrush = CreateSolidBrush(RGB(0, 0, 0));     // кисть для rt3_rand

HANDLE hMutexType1;                                     // м'ютекс для потоків типу 1
HANDLE hMutexType2;                                     // м'ютекс для потоків типу 2
HANDLE hMutexType3;                                     // м'ютекс для потоків типу 3
HANDLE hMutexType4;                                     // м'ютекс для потоків типу 4

HANDLE hEvent;                                          // подія для запуску/зупинки потоків (замість StopThread)

DWORD dwThreadToExit = 0;                               // цільовий потік для вимкнення 
static BOOL isExitThread = FALSE;                       // прапор для виходу з потоку, коли відомий dwThreadToExit

HPEN hBlackPen = CreatePen(PS_SOLID, 1, BLACK_PEN);
HBRUSH hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI        ThreadProc1(LPVOID lpParam);        // натуральні числа
DWORD WINAPI        ThreadProc2(LPVOID lpParam);        // Фібоначчі
DWORD WINAPI        ThreadProc3(LPVOID lpParam);        // випадкові прямокутники
DWORD WINAPI        ThreadProc4(LPVOID lpParam);        // підрахунок викликів

int CreateThreadSimple(LPTHREAD_START_ROUTINE ThreadFunc, LPVOID lpParam); // спрощене створення потока
void WaitForThreadsAndSyncHandles();                    // очікування всіх дескрипторів
void CloseHandles();                                    // закриття дескрипторів потоків, м'ютексів та події



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    srand(iSeed);

    hMutexType1 = CreateMutexW(NULL, FALSE, NULL);
    hMutexType2 = CreateMutexW(NULL, FALSE, NULL);
    hMutexType3 = CreateMutexW(NULL, FALSE, NULL);
    hMutexType4 = CreateMutexW(NULL, FALSE, NULL);

    hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT3));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT3);
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
            case ID_THREADS_START:
                if (vhThreads.size() != 0) {
                    SetEvent(hEvent);
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                }

                if (CreateThreadSimple(ThreadProc1, NULL) == 1) {
                    WaitForThreadsAndSyncHandles();
                    CloseHandles();
                    return 1;
                }
                if (CreateThreadSimple(ThreadProc2, NULL) == 1) {
                    WaitForThreadsAndSyncHandles();
                    CloseHandles();
                    return 1;
                }
                if (CreateThreadSimple(ThreadProc3, NULL) == 1) {
                    WaitForThreadsAndSyncHandles();
                    CloseHandles();
                    return 1;
                }
                if (CreateThreadSimple(ThreadProc4, NULL) == 1) {
                    WaitForThreadsAndSyncHandles();
                    CloseHandles();
                    return 1;
                }
                break;

            case ID_THREADS_STOP:
                WaitForMultipleObjects(vhThreads.size(), (HANDLE*)&vhThreads, TRUE, INFINITE);
                ResetEvent(hEvent);
                InvalidateRect(hWnd, NULL, TRUE);
                break;

            case ID_CREATE_TYPE1:
                if (CreateThreadSimple(ThreadProc1, NULL) == 1) {
                    WaitForThreadsAndSyncHandles();
                    CloseHandles();
                    return 1;
                }
                break;
            case ID_CREATE_TYPE2:
                if (CreateThreadSimple(ThreadProc2, NULL) == 1) {
                    WaitForThreadsAndSyncHandles();
                    CloseHandles();
                    return 1;
                }
                break;
            case ID_CREATE_TYPE3:
                if (CreateThreadSimple(ThreadProc3, NULL) == 1) {
                    WaitForThreadsAndSyncHandles();
                    CloseHandles();
                    return 1;
                }
                break;
            case ID_CREATE_TYPE4:
                if (CreateThreadSimple(ThreadProc4, NULL) == 1) {
                    WaitForThreadsAndSyncHandles();
                    CloseHandles();
                    return 1;
                }
                break;

            case ID_THREADS_DESTROYLAST: // destroying last created
                dwThreadToExit = vdwThreadIDs.back(); 
                isExitThread = TRUE;
                 
                if (vhThreads.size() > 0 && dwThreadToExit == vdwThreadIDs.back()) {
                    CloseHandle(vhThreads.back());
                    vdwThreadIDs.pop_back();
                    vhThreads.pop_back();
                }
                break;
            
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                WaitForMultipleObjects(vhThreads.size(), (HANDLE *)&vhThreads, TRUE, INFINITE);
                CloseHandles();
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

        rt1 = { 0, 0, wndWidth, wndHeight / 4 };
        rt2 = { 0, rt1.bottom, wndWidth, rt1.bottom + wndHeight / 4 };
        rt3 = { 0, rt2.bottom, wndWidth, rt2.bottom + wndHeight / 4 };
        rt4 = { 0, rt3.bottom, wndWidth, rt3.bottom + wndHeight / 4 };

        InvalidateRect(hWnd, NULL, true);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            
            // Потік 1
            FrameRect(hdc, &rt1, hBlackBrush);
            DrawText(hdc, LPCSTR(sz0_AndNaturalsStr), strlen(sz0_AndNaturalsStr), &rt1, DT_SINGLELINE | DT_CENTER); 

            // Потік 2
            FrameRect(hdc, &rt2, hBlackBrush);
            DrawText(hdc, LPCSTR(szFibonacciStr), strlen(szFibonacciStr), &rt2, DT_SINGLELINE | DT_CENTER);         

            // Потік 3
            FrameRect(hdc, &rt3, hBlackBrush);
            DrawText(hdc, LPCSTR(szThreadCodeStr), strlen(szThreadCodeStr), &rt3, DT_SINGLELINE | DT_CENTER);
            SelectObject(hdc, hRandBrush);
            Rectangle(hdc, rt3_rand.left, rt3_rand.top, rt3_rand.right, rt3_rand.bottom);

            // Потік 4
            FrameRect(hdc, &rt4, hBlackBrush);
            DrawText(hdc, LPCSTR(szThreadCallsCountersStr), strlen(szThreadCallsCountersStr), &rt4, DT_SINGLELINE | DT_CENTER);

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

// Потік n++
DWORD WINAPI ThreadProc1(LPVOID lpParam) {
    DWORD dwCurrThrID = GetCurrentThreadId();

    std::cout << "Потік " << dwCurrThrID << " типу 1 очікує сигналу...\n";
    WaitForSingleObject(hMutexType1, INFINITE);
    while (WaitForSingleObject(hEvent, INFINITE) != WAIT_ABANDONED ||
           WaitForSingleObject(hEvent, INFINITE) != WAIT_FAILED)
    { 
        if (isExitThread && dwThreadToExit == dwCurrThrID) {
            std::cout << "Потік " << dwCurrThrID << " типу 1 вимикається.\n";
            ExitThread(0);
        }

        std::cout << "Потік " << dwCurrThrID << " типу 1 отримав сигнал і виконується.\n";
        Sleep(1000);
        iThread1Counter += 1;

        i0_Naturals += 1;

        sprintf_s(sz0_AndNaturalsStr, (size_t)iSTR_SIZE - 1, 
                  "Thread: %d; n: %I64u", 
                  dwCurrThrID, i0_Naturals);
        InvalidateRect(hWnd, &rt1, TRUE);

        std::cout << "Потік " << dwCurrThrID << " типу 1 завершує роботу.\n";
        ReleaseMutex(hMutexType1);
    }
    return 0;
}

// Потік Fib
DWORD WINAPI ThreadProc2(LPVOID lpParam) {
    DWORD dwCurrThrID = GetCurrentThreadId();

    std::cout << "Потік " << dwCurrThrID << " типу 2 очікує сигналу...\n";
    WaitForSingleObject(hMutexType2, INFINITE);
    while (WaitForSingleObject(hEvent, INFINITE) != WAIT_ABANDONED || 
           WaitForSingleObject(hEvent, INFINITE) != WAIT_FAILED)
    {                                                                                                           
        if (isExitThread && dwThreadToExit == dwCurrThrID) {
            std::cout << "Потік " << dwCurrThrID << " типу 1 вимикається.\n";
            ExitThread(0);
        }

        std::cout << "Потік " << dwCurrThrID << " типу 2 отримав сигнал і виконується.\n";
        Sleep(1000);
        iThread2Counter += 1;

        if (iThread2Counter == 1) {
            sprintf_s(szFibonacciStr, (size_t)iSTR_SIZE - 1,
                "Thread: %d; n: %I64u",
                dwCurrThrID, iFibonacci_n_2);
            InvalidateRect(hWnd, &rt2, TRUE);

            std::cout << "Потік " << dwCurrThrID << " типу 2 завершує роботу.\n";
            ReleaseMutex(hMutexType2);
        }
        else if (iThread2Counter == 2) {
            sprintf_s(szFibonacciStr, (size_t)iSTR_SIZE - 1,
                "Thread: %d; n: %I64u",
                dwCurrThrID, iFibonacci_n_1);
            InvalidateRect(hWnd, &rt2, TRUE);

            std::cout << "Потік " << dwCurrThrID << " типу 2 завершує роботу.\n";
            ReleaseMutex(hMutexType2);
        }
        else {
            iFibonacci_n = iFibonacci_n_2 + iFibonacci_n_1;
            iFibonacci_n_2 = iFibonacci_n_1;
            iFibonacci_n_1 = iFibonacci_n;

            sprintf_s(szFibonacciStr, (size_t)iSTR_SIZE - 1,
                "Thread: %d; n: %I64u",
                dwCurrThrID, iFibonacci_n);
            InvalidateRect(hWnd, &rt2, TRUE);

            std::cout << "Потік " << dwCurrThrID << " типу 2 завершує роботу.\n";
            ReleaseMutex(hMutexType2);
        }
    }
    return 0;
}

// Потік прямокутників
DWORD WINAPI ThreadProc3(LPVOID lpParam) {
    DWORD dwCurrThrID = GetCurrentThreadId();
    int iLowerBound = 1;

    int iWidthLimit = abs(rt3.right - rt3.left);
    int iHeightLimit = abs(rt3.bottom - rt3.top);

    std::cout << "Потік " << dwCurrThrID << " типу 3 очікує сигналу...\n";
    WaitForSingleObject(hMutexType3, INFINITE);
    while (WaitForSingleObject(hEvent, INFINITE) != WAIT_ABANDONED ||
           WaitForSingleObject(hEvent, INFINITE) != WAIT_FAILED)
    {
        if (isExitThread && dwThreadToExit == dwCurrThrID) {
            std::cout << "Потік " << dwCurrThrID << " типу 1 вимикається.\n";
            ExitThread(0);
        }

        std::cout << "Потік " << dwCurrThrID << " типу 3 отримав сигнал і виконується.\n";
        Sleep(1000);
        iThread3Counter += 1;

        sprintf_s(szThreadCodeStr, (size_t)iSTR_SIZE - 1,
            "Thread: %d",
            dwCurrThrID);

        int iColourR = rand() % 256;
        int iColourG = rand() % 256;
        int iColourB = rand() % 256;

        hRandBrush = CreateSolidBrush(RGB(iColourR, iColourG, iColourB));

        int randomWidth = rand() % iWidthLimit + iLowerBound;
        int randomHeight = rand() % iHeightLimit + iLowerBound;

        int left = rand() % (iWidthLimit - 10) + rt3.left;                  // від даного лівого краю прямокутника до (+ ліміт ширини - 10)
        int right = rand() % (iWidthLimit - (left - rt3.left)) + left;      // від отриманого лівого до (+ залишковий ліміт ширини)
        int top = rand() % (iHeightLimit - 10) + rt3.top;                   // аналогічно
        int bottom = rand() % (iHeightLimit - (top - rt3.top)) + top;

        rt3_rand = { left, top, right, bottom };

        InvalidateRect(hWnd, &rt3, TRUE);

        std::cout << "Потік " << dwCurrThrID << " типу 3 завершує роботу.\n";
        ReleaseMutex(hMutexType3);
    }
    return 0;
}

// Потік підрахунку
DWORD WINAPI ThreadProc4(LPVOID lpParam) {
    DWORD dwCurrThrID = GetCurrentThreadId();

    std::cout << "Потік " << dwCurrThrID << " типу 4 очікує сигналу...\n";
    WaitForSingleObject(hMutexType4, INFINITE);
    while (WaitForSingleObject(hEvent, INFINITE) != WAIT_ABANDONED ||
           WaitForSingleObject(hEvent, INFINITE) != WAIT_FAILED)
    {
        if (isExitThread && dwThreadToExit == dwCurrThrID) {
            std::cout << "Потік " << dwCurrThrID << " типу 1 вимикається.\n";
            ExitThread(0);
        }

        std::cout << "Потік " << dwCurrThrID << " типу 4 отримав сигнал і виконується.\n";
        Sleep(1000);

        sprintf_s(szThreadCallsCountersStr, (size_t)iSTR_SIZE - 1,
            "Thread: %d; T1: %I64u; T2: %I64u; T3: %I64u",
            dwCurrThrID, iThread1Counter, iThread2Counter, iThread3Counter);
        InvalidateRect(hWnd, &rt4, TRUE);

        std::cout << "Потік " << dwCurrThrID << " типу 4 завершує роботу.\n";
        ReleaseMutex(hMutexType4);
    }
    return 0;
}

int CreateThreadSimple(LPTHREAD_START_ROUTINE ThreadFunc, LPVOID lpParam) {
    vdwThreadIDs.push_back(vhThreads.size());
    HANDLE hThread = CreateThread(NULL, 0, ThreadFunc, lpParam, NULL, &vdwThreadIDs[vhThreads.size()]);

    if (hThread) {
        vhThreads.push_back(hThread);
    }
    else {
        vdwThreadIDs.pop_back(); // failed to create
        vhThreads.pop_back();
        return 1;
    }
    return 0;
}


void WaitForThreadsAndSyncHandles() {
    WaitForMultipleObjects(vhThreads.size(), (HANDLE*)&vhThreads, TRUE, INFINITE);

    WaitForSingleObject(hMutexType1, INFINITE);
    WaitForSingleObject(hMutexType2, INFINITE);
    WaitForSingleObject(hMutexType3, INFINITE);
    WaitForSingleObject(hMutexType4, INFINITE);
    WaitForSingleObject(hEvent, INFINITE);    
}

void CloseHandles() {
    for (int i = vhThreads.size(); i > 0; i--) {
        CloseHandle(vhThreads[i]);
        vdwThreadIDs.pop_back();
        vhThreads.pop_back();
    }
    CloseHandle(hMutexType1);
    CloseHandle(hMutexType2);
    CloseHandle(hMutexType3);
    CloseHandle(hMutexType4);
    CloseHandle(hEvent);
}