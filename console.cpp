#include "console.h"
#include "ui_console.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

Console::Console(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Console)
{
    ui->setupUi(this);
    setWindowTitle("Console");
}

Console::~Console()
{
    delete ui;
}

void Console::print(QString line, QColor textColor)
{
    ui->textBrowser->setTextColor(textColor);
    ui->textBrowser->append(line);
}

void Console::on_actionClear_triggered()
{
    ui->textBrowser->clear();
}


void Console::on_actionSave_to_file_triggered()
{
    QTextDocument* textDoc = ui->textBrowser->document();
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

