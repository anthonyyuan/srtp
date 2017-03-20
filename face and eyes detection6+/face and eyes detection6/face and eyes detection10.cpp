/*
���ܣ�ʵ��գ�ۼ����ȵȣ�����Զ���������֣�
�汾��3.0 
ʱ�䣺2015-4-5
*/
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace std;
using namespace cv;

//-------------------------�Զ������----------------------------
#define initial_time					3		//���������ʼ��ʱ��
#define width_needed 					400 	//��������ͼ�񳤶�Ĭ�ϳ����Ϊ4��3
#define rate 							10 		//������Ƶ��
#define threshold_limit 				45 		//�����۾���ֵ������
#define integral_threshold 				36  	//����գ�ۻ������ޣ��ֶ���ʽ��
#define blink_intergral_coefficient		0.8		//գ�ۻ��������ж�ϵ������ռ���ۻ��ֵĶ���������ۣ�
#define close_eyes_time_limit 			2 		//������۱���ʱ�䳤��
#define blink_frequency_time 			10 		//����գ��Ƶ�ʼ���ʱ��
#define blink_frequency_number_limit 	8 		//����գ��Ƶ�ʱ�������
#define faces_empty_time                5		//����������ʧ����ʱ�䳤��
#define eyes_empty_time					10		//�����۲���ʧ����ʱ�䳤��
#define mode 							0 		//ѡ����Ƶͼ����Դ��0-�򿪵����Դ�����ͷ��1-��usb����ͷ 
#define set_size						0		//ѡ����Ƶ����������0-����������1-�������
#define blink_judge_method				1		//ѡ��գ�ۻ���ȷ����ʽ��0-�ֶ���1-�Զ�
#define integral_show					1		//�Ƿ���ʾ�۲����֣�1-�ǣ�0-��
#define blink_number_all_show			1 		//�Ƿ���ʾ��գ�۴�����1-�ǣ�0-��
#define close_eyes_number_show			0		//�Ƿ���ʾ��������֡����1-�ǣ�0-��
#define faces_width_show				0		//�Ƿ���ʾ���������ȣ�1-�ǣ�0-��
#define playmusic						1		//�Ƿ񲥷����֣�1-�ǣ�0-��

//-------------------------����ȫ�ֱ���---------------------------
int total_integral=0;					//��ʼ��ʱ�۲��Ļ����ۼ�ֵ
int total_faces_width=0;				//��ʼ��ʱ��������ۼ�ֵ
int integral_coefficient=0;				//���������ƽ��/�۾����֡�ϵ������

//--------------------------����׼��------------------------------
void adjust( Mat frame );				//�����û���������ͷ����
void initial( Mat frame );				//������ʼ������
void detectEyeAndFace( Mat frame );		//������⺯��

//���ؼ�����
//�����������ļ����Ƶ���ǰ�����£���ǰ�ļ�·��Ӧ����OpenCV��װ·���µ�sources\data\haarcascadesĿ¼��
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_mcs_eyepair_big.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

