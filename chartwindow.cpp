#include "chartwindow.h"
#include "ui_chartwindow.h"
#include "tools.hpp"
#include <limits>

ChartWindow::ChartWindow(QWidget *parent) :
    QDialog(parent),
    chart(new QChart()),
    chartView(new QChartView(chart, this)),
    ui(new Ui::ChartWindow)
{
    ui->setupUi(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    auto layout = new QVBoxLayout();
    layout->addWidget(chartView);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
    setMinimumSize(size());
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setBackgroundVisible(true);
    chart->legend()->attachToChart();
}

ChartWindow::~ChartWindow()
{
    delete ui;
}

void ChartWindow::addSeries(const std::vector<std::pair<float, float>>& series, QString legendName, QString xName, QString yName, QColor color)
{
    QLineSeries *lineSeries = new QLineSeries();
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    for(const auto& point : series)
    {
        *lineSeries << QPointF(point.first, point.second);
        if(point.first < minX) minX = point.first;
        if(point.first > maxX) maxX = point.first;
    }
    lineSeries->setColor(color);
    lineSeries->setName(legendName);
    chart->addSeries(lineSeries);
    chart->createDefaultAxes();
    ((QValueAxis*)chart->axes(Qt::Horizontal)[0])->setRange(minX, maxX);
    ((QValueAxis*)chart->axes(Qt::Horizontal)[0])->setTickCount(11);
    ((QValueAxis*)chart->axes(Qt::Horizontal)[0])->setTitleText(xName);
    ((QValueAxis*)chart->axes(Qt::Vertical)[0])->setTitleText(yName);
}

void ChartWindow::clear()
{
    chart->removeAllSeries();
}
