#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QDialog>
#include <QtNetwork/qudpsocket.h>
#include <QtSerialPort/QSerialPortInfo>




namespace Ui {
class SerialPort;
}

class SerialPort : public QDialog
{
    Q_OBJECT

public:
    explicit SerialPort(QWidget *parent = 0);
    ~SerialPort();

    static QByteArray hexStringtoByteArray(QString hex); //将格式化后的16进制字符串转化为报文
    static void formatString(QString &org, int n=2, const QChar &ch=QChar(' ')); //将16进制字符串重新格式化
//    static QString a = QString("55aa000014010001ffffffff00000404");
//    static QString tail = QString("12345678abcd");

signals:
    void serialPortIsOpen(bool IsOpen);

public slots:
    void sendData();                                                            /* 测试程序 */
    void sendData(const char *data);

    void sendMessage(QString data);

    void updateReceive();
    void updateSerialPortList();


private slots:
    void on_pushButtonOpenSerialPot_clicked();
    void on_pushButtonCloseSerialPort_clicked();


private:
    Ui::SerialPort *ui;
    QSerialPort *my_serialport;
    QTimer *timerForPort;

    QUdpSocket *commu;


};

#endif // SERIALPORT_H
