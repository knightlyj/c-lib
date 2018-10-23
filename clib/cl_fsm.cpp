#include "cl_fsm.h"

void CL_FsmUpdate(CL_Fsm* fsm, uint16_t interval)
{
    CL_StateAction actionFunc;
    CL_StateUpdate updateFunc;

    assert(fsm->curStateIdx < fsm->statesNum);

    if (fsm->initialized == CL_FALSE)
    {
        actionFunc = fsm->states[fsm->curStateIdx].onStart;
        if (actionFunc != CL_NULL)
            actionFunc(fsm);

        fsm->initialized = CL_TRUE;
    }

    updateFunc = fsm->states[fsm->curStateIdx].update;
    if (updateFunc != CL_NULL)
        updateFunc(fsm, interval);
}

void CL_FsmChangeState(CL_Fsm* fsm, uint8_t stateIndex)
{
    CL_StateAction actionFunc;
    assert(fsm->curStateIdx < fsm->statesNum);

    if (stateIndex < fsm->statesNum)
    {
        //停止当前状态
        actionFunc = fsm->states[fsm->curStateIdx].onStop;
        if (actionFunc != CL_NULL)
            actionFunc(fsm);

        //开始新状态
        fsm->curStateIdx = stateIndex;
        actionFunc = fsm->states[fsm->curStateIdx].onStart;
        if (actionFunc != CL_NULL)
            actionFunc(fsm);
    }
}

