#include "OpticalFlow.h"

OpticalFlow::OpticalFlow()
{

}

OpticalFlow::~OpticalFlow()
{

}

void OpticalFlow::convertFlowToImage(const cv::Mat& flow, cv::Mat& img, double threshold)
{
	for (size_t i = 0; i < flow.rows; i++)
	{
		for (size_t j = 0; j < flow.cols; j++)
		{
			const cv::Point2f& fxy = flow.at<cv::Point2f>(i, j);
			img.at<uchar>(i, j) = sqrt(fxy.x * fxy.x + fxy.y * fxy.y) > threshold ? 255 : 0;
		}
	}
}

void OpticalFlow::processOpticalFlow(cv::Mat& leftImage, cv::Mat& rightImage, cv::Mat& prevLeftImage, cv::Mat& prevRightImage)
{
	grey1.create(leftImage.size(), CV_8UC1);
	grey2.create(rightImage.size(), CV_8UC1);
	cvtColor(leftImage, grey1, CV_BGR2GRAY);
	cvtColor(rightImage, grey2, CV_BGR2GRAY);
	calcOpticalFlowFarneback(prevLeftImage, grey1, flow1, 0.5, 3, 15, 3, 5, 1.2, 0);
	calcOpticalFlowFarneback(prevRightImage, grey2, flow2, 0.5, 3, 15, 3, 5, 1.2, 0);

	cv::Mat imgX1(leftImage.size(), CV_8UC1);
	cv::Mat imgX2(rightImage.size(), CV_8UC1);
	convertFlowToImage(flow1, imgX1, 2.0);
	convertFlowToImage(flow2, imgX2, 0.5);

	cv::vector<cv::vector<cv::Point>> contours1, contours2;
	cv::vector<cv::Vec4i> hierarchy1, hierarchy2;
	findContours(imgX1, contours1, hierarchy1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	findContours(imgX2, contours2, hierarchy2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	if (contours1.size() > 0 && contours2.size() > 0)
	{
		int contourNumber1 = 0;
		for (size_t i = 1; i < contours1.size(); i++)
		{
			if (contours1[i].size() > contours1[contourNumber1].size())
			{
				contourNumber1 = i;
			}
		}
		int temp1 = 0;
		int temp2 = 0;
		for (size_t i = 1; i < contours1[contourNumber1].size(); i++)
		{
			if (contours1[contourNumber1][i].x > contours1[contourNumber1][temp1].x)
			{
				temp1 = i;
				temp2 = temp1;
			}
			else if (contours1[contourNumber1][i].x == contours1[contourNumber1][temp1].x)
			{
				if (contours1[contourNumber1][i].y < contours1[contourNumber1][temp1].y)
				{
					temp1 = i;
				}
				if (contours1[contourNumber1][i].y > contours1[contourNumber1][temp2].y)
				{
					temp2 = i;
				}
			}
		}
		cathetertip1.x = contours1[contourNumber1][temp1].x;
		cathetertip1.y = (contours1[contourNumber1][temp1].y + contours1[contourNumber1][temp2].y) / 2;

		int contourNumber2 = 0;
		for (size_t i = 1; i < contours2.size(); i++)
		{
			if (contours2[i].size() > contours2[contourNumber2].size())
			{
				contourNumber2 = i;
			}
		}
		temp1 = 0;
		temp2 = 0;
		for (size_t i = 1; i < contours2[contourNumber2].size(); i++)
		{
			if (contours2[contourNumber2][i].x < contours2[contourNumber2][temp1].x)
			{
				temp1 = i;
				temp2 = temp1;
			}
			else if (contours2[contourNumber2][i].x == contours2[contourNumber2][temp1].x)
			{
				if (contours2[contourNumber2][i].y < contours2[contourNumber2][temp1].y)
				{
					temp1 = i;
				}
				if (contours2[contourNumber2][i].y > contours2[contourNumber2][temp2].y)
				{
					temp2 = i;
				}
			}
		}
		cathetertip2.x = contours2[contourNumber2][temp1].x;
		cathetertip2.y = (contours2[contourNumber2][temp1].y + contours2[contourNumber2][temp2].y) / 2;
	}
	emit opticalFlowFinished(cathetertip1, cathetertip2);
}

void OpticalFlow::test2()
{
	int i = 0;
	i++;
	i++;
}