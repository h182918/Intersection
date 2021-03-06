// Intersection.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Intersection.h"
#include <iostream>
#include <list>

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//Our globale Variables:
RECT rect;
int eastState = 1;
int southState = 3;
HWND hWestSpawnRate;
HWND hNorthSpawnRate;
int pw = 10;
int pn = 10;

class Car {
public:
    int left;
    int right;
    int top;
    int bottom;
    int direction;
    HBRUSH color;

    void draw(HDC* hdc) {
        Rectangle(*hdc, left, top, right, bottom);
    }
    ~Car();


};

Car::~Car() {

}

void createRoad(HDC);
void createCar(int);
void paintCar(Car*);
void drawTrafficLight(HDC, int, int, int);
void updateState();
void moveCars();
void deleteCar(Car*);
void clearCarList();

list<Car*> carList;

//Our colors
COLORREF red = RGB(255, 0, 0);
COLORREF yellow = RGB(255, 255, 0);
COLORREF green = RGB(0, 255, 0);
COLORREF grey = RGB(192, 192, 192);
COLORREF black = RGB(0, 0, 0);

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	SpawnRateDialog(HWND, UINT, WPARAM, LPARAM);



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
    LoadStringW(hInstance, IDC_INTERSECTION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_INTERSECTION));

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

    return (int)msg.wParam;
}

INT_PTR CALLBACK SpawnRateDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        static HWND hEdit;
    case WM_INITDIALOG:
        hWestSpawnRate = CreateWindow(
            L"EDIT",
            L"",
            WS_BORDER | WS_CHILD | WS_VISIBLE,
            170, 50, 50, 20,
            hDlg, NULL, NULL, NULL);

        hNorthSpawnRate = CreateWindow(
            L"EDIT",
            L"",
            WS_BORDER | WS_CHILD | WS_VISIBLE,
            170, 30, 50, 20,
            hDlg, NULL, NULL, NULL);
        SetWindowTextA(hWestSpawnRate, "pw");
        SetWindowTextA(hNorthSpawnRate, "pn");
        hEdit = GetDlgItem(hDlg, IDC_EDIT1);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            TCHAR northRate[3];
            TCHAR westRate[3];
            GetWindowText(hNorthSpawnRate, northRate, 3);
            GetWindowText(hWestSpawnRate, westRate, 3);

            pn = _tstoi(northRate);
            pw = _tstoi(westRate);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
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

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INTERSECTION));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_INTERSECTION);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    SetTimer(hWnd, 1, 25, 0);
    SetTimer(hWnd, 2, 1000, 0);
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
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rect);
        createRoad(hdc);
        drawTrafficLight(hdc, 60, 70, eastState);
        drawTrafficLight(hdc, -100, -190, southState);

        list<Car*>::iterator i;
        for (i = carList.begin(); i != carList.end(); i++) {
            HGDIOBJ hOrg = SelectObject(hdc, (*i)->color);
            if ((*i)->direction == 1) {
                (*i)->top = rect.bottom / 2 - 40;
                (*i)->bottom = rect.bottom / 2 - 15;
            }
            else {
                (*i)->left = rect.right / 2 - 40;
                (*i)->right = rect.right / 2 - 15;
            }
            (*i)->draw(&hdc);
            SelectObject(hdc, hOrg);
        }

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_TIMER:
    {
        switch (wParam) {
        case 1:
            moveCars();
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        case 2:
            updateState();
            if (rand() % 100 < pn) {
                createCar(1);
            }
            if (rand() % 100 < pw) {
                createCar(2);
            }
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
    }
    break;
    case WM_LBUTTONDOWN:
    {
        DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, SpawnRateDialog);
        InvalidateRect(hWnd, 0, true);
    }
    break;
    case WM_RBUTTONDOWN:
        
    case WM_DESTROY:
        KillTimer(hWnd, 1);
        KillTimer(hWnd, 2);
        clearCarList();
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



