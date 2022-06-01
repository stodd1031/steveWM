extern "C"
{
    #include <X11/Xutil.h>
}
#include <unordered_map>
#include <vector>
#include <list>
#include <iostream>
#include <X11/XKBlib.h>
#include <X11/cursorfont.h>
#include <unistd.h>
#include <algorithm>
#include "macros.h"
#include "frame.h"
#include <X11/extensions/Xrandr.h>

class WindowManager
{
    public:
    void loop();
    WindowManager();

    private:

    Display* display;
    Window root;
    Window active;

    void handleKeyPress(XKeyEvent *ev);
    void handleKeyRelease(XKeyEvent *ev);
    void handleButtonPress(XButtonEvent *ev);
    void handleMotionNotify(XMotionEvent *ev);

    void handleConfigureRequest(XConfigureRequestEvent *ev);
    void handleMapRequest(XMapRequestEvent *ev);
    void frame(Window client);
    void handleUnmapRequest(XUnmapEvent *ev);
    void unframe(Window client);
    void handleUnmapNotify(XUnmapEvent *ev);

    std::unordered_map<Window, Frame> frameHandlesTOFrame;
    std::vector<Window> applications;
    int switchFrame = 0;

    int cursIntX;
    int cursIntY;
    int winIntX;
    int winIntY;
    unsigned int winIntWidth;
    unsigned int winIntHeight;
    int corner;
    void moveResizeWindow(XMotionEvent *ev, int x, int y);

    int monitorCnt;
    XRRMonitorInfo* xMonitors;

};

