#ifndef mainHeader
#define mainHeader 
// STL Header
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "time.h"
#include "stdio.h"

// OpenCV Header
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// NiTE Header
#include <NiTE.h>

// namespace
using namespace std;
using namespace nite; 
//跌倒的閾值 應該會被蓋掉?
int fallDownThreshold = 700;

//動作事件1:停止 2走 3坐起來 4站起來 5跌倒 6坐下 7躺下 
typedef enum {
	INCIDENT_NONE,//0
	INCIDENT_STOP,//1
	INCIDENT_WALK,//2
	INCIDENT_SIT_UP,//3
	INCIDENT_STANDUP,//4
	INCIDENT_FALLDOWN,//5
	INCIDENT_SIT_DOWN,//6
	INCIDENT_LAYDOWN,//7
} MovementIncident;

//狀態 1：站立；2：走著；3：跌倒；4：坐下；5：躺；
typedef enum 
{
	STATUS_NONE,//0
	STATUS_STAND,//1
	STATUS_WALK,//2
	STATUS_FAILDOWN,//3
	STATUS_SITDOWN,//4
	STATUS_LAYDOWN,//5
} STATUS;


//偵測人體關節點距離地面的高度。
float cegao(NitePoint3f planePoint, NitePoint3f planeNormal, float xx, float yy, float zz)
{//planePoint 地平面上的點;
	//planeNormal 地平面上的法線;
	//xx,yy,zz 分別是測量點的x,y,z分量
	float high;
	high = fabs(planeNormal.x*(xx - planePoint.x) +
		planeNormal.y*(yy - planePoint.y) +
		planeNormal.z*(zz - planePoint.z)) / sqrt(planeNormal.x*planeNormal.x + planeNormal.y*planeNormal.y + planeNormal.z*planeNormal.z);
	return high;
}

//初始狀態偵測
int firststate(int eventf)
{
	int states = INCIDENT_NONE;//未偵測出初始狀態，必須繼續偵測。
	if (eventf == INCIDENT_NONE)
		states = states;
	if (eventf == INCIDENT_STOP || eventf == INCIDENT_STANDUP)
		states = STATUS_STAND;
	else if (eventf == INCIDENT_WALK)
		states = STATUS_WALK;
	else if (eventf == INCIDENT_FALLDOWN)
		states = STATUS_FAILDOWN;
	else if (eventf == INCIDENT_SIT_DOWN || eventf == INCIDENT_SIT_UP)
		states = STATUS_SITDOWN;
	else if (eventf == INCIDENT_LAYDOWN)
		states = STATUS_LAYDOWN;
	return states;
}


//狀態轉移圖
//shijian-> 1:停止 2走 3坐起來 4站起來 5跌倒 6坐下 7躺下 
int stateTransfer(int shijian, int states)
{
	int nextstates = STATUS_NONE;
	if (shijian == INCIDENT_NONE)
		nextstates = states; //事件為0，保持原始狀態
	if (states == 0)       //前一個狀態為0，不能用這個轉換。
		nextstates = 0;
	
	if (states == 2)
	{
		if (shijian == 5)
			nextstates = 3;
		else if (shijian == 1)
			nextstates = 1;
		else if (shijian == 2)
			nextstates = 2;
	}

	if (states == 3)
	{
		if (shijian == 4)
			nextstates = 1;
		else if (shijian == 3)
			nextstates = 4;
		else if (shijian == 5)
			nextstates = 3;
	}

	if (states == 4)
	{
		if (shijian == 4)
			nextstates = 1;
		else if (shijian == 5)
			nextstates = 3;
		else if (shijian == 7)
			nextstates = 5;
		else if (shijian == 6 || shijian == 3)
			nextstates = 4;
	}

	if (states == 5)
	{
		if (shijian == 3)
			nextstates = 4;
		if (shijian == 7)
			nextstates = 5;
	}
	if (states == 1)
	{
		if (shijian == 2)
			nextstates = 2;
		else if (shijian == 6)
			nextstates = 4;
		else if (shijian == 4)
			nextstates = 1;
		else if (shijian == 5)
			nextstates = 3;
		else if (shijian == 1)
			nextstates = 1;
	}
	return nextstates;
}

