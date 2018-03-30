#include "app.h"
#include <stdexcept>

bool CreateContext(HDC hDC, HGLRC & hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int nFormat = ChoosePixelFormat(hDC, &pfd);
    if (nFormat == 0) {
        return false;
    }

    // create a temporary context so that we can call various glew/opengl functions before 
    // creating the actual context.
    SetPixelFormat(hDC, nFormat, &pfd);
    HGLRC hRCTmp = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRCTmp);

    // init glew
    glewInit();

    const char * str = wglGetExtensionsStringARB(hDC);

    // we want a OpenGL 3.2 context
    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 2,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    };

    // glewIsSupported("WGL_ARB_create_context") seems to not work correctly.
    if (!strcmp(wglGetExtensionsStringARB(hDC), "WGL_ARB_create_context")) {
        return false;
    }

    // create a new context and destroy the old one.
    hRC = wglCreateContextAttribsARB(hDC,0, attribs);
    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(hRCTmp);
    wglMakeCurrent(hDC, hRC);

    return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_DESTROY:
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND CreatePrimaryWindow(int w, int h, const wchar_t * title)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.hInstance = GetModuleHandle(0);
    wc.lpszClassName = L"glwnd";
    wc.lpfnWndProc = WndProc;
    wc.style = CS_OWNDC;

    RegisterClass(&wc);

    HWND hWnd = CreateWindow(L"glwnd", title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
        CW_USEDEFAULT, w, h, 0,0, wc.hInstance, 0);

    return hWnd;
}

namespace mt {

App::App(const wchar_t * Title, size_t W, size_t H)
{
    _hWnd = CreatePrimaryWindow(W, H, Title);
    if (!_hWnd) {
        throw std::runtime_error("Failed to create Window.");
    }
    
    _hDC = GetDC(_hWnd);

    if (!CreateContext(_hDC, _hRC)) {
        DestroyWindow(_hWnd);
        throw std::runtime_error("Failed to create OpenGL context.");
    }

    ShowWindow(_hWnd, SW_SHOW);

     glGenVertexArrays(1, &_vao);
     glBindVertexArray(_vao);
}

void App::run()
{
    MSG msg;
    bool running = true;
    while(running) {
        while (PeekMessage(&msg, 0,0,0,PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Frame();
        SwapBuffers(_hDC);
    }
}

App::~App()
{
    if (_hWnd) {
        DestroyWindow(_hWnd);
    }
}

} // namespace mt
