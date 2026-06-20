#pragma once

#include <QColor>

namespace sym {

struct Theme {
    QColor substrateColor;
    int substrateAlpha = 255;

    QColor frameColor;
    QColor textColor;

    QColor widgetColorNormal;
    QColor widgetColorFocused;
    QColor widgetColorEditing;
    QColor digitHighlightColor;
    QColor widgetColorDisabled;
};

} // namespace sym
