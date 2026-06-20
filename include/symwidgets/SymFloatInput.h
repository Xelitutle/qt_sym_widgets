#pragma once

#include "symwidgets/SymWidget.h"

namespace sym {

class SymFloatInput : public SymWidget {
    Q_OBJECT
public:
    SymFloatInput(double minValue, double maxValue, int precision, SymWidget* parent = nullptr);

    void setValue(double value);
    double value() const;
    double minValue() const { return m_minValue; }
    double maxValue() const { return m_maxValue; }
    int precision() const { return m_precision; }
    bool focusable() const override { return true; }

signals:
    void changed(double value);
    void startEdit();
    void stopEdit();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    enum class CellKind { Sign, IntDigit, Dot, FracDigit };

    bool isSigned() const { return m_minValue < 0; }
    int totalCells() const { return (isSigned() ? 1 : 0) + m_intDigits + 1 + m_precision; }
    CellKind cellKind(int cell) const;
    int combinedDigitIndex(int cell) const; // valid for IntDigit/FracDigit cells

    void beginEdit();
    void commitEdit();
    void bumpDigit(int cell, int delta);
    void stepWholeValue(long long deltaScaledUnits);
    QString combinedDigitsString() const;
    QString displayText() const;
    long long scale() const;

    double m_minValue;
    double m_maxValue;
    int m_precision;
    int m_intDigits;
    bool m_negative = false;
    long long m_absScaled = 0;
    int m_cursor = 0;
};

} // namespace sym
