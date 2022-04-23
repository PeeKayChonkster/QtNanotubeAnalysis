#include "fullrangeanalysisconfig.h"
#include "ui_fullrangeanalysisconfig.h"

FullRangeAnalysisConfig::FullRangeAnalysisConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FullRangeAnalysisConfig)
{
    ui->setupUi(this);
}

FullRangeAnalysisConfig::~FullRangeAnalysisConfig()
{
    delete ui;
}
