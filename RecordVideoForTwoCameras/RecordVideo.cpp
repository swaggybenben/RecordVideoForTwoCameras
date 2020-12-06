#include <iomanip>
#include "MyMath/MyMath.h"
#include "RecordVideo.h"

using namespace cv;

RecordVideo::RecordVideo(QWidget *parent)
	: QMainWindow(parent)
{
	
	/*capture1.open("camera1.avi");
	capture2.open("camera2.avi");
	capture1.set(CV_CAP_PROP_POS_FRAMES, 124);
	capture1 >> frame1;
	imwrite("SideImage.jpg", frame1);
	capture2.set(CV_CAP_PROP_POS_FRAMES, 124);
	capture2 >> frame2;
	imwrite("RearImage.jpg", frame2);*/

	setWindowState(Qt::WindowMaximized);
	ui.setupUi(this);
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(readFrame()));
	this->setMouseTracking(true);
	framecounter = 0;
	counter = 0;
	{
		cameraSide[0] = 1445.87837; cameraSide[1] = 1441.04345; cameraSide[2] = 601.16975; cameraSide[3] = 329.05257; cameraSide[4] = 0.32480; cameraSide[5] = -1.02476; cameraSide[6] = -0.01318; cameraSide[7] = -0.00578;
		cameraRear[0] = 1029.35272; cameraRear[1] = 1023.56127; cameraRear[2] = 314.50741; cameraRear[3] = 194.19593; cameraRear[4] = 0.36710; cameraRear[5] = -1.51662; cameraRear[6] = -0.01649; cameraRear[7] = -0.01140;
		R[0] = -0.143573908637465; R[1] = -0.0294185082471390; R[2] = 0.989202246323306; R[3] = 0.0494511193338839; R[4] = 0.998096136527309; R[5] = 0.0368603994265622; R[6] = -0.988403318263950; R[7] = 0.0542093499478895; R[8] = -0.141845785355225;
		T[0] = -65.1684314036080; T[1] = -0.822682459620569; T[2] = 102.331072468238;
	}
	iconVisibility = false;
	hint = new QLabel("Please connect cameras first.");
	ui.statusBar->addWidget(hint);
	QImage recordIcon;
	recordIcon.load("recordIcon.jpg");
	ui.recordIconLabel->setPixmap(QPixmap::fromImage(recordIcon));
	ui.recordIconLabel->setVisible(iconVisibility);
}

RecordVideo::~RecordVideo()
{
	capture1.release();
	capture2.release();
}

void RecordVideo::initialize()
{
	fileSavePath = QFileDialog::getExistingDirectory(this, "File Path", "./");

	capture1.open(2);
	capture2.open(1);

	capture1.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
	QThread::msleep(200);
	capture1.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	QThread::msleep(200);
	capture1.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	QThread::msleep(200);
	capture1.set(CV_CAP_PROP_FPS, 25.0);
	QThread::msleep(200);

	capture2.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
	QThread::msleep(200);
	capture2.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	QThread::msleep(200);
	capture2.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	QThread::msleep(200);
	capture2.set(CV_CAP_PROP_FPS, 25.0);
	QThread::msleep(200);

	write1.open((fileSavePath+"/camera1.avi").toStdString(), CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(1280, 720), true);
	//write1.open("camera1.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(1280, 720), true);
	write2.open((fileSavePath+"/camera2.avi").toStdString(), CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(640, 480), true);

	if (capture1.isOpened()&&capture2.isOpened())
	{
		QMessageBox::information(this, tr("Notice"), tr("Cameras are connected successfully. You can record videos now."));
	}

	hint->setText(" ");
}

void RecordVideo::startRecord()
{
	ui.recordTimeLabel->setText("00:00");
	iconVisibility = !iconVisibility;
	ui.recordIconLabel->setVisible(iconVisibility);
	ui.emptyLabel->setVisible(!iconVisibility);
	minute = 0;
	second = 0;
	timer->start(40);
}

void RecordVideo::stopRecord()
{
	timer->stop();
	write1.release();
	write2.release();
	//capture1.release();
	//capture2.release();
	delete timer;

	QMessageBox::information(this, tr("Notice"), tr("The videos have been saved. Next click Snapshot button."));

	hint->setText("Click three points on the edge of the target in the left image.");
}

void RecordVideo::readFrame()
{
	capture1 >> frame1;
	capture2 >> frame2;
	write1.write(frame1);
	write2.write(frame2);

	image1 = QImage((const uchar*)frame1.data, frame1.cols, frame1.rows, QImage::Format_RGB888).rgbSwapped();
	int height1 = image1.height() / 1;
	int width1 = image1.width() / 1;
	ui.camera1Label->resize(width1, height1);
	ui.camera1Label->setPixmap(QPixmap::fromImage(image1.scaled(width1, height1)));

	image2 = QImage((const uchar*)frame2.data, frame2.cols, frame2.rows, QImage::Format_RGB888).rgbSwapped();
	int height2 = image2.height() / 1;
	int width2 = image2.width() / 1;
	ui.camera2Label->resize(width2, height2);
	ui.camera2Label->setPixmap(QPixmap::fromImage(image2.scaled(width2, height2)));

	framecounter++;
	if (framecounter==25)
	{
		second++;
		if (second==60)
		{
			second = 0;
			minute++;
		}
		minutestr = QString::number(minute);
		secondstr = QString::number(second);
		if (minutestr.length()<2)
		{
			minutestr.insert(0, "0");
		}
		if (secondstr.length() < 2)
		{
			secondstr.insert(0, "0");
		}
		ui.recordTimeLabel->setText(minutestr + ":" + secondstr);
		iconVisibility = !iconVisibility;
		ui.recordIconLabel->setVisible(iconVisibility);
		ui.emptyLabel->setVisible(!iconVisibility);
		framecounter = 0;
	}
}

