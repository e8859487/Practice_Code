#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include <iostream>
#include <fstream>
#include <time.h>
#include <string>

// OpenCV Header
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// NiTE Header
#include <NiTE.h>

// OpenNI Header
#include <OpenNI.h>
using namespace std;
using namespace nite;
//using namespace openni;
string skeletonNUMsStr = "";
int Track2PeopleFromCam();//從相機讀取骨架資訊
int Track2PeopleFromOni();//從ONI讀取骨架資訊

int main(){
	return Track2PeopleFromOni();
}
void OutputAllUsers(const nite::Array<UserData>& aUsers){
	cout << "current USERS : ";
	for (int i = 0; i < aUsers.getSize(); i++){
		cout << aUsers[i].getId();
	}
	cout << endl;
}
int Track2PeopleFromOni(){
	char *filename = "Recoder07_Intersect.oni";
#pragma region INIT
	//Initial Openni
	if (openni::OpenNI::initialize() != STATUS_OK){
		cerr << "OpenNI Initial Error: " << openni::OpenNI::getExtendedError() << endl;
		std::system("PAUSE");
		return -1;
	}
	//Open Device
	openni::Device devDevice;
	if (devDevice.open(openni::ANY_DEVICE) != STATUS_OK)
	{
		cerr << "Can't Open the file: " << filename << "\n" << openni::OpenNI::getExtendedError() << endl;
		std::system("PAUSE");
		return -1;
	}

	// Initial NiTE
	if (NiTE::initialize() != STATUS_OK){
		cerr << "NiTE initial error" << endl;
		std::system("PAUSE");
		return -1;
	}

	// create user tracker
	UserTracker mUserTracker;
	if (mUserTracker.create(&devDevice) != STATUS_OK){
		cerr << "Can't create user tracker" << endl;
		std::system("PAUSE");
		return -1;
	}

	cv::namedWindow("Skeleton_Image", CV_WINDOW_AUTOSIZE);
	std::cout << "start detect" << endl;
#pragma endregion

	int SkeletonNums = 0;
	while (true){
		//system("cls");

		// get user frame
		UserTrackerFrameRef mUserFrame;
		if (mUserTracker.readFrame(&mUserFrame) == nite::STATUS_OK){

			openni::VideoFrameRef vfDephFrame = mUserFrame.getDepthFrame();
			const cv::Mat mImageDepth(vfDephFrame.getHeight(), vfDephFrame.getWidth(), CV_16UC1, const_cast<void*>(vfDephFrame.getData()));
			// re-map depth data [0,Max] to [0,255]
			cv::Mat mScaleDepth;
			mImageDepth.convertTo(mScaleDepth, CV_8U, 255.0 / 10000);

			//convert gray-scale to color
			cv::Mat mImageBGR;
			cv::cvtColor(mScaleDepth, mImageBGR, CV_GRAY2BGR);

			//get users data
			const nite::Array<UserData>& aUsers = mUserFrame.getUsers();
//			int num = aUsers.getSize() > 1 ? 1 : aUsers.getSize();	//限定使用者數量<=2人
		
			for (int i = 0; i < aUsers.getSize(); ++i){
				cout << i;
				const UserData& rUser = aUsers[i];
				// check user status

				if (rUser.isNew()){
					//std::this_thread::sleep_for(std::chrono::milliseconds(500));
					cout << "New User [" << rUser.getId() << "] found." << endl;
					if (SkeletonNums < 2){
						mUserTracker.startSkeletonTracking(rUser.getId());
						cout << "start Tracking [" << rUser.getId() << "] skeleton." << endl;
						OutputAllUsers(aUsers);
						
					//	SkeletonNums++;
						skeletonNUMsStr = skeletonNUMsStr + to_string(rUser.getId());
					}
				}

				if (rUser.isLost()){
					std::this_thread::sleep_for(std::chrono::milliseconds(500));

					cout << "User [" << rUser.getId() << "] lost." << endl;
					OutputAllUsers(aUsers);
					skeletonNUMsStr.find()
				}
				const int SkeletonSize = 9;//只顯示身體9個座標點
				if (rUser.isVisible()){
					const Skeleton& rSkeleton = rUser.getSkeleton();
					if (rSkeleton.getState() == SKELETON_TRACKED){
						// build joints array
#pragma region Draw_Line_Circle_USRID
						nite::SkeletonJoint aJoints[SkeletonSize];
						aJoints[0] = rSkeleton.getJoint(JOINT_HEAD);
						aJoints[1] = rSkeleton.getJoint(JOINT_NECK);
						aJoints[2] = rSkeleton.getJoint(JOINT_LEFT_SHOULDER);
						aJoints[3] = rSkeleton.getJoint(JOINT_RIGHT_SHOULDER);
						aJoints[4] = rSkeleton.getJoint(JOINT_LEFT_ELBOW);
						aJoints[5] = rSkeleton.getJoint(JOINT_RIGHT_ELBOW);
						aJoints[6] = rSkeleton.getJoint(JOINT_LEFT_HAND);
						aJoints[7] = rSkeleton.getJoint(JOINT_RIGHT_HAND);
						aJoints[8] = rSkeleton.getJoint(JOINT_TORSO);
						//aJoints[9] = rSkeleton.getJoint(JOINT_LEFT_HIP);
						//aJoints[10] = rSkeleton.getJoint(JOINT_RIGHT_HIP);
						//aJoints[11] = rSkeleton.getJoint(JOINT_LEFT_KNEE);
						//aJoints[12] = rSkeleton.getJoint(JOINT_RIGHT_KNEE);
						//aJoints[13] = rSkeleton.getJoint(JOINT_LEFT_FOOT);
						//aJoints[14] = rSkeleton.getJoint(JOINT_RIGHT_FOOT);  

						cv::Point2f aPoint[SkeletonSize];
						for (int s = 0; s < SkeletonSize; ++s)
						{
							const Point3f& rPos = aJoints[s].getPosition();
							mUserTracker.convertJointCoordinatesToDepth(rPos.x, rPos.y, rPos.z, &(aPoint[s].x), &(aPoint[s].y));
						}

						// draw line
						cv::line(mImageBGR, aPoint[0], aPoint[1], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[1], aPoint[2], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[1], aPoint[3], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[2], aPoint[4], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[3], aPoint[5], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[4], aPoint[6], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[5], aPoint[7], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[1], aPoint[8], cv::Scalar(255, 0, 0), 3);
						//cv::line(mImageBGR, aPoint[8], aPoint[9], cv::Scalar(255, 0, 0), 3);
						//cv::line(mImageBGR, aPoint[8], aPoint[10], cv::Scalar(255, 0, 0), 3);
						//cv::line(mImageBGR, aPoint[9], aPoint[11], cv::Scalar(255, 0, 0), 3);
						//cv::line(mImageBGR, aPoint[10], aPoint[12], cv::Scalar(255, 0, 0), 3);
						//cv::line(mImageBGR, aPoint[11], aPoint[13], cv::Scalar(255, 0, 0), 3);
						//cv::line(mImageBGR, aPoint[12], aPoint[14], cv::Scalar(255, 0, 0), 3);
						// draw joint
						for (int s = 0; s < SkeletonSize; ++s){
							if (aJoints[s].getPositionConfidence() > 0.5)
								cv::circle(mImageBGR, aPoint[s], 3, cv::Scalar(0, 0, 255), 2);
							else
								cv::circle(mImageBGR, aPoint[s], 3, cv::Scalar(0, 255, 0), 2);
						}
						//draw user id
						cv::putText(mImageBGR, "USER" + to_string(rUser.getId()), aPoint[0], cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(255, 255, 255));
#pragma endregion
					}
				}

			}//end for


			cv::imshow("Skeleton_Image", mImageBGR);
			mUserFrame.release();
		}
		else{
			cerr << "Can't get user frame or finished";
			std::system("PAUSE");
			break;
		}

		// check keyboard
		if (cv::waitKey(1) == 'q'){
			cv::destroyAllWindows();
			break;
		}
	}
	// stop
	std::system("PAUSE");

	mUserTracker.destroy();
	devDevice.close();
	openni::OpenNI::shutdown();
	NiTE::shutdown();
	std::cout << "done!!";
	return 0;
}



