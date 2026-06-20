#include "symwidgets/SymFrame.h"

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QVector>
#include <cmath>
#include <functional>

#include "symwidgets/SymButton.h"
#include "symwidgets/SymMainWidget.h"

namespace sym {

namespace {
double centerDistance(SymWidget* a, SymWidget* b, QWidget* relativeTo)
{
    const QRect aRect(a->mapTo(relativeTo, QPoint(0, 0)), a->size());
    const QRect bRect(b->mapTo(relativeTo, QPoint(0, 0)), b->size());
    const QPoint d = aRect.center() - bRect.center();
    return std::sqrt(static_cast<double>(d.x()) * d.x() + static_cast<double>(d.y()) * d.y());
}
} // namespace

SymFrame::SymFrame(const QString& title, SymWidget* parent)
    : SymWidget(parent)
    , m_title(title)
{
    setFocusPolicy(Qt::NoFocus);
    setCellSize(parent->widthCells(), parent->heightCells());
}

SymFrame::SymFrame(const QString& title, int rows, int cols, SymWidget* parent)
    : SymWidget(parent)
    , m_title(title)
{
    setFocusPolicy(Qt::NoFocus);
    setCellSize(cols, rows);
}

void SymFrame::paintEvent(QPaintEvent*)
{
    const int rows = heightCells();
    const int cols = widthCells();
    if (rows <= 0 || cols <= 0)
        return;

    QVector<QVector<bool>> occupied(rows, QVector<bool>(cols, false));
    for (QObject* obj : children()) {
        auto* sw = qobject_cast<SymWidget*>(obj);
        if (!sw)
            continue;
        for (int r = sw->row(); r < sw->row() + sw->heightCells() && r < rows; ++r) {
            if (r < 0)
                continue;
            for (int c = sw->col(); c < sw->col() + sw->widthCells() && c < cols; ++c) {
                if (c < 0)
                    continue;
                occupied[r][c] = true;
            }
        }
    }

    // Cells reserved to the immediate left/right of every focusable widget,
    // kept blank at all times so the focus brackets have room to appear
    // without ever needing to overwrite a border character.
    QVector<QVector<SymWidget*>> bracketZone(rows, QVector<SymWidget*>(cols, nullptr));
    for (QObject* obj : children()) {
        auto* sw = qobject_cast<SymWidget*>(obj);
        if (!sw || !sw->focusable())
            continue;
        const int r = sw->row();
        if (r < 0 || r >= rows)
            continue;
        const int leftCol = sw->col() - 1;
        const int rightCol = sw->col() + sw->widthCells();
        if (leftCol >= 0 && leftCol < cols && !occupied[r][leftCol])
            bracketZone[r][leftCol] = sw;
        if (rightCol >= 0 && rightCol < cols && !occupied[r][rightCol])
            bracketZone[r][rightCol] = sw;
    }

    auto isBlocked = [&](int r, int c) { return occupied[r][c] || bracketZone[r][c] != nullptr; };

    QVector<QVector<QChar>> grid(rows, QVector<QChar>(cols, QChar()));
    for (int c = 0; c < cols; ++c) {
        if (!isBlocked(0, c))
            grid[0][c] = QChar(0x2500); // ─
        if (!isBlocked(rows - 1, c))
            grid[rows - 1][c] = QChar(0x2500);
    }
    for (int r = 0; r < rows; ++r) {
        if (!isBlocked(r, 0))
            grid[r][0] = QChar(0x2502); // │
        if (!isBlocked(r, cols - 1))
            grid[r][cols - 1] = QChar(0x2502);
    }
    if (!isBlocked(0, 0))
        grid[0][0] = QChar(0x250C); // ┌
    if (!isBlocked(0, cols - 1))
        grid[0][cols - 1] = QChar(0x2510); // ┐
    if (!isBlocked(rows - 1, 0))
        grid[rows - 1][0] = QChar(0x2514); // └
    if (!isBlocked(rows - 1, cols - 1))
        grid[rows - 1][cols - 1] = QChar(0x2518); // ┘

    if (!m_title.isEmpty()) {
        const int maxLen = qMax(0, cols - 2);
        const QString title = (QStringLiteral(" ") + m_title + QStringLiteral(" ")).left(maxLen);
        for (int i = 0; i < title.length(); ++i) {
            const int c = 2 + i;
            if (!isBlocked(0, c))
                grid[0][c] = title.at(i);
        }
    }

    QVector<QVector<QColor>> color(rows, QVector<QColor>(cols, theme().frameColor));
    QVector<QVector<QColor>> background(rows, QVector<QColor>(cols, QColor(Qt::transparent)));

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            SymWidget* sw = bracketZone[r][c];
            if (!sw || !sw->hasFocus())
                continue;
            grid[r][c] = (c < sw->col()) ? QChar('[') : QChar(']');
            color[r][c] = sw->currentColor();
            background[r][c] = sw->currentBackgroundColor();
        }
    }

    QPainter painter(this);
    painter.setFont(font());
    const int cellW = gridMetrics().cellWidthPx();
    const int cellH = gridMetrics().cellHeightPx();
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c].isNull())
                continue;
            const QRect cellRect(c * cellW, r * cellH, cellW, cellH);
            if (background[r][c].alpha() != 0)
                painter.fillRect(cellRect, background[r][c]);
            painter.setPen(color[r][c]);
            painter.drawText(cellRect, Qt::AlignCenter, QString(grid[r][c]));
        }
    }
}