void drawTrafficLight(HDC hdc, int x, int y, int state) {
    int padding = 5;
    int width = 30 + padding * 2;
    int heightRec = width * 3 + padding * 2;

    //rectangle
    int left = rect.right / 2 + x;
    int top = rect.bottom / 2 - y;
    int right = rect.right / 2 + x + width;
    int bottom = rect.bottom / 2 - y - heightRec;

    //top light
    int leftT = left + padding;
    int topT = top - heightRec + padding;
    int rightT = left + width - padding;
    int bottomT = top - (width * 2) - padding * 3;

    //mid light
    int leftM = left + padding;
    int topM = top - heightRec + width + padding * 2;
    int rightM = rect.right / 2 + x + width - padding;
    int bottomM = rect.bottom / 2 - y - width - padding * 2;

    //bottom light
    int leftB = left + padding;
    int topB = top - heightRec + width * 2 + padding * 3;
    int rightB = rect.right / 2 + x + width - padding;
    int bottomB = rect.bottom / 2 - y - padding;


    //create rectangle 
    HBRUSH hBrush = CreateSolidBrush(black);
    HGDIOBJ hOrg = SelectObject(hdc, hBrush);

    Rectangle(hdc, left, top, right, bottom);

    SelectObject(hdc, hOrg);
    DeleteObject(hBrush);

    if (state == 1) {

        //Creating a red brush
        HBRUSH hBrush = CreateSolidBrush(red);
        HGDIOBJ hOrg = SelectObject(hdc, hBrush);

        //Ellipse on top
        Ellipse(hdc, leftT, topT, rightT, bottomT);

        //cleanup
        SelectObject(hdc, hOrg);
        DeleteObject(hBrush);


        //Creating grey brush
        HBRUSH hBrushG = CreateSolidBrush(grey);
        HGDIOBJ hOrgG = SelectObject(hdc, hBrushG);

        //Ellipse in the middle
        Ellipse(hdc, leftM, topM, rightM, bottomM);


        //Ellipse on bottom
        Ellipse(hdc, leftB, topB, rightB, bottomB);

        SelectObject(hdc, hOrgG);
        DeleteObject(hBrushG);

    }

    if (state == 2)

    {

        //Red brush
        HBRUSH hBrush = CreateSolidBrush(red);
        HGDIOBJ hOrg = SelectObject(hdc, hBrush);


        //Red ellipse on top
        Ellipse(hdc, leftT, topT, rightT, bottomT);

        //Cleanup
        SelectObject(hdc, hOrg);
        DeleteObject(hBrush);

        //creating yellow brush
        HBRUSH hBrushG = CreateSolidBrush(yellow);
        HGDIOBJ hOrgG = SelectObject(hdc, hBrushG);

        //Yellow ellipse in the middle
        Ellipse(hdc, leftM, topM, rightM, bottomM);

        SelectObject(hdc, hOrgG);
        DeleteObject(hBrushG);

        HBRUSH hBrushB = CreateSolidBrush(grey);
        HGDIOBJ hOrgB = SelectObject(hdc, hBrushB);

        //grey ellipse on bottom
        Ellipse(hdc, leftB, topB, rightB, bottomB);

        SelectObject(hdc, hOrgB);
        DeleteObject(hBrushB);

    }

    if (state == 3)

    {

        //Grey brush
        HBRUSH hBrush = CreateSolidBrush(grey);
        HGDIOBJ hOrg = SelectObject(hdc, hBrush);


        //grey ellipse on top
        Ellipse(hdc, leftT, topT, rightT, bottomT);

        //grey ellipse in the middle
        Ellipse(hdc, leftM, topM, rightM, bottomM);


        //cleanup grey
        SelectObject(hdc, hOrg);
        DeleteObject(hBrush);


        //creating green brush
        HBRUSH hBrushG = CreateSolidBrush(green);
        HGDIOBJ hOrgG = SelectObject(hdc, hBrushG);

        //green ellipse on bottom
        Ellipse(hdc, leftB, topB, rightB, bottomB);

        SelectObject(hdc, hOrgG);
        DeleteObject(hBrushG);

    }

    if (state == 4)

    {
        //creating yellow brush
        HBRUSH hBrushG = CreateSolidBrush(yellow);
        HGDIOBJ hOrgG = SelectObject(hdc, hBrushG);

        //Yellow ellipse in the middle
        Ellipse(hdc, leftM, topM, rightM, bottomM);

        SelectObject(hdc, hOrgG);
        DeleteObject(hBrushG);


        //creating grey brush
        HBRUSH hBrushB = CreateSolidBrush(grey);
        HGDIOBJ hOrgB = SelectObject(hdc, hBrushB);

        //grey ellipse on top 
        Ellipse(hdc, leftT, topT, rightT, bottomT);

        //grey ellipse on bottom
        Ellipse(hdc, leftB, topB, rightB, bottomB);

        SelectObject(hdc, hOrgB);
        DeleteObject(hBrushB);

    }

}

