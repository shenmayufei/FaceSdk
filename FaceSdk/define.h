#ifndef __DEFINE_H__
#define __DEFINE_H__

enum SexType
{
	Female=0,
	Male
};

typedef struct _FaceInfoStruct
{
	unsigned int age; //����
	unsigned int sex; //�Ա� 0 Ů 1 ��
}_FaceInfo;

typedef struct _TrajectoryStruct
{
	unsigned int total ;//������
	unsigned int fall;	//����
	unsigned int punch;	//��ȭ
	unsigned int raise;	//����
	unsigned int nTracker; //��ǰͼƬ�е�����
	int	id;			//id
	int x;			//x����
	int y;			//y����
	int motion;		//�˶���̬ punch = 1, raise_hand = 2, fall = 3
}_TrajectoryInfo;

#endif