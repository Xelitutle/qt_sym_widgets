#include "symwidgets/Label.h"

#include <QPainter>

namespace sym {

Label::Label(const QString& text, SymWidget* parent)
    : SymWidget(parent)
    , m_text(text)
{
    setCellSize(qMax(1, m_text.length()), 1);
}

void Label::setText(const QString& text)
{
    m_text = text;
    setCellSize(qMax(1, m_text.length()), 1);
    update();
}

void Label::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setFont(font());
    painter.setPen(theme().textColor);
    painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, m_text);
}

} // namespace sym
