// STL Header
#include <iostream>
#include <string>
#include <vector>

// OpenCV Header
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// OpenNI Header
#include <OpenNI.h>

// namespace
using namespace std;
using namespace openni;

class CDevice
{
public:
	string        sWindow;
	Device*       pDevice;
	VideoStream*  pDepthStream;

	CDevice(int idx, const char* uri)
	{
		pDevice = new Device();
		pDevice->open(uri);

		pDepthStream = new VideoStream();
		pDepthStream->create(*pDevice, SENSOR_DEPTH);

		// create OpenCV Window
		stringstream mStrStream;
		mStrStream << "Sensor_" << idx << endl;
		sWindow = mStrStream.str();
		cv::namedWindow(sWindow.c_str(), CV_WINDOW_AUTOSIZE);

		// start
		pDepthStream->start();
	}
};

int main(int argc, char **argv)
{
	// Initial OpenNI
	if (OpenNI::initialize() != STATUS_OK){
		cerr << "OpenNi Initial Error : " << OpenNI::getExtendedError() << endl;
		system("PAUSE");
		return -1;
	}

	// enumerate devices
	Array<DeviceInfo> aDeviceList;
	OpenNI::enumerateDevices(&aDeviceList);

	// output information
	vector<CDevice>  vDevices;
	cout << "There are " << aDeviceList.getSize()
		<< " devices on this system." << endl;
	for (int i = 0; i < aDeviceList.getSize(); ++i)
	{
		cout << "Device " << i << "\n";
		const DeviceInfo& rDevInfo = aDeviceList[i];

		cout << " " << rDevInfo.getName() << " by " << rDevInfo.getVendor() << "\n";
		cout << " PID: " << rDevInfo.getUsbProductId() << "\n";
		cout << " VID: " << rDevInfo.getUsbVendorId() << "\n";
		cout << " URI: " << rDevInfo.getUri() << endl;

		// initialize
		CDevice mDevWin(i, aDeviceList[i].getUri());
		vDevices.push_back(mDevWin);
	}

	while (true)
	{
		for (vector<CDevice>::iterator itDev = vDevices.begin();
			itDev != vDevices.end(); ++itDev)
		{
			// get depth frame
			VideoFrameRef vfFrame;
			itDev->pDepthStream->readFrame(&vfFrame);

			// convert data to OpenCV format
			const cv::Mat mImageDepth(vfFrame.getHeight(), vfFrame.getWidth(),
				CV_16UC1,
				const_cast<void*>(vfFrame.getData()));

			// re-map depth data [0,Max] to [0,255]
			cv::Mat mScaledDepth;
			mImageDepth.convertTo(mScaledDepth, CV_8U,
				255.0 / itDev->pDepthStream->getMaxPixelValue());

			// show image
			cv::imshow(itDev->sWindow.c_str(), mScaledDepth);

			vfFrame.release();
		}

		// check keyboard
		if (cv::waitKey(1) == 'q')
			break;
	}

	// stop
	for (vector<CDevice>::iterator itDev = vDevices.begin();
		itDev != vDevices.end(); ++itDev)
	{
		itDev->pDepthStream->stop();
		itDev->pDepthStream->destroy();
		delete itDev->pDepthStream;

		itDev->pDevice->close();
		delete itDev->pDevice;
	}
	OpenNI::shutdown();

	return 0;
}