#include "symwidgets/SymWidget.h"

#include <QApplication>
#include <QEvent>
#include <QFocusEvent>

#include "symwidgets/SymFrame.h"
#include "symwidgets/SymMainWidget.h"

namespace sym {

SymWidget::SymWidget(SymWidget* parent)
    : QWidget(parent)
{
}

SymWidget::SymWidget(QWidget* parent)
    : QWidget(parent)
{
}

void SymWidget::moveToCell(int col, int row)
{
    m_row = row;
    m_col = col;
    const GridMetrics& gm = gridMetrics();
    QWidget::move(col * gm.cellWidthPx(), row * gm.cellHeightPx());
}

void SymWidget::setCellSize(int widthCells, int heightCells)
{
    m_widthCells = widthCells;
    m_heightCells = heightCells;
    const GridMetrics& gm = gridMetrics();
    QWidget::resize(widthCells * gm.cellWidthPx(), heightCells * gm.cellHeightPx());
}

SymMainWidget* SymWidget::mainWidget() const
{
    if (m_mainWidgetCache)
        return m_mainWidgetCache;

    if (auto* self = qobject_cast<SymMainWidget*>(const_cast<SymWidget*>(this))) {
        m_mainWidgetCache = self;
        return m_mainWidgetCache;
    }

    QWidget* p = parentWidget();
    while (p) {
        if (auto* main = qobject_cast<SymMainWidget*>(p)) {
            m_mainWidgetCache = main;
            return m_mainWidgetCache;
        }
        p = p->parentWidget();
    }

    return nullptr;
}

const GridMetrics& SymWidget::gridMetrics() const
{
    return mainWidget()->gridMetrics();
}

const Theme& SymWidget::theme() const
{
    return mainWidget()->theme();
}

const QFont& SymWidget::font() const
{
    return mainWidget()->font();
}

SymWidget::VisualState SymWidget::visualState() const
{
    if (!isEnabled())
        return VisualState::Disabled;
    if (hasFocus() && m_editing)
        return VisualState::Editing;
    if (hasFocus())
        return VisualState::Focused;
    return VisualState::Normal;
}

QColor SymWidget::currentColor() const
{
    switch (visualState()) {
    case VisualState::Disabled:
        return theme().widgetColorDisabled;
    case VisualState::Focused:
    case VisualState::Editing:
        return theme().widgetColorFocused;
    default:
        return theme().widgetColorNormal;
    }
}

QColor SymWidget::currentBackgroundColor() const
{
    if (visualState() == VisualState::Focused || visualState() == VisualState::Editing)
        return QColor(255, 255, 255);
    return Qt::transparent;
}

bool SymWidget::consumeActivationClick()
{
    const bool wasAlreadyFocused = !m_focusJustGained;
    m_focusJustGained = false;
    return wasAlreadyFocused;
}

void SymWidget::focusInEvent(QFocusEvent* event)
{
    m_focusJustGained = true;
    update();
    if (parentWidget())
        parentWidget()->update();
    QWidget::focusInEvent(event);
}

void SymWidget::focusOutEvent(QFocusEvent* event)
{
    m_focusJustGained = false;
    update();
    if (parentWidget())
        parentWidget()->update();
    QWidget::focusOutEvent(event);
}

void SymWidget::setVisible(bool visible)
{
    const bool losingFocus = !visible && QApplication::focusWidget() == this;
    QWidget::setVisible(visible);
    if (losingFocus)
        redirectFocusAwayFromSelf();
}

void SymWidget::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::EnabledChange && !isEnabled() && QApplication::focusWidget() == this)
        redirectFocusAwayFromSelf();
}

SymFrame* SymWidget::enclosingFrame() const
{
    QWidget* p = parentWidget();
    while (p) {
        if (auto* frame = qobject_cast<SymFrame*>(p))
            return frame;
        p = p->parentWidget();
    }
    return nullptr;
}

void SymWidget::redirectFocusAwayFromSelf()
{
    if (SymFrame* frame = enclosingFrame()) {
        if (SymWidget* target = frame->findFallbackFocusable(this)) {
            target->setFocus(Qt::OtherFocusReason);
            return;
        }
    }
    clearFocus();
}

} // namespace sym