void RecordVideo::measure()
{
	capture1 >> frame1;
	capture2 >> frame2;

	image1 = QImage((const uchar*)frame1.data, frame1.cols, frame1.rows, QImage::Format_RGB888).rgbSwapped();
	//image1.load("SideImage.jpg");
	int height1 = image1.height() / 1;
	int width1 = image1.width() / 1;
	ui.camera1Label->resize(width1, height1);
	ui.camera1Label->setPixmap(QPixmap::fromImage(image1.scaled(width1, height1)));

	image2 = QImage((const uchar*)frame2.data, frame2.cols, frame2.rows, QImage::Format_RGB888).rgbSwapped();
	//image2.load("RearImage.jpg");
	int height2 = image2.height() / 1;
	int width2 = image2.width() / 1;
	ui.camera2Label->resize(width2, height2);
	ui.camera2Label->setPixmap(QPixmap::fromImage(image2.scaled(width2, height2)));

	//ui.result1Label->setText(QString::number(distance1) + "mm");

	imwrite((fileSavePath+"/SideImage.jpg").toStdString(), frame1);
	imwrite((fileSavePath+"/RearImage.jpg").toStdString(), frame2);

	ui.camera1Label->setMouseTracking(true);
	ui.camera2Label->setMouseTracking(true);

	/*Mat graySide;
	cvtColor(frame1, graySide, COLOR_BGR2GRAY);
	Mat blurSide;
	GaussianBlur(graySide, blurSide, Size(9, 9), 2, 2);
	Mat cannySide;
	Canny(blurSide, cannySide, 200, 250, 5);
	vector<Vec3f> circleSide;
	HoughCircles(cannySide, circleSide, CV_HOUGH_GRADIENT, 1, 200, 100, 30, 25, 75);

	Mat grayRear;
	cvtColor(frame2, grayRear, COLOR_BGR2GRAY);
	Mat blurRear;
	GaussianBlur(grayRear, blurRear, Size(9, 9), 2, 2);
	Mat cannyRear;
	Canny(blurRear, cannyRear, 200, 250, 5);
	vector<Vec3f> circleRear;
	HoughCircles(cannyRear, circleRear, CV_HOUGH_GRADIENT, 1, 200, 100, 30, 25, 75);*/
}

void RecordVideo::loadImage()
{
	fileSavePath = QFileDialog::getExistingDirectory(this, "File Path", "./");
	frame1 = imread((fileSavePath + "/SideImage.jpg").toStdString());
	image1 = QImage((const uchar*)frame1.data, frame1.cols, frame1.rows, QImage::Format_RGB888).rgbSwapped();
	int height1 = image1.height() / 1;
	int width1 = image1.width() / 1;
	ui.camera1Label->resize(width1, height1);
	ui.camera1Label->setPixmap(QPixmap::fromImage(image1.scaled(width1, height1)));
	frame2 = imread((fileSavePath + "/RearImage.jpg").toStdString());
	image2 = QImage((const uchar*)frame2.data, frame2.cols, frame2.rows, QImage::Format_RGB888).rgbSwapped();
	int height2 = image2.height() / 1;
	int width2 = image2.width() / 1;
	ui.camera2Label->resize(width2, height2);
	ui.camera2Label->setPixmap(QPixmap::fromImage(image2.scaled(width2, height2)));
	ui.camera1Label->setMouseTracking(true);
	ui.camera2Label->setMouseTracking(true);
	hint->setText("Click three points on the edge of the target in the left image.");
}

void RecordVideo::loadVideo()
{
	fileSavePath = QFileDialog::getExistingDirectory(this, "File Path", "./");
	capture1.open((fileSavePath + "/camera1.avi").toStdString());
	capture2.open((fileSavePath + "/camera2.avi").toStdString());
	if (!(capture1.isOpened()&&capture2.isOpened()))
	{
		QMessageBox::information(this, tr("Fail"), tr("Fail to load video."));
	}
	else
	{
		QMessageBox::information(this, tr("Success"), tr("Video is loaded."));
	}
	frame_num = 0;
	trackedTip1.x = 0;
	trackedTip1.y = 800;
	trackedTip2.x = 0;
	trackedTip2.y = 300;
	upRow1 = 0; downRow1 = 200; leftColumn1 = 600; rightColumn1 = 1000;
	upRow2 = 0; downRow2 = 200; leftColumn2 = 100; rightColumn2 = 500;
	while (true)
	{
		capture1 >> frame1;
		capture2 >> frame2;
		if (frame_num == 400)
		{
			frame_num++;

			prev_image1 = frame1(Range(upRow1, downRow1), Range(leftColumn1, rightColumn1));
			cvtColor(prev_image1, prev_grey1, CV_BGR2GRAY);
			prev_image2 = frame2(Range(upRow2, downRow2), Range(leftColumn2, rightColumn2));
			cvtColor(prev_image2, prev_grey2, CV_BGR2GRAY);

			break;
		}
		frame_num++;
	}

	processedVideo1.open("ProcessedCamera1.avi", CV_FOURCC('M', 'J', 'P', 'G'), 5.0, Size(1280, 720), true);
	processedVideo2.open("ProcessedCamera2.avi", CV_FOURCC('M', 'J', 'P', 'G'), 5.0, Size(640, 480), true);
}

