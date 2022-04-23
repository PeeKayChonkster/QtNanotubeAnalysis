#ifndef FULLRANGEANALYSISCONFIG_H
#define FULLRANGEANALYSISCONFIG_H

#include <QDialog>

namespace Ui {
class FullRangeAnalysisConfig;
}

class FullRangeAnalysisConfig : public QDialog
{
    Q_OBJECT

public:
    explicit FullRangeAnalysisConfig(QWidget *parent = nullptr);
    ~FullRangeAnalysisConfig();

private:
    Ui::FullRangeAnalysisConfig *ui;
};

#endif // FULLRANGEANALYSISCONFIG_H
