#ifndef _STORE_MANAGER_H
#define _STORE_MANAGER_H	 

#include <stdio.h>
#include <stdlib.h>
#include </opt/install/include/sqlite3.h>
#include <string.h>
#include <time.h>




typedef struct DevInfo_Opr
{
	int Sn;			//Ψһ����
	int ShortAddr;	//�̵�ַ
	int Endpoint;	//ͨ·��ַ
	int ProfileID;	//Profileid
	int DeviceID;	//�豸ID
	int ZoneType ;	//
	int DevState;	//�豸״̬
	int OnlineState ;//����״̬
	int Val;		//ֵ
	int SnNo;		//SN��
	int BDType1;	//������1
	int BDTypeSn1;	//����������1
	int BDType2;	//������2
	int BDTypeSn2;	//����������2
	int Rs1;		//IEEE ��ַ
	int Rs2;		//IEEE ��ַ 8�ֽ�
	int Rs3;		//�豸������־
	int Rs4;		//
	int Rs5;		
	int Namelen;	//���ֳ���
	unsigned char Name[22];//����
	
}T_DevInfo_Opr,*PT_DevInfo_Opr;



typedef struct GroupInfo_Opr
{
	unsigned char Name[22];
	int Namelen;
	int Nature;
	int Uid;
	int Rs1;
	int Rs2;
	
}T_GroupInfo_Opr,*PT_GroupInfo_Opr;


typedef struct LdInfo_Opr
{
	int Sn ;
	int ShortAddr;
	int Endpoint;
	int SensorType ;
	int TriggerType ;
	int TriggerSn;
	int Rs1;
	int Rs2;
	
}T_LdInfo_Opr,*PT_LdInfo_Opr;

typedef struct UserInfoOpr
{
	int Sn;
	unsigned char FtState;
	unsigned char SecState;
	int PassWord;
	int AuthTime;
	int AuthHoli;
	int StartRtc;
	int EndRtc;
	unsigned char *Name;
	unsigned char *Addr;
}UserInfo_Struct,*PT_UserInfoOpr;

/*ˢ������ṹ*/
typedef struct PayCardInfoOpr
{
	int   Sn;									//��Ƭ���к�;
	unsigned char  ReaderSn;					//���������;
	unsigned char  States;						//״̬��Ԥ��;
	unsigned char  Record_Type;					//��¼����;
	unsigned char  Verif_result;				//��֤���;
	unsigned int Time;							//ˢ��ʱ��;
	unsigned char  Reserve[4];					//Ԥ��;
}T_PayCardInfo_Opr,*PT_PayCardInfo_Opr;   

/*�¼�����ṹ*/
typedef struct EventStorageOpr
{
	int Channel_Number;							//ͨ����;
	unsigned char  Event_Type;					//�¼�����;
	unsigned char  Evetnt_Len;					//�¼�����;
	unsigned char  Event_Source;				//ʱ����Դ;	
	unsigned char  Event_Data[8];				//ʱ������;
}T_EventStorage_Opr,*PT_EventStorage_Opr;   




