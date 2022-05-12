#include "console.h"
#include "ui_console.h"
#include "tools.hpp"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

Console::Console(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Console)
{
    ui->setupUi(this);
    setWindowTitle("Console");
    setMinimumSize(size());
}

Console::~Console()
{
    delete ui;
}

void Console::print(QString line, QColor textColor)
{
    if(!isVisible()) show();
    ui->textEdit->setTextColor(textColor);
    ui->textEdit->append(line);
    ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
    update();
}

void Console::showEvent(QShowEvent *event)
{
    if(!event->spontaneous())
    {
        static const int xOffset = 5;
        QMainWindow* mainWindow = Tools::getMainWindow();
        QPoint mainWindowPos = mainWindow->pos();
        resize(width(), mainWindow->height());
        move(mainWindowPos.x() - width() - xOffset, mainWindowPos.y());
    }
}

void Console::on_actionClear_triggered()
{
    ui->textEdit->clear();
}


void Console::on_actionSave_to_file_triggered()
{
    QTextDocument* textDoc = ui->textEdit->document();
    QString text = textDoc->toPlainText();
    QString path = QFileDialog::getSaveFileName(this, "Save to file", ".", "Text file (*.txt)");
    if(!path.isEmpty())
    {
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, "Unable to open file", file.errorString());
            return;
        }
        else
        {
            QDataStream out(&file);
            out << text;
        }
    }
}

