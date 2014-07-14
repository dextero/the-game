#include "window/window.h"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    sb::Window window(800, 600);

    while (window.isOpened()) {
        sb::Event event;

        while (window.getEvent(event)) {
            switch (event.type) {
            case sb::Event::KeyPressed:
                window.close();
                break;
            default:
                break;
            }
        }
    }

    return 0;
}

