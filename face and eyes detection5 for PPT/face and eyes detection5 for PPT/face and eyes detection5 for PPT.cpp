/*
���ܣ�ʵ��գ�ۼ�����
�汾��2.0 
ʱ�䣺2014-11-25
*/
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define width_needed           250      //��������ͼ�񳤶�
#define rate                   10      //������Ƶ��
#define threshold_limit        15       //�����۾���ֵ������
#define integral_threshold     70      //����գ�ۻ�������
#define close_eyes_time_limit  2        //������۱���ʱ�䳤��
#define blink_frequency_time   10       //����գ��Ƶ�ʼ���ʱ��
#define blink_frequency_number_limit  10       //����գ��Ƶ�ʱ�������


void detectEyeAndFace( Mat frame );//������⺯��

//���ؼ�����
//�����������ļ����Ƶ���ǰ�����£���ǰ�ļ�·��Ӧ����OpenCV��װ·���µ�sources\data\haarcascadesĿ¼��
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_mcs_eyepair_big.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

int main(void)
{
	//�ж�face_cascade_name��eye_cascade_name�ܹ�˳������
	if( !face_cascade.load( face_cascade_name ) ){ cout<<"face_cascade_name����ʧ��"<<endl; system("puase");return -1; }
	if( !eyes_cascade.load( eyes_cascade_name ) ){ cout<<"eye_cascade_name����ʧ��"<<endl;system("puase"); return -1; }
	
    VideoCapture cap("3.avi");	//������ͷ��0�򿪵����Դ�����ͷ��1��usb����ͷ 
	if(!cap.isOpened()){cout<<"����ͷ���Ӵ���"<<endl;system("puase");return -1;}

	//��ʾԭ��Ƶͼ��ʹ����ͼ���ȣ��߶�
	//int width=cap.get(CV_CAP_PROP_FRAME_WIDTH);//640
	//int hight=cap.get(CV_CAP_PROP_FRAME_HEIGHT);//480
	
	//cout<<"initial"<<endl;
	//cout<<"width:"<<width<<endl;
	//cout<<"hight:"<<hight<<endl<<endl;

	Mat frame;//����һ֡ͼ��
	cap>>frame;//ȡ��һ֡
	namedWindow("�۾����������ټ��",WINDOW_NORMAL);//������Ƶ����
	namedWindow("�۾��������",WINDOW_NORMAL);//�����۾�������򴰿ڣ����ˣ�
	namedWindow("�۾�",WINDOW_NORMAL);//�����۾�������򴰿ڣ����ˣ�
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

		//ʶ����
		detectEyeAndFace( frame ); 

		if(waitKey(1000/rate)>=0)stop = true;//������Ƶ֡�����ͨ������ֹͣ��Ƶ
	}
	return 0;
}