void createRoad(HDC hdc) {
    HBRUSH greyBrush = CreateSolidBrush(RGB(192, 192, 192));
    HBRUSH yellowBrush = CreateSolidBrush(RGB(255, 255, 0));
    HBRUSH darkGreyBrush = CreateSolidBrush(RGB(70, 70, 70));
    HPEN greyPen = CreatePen(0, 0, RGB(192, 192, 192));
    HPEN yellowPen = CreatePen(0, 0, RGB(255, 255, 0));
    HPEN darkGreyPen = CreatePen(0, 0, RGB(70, 70, 0));

    HGDIOBJ hOrg = SelectObject(hdc, greyBrush);
    HGDIOBJ hPen = SelectObject(hdc, greyPen);

    Rectangle(hdc, 0, rect.bottom / 2 - 50, rect.right, rect.bottom / 2 + 50);
    Rectangle(hdc, rect.right / 2 - 50, 0, rect.right / 2 + 50, rect.bottom);

    SelectObject(hdc, yellowBrush);
    SelectObject(hdc, yellowPen);

    Rectangle(hdc, 0, rect.bottom / 2 - 1, rect.right, rect.bottom / 2 + 1);
    Rectangle(hdc, rect.right / 2 - 1, 0, rect.right / 2 + 1, rect.bottom);

    SelectObject(hdc, darkGreyBrush);
    SelectObject(hdc, darkGreyPen);

    Rectangle(hdc, rect.right / 2 - 50, rect.bottom / 2 - 50, rect.right / 2 + 50, rect.bottom / 2 + 50);

    SelectObject(hdc, hOrg);
    SelectObject(hdc, hPen);

    DeleteObject(greyBrush);
    DeleteObject(greyPen);
    DeleteObject(darkGreyBrush);
    DeleteObject(darkGreyPen);
    DeleteObject(yellowPen);
    DeleteObject(yellowBrush);

}

void updateState() {

    if (eastState == 2 || eastState == 3) {
        eastState++;
        southState = 1;
    }

    if (southState == 2 || southState == 3) {
        southState++;
        eastState = 1;
    }

    if (eastState == 4) {
        eastState = 1;
        southState++;
    }

    if (southState == 4) {
        southState = 1;
        eastState++;
    }

}

void createCar(int i) {
    Car* car = new Car;

    if (i == 1) {
        car->left = -30;
        car->right = car -> left + 25;
        car->top = rect.bottom / 2 - 40;
        car->bottom = rect.bottom / 2 - 15;
        car->direction = 1;
    }
    else if(i == 2) {
        car->left = rect.right / 2 - 40;
        car->right = rect.right / 2 - 15;
        car->top = -30;
        car->bottom = car->top + 25;
        car->direction = 2;
    }

    paintCar(car);
    carList.push_back(car);
}

void paintCar(Car* car) {
    HBRUSH yellowBrush = CreateSolidBrush(RGB(255, 255, 0));
    HBRUSH blueBrush = CreateSolidBrush(RGB(30, 144, 255));
    HBRUSH greenBrush = CreateSolidBrush(RGB(124, 252, 0));
    HBRUSH redBrush = CreateSolidBrush(RGB(220, 20, 60));
    HBRUSH orangeBrush = CreateSolidBrush(RGB(255, 165, 0));
    HBRUSH brushes[5] = { yellowBrush, blueBrush, greenBrush, redBrush, orangeBrush };
    int r = rand() % 5;
    car->color = brushes[r];

    for (int i = 0; i < 5; i++) {
        if (i != r) {
            DeleteObject(brushes[i]);
        }
    }   
}


void moveCars() {

    list<Car*>::iterator i;
    
    Car* startCar1 = new Car;
    Car* lastEast = startCar1;
    lastEast -> left = 2000;

    Car* startCar2 = new Car;
    Car* lastSouth= startCar2;
    lastSouth -> top = 2000;

    for (i = carList.begin(); i != carList.end(); i++) {

        if (((*i)->direction == 1)) {
            if ((southState == 3) || ((*i)->right < rect.right / 2 - 55) || ((*i)->right > rect.right / 2 - 50)) {
                if ((lastEast -> left) - ((*i) -> right) > 5) {
                        (*i)->left += 2;
                        (*i)->right += 2;
                }
            }
            if ((*i)->left > rect.right) {
                lastEast = startCar1;
                lastEast->left = 2000;
                deleteCar(*i);
                carList.erase(i);
                break;
            }
            else {
                lastEast = *i;
            }
            
        }
        else if (((*i)->direction == 2)) {
            if ((eastState == 3) || ((*i)->bottom < rect.bottom / 2 - 55) || ((*i)->bottom > rect.bottom / 2 - 50)) {
                if ((lastSouth->top) - ((*i)->bottom) > 5) {
                    (*i)->top += 2;
                    (*i)->bottom += 2;
                } 

            }
            if ((*i)->top > rect.bottom) {
                lastSouth = startCar2;
                lastSouth->top = 2000;
                deleteCar(*i);
                carList.erase(i);
                break;
            }
            else {
                lastSouth = *i;
            }
            
        }     

    }
    delete(startCar1);
    delete(startCar2);
}

void deleteCar(Car* car) {
    DeleteObject(car->color);
    delete(car);
}

void clearCarList() {
    list<Car*>::iterator i;
    for (i = carList.begin(); i != carList.end(); i++) {
        deleteCar(*i);
    }
}