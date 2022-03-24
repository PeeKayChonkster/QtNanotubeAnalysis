#ifndef MANUALANALYSISCONFIG_H
#define MANUALANALYSISCONFIG_H

#include <QDialog>

namespace Ui {
class ManualAnalysisConfig;
}

class ManualAnalysisConfig : public QDialog
{
    Q_OBJECT

public:
    ManualAnalysisConfig() = delete;
    explicit ManualAnalysisConfig(class MainWindow* parent = nullptr);
    ~ManualAnalysisConfig();

private slots:
    void accept() override;

    void on_thresholdSlider_valueChanged(int value);

    void on_thresholdInput_valueChanged(double arg1);

private:
    Ui::ManualAnalysisConfig *ui;
    MainWindow* parent = nullptr;
    const int maxSliderValue = 10000;
};

#endif // MANUALANALYSISCONFIG_H