//---------------------------������--------------------------------------------------
//-----------------------------------------------------------------------------------
int main(void)
{
	//�ж�face_cascade_name��eye_cascade_name�ܹ�˳������
	if( !face_cascade.load( face_cascade_name ) ){ cout<<"face_cascade_name����ʧ��"<<endl;getchar();return -1; }
	if( !eyes_cascade.load( eyes_cascade_name ) ){ cout<<"eye_cascade_name����ʧ��"<<endl;getchar(); return -1; }
	
	VideoCapture cap(mode);		//������ͷ
	if(!cap.isOpened()){cout<<"����ͷ���Ӵ���"<<endl;getchar();return -1;}

	//--------------------����ͼ���С-----------------------------
	//��ʾԭ��Ƶͼ��ʹ����ͼ���ȣ��߶�
	int width0=cap.get(CV_CAP_PROP_FRAME_WIDTH);	//640
	int hight0=cap.get(CV_CAP_PROP_FRAME_HEIGHT);	//480
	cout<<"initial"<<endl;
	cout<<"width:"<<width0<<endl;
	cout<<"hight:"<<hight0<<endl<<endl;

	Mat frame;		//����һ֡ͼ��
	cap>>frame;		//��ȡ��һ֡��ɾ�������У��ᷢ������
	cap>>frame;
	int hight1;
	int width1;

	//������Ƶ��С
	bool set_size1=set_size;
	if(set_size1==1)//1.�����������
	{
		resize(frame, frame, Size(), width_needed*1.0/width0, width_needed*1.0/width0);
		//�ı�����Ƶͼ���ȣ��߶�
		hight1=frame.rows;
		width1=frame.cols;
	}
	else//2.������������
	{
		//�ı���Ƶͼ���ȣ��߶�(������Ƶ�����Ĺ�ϵ�����ܲ����ã��ɽ����������û�ж������)
		cap.set(CV_CAP_PROP_FRAME_WIDTH,width_needed);
		cap.set(CV_CAP_PROP_FRAME_HEIGHT,width_needed*3/4);	
		//�ı�����Ƶͼ���ȣ��߶ȣ�Ƶ��
		width1=cap.get(CV_CAP_PROP_FRAME_WIDTH);
		hight1=cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		cap>>frame;
	}

	//��ʾ�ı�����Ƶͼ���ȣ��߶�
	cout<<"result"<<endl;
	cout<<"width:"<<width1<<endl;
	cout<<"hight:"<<hight1<<endl<<endl;

	namedWindow("�۾����������ټ��",WINDOW_NORMAL);	//������Ƶ����
	namedWindow("�۾��������",WINDOW_NORMAL);			//�����۾�������򴰿�
	namedWindow("�۾�",WINDOW_NORMAL);					//�����۾����򴰿�
	namedWindow("�۾�����ֵ����",WINDOW_NORMAL);		//�����۾���ֵ�����򴰿�
	
	//-----------------�ȴ��û���������ͷ-------------------------
	cout<<"�뽫����ͷ�������ʵ�λ�ã������κ��ַ�ȷ��"<<endl;
	bool stop = false;
	while(!stop)
	{
		cap>>frame;			//ȡ��һ֡
		if(!frame.empty())
		{
			if(set_size1!=0)//������Ƶ��С1.�����������
			{
				resize(frame, frame, Size(), width_needed*1.0/width0, width_needed*1.0/width0);
			}

			//�û���������ͷ����
			adjust( frame );

			if(waitKey(1000/rate)>=0)stop = true;//������Ƶ֡�����ͨ����������ȷ��
		}
	}
	cout<<"�˳����ڽ׶�"<<endl;

	//---------------------������ʼ��-----------------------------
	cout<<"�뱣������"<<initial_time<<"�룬"<<"����ǰ��������˳���ʼ���׶�"<<endl;
	int i=0;
	stop = false;
	while( i < initial_time*rate && !stop )
	{
		cap>>frame;			//ȡ��һ֡
		if(!frame.empty())
		{
			if(set_size1!=0)//������Ƶ��С1.�����������
			{
				resize(frame, frame, Size(), width_needed*1.0/width0, width_needed*1.0/width0);
			}

			//��ʼ������
			initial( frame );

			if(waitKey(1000/rate)>=0)stop = true;//������Ƶ֡�����ͨ������ֹͣ��Ƶ
			i++;
		}
	}

	int average_integral=total_integral/(initial_time*rate);					//��ʼ��ʱ�۲��Ļ����ۼ�ֵƽ��
	int average_faces_width=total_faces_width/(initial_time*rate);				//��ʼ��ʱ��������ۼ�ֵƽ��
	if(average_integral!=0)integral_coefficient=average_faces_width*average_faces_width/average_integral;//���������ƽ��/�۾����֡�ϵ������
	else integral_coefficient=200;//Ĭ��ֵ200�����������

	cout<<"�˳���ʼ���׶�"<<endl;

	//---------------------��ʼ��Ƶ����----------------------------
	cout<<"��ʼ��⣬�����κ��ַ�ֹͣ"<<endl;
	stop = false;
	while(!stop)
	{
		cap>>frame;			//ȡ��һ֡
		if(!frame.empty())
		{
			if(set_size1!=0)//������Ƶ��С1.�����������
			{
				resize(frame, frame, Size(), width_needed*1.0/width0, width_needed*1.0/width0);
			}

			//ʶ����
			detectEyeAndFace( frame ); 

			if(waitKey(1000/rate)>=0)stop = true;//������Ƶ֡�����ͨ������ֹͣ��Ƶ
		}
	}
	return 0;
}

