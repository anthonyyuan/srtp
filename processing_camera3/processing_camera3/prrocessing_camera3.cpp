#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace std; 
using namespace cv;

int main()
{
	VideoCapture cap(0);	//������ͷ 
	if(!cap.isOpened())
	{
		cout<<"����ͷ���Ӵ���"<<endl;
		system("puase");
		return -1;
	}

	//��ʾԭ��Ƶͼ���ȣ��߶�
	double width=cap.get(CV_CAP_PROP_FRAME_WIDTH);//640
	double hight=cap.get(CV_CAP_PROP_FRAME_HEIGHT);//480
	
	cout<<"initial"<<endl;
	cout<<"width:"<<width<<endl;
	cout<<"hight:"<<hight<<endl<<endl;


	Mat frame;//����֡
	Mat edges;
	cap>>frame;//��ȡ��һ֡��ɾ�������У��ᷢ������
	cap>>frame;

	//�ı�ͼ���ȣ��߶�
	resize(frame, frame, Size(), 0.5, 0.5);

	//��ʾ�ı�����Ƶͼ���ȣ��߶�
	hight=frame.rows;
	width=frame.cols;

	cout<<"result"<<endl;
	cout<<"width:"<<width<<endl;
	cout<<"hight:"<<hight<<endl<<endl;

	//��ʼ��Ƶ����
	bool stop = false;
	while(!stop)
	{
		cap>>frame;//��ȡ��Ƶ��һ֡

		resize(frame, frame, Size(), 0.5, 0.5); //�ı�ͼ���С
		cvtColor(frame, edges, CV_BGR2GRAY);//��ԭͼ��ת��Ϊ�Ҷ�ͼ��
		imshow("��ǰ��Ƶ",edges);//��ʾ��Ƶ
		if(waitKey(1)>=0)//�ı�����ͷƵ�ʣ�ͨ������ֹͣ��Ƶ
			stop = true;
	}
	return 0;
}