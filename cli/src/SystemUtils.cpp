#include "SystemUtils.h"
#include <iostream>
#include <ctime>
#include <csignal>
#include <iomanip>
#include <sstream>

#ifdef __linux__
    #include <sys/stat.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <unistd.h>
#elif _WIN32
    #include <windows.h>
    #include <conio.h>
    #include <direct.h>
#endif

volatile std::atomic<bool> g_exit_requested(false);

void signal_handler_internal(int) {
    g_exit_requested = true;
}

void SystemUtils::setupSignalHandler() {
    signal(SIGINT, signal_handler_internal);
    signal(SIGTERM, signal_handler_internal);
}

std::string SystemUtils::getCurrentTimestampString() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H_%M_%S", timeinfo);
    return std::string(buffer);
}

bool SystemUtils::createDirectory(const std::string& path) {
    #ifdef __linux__
        return mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
    #elif _WIN32
        return _mkdir(path.c_str()) == 0;
    #endif
}

void SystemUtils::sleepMs(int milliseconds) {
    #ifdef __linux__
        usleep(milliseconds * 1000);
    #elif _WIN32
        Sleep(milliseconds);
    #endif
}

// Implementazione specifica per Linux di kbhit
#ifdef __linux__
int _linux_kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;
    if (!initialized) {
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }
    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
#endif

bool SystemUtils::getKeyboardInput(char *c) {
    #ifdef __linux__
        if (_linux_kbhit()) {
            *c = getc(stdin);
            return true;
        }
    #elif _WIN32
        if (_kbhit()) {
            *c = _getch();
            return true;
        }
    #endif
    return false;
}