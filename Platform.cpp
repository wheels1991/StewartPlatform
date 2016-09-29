#include "Platform.h"
#include <QMatrix4x4>
#include <QDebug>
#include <qmath.h>

Platform::Platform(Type type)
{
    stewartType = type;
    /* 机械参数初始化，根据不同平台进行修正 */
    if (stewartType == SteerMotor) {                                                   //舵机结构
        topRadius = 75;
        topInterval = 140;
        bottomRadius = 110;
        bottomInterval = 80.48;
        lengthOfSteerWheel = 16.5;
        lengthOfCardan = 0;
        lengthOfBar = 200;
        topPlatform = QVector<QVector3D>(6);
        bottomPlatform = QVector<QVector3D>(6);
        endOfSteelWheel = QVector<QVector3D>(6);
        cardanCenter = QVector<QVector3D>(6);
        motorBar = qSqrt(qPow(lengthOfSteerWheel, 2) + qPow(lengthOfCardan, 2));
        theta0 = qRadiansToDegrees(qAtan(lengthOfCardan / lengthOfSteerWheel));                 /* 弧度制 */
        range = QVector<QVector3D>(6);
        range[0] = QVector3D(-50, 0, 50);
        range[1] = QVector3D(-50, 0, 50);
        baseLength = 192;                                                       //平台基本高度
        range[2] = QVector3D(baseLength - 20, baseLength, baseLength + 20);
        range[3] = QVector3D(-15, 0, 15);
        range[4] = QVector3D(-15, 0, 15);
        range[5] = QVector3D(-15, 0, 15);
    } else if (stewartType == StepperMotor) {                                          //步进电机结构
        topRadius = 75;
        topInterval = 140;
        bottomRadius = 110;
        bottomInterval = 80.48;
        lengthOfSteerWheel = 16.5;                                              //参数未使用
        lengthOfCardan = 0;                                                     //参数未使用
        lengthOfBar = 200;                                                      //参数未使用
        topPlatform = QVector<QVector3D>(6);
        bottomPlatform = QVector<QVector3D>(6);
        endOfSteelWheel = QVector<QVector3D>(6);
        cardanCenter = QVector<QVector3D>(6);
        motorBar = qSqrt(qPow(lengthOfSteerWheel, 2) + qPow(lengthOfCardan, 2));//参数未使用
        theta0 = qRadiansToDegrees(qAtan(lengthOfCardan / lengthOfSteerWheel)); //参数未使用
        range = QVector<QVector3D>(6);
        range[0] = QVector3D(-50, 0, 50);
        range[1] = QVector3D(-50, 0, 50);
        baseLength = 280;                                                       //电动缸基本长度
        range[2] = QVector3D(baseLength, baseLength, baseLength + 100);
        range[3] = QVector3D(-15, 0, 15);
        range[4] = QVector3D(-15, 0, 15);
        range[5] = QVector3D(-15, 0, 15);
    }

}

Platform::~Platform()
{

}


QVector3D Platform::Inverse(QVector3D point)
{
    QMatrix4x4 Txyz;
    Txyz.translate(x, y, z);                                               /* 此处的顺序跟matlab相反 */
    QMatrix4x4 Ra;
    Ra.rotate(a, 1, 0, 0);
    QMatrix4x4 Rb;
    Rb.rotate(b, 0, 1, 0);
    QMatrix4x4 Rc;
    Rc.rotate(c, 0, 0, 1);

    QVector3D p = Txyz * Rc * Rb * Ra * point;

    return p;
}
/*!
 * \brief Platform::Error
 * 计算电机motorID在theta角时,万向节中心到动平台对应参考点的距离与杆长的偏差
 * \return
 */
