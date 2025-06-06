# DMCPU-emulator
An emulator for a CPU that I'm designing made in C

The asm syntax is:
  Out In1 In2 Func

  Out is the address that the result is gonna be stored at. 
  In1, In2 are the inputs for the instructions.
  Func is the instruction that you want to execute.

  reg1 4 0 add // adds 4 and 0 into reg1.
  reg2 10 reg1 sub // subtracts the value from reg1 from 10 and stores the result in reg2.

  When the CPU branches it will always branch to the address that is stored in reg6.
  reg6 also acts as the RAM address.

The instructions are:
  0 / add / adds the inputs and stores the result in the output, if result is greater than 255 it sets the carry flag, if its negative it sets the negative flag, and if its zero it sets the zero flag
  1 / sub / subtracts the inputs and stores the result in the output, if result isnt negative it sets the carry flag, if its negative it sets the negative flag, and if its zero it sets the zero flag
  2 / or / executes the binary or instruction on the inputs and if the result is negative it sets the negative flag, and if its zero it sets the zero flag
  3 / nor / executes the binary nor instruction on the inputs and if the result is negative it sets the negative flag, and if its zero it sets the zero flag
  4 / and / executes the binary and instruction on the inputs and if the result is negative it sets the negative flag, and if its zero it sets the zero flag
  5 / nand / executes the binary nand instruction on the inputs and if the result is negative it sets the negative flag, and if its zero it sets the zero flag
  6 / xor / executes the binary xor instruction on the inputs and if the result is negative it sets the negative flag, and if its zero it sets the zero flag
  7 / xnor / executes the binary xnor instruction on the inputs and if the result is negative it sets the negative flag, and if its zero it sets the zero flag
  8 / shl / executes the binary shift left instruction on the first input, it also sets the carry flag if the result is greater than 255, if its zero it sets the zero flag and if its greater than 127 it sets the negative flag
  9 / shr / executes the binary shift right instruction on the first input, also if the result is zero it sets the zero flag and if its greater than 127 it sets the negative flag
  10 / rol / executes the binary rotate left instruction on the first input, it also sets the carry flag if the result is greater than 255, if its zero it sets the zero flag and if its greater than 127 it sets the negative flag
  11 / ror / 9 / shr / executes the binary rotate right instruction on the first input, also if the result is zero it sets the zero flag and if its greater than 127 it sets the negative flag
  12 / beq / branches the program counter to the address stored in reg6 if the inputs are the same
  13 / bne / branches the program counter to the address stored in reg6 if the inputs arent the same
  14 / bnl / branches if the first input isnt less than the second input
  15 / bls / branches if the first input is less than the second input
  16 / bgr / branches if the first input is greater than the second input
  17 / bng / branches if the first input isnt greater than the second input
  18 / bic / branches if the carry flag is set
  19 / bnc / branches if the carry flag isnt set
  20 / bin / branches if the negative flag is set
  21 / bnn / branches if the negatice flag isnt set
  22 / biz / branches if the zero flag is set
  23 / bnz / branches if the zero flag isnt set
  63 / brk / stops the program
