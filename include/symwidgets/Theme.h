#pragma once

#include <QColor>

namespace sym {

/**
 * @brief Color configuration for a SymMainWidget tree.
 *
 * Passed once to the SymMainWidget constructor and immutable for the
 * lifetime of the application — none of the fields change at runtime.
 * Every other SymWidget reads it through SymWidget::theme(), which resolves
 * to the owning SymMainWidget's instance.
 */
struct Theme {
    /** Fill color of the SymMainWidget background. */
    QColor substrateColor;
    /** Alpha (0-255) applied to substrateColor when painting the background. */
    int substrateAlpha = 255;

    /** Color of SymFrame borders and titles, uniform across all frames. */
    QColor frameColor;
    /** Text color used by SymLabel. */
    QColor textColor;

    /** Color of an active (focusable) widget that has neither focus nor is being edited. */
    QColor widgetColorNormal;
    /** Color of an active widget that has focus but is not being edited (includes the `[ ]` focus brackets). */
    QColor widgetColorFocused;
    /** Color of an active widget while it is being edited (numeric inputs only). */
    QColor widgetColorEditing;
    /** Color of the digit currently under the cursor while editing SymIntInput/SymFloatInput. */
    QColor digitHighlightColor;
    /** Color of any widget in the Disabled state (`!isEnabled()`), regardless of focus/editing. */
    QColor widgetColorDisabled;
};

} // namespace sym
