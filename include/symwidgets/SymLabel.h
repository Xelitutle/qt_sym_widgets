#pragma once

#include <QStringList>

#include "symwidgets/SymWidget.h"

namespace sym {

class SymLabel : public SymWidget {
    Q_OBJECT
public:
    enum class Alignment { Left, Right, Center };

    explicit SymLabel(const QString& text, SymWidget* parent = nullptr);

    void setText(const QString& text);
    QString text() const { return m_text; }

    // Alignment applies per line, within the width of the longest line (\n
    // splits text into multiple lines; the widget's cell width is always
    // that of its longest line). Padding is computed in whole cells so
    // every character still lands on a cell boundary.
    void setAlignment(Alignment alignment);
    Alignment alignment() const { return m_alignment; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void rebuildLines();

    QString m_text;
    QStringList m_lines;
    Alignment m_alignment = Alignment::Left;
};

} // namespace sym
