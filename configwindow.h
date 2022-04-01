#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QDialog>

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigWindow(QWidget *parent = nullptr);
    ~ConfigWindow();

private slots:
    void on_buttonBox_accepted();

    void on_brushSizeSlider_valueChanged(int value);

private:
    Ui::ConfigWindow *ui;
    const uint8_t brushMaxSize = 200u;
    const uint8_t brushMinSize = 1u;

    void showEvent(QShowEvent* event) override;
};

#endif // CONFIGWINDOW_H
