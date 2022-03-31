#include "imageconfig.h"
#include "ui_imageconfig.h"
#include "tools.hpp"

ImageConfig::ImageConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageConfig)
{
    ui->setupUi(this);
    setFixedSize(size());
}

ImageConfig::~ImageConfig()
{
    delete ui;
}

void ImageConfig::on_buttonBox_accepted()
{
    tools::getMainWindow()->analyzer.pixelSize_nm = ui->pixelSizeInput->value();
}