//異常狀態偵測
//shijian-> 1:停止 2走 3坐起來 4站起來 5跌倒 6坐下 7躺下 
int unnormalzhenc(int shijian, int states)
{
	int yichang = 0;//當為0的時候表示系統未處於異常情況。
	if (shijian == INCIDENT_NONE || states == STATUS_NONE)
		yichang = 0;

	if (states == STATUS_STAND)
	{
		if (shijian == INCIDENT_SIT_UP || shijian == INCIDENT_LAYDOWN)
			yichang = 1;
	}

	if (states == STATUS_WALK)
	{
		if (shijian == INCIDENT_SIT_UP || shijian == INCIDENT_STANDUP || shijian == INCIDENT_SIT_DOWN || shijian == INCIDENT_LAYDOWN)
			yichang = 1;
	}

	if (states == STATUS_FAILDOWN)
	{
		if (shijian == INCIDENT_STOP || shijian == INCIDENT_WALK || shijian == INCIDENT_SIT_DOWN || shijian == INCIDENT_LAYDOWN)
			yichang = 1;
	}

	if (states == STATUS_SITDOWN)
	{
		if (shijian == INCIDENT_STOP || shijian == INCIDENT_WALK)  //if(shijian==1||shijian==2||shijian==3)
			yichang = 1;
	}

	if (states == STATUS_LAYDOWN)
	{
		if (shijian == INCIDENT_STOP || shijian == INCIDENT_WALK || shijian == INCIDENT_STANDUP || shijian == INCIDENT_FALLDOWN || shijian == INCIDENT_SIT_DOWN)
			yichang = 1;
	}

	if (yichang == 1)
		cout << "可能有異常發生！！" << endl;
	return yichang;
}



//事件偵測  (沒用到!!)
//eventsj=eventzhence(fheadC,ftorsoC,fhead,ftorso,humanhigh,zhixgao，nowhigh，zhixinhigh); 
//shijian-> 1:停止 2走 3坐起來 4站起來 5跌倒 6坐下 7躺下 
int eventzhence(cv::Point3f headq, cv::Point3f quxinq, cv::Point3f head, cv::Point3f quxin, float shenhigh, float zxhih, float dih, float nzxhigh) //(前一幀,後一幀,身高,頭距離地面的高度)
{
	int shijian = 0;

	if (fabs(dih - shenhigh) > 100 && fabs(nzxhigh - zxhih) > 100) //第一個分支點.
	{
		if (head.y - headq.y > 20 && quxin.y - quxinq.y > 20) //第三個分支點.
		{
			if (fabs(dih - shenhigh) > 300 && fabs(nzxhigh - zxhih) > 300)//第四個分支點.
				shijian = 3; //坐起來;
			else
				shijian = 4; //站起來
		}
		else
		{
			if (fabs(dih - shenhigh) > 500 && fabs(nzxhigh - zxhih) > 500)//第5個分支點
				shijian = 5;//跌倒.
			else
			{
				if (head.y - quxin.y > 200 && fabs(dih - shenhigh) > 300 && fabs(nzxhigh - zxhih) > 300)//第6個分支點
					shijian = 6; //坐下
				else
					shijian = 7;//躺下;
			}
		}
	}
	else
	{
		if ((headq.x - head.x)*(headq.x - head.x) + (headq.z - head.z)*(headq.z - head.z) > 2000 && (quxinq.x - quxin.x)*(quxinq.x - quxin.x) + (quxinq.z - quxin.z)*(quxinq.z - quxin.z) > 2000) //第二個分支點.
			shijian = 2;//走
		else
			shijian = 1; //停止
	}
	return shijian;
}

//shijian-> 1:停止 2走 3坐起來 4站起來 5跌倒 6坐下 7躺下 
int eventDetect(cv::Point3f head_Pre, cv::Point3f torsol_Pre, cv::Point3f head, cv::Point3f torsol, cv::Point3f head_Ini, cv::Point3f torsol_Ini, cv::Point3f neckkk) //(前一幀,後一幀,初始幀)
{
	int shijian = INCIDENT_NONE;

	if (head_Ini.y - head.y > 200 && torsol_Ini.y - torsol.y > 200) //第一個分支點  :初始與現在的頭與身 Dy 大於200
	{
		if (head.y - head_Pre.y > 35 && torsol.y - torsol_Pre.y > 35) //第三個分支點.20 40//  前後頭身Dy 大於35
		{
			if (head_Ini.y - head.y > 300 && torsol_Ini.y - torsol.y > 300)
				shijian = INCIDENT_SIT_UP; //坐起來
		}
		else if (head_Ini.y - head.y > fallDownThreshold &&torsol_Ini.y - torsol.y > fallDownThreshold)//跌倒的判斷式很奇怪
		{
			if (head.y - neckkk.y > 60)
				shijian = INCIDENT_FALLDOWN;//跌倒
		}
		else if (head.y - torsol.y > 200)
		{
			if (head_Ini.y - head.y > 300 && torsol_Ini.y - torsol.y > 300 && head.y - head_Pre.y < -30 && torsol.y - torsol_Pre.y < -30)
				shijian = INCIDENT_SIT_DOWN; //坐下
		}
		else
			shijian = INCIDENT_LAYDOWN; //躺下  
	}
	else//頭與身 Dy 小於200
	{
		if (head.y - head_Pre.y > 100 && torsol.y - torsol_Pre.y > 100)
			shijian = INCIDENT_STANDUP; //站起來
		else if ((head_Pre.x - head.x)*(head_Pre.x - head.x) + (head_Pre.z - head.z)*(head_Pre.z - head.z) > 3000 && 
			     (torsol_Pre.x - torsol.x)*(torsol_Pre.x - torsol.x) + (torsol_Pre.z - torsol.z)*(torsol_Pre.z - torsol.z) > 3000) //第二個分支點.
			shijian = INCIDENT_WALK;//走
		else
			shijian = INCIDENT_STOP; //停止
	}
	return shijian;
}