void RecordVideo::loadNextFrame()
{
	capture1 >> frame1;
	capture2 >> frame2;
	if (frame1.empty())
	{
		QMessageBox::information(this, tr("Warning"), tr("Get to the last frame."));
		return;
	}
	if (frame_num%5==0)
	{
		videoImage1 = frame1(Range(upRow1, downRow1), Range(leftColumn1, rightColumn1));
		videoImage2 = frame2(Range(upRow2, downRow2), Range(leftColumn2, rightColumn2));
		grey1.create(videoImage1.size(), CV_8UC1);
		grey2.create(videoImage2.size(), CV_8UC1);
		cvtColor(videoImage1, grey1, CV_BGR2GRAY);
		cvtColor(videoImage2, grey2, CV_BGR2GRAY);
		calcOpticalFlowFarneback(prev_grey1, grey1, flow1, 0.5, 3, 15, 3, 5, 1.2, 0);
		calcOpticalFlowFarneback(prev_grey2, grey2, flow2, 0.5, 3, 15, 3, 5, 1.2, 0);

		Mat imgX1(videoImage1.size(), CV_8UC1);
		Mat imgX2(videoImage2.size(), CV_8UC1);
		convertFlowToImage(flow1, imgX1, 2.0);
		convertFlowToImage(flow2, imgX2, 0.5);

		vector<vector<Point>> contours1, contours2;
		vector<Vec4i> hierarchy1, hierarchy2;
		findContours(imgX1, contours1, hierarchy1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		findContours(imgX2, contours2, hierarchy2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		if (contours1.size() > 0 && contours2.size()>0)
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
			Point cathetertip1;
			cathetertip1.x = contours1[contourNumber1][temp1].x;
			cathetertip1.y = (contours1[contourNumber1][temp1].y + contours1[contourNumber1][temp2].y) / 2;
			trackedTip1.x = leftColumn1 + cathetertip1.x;
			trackedTip1.y = upRow1 + cathetertip1.y;
			if (trackedTip1.y < 200)
			{
				upRow1 = 0;
				downRow1 = trackedTip1.y + 200;
			}
			else if (trackedTip1.y > 520)
			{
				upRow1 = trackedTip1.y - 200;
				downRow1 = 720;
			}
			else
			{
				upRow1 = trackedTip1.y - 200;
				downRow1 = trackedTip1.y + 200;
			}
			if (trackedTip1.x < 200)
			{
				leftColumn1 = 0;
				rightColumn1 = trackedTip1.x + 200;
			}
			else if (trackedTip1.x > 1080)
			{
				leftColumn1 = trackedTip1.x - 200;
				rightColumn1 = 1280;
			}
			else
			{
				leftColumn1 = trackedTip1.x - 200;
				rightColumn1 = trackedTip1.x + 200;
			}

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
			Point cathetertip2;
			cathetertip2.y = (contours2[contourNumber2][temp1].y + contours2[contourNumber2][temp2].y) / 2;
			cathetertip2.x = contours2[contourNumber2][temp1].x;
			trackedTip2.x = leftColumn2 + cathetertip2.x;
			trackedTip2.y = upRow2 + cathetertip2.y;
			if (trackedTip2.y < 200)
			{
				upRow2 = 0;
				downRow2 = trackedTip2.y + 200;
			}
			else if (trackedTip2.y > 280)
			{
				upRow2 = trackedTip2.y - 200;
				downRow2 = 480;
			}
			else
			{
				upRow2 = trackedTip2.y - 200;
				downRow2 = trackedTip2.y + 200;
			}
			if (trackedTip2.x <200)
			{
				leftColumn2 = 0;
				rightColumn2 = trackedTip2.x + 200;
			}
			else if (trackedTip2.x > 440)
			{
				leftColumn2 = trackedTip2.x - 200;
				rightColumn2 = 640;
			}
			else
			{
				leftColumn2 = trackedTip2.x - 200;
				rightColumn2 = trackedTip2.x + 200;
			}

			triangulation(trackedTip1, trackedTip2, catheterTip);
			double trackedDistance = sqrt((catheterTip.x - 6.2) * (catheterTip.x - 6.2) + (catheterTip.y - 8.6) * (catheterTip.y - 8.6) + (catheterTip.z - 61.3) * (catheterTip.z - 61.3));
			std::stringstream stream;
			stream << std::fixed << std::setprecision(1) << trackedDistance;
			putText(frame1, stream.str(), Point(20, 700), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2, 8, false);
		}

		prev_image1 = frame1(Range(upRow1, downRow1), Range(leftColumn1, rightColumn1));
		prev_image2 = frame2(Range(upRow2, downRow2), Range(leftColumn2, rightColumn2));
		prev_grey1.create(prev_image1.size(), CV_8UC1);
		prev_grey2.create(prev_image2.size(), CV_8UC1);
		cvtColor(prev_image1, prev_grey1, CV_BGR2GRAY);
		cvtColor(prev_image2, prev_grey2, CV_BGR2GRAY);
		circle(frame1, trackedTip1, 2, (0, 0, 255), 2);
		circle(frame2, trackedTip2, 2, (0, 0, 255), 2);

		drawContours(videoImage1, contours1, -1, Scalar(0, 255, 0));
		//drawContours(videoImage2, contours2, -1, Scalar(0, 255, 0));

		//drawContours(videoImage, contours, contourNumber, Scalar(0,255,0), 1, 8, hierarchy);

		//image1 = QImage((const uchar*)videoImage1.data, videoImage1.cols, videoImage1.rows, videoImage1.step, QImage::Format_RGB888);
		image1 = QImage((const uchar*)frame1.data, frame1.cols, frame1.rows, QImage::Format_RGB888).rgbSwapped();
		int height1 = image1.height() / 1;
		int width1 = image1.width() / 1;
		ui.camera1Label->resize(width1, height1);
		ui.camera1Label->setPixmap(QPixmap::fromImage(image1.scaled(width1, height1)));
		//image2 = QImage((const uchar*)videoImage2.data, videoImage2.cols, videoImage2.rows, videoImage2.step, QImage::Format_RGB888);
		image2 = QImage((const uchar*)frame2.data, frame2.cols, frame2.rows, QImage::Format_RGB888).rgbSwapped();
		int height2 = image2.height() / 1;
		int width2 = image2.width() / 1;
		ui.camera2Label->resize(width2, height2);
		ui.camera2Label->setPixmap(QPixmap::fromImage(image2.scaled(width2, height2)));
		/*imwrite("test1.jpg", imgX);
		imwrite("test2.jpg", videoImage);

		image2 = QImage((const uchar*)videoImage.data, videoImage.cols, videoImage.rows, videoImage.step, QImage::Format_RGB888);
		height1 = image2.height() / 1;
		width1 = image2.width() / 1;
		ui.camera2Label->resize(width1, height1);
		ui.camera2Label->setPixmap(QPixmap::fromImage(image2.scaled(width1, height1)));
		imwrite("test3.jpg", prev_grey);
		imwrite("test4.jpg", grey);*/

		processedVideo1.write(frame1);
		processedVideo2.write(frame2);

		//std::swap(prev_grey, grey);
		//std::swap(prev_image, videoImage);
	}
	frame_num++;
}

void RecordVideo::drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step, double, const Scalar& color)
{
	for (size_t y = 0; y < cflowmap.rows; y += step)
	{
		for (size_t x = 0; x < cflowmap.cols; x += step)
		{
			const Point2f& fxy = flow.at<Point2f>(y, x);
			line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), color);
			circle(cflowmap, Point(x, y), 2, color, -1);
		}
	}
}

