/**********************************************
    使用例子:

    定义
    CL_FsmState fsmStates[] =
    {
        CL_FSM_STATE(PowerUpStart, PowerUpStop, PowerUpUpdate),      //power up
        CL_FSM_STATE(CL_NULL, CL_NULL, WorkingUpdate),      //working
        CL_FSM_STATE(RebootStart, RebootStop, RebootUpdate),      //reboot
    };

    CL_FSM_DEF_INIT(testFsm, fsmStates, PowerUp);

    调用
    CL_FsmUpdate(&testFsm, interval);
 **********************************************/
#include "cl_common.h"

struct CL_Fsm;

//------------函数指针原型---------------------
typedef void(*CL_StateAction)(CL_Fsm* fsm);
typedef void(*CL_StateUpdate)(CL_Fsm* fsm, uint16_t interval); //单位为ms



struct CL_FsmState
{
    CL_StateAction onStart;	//开始
    CL_StateAction onStop;	//结束
    CL_StateUpdate update;	//update
};

struct CL_Fsm
{
    CL_FsmState* states;
    uint8_t statesNum;	//state总数
    uint8_t curStateIdx; //当前状态的索引
    CL_BOOL initialized;
};


extern void CL_FsmUpdate(CL_Fsm* fsm, uint16_t interval);
extern void CL_FsmChangeState(CL_Fsm* fsm, uint8_t stateIndex);


#define CL_FSM_STATE(start, stop, update) \
    {start, stop, update}

/**
* @brief FSM定义,需要状态数组
*/
#define CL_FSM_DEF_INIT(name, state_array, firstState) \
CL_Fsm name = {state_array, CL_ARRAY_LENGTH(state_array), firstState, CL_FALSE};

/**
* @brief 同上,但是用来声明
*/
#define CL_FSM_DECL(name) \
extern CL_Fsm name;

