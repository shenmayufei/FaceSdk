#pragma once

#include <imiCV.h>

struct ObjectInfo
{
	int id;						//Ŀ������
	int sex;					//�Ա�
	int age;					//����
	int x;						//Ŀ������Ͻ�x����
	int y;						//Ŀ������Ͻ�y����
	int width;					//Ŀ�����
	int height;					//Ŀ���߶�
	double vx;					//Ŀ��x�����ٶ�
	double vy;					//Ŀ��y�����ٶ�
	int maxque = 100;			//Ŀ��켣�����Ŀ
	std::vector<int>xque;			//Ŀ��켣
	std::vector<int>yque;			//Ŀ��켣
	std::vector<cv::Point> land;
};

#include <opencv2/opencv.hpp>
class FaceTracker
{
public:
	FaceTracker()
	{
		maxCount = 100;
		qLevel = 0.01;
		minDist = 1.0;
		winSize = cv::Size(40, 40);
		level = 4;
		threshold = 10;
		scale = 1;
		//pFast = cv::FastFeatureDetector::create(threshold);
		pFast = new cv::FastFeatureDetector(threshold);
	}

	void trace(cv::Mat &frame_, std::vector<imi::FaceInfo> &vInfo, std::vector<imi::FaceInfo>& vOut, bool bInit)
	{
		cv::Mat frame;
		if (frame_.cols <= 640)
			frame = frame_;
		else
		{
			scale = 2;
			cv::resize(frame_, frame, cv::Size(frame_.cols / scale, frame_.rows / scale));
		}

		std::vector<ObjectInfo> obj(vInfo.size());

		for (int i = 0; i < vInfo.size(); i++)
		{
			obj[i].x = vInfo[i].bbox.x / scale;
			obj[i].y = vInfo[i].bbox.y / scale;
			obj[i].width = vInfo[i].bbox.width / scale;
			obj[i].height = vInfo[i].bbox.height / scale;
			
			obj[i].xque.push_back(vInfo[i].bbox.x / scale);
			obj[i].yque.push_back(vInfo[i].bbox.y / scale);
			
			for (int k = 0; k < 5; k++)
			{
				obj[i].land.push_back(cv::Point(vInfo[i].landmark[k].x / scale, vInfo[i].landmark[k].y / scale));
			}
		}
		std::vector<ObjectInfo> tar = obj;
		bool isLoss = false;
		cv::Mat m_temp = frame.clone();

		trace(frame, m_temp, obj, tar, bInit, isLoss);


		vOut.resize(tar.size());
		for (int i = 0; i < tar.size(); i++)
		{
			vOut[i].bbox.x = tar[i].x * scale;
			vOut[i].bbox.y = tar[i].y * scale;
			vOut[i].bbox.width = tar[i].width * scale;
			vOut[i].bbox.height = tar[i].height * scale;

			for (int k = 0; k < 5; k++)
			{
				vOut[i].landmark[k].x = tar[i].land[k].x *scale;
				vOut[i].landmark[k].y = tar[i].land[k].y * scale;
			}
		}
		//cv::imshow("m_temp", m_temp);
	}
protected:
	void trace(cv::Mat &frame, cv::Mat &output, std::vector<ObjectInfo>& obj, std::vector<ObjectInfo>& objTraced, bool bInit, bool& isLoss)
	{
		if (frame.channels() == 3)
			cv::cvtColor(frame, nextImg, CV_BGR2GRAY);
		else
			frame.copyTo(nextImg);


		//Ϊ��һ֡���ٳ�ʼ�������㼯�ͻҶ�ͼ��
	
		if (obj.empty())
		{
			isLoss = true;
			return;
		}

		//�����ǰ֡Ϊ��һ֡
		if (prevImg.empty()) nextImg.copyTo(prevImg);

		//��������������ֵ�����¼��������
		addNewPoint(obj, bInit);
		if (bInit)
			prevPts = nextPts;
		//if (prevPts.empty())
		//	return;


		//����ǰ����֡�Ҷ�ͼ����ǰһ֡�������ڵ�ǰ֡��λ��
		if (!bInit)
		{
			nextPts.clear();
			if (!prevPts.empty())
			{
				cv::calcOpticalFlowPyrLK(prevImg, nextImg, prevPts, nextPts, status, err, winSize, level);
				cv::calcOpticalFlowPyrLK(nextImg, prevImg, nextPts, bkPts, status, err, winSize, level);
			}
				


			//���������

			//�����ƶ����볬����ֵ��������
			discardTrackedPoint();

			//����Ŀ�����λ��
			isLoss = getObjTraced(obj, objTraced);
			//����Ŀ���֮���������
			acceptTrackedPoint(obj, objTraced);
		}
#ifndef __ANDROID__
		//if (!output.empty())
		//	drawTrackedPoint(frame, output);
#endif
		std::swap(nextPts, prevPts);
		std::swap(prevImg, nextImg);

	};

private:
	//����������Ŀ����minCount�������������
	void addNewPoint(std::vector<ObjectInfo>& obj, bool bInit)
	{
		//cv::Mat prevMM = prevImg.clone();
		cv::Rect range(0, 0, nextImg.cols, nextImg.rows);
		std::vector<cv::Point2f>& refPts = bInit ? nextPts : prevPts;
		if (bInit)
			refPts.clear();
		for (int i = 0; i < obj.size(); i++)
		{
			cv::Rect r(obj[i].x, obj[i].y, obj[i].width, obj[i].height);
			for (int j = 0; j < 5; j++)
			{
				cv::Point p = obj[i].land[j];
				cv::Rect sub_r = cv::Rect(p.x - r.width / 8, p.y - r.height / 8, r.width / 4, r.height / 4) & range & r;

				int count = 0;
				for (int k = 0; k < refPts.size(); k++)
				{
					if (sub_r.contains(refPts[k]))
						count++;
				}
				if (count <= maxCount*0.6 / 5)
				{
					std::string detecter = "fast"; //good ����fast
					features.clear();
					featureDetect(bInit? nextImg:prevImg, sub_r, detecter);
					refPts.insert(refPts.end(), features.begin(), features.end());
					initial.insert(initial.end(), features.begin(), features.end());
					//for (int c = 0; c < features.size(); c++)
					//{
					//	cv::circle(prevMM, features[c], 2, cv::Scalar(255, 0, 0), -1);
					//}
				
				}
				//cv::rectangle(prevMM, sub_r, cv::Scalar(0, 255, 0), 2);

			}
		}
	}

