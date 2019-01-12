#include "stdio.h"
#include "stdint.h"

extern void IdentityTest(void);
extern void EqualTest(void);
extern void CopyTest(void);
extern void AddTest(void);
extern void SubTest(void);
extern void ScaleTest();
extern void TransposeTest();
extern void SelfTransposeTest();
extern void MultiplyTest();
extern void InverseTest();

int main()
{
    IdentityTest();
    EqualTest();
    CopyTest();
    AddTest();
    SubTest();
    ScaleTest();
    TransposeTest();
    SelfTransposeTest();
    MultiplyTest();
    InverseTest();

    system("PAUSE");
}
