#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QWidget>

#include "symwidgets/SymLabel.h"
#include "symwidgets/SymButton.h"
#include "symwidgets/SymCheckBox.h"
#include "symwidgets/SymFloatInput.h"
#include "symwidgets/SymFrame.h"
#include "symwidgets/SymIntInput.h"
#include "symwidgets/SymMainWidget.h"

using namespace sym;

namespace {

Theme makeTheme()
{
    Theme theme;
    theme.substrateColor = QColor(67, 67, 67, 127);
    theme.substrateAlpha = 235;
    theme.frameColor = QColor(255, 255, 255);
    theme.textColor = QColor(127, 127, 127);
    theme.widgetColorNormal = QColor(255, 255, 255);
    theme.widgetColorFocused = QColor(1, 1, 1);
    theme.widgetColorEditing = QColor(1, 1, 1);
    theme.digitHighlightColor = QColor(255, 255, 255);
    theme.widgetColorDisabled = QColor(100, 100, 100);
    return theme;
}

// Diagonal-ramp GRAY8 test pattern ("косой матрас") — value at each pixel is
// (x + y) wrapped into a byte, producing repeating 45-degree stripes.
QImage makeDiagonalRampImage(int width, int height)
{
    QImage image(width, height, QImage::Format_Grayscale8);
    for (int y = 0; y < height; ++y) {
        uchar* line = image.scanLine(y);
        for (int x = 0; x < width; ++x)
            line[x] = static_cast<uchar>((x + y) & 0xFF);
    }
    return image;
}

class BackgroundWidget : public QWidget {
public:
    explicit BackgroundWidget(QImage image, QWidget* parent = nullptr)
        : QWidget(parent)
        , m_image(std::move(image))
    {
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.drawImage(rect(), m_image);
    }

private:
    QImage m_image;
};

} // namespace

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    GridMetrics metrics;
    metrics.rows = 22;
    metrics.cols = 56;
    metrics.screenWidthPx = 1024;
    metrics.screenHeightPx = 768;

    auto* background = new BackgroundWidget(makeDiagonalRampImage(metrics.screenWidthPx, metrics.screenHeightPx));
    background->setWindowTitle(QStringLiteral("SymWidgets — демонстрация"));
    background->resize(metrics.screenWidthPx, metrics.screenHeightPx);

    SymMainWidget mainWidget(metrics, makeTheme(), QStringLiteral(ETC_DIR) + "/font/andalemono.ttf", background);

    // SymMainWidget auto-sizes itself to cols*cellWidthPx x rows*cellHeightPx,
    // which can be smaller than the window when the grid doesn't divide it
    // evenly. The substrate itself should still cover the whole window, so
    // stretch it to the full background size (TZ explicitly sanctions this
    // QWidget* cast as the way to bypass the hidden resize()) and instead
    // center the grid content inside it by offsetting the root SymFrame.
    static_cast<QWidget*>(&mainWidget)->resize(metrics.screenWidthPx, metrics.screenHeightPx);

    auto* outer = new SymFrame(QStringLiteral("Знакосимвольный интерфейс"), &mainWidget);
    const int gridWidthPx = metrics.cols * metrics.cellWidthPx();
    const int gridHeightPx = metrics.rows * metrics.cellHeightPx();
    outer->move((metrics.screenWidthPx - gridWidthPx) / 2, (metrics.screenHeightPx - gridHeightPx) / 2);

    auto* statusLabel = new SymLabel(QStringLiteral("Статус: готов"), outer);
    statusLabel->moveToCell(1, 1);

    auto* nested = new SymFrame(QStringLiteral("Вложенная рамка"), 4, 20, outer);
    nested->moveToCell(1, 2);

    auto* okButton = new SymButton(QStringLiteral("Продолжить"), nested);
    okButton->moveToCell(2, 1);

    auto* cancelButton = new SymButton(QStringLiteral("Отмена"), nested);
    cancelButton->moveToCell(2, 2);

    auto* checkLabel = new SymLabel(QStringLiteral("Флаг:"), outer);
    checkLabel->moveToCell(1, 6);

    auto* checkBox = new SymCheckBox(outer);
    checkBox->moveToCell(7, 6);

    auto* intLabel = new SymLabel(QStringLiteral("Целое:"), outer);
    intLabel->moveToCell(1, 7);

    auto* intInput = new SymIntInput(-99, 99, outer);
    intInput->moveToCell(8, 7);

    auto* intInputDis = new SymIntInput(-99, 99, outer);
    intInputDis->moveToCell(16, 7);
    intInputDis->setEnabled(false);

    auto* blinkLabel = new SymLabel(QStringLiteral("Мигающее:"), outer);
    blinkLabel->moveToCell(1, 10);

    auto* blinkInput = new SymIntInput(0, 99, outer);
    blinkInput->moveToCell(12, 10);

    auto* blinkTimer = new QTimer(&mainWidget);
    QObject::connect(blinkTimer, &QTimer::timeout, [blinkInput] {
        blinkInput->setVisible(!blinkInput->isVisible());
    });
    blinkTimer->start(3000);

    auto* floatLabel = new SymLabel(QStringLiteral("Вещественное:"), outer);
    floatLabel->moveToCell(1, 8);

    auto* floatInput = new SymFloatInput(-999.999, 999.999, 3, outer);
    floatInput->moveToCell(15, 8);

    auto* centerLabel = new SymLabel(QStringLiteral("Многострочный\nпо\nцентру"), outer);
    centerLabel->setAlignment(SymLabel::Alignment::Center);
    centerLabel->moveToCell(1, 11);

    auto* rightLabel = new SymLabel(QStringLiteral("Многострочный\nпо\nправому краю"), outer);
    rightLabel->setAlignment(SymLabel::Alignment::Right);
    rightLabel->moveToCell(1, 14);

    auto* quitButton = new SymButton(QStringLiteral("Выход"), outer);
    quitButton->moveToCell(3, 21);

    QObject::connect(okButton, &SymButton::pressed, [statusLabel] {
        statusLabel->setText(QStringLiteral("Статус: нажата OK"));
    });
    QObject::connect(cancelButton, &SymButton::pressed, [statusLabel] {
        statusLabel->setText(QStringLiteral("Статус: нажата Cancel"));
    });
    QObject::connect(checkBox, &SymCheckBox::changed, [statusLabel](bool checked) {
        statusLabel->setText(QStringLiteral("Статус: чекбокс = ") + (checked ? "true" : "false"));
    });
    QObject::connect(intInput, &SymIntInput::changed, [statusLabel](long long value) {
        statusLabel->setText(QStringLiteral("Статус: int = %1").arg(value));
    });
    QObject::connect(floatInput, &SymFloatInput::changed, [statusLabel](double value) {
        statusLabel->setText(QStringLiteral("Статус: float = %1").arg(value, 0, 'f', 2));
    });
    QObject::connect(quitButton, &SymButton::pressed, &app, &QApplication::quit);

    background->show();
    okButton->setFocus(Qt::OtherFocusReason);

    return app.exec();
}
