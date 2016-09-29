#include "SixJoints.h"
#include "ui_SixJoints.h"
#include "QDoubleSpinBox"
#include "QSlider"
#include <QTimer>
#include <QTextStream>
#include <QDebug>
#include "SerialPort.h"

const char indexPropertyName[] = "index";

SixJoints::SixJoints(QWidget *parent, Platform::Type type) :
    QDialog(parent),
    ui(new Ui::SixJoints)
{
    ui->setupUi(this);
    stewartType = type;
    if (type == Platform::SteerMotor) {
        qDebug() << "This is steer structure";
    } else if (type == Platform::StepperMotor) {
        qDebug() << "This is stepper structure";
    }

    stewart = new Platform(type);
    //轴控制模式下为每一个slider设置量程及零点
    //TCP模式下，为每一个slider设置量程及零点
    for (int index = 1; index <= 6; index++) {
        QSlider *slider = findChild<QSlider *>(QString("verticalSliderTCP%1").arg(index));
        QDoubleSpinBox *doubleSpinBox = findChild<QDoubleSpinBox *>(QString("doubleSpinBoxTCP%1").arg(index));

        slider->setMaximum(stewart->range[index - 1][2]);
        slider->setMinimum(stewart->range[index - 1][0]);
        slider->setValue(stewart->range[index - 1][1]);

        doubleSpinBox->setMaximum(stewart->range[index - 1][2]);
        doubleSpinBox->setMinimum(stewart->range[index - 1][0]);
        doubleSpinBox->setValue(stewart->range[index - 1][1]);

        connect(doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onTCPValueChangedFromSpinBox()));
        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onTCPValueChangedFromSlider()));
        doubleSpinBox->setProperty(indexPropertyName, index);
        slider->setProperty(indexPropertyName, index);
    }
    //设置TCP模式不可用
    ui->groupBoxTCP->setEnabled(1);

    pose = QVector<double>(6);
    timerForRecode = new QTimer(this);
    timerForStart = new QTimer(this);
}

SixJoints::~SixJoints()
{
    delete ui;
}

