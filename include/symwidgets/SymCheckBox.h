#pragma once

#include "symwidgets/SymWidget.h"

namespace sym {

class SymCheckBox : public SymWidget {
    Q_OBJECT
public:
    explicit SymCheckBox(SymWidget* parent = nullptr);

    void setChecked(bool checked);
    bool checked() const { return m_checked; }
    bool focusable() const override { return true; }
    QColor currentColor() const override;
    QColor currentBackgroundColor() const override;
    bool isPressed() const { return m_pressed; }

public slots:
    // Drives the pressed/held visual state; see SymButton::setPressed().
    void setPressed(bool pressed);

signals:
    void changed(bool checked);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    void toggle();

    bool m_checked = false;
    bool m_pressed = false;
};

} // namespace sym
