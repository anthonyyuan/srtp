#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace std; 
using namespace cv;

//����Ƶ�ļ����Ƶ���ǰ�����£�������ΪSampe
int main()
{
	VideoCapture cap("3.avi");	//����Ƶ  
	if(!cap.isOpened())
	{
		cout<<"��Ƶ��ȡ����"<<endl;
		system("puase");
		return -1;
	}

	double rate=cap.get(CV_CAP_PROP_FPS);//��ȡ֡��
	int delay=1000/rate;//ÿ֮֡����ӳ�����Ƶ��֡�����Ӧ

	Mat frame;//����֡
	Mat edges;

	//��ʼ��Ƶ����
	bool stop = false;
	while(!stop)
	{
		if(!cap.read(frame))//��ȡ��Ƶ��һ֡
		{
			cout<<"��Ƶ����"<<endl;
			system("pause");
			break;
		}
		cvtColor(frame, edges, CV_BGR2GRAY);//��ԭͼ��ת��Ϊ�Ҷ�ͼ��
		GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);//��˹�˲�
		Canny(edges, edges, 0, 30, 3);//��Ե���
		imshow("��ǰ��Ƶ",edges);//��ʾ��Ƶ
		if(waitKey(delay)>=0)//�����ӳ٣�ͨ������ֹͣ��Ƶ
			stop = true;
	}
	return 0;
}