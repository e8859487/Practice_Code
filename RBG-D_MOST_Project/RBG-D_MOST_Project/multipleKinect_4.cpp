// STL Header
#include <iostream>

// OpenCV Header
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// OpenNI Header
#include <OpenNI.h>

// namespace
using namespace std;
using namespace openni;

int main(){
	// Initial OpenNI
	if (OpenNI::initialize() != STATUS_OK)
	{
		cerr << "OpenNI Initial Error: " << OpenNI::getExtendedError() << endl;
		return -1;
	}

	Array<DeviceInfo> deviceInfoList;
	OpenNI::enumerateDevices(&deviceInfoList);
	cout << "DEVICE NUMBERS : " << deviceInfoList.getSize()<<endl;
	for (int i = 0; i < deviceInfoList.getSize(); ++i){
		cout << "Name:" << deviceInfoList[i].getName() << endl;
		cout << "Uri:" << deviceInfoList[i].getUri() << endl;
		cout << "UsbProductId:" << deviceInfoList[i].getUsbProductId() << endl;
		cout << "UsbVendorId:" << deviceInfoList[i].getUsbVendorId() << endl;
		cout << "Vendor:" << deviceInfoList[i].getVendor() << endl;
		cout << endl;
	}

#pragma region Device1
	// Open Device
	Device	devDevice_1;
	if (devDevice_1.open(deviceInfoList[0].getUri()) != STATUS_OK)
	{
		cerr << "Can't Open Device: " << OpenNI::getExtendedError() << endl;
		return -1;
	}

	// Create depth stream
	VideoStream vsDepth;
	if (devDevice_1.hasSensor(SENSOR_DEPTH))
	{
		if (vsDepth.create(devDevice_1, SENSOR_DEPTH) == STATUS_OK)
		{
			// set video mode
			VideoMode mMode;
			mMode.setResolution(640, 480);
			mMode.setFps(30);
			mMode.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);

			if (vsDepth.setVideoMode(mMode) != STATUS_OK)
				cout << "Can't apply VideoMode: " << OpenNI::getExtendedError() << endl;
		}
		else
		{
			cerr << "Can't create depth stream on device: " << OpenNI::getExtendedError() << endl;
			system("PAUSE");
			return -1;
		}
	}
	else
	{
		cerr << "ERROR: This device does not have depth sensor" << endl;
	}

	// create OpenCV Window and start
	if (vsDepth.isValid())
	{
		cv::namedWindow("Depth Image", CV_WINDOW_AUTOSIZE);
		vsDepth.start();
	}
#pragma endregion

	
 
#pragma region device2
	Device	devDevice_2;
	if (devDevice_2.open(deviceInfoList[1].getUri()) != STATUS_OK)
	{
		cerr << "Can't Open Device: " << OpenNI::getExtendedError() << endl;
		return -1;
	}

	// Create depth stream
	VideoStream vsDepth2;
	if (devDevice_2.hasSensor(SENSOR_DEPTH))
	{
		if (vsDepth2.create(devDevice_2, SENSOR_DEPTH) == STATUS_OK)
		{
			// set video mode
			VideoMode mMode;
			mMode.setResolution(640, 480);
			mMode.setFps(30);
			mMode.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);

			if (vsDepth2.setVideoMode(mMode) != STATUS_OK)
				cout << "Can't apply VideoMode: " << OpenNI::getExtendedError() << endl;
		}
		else
		{
			cerr << "Can't create depth stream on device: " << OpenNI::getExtendedError() << endl;
			system("PAUSE");
			return -1;
		}
	}
	else
	{
		cerr << "ERROR: This device does not have depth sensor" << endl;
	}

	// create OpenCV Window and start
	if (vsDepth2.isValid())
	{
		cv::namedWindow("Depth Image2", CV_WINDOW_AUTOSIZE);
		vsDepth2.start();
	}
#pragma endregion

 
#pragma region device3
	Device	devDevice_3;
	if (devDevice_3.open(deviceInfoList[2].getUri()) != STATUS_OK)
	{
		cerr << "Can't Open Device: " << OpenNI::getExtendedError() << endl;
		return -1;
	}

	// Create depth stream
	VideoStream vsDepth3;
	if (devDevice_3.hasSensor(SENSOR_DEPTH))
	{
		if (vsDepth3.create(devDevice_3, SENSOR_DEPTH) == STATUS_OK)
		{
			// set video mode
			VideoMode mMode;
			mMode.setResolution(640, 480);
			mMode.setFps(30);
			mMode.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);

			if (vsDepth3.setVideoMode(mMode) != STATUS_OK)
				cout << "Can't apply VideoMode: " << OpenNI::getExtendedError() << endl;
		}
		else
		{
			cerr << "Can't create depth stream on device: " << OpenNI::getExtendedError() << endl;
			system("PAUSE");
			return -1;
		}
	}
	else
	{
		cerr << "ERROR: This device does not have depth sensor" << endl;
	}

	// create OpenCV Window and start
	if (vsDepth3.isValid())
	{
		cv::namedWindow("Depth Image3", CV_WINDOW_AUTOSIZE);
		vsDepth3.start();
	}
