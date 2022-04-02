#ifndef CURRENTMASKANALYSISCONFIG_H
#define CURRENTMASKANALYSISCONFIG_H

#include <QDialog>

namespace Ui {
class CurrentMaskAnalysisConfig;
}

class CurrentMaskAnalysisConfig : public QDialog
{
    Q_OBJECT

public:
    explicit CurrentMaskAnalysisConfig(QWidget *parent = nullptr);
    ~CurrentMaskAnalysisConfig();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::CurrentMaskAnalysisConfig *ui;

    void showEvent(QShowEvent* event) override;

    void accept() override;
};

#endif // CURRENTMASKANALYSISCONFIG_H
