#include <QApplication>
#include <QTimer>

#include "symwidgets/Label.h"
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
    theme.substrateColor = QColor(20, 24, 28);
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

} // namespace

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    GridMetrics metrics;
    metrics.rows = 22;
    metrics.cols = 56;
    metrics.screenWidthPx = 1024;
    metrics.screenHeightPx = 768;

    SymMainWidget mainWidget(metrics, makeTheme(), QStringLiteral(ETC_DIR) + "/font/andalemono.ttf");
    mainWidget.setWindowTitle(QStringLiteral("SymWidgets — демонстрация"));

    auto* outer = new SymFrame(QStringLiteral("Знакосимвольный интерфейс"), &mainWidget);

    auto* statusLabel = new Label(QStringLiteral("Статус: готов"), outer);
    statusLabel->moveToCell(1, 1);

    auto* nested = new SymFrame(QStringLiteral("Вложенная рамка"), 4, 20, outer);
    nested->moveToCell(1, 2);

    auto* okButton = new SymButton(QStringLiteral("Продолжить"), nested);
    okButton->moveToCell(2, 1);

    auto* cancelButton = new SymButton(QStringLiteral("Отмена"), nested);
    cancelButton->moveToCell(2, 2);

    auto* checkLabel = new Label(QStringLiteral("Флаг:"), outer);
    checkLabel->moveToCell(1, 6);

    auto* checkBox = new SymCheckBox(outer);
    checkBox->moveToCell(7, 6);

    auto* intLabel = new Label(QStringLiteral("Целое:"), outer);
    intLabel->moveToCell(1, 7);

    auto* intInput = new SymIntInput(-99, 99, outer);
    intInput->moveToCell(8, 7);

    auto* intInputDis = new SymIntInput(-99, 99, outer);
    intInputDis->moveToCell(16, 7);
    intInputDis->setEnabled(false);

    auto* blinkLabel = new Label(QStringLiteral("Мигающее:"), outer);
    blinkLabel->moveToCell(1, 10);

    auto* blinkInput = new SymIntInput(0, 99, outer);
    blinkInput->moveToCell(12, 10);

    auto* blinkTimer = new QTimer(&mainWidget);
    QObject::connect(blinkTimer, &QTimer::timeout, [blinkInput] {
        blinkInput->setVisible(!blinkInput->isVisible());
    });
    blinkTimer->start(3000);

    auto* floatLabel = new Label(QStringLiteral("Вещественное:"), outer);
    floatLabel->moveToCell(1, 8);

    auto* floatInput = new SymFloatInput(-999.999, 999.999, 3, outer);
    floatInput->moveToCell(15, 8);

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

    mainWidget.show();
    okButton->setFocus(Qt::OtherFocusReason);

    return app.exec();
}
