#ifndef COMMONDEF_VPR_H
#define COMMONDEF_VPR_H

/*
1.11���Ͷ���
*/
typedef struct _vlp_info
{
	int vlpInfoSize;				//ʶ��ṹ���С
	char vlpTime[20];          //ʶ��ʱ�䣬��ʽ��yyyyMMddHHmmsszzz��
	int vlpCarClass;				//����
	unsigned char vlpColor[2];	//������ɫ(���ֱ���)
	//��00����ɫ����01����ɫ����02����ɫ����03����ɫ
	unsigned char vlpText[16];	//�������֣�GBK����
	unsigned int vlpReliability; //ʶ���ƿ��Ŷ�(������λ��ʾ9999��ʾΪ99.99%)
	unsigned int imageLength[3];	//ʶ��ͼƬ����:[0]=����ͼ���ȣ�[1]=����ͼ���ȣ�[2]=��ֵ��ͼ����
	unsigned char* image[3];		//ʶ��ͼƬ��[0]=����ͼ��[1]=����ͼ��[2]=��ֵ��ͼ
}T_VLPINFO;


/* �豸״̬�ص�����CBFun_GetDevStatus
����ֵ	����ֵ����	����ֵ˵��
int	--
����	����	��������	����	����	����
����	nHandle		int 	4	�豸���
nStatus		int		4	������,0��ʾ����
pUser		void*	4	�û��Զ�������
����	�����豸״̬��Ϣ
*/
//typedef void(WINAPI *CBFun_GetDevStatus)(int nHandle, int nStatus, void* pUser);
typedef void( *CBFun_GetDevStatus)(int nHandle, int nStatus, void* pUser);

/* ʶ�����ص�����CBFun_GetRegResult
��������
����ֵ	����ֵ����	����ֵ˵��
void	--
����	����	��������	����		����	����
����	nHandle		int 		4		�豸���
pVlpResult	T_VLPINFO*	4		ʶ�����ṹ��
pUser		void*		4		�û��Զ�������
����	����ץ��ʶ����
*/
//typedef void(WINAPI *CBFun_GetRegResult)(int nHandle, T_VLPINFO* pVlpResult, void *pUser);
typedef void( *CBFun_GetRegResult)(int nHandle, T_VLPINFO* pVlpResult, void *pUser);

#endif