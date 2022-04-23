#include "fullrangeanalysisconfig.h"
#include "ui_fullrangeanalysisconfig.h"
#include "tools.hpp"

FullRangeAnalysisConfig::FullRangeAnalysisConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FullRangeAnalysisConfig)
{
    ui->setupUi(this);
    setWindowTitle("Full range analysis config");
    setLocale(QLocale::C);
    setFixedSize(size());
}

FullRangeAnalysisConfig::~FullRangeAnalysisConfig()
{
    delete ui;
}

void FullRangeAnalysisConfig::on_buttonBox_accepted()
{
    Tools::getMainWindow()->setPixelSize(ui->pixelSizeInput->value());
    Tools::getMainWindow()->setMinPixelInElement(ui->minPixelsInput->value());
    Tools::getMainWindow()->startFullRangeAnalysis(ui->createTableCheckbox->isChecked());
}

