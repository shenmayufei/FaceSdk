#pragma once

#include <opencv2/opencv.hpp>

class MaskProcess
{
public:
	static void merge(cv::Mat src, cv::Mat mask, cv::Mat& dst)
	{
		if (src.channels() == 1)
		{
			cv::Mat color;
			cv::cvtColor(src, color, CV_GRAY2BGR);
			src = color;
		}
		dst.create(src.size(), CV_8UC3);
		for (int i = 0; i < src.rows; i++)
		{
			uchar* pSrc = src.data + i*src.step[0];
			uchar* pDst = dst.data + i*dst.step[0];
			uchar* pMask = mask.data + i*mask.step[0];
			for (int j = 0; j < src.cols; j++, pMask++, pSrc += 3, pDst += 3)
			{
				if (pMask[0])
				{
					pDst[0] = pSrc[0] * 0.6;
					pDst[1] = pSrc[1] * 0.5 + 127;
					pDst[2] = pSrc[2] * 0.6;
				}
				else
				{
					pDst[0] = pSrc[0];
					pDst[1] = pSrc[1];
					pDst[2] = pSrc[2];
				}
			}
		}
	}

	static cv::Mat generate_gaussian(cv::Size sz, cv::Point center, double deltaX = 15000, double deltaY = 8000)
	{
		cv::Mat_<float> mask(sz);
		mask.setTo(0);

		if (deltaX <= 0)
			deltaX = sz.width*sz.width / 2;
		if (deltaY <= 0)
			deltaY = sz.height*sz.height / 4;

		float dY = deltaY, dX = deltaX;
		for (int i = 0; i < sz.height; i++)
		{
			float* pMask = (float*)(mask.data + i*mask.step[0]);
			for (int j = 0; j < sz.width; j++, pMask++)
			{
				*pMask = expf(-(i - center.y)*(i - center.y) / dY - (j - center.x)*(j - center.x)/ dX);
			}
		}
		return mask;
	}

	static float otsu_thresh(double* Histogram, float & ratio, float rMin = 0, float rMax = 0)
	{
		int T = 0; //Otsu�㷨��ֵ  
		double varValue = 0; //��䷽���м�ֵ����  
		double w0 = 0; //ǰ�����ص�����ռ����  
		double w1 = 0; //�������ص�����ռ����  
		double u0 = 0; //ǰ��ƽ���Ҷ�  
		double u1 = 0; //����ƽ���Ҷ�  
		double minW = 0;
		float totalNum = 0;
		for (int i = 0; i < 256; i++)
		{
			totalNum += Histogram[i];
		}
		if (totalNum <= 1e-8)
			return 0;
		for (int i = 0; i < 255; i++)
		{
			//ÿ�α���֮ǰ��ʼ��������  
			w1 = 0;       u1 = 0;       w0 = 0;       u0 = 0;
			//***********����������ֵ����**************************  
			for (int j = 0; j <= i; j++) //�������ָ�ֵ����  
			{
				w1 += Histogram[j];  //�����������ص�����  
				u1 += j*Histogram[j]; //�������������ܻҶȺ�  
			}
			if (w1 == 0) //�����������ص���Ϊ0ʱ�˳�  
			{
				continue;
			}
			u1 = u1 / w1; //��������ƽ���Ҷ�  
			w1 = w1 / totalNum; // �����������ص�����ռ����  
								//***********����������ֵ����**************************  

								//***********ǰ��������ֵ����**************************  
			for (int k = i + 1; k < 255; k++)
			{
				w0 += Histogram[k];  //ǰ���������ص�����  
				u0 += k*Histogram[k]; //ǰ�����������ܻҶȺ�  
			}
			if (w0 == 0) //ǰ���������ص���Ϊ0ʱ�˳�  
			{
				continue;
			}
			if (rMin > 0 && w1 < rMin || rMax > 0 && w1 > rMax)
				continue;

			u0 = u0 / w0; //ǰ������ƽ���Ҷ�  
			w0 = w0 / totalNum; // ǰ���������ص�����ռ����  
								//***********ǰ��������ֵ����**************************  

								//***********��䷽�����******************************  
			double varValueI = w0*w1*(u1 - u0)*(u1 - u0); //��ǰ��䷽�����  
			if (varValue < varValueI)
			{
				varValue = varValueI;
				T = i;
				minW = w1;
			}
		}
		ratio = minW;
		return T;
	}

	static cv::Mat masked_otsu(cv::Mat src, cv::Mat mask, cv::Mat weight, float& thresh, bool bInv,
		float rMin = 0, float rMax = 0, 
		float dark_cut = 0, float light_cut = 0
	)
	{
#if 1
		if (src.channels() == 3)
		{
			cv::Mat gray;
			cv::cvtColor(src, gray, CV_BGR2GRAY);
			src = gray;
		}
#endif
		cv::Mat res;
	
		double Histogram[256] = { 0 }; //�Ҷ�ֱ��ͼ���±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ�����ص�����  
	//	uchar *data = src.data;
	//	double totalNum = 0; // image.rows*image.cols; //��������  
												 //����Ҷ�ֱ��ͼ�ֲ���Histogram�����±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ���ص���  

		if (!mask.empty())
		{
			for (int i = 0; i < src.rows; i++)   //Ϊ������������û�а�rows��cols���������  
			{
				uchar* pSrc = src.data + i*src.step[0];
				uchar* pMask = mask.data + i*mask.step[0];
				for (int j = 0; j < src.cols; j++, pSrc++, pMask++)
				{
					
					if (*pMask)
					{
						float val = weight.empty() ? 1 : weight.at<float>(i, j);
						Histogram[*pSrc] += val;
					}
	
				}
			}
		}
		else
		{
			for (int i = 0; i < src.rows; i++)   //Ϊ������������û�а�rows��cols���������  
			{
				uchar* pSrc = src.data + i*src.step[0];
				for (int j = 0; j < src.cols; j++, pSrc++)
				{
					float val = weight.empty() ? 1 : weight.at<float>(i, j);
					Histogram[*pSrc] += val;
				}
			}
		}
#if 0
		if (totalNum == 0)
		{
			res.create(src.size(), CV_8UC1);
			res.setTo(0);
			thresh = 0;
			return res;
		}
#endif
		float rr = 0;
		float r_dark = 0;
		float r_light = 1;
		float t_dark = -1, t_light = -1;
		if (dark_cut > 0)
		{
			t_dark = otsu_thresh(Histogram, r_dark, 0, dark_cut);
		}
		if (light_cut > 0)
		{
			t_light = otsu_thresh(Histogram, r_light, light_cut, 0);
		}

		float r_min = rMin > 0 ? ((r_light - r_dark)*rMin + r_dark) : r_dark;
		float r_max = rMax > 0 ? ((r_light - r_dark)*rMax + r_dark) : r_light;
		float T = otsu_thresh(Histogram, rr, r_min, r_max);

		//std::cout << "otsu r_min:" << r_min << ",r_max:" << r_max << std::endl;
	
		cv::Mat mDark, mLight;
		cv::threshold(src, res, T, 255, bInv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY);
		if (t_dark >= 0)
		{
			cv::threshold(src, mDark, t_dark, 255, CV_THRESH_BINARY);
			res &= mDark;
		}
		if (t_light >= 0)
		{
			cv::threshold(src, mLight, t_light, 255, CV_THRESH_BINARY_INV);
			res &= mLight;
		}
	
		thresh = T;
		if(!mask.empty())
			res &= mask;
		return res;
	}
};