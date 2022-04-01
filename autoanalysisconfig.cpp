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
     tools::getMainWindow()->setPixelSize(ui->pixelSizeInput->value());
     tools::getMainWindow()->setExtremumDeltaStep(ui->deltaStepInput->value());
     tools::getMainWindow()->setExtremumOverflowTolerance(ui->overflowToleranceInput->value());
     tools::getMainWindow()->setProcessFullRange(ui->processFullRangeCheckbox->isChecked());
     tools::getMainWindow()->setMinPixelInTube(ui->minPixelsInput->value());
     tools::getMainWindow()->startAutoAnalysis();
}

void AutoAnalysisConfig::showEvent(QShowEvent *event)
{
    ui->pixelSizeInput->setValue(tools::getMainWindow()->getPixelSize());
}
