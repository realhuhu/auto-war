#include "enum.h"

std::string PreviousToString(Previous previous) {
    switch (previous) {
        case Previous::LEFT:
            return "左侧";
        case Previous::TOP:
            return "上方";
        case Previous::RIGHT:
            return "右侧";
        case Previous::DOWN:
            return "下方";
        case Previous::LEFT_CENTER:
            return "正左侧";
        case Previous::TOP_CENTER:
            return "正上方";
        case Previous::RIGHT_CENTER:
            return "正右侧";
        case Previous::DOWN_CENTER:
            return "正下方";
        case Previous::INNER:
            return "内部";
        case Previous::NONE:
            return "任意";
        default:
            return "ERROR";
    }
}