void RecordVideo::convertFlowToImage(const Mat& flow, Mat& img, double threshold)
{
	for (size_t i = 0; i < flow.rows; i++)
	{
		for (size_t j = 0; j < flow.cols; j++)
		{
			const Point2f& fxy = flow.at<Point2f>(i, j);
			img.at<uchar>(i, j) = sqrt(fxy.x * fxy.x + fxy.y * fxy.y) > threshold ? 255 : 0;
		}
	}
}

void RecordVideo::setMouseState(MouseState ms, int wheelVal)
{
	QString style_active = "border:2px solid black;background:green";
	QString style_release = "border:2px solid black;";
	setMouseUIdefault();
	QMessageBox msgBox;
	QPoint mousePosition;
	switch (ms)
	{
	case RecordVideo::L_C:
		break;
	case RecordVideo::R_C:
		msgBox.setText("Warning!");
		msgBox.exec();
		break;
	case RecordVideo::M_C:
		break;
	case RecordVideo::L_DC:
		break;
	case RecordVideo::R_DC:
		break;
	case RecordVideo::Wheel:
		break;
	case RecordVideo::Release:
		break;
	default:
		break;
	}
}

void RecordVideo::saveResults()
{
	QFile csvFile(fileSavePath+"/Result.csv");
	QString stringLine;
	stringLine = "Point, X, Y, Z\n";
	csvFile.open(QIODevice::WriteOnly);
	csvFile.write(stringLine.toLatin1());
	stringLine = "Target, " + QString::number(targetCenter.x, 'f', 1) + ", " + QString::number(targetCenter.y, 'f', 1) + ", " + QString::number(targetCenter.z, 'f', 1) + "\n";
	csvFile.write(stringLine.toLatin1());
	stringLine = "Catheter Tip, " + QString::number(catheterTip.x, 'f', 1) + ", " + QString::number(catheterTip.y, 'f', 1) + ", " + QString::number(catheterTip.z, 'f', 1) + "\n";
	csvFile.write(stringLine.toLatin1());
	stringLine = "Line Vector, " + QString::number(catheterTip.x - catheterPoint.x, 'f', 1) + ", " + QString::number(catheterTip.y - catheterPoint.y, 'f', 1) + ", " + QString::number(catheterTip.z - catheterPoint.z, 'f', 1) + "\n";
	csvFile.write(stringLine.toLatin1());
	stringLine = "Distance between Catheter Tip and Target, " + QString::number(finalDistance1, 'f', 1) + "\n";
	csvFile.write(stringLine.toLatin1());
	stringLine = "Distance between Catheter Line and Target, " + QString::number(finalDistance2, 'f', 1) + "\n";
	csvFile.write(stringLine.toLatin1());
}