int Recoard2PeopleFromCam(){
	// Initial NiTE
	if (NiTE::initialize() != STATUS_OK){
		cerr << "NiTE initial error" << endl;
		std::system("PAUSE");
		return -1;
	}
	// create user tracker
	UserTracker mUserTracker;
	if (mUserTracker.create() != STATUS_OK){
		cerr << "Can't create user tracker" << endl;
		std::system("PAUSE");
		return -1;
	}

	cv::namedWindow("Skeleton_Image", CV_WINDOW_AUTOSIZE);
	cout << "start detect" << endl;

	while (true){
		// get user frame
		UserTrackerFrameRef mUserFrame;
		if (mUserTracker.readFrame(&mUserFrame) == nite::STATUS_OK){
			openni::VideoFrameRef vfDephFrame = mUserFrame.getDepthFrame();
			const cv::Mat mImageDepth(vfDephFrame.getHeight(), vfDephFrame.getWidth(), CV_16UC1, const_cast<void*>(vfDephFrame.getData()));
			// re-map depth data [0,Max] to [0,255]
			cv::Mat mScaleDepth;
			mImageDepth.convertTo(mScaleDepth, CV_8U, 255.0 / 10000);

			//convert gray-scale to color
			cv::Mat mImageBGR;
			cv::cvtColor(mScaleDepth, mImageBGR, CV_GRAY2BGR);

			//get users data
			const nite::Array<UserData>& aUsers = mUserFrame.getUsers();
			for (int i = 0; i < aUsers.getSize(); ++i){
				const UserData& rUser = aUsers[i];
				// check user status

				if (rUser.isNew()){
					cout << "New User [" << rUser.getId() << "] found." << endl;
					if (rUser.getId() == 0 || rUser.getId() == 1)
					{
						mUserTracker.startSkeletonTracking(rUser.getId());
						cout << "start Tracking [" << rUser.getId() << "] skeleton." << endl;
					}
				}

				else if (rUser.isLost()){
					cout << "User [" << rUser.getId() << "] lost." << endl;
				}
				const int SkeletonSize = 15;
				if (rUser.isVisible() && (rUser.getId() == 0 || rUser.getId() == 1)){
					const Skeleton& rSkeleton = rUser.getSkeleton();
					if (rSkeleton.getState() == SKELETON_TRACKED){
						// build joints array
						nite::SkeletonJoint aJoints[SkeletonSize];
						aJoints[0] = rSkeleton.getJoint(nite::JOINT_HEAD);
						aJoints[1] = rSkeleton.getJoint(JOINT_NECK);
						aJoints[2] = rSkeleton.getJoint(JOINT_LEFT_SHOULDER);
						aJoints[3] = rSkeleton.getJoint(JOINT_RIGHT_SHOULDER);
						aJoints[4] = rSkeleton.getJoint(JOINT_LEFT_ELBOW);
						aJoints[5] = rSkeleton.getJoint(JOINT_RIGHT_ELBOW);
						aJoints[6] = rSkeleton.getJoint(JOINT_LEFT_HAND);
						aJoints[7] = rSkeleton.getJoint(JOINT_RIGHT_HAND);
						aJoints[8] = rSkeleton.getJoint(JOINT_TORSO);
						aJoints[9] = rSkeleton.getJoint(JOINT_LEFT_HIP);
						aJoints[10] = rSkeleton.getJoint(JOINT_RIGHT_HIP);
						aJoints[11] = rSkeleton.getJoint(JOINT_LEFT_KNEE);
						aJoints[12] = rSkeleton.getJoint(JOINT_RIGHT_KNEE);
						aJoints[13] = rSkeleton.getJoint(JOINT_LEFT_FOOT);
						aJoints[14] = rSkeleton.getJoint(JOINT_RIGHT_FOOT);
						cv::Point2f aPoint[SkeletonSize];
						for (int s = 0; s < SkeletonSize; ++s)
						{
							const Point3f& rPos = aJoints[s].getPosition();
							mUserTracker.convertJointCoordinatesToDepth(rPos.x, rPos.y, rPos.z, &(aPoint[s].x), &(aPoint[s].y));
						}

						// draw line
						cv::line(mImageBGR, aPoint[0], aPoint[1], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[1], aPoint[2], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[1], aPoint[3], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[2], aPoint[4], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[3], aPoint[5], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[4], aPoint[6], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[5], aPoint[7], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[1], aPoint[8], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[8], aPoint[9], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[8], aPoint[10], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[9], aPoint[11], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[10], aPoint[12], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[11], aPoint[13], cv::Scalar(255, 0, 0), 3);
						cv::line(mImageBGR, aPoint[12], aPoint[14], cv::Scalar(255, 0, 0), 3);
						// draw joint
						for (int s = 0; s < SkeletonSize; ++s){
							if (aJoints[s].getPositionConfidence() > 0.5)
								cv::circle(mImageBGR, aPoint[s], 3, cv::Scalar(0, 0, 255), 2);
							else
								cv::circle(mImageBGR, aPoint[s], 3, cv::Scalar(0, 255, 0), 2);
						}
					}
				}

			}//end for
			cv::imshow("Skeleton_Image", mImageBGR);
			mUserFrame.release();

 
		}
		else{
			cerr << "Can't get user frame";
		}

		// check keyboard
		if (cv::waitKey(1) == 'q'){
			cv::destroyAllWindows();
			break;
		}

	}
	// stop
	mUserTracker.destroy();
	NiTE::shutdown();

	system("pause");
}
