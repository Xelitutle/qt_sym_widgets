#pragma once

namespace sym {

struct GridMetrics {
    int rows = 1;
    int cols = 1;
    int screenWidthPx = 0;
    int screenHeightPx = 0;

    int cellWidthPx() const { return screenWidthPx / cols; }
    int cellHeightPx() const { return screenHeightPx / rows; }
};

} // namespace sym