void RecordVideo::tipIsSelected()
{
	tipSelected = true;
	hint->setText("Click catheter tip and another point in the left image.");
}

void RecordVideo::baseIsSelected()
{
	tipSelected = false;
	hint->setText("Click catherter white base and another point in the left image.");
}

void RecordVideo::setMouseUIdefault()
{

}

void RecordVideo::mouseMoveEvent(QMouseEvent* event)
{
	QPoint p_ab = event->globalPos();
	QPoint p_re = event->pos();
}

void RecordVideo::mousePressEvent(QMouseEvent* event)
{
	if (event->button()==Qt::LeftButton)
	{
		if (counter < 3)
		{
			QPoint mousePosition = event->globalPos();
			mousePosition = ui.camera1Label->mapFromGlobal(mousePosition);
			int xoffset = (ui.camera1Label->contentsRect().width() - ui.camera1Label->pixmap()->rect().width()) / 2;
			int yoffset = (ui.camera1Label->contentsRect().height() - ui.camera1Label->pixmap()->rect().height()) / 2;
			leftCircle[counter].x = mousePosition.x() - xoffset;
			leftCircle[counter].y = mousePosition.y() - yoffset;
			circle(frame1, leftCircle[counter], 3, Scalar(0, 255, 0), -1, 8);
			image1 = QImage((const uchar*)frame1.data, frame1.cols, frame1.rows, QImage::Format_RGB888).rgbSwapped();
			int height1 = image1.height() / 1;
			int width1 = image1.width() / 1;
			ui.camera1Label->resize(width1, height1);
			ui.camera1Label->setPixmap(QPixmap::fromImage(image1.scaled(width1, height1)));
			counter++;
			if (counter == 3)
			{
				double a = leftCircle[0].x - leftCircle[1].x;
				double b = leftCircle[0].y - leftCircle[1].y;
				double c = leftCircle[0].x - leftCircle[2].x;
				double d = leftCircle[0].y - leftCircle[2].y;
				double e = ((leftCircle[0].x * leftCircle[0].x - leftCircle[1].x * leftCircle[1].x) + (leftCircle[0].y * leftCircle[0].y - leftCircle[1].y * leftCircle[1].y)) / 2.0;
				double f = ((leftCircle[0].x * leftCircle[0].x - leftCircle[2].x * leftCircle[2].x) + (leftCircle[0].y * leftCircle[0].y - leftCircle[2].y * leftCircle[2].y)) / 2.0;
				double det = b * c - a * d;
				leftCircleCenter.x = -(d * e - b * f) / det;
				leftCircleCenter.y = -(a * f - c * e) / det;
				double r = sqrt((leftCircle[0].x - leftCircleCenter.x) * (leftCircle[0].x - leftCircleCenter.x) + (leftCircle[0].y - leftCircleCenter.y) * (leftCircle[0].y - leftCircleCenter.y));
				circle(frame1, leftCircleCenter, r, Scalar(0, 255, 0), 1, 8);
				image1 = QImage((const uchar*)frame1.data, frame1.cols, frame1.rows, QImage::Format_RGB888).rgbSwapped();
				int height1 = image1.height() / 1;
				int width1 = image1.width() / 1;
				ui.camera1Label->resize(width1, height1);
				ui.camera1Label->setPixmap(QPixmap::fromImage(image1.scaled(width1, height1)));
				hint->setText("Click three points on the edge of the target in the right image.");
			}
		}
		else if (counter > 2 && counter < 6)
		{
			QPoint mousePosition = event->globalPos();
			mousePosition = ui.camera2Label->mapFromGlobal(mousePosition);
			int xoffset = (ui.camera2Label->contentsRect().width() - ui.camera2Label->pixmap()->rect().width()) / 2;
			int yoffset = (ui.camera2Label->contentsRect().height() - ui.camera2Label->pixmap()->rect().height()) / 2;
			rightCircle[counter-3].x = mousePosition.x() - xoffset;
			rightCircle[counter-3].y = mousePosition.y() - yoffset;
			circle(frame2, rightCircle[counter-3], 3, Scalar(0, 255, 0), -1, 8);
			image2 = QImage((const uchar*)frame2.data, frame2.cols, frame2.rows, QImage::Format_RGB888).rgbSwapped();
			int height2 = image2.height() / 1;
			int width2 = image2.width() / 1;
			ui.camera2Label->resize(width2, height2);
			ui.camera2Label->setPixmap(QPixmap::fromImage(image2.scaled(width2, height2)));
			counter++;
			if (counter == 6)
			{
				double a = rightCircle[0].x - rightCircle[1].x;
				double b = rightCircle[0].y - rightCircle[1].y;
				double c = rightCircle[0].x - rightCircle[2].x;
				double d = rightCircle[0].y - rightCircle[2].y;
				double e = ((rightCircle[0].x * rightCircle[0].x - rightCircle[1].x * rightCircle[1].x) + (rightCircle[0].y * rightCircle[0].y - rightCircle[1].y * rightCircle[1].y)) / 2.0;
				double f = ((rightCircle[0].x * rightCircle[0].x - rightCircle[2].x * rightCircle[2].x) + (rightCircle[0].y * rightCircle[0].y - rightCircle[2].y * rightCircle[2].y)) / 2.0;
				double det = b * c - a * d;
				rightCircleCenter.x = -(d * e - b * f) / det;
				rightCircleCenter.y = -(a * f - c * e) / det;
				double r = sqrt((rightCircle[0].x - rightCircleCenter.x) * (rightCircle[0].x - rightCircleCenter.x) + (rightCircle[0].y - rightCircleCenter.y) * (rightCircle[0].y - rightCircleCenter.y));
				circle(frame2, rightCircleCenter, r, Scalar(0, 255, 0), 1, 8);
				image2 = QImage((const uchar*)frame2.data, frame2.cols, frame2.rows, QImage::Format_RGB888).rgbSwapped();
				int height2 = image2.height() / 1;
				int width2 = image2.width() / 1;
				ui.camera2Label->resize(width2, height2);
				ui.camera2Label->setPixmap(QPixmap::fromImage(image2.scaled(width2, height2)));
				hint->setText("Select catheter tip or white base with radio button.");
			}
		}
		else if (counter > 5 && counter < 8)
		{
			QPoint mousePosition = event->globalPos();
			mousePosition = ui.camera1Label->mapFromGlobal(mousePosition);
			int xoffset = (ui.camera1Label->contentsRect().width() - ui.camera1Label->pixmap()->rect().width()) / 2;
			int yoffset = (ui.camera1Label->contentsRect().height() - ui.camera1Label->pixmap()->rect().height()) / 2;
			leftLine[counter-6].x = mousePosition.x() - xoffset;
			leftLine[counter-6].y = mousePosition.y() - yoffset;
			circle(frame1, leftLine[counter-6], 3, Scalar(0, 255, 0), -1, 8);
			image1 = QImage((const uchar*)frame1.data, frame1.cols, frame1.rows, QImage::Format_RGB888).rgbSwapped();
			int height1 = image1.height() / 1;
			int width1 = image1.width() / 1;
			ui.camera1Label->resize(width1, height1);
			ui.camera1Label->setPixmap(QPixmap::fromImage(image1.scaled(width1, height1)));
			counter++;
			if (counter == 8)
			{
				line(frame1, leftLine[0], leftLine[1], Scalar(0, 255, 0), 2, 8);
				image1 = QImage((const uchar*)frame1.data, frame1.cols, frame1.rows, QImage::Format_RGB888).rgbSwapped();
				int height1 = image1.height() / 1;
				int width1 = image1.width() / 1;
				ui.camera1Label->resize(width1, height1);
				ui.camera1Label->setPixmap(QPixmap::fromImage(image1.scaled(width1, height1)));
				if (tipSelected)
				{
					hint->setText("Click catheter tip and another point in the right image.");
				}
				else
				{
					hint->setText("Click catheter white base and another point in the right image.");
				}
			}
		}
		else if (counter > 7 && counter < 10)
		{
			QPoint mousePosition = event->globalPos();
			mousePosition = ui.camera2Label->mapFromGlobal(mousePosition);
			int xoffset = (ui.camera2Label->contentsRect().width() - ui.camera2Label->pixmap()->rect().width()) / 2;
			int yoffset = (ui.camera2Label->contentsRect().height() - ui.camera2Label->pixmap()->rect().height()) / 2;
			rightLine[counter - 8].x = mousePosition.x() - xoffset;
			rightLine[counter - 8].y = mousePosition.y() - yoffset;
			circle(frame2, rightLine[counter - 8], 3, Scalar(0, 255, 0), -1, 8);
			image2 = QImage((const uchar*)frame2.data, frame2.cols, frame2.rows, QImage::Format_RGB888).rgbSwapped();
			int height2 = image2.height() / 1;
			int width2 = image2.width() / 1;
			ui.camera2Label->resize(width2, height2);
			ui.camera2Label->setPixmap(QPixmap::fromImage(image2.scaled(width2, height2)));
			counter++;
			if (counter == 10)
			{
				line(frame2, rightLine[0], rightLine[1], Scalar(0, 255, 0), 2, 8);
				image2 = QImage((const uchar*)frame2.data, frame2.cols, frame2.rows, QImage::Format_RGB888).rgbSwapped();
				int height2 = image2.height() / 1;
				int width2 = image2.width() / 1;
				ui.camera2Label->resize(width2, height2);
				ui.camera2Label->setPixmap(QPixmap::fromImage(image2.scaled(width2, height2)));
				hint->setText("Click again to calculate.");
			}
		}
		else if (counter == 10)
		{
			if (tipSelected)
			{
				triangulation(leftCircleCenter, rightCircleCenter, targetCenter);
				triangulation(leftLine[0], rightLine[0], catheterTip);
				triangulation(leftLine[1], rightLine[1], catheterPoint);
			}
			else
			{
				triangulation(leftCircleCenter, rightCircleCenter, targetCenter);
				triangulation(leftLine[0], rightLine[0], whiteBase);
				triangulation(leftLine[1], rightLine[1], catheterPoint);
				double d = sqrt(pow((catheterPoint.x - whiteBase.x), 2.0) + pow((catheterPoint.y - whiteBase.y), 2.0) + pow((catheterPoint.z - whiteBase.z), 2.0));
				catheterTip.x = whiteBase.x - 30.5 * (whiteBase.x - catheterPoint.x) / d;
				catheterTip.y = whiteBase.y - 30.5 * (whiteBase.y - catheterPoint.y) / d;
				catheterTip.z = whiteBase.z - 30.5 * (whiteBase.z - catheterPoint.z) / d;
			}
			distance1 = sqrt((targetCenter.x - catheterTip.x) * (targetCenter.x - catheterTip.x) + (targetCenter.y - catheterTip.y) * (targetCenter.y - catheterTip.y) + (targetCenter.z - catheterTip.z) * (targetCenter.z - catheterTip.z));
			double a = sqrt(pow((catheterPoint.x - catheterTip.x), 2.0) + pow((catheterPoint.y - catheterTip.y), 2.0) + pow((catheterPoint.z - catheterTip.z), 2.0));
			double b = sqrt(pow((catheterPoint.x - targetCenter.x), 2.0) + pow((catheterPoint.y - targetCenter.y), 2.0) + pow((catheterPoint.z - targetCenter.z), 2.0));
			double c = sqrt(pow((targetCenter.x - catheterTip.x), 2.0) + pow((targetCenter.y - catheterTip.y), 2.0) + pow((targetCenter.z - catheterTip.z), 2.0));
			double cosc = (pow(a, 2.0) + pow(b, 2.0) - pow(c, 2.0)) / (2 * a * b);
			double sinc = sqrt(1 - pow(cosc, 2.0));
			distance2 = b * sinc;
			ui.targetResultLabel->setText("(" + QString::number(targetCenter.x, 'f', 1) + ", " + QString::number(targetCenter.y, 'f', 1) + ", " + QString::number(targetCenter.z, 'f', 1) + ")");
			ui.tipResultLabel->setText("(" + QString::number(catheterTip.x, 'f', 1) + ", " + QString::number(catheterTip.y, 'f', 1) + ", " + QString::number(catheterTip.z, 'f', 1) + ")");
			ui.vectorResultLabel->setText("(" + QString::number(catheterTip.x-catheterPoint.x, 'f', 1) + ", " + QString::number(catheterTip.y - catheterPoint.y, 'f', 1) + ", " + QString::number(catheterTip.z - catheterPoint.z, 'f', 1) + ")");
			if (distance1<2.05)
			{
				finalDistance1 = 0.0;
				ui.result1Label->setText(QString::number(finalDistance1) + "mm");
			}
			else
			{
				finalDistance1 = distance1 - 2.0;
				ui.result1Label->setText(QString::number(finalDistance1, 'f', 1) + "mm");
			}
			if (distance2<2.05)
			{
				finalDistance2 = 0.0;
				ui.result2Label->setText(QString::number(finalDistance2) + "mm");
			}
			else
			{
				finalDistance2 = distance2 - 2.0;
				ui.result2Label->setText(QString::number(finalDistance2, 'f', 1) + "mm");
			}
			hint->setText(" ");
		}
	}
	else if (event->button()==Qt::RightButton)
	{
		setMouseState(MouseState::R_C, 0);
	}
	else if (event->button()==Qt::MidButton)
	{
		setMouseState(MouseState::M_C, 0);
	}
}

