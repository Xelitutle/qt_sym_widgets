#pragma once

#include "symwidgets/SymWidget.h"

namespace sym {

class SymButton : public SymWidget {
    Q_OBJECT
public:
    explicit SymButton(const QString& text, SymWidget* parent = nullptr);

    QString text() const { return m_text; }
    bool focusable() const override { return true; }
    QColor currentColor() const override;
    QColor currentBackgroundColor() const override;
    bool isPressed() const { return m_pressed; }

public slots:
    // Drives the pressed/held visual state. Intended to be fed both by local
    // input (keyboard/mouse, see below) and by external sources (e.g. a
    // physical button reporting its own down/up events). Emits pressed()
    // only on the actual false->true transition.
    void setPressed(bool pressed);

signals:
    void pressed();
    void focusGained();
    void focusLost();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    QString m_text;
    bool m_pressed = false;
};

} // namespace sym
