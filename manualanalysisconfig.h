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
    explicit ManualAnalysisConfig(QWidget* parent);
    ~ManualAnalysisConfig();

private slots:
    void on_thresholdSlider_valueChanged(int value);

    void on_thresholdInput_valueChanged(double arg1);

    void on_buttonBox_accepted();

private:
    Ui::ManualAnalysisConfig *ui;
    const int maxSliderValue = 1000;

    void showEvent(QShowEvent* event) override;
};

#endif // MANUALANALYSISCONFIG_H
