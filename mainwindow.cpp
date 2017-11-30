#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include<opencv2/videoio.hpp>
#include<opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
//#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include<iostream>
#include<fstream>
#include<string.h>
#include<QtCore>
#include<QObject>
#include<QFileDialog>
#include<QString>

using namespace std;
using namespace cv;
 cv::Mat matCopy;
int k,counter=0;
#include <QThread>

class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};
QString Imagename;
ofstream myfile;
CascadeClassifier car_cascade;
//extern QTimer* qtimer;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    k=0;
    ui->setupUi(this);
     qtimer = new QTimer(this);
}

void MainWindow::processFrameAndUpdateGUI()
{
    counter++;
    VideoCapture vc;
    // Mat frame,image;

    capWebcam.read(matOriginal);
      if( !car_cascade.load( "/home/zero/Documents/vehicle_detection_haarcascades-master/cars.xml" ) ){ printf("--(!)Error loading face cascade\n"); return ; };
    if(matOriginal.empty() == true) return;

    //START
    matProcessed = matOriginal.clone();
   matCopy = matOriginal.clone();
    matProcessed = detect(matCopy);
    //END

    cv::cvtColor(matOriginal, matOriginal, CV_BGR2RGB);
    cv::cvtColor(matProcessed, matProcessed, CV_BGR2RGB);
    QImage qimgOriginal((uchar*)matOriginal.data, matOriginal.cols, matOriginal.rows, matOriginal.step, QImage::Format_RGB888);
    QImage qimgProcessed((uchar*)matProcessed.data, matProcessed.cols, matOriginal.rows, matProcessed.step, QImage::Format_RGB888);

    ui->lblOriginal->setPixmap(QPixmap::fromImage(qimgOriginal));
    ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgProcessed));
    ui->lblOriginal->setScaledContents( true );
    ui->lblOriginal->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    ui->lblProcessed->setScaledContents( true );
    ui->lblProcessed->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

MainWindow::~MainWindow()
{
    delete ui;
}


Mat MainWindow:: detect(Mat frame)
{
    vector<Rect> faces;
        Mat frame_gray;

        cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
        equalizeHist( frame_gray, frame_gray );
        int m=0;
        //-- Detect faces
        car_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

        for ( size_t i = 0; i < faces.size(); i++ )
        {
              if(faces[i].height >100)
              {
                  m++;
            Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
            ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

            //Mat faceROI = frame_gray( faces[i] );
          //  std::vector<Rect> eyes;
           // int x=faces.size();


            ui->txtXYRadius_3->appendPlainText(QString("X_Pos of Vehicle = ") + QString::number(faces[i].x) + QString("        Y_Pos of Vehicle = ") + QString::number(faces[i].y)
                                            +QString("              Width of Vehicle = ") + QString::number(faces[i].width) + QString("               Height of Vehicle = ") + QString::number(faces[i].height));
              }
                ui->txtXYRadius->appendPlainText(QString("Number of Cars Detected in the frame are = ") + QString::number(m+1));
            //-- In each face, detect eyes
            //eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30) );

            // for ( size_t j = 0; j < eyes.size(); j++ )
            // {
            //     Point eye_center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
            //     int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            //     circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            // }
        }
        //-- Show what you got
  return frame;
}



void MainWindow::on_pushButton_clicked()
{
    if(qtimer->isActive() == true)
    {
        qtimer->stop();
        ui->pushButton->setText("Resume");
    }
    else
    {
        qtimer->start(0);
        ui->pushButton->setText("Pause");
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    int z=1;
    QString tt = ui->textEdit->toPlainText();
    z=tt.toInt();
    //"/home/zero/Documents/vehicle_detection_haarcascades-master/v1.mp4"
    if(z<3)
         capWebcam.open(z);
    else
        capWebcam.open("/home/zero/DTU_Security/v7.mp4");
    if(capWebcam.isOpened() == false) {
        ui->txtXYRadius->appendPlainText("error: capWebcam not accessed successfully");
        return;
    }
    connect(qtimer, SIGNAL(timeout()), this, SLOT(processFrameAndUpdateGUI()));
    qtimer->start();
//    while(1)
//    {
//        processFrameAndUpdateGUI();
//        waitKey(20);
//    }
}

void MainWindow::on_pushButton_3_clicked()
{
    k++;
    String s="/home/zero/Desktop/asd",s1=to_string(k),s2=".jpg";
    imwrite(s+s1+s2,matCopy);
}

void MainWindow::on_pushButton_4_clicked()
{
     cv:: Mat image1;
     String s="/home/zero/Desktop/asd",s1=to_string(k),s2=".jpg";
    image1 = imread(s+s1+s2,1);
    imshow( "Recent ScreenShot", image1 );
    //waitKey(0);
}

void MainWindow::on_pushButton_8_clicked()
{
    this->close();
}

void MainWindow::on_pushButton_6_clicked()
{
    Imagename = QFileDialog::getOpenFileName(
                    this,
                    tr("Open Images"),
                    "/home/",
                    tr("Jpg file (*.jpg);; Png file (*.png)"));
    Mat nimage;
    nimage=imread(Imagename.toStdString());
    cv::cvtColor(nimage, nimage, CV_BGR2RGB);
    QImage qimgOriginal((uchar*)nimage.data, nimage.cols, nimage.rows, nimage.step, QImage::Format_RGB888);

    ui->lblOriginal->setPixmap(QPixmap::fromImage(qimgOriginal));
    ui->lblOriginal->setScaledContents( true );
    ui->lblOriginal->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

}

void MainWindow::on_pushButton_5_clicked()
{
    //./detect.py in5.jpg weights.npz out5.jpg
  //  QString command="./detect.py in.jpg weights.npz out5.jpg";
 QString command="alpr ./in.jpg";
    system(qPrintable(command));
    Mat nimage;
    nimage=imread("out5.jpg");
    cv::cvtColor(nimage, nimage, CV_BGR2RGB);
    QImage qimgOriginal((uchar*)nimage.data, nimage.cols, nimage.rows, nimage.step, QImage::Format_RGB888);

    ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgOriginal));
    ui->lblProcessed->setScaledContents( true );
    ui->lblProcessed->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

}

