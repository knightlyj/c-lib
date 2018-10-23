/**********************************************
    ʹ������:

    ����
    CL_FsmState fsmStates[] =
    {
        CL_FSM_STATE(PowerUpStart, PowerUpStop, PowerUpUpdate),      //power up
        CL_FSM_STATE(CL_NULL, CL_NULL, WorkingUpdate),      //working
        CL_FSM_STATE(RebootStart, RebootStop, RebootUpdate),      //reboot
    };

    CL_FSM_DEF_INIT(testFsm, fsmStates, PowerUp);

    ����
    CL_FsmUpdate(&testFsm, interval);
 **********************************************/
#include "cl_common.h"

struct CL_Fsm;

//------------����ָ��ԭ��---------------------
typedef void(*CL_StateAction)(CL_Fsm* fsm);
typedef void(*CL_StateUpdate)(CL_Fsm* fsm, uint16_t interval); //��λΪms



struct CL_FsmState
{
    CL_StateAction onStart;	//��ʼ
    CL_StateAction onStop;	//����
    CL_StateUpdate update;	//update
};

struct CL_Fsm
{
    CL_FsmState* states;
    uint8_t statesNum;	//state����
    uint8_t curStateIdx; //��ǰ״̬������
    CL_BOOL initialized;
};


extern void CL_FsmUpdate(CL_Fsm* fsm, uint16_t interval);
extern void CL_FsmChangeState(CL_Fsm* fsm, uint8_t stateIndex);


#define CL_FSM_STATE(start, stop, update) \
    {start, stop, update}

/**
* @brief FSM����,��Ҫ״̬����
*/
#define CL_FSM_DEF_INIT(name, state_array, firstState) \
CL_Fsm name = {state_array, CL_ARRAY_LENGTH(state_array), firstState, CL_FALSE};

/**
* @brief ͬ��,������������
*/
#define CL_FSM_DECL(name) \
extern CL_Fsm name;

