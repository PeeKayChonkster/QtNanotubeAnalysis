#include "autoanalysisconfig.h"
#include "ui_autoanalysisconfig.h"
#include "mainwindow.h"
#include "tools.hpp"

AutoAnalysisConfig::AutoAnalysisConfig(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AutoAnalysisConfig)
{
    ui->setupUi(this);
    setLocale(QLocale::C);
}

AutoAnalysisConfig::~AutoAnalysisConfig()
{
    delete ui;
}

void AutoAnalysisConfig::on_processFullRangeCheckbox_stateChanged(int arg1)
{
    ui->overflowToleranceInput->setEnabled(!(bool)arg1);
}


void AutoAnalysisConfig::on_buttonBox_accepted()
{
     tools::getMainWindow()->analyzer.pixelSize_nm = ui->pixelSizeInput->value();
     tools::getMainWindow()->analyzer.extremumDeltaStep = ui->deltaStepInput->value();
     tools::getMainWindow()->analyzer.extremumOverflowTolerance = ui->overflowToleranceInput->value();
     tools::getMainWindow()->analyzer.processFullRange = ui->processFullRangeCheckbox->isChecked();
     tools::getMainWindow()->analyzer.minPixelsInTube = ui->minPixelsInput->value();
     tools::getMainWindow()->startAutoAnalysis();
}