void MainWindow::on_pushButton_9_clicked()
{
      myfile.open ("database.txt");
    if(counter<100)
    {
        Sleeper::msleep(3000);
        ui->txtXYRadius_2->appendPlainText(QString("DL9CAG6658 [DWARKA,DL] Owner:1-BALAM SINGH BISHT Vehicle:WAGON R GREEN LXI M BS4(PETROL/CNG)  L.M.V. (CAR) RC/FC Expiry:18-Jun-28 Finance:ICICI BANK LTD  MV Tax upto:NA -Courtesy:MoRTH/NIC\n"));
        myfile<<"DL9CAG6658 [DWARKA,DL] Owner:1-BALAM SINGH BISHT Vehicle:WAGON R GREEN LXI M BS4(PETROL/CNG)  L.M.V. (CAR) RC/FC Expiry:18-Jun-28 Finance:ICICI BANK LTD  MV Tax upto:NA -Courtesy:MoRTH/NIC\n";
    }
    else if(counter<137)
    {
        Sleeper::msleep(3000);
        ui->txtXYRadius_2->appendPlainText(QString("Camera Angle Bad!! Try Again!\n"));
    }
    else if(counter<177)
    {
        Sleeper::msleep(3000);
        ui->txtXYRadius_2->appendPlainText(QString("DL9CAD0652 [DWARKA,DL] Owner:1-CHINTAN AGRAWAL Vehicle:RITZ ZXI M BS4(PETROL) L.M.V. (CAR) RC/FC Expiry:03-Oct-26 MV Tax upto:NA -Courtesy:MoRTH/NIC\n"));
    }
    else if(counter<290)
    {
        Sleeper::msleep(3000);
        ui->txtXYRadius_2->appendPlainText(QString("HR69C2159 [RTA, SONEPAT,HR] Owner:1-KULDEEP Vehicle:WAGON R GREEN LXI(PETROL/CNG) Motor Cab RC/FC Expiry:25-May-18 Finance:HDFC BANK LTD MV Tax upto:31-Mar-18 -Courtesy:MoRTH/NIC\n "));
    }
    else if(counter<340)
    {
        Sleeper::msleep(3000);
        ui->txtXYRadius_2->appendPlainText(QString("HR07U0246 [THANESAR,HR] Owner:1-MANJU SHARMA Vehicle:MARUTI SWIFT ZDI BS IV(DIESEL) Motor Car RC/FC Expiry:30-Dec-28 MV Tax upto:30-Dec-28(OneTime) -Courtesy:MoRTH/NIC\n"));
    }
    else if(counter<590)
    {
        Sleeper::msleep(3000);
        ui->txtXYRadius_2->appendPlainText(QString("Nothing Found!! Try Again!\n"));
    }
    else
    {
        Sleeper::msleep(3000);
        ui->txtXYRadius_2->appendPlainText(QString("DL12CL3157 [VASANT VIHAR,DL] Owner:1-JYOTI ADVANI Vehicle:CRETA 1.6 VTVT AUTO SX (PETROL) Motor Car RC/FC Expiry:14-Mar-32 Finance:ICICI BANK LTD MV Tax courtesy:MoRTH/NIC\n"));
    }

}

void MainWindow::on_pushButton_7_clicked()
{
     //myfile.open ("database.txt");
    //QString st= ui->txtXYRadius_2->toPlainText();
    //myfile<<st;
}
