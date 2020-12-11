#pragma once
#include <QtCore>
#include <QMetaType>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

class OpticalFlow: public QObject
{
		Q_OBJECT
public:
	OpticalFlow();
	~OpticalFlow();

private:
	cv::Mat grey1, grey2, flow1, flow2;
	cv::Point cathetertip1, cathetertip2;
	void convertFlowToImage(const cv::Mat& flow, cv::Mat& img, double threshold);

public slots:
	void processOpticalFlow(cv::Mat& leftImage, cv::Mat& rightImage, cv::Mat& prevLeftImage, cv::Mat& prevRightImage);
	void test2();

signals:
	void opticalFlowFinished(cv::Point& leftCatheterTip, cv::Point& rightCatheterTip);
};