#include "enum.h"

std::string PreviousToString(Previous previous) {
    switch (previous) {
        case Previous::LEFT:
            return u8"左侧";
        case Previous::TOP:
            return u8"上方";
        case Previous::RIGHT:
            return u8"右侧";
        case Previous::DOWN:
            return u8"下方";
        case Previous::LEFT_CENTER:
            return u8"正左侧";
        case Previous::TOP_CENTER:
            return u8"正上方";
        case Previous::RIGHT_CENTER:
            return u8"正右侧";
        case Previous::DOWN_CENTER:
            return u8"正下方";
        case Previous::INNER:
            return u8"内部";
        case Previous::NONE:
            return u8"任意";
        default:
            return "ERROR";
    }
}