double Platform::Error(double theta, int motorID)
{
    //以下采用弧度制计算
    theta += theta0;
    theta = qDegreesToRadians(theta);

    QMatrix4x4 rotation120;
    rotation120.rotate(120, 0, 0, 1);
    QMatrix4x4 rotation240;
    rotation240.rotate(240, 0, 0, 1);
    switch (motorID) {
    case 0:
        cardanCenter[motorID] = QVector3D(-bottomInterval / 2 + motorBar * qCos(theta),
                                         -bottomRadius,
                                         motorBar * qSin(theta));
        break;
    case 1:
        cardanCenter[motorID] = QVector3D(bottomInterval / 2 - motorBar * qCos(theta),
                                       -bottomRadius,
                                       motorBar * qSin(theta));
        break;
    case 2:
        cardanCenter[motorID] = QVector3D(- bottomInterval / 2 + motorBar * qCos(theta),
                                       -bottomRadius,
                                       motorBar * qSin(theta));
        cardanCenter[motorID] = rotation120 * cardanCenter.at(motorID);
        break;
    case 3:
        cardanCenter[motorID] = QVector3D(bottomInterval / 2 - motorBar * qCos(theta),
                                       -bottomRadius,
                                       motorBar * qSin(theta));
        cardanCenter[motorID] = rotation120 * cardanCenter.at(motorID);
        break;
    case 4:
        cardanCenter[motorID] = QVector3D(- bottomInterval / 2 + motorBar * qCos(theta),
                                       -bottomRadius,
                                       motorBar * qSin(theta));
        cardanCenter[motorID] = rotation240 * cardanCenter.at(motorID);
        break;
    case 5:
        cardanCenter[motorID] = QVector3D(bottomInterval / 2 - motorBar * qCos(theta),
                                       -bottomRadius,
                                       motorBar * qSin(theta));
        cardanCenter[motorID] = rotation240 * cardanCenter.at(motorID);
        break;
    default:
        break;
    }
    double distance = cardanCenter.at(motorID).distanceToPoint(topPlatform.at(motorID));
    return distance - lengthOfBar;
}

void Platform::SetPos(double xp, double yp, double zp, double ap, double bp, double cp, Type type)
{
    stewartType = type;
    x = xp;
    y = yp;
    z = zp;
    a = ap;
    b = bp;
    c = cp;
}

void Platform::SetPos(QVector<double> pos, Type type)
{
    SetPos(pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], type);
}
/*!
 * \brief Platform::GetJoints
 * \return 当前位姿下是否有解，若有解，返回true.
 *         joints:电机转角
 */
bool Platform::GetJoints(QVector<double> &joints)
{
    if (joints.size() != 6){
        return false;
    }
    //计算动平台参考点的位置
    topPlatform[0]= QVector3D(-topInterval / 2, -topRadius, 0);
    topPlatform[1] = QVector3D(topInterval / 2, -topRadius, 0);
    QMatrix4x4 rotation120;
    rotation120.rotate(120, 0, 0, 1);
    topPlatform[2] = rotation120 * topPlatform.at(0);
    topPlatform[3] = rotation120 * topPlatform.at(1);
    topPlatform[4] = rotation120 * topPlatform.at(2);
    topPlatform[5] = rotation120 * topPlatform.at(3);
    for (int index = 0; index < 6; ++index) {
        topPlatform[index] = Inverse(topPlatform.at(index));                  /* 得到去平台参考点的位置 */
    }
    //计算定平台电机轴位置
    bottomPlatform[0] = QVector3D(-bottomInterval / 2, -bottomRadius, 0);
    bottomPlatform[1] = QVector3D(bottomInterval / 2, -bottomRadius, 0);
    bottomPlatform[2] = rotation120 * bottomPlatform.at(0);
    bottomPlatform[3] = rotation120 * bottomPlatform.at(1);
    bottomPlatform[4] = rotation120 * bottomPlatform.at(2);
    bottomPlatform[5] = rotation120 * bottomPlatform.at(3);

//    QVector<double> joints(6,0);
    if (stewartType == StepperMotor) {
        for (int index = 0; index < 6; ++index) {
            joints[index] = bottomPlatform[index].distanceToPoint(topPlatform[index]) - baseLength;
            if (joints[index] > 100 || joints[index] < 0) {                     //杆长范围约束
                return false;
            }
        }
        return true;
    } else if (stewartType == SteerMotor) {
        for (int index = 0; index < 6; ++index) {
            double min = -90;
            double max = 90;
            if (fzero(&Platform::Error, min, max, index, 0.2)) {
                joints[index] = min;
    //            qDebug() << "joint" << min;
    //            qDebug() << cardanCenter[index].distanceToPoint(topPlatform[index]);
            } else {
                return false;
            }
        }
        return true;
    }
}

bool Platform::fzero(func fun, double &min, double &max, int motorID, double precision)
{
    double fmin = (this->*fun)(min, motorID);
    double fmax = (this->*fun)(max, motorID);
    double middle = (min + max) * 0.5;
    double fmiddle = (this->*fun)(middle, motorID);
    double error = max - min;
    if ((fmin > 0 && fmax > 0) || (fmin < 0 && fmax < 0)) {                     /* 无解 */
        return false;
    }
    while (error > precision) {
        if ((fmin < 0 && fmiddle > 0) || (fmin > 0 && fmiddle < 0)) {           /* 解在(min, middle)之间 */
            max = middle;
            fmax = (this->*fun)(max, motorID);
        } else {
            min = middle;
            fmin = (this->*fun)(min, motorID);
        }
        middle = (min + max) * 0.5;
        fmiddle = (this->*fun)(middle, motorID);
        error = max - min;
    }
    min = middle;
    return true;
}

