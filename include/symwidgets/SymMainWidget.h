#pragma once

#include <QFont>
#include <QStringList>

#include "symwidgets/SymWidget.h"

namespace sym {

class SymMainWidget : public SymWidget {
    Q_OBJECT
public:
    SymMainWidget(const GridMetrics& metrics,
                  const Theme& theme,
                  const QString& fontFilePath,
                  QWidget* parent = nullptr);

    const GridMetrics& gridMetrics() const { return m_metrics; }
    const Theme& theme() const { return m_theme; }
    const QFont& font() const { return m_font; }

    // Ordered by priority: when a focused widget is disabled/hidden and a
    // fallback target must be picked, SymFrame looks for a SymButton whose
    // text() matches one of these, in order, before falling back to the
    // nearest focusable widget. Callers can extend this list at any time.
    void addFocusFallbackButtonText(const QString& text) { m_focusFallbackTexts.append(text); }
    const QStringList& focusFallbackButtonTexts() const { return m_focusFallbackTexts; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    GridMetrics m_metrics;
    Theme m_theme;
    QFont m_font;
    QStringList m_focusFallbackTexts = { QStringLiteral("Выход"), QStringLiteral("Назад") };
};

} // namespace sym