#pragma endregion


#pragma region device4
	Device	devDevice_4;
	if (devDevice_4.open(deviceInfoList[3].getUri()) != STATUS_OK)
	{
		cerr << "Can't Open Device: " << OpenNI::getExtendedError() << endl;
		return -1;
	}

	// Create depth stream
	VideoStream vsDepth4;
	if (devDevice_4.hasSensor(SENSOR_DEPTH))
	{
		if (vsDepth4.create(devDevice_4, SENSOR_DEPTH) == STATUS_OK)
		{
			// set video mode
			VideoMode mMode;
			mMode.setResolution(640, 480);
			mMode.setFps(40);
			mMode.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);

			if (vsDepth4.setVideoMode(mMode) != STATUS_OK)
				cout << "Can't apply VideoMode: " << OpenNI::getExtendedError() << endl;
		}
		else
		{
			cerr << "Can't create depth stream on device: " << OpenNI::getExtendedError() << endl;
			system("PAUSE");
			return -1;
		}
	}
	else
	{
		cerr << "ERROR: This device does not have depth sensor" << endl;
	}

	// create OpenCV Window and start
	if (vsDepth4.isValid())
	{
		cv::namedWindow("Depth Image4", CV_WINDOW_AUTOSIZE);
		vsDepth3.start();
	}
#pragma endregion
	// get max depth value
	int iMaxDepth = vsDepth.getMaxPixelValue();
	while (true){
		// check is depth stream is available
		if (vsDepth.isValid())
		{
			// get depth frame
			VideoFrameRef vfDepthFrame;
			if (vsDepth.readFrame(&vfDepthFrame) == STATUS_OK)
			{
				// convert data to OpenCV format
				const cv::Mat mImageDepth(vfDepthFrame.getHeight(), vfDepthFrame.getWidth(), CV_16UC1, const_cast<void*>(vfDepthFrame.getData()));

				// re-map depth data [0,Max] to [0,255]
				cv::Mat mScaledDepth;
				mImageDepth.convertTo(mScaledDepth, CV_8U, 255.0 / iMaxDepth);

				// show image
				cv::imshow("Depth Image", mScaledDepth);

				vfDepthFrame.release();
			}
		}
		if (vsDepth2.isValid() )
		{
			// get depth frame
			VideoFrameRef vfDepthFrame2;
			if (vsDepth2.readFrame(&vfDepthFrame2) == STATUS_OK)
			{
				// convert data to OpenCV format
				const cv::Mat mImageDepth(vfDepthFrame2.getHeight(), vfDepthFrame2.getWidth(), CV_16UC1, const_cast<void*>(vfDepthFrame2.getData()));

				// re-map depth data [0,Max] to [0,255]
				cv::Mat mScaledDepth;
				mImageDepth.convertTo(mScaledDepth, CV_8U, 255.0 / iMaxDepth);

				// show image
				cv::imshow("Depth Image2", mScaledDepth);

				vfDepthFrame2.release();
			}
		}
		if (vsDepth3.isValid() )
		{
			// get depth frame
			VideoFrameRef vfDepthFrame3;
			if (vsDepth3.readFrame(&vfDepthFrame3) == STATUS_OK)
			{
				// convert data to OpenCV format
				const cv::Mat mImageDepth(vfDepthFrame3.getHeight(), vfDepthFrame3.getWidth(), CV_16UC1, const_cast<void*>(vfDepthFrame3.getData()));

				// re-map depth data [0,Max] to [0,255]
				cv::Mat mScaledDepth;
				mImageDepth.convertTo(mScaledDepth, CV_8U, 255.0 / iMaxDepth);

				// show image
				cv::imshow("Depth Image3", mScaledDepth);

				vfDepthFrame3.release();
			}
		}
		if (vsDepth4.isValid() )
		{
			// get depth frame
			VideoFrameRef vfDepthFrame4;
			if (vsDepth4.readFrame(&vfDepthFrame4) == STATUS_OK)
			{
				// convert data to OpenCV format
				const cv::Mat mImageDepth(vfDepthFrame4.getHeight(), vfDepthFrame4.getWidth(), CV_16UC1, const_cast<void*>(vfDepthFrame4.getData()));

				// re-map depth data [0,Max] to [0,255]
				cv::Mat mScaledDepth;
				mImageDepth.convertTo(mScaledDepth, CV_8U, 255.0 / iMaxDepth);

				// show image
				cv::imshow("Depth Image4", mScaledDepth);

				vfDepthFrame4.release();
			}
		}
		// check keyboard
		if (cv::waitKey(1) == 'q')
			break;
	
	}
	vsDepth.stop();
	vsDepth2.stop();
	vsDepth3.stop();
	vsDepth4.stop();
	vsDepth.destroy();
	vsDepth2.destroy();
	vsDepth3.destroy();
	vsDepth4.destroy();
	devDevice_1.close();
	devDevice_2.close();
	devDevice_3.close();
	devDevice_4.close();
	OpenNI::shutdown();
	system("PAUSE");

}