SymWidget* SymFrame::findNearestFocusable(SymWidget* from, InputButton direction) const
{
    auto* self = const_cast<SymFrame*>(this);
    const QPoint fromTopLeft = from->mapTo(self, QPoint(0, 0));
    const QRect fromRect(fromTopLeft, from->size());
    const QPoint fromCenter = fromRect.center();

    SymWidget* best = nullptr;
    double bestDist = -1.0;

    std::function<void(const QObject*)> visit = [&](const QObject* node) {
        for (QObject* child : node->children()) {
            auto* sw = qobject_cast<SymWidget*>(child);
            if (sw && sw != from && sw->focusable() && sw->isEnabled() && sw->isVisible()) {
                const QPoint topLeft = sw->mapTo(self, QPoint(0, 0));
                const QRect r(topLeft, sw->size());
                const QPoint center = r.center();
                bool inDirection = false;
                switch (direction) {
                case InputButton::Up:    inDirection = center.y() < fromCenter.y(); break;
                case InputButton::Down:  inDirection = center.y() > fromCenter.y(); break;
                case InputButton::Left:  inDirection = center.x() < fromCenter.x(); break;
                case InputButton::Right: inDirection = center.x() > fromCenter.x(); break;
                }
                if (inDirection) {
                    const double dx = center.x() - fromCenter.x();
                    const double dy = center.y() - fromCenter.y();
                    const double dist = std::sqrt(dx * dx + dy * dy);
                    if (!best || dist < bestDist) {
                        best = sw;
                        bestDist = dist;
                    }
                }
            }
            visit(child);
        }
    };
    visit(this);
    return best;
}

SymWidget* SymFrame::findFallbackFocusable(SymWidget* from) const
{
    auto* self = const_cast<SymFrame*>(this);

    QVector<SymWidget*> candidates;
    std::function<void(const QObject*)> visit = [&](const QObject* node) {
        for (QObject* child : node->children()) {
            auto* sw = qobject_cast<SymWidget*>(child);
            if (sw && sw != from && sw->focusable() && sw->isEnabled() && sw->isVisible())
                candidates.append(sw);
            visit(child);
        }
    };
    visit(this);

    if (candidates.isEmpty())
        return nullptr;

    if (auto* main = mainWidget()) {
        for (const QString& text : main->focusFallbackButtonTexts()) {
            SymButton* best = nullptr;
            double bestDist = -1.0;
            for (SymWidget* sw : candidates) {
                auto* button = qobject_cast<SymButton*>(sw);
                if (!button || button->text() != text)
                    continue;
                const double dist = centerDistance(from, button, self);
                if (!best || dist < bestDist) {
                    best = button;
                    bestDist = dist;
                }
            }
            if (best)
                return best;
        }
    }

    SymWidget* nearest = nullptr;
    double bestDist = -1.0;
    for (SymWidget* sw : candidates) {
        const double dist = centerDistance(from, sw, self);
        if (!nearest || dist < bestDist) {
            nearest = sw;
            bestDist = dist;
        }
    }
    return nearest;
}

void SymFrame::keyPressEvent(QKeyEvent* event)
{
    InputButton direction;
    bool isNav = true;
    switch (event->key()) {
    case Qt::Key_Up:    direction = InputButton::Up;    break;
    case Qt::Key_Down:  direction = InputButton::Down;  break;
    case Qt::Key_Left:  direction = InputButton::Left;  break;
    case Qt::Key_Right: direction = InputButton::Right; break;
    default:
        isNav = false;
        break;
    }

    if (isNav) {
        if (auto* focused = qobject_cast<SymWidget*>(QApplication::focusWidget())) {
            if (SymWidget* target = findNearestFocusable(focused, direction)) {
                target->setFocus(Qt::OtherFocusReason);
                event->accept();
                return;
            }
        }
    }
    QWidget::keyPressEvent(event);
}

} // namespace sym
