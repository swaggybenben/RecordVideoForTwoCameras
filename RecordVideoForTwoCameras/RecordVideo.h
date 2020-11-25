#pragma once

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include <QImage>
#include <QThread>
#include <QMessageBox>
#include <QMouseEvent>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "ui_RecordVideo.h"

class RecordVideo : public QMainWindow
{
	Q_OBJECT

public:
	RecordVideo(QWidget *parent = Q_NULLPTR);
	~RecordVideo();

protected:
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void triangulation(cv::Point2d leftimage, cv::Point2d rightimage, cv::Point3d & marker);

private slots:
	void initialize();
	void startRecord();
	void stopRecord();
	void readFrame();
	void measure();
	void loadImage();
	void loadVideo();
	void loadNextFrame();
	void saveResults();
	void tipIsSelected();
	void baseIsSelected();

private:
	Ui::RecordVideoClass ui;
	cv::VideoCapture capture1, capture2;
	cv::Mat frame1, frame2;
	cv::VideoWriter write1, write2, processedVideo1, processedVideo2;
	QLabel *hint;
	QTimer *timer;
	double cameraSide[8], cameraRear[8], R[9], T[3];
	QImage image1, image2;
	cv::Point leftCircle[3], rightCircle[3];
	cv::Point2d leftCircleCenter, rightCircleCenter;
	cv::Point2d leftLine[2], rightLine[2];
	cv::Point3d targetCenter, catheterTip, catheterPoint, whiteBase, lineVector;
	int framecounter, counter;
	double distance1, distance2, finalDistance1, finalDistance2;
	bool iconVisibility;
	int minute, second;
	QString minutestr, secondstr;
	bool tipSelected;
	QString fileSavePath;
	//Frame counter during video processing
	int frame_num;

	//Optical Flow
	cv::Mat videoImage1, prev_image1, prev_grey1, grey1, flow1, cflow;
	cv::Mat videoImage2, prev_image2, prev_grey2, grey2, flow2;
	cv::Point trackedTip1, trackedTip2;
	int leftColumn1, rightColumn1, upRow1, downRow1;
	int leftColumn2, rightColumn2, upRow2, downRow2;
	void convertFlowToImage(const cv::Mat& flow, cv::Mat& img_x, double threshold);
	void drawOptFlowMap(const cv::Mat& flow, cv::Mat& cflowmap, int step, double, const cv::Scalar& color);

	enum MouseState
	{
		L_C,
		R_C,
		M_C,
		L_DC,
		R_DC,
		Wheel,
		Release
	};

	void setMouseState(MouseState ms, int wheelVal);
	void setMouseUIdefault();
};
