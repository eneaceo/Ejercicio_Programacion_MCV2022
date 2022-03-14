#pragma once

class CApplication {

    HWND hWnd;
    int  width = 1280;
    int  height = 800;
    static CApplication* the_app;

public:

    bool init(HINSTANCE hInstance);
    void run();
    HWND getHandle() {
        return hWnd;
    }

    void getDimensions(int& awidth, int& aheight) const;
    void setDimensions(int awidth, int aheight);

    static CApplication& get();
};
