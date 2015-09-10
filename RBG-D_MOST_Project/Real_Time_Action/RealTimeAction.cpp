//===========================================================================================================
//= 程式撰寫人 韓雲  ， 2015/09聖方修改                                                                             =
//= 即時動作識別：識別的動作是（走、跌倒、站起來、坐下、躺下、彎腰）。                                      =
//=                                                                                                         =
//===========================================================================================================
//通過連續採集最初20幀的骨架資料來確定：頭部和質心的初始位置（主要是高度）以及跌倒的閾值參數。2014年4月16日。
#include "Header.h";
int main(int argc, char **argv)
{
#pragma region Initial Variable
	time_t zuo = clock();//坐下 時刻
	time_t die = clock();//跌倒 時刻
	time_t zql = clock();//坐起來 時刻
	time_t zanql = clock();//站起來 時刻
	time_t firsttime = clock();//程式啟動 時刻

	time_t  zuoq = clock(); //上一次坐下 時刻
	time_t  zqlq = clock(); //上一次坐起來  表示上次的值。

	int armLen = 0; //手臂張開的長度
	int DSTimes = 0; //偵測的初始骨架次數。
	int Nowstate = 0;
	int nextstate = 0;//定義狀態變數,狀態都是未知的；
	//1：站立；2：走著；3：跌倒；4：坐下；5：躺； 其他均為非定義狀態。

	//ftorso_Ini初始值；ftorso：當前的值；ftorsoC:當前的前一時刻的值
	cv::Point3f ftorso_Ini, ftorso, ftorsoC;
	//fhead_Ini初始值；fhead：當前的值；fheadC:當前的前一時刻的值
	cv::Point3f fhead_Ini, fhead, fheadC;
	cv::Point3f  neck; //脖子

	//用於初始統計連續N幀的平均的骨架資料，用於初始高度及跌倒參數確定。
	cv::Point3f fheadf, ftorsof, leftsf, rightsf;

	cv::Point3f lefthand, righthand;

	int weizhi = 0; //狀態是否未知的識別字,初始狀態是未知的.0表示是未知的。
	int event_Now = 0;//事件 當下事件
	int event_Pre = 0; //事件 上一次發生的事件。


	int cishu = 0;//檢測的次數；
	int detectFlag = 0;//該變數表示是否是第一次檢測到骨架(實際上是假定第一次偵測到骨架時，人是站立的。目的是獲取人的高度)。

#pragma endregion

#pragma region Initial NITE&CV
	// Initial NiTE
	if (NiTE::initialize() != STATUS_OK)
	{
		cerr << "NiTE initial error" << endl;
		return -1;
	}

	// create user tracker
	UserTracker mUserTracker;
	if (mUserTracker.create() != STATUS_OK)
	{
		cerr << "Can't create user tracker" << endl;
		return -1;
	}
	mUserTracker.setSkeletonSmoothingFactor(0.8);

	// create OpenCV Window
	cv::namedWindow("User Image", CV_WINDOW_AUTOSIZE);

#pragma endregion

	// start
	while (true)
	{
		// get user frame
		UserTrackerFrameRef	mUserFrame;
		if (mUserTracker.readFrame(&mUserFrame) == nite::STATUS_OK)
		{
			// get depth data and convert to OpenCV format
			openni::VideoFrameRef vfDepthFrame = mUserFrame.getDepthFrame();
			const cv::Mat mImageDepth(vfDepthFrame.getHeight(), vfDepthFrame.getWidth(), CV_16UC1, const_cast<void*>(vfDepthFrame.getData()));
			// re-map depth data [0,Max] to [0,255]
			cv::Mat mScaledDepth;
			mImageDepth.convertTo(mScaledDepth, CV_8U, 255.0 / 10000);

			// convert gray-scale to color
			cv::Mat mImageBGR;
			cv::cvtColor(mScaledDepth, mImageBGR, CV_GRAY2BGR);

			// get users data
			const nite::Array<UserData>& aUsers = mUserFrame.getUsers();
			for (int i = 0; i < aUsers.getSize(); ++i)
			{
				//獲取地板
				//NitePoint3f planePoint, planeNormal;
				//planePoint = mUserFrame.getFloor().point;
			    //planeNormal = mUserFrame.getFloor().normal;

				const UserData& rUser = aUsers[i];

				// check user status
				if (rUser.isNew())
				{
					cout << "New User [" << rUser.getId() << "] found." << endl;
					//  start tracking for new user
					mUserTracker.setSkeletonSmoothingFactor(0.65); //平滑控制。
					mUserTracker.startSkeletonTracking(rUser.getId());
				}
				else if (rUser.isLost())
				{
					cout << "User [" << rUser.getId() << "] lost." << endl;
				}

				if (rUser.isVisible())
				{
					// get user skeleton
					const Skeleton& rSkeleton = rUser.getSkeleton();
					if (rSkeleton.getState() == SKELETON_TRACKED)
					{
#pragma region DrawSkeletonLineAndCircle
						// build joints array
						nite::SkeletonJoint aJoints[15];
						aJoints[0] = rSkeleton.getJoint(JOINT_HEAD);
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

						// convert joint position to image
						cv::Point2f aPoint[15];
						for (int s = 0; s < 15; ++s)
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
						for (int s = 0; s < 15; ++s)
						{
							if (aJoints[s].getPositionConfidence() > 0.5)
								cv::circle(mImageBGR, aPoint[s], 3, cv::Scalar(0, 0, 255), 2);
							else
								cv::circle(mImageBGR, aPoint[s], 3, cv::Scalar(0, 255, 0), 2);
						}
#pragma endregion

						//獲取初始值
						if (detectFlag == 0)
						{
							DSTimes++;
							if (DSTimes == 1)
							{
								fheadf.x = aJoints[0].getPosition().x;//站立時的頭的位置；
								fheadf.y = aJoints[0].getPosition().y;
								fheadf.z = aJoints[0].getPosition().z;

								ftorsof.x = aJoints[8].getPosition().x;//站立時的質心位置；
								ftorsof.y = aJoints[8].getPosition().y;
								ftorsof.z = aJoints[8].getPosition().z;

								leftsf.x = aJoints[2].getPosition().x;
								leftsf.y = aJoints[2].getPosition().y;
								leftsf.z = aJoints[2].getPosition().z;

								rightsf.x = aJoints[3].getPosition().x;
								rightsf.y = aJoints[3].getPosition().y;
								rightsf.z = aJoints[3].getPosition().z;

								lefthand.x = aJoints[6].getPosition().x;
								lefthand.y = aJoints[6].getPosition().y;
								lefthand.z = aJoints[6].getPosition().z;

								righthand.x = aJoints[7].getPosition().x;
								righthand.y = aJoints[7].getPosition().y;
								righthand.z = aJoints[7].getPosition().z;
								//為什麼要乘0.55?
								armLen = sqrt((lefthand.x - righthand.x)*(lefthand.x - righthand.x) +
									(lefthand.y - righthand.y)*(lefthand.y - righthand.y) + 
									(lefthand.z - righthand.z)*(lefthand.z - righthand.z))*0.55;
							}
							else if (DSTimes>1 && DSTimes <= 200)	//偵測次數於200次以內
							{
								fheadf.x = fheadf.x + aJoints[0].getPosition().x;//站立時的頭的位置；
								fheadf.y = fheadf.y + aJoints[0].getPosition().y;
								fheadf.z = fheadf.z + aJoints[0].getPosition().z;

								ftorsof.x = ftorsof.x + aJoints[8].getPosition().x;//站立時的質心位置；
								ftorsof.y = ftorsof.y + aJoints[8].getPosition().y;
								ftorsof.z = ftorsof.z + aJoints[8].getPosition().z;

								leftsf.x = aJoints[2].getPosition().x;
								leftsf.y = aJoints[2].getPosition().y;
								leftsf.z = aJoints[2].getPosition().z;

								rightsf.x = aJoints[3].getPosition().x;
								rightsf.y = aJoints[3].getPosition().y;
								rightsf.z = aJoints[3].getPosition().z;

								lefthand.x = aJoints[6].getPosition().x;
								lefthand.y = aJoints[6].getPosition().y;
								lefthand.z = aJoints[6].getPosition().z;

								righthand.x = aJoints[7].getPosition().x;
								righthand.y = aJoints[7].getPosition().y;
								righthand.z = aJoints[7].getPosition().z;
								//?????
								int cc = sqrt((lefthand.x - righthand.x)*(lefthand.x - righthand.x) + 
									(lefthand.y - righthand.y)*(lefthand.y - righthand.y) + 
									(lefthand.z - righthand.z)*(lefthand.z - righthand.z))*0.55;

								cout << "測試下身長：" << cc << endl;
								armLen = armLen + cc;
							}
							if (DSTimes >= 200)//計算頭、身初始值、跌倒閥值
							{
								detectFlag = 1;

								ftorso_Ini.x = ftorsof.x / DSTimes;
								ftorso_Ini.y = ftorsof.y / DSTimes;
								ftorso_Ini.z = ftorsof.z / DSTimes;

								fhead_Ini.x = fheadf.x / DSTimes;
								fhead_Ini.y = fheadf.y / DSTimes;
								fhead_Ini.z = fheadf.z / DSTimes;

								dieyuzhi = ((int)((armLen / DSTimes) / 100) * 100);

								cout << "跌倒的閾值：" << dieyuzhi << endl;
							}

							fhead.x = aJoints[JOINT_HEAD].getPosition().x;
							fhead.y = aJoints[JOINT_HEAD].getPosition().y;
							fhead.z = aJoints[JOINT_HEAD].getPosition().z;
							fheadC = fhead;

							ftorso.x = aJoints[JOINT_TORSO].getPosition().x;
							ftorso.y = aJoints[JOINT_TORSO].getPosition().y;
							ftorso.z = aJoints[JOINT_TORSO].getPosition().z;
							ftorsoC = ftorso;

							cout << "第" << DSTimes << "次" << "的頭部h：" << fhead.y << endl;
						}
						else
						{  
							if (cishu == 8)
							{
								cishu = 0;

								ftorso.x = aJoints[8].getPosition().x;
								ftorso.y = aJoints[8].getPosition().y;
								ftorso.z = aJoints[8].getPosition().z;

								fhead.x = aJoints[0].getPosition().x;
								fhead.y = aJoints[0].getPosition().y;
								fhead.z = aJoints[0].getPosition().z;

								neck.x = aJoints[1].getPosition().x;
								neck.y = aJoints[1].getPosition().y;
								neck.z = aJoints[1].getPosition().z;

								event_Now = eventDetect(fheadC, ftorsoC, fhead, ftorso, fhead_Ini, ftorso_Ini, neck);//(前一幀,後一幀,初始幀)

								//統計時間
								if (event_Now == INCIDENT_SIT_DOWN)
									zuo = clock();
								if (event_Now == INCIDENT_FALLDOWN)
									die = clock();
								if (event_Now == INCIDENT_SIT_UP)
									zql = clock();
								if (event_Now == INCIDENT_STANDUP)
									zanql = clock();

								if (event_Now == INCIDENT_FALLDOWN && difftime(zuo, firsttime)>0 && difftime(zuoq, zuo) != 0)
								{
									automendG(event_Now, difftime(die, zuo));
									zuoq = zuo;
								}

								if (event_Now == INCIDENT_STANDUP && difftime(zql, firsttime)>0 && difftime(zqlq, zql) != 0)
								{
									automendG(event_Now, difftime(zanql, zql));
									zqlq = zql;
								}

								
#pragma region 用於調試的，直接顯示出事件偵測。
								if (event_Pre != event_Now)
								{
									if (event_Now == INCIDENT_NONE)
										cout << "沒偵測到事件發生！" << endl;
									if (event_Now == INCIDENT_STOP)
										cout << "發生的事件: " << "停止" << endl;
									if (event_Now == INCIDENT_WALK)
										cout << "發生的事件: " << "走" << endl;
									if (event_Now == INCIDENT_SIT_UP)
										cout << "發生的事件: " << "坐起來" << endl;
									if (event_Now == INCIDENT_STANDUP)
										cout << "發生的事件: " << "站起來" << endl;
									if (event_Now == INCIDENT_FALLDOWN)
										cout << "發生的事件: " << "跌倒" << endl;
									if (event_Now == INCIDENT_SIT_DOWN)
										cout << "發生的事件: " << "坐下" << endl;
									if (event_Now == INCIDENT_LAYDOWN)
										cout << "發生的事件: " << "躺下" << endl;
								}
#pragma endregion


								event_Pre = event_Now;

								if (weizhi == 0)
								{
									nextstate = firststate(event_Now);
									if (nextstate != 0)
										weizhi = 1;
									if (nextstate != Nowstate)
									{
										if (nextstate == 1)
											cout << "目前狀態: " << "站著" << endl;
										if (nextstate == 2)
											cout << "目前狀態: " << "走著" << endl;
										if (nextstate == 3)
											cout << "目前狀態: " << "跌倒" << endl;
										if (nextstate == 4)
											cout << "目前狀態: " << "坐著" << endl;
										if (nextstate == 5)
											cout << "目前狀態: " << "躺著" << endl;
									}
									Nowstate = nextstate;
								}
								else if (weizhi == 1)
								{
									if (event_Now != 0)
									{
										if (unnormalzhenc(event_Now, Nowstate) == 1);
										{
											weizhi = 0;
										} 
										nextstate = stateTransfer(event_Now, Nowstate);
									}

									if (nextstate != Nowstate)
									{
										if (nextstate == 1)
											cout << "目前狀態: " << "站著" << endl;
										if (nextstate == 2)
											cout << "目前狀態: " << "走著" << endl;
										if (nextstate == 3)
											cout << "目前狀態: " << "跌倒" << endl;
										if (nextstate == 4)
											cout << "目前狀態: " << "坐著" << endl;
										if (nextstate == 5)
											cout << "目前狀態: " << "躺著" << endl;
									}
									Nowstate = nextstate;
								}
								ftorsoC = ftorso;
								fheadC = fhead;
							}
							cishu++;
						}
					}
				}
			}

			// show image
			cv::imshow("User Image", mImageBGR);


			mUserFrame.release();
		}
		else
		{
			cerr << "Can't get user frame" << endl;
		}

		// check keyboard
		if (cv::waitKey(1) == 'q')
			break;
	}

	// stop

	mUserTracker.destroy();
	NiTE::shutdown();

	return 0;
}
