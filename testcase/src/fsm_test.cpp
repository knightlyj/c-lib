#include <stdlib.h>
#include <time.h>
#include "stdio.h"
#include "cl_fsm.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "processthreadsapi.h"

using namespace std;

#define PowerUp		0
#define Working		1
#define Reboot		2

//-----------power up-------------------
void PowerUpStart(CL_Fsm* fsm)
{
    cout << "power up start" << endl;
}

void PowerUpUpdate(CL_Fsm* fsm, uint16_t interval)
{
    cout << "power up update " << interval << endl;
    CL_FsmChangeState(fsm, Working);
}

void PowerUpStop(CL_Fsm* fsm)
{
    cout << "power up stop" << endl;
}

//-------------working-----------------
void WorkingUpdate(CL_Fsm* fsm, uint16_t interval)
{
    cout << "working update " << interval << endl;
    CL_FsmChangeState(fsm, Reboot);
}

//------------reboot-----------------
void RebootStart(CL_Fsm* fsm)
{
    cout << "reboot start" << endl;
}

void RebootUpdate(CL_Fsm* fsm, uint16_t interval)
{
    cout << "reboot update " << interval << endl;
    CL_FsmChangeState(fsm, PowerUp);
}

void RebootStop(CL_Fsm* fsm)
{
    cout << "reboot stop" << endl;
}

CL_FsmState fsmStates[] =
{
    { PowerUpStart,     PowerUpStop,        PowerUpUpdate },      //power up
    { CL_NULL,          CL_NULL,            WorkingUpdate },      //working
    { RebootStart,      RebootStop,         RebootUpdate },      //reboot
};

CL_FSM_DEF_INIT(testFsm, fsmStates, PowerUp);

int main(int argc, char **argv)
{
    while (1)
    {
        CL_FsmUpdate(&testFsm, 1000);
        Sleep(1000);
    }

    cout << CL_ARRAY_LENGTH(fsmStates) << endl;
    system("PAUSE");
}