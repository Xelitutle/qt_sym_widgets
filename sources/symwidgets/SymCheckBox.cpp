#include "symwidgets/SymCheckBox.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

namespace sym {

namespace {
const QColor kPressedBg(1, 1, 1);
const QColor kPressedFg(255, 255, 255);
}

SymCheckBox::SymCheckBox(SymWidget* parent)
    : SymWidget(parent)
{
    setFocusPolicy(Qt::ClickFocus);
    setCellSize(1, 1);
}

void SymCheckBox::setChecked(bool checked)
{
    if (m_checked == checked)
        return;
    m_checked = checked;
    update();
    emit changed(m_checked);
}

void SymCheckBox::toggle()
{
    setChecked(!m_checked);
}

void SymCheckBox::setPressed(bool pressed)
{
    if (m_pressed == pressed)
        return;
    m_pressed = pressed;
    update();
    if (parentWidget())
        parentWidget()->update();
}

QColor SymCheckBox::currentColor() const
{
    if (m_pressed)
        return kPressedFg;
    return SymWidget::currentColor();
}

QColor SymCheckBox::currentBackgroundColor() const
{
    if (m_pressed)
        return kPressedBg;
    return SymWidget::currentBackgroundColor();
}

void SymCheckBox::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setFont(font());

    const QColor bg = currentBackgroundColor();
    if (bg.alpha() != 0)
        painter.fillRect(rect(), bg);

    painter.setPen(currentColor());
    painter.drawText(rect(), Qt::AlignCenter, m_checked ? QStringLiteral("√") : QStringLiteral("─"));
}

void SymCheckBox::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (!m_pressed) {
            setPressed(true);
            toggle();
        }
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void SymCheckBox::keyReleaseEvent(QKeyEvent* event)
{
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && !event->isAutoRepeat()) {
        setPressed(false);
        event->accept();
        return;
    }
    QWidget::keyReleaseEvent(event);
}

void SymCheckBox::mousePressEvent(QMouseEvent* event)
{
    if (consumeActivationClick() && !m_pressed) {
        setPressed(true);
        toggle();
    }
    QWidget::mousePressEvent(event);
}

void SymCheckBox::mouseReleaseEvent(QMouseEvent* event)
{
    setPressed(false);
    QWidget::mouseReleaseEvent(event);
}

void SymCheckBox::focusOutEvent(QFocusEvent* event)
{
    setPressed(false);
    SymWidget::focusOutEvent(event);
}

} // namespace sym
