#pragma once

#include "symwidgets/SymWidget.h"

namespace sym {

class SymLabel : public SymWidget {
    Q_OBJECT
public:
    explicit SymLabel(const QString& text, SymWidget* parent = nullptr);

    void setText(const QString& text);
    QString text() const { return m_text; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_text;
};

} // namespace sym
