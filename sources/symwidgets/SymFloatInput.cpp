#include "symwidgets/SymFloatInput.h"

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

SymFloatInput::SymFloatInput(double minValue, double maxValue, int precision, SymWidget* parent)
    : SymWidget(parent)
    , m_minValue(minValue)
    , m_maxValue(maxValue)
    , m_precision(precision)
{
    m_intDigits = qMax(digitsOf(static_cast<long long>(std::trunc(qAbs(m_minValue)))),
                        digitsOf(static_cast<long long>(std::trunc(qAbs(m_maxValue)))));
    setFocusPolicy(Qt::ClickFocus);
    setCellSize(totalCells(), 1);
    setValue(0.0);
}

long long SymFloatInput::scale() const
{
    long long s = 1;
    for (int i = 0; i < m_precision; ++i)
        s *= 10;
    return s;
}

double SymFloatInput::value() const
{
    const double v = static_cast<double>(m_absScaled) / static_cast<double>(scale());
    return m_negative ? -v : v;
}

void SymFloatInput::setValue(double value)
{
    if (value < m_minValue)
        value = m_minValue;
    if (value > m_maxValue)
        value = m_maxValue;
    m_negative = value < 0;
    m_absScaled = llround(qAbs(value) * scale());
    update();
}

QString SymFloatInput::combinedDigitsString() const
{
    return QString("%1").arg(m_absScaled, m_intDigits + m_precision, 10, QChar('0'));
}

SymFloatInput::CellKind SymFloatInput::cellKind(int cell) const
{
    int c = cell;
    if (isSigned()) {
        if (c == 0)
            return CellKind::Sign;
        c--;
    }
    if (c < m_intDigits)
        return CellKind::IntDigit;
    c -= m_intDigits;
    if (c == 0)
        return CellKind::Dot;
    return CellKind::FracDigit;
}

int SymFloatInput::combinedDigitIndex(int cell) const
{
    int c = cell;
    if (isSigned())
        c--;
    if (c < m_intDigits)
        return c;
    return c - 1; // skip dot
}

void SymFloatInput::beginEdit()
{
    if (isEditing())
        return;
    setEditing(true);
    m_cursor = (isSigned() ? 1 : 0) + m_intDigits - 1; // least significant integer digit
    emit startEdit();
    update();
}

void SymFloatInput::commitEdit()
{
    if (!isEditing())
        return;
    setEditing(false);
    setValue(value());
    emit changed(value());
    emit stopEdit();
    update();
}

void SymFloatInput::bumpDigit(int cell, int delta)
{
    const CellKind kind = cellKind(cell);
    if (kind == CellKind::Sign) {
        m_negative = !m_negative;
        update();
        return;
    }
    if (kind == CellKind::Dot)
        return;

    QString digits = combinedDigitsString();
    const int idx = combinedDigitIndex(cell);
    if (idx < 0 || idx >= digits.length())
        return;
    int d = digits.at(idx).digitValue();
    d = (d + delta + 10) % 10;
    digits[idx] = QChar('0' + d);
    m_absScaled = digits.toLongLong();
    update();
}

void SymFloatInput::stepWholeValue(long long deltaScaledUnits)
{
    long long signedScaled = (m_negative ? -1 : 1) * m_absScaled + deltaScaledUnits;
    m_negative = signedScaled < 0;
    m_absScaled = qAbs(signedScaled);
    update();
}

QString SymFloatInput::displayText() const
{
    QString digits = combinedDigitsString();
    QString intPart = digits.left(m_intDigits);
    QString fracPart = digits.right(m_precision);

    int firstNonZero = 0;
    while (firstNonZero < intPart.length() - 1 && intPart.at(firstNonZero) == QChar('0'))
        firstNonZero++;

    // While editing, leading zeros are revealed only down to the digit
    // currently under the cursor — not the whole integer part — so digits
    // the user hasn't reached yet stay blank.
    int firstVisible = firstNonZero;
    if (isEditing() && cellKind(m_cursor) == CellKind::IntDigit)
        firstVisible = qMin(firstVisible, combinedDigitIndex(m_cursor));

    intPart = QString(firstVisible, QChar(' ')) + intPart.mid(firstVisible);

    QString text;
    if (isSigned()) {
        const bool belowStep = (m_absScaled == 0);
        text += belowStep ? QChar(0x00B1) : (m_negative ? QChar('-') : QChar('+'));
    }
    text += intPart;
    text += QChar('.');
    text += fracPart;
    return text;
}

void SymFloatInput::paintEvent(QPaintEvent*)
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

    const QString text = displayText();
    const int cellW = width() / totalCells();
    for (int cell = 0; cell < totalCells() && cell < text.length(); ++cell) {
        const QRect cellRect(cell * cellW, 0, cellW, height());
        const bool highlighted = isEditing() && cell == m_cursor && cellKind(cell) != CellKind::Dot;
        if (highlighted)
            painter.fillRect(cellRect, kDigitBg);
        painter.setPen(highlighted ? theme().digitHighlightColor : fg);
        painter.drawText(cellRect, Qt::AlignCenter, QString(text.at(cell)));
    }
}

void SymFloatInput::keyPressEvent(QKeyEvent* event)
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
            int c = m_cursor;
            do {
                c = (c - 1 + totalCells()) % totalCells();
            } while (cellKind(c) == CellKind::Dot);
            m_cursor = c;
            update();
            event->accept();
            return;
        }
        break;
    case Qt::Key_Right:
        if (isEditing()) {
            int c = m_cursor;
            do {
                c = (c + 1) % totalCells();
            } while (cellKind(c) == CellKind::Dot);
            m_cursor = c;
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

void SymFloatInput::mousePressEvent(QMouseEvent* event)
{
    if (consumeActivationClick() && !isEditing())
        beginEdit();
    QWidget::mousePressEvent(event);
}

void SymFloatInput::focusOutEvent(QFocusEvent* event)
{
    if (isEditing())
        commitEdit();
    SymWidget::focusOutEvent(event);
}

} // namespace sym