	//�������⣬�ṩ���ּ�ⷽ��
	void featureDetect(cv::Mat &img, cv::Rect r, std::string detecter)
	{
		if (r.area() <= 0)
			return;
		//����Ŀ����������룬��״ΪԲ�Σ��뾶������Ŀ���
		//cv::Size imgSize = img.size();
		cv::Mat mask = cv::Mat::zeros(cv::Size(r.width,r.height), CV_8UC1);
		
		double alpha = 0.5;
		int xcenter = r.width / 2;
		int ycenter = r.height / 2;
		double radius = alpha * sqrt(r.width*r.height);

		circle(mask, cv::Point(xcenter, ycenter), radius, cv::Scalar(255), -1, 8);
		
		//��һ�ּ�ⷽ��
		if (detecter == "good")
		{
			goodFeaturesToTrack(img(r), features, maxCount / 5, qLevel, minDist, mask);
		}
		//�ڶ��ּ�ⷽ��
		else if (detecter == "fast")
		{
			std::vector<cv::KeyPoint> keyPoints;						//����������
			//cv::FastFeatureDetector fastDetecrtor(threshold);	//���ü����ֵ
			pFast->detect(img(r), keyPoints, mask);		//FAST��������
			cv::KeyPoint::convert(keyPoints, features);			//keyPointsתPoint2f
		}
		for (int i = 0; i < features.size(); i++)
			features[i] += cv::Point2f(r.x, r.y);
	}

	//����������ǰ����֡�ƶ����������ɾ��
	void discardTrackedPoint()
	{
		double maxShift = nextImg.cols / 2;
		int k = 0;

		for (int i = 0; i < prevPts.size(); i++)
		{
			if ((bkPts.empty() || cv::norm(prevPts[i] - bkPts[i]) <= 8) && \
				abs(nextPts[i].x - prevPts[i].x) < maxShift && \
				abs(nextPts[i].y - prevPts[i].y) < maxShift)
			{
				initial[k] = initial[i];
				prevPts[k] = prevPts[i];
				nextPts[k++] = nextPts[i];
			}
		}
		
		initial.resize(k);
		prevPts.resize(k);
		nextPts.resize(k);
	}

	//��׷�ٵ����������ڸ��º��Ŀ�������ɾ��
	void acceptTrackedPoint(std::vector<ObjectInfo>& obj, std::vector<ObjectInfo>& objTraced)
	{
		int k = 0;
		for (int i = 0; i < nextPts.size(); i++)
		{
			for (int j = 0; j < (int)objTraced.size(); ++j)
			{
				double alpha = 0.25*1.2; //�Դ��ڳ�ʼ���İ뾶
				double distx = nextPts[i].x - (objTraced[j].x + objTraced[j].width / 2);
				double disty = nextPts[i].y - (objTraced[j].y + objTraced[j].height / 2);
				double dist = sqrt(pow(distx, 2) + pow(disty, 2));
				double radius = alpha * sqrt(objTraced[j].width * objTraced[j].height);
				if (dist < radius)
				{
					initial[k] = initial[i];
					prevPts[k] = prevPts[i];
					nextPts[k++] = nextPts[i];
					break;
				}

			}
		}
		initial.resize(k);
		prevPts.resize(k);
		nextPts.resize(k);
	}

