#include "autoanalysisconfig.h"
#include "ui_autoanalysisconfig.h"
#include "mainwindow.h"

AutoAnalysisConfig::AutoAnalysisConfig(MainWindow* parent) :
    QDialog(parent),
    parent(parent),
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
    parent->analyzer.pixelSize_nm = ui->pixelSizeInput->value();
    parent->analyzer.extremumDeltaStep = ui->deltaStepInput->value();
    parent->analyzer.extremumOverflowTolerance = ui->overflowToleranceInput->value();
    parent->analyzer.processFullRange = ui->processFullRangeCheckbox->isChecked();
    parent->analyzer.minPixelsInTube = ui->minPixelsInput->value();
    parent->startAutoAnalysis();
}

