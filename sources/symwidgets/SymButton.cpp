#include "symwidgets/SymButton.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

namespace sym {

namespace {
const QColor kPressedBg(1, 1, 1);
const QColor kPressedFg(255, 255, 255);
}

SymButton::SymButton(const QString& text, SymWidget* parent)
    : SymWidget(parent)
    , m_text(text)
{
    setFocusPolicy(Qt::ClickFocus);
    setCellSize(qMax(1, m_text.length()), 1);
}

void SymButton::setPressed(bool pressed)
{
    if (m_pressed == pressed)
        return;
    m_pressed = pressed;
    update();
    if (parentWidget())
        parentWidget()->update();
    if (m_pressed)
        emit this->pressed();
}

QColor SymButton::currentColor() const
{
    if (m_pressed)
        return kPressedFg;
    return SymWidget::currentColor();
}

QColor SymButton::currentBackgroundColor() const
{
    if (m_pressed)
        return kPressedBg;
    return SymWidget::currentBackgroundColor();
}

void SymButton::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setFont(font());

    const QColor bg = currentBackgroundColor();
    if (bg.alpha() != 0)
        painter.fillRect(rect(), bg);

    painter.setPen(currentColor());
    painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, m_text);
}

void SymButton::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        setPressed(true);
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void SymButton::keyReleaseEvent(QKeyEvent* event)
{
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && !event->isAutoRepeat()) {
        setPressed(false);
        event->accept();
        return;
    }
    QWidget::keyReleaseEvent(event);
}

void SymButton::mousePressEvent(QMouseEvent* event)
{
    if (consumeActivationClick())
        setPressed(true);
    QWidget::mousePressEvent(event);
}

void SymButton::mouseReleaseEvent(QMouseEvent* event)
{
    setPressed(false);
    QWidget::mouseReleaseEvent(event);
}

void SymButton::focusInEvent(QFocusEvent* event)
{
    SymWidget::focusInEvent(event);
    emit focusGained();
}

void SymButton::focusOutEvent(QFocusEvent* event)
{
    setPressed(false);
    SymWidget::focusOutEvent(event);
    emit focusLost();
}

} // namespace sym
