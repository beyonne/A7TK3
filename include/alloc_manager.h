#ifndef _ALLOC_MANAGER_H_
#define _ALLOC_MANAGER_H_



#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void *)0) ///< NULL is defined in a somehow save manner
#endif

#ifndef FALSE
#define FALSE (0)        ///< FALSE is equal to 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)    ///< TRUE  is not equal to FALSE
#endif


#define MEMSIZE	(2048*5)
#define MEMYLSIZE  300


/* VideoMem的状态:
 * 空闲/用于预先准备显示内容/用于当前线程
 */
typedef enum {
	NMS_FREE = 0,
	NMS_USED_FOR_CUR,	
}E_NetMemState;

typedef struct NetMemOpr{	
	int ID;
	int HeadAddr;
	int EndAddr;
	int DataLen;
	E_NetMemState eMemState;
	unsigned char *DataAddr;
	struct NetMemOpr *ptNext;
}T_MemOpr, *PT_MemOpr;


int AllocMem(int iNum);
PT_MemOpr GetMem(int iID);
PT_MemOpr GetMemData(int iID);
int PutMem(int iID);
void ShowMem(void);
int GetDataNum(int fd);







#ifdef __cplusplus
}
#endif

#endif /* _H_ */
