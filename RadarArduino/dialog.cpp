#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , r(445.0) //needle radius (pixels)
    , angleOffset(0.05) //needle angle offset (rad)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //load bg image
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    pix = QPixmap(":/src/img/radar.png");
    scene->addPixmap(pix);

    //set up q serial port
    arduino = new QSerialPort;
    arduino_is_available = false;
    arduino_port_name = "";

    //initialize needle at 0 degrees
    QPen blackpen(Qt::black);
    QBrush graybrush(Qt::gray);
    t_up = angleOffset;
    t_lo = -angleOffset;
    triangle.append(QPointF(r*qCos(t_up) + 505, -r*qSin(t_up) + 495));
    triangle.append(QPointF(505,495));
    triangle.append(QPointF(r*qCos(t_lo) + 505, -r*qSin(t_lo) + 495));
    needle = scene->addPolygon(triangle,blackpen,graybrush);
    needle->setOpacity(0.30);

    // view #of available ports on system along with their vendor and product ID
    /*qDebug() << "Number of ports: " << QSerialPortInfo::availablePorts().length();
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier()){
            qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
        }
        if(serialPortInfo.hasProductIdentifier()){
            qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
        }
    }*/

    //check which port the arduino is on
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            if (serialPortInfo.vendorIdentifier() == arduino_uno_vendorID){
                if (serialPortInfo.productIdentifier() == arduino_uno_productID){
                    arduino_port_name = serialPortInfo.portName();
                    arduino_is_available = true;
                    qDebug() << "Port available!";
                }
            }
        }
    }
    //setup port if available
    if (arduino_is_available){
        //open and configure port
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        //slot for updating value
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
        //this->updateServo("0");
    }
    else{
        //give a msg
        QMessageBox::warning(this, "Port error", "Couldn't find arduino");
    }
}

void Dialog::readSerial(){
    QPen blackpen(Qt::black);
    QBrush graybrush(Qt::gray);
    QStringList bufferSplit = serialBuffer.split(",");
    if (bufferSplit.length() < 3){
        serialData = arduino->readAll();
        serialBuffer += QString::fromStdString(serialData.toStdString());
    }
    else{
        ui->range->setText(bufferSplit[1]);
        currAngle = ui->angle->text().toFloat();
        currAngle = qDegreesToRadians(currAngle);
        float dist = bufferSplit[1].toFloat();
        if (dist < 100){
            float radius = dist*4.5;
            float x = radius*qCos(currAngle);
            float y = radius*qSin(currAngle);
            float xT = x + 505;
            float yT = -1*y + 495;
            rect = scene->addRect(xT,yT,12,12,blackpen,graybrush);
            rect->setOpacity(0.75);
        }
        serialBuffer = "";
    }
}

void Dialog::updateServo(QString command){
    //get the angle and update the needle
    currAngle = qDegreesToRadians(command.toFloat());
    t_up = currAngle + angleOffset;
    t_lo = currAngle - angleOffset;
    triangle[0] = QPointF(r*qCos(t_up) + 505, -1*r*qSin(t_up) + 495);
    triangle[2] = QPointF(r*qCos(t_lo) + 505, -1*r*qSin(t_lo) + 495);
    needle->setPolygon(triangle);
    float temp = command.toFloat()/1.50; 
    QString commandNew = QString::number(temp);
    if (arduino->isWritable()){
        qDebug() << commandNew.toStdString().c_str();
        arduino->write(commandNew.toStdString().c_str());
    }
    else{
        qDebug() << "Could not write to serial";
    }
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_button0_clicked()
{
    servoSetting = "0";
    ui->angle->setText(servoSetting);
    ui->verticalSlider->setValue(0);
    updateServo(servoSetting);
}

void Dialog::on_button45_clicked()
{
    servoSetting = "45";
    ui->verticalSlider->setValue(45);
    ui->angle->setText(servoSetting);
    updateServo("45");
}

void Dialog::on_button90_clicked()
{
    servoSetting = "90";
    ui->verticalSlider->setValue(90);
    ui->angle->setText(servoSetting);
    updateServo("90");
}

void Dialog::on_button135_clicked()
{
    servoSetting = "135";
    ui->verticalSlider->setValue(135);
    ui->angle->setText(servoSetting);
    updateServo("135");
}

void Dialog::on_button180_clicked()
{
    servoSetting = "180";
    ui->verticalSlider->setValue(180);
    ui->angle->setText(servoSetting);
    updateServo("180");
}

void Dialog::on_verticalSlider_valueChanged(int value)
{
    QString servoSetting_str = QString::number(value);
    ui->angle->setText(servoSetting_str);
    updateServo(servoSetting_str);
}


