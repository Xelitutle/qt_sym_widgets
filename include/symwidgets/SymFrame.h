#pragma once

#include "symwidgets/SymWidget.h"

namespace sym {

enum class InputButton { Up, Down, Left, Right };

class SymFrame : public SymWidget {
    Q_OBJECT
public:
    SymFrame(const QString& title, SymWidget* parent);
    SymFrame(const QString& title, int rows, int cols, SymWidget* parent);

    QString title() const { return m_title; }
    bool focusable() const override { return false; }

    SymWidget* findNearestFocusable(SymWidget* from, InputButton direction) const;

    // Used when `from` is about to lose focus because it was disabled or
    // hidden. Prefers a SymButton whose text matches one of
    // SymMainWidget::focusFallbackButtonTexts() (in priority order, nearest
    // match wins ties); falls back to the overall nearest focusable widget.
    SymWidget* findFallbackFocusable(SymWidget* from) const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QString m_title;
};

} // namespace sym
