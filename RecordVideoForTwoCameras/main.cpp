#include "RecordVideo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RecordVideo w;
	w.show();
	return a.exec();
}
