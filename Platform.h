#ifndef PLATFORM_H
#define PLATFORM_H
#include <QVector>
#include <QVector3D>

class Platform;
typedef double (Platform::*func)(double, int);

class Platform
{
public:
    enum Type{
        SteerMotor,
        StepperMotor
    };

    explicit Platform(Type type);
    ~Platform();
    void SetPos(double xp, double yp, double zp, double ap, double bp, double cp, Type type);
    void SetPos(QVector<double> pos, Type type);
    bool GetJoints(QVector<double> &joints);

private:
    bool fzero(func fun, double &min, double &max, int motorID, double precision);
    QVector3D Inverse(QVector3D point);
    double Error(double theta, int motorID);

public:
    QVector<QVector3D> range;                                                           //(min, origine, max)
private:
    /* pose of stewart platform */
    double x;
    double y;
    double z;
    double a;
    double b;
    double c;

    Type stewartType;


    /* mechanical parameters of stewart platform */
    double topRadius;                                                           /* 动平台参考点六边形内切圆半径 */
    double topInterval;                                                         /* 动平台相邻参考点间隔 */
    double bottomRadius;                                                        /* 定平台参考点六边形内切圆半径 */
    double bottomInterval;                                                      /* 定平台相邻参考点间隔 */
    double lengthOfSteerWheel;                                                  /* 电机连杆长度 */
    double lengthOfCardan;                                                      /* 万向节中心到舵盘径线的距离 */
    double lengthOfBar;                                                         /* 连杆长度 */
    double motorBar;
    double theta0;
    double baseLength;
    /* 主要参考点 */
    QVector<QVector3D> topPlatform;                                             /* 上平台参考点 */
    QVector<QVector3D> bottomPlatform;                                          /* 下平台参考点，即电机轴心 */
    QVector<QVector3D> endOfSteelWheel;                                         /* 舵盘半径 */
    QVector<QVector3D> cardanCenter;                                            /* 万向节中心到舵盘径线的距离 */

};

#endif // PLATFORM_H