	//���Ŀ������һ֡��λ��
	bool getObjTraced(std::vector<ObjectInfo>& obj, std::vector<ObjectInfo>& objTraced)
	{
		bool isLoss = false;

		for (int j = 0; j < (int)obj.size(); ++j)
		{
			int dx = 0;
			int dy = 0;
			int count = 0;
			for (int i = 0; i < (int)nextPts.size(); ++i)
			{
				double alpha = 0.4;
				double distx = prevPts[i].x - (obj[j].x + obj[j].width / 2);
				double disty = prevPts[i].y - (obj[j].y + obj[j].height / 2);
				double dist = sqrt(pow(distx, 2) + pow(disty, 2));
				double radius = alpha* sqrt(obj[j].width * obj[j].height);
				if (dist < radius)
				{
					dx += int(nextPts[i].x - prevPts[i].x);
					dy += int(nextPts[i].y - prevPts[i].y);
					count++;
				}
			}
			dx = dx / (count + 1E-15);
			dy = dy / (count + 1E-15);

			objTraced[j].land.resize(5);
			for (int k = 0; k < 5; k++)
			{
				cv::Point pLand = obj[j].land[k];
				int w = obj[j].width / 4;
				int h = obj[j].height / 4;
				double alpha = 0.6;
				float sub_count = 0;
				float sub_dx = 0, sub_dy = 0;
				for (int i = 0; i < (int)nextPts.size(); i++)
				{	
					double distx = prevPts[i].x - (pLand.x);
					double disty = prevPts[i].y - (pLand.y);
					double dist = sqrt(pow(distx, 2) + pow(disty, 2));
					double radius = alpha* sqrt(w * h);
					if (dist < radius)
					{
						double t = 1.0;
						//double t = pow(2.0, -1.0* dist / radius);
						sub_dx += (nextPts[i].x - prevPts[i].x)*t;
						sub_dy += (nextPts[i].y - prevPts[i].y)*t;
						sub_count += t;
					}
				}
				if (sub_count == 0)
				{
					objTraced[j].land[k] = obj[j].land[k] + cv::Point(dx, dy);
				}
				else
				{
					sub_dx /= sub_count;
					sub_dy /= sub_count;
					//sub_dx = sub_dx *0.5 + dx*0.5;
					//sub_dy = sub_dy *0.5 + dy*0.5;
					objTraced[j].land[k] = obj[j].land[k] + cv::Point(sub_dx, sub_dy);
				}

			}

			//if (!objTraced[j].xque.empty())
			{
				objTraced[j].x = obj[j].x + dx;
				objTraced[j].y = obj[j].y + dy;
			}

			objTraced[j].xque.push_back(objTraced[j].x + objTraced[j].width / 2);
			objTraced[j].yque.push_back(objTraced[j].y + objTraced[j].height / 2);

			if (objTraced[j].xque.size() > objTraced[j].maxque)
			{
				objTraced[j].xque.erase(objTraced[j].xque.begin());
				objTraced[j].yque.erase(objTraced[j].yque.begin());
			}

			//���Ŀ���Խ�߽磬��һ֡���¿�ʼ���
			if (objTraced[j].x<0 || objTraced[j].x + objTraced[j].width>nextImg.cols || \
				objTraced[j].y<0 || objTraced[j].y + objTraced[j].height>nextImg.rows)
			{
				isLoss = true;
				objTraced[j].xque.resize(0);
				objTraced[j].yque.resize(0);
				obj[j].xque.resize(0);
				obj[j].yque.resize(0);
			}
		}

		return isLoss;
	}

	//��׷�ٵ�׷�ӵ�ͼ����ȥ
	void  drawTrackedPoint(cv::Mat &frame, cv::Mat &output)
	{
		for (int i = 0; i < nextPts.size(); i++)
		{
			circle(output, prevPts[i], 3, cv::Scalar(255, 0, 0), -1);
			line(output, prevPts[i], nextPts[i], cv::Scalar::all(0));
			circle(output, nextPts[i], 3, cv::Scalar(0, 255, 0), -1);
		}
	}

private:
	cv::Mat nextImg;				//ǰһ֡�ĻҶ�ͼ
	cv::Mat prevImg;				//��һ֡�ĻҶ�ͼ
	std::vector<cv::Point2f> prevPts;	//ǰһ֡��������
	std::vector<cv::Point2f> nextPts;	//��һ֡��������

	std::vector<cv::Point2f> bkPts;

	std::vector<cv::Point2f> initial;	//��ʼ������
	std::vector<cv::Point2f> features;	//��⵽������
	int maxCount;				//������������Ŀ
	double qLevel;				//��������ָ��
	double minDist;				//������֮����С���̾���
	std::vector<uchar> status;		//�����㱻�ɹ����ٵı�־
	std::vector<float> err;			//������С��������
	cv::Size winSize;				//ÿ���������ڴ�С
	int level;					//����������
	int threshold;				//FAST�����ֵ

	int scale;

	cv::Ptr<cv::FastFeatureDetector> pFast;
};