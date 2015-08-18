#include "Header.h"


void tutorial00(){

	const int FRAMESIZE = 600;
	IplImage *image = cvCreateImage(cvSize(FRAMESIZE, FRAMESIZE), 8, 3);
	cvNamedWindow("方格圖", 1);
	cvResizeWindow("方格圖", FRAMESIZE, FRAMESIZE);
	cvSet(image, cvScalar(244, 244, 244));

	int x = 50, y = 50;
	int width = 50;
	int squareNum = 10;	//格子橫排數量
	int squareSize = width * squareNum;	//總寬/高
	CvScalar Color = cvScalar(0, 0, 0);	//方格顏色

	/*	畫線	*/
	for (int i = 0; i <= squareNum; i++){
		cvLine(image, cvPoint(x + width * i, y), cvPoint(x + width * i, y + squareSize), Color, 3);
	}

	for (int i = 0; i <= squareNum; i++){
		cvLine(image, cvPoint(x, y + width * i), cvPoint(x + squareSize, y + width * i), Color, 3);
	}


	/*	上顏色	*/
	const int offset = 5; //填滿標記點位移
	int oddFlag = 0;	//基偶數辨認旗標
	for (int i = 1; i <= squareNum; i++)
	{
		for (int j = 1; j <= squareNum; j++){
			if (j%2==oddFlag)
				cvFloodFill(image, cvPoint(x + j * width - offset, y - offset + i * width), Color);
		}
		oddFlag++;
		oddFlag = oddFlag % 2;
	}

	cvShowImage("方格圖", image);

	cvWaitKey(0);
}