typedef struct SqlStoreOpr {	
	char *name;
	//�����ݿ⣬�������򴴽�
	//name:���ݿ���
	int (*sql_open)(char *dbname,sqlite3 *db);

	//�ر����ݿ�����
	int (*sql_close)(sqlite3 *db);
	
	//������
	int (*sql_create_table)(sqlite3 *db,char *tablename);
	
	//ɾ����
	int (*sql_delete_table)(sqlite3 *db,char *tablename);

	//����һ�������С�/datatype:Ϊ�������ͣ�ΨһԼ�����ԼӺ���
	int (*sql_add_tablecol)(sqlite3 *db,char *tablename,char *colname,char *datatype);

	//�������� create index index_name on table_name(COLNAME);
	int (*sql_create_index)(sqlite3 *db,char *tablename,char *index_name,char *colname);

	//ɾ������
	int (*sql_delete_index)(sqlite3 * db,char * index_name);

	//�ؽ�����
	int (*sql_reindex)(sqlite3 *db,char *tablename);
	
	//�����ͱ�����ݷ���
	int (*sql_data_analyze)(void);

	//��������
	int (*sql_db_clean)(sqlite3 * db);
	
	//��ʼ����
	int (*sql_begin)(sqlite3 *db);
	
	//�ύ����
	int (*sql_commit)(sqlite3 *db);
	
	//�����û���Ϣ��
	int (*sql_create_User_table)(sqlite3 *db,char *tablename);
	
	//���뵥���û����ݡ������������ʱע��ʹ�����﹦��
	int (*sql_insert_userinfo)(sqlite3 *db,char *tablename,UserInfo_Struct UserInfo);

	//����ˢ����Ϣ��
	int (*sql_create_PayCard_table)(sqlite3 *db,char *tablename);
	
	//���뵥��ˢ����¼���ݡ������������ʱע��ʹ�����﹦��
	int (*sql_insert_paycardinfo)(sqlite3 *db,char *tablename,T_PayCardInfo_Opr Info);
	
	//�����¼���¼��Ϣ��
	int (*sql_create_event_table)(sqlite3 *db,char *tablename);
	
	//���뵥���¼���¼���ݡ������������ʱע��ʹ�����﹦��
	int (*sql_insert_eventinfo)(sqlite3 *db,char *tablename,T_EventStorage_Opr Info); 
	
	//��ѯ�û�����
	int (*sql_Select_user_Sn)(sqlite3 *db,char *tablename,int val ,void *param);
	
	//ɾ���û�����
	int (*sql_delete_table_data)(sqlite3 *db,char *tablename,int val);
	
	//�޸��û�����
	int (*sql_update_table_data)(sqlite3 *db,char *tablename,int val,UserInfo_Struct UserInfo);

	int (*sql_create_Dev_table)(sqlite3 *db,char *tablename);
	int (*sql_insert_Devinfo)(sqlite3 *db,char *tablename,PT_DevInfo_Opr Info);
	int (*sql_update_Devtable)(sqlite3 *db,char *tablename,int ShortAddr,int Endpoint,PT_DevInfo_Opr Info);
	int (*sql_Select_Dev)(sqlite3 *db,char *tablename,int ShortAddr,int Endpoint,void *param);
	int (*sql_delete_Dev)(sqlite3 *db,char *tablename,int ShortAddr,int Endpoint);
	int (*sql_Select_AllDev)(sqlite3 *db,char *tablename,int id,void *param);
	int (*sqlSelectMaxid)(sqlite3 *db,char *tablename,void *param);
	int (*sql_create_group_table)(sqlite3 *db,char *tablename);
	int (*sql_Select_Group)(sqlite3 *db,char *tablename,char *Name ,void *param);
	int (*sql_insert_Groupinfo)(sqlite3 *db,char *tablename,PT_GroupInfo_Opr Info);
	int (*sql_update_Grouptable)(sqlite3 *db,char *tablename,char *name,PT_GroupInfo_Opr Info);
	int (*sql_delete_Group)(sqlite3 *db,char *tablename,char *name);
	int (*sql_Select_GroupUid)(sqlite3 *db,char *tablename,int uid ,void *param);
	int (*sql_create_Ld_table)(sqlite3 * db,char * tablename);
	int (*sql_Select_Ld)(sqlite3 * db,char * tablename,int Sn,int SensorType,int TriggerType,void * param);
	int (*sql_delete_Ld)(sqlite3 * db,char * tablename,int Sn,int SensorType,int TriggerType);
	int (*sql_insert_Ldinfo)(sqlite3 * db,char * tablename,PT_LdInfo_Opr Info);
	int (*sql_update_Ldinfo)(sqlite3 *db,char *tablename,int Sn,int SensorType,int TriggerType ,PT_LdInfo_Opr Info);
	struct SqlStoreOpr *ptNext;
}T_SqlStoreOpr, *PT_SqlStoreOpr;



typedef struct FileStoreOpr {	
	char *name;
	int (*WriteFile)(char *name,char *p,int Len);
	int (*ReadFile)(char *name,char *p,int Len);

	struct FileStoreOpr *ptNext;
}T_FileStoreOpr, *PT_FileStoreOpr;


typedef struct Selcet_Opr
{
	int colnum;
	char **ColName;
	char **ColVal;
}T_Selcet_Opr,*PT_Selcet_Opr;


#define CALLBACK(CB) void (CB)(void *pvParam,int nColumn,char **ColVal,char **ColName)


typedef struct ReceParam {
	void *data;                   // pointer to user specific data
	CALLBACK(*cb);       // function to call after reading
} ReceParam;







extern sqlite3 *db;
int StoreInit(void);
int FileStoreInit(void);
int SqlStoreInit(void);
int open_sqlite3(char *dbname);
PT_SqlStoreOpr GetDefultSqlOpr(void);
PT_FileStoreOpr GetDefultFileOpr(void);
int DeleteDev(int ShortAddr,int Endpoint);


#endif

