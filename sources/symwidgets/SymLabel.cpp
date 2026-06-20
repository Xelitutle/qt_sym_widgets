#include "symwidgets/SymLabel.h"

#include <QPainter>

namespace sym {

SymLabel::SymLabel(const QString& text, SymWidget* parent)
    : SymWidget(parent)
    , m_text(text)
{
    rebuildLines();
}

void SymLabel::setText(const QString& text)
{
    m_text = text;
    rebuildLines();
    update();
}

void SymLabel::setAlignment(Alignment alignment)
{
    if (m_alignment == alignment)
        return;
    m_alignment = alignment;
    update();
}

void SymLabel::rebuildLines()
{
    m_lines = m_text.split(QChar('\n'));

    int maxLen = 1;
    for (const QString& line : m_lines)
        maxLen = qMax(maxLen, line.length());

    setCellSize(maxLen, qMax(1, m_lines.count()));
}

void SymLabel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setFont(font());
    painter.setPen(theme().textColor);

    const int cellW = gridMetrics().cellWidthPx();
    const int cellH = gridMetrics().cellHeightPx();
    const int maxLen = widthCells();

    for (int i = 0; i < m_lines.count(); ++i) {
        const QString& line = m_lines.at(i);
        const int pad = maxLen - line.length();

        int leftPad = 0;
        switch (m_alignment) {
        case Alignment::Left:   leftPad = 0;       break;
        case Alignment::Right:  leftPad = pad;     break;
        case Alignment::Center: leftPad = pad / 2; break;
        }

        const QRect lineRect(leftPad * cellW, i * cellH, line.length() * cellW, cellH);
        painter.drawText(lineRect, Qt::AlignLeft | Qt::AlignVCenter, line);
    }
}

} // namespace sym
