#include "windowManager.h"
#include <X11/Xutil.h>
#include <bitset>

int main()
{

    //std::cout << (1 << 31) << std::endl;

    XInitThreads();
    WindowManager window;

    window.loop();
    return 0;
}