void RecordVideo::mouseReleaseEvent(QMouseEvent* event)
{
	setMouseState(MouseState::Release, 0);
}

void RecordVideo::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button()==Qt::LeftButton)
	{
		setMouseState(MouseState::L_DC, 0);
	}
	else if (event->button()==Qt::RightButton)
	{
		setMouseState(MouseState::R_DC, 0);
	}
}

void RecordVideo::wheelEvent(QWheelEvent* event)
{
	int wheel_val = event->delta();
	setMouseState(MouseState::Wheel, wheel_val);
}

//void RecordVideo::triangulation(Point2d leftimage, Point2d rightimage, Point3d & marker)
//{
//	double leftx = (leftimage.x - cameraSide[2]) / cameraSide[0];
//	double lefty = (leftimage.y - cameraSide[3]) / cameraSide[1];
//	double r1 = sqrt(leftx * leftx + lefty * lefty);
//	double x1 = (leftx - leftx * (cameraSide[4] * r1 * r1 + cameraSide[5] * r1 * r1 * r1 * r1) - 2 * cameraSide[6] * leftx * lefty - cameraSide[7] * (r1 * r1 + 2 * leftx * leftx)) * cameraSide[0];
//	double y1 = (lefty - lefty * (cameraSide[4] * r1 * r1 + cameraSide[5] * r1 * r1 * r1 * r1) - 2 * cameraSide[7] * leftx * lefty - cameraSide[6] * (r1 * r1 + 2 * lefty * lefty)) * cameraSide[1];
//	double rightx = (rightimage.x - cameraRear[2]) / cameraRear[0];
//	double righty = (rightimage.y - cameraRear[3]) / cameraRear[1];
//	double r2 = sqrt(rightx * rightx + righty * righty);
//	double x2 = (rightx - rightx * (cameraRear[4] * r2 * r2 + cameraRear[5] * r2 * r2 * r2 * r2) - 2 * cameraRear[6] * rightx * righty - cameraRear[7] * (r2 * r2 + 2 * rightx * rightx)) * cameraRear[0];
//	double y2 = (righty - righty * (cameraRear[4] * r2 * r2 + cameraRear[5] * r2 * r2 * r2 * r2) - 2 * cameraRear[7] * rightx * righty - cameraRear[6] * (r2 * r2 + 2 * righty * righty)) * cameraRear[1];
//	marker.z = (cameraSide[0] * cameraSide[1] * x2 * T[2] - cameraRear[0] * cameraSide[0] * cameraSide[1] * T[0]) / (cameraRear[0] * (cameraSide[1] * R[0] * x1 + cameraSide[0] * R[1] * y1 + cameraSide[0] * cameraSide[1] * R[2]) - x2 * (cameraSide[1] * R[6] * x1 + cameraSide[0] * R[7] * y1 + cameraSide[0] * cameraSide[1] * R[8])) * 0.883850;
//	marker.x = marker.z * x1 / cameraSide[0];
//	marker.y = marker.z * y1 / cameraSide[1];
//}