//------------------------�����û���������ͷ����-------------------------------------
//-----------------------------------------------------------------------------------
void adjust( Mat frame )
{
	//-----------------------��������------------------------------
	vector<Rect> faces; 
	vector<Rect> eyes;
	Mat grayFrame;										//����Ҷ�ͼ
	Mat faceROI;										//��ǰ��ע����������
	Mat eyeROI;											//��ǰ��ע���۲�����

	//-----------------------��ʼ���------------------------------
	cvtColor( frame, grayFrame, CV_BGR2GRAY );			//ת��Ϊ�Ҷ�ͼ
	equalizeHist( grayFrame, grayFrame );				//�Ҷ�ͼ���⻯
	face_cascade.detectMultiScale( grayFrame, faces, 1.1, 2, 0, Size(10,10));//�������

	//-----------------------������ʾ------------------------------
	if(!faces.empty())
	{
		//�þ��α�ע����
		Point p1(faces[0].x,faces[0].y),p2(p1.x + faces[0].width,p1.y + faces[0].height);
		rectangle(frame,p1,p2,Scalar( 255, 0, 255 ),2,8,0);

		faceROI = grayFrame( faces[0] );	//�õ���ǰ��ע����������
		equalizeHist( faceROI, faceROI );	//�����Ҷ�ͼ���⻯
		imshow( "�۾��������", faceROI );	//��ʾ�۾��������
		eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0,Size(2,2));//�۲����
		
		//--------------------�۲�����----------------------------
		if(!eyes.empty())
		{
			//�þ��α�ע�۾�
			Point p3(p1.x + eyes[0].x,p1.y + eyes[0].y),p4(p3.x + eyes[0].width,p3.y + eyes[0].height);
			rectangle(frame,p3,p4,Scalar( 255, 0, 0 ),2,8,0);

			eyeROI = faceROI( eyes[0] );		//�õ���ǰ��ע���۲�����
			imshow( "�۾�", eyeROI );			//��ʾ�۾�����
			threshold(eyeROI,eyeROI, threshold_limit, 255, THRESH_BINARY);//���۾��Ҷ�ͼ��ֵ��
			imshow( "�۾�����ֵ����", eyeROI );//��ʾ��ֵ���۾�����

		}//�۲��������
	}//�����������

	imshow( "�۾����������ټ��", frame );//��ʾ����ͷ����
}

//------------------------�����ʼ������---------------------------------------------
//-----------------------------------------------------------------------------------
void initial( Mat frame )
{
	//-----------------------��������------------------------------
	vector<Rect> faces; 
	vector<Rect> eyes;
	static vector<Rect> faces_front; 
	static vector<Rect> eyes_front;
	Mat grayFrame;										//����Ҷ�ͼ
	Mat faceROI;										//��ǰ��ע����������
	Mat eyeROI;											//��ǰ��ע���۲�����
	int k,l;
	int integral=0;										//��ֵͼ����

	//-----------------------��ʼ���------------------------------
	cvtColor( frame, grayFrame, CV_BGR2GRAY );			//ת��Ϊ�Ҷ�ͼ
	equalizeHist( grayFrame, grayFrame );				//�Ҷ�ͼ���⻯
	face_cascade.detectMultiScale( grayFrame, faces, 1.1, 2, 0, Size(10,10));//�������

	//-------------------����������ʧ���--------------------------
	if(faces.empty())
	{
		faces=faces_front;				//�����һ֡û�в�׽������������ͼ�������⣬��������һ֡
	}

	//-----------------------��������------------------------------
	if(!faces.empty())
	{
		//�þ��α�ע����
		Point p1(faces[0].x,faces[0].y),p2(p1.x + faces[0].width,p1.y + faces[0].height);
		rectangle(frame,p1,p2,Scalar( 255, 0, 255 ),2,8,0);

		faceROI = grayFrame( faces[0] );	//�õ���ǰ��ע����������
		equalizeHist( faceROI, faceROI );	//�����Ҷ�ͼ���⻯
		imshow( "�۾��������", faceROI );	//��ʾ�۾��������
		eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0,Size(2,2));//�۲����

		//----------------�����۲���ʧ���------------------------
		if(eyes.empty())
		{
				eyes=eyes_front;			//�����һ֡û�в�׽���������۲�ͼ�������⣬��������һ֡
		}
		
		//--------------------�۲�����----------------------------
		if(!eyes.empty()&&eyes[0].x>0&&eyes[0].y>0&&eyes[0].x+eyes[0].width<faces[0].width&&eyes[0].y+eyes[0].height<faces[0].height)
		{
			//�þ��α�ע�۾�
			Point p3(p1.x + eyes[0].x,p1.y + eyes[0].y),p4(p3.x + eyes[0].width,p3.y + eyes[0].height);
			rectangle(frame,p3,p4,Scalar( 255, 0, 0 ),2,8,0);

			eyeROI = faceROI( eyes[0] );		//�õ���ǰ��ע���۲�����
			imshow( "�۾�", eyeROI );			//��ʾ�۾�����
			threshold(eyeROI,eyeROI, threshold_limit, 255, THRESH_BINARY);//���۾��Ҷ�ͼ��ֵ��
			imshow( "�۾�����ֵ����", eyeROI );//��ʾ��ֵ���۾�����

			//--------------�����۾�����---------------------------
			for(k=1;k<eyeROI.rows;k++)
			{
				for(l=1;l<eyeROI.cols;l++)
				{
					if(eyeROI.at<uchar>(k,l)==0)integral++;
				}
			}
			bool integral_show1=integral_show;
			if(integral_show1==1){cout<<"�۲����֣�"<<integral<<endl;}	//��ʾ�۲�����
			total_integral+=integral;	//ÿ�μ��Ļ���ֵ�ۼ�
			total_faces_width+=faces[0].width;
		}//�۲��������
	}//�����������

	faces_front=faces;	//������һ֡����
	eyes_front=eyes;	//������һ֡�۲�
	imshow( "�۾����������ټ��", frame );//��ʾ����ͷ����
}

