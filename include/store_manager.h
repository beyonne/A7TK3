#ifndef _STORE_MANAGER_H
#define _STORE_MANAGER_H	 

#include <stdio.h>
#include <stdlib.h>
#include </opt/install/include/sqlite3.h>
#include <string.h>
#include <time.h>




typedef struct DevInfo_Opr
{
	int Sn;			//唯一索引
	int ShortAddr;	//短地址
	int Endpoint;	//通路地址
	int ProfileID;	//Profileid
	int DeviceID;	//设备ID
	int ZoneType ;	//
	int DevState;	//设备状态
	int OnlineState ;//在线状态
	int Val;		//值
	int SnNo;		//SN号
	int BDType1;	//绑定类型1
	int BDTypeSn1;	//绑定类型索引1
	int BDType2;	//绑定类型2
	int BDTypeSn2;	//绑定类型索引2
	int Rs1;		//IEEE 地址
	int Rs2;		//IEEE 地址 8字节
	int Rs3;		//设备布防标志
	int Rs4;		//
	int Rs5;		
	int Namelen;	//名字长度
	unsigned char Name[22];//名字
	
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

/*刷卡储存结构*/
typedef struct PayCardInfoOpr
{
	int   Sn;									//卡片序列号;
	unsigned char  ReaderSn;					//读卡器编号;
	unsigned char  States;						//状态，预留;
	unsigned char  Record_Type;					//记录类型;
	unsigned char  Verif_result;				//验证结果;
	unsigned int Time;							//刷卡时间;
	unsigned char  Reserve[4];					//预留;
}T_PayCardInfo_Opr,*PT_PayCardInfo_Opr;   

/*事件储存结构*/
typedef struct EventStorageOpr
{
	int Channel_Number;							//通道号;
	unsigned char  Event_Type;					//事件类型;
	unsigned char  Evetnt_Len;					//事件长度;
	unsigned char  Event_Source;				//时间来源;	
	unsigned char  Event_Data[8];				//时间数据;
}T_EventStorage_Opr,*PT_EventStorage_Opr;   




typedef struct SqlStoreOpr {	
	char *name;
	//打开数据库，不存在则创建
	//name:数据库名
	int (*sql_open)(char *dbname,sqlite3 *db);

	//关闭数据库连接
	int (*sql_close)(sqlite3 *db);
	
	//创建表
	int (*sql_create_table)(sqlite3 *db,char *tablename);
	
	//删除表
	int (*sql_delete_table)(sqlite3 *db,char *tablename);

	//增加一个表内列、/datatype:为数据类型，唯一约束可以加后面
	int (*sql_add_tablecol)(sqlite3 *db,char *tablename,char *colname,char *datatype);

	//创建索引 create index index_name on table_name(COLNAME);
	int (*sql_create_index)(sqlite3 *db,char *tablename,char *index_name,char *colname);

	//删除索引
	int (*sql_delete_index)(sqlite3 * db,char * index_name);

	//重建索引
	int (*sql_reindex)(sqlite3 *db,char *tablename);
	
	//索引和表的数据分析
	int (*sql_data_analyze)(void);

	//数据清理
	int (*sql_db_clean)(sqlite3 * db);
	
	//开始事物
	int (*sql_begin)(sqlite3 *db);
	
	//提交事物
	int (*sql_commit)(sqlite3 *db);
	
	//创建用户信息表
	int (*sql_create_User_table)(sqlite3 *db,char *tablename);
	
	//插入单条用户数据、插入多条数据时注意使用事物功能
	int (*sql_insert_userinfo)(sqlite3 *db,char *tablename,UserInfo_Struct UserInfo);

	//创建刷卡信息表
	int (*sql_create_PayCard_table)(sqlite3 *db,char *tablename);
	
	//插入单条刷卡记录数据、插入多条数据时注意使用事物功能
	int (*sql_insert_paycardinfo)(sqlite3 *db,char *tablename,T_PayCardInfo_Opr Info);
	
	//创建事件记录信息表
	int (*sql_create_event_table)(sqlite3 *db,char *tablename);
	
	//插入单条事件记录数据、插入多条数据时注意使用事物功能
	int (*sql_insert_eventinfo)(sqlite3 *db,char *tablename,T_EventStorage_Opr Info); 
	
	//查询用户数据
	int (*sql_Select_user_Sn)(sqlite3 *db,char *tablename,int val ,void *param);
	
	//删除用户数据
	int (*sql_delete_table_data)(sqlite3 *db,char *tablename,int val);
	
	//修改用户数据
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

