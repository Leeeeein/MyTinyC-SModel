//
// Created by hujianzhe
//

#ifndef	UTIL_C_COMPONENT_DATAQUEUE_H
#define	UTIL_C_COMPONENT_DATAQUEUE_H

#include "../datastructs/list.h"
#include "../sysapi/atomic.h"
#include "../sysapi/ipc.h"

typedef struct DataQueue_t {
	CriticalSection_t m_cslock;
	ConditionVariable_t m_condition;
	List_t m_datalist;
	Atom8_t m_wakeup;
	char m_initok;
} DataQueue_t;

#ifdef __cplusplus
extern "C" {
#endif

__declspec_dll DataQueue_t* dataqueueInit(DataQueue_t* dq);
__declspec_dll void dataqueuePush(DataQueue_t* dq, ListNode_t* data);
__declspec_dll void dataqueuePushList(DataQueue_t* dq, List_t* list);
__declspec_dll ListNode_t* dataqueuePopWait(DataQueue_t* dq, int msec, size_t expect_cnt);
__declspec_dll void dataqueueWake(DataQueue_t* dq);
__declspec_dll ListNode_t* dataqueueClean(DataQueue_t* dq);
__declspec_dll ListNode_t* dataqueueDestroy(DataQueue_t* dq);

#ifdef __cplusplus
}
#endif

#endif