//------------------------�����⺯��-----------------------------------------------
//-----------------------------------------------------------------------------------
void detectEyeAndFace( Mat frame )
{
	//-----------------------��������------------------------------
	vector<Rect> faces; 
	vector<Rect> eyes;
	static vector<Rect> faces_front; 
	static vector<Rect> eyes_front; 
	Mat grayFrame;										//����Ҷ�ͼ
	Mat faceROI;										//��ǰ��ע����������
	Mat eyeROI;											//��ǰ��ע���۲�����
	int k,l;
	int integral=0;										//��ֵͼ����
	static int close_eyes_number=0;						//������������֡��
	static int blink_number_all=0;						//����գ���ܴ���
    static bool blink_record[blink_frequency_time*rate];//�����۾�գ�ۼ�¼����
    static int blink_record_i=0;						//���嵱ǰ֡���۾�գ�ۼ�¼�����е�λ��
    static bool eyestate=1;								//�����۾�״̬
	static int faces_empty_number;						//������ʧ����
	static int eyes_empty_number;						//�۲���ʧ����
    int blink_number=0;									//����ʱ����գ�۴���

	//-----------------------��ʼ���------------------------------
	cvtColor( frame, grayFrame, CV_BGR2GRAY );			//ת��Ϊ�Ҷ�ͼ
	equalizeHist( grayFrame, grayFrame );				//�Ҷ�ͼ���⻯
	face_cascade.detectMultiScale( grayFrame, faces, 1.1, 2, 0, Size(10,10));//�������

	//-------------------����������ʧ���--------------------------
	if(faces.empty())
	{
		if(faces_empty_number<faces_empty_time*rate)
		{
			faces=faces_front;				//�����һ֡û�в�׽������������ͼ�������⣬��������һ֡
			faces_empty_number++;
		}
		else
		{
			cout<<"����ͼ��ʧ��"<<endl;
			faces_empty_number=0;
		}
	}
	else{faces_empty_number=0;}

	//-----------------------��������------------------------------
	if(!faces.empty())
	{
		//�þ��α�ע����
		Point p1(faces[0].x,faces[0].y),p2(p1.x + faces[0].width,p1.y + faces[0].height);
		rectangle(frame,p1,p2,Scalar( 255, 0, 255 ),2,8,0);

		bool faces_width_show1=faces_width_show;
		if(faces_width_show1==1){cout<<"����������"<<faces[0].width<<endl;}//��ʾ����������
		faceROI = grayFrame( faces[0] );	//�õ���ǰ��ע����������
		equalizeHist( faceROI, faceROI );	//�����Ҷ�ͼ���⻯
		imshow( "�۾��������", faceROI );	//��ʾ�۾��������
		eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0,Size(2,2));//�۲����

		//----------------�����۲���ʧ���------------------------
		if(eyes.empty())
		{
			if(eyes_empty_number<eyes_empty_time*rate)
			{
				eyes=eyes_front;			//�����һ֡û�в�׽���������۲�ͼ�������⣬��������һ֡
				eyes_empty_number++;
			}
			else
			{
				cout<<"�۲�ͼ��ʧ��"<<endl;
				eyes_empty_number=0;
			}
		}
		else{eyes_empty_number=0;}
		
		//--------------------�۲�����----------------------------
		if(!eyes.empty()&&eyes[0].x>0&&eyes[0].y>0&&eyes[0].x+eyes[0].width<faces[0].width&&eyes[0].y+eyes[0].height<faces[0].height)
		{
			//�þ��α�ע�۾�
			Point p3(p1.x + eyes[0].x,p1.y + eyes[0].y),p4(p3.x + eyes[0].width,p3.y + eyes[0].height);
			rectangle(frame,p3,p4,Scalar( 255, 0, 0 ),2,8,0);

			eyeROI = faceROI( eyes[0] );		//�õ���ǰ��ע���۲�����
			imshow( "�۾�", eyeROI );			//��ʾ�۾�����
			threshold(eyeROI,eyeROI, threshold_limit, 255, THRESH_BINARY);//���۾��Ҷ�ͼ��ֵ��
			imshow( "�۾�����ֵ����", eyeROI );//��ʾ��ֵ���۾�����

			//--------------�����۾�����---------------------------
			for(k=1;k<eyeROI.rows;k++)
			{
				for(l=1;l<eyeROI.cols;l++)
				{
					if(eyeROI.at<uchar>(k,l)==0)integral++;
				}
			}

			bool integral_show1=integral_show;
			if(integral_show1==1){cout<<"�۲����֣�"<<integral<<endl;}//��ʾ�۲�����

			//-------------գ���б��봦��---------------------------
			bool blink_judge_method1=blink_judge_method;
			int integral_threshold1;
			if(blink_judge_method1==0)integral_threshold1=integral_threshold;//գ�ۻ���ȷ����ʽ��0-�ֶ�
			else integral_threshold1=faces[0].width*faces[0].width/integral_coefficient*blink_intergral_coefficient;//գ�ۻ���ȷ����ʽ��1-�Զ�

			if(integral<integral_threshold1)//*����㣩��һ֡����
			{
				close_eyes_number++;
				bool close_eyes_number_show1=close_eyes_number_show;
				if(close_eyes_number_show1==1){cout<<"��������֡����"<<close_eyes_number<<endl;}//��ʾ��������֡��
			    if(eyestate==0)//**���ڲ㣩ǰһ֡����
			    {
			        blink_number_all++;
			        bool blink_number_all_show1=blink_number_all_show;
			        if(blink_number_all_show1==1){cout<<"��գ�۴�����"<<blink_number_all<<endl;}//��ʾ��գ�۴���
			        blink_record[blink_record_i]=1;
			    }
			    else//**���ڲ㣩ǰһ֡����
				{
					if(close_eyes_number>close_eyes_time_limit*rate)
					{
						close_eyes_number=0;
						cout<<"����ʱ�䳬�����ƣ�"<<endl;
						if (playmusic==1)//�Ƿ񲥷�����
						{
							PlaySound (TEXT("����.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
						}
					}
					//blink_record[blink_record_i]=0;//��ʡ��
				}
				eyestate=1;
		    }
		    else//*����㣩��һ֡����
			{
				close_eyes_number=0;
				eyestate=0;
				//blink_record[blink_record_i]=0;//��ʡ��
			}
			blink_record_i++;//֡���۾�գ�ۼ�¼�����е�λ��++

			//-----------����ʱ����գ�۴���------------------------
			if(blink_record_i==blink_frequency_time*rate)
			{
				for(int i=0;i<blink_frequency_time*rate;i++)
				{
					if(blink_record[i]==1)blink_number++;
				}
				if(blink_number>blink_frequency_number_limit)
				{
					blink_number=0;
					cout<<"գ��Ƶ�ʹ��ߣ�"<<endl;
					if (playmusic==1)//�Ƿ񲥷�����
					{
						PlaySound (TEXT("����.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
					}		
				}
				blink_record_i=0;
				memset(blink_record, 0, sizeof(blink_record));
			}

		}//�۲��������
	}//�����������

	faces_front=faces;	//������һ֡����
	eyes_front=eyes;	//������һ֡�۲�
	imshow( "�۾����������ټ��", frame );//��ʾ����ͷ����
}