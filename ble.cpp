
#include "ble.h"
#include <QDebug>
#include <QtEndian>


Ble::Ble() : m_state(Ble::bluetoothleState::Idle)

{

m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

connect(m_discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
        this, SLOT(addDevice(const QBluetoothDeviceInfo&)));
connect(m_discoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
        this, SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));

}
Ble::~Ble()

{
}

void Ble::deviceSearch(){

    qDeleteAll(m_listDevices);
    m_listDevices.clear();

    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    setState(Scanning);
    qDebug() <<"searching devices"<<endl;
}

void Ble::addDevice(const QBluetoothDeviceInfo &info)
{
    if(info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        DeviceInfo *dev = new DeviceInfo(info);
        m_listDevices.append(dev);
        qDebug() << "BLE device found"<<endl;
    }
}
void Ble::scanFinished()
{
    if (m_listDevices.size()==0){
        qDebug() << "NO devices"<<endl;
    }
    else
    {
       for (int i = 0; i < m_listDevices.size(); i++) {
           m_foundDevices.append(((DeviceInfo*) m_listDevices.at(i))->getName());
        }
    }
    setState(ScanFinished);
}

void Ble::setState(Ble::bluetoothleState newState)
{
    if (m_state == newState)
        return;

    m_state = newState;
    emit changedState(newState);
}

Ble::bluetoothleState Ble::getState() const {
    return m_state;
}
void Ble::getDeviceList(QList<QString> &qlDevices){

    if(m_state == bluetoothleState::ScanFinished)
    {
        qlDevices = m_foundDevices;
    }
    else
    {
        qlDevices.clear();
    }

}
void Ble::startConnect(int i){


    m_device.setDevice(((DeviceInfo*)m_listDevices.at(i))->getDevice());

    if (m_controler) {
        m_controler->disconnectFromDevice();
        delete m_controler;
        m_controler = 0;

    }

    /* 2 Step: QLowEnergyController */
    m_controler = new QLowEnergyController(m_device.getDevice(), this);
    m_controler ->setRemoteAddressType(QLowEnergyController::RandomAddress);

    connect(m_controler, SIGNAL(serviceDiscovered(QBluetoothUuid)),
            this, SLOT(serviceDiscovered(QBluetoothUuid)));
    connect(m_controler, SIGNAL(discoveryFinished()),
            this, SLOT(serviceScanDone()));
    connect(m_controler, SIGNAL(error(QLowEnergyController::Error)),
            this, SLOT(controllerError(QLowEnergyController::Error)));
    connect(m_controler, SIGNAL(connected()),
            this, SLOT(deviceConnected()));
    connect(m_controler, SIGNAL(disconnected()),
            this, SLOT(deviceDisconnected()));

    /* Start connecting to device */
    m_controler->connectToDevice();
    //setState(Connecting);

}



void Ble::serviceDiscovered(const QBluetoothUuid &gatt){

    if(gatt==QBluetoothUuid(QUuid(UARTSERVICENORDIC))){
        m_foundUART =true;
        qDebug() << "UART service found!";
    }
}



void Ble::serviceScanDone(){

    delete m_service;
    m_service=0;

    if(m_foundUART){
       qDebug() << "Connecting to UART service...";
        m_service = m_controler->createServiceObject(QBluetoothUuid(QUuid(UARTSERVICENORDIC)),this);
    }

    if(!m_service){
        qDebug() <<"UART service not found";
        return;
    }
    connect(m_service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
            this, SLOT(serviceStateChanged(QLowEnergyService::ServiceState)));
    connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
            this, SLOT(updateData(QLowEnergyCharacteristic,QByteArray)));
    connect(m_service, SIGNAL(descriptorWritten(QLowEnergyDescriptor,QByteArray)),
            this, SLOT(confirmedDescriptorWrite(QLowEnergyDescriptor,QByteArray)));

    m_service->discoverDetails();
}

void Ble::deviceDisconnected()
{
    qDebug() << "UART service disconnected";
    qWarning() << "Remote device disconnected";
}

void Ble::deviceConnected()
{
   qDebug() << "Device connected";
    m_controler->discoverServices();
    //setState(Connected);
}

void Ble::controllerError(QLowEnergyController::Error error)
{
    qDebug() << "Cannot connect to remote device.";
    qWarning() << "Controller Error:" << error;
}
void Ble::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        qDebug() << "The Bluetooth adaptor is powered off, power it on before doing discovery.";
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        qDebug() << "Writing or reading from the device resulted in an error.";
    else
        qDebug() << "An unknown error has occurred.";
}


void Ble::serviceStateChanged(QLowEnergyService::ServiceState s)
{

    // A descriptoc can only be written if the service is in the ServiceDiscovered state
    switch (s) {
    case QLowEnergyService::ServiceDiscovered:
    {

        //looking for the TX characteristic
        const QLowEnergyCharacteristic TxChar = m_service->characteristic(QBluetoothUuid(QUuid(TXUUID)));
        if (!TxChar.isValid()){
            qDebug() << "Tx characteristic not found";
            break;
        }

         //looking for the RX characteristic
        const QLowEnergyCharacteristic  RxChar = m_service->characteristic(QBluetoothUuid(QUuid(RXUUID)));
        if (!RxChar.isValid()) {
           qDebug() << "Rx characteristic not found";
            break;
        }


       // Bluetooth LE spec Where a characteristic can be notified, a Client Characteristic Configuration descriptor
       // shall be included in that characteristic as required by the Bluetooth Core Specification
       // Tx notify is enabled
       const QLowEnergyDescriptor m_notificationDescTx = TxChar.descriptor(
                   QBluetoothUuid::ClientCharacteristicConfiguration);
        if (m_notificationDescTx.isValid()) {
            // enable notification
            m_service->writeDescriptor(m_notificationDescTx, QByteArray::fromHex("0100"));

            }

        break;
    }
    default:
        //nothing for now
        break;
    }
}

void Ble::updateData(const QLowEnergyCharacteristic &c,const QByteArray &value)
{
    if (c.uuid() != QBluetoothUuid(QUuid(TXUUID)))
           return;
    
    QDataStream in(value);
    in.setByteOrder(QDataStream::LittleEndian);
    qint16 result;
    in >> result;
    qDebug() <<result<< endl;
    emit newData(result);
}

void Ble::confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                                         const QByteArray &value)
{
    if (d.isValid() && d == m_notificationDescTx && value == QByteArray("0000")) {
        //disabled notifications -> assume disconnect intent
        m_controler->disconnectFromDevice();
        delete m_service;
        m_service = 0;
    }
}

