#include <Windows.h>
#include "GammaController.h"
//#include "Util.h"
#include <WindowsX.h>

#define MUTEX_NAME L"PokemonFlash"

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;
TCHAR szWindowClass[] = L"PokemonFlash";
TCHAR szWindowTitle[] = L"PokemonFlash";

GammaController gammaController;
double radianR = 1;
double radianG = 0.5;
double radianB = 0;

#define UILANG_JAPANESE 0x411
#define UILANG_ENGLISH 0x409
void SetUILocale()
{
  UINT localeId = GetUserDefaultLCID();
  if(UILANG_JAPANESE != localeId){
    localeId = UILANG_ENGLISH;
  }
  ::SetThreadUILanguage(localeId);
}

CMutex g_Mutex;

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

  // ロケールの設定
  ::SetUILocale();

	// 多重起動防止
  try{
    g_Mutex.createMutex(MUTEX_NAME);
	}catch(std::exception e){
		::ErrorMessageBox(L"Duplicate boot error");
		exit(0);
	}

  MyRegisterClass(hInstance);
  if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

  ::MessageBox(NULL, L"Quit?", L"Confirm", MB_OK);
  gammaController.setGamma(DEFAULT_GAMMA);
  return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;
	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

	hWnd = CreateWindow(szWindowClass, szWindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if(!hWnd){
		return FALSE;
	}

	ShowWindow(hWnd, SW_HIDE); // ウインドウは非表示
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  static UINT timer = NULL;

  switch(message){
  case WM_CREATE:
    timer = ::SetTimer(hWnd, 0, 10, NULL);
    return TRUE;

  case WM_DESTROY:
    ::KillTimer(hWnd, timer);
    return TRUE;

  case WM_TIMER:
    gammaController.setGamma(
      sin(radianR) + 1.5,
      sin(radianG) + 1.5,
      sin(radianB) + 1.5);
    
    radianR += 0.2;
    radianG += 0.2;
    radianB += 0.2;
    return TRUE;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}
