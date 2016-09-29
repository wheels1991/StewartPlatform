#ifndef SIXJOINTS_H
#define SIXJOINTS_H

#include <QDialog>
#include "Platform.h"
#include <QFile>


namespace Ui {
class SixJoints;
}

class SixJoints : public QDialog
{
    Q_OBJECT

public:
    explicit SixJoints(QWidget *parent, Platform::Type type);
    ~SixJoints();
    bool GetJoints(QVector<double> &joints);

signals:
    void TCPChanged(QVector<qreal> pos);
    void UpdateJoints(QVector<double> joints);
    void SendDataBySerial(const char *data);
    void SendMessage(QString date);


public slots:
    void SetPos(double xp, double yp, double zp, double ap, double bp, double cp);
    void SetPos(QVector<double> pos);

private slots:
    void on_pushButtonReset_clicked();
    void onTCPValueChangedFromSpinBox();
    void onTCPValueChangedFromSlider();

    void on_start_clicked();
    void on_record_clicked();
    void Record();
    void Start();

private:
    Ui::SixJoints *ui;
    Platform *stewart;
    Platform::Type stewartType;

    QTimer *timerForRecode;
    QTimer *timerForStart;
    QVector<double> pose;

    const int driveRate = 10000;
    const int sampleRate = 20;                                                  //采样率，单位Hz

};

#endif // SIXJOINTS_H
