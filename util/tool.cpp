#include "tool.h"

QString WIdToQSting(WId wid) { return QString::number(reinterpret_cast<qulonglong>(reinterpret_cast<HWND>(wid)), 16); }