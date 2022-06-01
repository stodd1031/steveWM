extern "C"
{
    #include <X11/Xutil.h>
}
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <X11/extensions/Xrandr.h>
#include <algorithm>
#include <cmath>

class Frame
{
    public:
    Frame(Display *displayPass, Window rootPass, Window clientPass);
    Frame();
    
    Window getFrameHandle();
    Window getClientHandle();

    void moveFrame(int x, int y);
    void moveResizeFrame(int x, int y, int width, int height);

    void handleFrameButtonPress(XButtonEvent *ev);

    void handleWindowDrag(XMotionEvent *ev, int cursIntX, int cursIntY, int winIntX, int winIntY);
    void handleWindowResize(XMotionEvent *ev, int cursIntX, int cursIntY, int winIntX, int winIntY, int winIntWidth, int winIntHeight, int corner);
    void handleWindowDragOrResize(XMotionEvent *ev, int cursIntX, int cursIntY, int winIntX, int winIntY, int winIntWidth, int winIntHeight);

    private:

    Display *display;

    Window frame;
    Window client;
    Window root;

    Window closeButton;
    Window maximizeButton;
    Window minimizeButton;
    Window titlebar;

    void handleMaximizeButton(XButtonEvent *ev);
    void handleMinimizeButton();
    void handleCloseButton();

    void maximize(int x, int y, int width, int height);
    void unmaximize();

    int frameX;
    int frameY;
    int frameWidth;
    int frameHeight;

    bool maximized = false;


    int borderWidth = 3;
    
    //int frameWidth;
};