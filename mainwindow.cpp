#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <qcustomplot.h>
#include <qsharedpointer.h>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_layout(new QGridLayout),
    m_realPlot(new PlotArea),
    m_exitButton(new QPushButton()),
    m_saveButton(new QPushButton()),
    m_connectButton(new QPushButton()),
    m_bleButton(new QPushButton()),
    m_comboboxOfDevices(new QComboBox),
    m_status(new QLabel),
    blueLE(new Ble),
    m_window(new QWidget)


{


    ui->setupUi(this);

    m_isBleButton=false;
    m_comboboxOfDevices->clear();
    m_comboboxOfDevices->setEnabled(false);
    m_status.data()->setAlignment(Qt::AlignCenter);
    m_bleButton->setText("Search Devices");
    m_status->setText("Please search devices");
 m_status.data()->setAlignment(Qt::AlignCenter);
    m_status->setMaximumHeight(40);
    setCentralWidget(m_window);
    m_exitButton->setText("Exit");
    m_realPlot->m_startButton->setText("Start");
    m_saveButton->setText("SaveToJPG");
    m_connectButton->setText("Connect");
    m_connectButton->setEnabled(false);
    m_realPlot.data()->m_startButton->setEnabled(false);
    m_saveButton->setFixedSize(200,80);
    m_realPlot->m_pauseButton->setFixedSize(200,80);
    m_realPlot->m_acquisitionButton->setFixedSize(200,80);



    //Signals and slots
    connect(m_exitButton.data(), SIGNAL(clicked()),  this, SLOT(close()));
    connect(m_saveButton.data(), SIGNAL(clicked()), this, SLOT(savePlot()));
    connect(m_realPlot.data()->m_startButton, SIGNAL(clicked()), this, SLOT(startPloting()));
    connect(m_bleButton.data(), SIGNAL(clicked()), this, SLOT(clickedBlebutton()));
    connect(blueLE.data(), SIGNAL(changedState(Ble::bluetoothleState)),this,SLOT(changedState(Ble::bluetoothleState)));
    connect(m_connectButton.data(), SIGNAL(clicked()), this, SLOT(clickedConnectbutton()));
    connect(blueLE.data(), SIGNAL(newData(int)), this, SLOT(DataHandler(int)));


    m_layout->addWidget(m_exitButton.data(), 6, 2,1,2);
    m_layout->addWidget(m_realPlot.data()->m_startButton, 4, 2,1,2);
    m_layout->addWidget(m_bleButton.data(), 2, 2,1,1);
    m_layout->addWidget(m_comboboxOfDevices.data(), 3,2,1,2);
    m_layout->addWidget(m_status.data(), 1,2,1,2);
    m_layout->addWidget(m_connectButton.data(), 2,3);





    setWindowTitle("RealPlot - ECG");

    m_window->setLayout(m_layout.data());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::savePlot()
{
    m_realPlot->saveJpg("/Users/sandrafraczek/BioPlot/plot.jpg",800,300);
}
void MainWindow::clickedBlebutton(){

    blueLE->deviceSearch();
    m_comboboxOfDevices->setEnabled(true);
}

void MainWindow::clickedConnectbutton(){
    emit changedDevice(m_comboboxOfDevices->currentIndex());
}

void MainWindow::startPloting()
{
    m_layout->addWidget(m_realPlot.data(), 1,1,3,3) ;
    setMinimumSize(700,350);
    m_realPlot->m_startButton->close();
    m_comboboxOfDevices->close();
    m_status->close();
    m_connectButton->close();
    m_bleButton->close();
    m_layout->addWidget(m_saveButton.data(), 5, 1,1,1);
    m_layout->addWidget(m_realPlot->m_pauseButton.data(), 5,2,1,1);
    m_layout->addWidget(m_realPlot->m_acquisitionButton.data(), 5,3,1,1);
}

void MainWindow::changedState(Ble::bluetoothleState state){

    qDebug() << state;

    switch(state){

    case Ble::Scanning:
    {
        m_comboboxOfDevices->clear();



        m_status->setText("Searching...");
        break;
    }


    case Ble::ScanFinished:
    {
        blueLE->getDeviceList(m_searchDevices);

        if(!m_searchDevices.empty()){

            for (int i = 0; i < m_searchDevices.size(); i++)
            {
                m_comboboxOfDevices->addItem(m_searchDevices.at(i));
                qDebug() << m_comboboxOfDevices->itemText(i);
            }

            m_connectButton->setEnabled(true);
            connect(this, SIGNAL(changedDevice(int)), blueLE.data(),SLOT(startConnect(int)));




            m_status->setText("Please select BLE device");


        }
        else
        {
            m_status->setText("No Low Energy devices found");

        }

        break;
    }
    }
}
void MainWindow::DataHandler(int result){

    m_status->setText("Data Acquisition. Press start to show graph.");
    m_realPlot->m_startButton->setEnabled(true);
    m_realPlot->m_myVector.append(result);
}