//�����⺯��
void detectEyeAndFace( Mat frame )
{
	vector<Rect> empty;
	vector<Rect> faces; 
	vector<Rect> eyes;
	static vector<Rect> faces_front; 
	static vector<Rect> eyes_front; 
	Mat grayFrame;//����Ҷ�ͼ
	Mat faceROI;//��ǰ��ע����������
	Mat eyeROI;//��ǰ��ע���۲�����
	int k,l;
	int integral=0;//��ֵͼ����
	static int close_eyes_number=0;                //������������֡��
	static int blink_number_all=0;                 //����գ���ܴ���
    static bool blink_record[blink_frequency_time*rate];//�����۾�գ�ۼ�¼����
    static int blink_record_i=0;                   //���嵱ǰ֡���۾�գ�ۼ�¼�����е�λ��
    static bool eyestate=0;                        //�����۾�״̬
    static int blink_number=0;                     //����ʱ����գ�۴���


	cvtColor( frame, grayFrame, CV_BGR2GRAY );//ת��Ϊ�Ҷ�ͼ
	equalizeHist( grayFrame, grayFrame );//�Ҷ�ͼ���⻯

	face_cascade.detectMultiScale( grayFrame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE);//�������
	if(faces.empty())
	{
		faces=faces_front;//�����һ֡û�в�׽����������������һ֡
	}

	for( size_t i = 0; i < faces.size(); i++ )
	{
		//�þ��α�ע����
		Point p1(faces[i].x,faces[i].y),p2(p1.x + faces[i].width,p1.y + faces[i].height);
		rectangle(frame,p1,p2,Scalar( 255, 0, 255 ),2,8,0);
		//��Բ�α�ע����
		//Point center( int(faces[i].x + faces[i].width*0.5), int(faces[i].y + faces[i].height*0.5) );
		//ellipse( frame, center, Size( int(faces[i].width*0.5), int(faces[i].height*0.5)), 0, 0, 360, Scalar( 255, 0, 255 ), 2, 8, 0 );

		faceROI = grayFrame( faces[i] );  //�õ���ǰ��ע����������
		//��ʾ�۾��������
		imshow( "�۾��������", faceROI );

		eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE);//�۲����
		if(eyes.empty())
		{
			eyes=eyes_front;//�����һ֡û�в�׽����������������һ֡
		}


		for( size_t j = 0; j < eyes.size(); j++ )
		{
			//�þ��α�ע�۾�
			Point p3(p1.x + eyes[j].x,p1.y + eyes[j].y),p4(p3.x + eyes[j].width,p3.y + eyes[j].height);
			rectangle(frame,p3,p4,Scalar( 255, 0, 0 ),2,8,0);
			//��Բ�α�ע�۾�
			//Point center( int(faces[i].x + eyes[j].x + eyes[j].width*0.5), int(faces[i].y + eyes[j].y + eyes[j].height*0.5) ); 
			//int radius = cvRound( (eyes[j].width + eyes[i].height)*0.25 );
			//circle( frame, center, radius, Scalar( 255, 0, 0 ), 3, 8, 0 );
			
			eyeROI = faceROI( eyes[j] );  //�õ���ǰ��ע����������
			//imshow( "�۾�", eyeROI );/////////////
			threshold(eyeROI,eyeROI, threshold_limit, 255, THRESH_BINARY);//���۾��Ҷ�ͼ��ֵ��///////////////////////////////
		    //��ʾ�۾��������
		    imshow( "�۾�", eyeROI );/////////////
			//�����۾�����
			for(k=1;k<eyeROI.rows;k++)
			{
				for(l=1;l<eyeROI.cols;l++)
				{
					if(eyeROI.at<uchar>(k,l)==0)integral++;
				}
			}
			//cout<<integral<<endl;//////////////////////////////////////////////////////////////
			//գ���б��봦��
			if(integral<integral_threshold)//*******************************��һ֡����
			{
				close_eyes_number++;
				//cout<<"����֡��"<<close_eyes_number<<endl;////////////
				//����ʱ�����
				if(close_eyes_number>(close_eyes_time_limit*rate))
				{
					cout<<"����ʱ�䳬�����ƣ�"<<endl;
				}
			    if(eyestate==0)//**************ǰһ֡����
			    {
			        blink_number_all++;
			        blink_record[blink_record_i]=1;
			        cout<<blink_number_all<<endl;//��գ�۴���//////////////////////////////////////
					eyestate=1;
			    }else//************************ǰһ֡����
				{
					blink_record[blink_record_i]=0;
				}
		    }else//*********************************************************��һ֡����
			{
				close_eyes_number=0;
				blink_record[blink_record_i]=0;
				if(eyestate==1)//***************ǰһ֡����
				{
					eyestate=0;
				}else//*************************ǰһ֡����
				{
				}
			}
		}
	}
	blink_record_i++;
	//����ʱ����գ�۴���
	if(blink_record_i==blink_frequency_time*rate)
	{
		for(int i=0;i<blink_frequency_time*rate;i++)
		{
			if(blink_record[i]==1)blink_number++;
		}
		if(blink_number>blink_frequency_number_limit)
		{
			cout<<"գ��Ƶ�ʹ��ߣ�"<<endl;
		}
		blink_record_i=0;
	}
	faces_front=empty;
	eyes_front=empty;
	faces_front=faces;//������һ֡����
	eyes_front=eyes; //������һ֡�۲�
	faces=empty;
	eyes=empty;

	imshow( "�۾����������ټ��", frame );
}