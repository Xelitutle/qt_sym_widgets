#pragma once

#include "symwidgets/SymWidget.h"

namespace sym {

class SymIntInput : public SymWidget {
    Q_OBJECT
public:
    SymIntInput(long long minValue, long long maxValue, SymWidget* parent = nullptr);

    void setValue(long long value);
    long long value() const;
    long long minValue() const { return m_minValue; }
    long long maxValue() const { return m_maxValue; }
    bool focusable() const override { return true; }

signals:
    void changed(long long value);
    void startEdit();
    void stopEdit();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    bool isSigned() const { return m_minValue < 0; }
    int slotCount() const { return m_numDigits + (isSigned() ? 1 : 0); }
    bool isSignSlot(int slot) const { return isSigned() && slot == 0; }
    int digitIndexForSlot(int slot) const { return slot - (isSigned() ? 1 : 0); }

    void beginEdit();
    void commitEdit();
    void bumpDigit(int slot, int delta);
    void stepWholeValue(int delta);
    QString digitsString() const;
    QString displayDigitsString() const;

    long long m_minValue;
    long long m_maxValue;
    int m_numDigits;
    bool m_negative = false;
    long long m_absValue = 0;
    int m_cursor = 0;
};

} // namespace sym