bool SixJoints::GetJoints(QVector<double> &joints)
{
    if (stewart->GetJoints(joints)) {
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief SixJoints::onTCPValueChanged              由spinBox触发后，更改slider的值，并将TCP发射出去
 */
void SixJoints::onTCPValueChangedFromSpinBox()
{
    //spinBox的变化引起slider的变化
    QObject *doubleSpinBox = sender();
    if (!doubleSpinBox) {
        return;
    }
    bool ok = false;
    int index = doubleSpinBox->property(indexPropertyName).toInt(&ok);
    if (!ok) {
        return;
    }
    QDoubleSpinBox *temp = findChild<QDoubleSpinBox *>(QString("doubleSpinBoxTCP%1").arg(index));
    QSlider *slider = findChild<QSlider *>(QString("verticalSliderTCP%1").arg(index));
    slider->setValue((int)(temp->value()));

    //发射TCP变化的信号给串口
    QVector<qreal> TCP(6,0);
    for (int id = 1; id <= 6; id++) {
        QDoubleSpinBox *doubleSpinBox = findChild<QDoubleSpinBox *>(QString("doubleSpinBoxTCP%1").arg(id));
        TCP[id - 1] = doubleSpinBox->value();
    }
    emit TCPChanged(TCP);

}
/*!
 * \brief SixJoints::onTCPValueChanged1             由slider触发后，更改spinbox的值
 */
void SixJoints::onTCPValueChangedFromSlider()
{
    //slider的变化引起spinbox的变化
    QObject *sliderSender = sender();
    if (!sliderSender) {
        return;
    }
    bool ok = false;
    int index = sliderSender->property(indexPropertyName).toInt(&ok);
    if (!ok) {
        return;
    }
    QSlider *slider = findChild<QSlider *>(QString("verticalSliderTCP%1").arg(index));
    QDoubleSpinBox *doubleSpinBox = findChild<QDoubleSpinBox *>(QString("doubleSpinBoxTCP%1").arg(index));
    doubleSpinBox->setValue((qreal)(slider->value()));

    QVector<double> pos(6);
    for (int id = 1; id <=6; ++id) {
        QDoubleSpinBox *spinBox = findChild<QDoubleSpinBox *>(QString("doubleSpinBoxTCP%1").arg(id));
        pos[id - 1] = spinBox->value();
    }
    SetPos(pos);
}
/*!
 * \brief SixJoints::on_pushButtonReset_clicked     响应“复位”按键
 */
void SixJoints::on_pushButtonReset_clicked()
{
    for (int index = 1; index <= 6; index++) {
        QDoubleSpinBox *doubleSpinBox = findChild<QDoubleSpinBox *>(QString("doubleSpinBoxTCP%1").arg(index));
        doubleSpinBox->setValue(stewart->range[index - 1][1]);
    }
}

void SixJoints::SetPos(double xp, double yp, double zp, double ap, double bp, double cp)
{
    stewart->SetPos(xp, yp, zp, ap, bp, cp,Platform::StepperMotor);
    QVector<double> pos(6);
    pos << xp << yp << zp << ap << bp << cp;
    SetPos(pos);
}

void SixJoints::SetPos(QVector<double> pos)
{
    pose = pos;
    if (stewartType == Platform::SteerMotor) {
        stewart->SetPos(pos, Platform::SteerMotor);
    } else if (stewartType == Platform::StepperMotor) {
        stewart->SetPos(pos, Platform::StepperMotor);
    }
    QVector<double> joints(6);

    if (stewart->GetJoints(joints)) {
        emit UpdateJoints(joints);
        char sum = 0;
        for (int index = 1; index <= 6; ++index) {
            QLabel *label = findChild<QLabel *>(QString("joint%1").arg(index));
            label->setText( QString("%1").arg(joints.at(index - 1)));
        }
        if (stewartType == Platform::SteerMotor) {
            joints[0] = 90 + joints[0];
            joints[1] = 90 - joints[1];
            joints[2] = 90 + joints[2];
            joints[3] = 90 - joints[3];
            joints[4] = 90 + joints[4];
            joints[5] = 90 - joints[5];
            for (int index = 1; index <= 6; ++index) {
                sum += char(joints[index - 1]);
            }
        } else if (stewartType == Platform::StepperMotor) {
            for (int index = 1; index <= 6; ++index) {
                sum += char(joints[index - 1]);
            }
        }
        char serialData[] = {0xFF, 0XFE,
                             char(joints[0]), char(joints[1]),
                             char(joints[2]), char(joints[3]),
                             char(joints[4]), char(joints[5]),
                             char(sum)};
        emit SendDataBySerial(serialData);
        QString data = QString("55aa000014010001ffffffff00000404");
        data += QString::number(50).sprintf("%08x",50);                         //时间码
        data += QString::number(int(joints[0] * driveRate)).sprintf("%08x",int(joints[0] * driveRate));           //1到6号电机的长度码
        data += QString::number(int(joints[1] * driveRate)).sprintf("%08x",int(joints[1] * driveRate));
        data += QString::number(int(joints[2] * driveRate)).sprintf("%08x",int(joints[2] * driveRate));
        data += QString::number(int(joints[5] * driveRate)).sprintf("%08x",int(joints[3] * driveRate));
        data += QString::number(int(joints[4] * driveRate)).sprintf("%08x",int(joints[4] * driveRate));
        data += QString::number(int(joints[3] * driveRate)).sprintf("%08x",int(joints[5] * driveRate));
        data += QString("12345678abcd"); //固定格式的尾码
        emit SendMessage(data);
    }
}


void SixJoints::on_start_clicked()
{
    static bool isStartMove = true;
    if (isStartMove) {
        ui->start->setText(tr("停止播放"));
        isStartMove = false;
        timerForStart->start(1000 / sampleRate);                                              //! 播放一个姿态的时间
        connect(timerForStart, SIGNAL(timeout()), this, SLOT(Start()));
        ui->record->setEnabled(false);
        ui->pushButtonReset->setEnabled(false);
        ui->groupBoxTCP->setEnabled(false);
    } else {
        ui->start->setText(tr("循环播放"));
        isStartMove = true;
        timerForStart->stop();
        ui->record->setEnabled(true);
        ui->pushButtonReset->setEnabled(true);
        ui->groupBoxTCP->setEnabled(true);
    }
}

void SixJoints::on_record_clicked()
{
    static bool isStartRecode = true;
    if (isStartRecode) {
        ui->record->setText(tr("停止记录"));
        isStartRecode = false;
        timerForRecode->start(1000 / sampleRate);                                             //! 录制一个姿态的时间
        connect(timerForRecode, SIGNAL(timeout()), this, SLOT(Record()));
        ui->start->setEnabled(false);
    } else {
        ui->record->setText(tr("开始记录"));
        isStartRecode = true;
        timerForRecode->stop();
        ui->start->setEnabled(true);
    }
}
//#include <QDialog>
void SixJoints::Record()
{

    QFile fileRecordByPose(QString("RecordByPose.csv"));
    QFile fileRecordByJoints(QString("RecordByJoints.csv"));

    if (!fileRecordByPose.open(QIODevice::Append)) {
        return;
    }
    if (!fileRecordByJoints.open(QIODevice::Append)) {
        return;
    }
    QTextStream csvByPose(&fileRecordByPose);
    QTextStream csvByJoints(&fileRecordByJoints);
    csvByPose << pose[0] << ',' << pose[1] << ',' << pose[2] << ','
              << pose[3] << ',' << pose[4] << ',' << pose[5] << '\n';
    if (stewartType == Platform::SteerMotor) {
        for (int index = 1; index <= 6; ++index) {
            QLabel *label = findChild<QLabel *>(QString("joint%1").arg(index));
            switch (index) {
            case 1:
                csvByJoints << 90 + int(label->text().toDouble()) << ',';
                break;
            case 2:
                csvByJoints << 90 - int(label->text().toDouble()) << ',';
                break;

            case 3:
                csvByJoints << 90 + int(label->text().toDouble()) << ',';
                break;
            case 4:
                csvByJoints << 90 - int(label->text().toDouble()) << ',';
                break;
            case 5:
                csvByJoints << 90 + int(label->text().toDouble()) << ',';
                break;
            case 6:
                csvByJoints << 90 - int(label->text().toDouble()) << '\n';
                break;
            }
        }
    } else if (stewartType == Platform::StepperMotor) {
        for (int index = 1; index <= 6; ++index) {
            QLabel *label = findChild<QLabel *>(QString("joint%1").arg(index));
            if (index == 6) {
                csvByJoints << int(label->text().toDouble()) << '\n';
            } else {
                csvByJoints << int(label->text().toDouble()) << ',';
            }
        }
    }

}

void SixJoints::Start()
{
    QFile fileRecode(QString("RecordByPose.csv"));
    if (!fileRecode.isOpen() && !fileRecode.open(QIODevice::ReadOnly)) {
        return;
    }
    static QTextStream csv(&fileRecode);
    QString line;
    csv >> line;
    if (line.isEmpty()) {
        return;
    }
    QStringList strList = line.split(",");
    for(int index = 0; index < 6; index++) {
        pose[index] = strList[index].toDouble();
    }
    for (int index = 1; index <= 6; index++) {
        QSlider *slider = findChild<QSlider *>(QString("verticalSliderTCP%1").arg(index));
        slider->setValue(pose[index - 1]);
    }

}