//自動校正。即當偵測出“坐下”事件後，立即檢測到質心和頭依然高速向下移動，則需要更正；
// 同理，當偵測到“坐起來”事件後，立即檢測出質心和頭依然高速向上移動，則需更正。
void automend(int events, int states, cv::Point3f headq, cv::Point3f zxq, cv::Point3f headn, cv::Point3f zxn) // (前一個事件，前一次的狀態，前一次頭的位置，前一次質心位置，當前頭的位置，當前質心的位置)
{
	int ss = 0; //0:無更正，1：更正“坐下”事件及狀態；2：更正“做起來”事件及狀態

	if (events == INCIDENT_SIT_DOWN && headq.y - headn.y > 150 && zxq.y - zxn.y > 150)
		ss = 1;

	if (events == INCIDENT_SIT_UP && headq.y - headn.y < -200 && zxq.y - zxn.y < -200)
		ss = 2;

	if (ss == 1)
	{
		cout << "前一個偵測事件“坐下”錯誤，應該是“跌倒的過程”！！" << endl;
		if (states == 4)
			cout << "前一個狀態“坐著”錯誤，應該是“跌倒的過程中”！！" << endl;

	}
	if (ss == 2)
	{
		cout << "前一個偵測事件“坐起來”錯誤，應該是“站起來的過程”！！" << endl;
		if (states == 4)
			cout << "前一個狀態“坐著”錯誤，應該是“站起來的過程中”！！" << endl;
	}
}


//採用發生的時間來偵測。
//即：如果在發生“坐下”事件後，小於N幀之內偵測到“跌倒”事件發生，則之前偵測的“坐下”事件是錯誤的。
//同理：如果在偵測到“站起來”事件後，小於N幀之內偵測到“站起來”事件發生，則之前偵測的“站起來”事件是錯誤的。
//或者換種說法是：如果偵測到跌倒事件的時候，在此之前很多時間偵測到的“坐下”事件肯定是錯誤的，必須更正。
//“坐起來”也是類似的情況。
//同時，還需要進一步的更正狀態。
void automendG(int events, int n) // (當前事件，時間)
{
	int ss = 0; //0:無更正，1：更正“坐下”事件及狀態；2：更正“做起來”事件及狀態

	if (events == INCIDENT_FALLDOWN && n > 10 && n < 2000) //n表示當前發生跌倒事件的事件-前一次發生坐下的時間
		ss = 1;

	if (events == INCIDENT_STANDUP && n > 10 && n < 2000) //n表示當前發生站起來事件的事件-前一次發生坐起來的時間
		ss = 2;

	if (ss == 1)
	{
		cout << "前一個偵測事件“坐下”錯誤，應該是“跌倒的過程”！！" << endl;
		cout << "如果前一個狀態是“坐著”，則是錯誤的，應該是“跌倒的過程中”！！" << endl;
	}
	if (ss == 2)
	{
		cout << "前一個偵測事件“坐起來”錯誤，應該是“站起來的過程”！！" << endl;
		cout << "如果前一個狀態是“坐著”，則是錯誤的，應該是“站起來的過程中”！！" << endl;
	}
}




//顯示出現在狀態
void displayState(int nextState, int nowState){
	if (nextState != nowState)
	{
		if (nextState == 1)
			cout << "目前狀態: " << "站著" << endl;
		if (nextState == 2)
			cout << "目前狀態: " << "走著" << endl;
		if (nextState == 3)
			cout << "目前狀態: " << "跌倒" << endl;
		if (nextState == 4)
			cout << "目前狀態: " << "坐著" << endl;
		if (nextState == 5)
			cout << "目前狀態: " << "躺著" << endl;
	}
}
#endif // !mainHeader





