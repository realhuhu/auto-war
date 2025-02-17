# include "simple.h"

float getScale() {
    MONITORINFOEX info = {};
    POINT ptZero = {0, 0};
    info.cbSize = sizeof(info);
    GetMonitorInfo(MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY), &info);
    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    return static_cast<float>(devMode.dmPelsWidth) / static_cast<float>(info.rcMonitor.right - info.rcMonitor.left);
}

int main(int argc, char *argv[]) {
    state.scale = getScale();

    QApplication app(argc, argv);
    PanelWidget window;
    window.show();
    return QApplication::exec();
}