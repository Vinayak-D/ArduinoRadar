#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QtWidgets>
#include <QtGui>
#include <QtMath>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();


private slots:
    void updateServo(QString command);
    void readSerial();
    void on_button0_clicked();
    void on_button90_clicked();
    void on_button180_clicked();
    void on_verticalSlider_valueChanged(int value);
    void on_button45_clicked();
    void on_button135_clicked();

private:
    QGraphicsScene *scene;
    QPixmap pix;
    QGraphicsItem *rect;
    float currAngle;
    const float r;
    const float angleOffset;
    float t_up;
    float t_lo;
    QPolygonF triangle;
    QGraphicsPolygonItem* needle;
    QSerialPort *arduino;
    Ui::Dialog *ui;
    static const quint16 arduino_uno_vendorID = 9025;
    static const quint16 arduino_uno_productID = 67;
    QString arduino_port_name;
    bool arduino_is_available;
    QByteArray serialData;
    QString serialBuffer;
    QString servoSetting;
};
#endif // DIALOG_H
