#include <Windows.h>
#include "mtapp.h"
#include <stdexcept>

int main()
{
    try {
        mt::MegaTextureApp app(L"MegaTexture", 1920, 1200);
        app.run();
    } catch(std::runtime_error & err) {
        MessageBoxA(0, err.what(), "Fatal Error", MB_ICONEXCLAMATION);
    }
    return 0;
}