void RecordVideo::triangulation(Point2d leftimage, Point2d rightimage, Point3d& marker)
{
	double leftx = (leftimage.x - cameraSide[2]) / cameraSide[0];
	double lefty = (leftimage.y - cameraSide[3]) / cameraSide[1];
	double r1 = sqrt(leftx * leftx + lefty * lefty);
	double x1 = (leftx - leftx * (cameraSide[4] * r1 * r1 + cameraSide[5] * r1 * r1 * r1 * r1) - 2 * cameraSide[6] * leftx * lefty - cameraSide[7] * (r1 * r1 + 2 * leftx * leftx)) * cameraSide[0];
	double y1 = (lefty - lefty * (cameraSide[4] * r1 * r1 + cameraSide[5] * r1 * r1 * r1 * r1) - 2 * cameraSide[7] * leftx * lefty - cameraSide[6] * (r1 * r1 + 2 * lefty * lefty)) * cameraSide[1];
	double rightx = (rightimage.x - cameraRear[2]) / cameraRear[0];
	double righty = (rightimage.y - cameraRear[3]) / cameraRear[1];
	double r2 = sqrt(rightx * rightx + righty * righty);
	double x2 = (rightx - rightx * (cameraRear[4] * r2 * r2 + cameraRear[5] * r2 * r2 * r2 * r2) - 2 * cameraRear[6] * rightx * righty - cameraRear[7] * (r2 * r2 + 2 * rightx * rightx)) * cameraRear[0];
	double y2 = (righty - righty * (cameraRear[4] * r2 * r2 + cameraRear[5] * r2 * r2 * r2 * r2) - 2 * cameraRear[7] * rightx * righty - cameraRear[6] * (r2 * r2 + 2 * righty * righty)) * cameraRear[1];

	double** a;
	a = new double* [4];
	for (size_t i = 0; i < 4; i++)
	{
		a[i] = new double[3];
	}
	a[0][0] = cameraSide[0]; a[0][1] = 0; a[0][2] = -x1; a[1][0] = 0; a[1][1] = cameraSide[1]; a[1][2] = -y1; a[2][0] = cameraRear[0] * R[0] - x2 * R[6]; a[2][1] = cameraRear[0] * R[1] - x2 * R[7]; a[2][2] = cameraRear[0] * R[2] - x2 * R[8]; a[3][0] = cameraRear[1] * R[3] - y2 * R[6]; a[3][1] = cameraRear[1] * R[4] - y2 * R[7]; a[3][2] = cameraRear[1] * R[5] - y2 * R[8];
	double** b;
	b = new double* [4];
	for (size_t i = 0; i < 4; i++)
	{
		b[i] = new double[1];
	}
	b[0][0] = 0; b[1][0] = 0; b[2][0] = x2 * T[2] - cameraRear[0] * T[0]; b[3][0] = y2 * T[2] - cameraRear[1] * T[1];
	double** x;
	x = new double* [3];
	for (size_t i = 0; i < 3; i++)
	{
		x[i] = new double[1];
	}
	CMyMath matrixCal;
	matrixCal.LeastSquare(a, 4, 3, 1, b, x);
	double scaleFactor = 0.899757;
	marker.z = x[2][0] * scaleFactor;
	marker.x = x[0][0] * scaleFactor;
	marker.y = x[1][0] * scaleFactor;
	for (size_t i = 0; i < 3; i++)
	{
		delete[] x[i];
	}
	delete[]x;
	for (size_t i = 0; i < 4; i++)
	{
		delete[] b[i];
	}
	delete[]b;
	for (size_t i = 0; i < 4; i++)
	{
		delete[]a[i];
	}
	delete[]a;
}