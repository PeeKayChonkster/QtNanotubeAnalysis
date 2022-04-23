#ifndef THRESHOLDANALYSISCONFIG_H
#define THRESHOLDANALYSISCONFIG_H

#include <QDialog>

namespace Ui {
class ThresholdAnalysisConfig;
}

class ThresholdAnalysisConfig : public QDialog
{
    Q_OBJECT

public:
    explicit ThresholdAnalysisConfig(QWidget *parent = nullptr);
    ~ThresholdAnalysisConfig();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ThresholdAnalysisConfig *ui;

    void showEvent(QShowEvent *event) override;
};

#endif // THRESHOLDANALYSISCONFIG_H
