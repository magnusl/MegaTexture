#ifndef APP_H
#define APP_H

#include <Windows.h>
#include <GL\glew.h>
#include <GL\wglew.h>

namespace mt {

///
/// \class  App
///
class App
{
public:
    App(const wchar_t * Title, size_t W, size_t H);
    virtual ~App();

    void run();

    // execute a single frame
    virtual void Frame() = 0;

protected:
    HWND    _hWnd;
    HGLRC   _hRC;
    HDC     _hDC;
    GLuint  _vao;
};

} // namespace mt

#endif
