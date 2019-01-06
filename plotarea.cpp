#include "plotarea.h"
#include "ui_plotarea.h"
#include"mainwindow.h"
#include <QtDebug>
#include <QString>

qreal PlotArea::m_xValue;
qreal PlotArea::m_secValue;
qreal PlotArea::m_timeValue;

PlotArea::PlotArea(QWidget *parent) :
    QCustomPlot (parent),
    ui(new Ui::PlotArea),
    m_pauseButton(new QPushButton),
    m_acquisitionButton(new QPushButton)
{
    ui->setupUi(this);
    m_secValue = 60 * 60 * m_currentTime.hour() + 60 * m_currentTime.minute() + m_currentTime.second();
    m_timeValue = 1000 * m_secValue + m_currentTime.msec();
    m_startButton = new QPushButton();
    m_pauseButton->setText("Pause");
    m_pauseButton->setCheckable(true);
    m_myVector.clear();
    m_acquisitionButton->setText("Start Acquisition");
    m_acquisitionButton->setCheckable(true);

    myTimer  = new QTimer(this);



    connect(m_startButton, SIGNAL(clicked()), this, SLOT(addPlot()));
    connect(myTimer, SIGNAL(timeout()), this, SLOT(update()));
    connect(m_pauseButton.data(), SIGNAL(toggled(bool)), this, SLOT(pauseButton_pressed()));
    connect(m_acquisitionButton.data(), SIGNAL(toggled(bool)), this, SLOT(acqusitonButton_pressed()));


}

PlotArea::~PlotArea()
{
    delete ui;
}

void PlotArea::update()
{

    //this->readFile();
    if(m_xValue<=m_myVector.size()){



         //Add the time the x data buffer
        m_XData.append( m_xValue );

        m_YData.append(m_myVector[m_xValue]);

        if( m_XData.size() > 100 ){
            m_XData.remove( 0 );
            m_YData.remove( 0 );
        }


        graph()->setData(m_XData, m_YData);


        if(isAcquisition){
            writeFile();
        }

        resizeXAxis();
        m_xValue++;
        replot();
    }

}
void PlotArea::resizeXAxis()
{
    QVector<double>::iterator xMaxIt = std::max_element( m_XData.begin() , m_XData.end() );
    QVector<double>::iterator xMinIt = std::min_element( m_XData.begin() , m_XData.end() );

    qreal xPlotMin = *xMinIt;
    qreal xPlotMax = *xMaxIt;

    xAxis->setRange(xPlotMin ,xPlotMax);
}

void PlotArea::readFile(){
    QFile file("/Users/sandrafraczek/BioPlot/dane2.txt");
    QString text;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "WARNING", "File not open!");
    }

    QTextStream ts(&file);
    while(!ts.atEnd()){
        m_lineFromFile = ts.readLine();
        m_myVector.append(m_lineFromFile.toDouble());
    }

    file.close();
}

void PlotArea::writeFile() {


    QFile file("/Users/sandrafraczek/BioPlot/data.txt");
    if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {

        QTextStream stream(&file);
        stream << "Y_value: " << m_YData.back() << "[mV] X_value: " << m_XData.back() << "[ms]" <<endl;
        file.flush();
        qDebug() << "Writing finished";
    file.close();
}
}
void PlotArea::pauseButton_pressed()

{

    if (m_pauseButton->isChecked()){
        myTimer->stop();
        m_pauseButton->setText("Start");
    }
    else
    {
        myTimer->start(50);
        m_pauseButton->setText("Pause");
    }

}

void PlotArea::addPlot(){
    
    addGraph();
    myTimer->start(50);
    
    this->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    this->xAxis->setDateTimeFormat("mm:ss:ms");

    xAxis->setLabel("Time");
    yAxis->setLabel("SAADC result");
    yAxis->setRange(0, 1000);

    graph()->rescaleAxes();
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

}

void  PlotArea::acqusitonButton_pressed()
{
    if (m_acquisitionButton->isChecked()){
        isAcquisition=true;
        m_acquisitionButton->setText("Stop Acqustion");
    }
    else
    {
        isAcquisition=false;
        m_acquisitionButton->setText("Start Acqusiton");
    }
}
