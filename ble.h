#ifndef BLE_H
#define BLE_H
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QSettings>
#include <deviceinfo.h>
#include <QObject>
#include <qregularexpression.h>

#define UARTSERVICENORDIC "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define RXUUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define TXUUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

class Ble : public QObject
{
    Q_OBJECT


public:
    enum bluetoothleState {
        Idle = 0,
        Scanning,
        ScanFinished
    };
    Q_ENUM(bluetoothleState)

    Ble();
    ~Ble();


    void deviceSearch();
    void setState(Ble::bluetoothleState newState);
    Ble::bluetoothleState getState() const;
    void getDeviceList(QList<QString> &Devices);


private:
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;
    QLowEnergyService *m_service;
    QLowEnergyController *m_controler;

    QList<QObject *> m_listDevices;
    QList<QString> m_foundDevices;
     QLowEnergyDescriptor m_notificationDescTx;

    Ble::bluetoothleState m_state;
    DeviceInfo m_device;
    QString m_status;
    bool m_foundUART;


private slots:
    void addDevice(const QBluetoothDeviceInfo &info);
    void scanFinished();
    void startConnect(int index);
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error);
    void serviceDiscovered(const QBluetoothUuid &svc);
    void serviceScanDone();
    void controllerError(QLowEnergyController::Error e);
    void deviceConnected();
    void deviceDisconnected();
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void updateData(const QLowEnergyCharacteristic &c,const QByteArray &value);
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &d,const QByteArray &value);
signals:
    void changedState(Ble::bluetoothleState newState);
    void error(QString message);
    void newData(int s);




};

#endif // BLE_H
