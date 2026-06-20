#include "symwidgets/SymLabel.h"

#include <QPainter>

namespace sym {

SymLabel::SymLabel(const QString& text, SymWidget* parent)
    : SymWidget(parent)
    , m_text(text)
{
    setCellSize(qMax(1, m_text.length()), 1);
}

void SymLabel::setText(const QString& text)
{
    m_text = text;
    setCellSize(qMax(1, m_text.length()), 1);
    update();
}

void SymLabel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setFont(font());
    painter.setPen(theme().textColor);
    painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, m_text);
}

} // namespace sym
