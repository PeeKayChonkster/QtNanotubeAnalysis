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
    setFixedSize(size());
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
     Tools::getMainWindow()->setPixelSize(ui->pixelSizeInput->value());
     Tools::getMainWindow()->setExtremumDeltaStep(ui->deltaStepInput->value());
     Tools::getMainWindow()->setExtremumOverflowTolerance(ui->overflowToleranceInput->value());
     Tools::getMainWindow()->setProcessFullRange(ui->processFullRangeCheckbox->isChecked());
     Tools::getMainWindow()->setMinPixelInTube(ui->minPixelsInput->value());
     Tools::getMainWindow()->startAutoAnalysis();
}

void AutoAnalysisConfig::showEvent(QShowEvent *event)
{
    ui->pixelSizeInput->setValue(Tools::getMainWindow()->getPixelSize());
}
