#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int main() {
    // MEMORY
    uint8_t RAM[256], Stack[256];
    uint8_t SP = 0, Out, In1, In2, Func;
    uint8_t Memory[] = {0, 0, 0, 0, 0, 0, 0, RAM[Memory[6]], Stack[SP], 0};
    bool Error = false, CarryFlag = false, ZeroFlag = true, NegFlag = false;
    uint8_t progOut[] = {8, 8, 8, 5, 5, 5, 0};
    uint8_t progIn2[] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t progIn1[] = {1, 2, 3, 8, 8, 8, 0};
    uint8_t progFunc[] = {192, 192, 192, 128, 128, 128, 255};


    // MAIN LOOP
    while (progFunc[Memory[9]] != 255  && Error == false) {
        Memory[0] = 0;
        Func = progFunc[Memory[9]] & 63;
        if ((progFunc[Memory[9]] & 128) == 128) {
            In2 = progIn2[Memory[9]];
        } else {
            In2 = Memory[progIn2[Memory[9]]];
        }
        if ((progFunc[Memory[9]] & 64) == 64) {
            In1 = progIn1[Memory[9]];
        } else {
            In1 = Memory[progIn1[Memory[9]]];
        }
        Out = progOut[Memory[9]];

        if (progOut[Memory[9]] == 8) {
            SP += 1;
        }

        printf("Func: %d\n", Func);
        printf("In1: %d\n", In1);
        printf("In2: %d\n", In2);
        printf("Out: %d\n", Out);

        switch (Func) {
            case 0:
                Memory[Out] = In1 + In2;
                if (Memory[Out] < In1) {
                    CarryFlag = true;
                } else CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if ((Memory[Out] & 128) == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 1:
                Memory[Out] = In1 + (256 - In2 & 255);
                if (Memory[Out] < In1) {
                    CarryFlag = true;
                } else CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if (Memory[Out] & 128 == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 2:
                Memory[Out] = In1 | In2;
                CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if (Memory[Out] & 128 == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 3:
                Memory[Out] = 255 - In1 | In2;
                CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if (Memory[Out] & 128 == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 4:
                Memory[Out] = In1 & In2;
                CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if (Memory[Out] & 128 == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 5:
                Memory[Out] = 255 - (In1 & In2);
                CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if (Memory[Out] & 128 == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 6:
                Memory[Out] = In1 ^ In2;
                CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if (Memory[Out] & 128 == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 7:
                Memory[Out] = 255 - (In1 ^ In2);
                CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if (Memory[Out] & 128 == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 8:
                Memory[Out] = In1 << 1;
                CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if (Memory[Out] & 128 == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 9:
                Memory[Out] = In1 >> 1;
                CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if (Memory[Out] & 128 == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 10:
                Memory[Out] = In1 << 1;
                if (In1 & 128 == 1) {
                    Memory[Out] += 1;
                }
                CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if (Memory[Out] & 128 == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 11:
                Memory[Out] = In1 >> 1;
                if ((In1 & 1) == 1) {
                    Memory[Out] += 128;
                }
                CarryFlag = false;
                if (Memory[Out] == 0) {
                    ZeroFlag = true;
                } else ZeroFlag = false;
                if (Memory[Out] & 128 == 1) {
                    NegFlag = true;
                } else NegFlag = false;
                break;
            case 12:
                if (In1 == In2) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            case 13:
                if (In1 != In2) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            case 14:
                if (In1 >= In2) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            case 15:
                if (In1 < In2) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            case 16:
                if (In1 > In2) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            case 17:
                if (In1 <= In2) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            case 18:
                if (CarryFlag) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            case 19:
                if (!CarryFlag) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            case 20:
                if (NegFlag) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            case 21:
                if (!NegFlag) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            case 22:
                if (ZeroFlag) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            case 23:
                if (!ZeroFlag) {
                    Memory[9] = Memory[6];
                }
                ZeroFlag = true;
                CarryFlag = false;
                NegFlag = false;
                break;
            default:
                printf("Invalid program function at address: %d\n", Memory[9]);
                Error = true;
        }
        if (((progFunc[Memory[9]] & 64) == 0 && progIn1[Memory[9]] == 8) || ((progFunc[Memory[9]] & 128) == 0 && progIn2[Memory[9]] == 8)) {
            SP -= 1;
        }

        printf("SP: %d\n", SP);
        printf("Reg5: %d\n", Memory[5]);
        Memory[9] += 1;
    }
    printf("Program exited successfully\n");

    return 0;
}
