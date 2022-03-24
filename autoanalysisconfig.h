#ifndef AUTOANALYSISCONFIG_H
#define AUTOANALYSISCONFIG_H

#include <QDialog>

namespace Ui {
class AutoAnalysisConfig;
}

class AutoAnalysisConfig : public QDialog
{
    Q_OBJECT

public:
    explicit AutoAnalysisConfig(class MainWindow *parent = nullptr);
    ~AutoAnalysisConfig();

private slots:
    void on_processFullRangeCheckbox_stateChanged(int arg1);

    void on_buttonBox_accepted();

private:
    MainWindow* parent = nullptr;
    Ui::AutoAnalysisConfig *ui;
};

#endif // AUTOANALYSISCONFIG_H
