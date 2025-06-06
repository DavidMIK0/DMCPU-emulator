#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // MEMORY
    uint8_t RAM[256], Stack[256];
    uint8_t SP = 0, Out, In1, In2, Func;
    uint8_t Memory[10] = {0};
    bool Error = false, CarryFlag = false, ZeroFlag = true, NegFlag = false;

    // --- LOAD PROGRAM ARRAYS FROM FILE ---
    uint8_t progOut[256], progIn1[256], progIn2[256], progFunc[256];
    uint8_t progLen = 0;
    FILE *in = fopen("..\\DMCPU assembler\\program.txt", "r");
    if (!in) {
        perror("Could not open program.txt");
        return 1;
    }
    fscanf(in, "%d", &progLen);
    for (int i = 0; i < progLen; i++)
        fscanf(in, "%hhu %hhu %hhu %hhu", &progOut[i], &progIn1[i], &progIn2[i], &progFunc[i]);
    fclose(in);

    // MAIN LOOP
    while (!Error) {
        if (Memory[9] >= progLen) {
            printf("Program counter out of bounds!\n");
            break;
        }
        Memory[0] = 0;
        Func = progFunc[Memory[9]] & 63;

        // ----------- INPUT FETCH WITH STACK POP LOGIC -----------
        // In1
        if ((progFunc[Memory[9]] & 64) == 64) {
            In1 = progIn1[Memory[9]];
        } else {
            if (progIn1[Memory[9]] == 8) {
                SP -= 1;
                In1 = Stack[SP];
            } else if (progIn1[Memory[9]] == 7) {
                In1 = RAM[Memory[6]];
            } else {
                In1 = Memory[progIn1[Memory[9]]];
            }
        }
        // In2
        if ((progFunc[Memory[9]] & 128) == 128) {
            In2 = progIn2[Memory[9]];
        } else {
            if (progIn2[Memory[9]] == 8) {
                SP -= 1;
                In2 = Stack[SP];
            } else if (progIn2[Memory[9]] == 7) {
                In2 = RAM[Memory[6]];
            } else {
                In2 = Memory[progIn2[Memory[9]]];
            }
        }
        Out = progOut[Memory[9]];

        Memory[7] = RAM[Memory[6]];
        Memory[8] = Stack[SP];

        // ALU instructions
        switch (Func) {
            case 0: // add
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = In1 + In2;
                    CarryFlag = (Stack[SP] > 255);
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = In1 + In2;
                    CarryFlag = (RAM[Memory[6]] > 255);
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                } else {
                    Memory[Out] = In1 + In2;
                    CarryFlag = (Memory[Out] > 255);
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                }
                break;
            case 1: // sub
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = (In1 - In2);
                    CarryFlag = !(In1 < In2);
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = (In1 - In2);
                    CarryFlag = !(In1 < In2);
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                } else {
                    Memory[Out] = (In1 - In2) & 0xFF;
                    CarryFlag = !(In1 < In2);
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                }
                break;
            case 2: // or
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = In1 | In2;
                    CarryFlag = false; // OR operation does not set carry
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = In1 | In2;
                    CarryFlag = false; // OR operation does not set carry
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                } else {
                    Memory[Out] = In1 | In2;
                    CarryFlag = false; // OR operation does not set carry
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                }
                break;
            case 3: // nor
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = ~(In1 | In2);
                    CarryFlag = false; // NOR operation does not set carry
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = ~(In1 | In2) & 0xFF;
                    CarryFlag = false; // NOR operation does not set carry
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                } else {
                    Memory[Out] = ~(In1 | In2) & 0xFF;
                    CarryFlag = false; // NOR operation does not set carry
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                }
                break;
            case 4: // and
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = In1 & In2;
                    CarryFlag = false; // AND operation does not set carry
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = In1 & In2;
                    CarryFlag = false; // AND operation does not set carry
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                } else {
                    Memory[Out] = In1 & In2;
                    CarryFlag = false; // AND operation does not set carry
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                }
                break;
            case 5: // nand
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = ~(In1 & In2) & 0xFF;
                    CarryFlag = false; // NAND operation does not set carry
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = ~(In1 & In2) & 0xFF;
                    CarryFlag = false; // NAND operation does not set carry
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                } else {
                    Memory[Out] = ~(In1 & In2) & 0xFF;
                    CarryFlag = false; // NAND operation does not set carry
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                }
                break;
            case 6: // xor
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = In1 ^ In2;
                    CarryFlag = false; // XOR operation does not set carry
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = In1 ^ In2;
                    CarryFlag = false; // XOR operation does not set carry
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                } else {
                    Memory[Out] = In1 ^ In2;
                    CarryFlag = false; // XOR operation does not set carry
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                }
                break;
            case 7: // xnor
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = ~(In1 ^ In2);
                    CarryFlag = false; // XNOR operation does not set carry
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = ~(In1 ^ In2);
                    CarryFlag = false; // XNOR operation does not set carry
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                } else {
                    Memory[Out] = ~(In1 ^ In2);
                    CarryFlag = false; // XNOR operation does not set carry
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                }
                break;
            case 8: // shl
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = (In1 << 1);
                    CarryFlag = (In1 & 0x80) != 0;
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = (In1 << 1);
                    CarryFlag = (In1 & 0x80) != 0;
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                } else {
                    Memory[Out] = (In1 << 1);
                    CarryFlag = (In1 & 0x80) != 0;
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                }
                break;
            case 9: // shr
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = (In1 >> 1);
                    CarryFlag = (In1 & 0x01) != 0;
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = (In1 >> 1);
                    CarryFlag = (In1 & 0x01) != 0;
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                } else {
                    Memory[Out] = (In1 >> 1);
                    CarryFlag = (In1 & 0x01) != 0;
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                }
                break;
            case 10: // rol
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = (In1 << 1) | (In1 >> 7);
                    CarryFlag = (In1 & 0x80) != 0;
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = (In1 << 1) | (In1 >> 7);
                    CarryFlag = (In1 & 0x80) != 0;
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                } else {
                    Memory[Out] = (In1 << 1) | (In1 >> 7);
                    CarryFlag = (In1 & 0x80) != 0;
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                }
                break;
            case 11: // ror
                if (progOut[Memory[9]] == 8) {
                    Stack[SP] = (In1 >> 1) | (In1 << 7);
                    CarryFlag = (In1 & 0x01) != 0;
                    ZeroFlag = (Stack[SP] == 0);
                    NegFlag = (Stack[SP] & 0x80) != 0; // Check if the sign bit is set
                    Stack[SP] &= 0xFF; // Keep it within 8 bits
                    SP += 1;
                } else if (progOut[Memory[9]] == 7) {
                    RAM[Memory[6]] = (In1 >> 1) | (In1 << 7);
                    CarryFlag = (In1 & 0x01) != 0;
                    ZeroFlag = (RAM[Memory[6]] == 0);
                    NegFlag = (RAM[Memory[6]] & 0x80) != 0; // Check if the sign bit is set
                    RAM[Memory[6]] &= 0xFF; // Keep it within 8 bits
                } else {
                    Memory[Out] = (In1 >> 1) | (In1 << 7);
                    CarryFlag = (In1 & 0x01) != 0;
                    ZeroFlag = (Memory[Out] == 0);
                    NegFlag = (Memory[Out] & 0x80) != 0; // Check if the sign bit is set
                    Memory[Out] &= 0xFF; // Keep it within 8 bits
                }
                break;

            // branch instructions
            case 12: // beq
                if (In1 == In2) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 13: // bne
                if (In1 != In2) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 14: // bnl
                if (In1 >= In2) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 15: // bls
                if (In1 < In2) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 16: // bgr
                if (In1 > In2) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 17: // bng
                if (In1 <= In2) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 18: // bic
                if (CarryFlag) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 19: // bnc
                if (!CarryFlag) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 20: // bin
                if (NegFlag) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 21: // bnn
                if (!NegFlag) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 22: // biz
                if (ZeroFlag) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 23: // bnz
                if (!ZeroFlag) {
                    Memory[9] = Memory[6] - 1;
                }
                break;
            case 63: // brk
                printf("Program exited successfully\n");
                Error = true;
                break;
            default:
                printf("Invalid program function at address: %d\n", Memory[9]);
                Error = true;
        }

        if (Error) break;

        Memory[9] += 1;
    }
    printf("Reg5: %d\n", Memory[5]);
    return 0;
}
