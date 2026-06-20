#pragma once

#include <QWidget>

#include "symwidgets/GridMetrics.h"
#include "symwidgets/Theme.h"

namespace sym {

class SymMainWidget;
class SymFrame;

class SymWidget : public QWidget {
    Q_OBJECT
public:
    explicit SymWidget(SymWidget* parent = nullptr);

    void setVisible(bool visible) override;

    void moveToCell(int col, int row);
    int row() const { return m_row; }
    int col() const { return m_col; }

    int widthCells() const { return m_widthCells; }
    int heightCells() const { return m_heightCells; }

    virtual bool focusable() const { return false; }

    // The color this widget is currently painting itself with (depends on
    // Normal/Focused/Editing/Disabled state, and any transient effect such
    // as a button's press flash). Used by SymFrame to color the focus
    // brackets to match.
    virtual QColor currentColor() const;

    // The background color this widget is currently painting itself with
    // (transparent in Normal state, filled while Focused/Editing, or during
    // a transient effect such as a button's press flash). Used by SymFrame
    // to fill the focus brackets' background to match.
    virtual QColor currentBackgroundColor() const;

protected:
    enum class VisualState { Normal, Focused, Editing, Disabled };

    // Used only by SymMainWidget, which is the root of the tree and therefore
    // needs a plain QWidget parent (or none) instead of a SymWidget parent.
    explicit SymWidget(QWidget* parent);

    SymMainWidget* mainWidget() const;
    const GridMetrics& gridMetrics() const;
    const Theme& theme() const;
    const QFont& font() const;

    void setCellSize(int widthCells, int heightCells);

    VisualState visualState() const;
    bool isEditing() const { return m_editing; }
    void setEditing(bool editing) { m_editing = editing; }

    // Returns true if this click happened while the widget already had focus
    // (i.e. it is an activation click, not a focus-transferring click).
    bool consumeActivationClick();

    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    using QWidget::resize;

    SymFrame* enclosingFrame() const;
    void redirectFocusAwayFromSelf();

    int m_row = 0;
    int m_col = 0;
    int m_widthCells = 1;
    int m_heightCells = 1;
    bool m_editing = false;
    bool m_focusJustGained = false;
    mutable SymMainWidget* m_mainWidgetCache = nullptr;
};

} // namespace sym
