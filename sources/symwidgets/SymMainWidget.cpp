#include "symwidgets/SymMainWidget.h"

#include <QFontDatabase>
#include <QFontMetrics>
#include <QPainter>

namespace sym {

namespace {

// Picks a pixel size so the glyph fits inside the cell box, then stretches
// the font horizontally so its advance width exactly matches cellWidthPx —
// without this, box-drawing characters (─, │) leave gaps between cells and
// frame borders don't read as a single continuous line.
QFont fitFontToCell(const QFont& baseFont, const GridMetrics& metrics)
{
    QFont font = baseFont;
    const QChar calibrationChar(QLatin1Char('0'));

    int pixelSize = 1;
    for (int candidate = 4; candidate <= 256; ++candidate) {
        QFont probe = font;
        probe.setPixelSize(candidate);
        const QFontMetrics fm(probe);
        if (fm.horizontalAdvance(calibrationChar) > metrics.cellWidthPx() || fm.height() > metrics.cellHeightPx())
            break;
        pixelSize = candidate;
    }
    font.setPixelSize(pixelSize);

    const QFontMetrics fm(font);
    const int charWidth = fm.horizontalAdvance(calibrationChar);
    if (charWidth > 0) {
        const int stretch = qRound(100.0 * metrics.cellWidthPx() / charWidth);
        font.setStretch(qBound(1, stretch, 4000));
    }
    return font;
}

} // namespace

SymMainWidget::SymMainWidget(const GridMetrics& metrics,
                              const Theme& theme,
                              const QString& fontFilePath,
                              QWidget* parent)
    : SymWidget(parent)
    , m_metrics(metrics)
    , m_theme(theme)
{
    QString family;
    const int fontId = QFontDatabase::addApplicationFont(fontFilePath);
    if (fontId != -1) {
        const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty())
            family = families.first();
    }
    QFont font = family.isEmpty() ? QFont() : QFont(family);
    font.setStyleHint(QFont::Monospace);
    m_font = fitFontToCell(font, m_metrics);

    setCellSize(m_metrics.cols, m_metrics.rows);
}

void SymMainWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QColor color = m_theme.substrateColor;
    color.setAlpha(m_theme.substrateAlpha);
    painter.fillRect(rect(), color);
}

} // namespace sym
