#include "symwidgets/SymIntInput.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <cmath>

namespace sym {

namespace {
int digitsOf(long long v)
{
    v = qAbs(v);
    int n = 1;
    while (v >= 10) {
        v /= 10;
        n++;
    }
    return n;
}
}

SymIntInput::SymIntInput(long long minValue, long long maxValue, SymWidget* parent)
    : SymWidget(parent)
    , m_minValue(minValue)
    , m_maxValue(maxValue)
{
    m_numDigits = qMax(digitsOf(m_minValue), digitsOf(m_maxValue));
    setFocusPolicy(Qt::ClickFocus);
    setCellSize(slotCount(), 1);
    setValue(0);
}

long long SymIntInput::value() const
{
    return m_negative ? -m_absValue : m_absValue;
}

void SymIntInput::setValue(long long value)
{
    if (value < m_minValue)
        value = m_minValue;
    if (value > m_maxValue)
        value = m_maxValue;
    m_negative = value < 0;
    m_absValue = qAbs(value);
    update();
}

QString SymIntInput::digitsString() const
{
    return QString("%1").arg(m_absValue, m_numDigits, 10, QChar('0'));
}

QString SymIntInput::displayDigitsString() const
{
    const QString digits = digitsString();

    int firstNonZero = 0;
    while (firstNonZero < digits.length() - 1 && digits.at(firstNonZero) == QChar('0'))
        firstNonZero++;

    // While editing, leading zeros are revealed only down to the digit
    // currently under the cursor, mirroring SymFloatInput's behavior.
    int firstVisible = firstNonZero;
    if (isEditing() && !isSignSlot(m_cursor)) {
        const int idx = digitIndexForSlot(m_cursor);
        if (idx >= 0 && idx < digits.length())
            firstVisible = qMin(firstVisible, idx);
    }

    return QString(firstVisible, QChar(' ')) + digits.mid(firstVisible);
}

void SymIntInput::beginEdit()
{
    if (isEditing())
        return;
    setEditing(true);
    m_cursor = slotCount() - 1; // least significant digit
    emit startEdit();
    update();
}

void SymIntInput::commitEdit()
{
    if (!isEditing())
        return;
    setEditing(false);
    setValue(value());
    emit changed(value());
    emit stopEdit();
    update();
}

void SymIntInput::bumpDigit(int slot, int delta)
{
    if (isSignSlot(slot)) {
        m_negative = !m_negative;
        update();
        return;
    }
    QString digits = digitsString();
    const int idx = digitIndexForSlot(slot);
    if (idx < 0 || idx >= digits.length())
        return;
    int d = digits.at(idx).digitValue();
    d = (d + delta + 10) % 10;
    digits[idx] = QChar('0' + d);
    m_absValue = digits.toLongLong();
    update();
}

void SymIntInput::stepWholeValue(int delta)
{
    long long v = value() + delta;
    m_negative = v < 0;
    m_absValue = qAbs(v);
    update();
}

void SymIntInput::paintEvent(QPaintEvent*)
{
    static const QColor kFocusBg(255, 255, 255);
    static const QColor kDigitBg(1, 1, 1);

    QPainter painter(this);
    painter.setFont(font());

    QColor bg = Qt::transparent;
    QColor fg;
    switch (visualState()) {
    case VisualState::Disabled:
        fg = theme().widgetColorDisabled;
        break;
    case VisualState::Focused:
    case VisualState::Editing:
        bg = kFocusBg;
        fg = theme().widgetColorFocused;
        break;
    default:
        fg = theme().widgetColorNormal;
        break;
    }

    if (bg.alpha() != 0)
        painter.fillRect(rect(), bg);

    QString text;
    if (isSigned()) {
        if (!isEditing() && m_absValue == 0)
            text += QChar(0x00B1);
        else
            text += m_negative ? QChar('-') : QChar('+');
    }
    text += displayDigitsString();

    const int cellW = width() / slotCount();
    for (int slot = 0; slot < slotCount(); ++slot) {
        const QRect cellRect(slot * cellW, 0, cellW, height());
        const bool highlighted = isEditing() && slot == m_cursor;
        if (highlighted)
            painter.fillRect(cellRect, kDigitBg);
        painter.setPen(highlighted ? theme().digitHighlightColor : fg);
        painter.drawText(cellRect, Qt::AlignCenter, QString(text.at(slot)));
    }
}

void SymIntInput::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        if (isEditing()) {
            bumpDigit(m_cursor, 1);
            event->accept();
            return;
        }
        break;
    case Qt::Key_Down:
        if (isEditing()) {
            bumpDigit(m_cursor, -1);
            event->accept();
            return;
        }
        break;
    case Qt::Key_Plus:
        beginEdit();
        stepWholeValue(1);
        event->accept();
        return;
    case Qt::Key_Minus:
        beginEdit();
        stepWholeValue(-1);
        event->accept();
        return;
    case Qt::Key_Left:
        if (isEditing()) {
            m_cursor = (m_cursor - 1 + slotCount()) % slotCount();
            update();
            event->accept();
            return;
        }
        break;
    case Qt::Key_Right:
        if (isEditing()) {
            m_cursor = (m_cursor + 1) % slotCount();
            update();
            event->accept();
            return;
        }
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (isEditing())
            commitEdit();
        else
            beginEdit();
        event->accept();
        return;
    default:
        break;
    }
    QWidget::keyPressEvent(event);
}

void SymIntInput::mousePressEvent(QMouseEvent* event)
{
    if (consumeActivationClick() && !isEditing())
        beginEdit();
    QWidget::mousePressEvent(event);
}

void SymIntInput::focusOutEvent(QFocusEvent* event)
{
    if (isEditing())
        commitEdit();
    SymWidget::focusOutEvent(event);
}

} // namespace sym
