// Disassembler_Alpha.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <vector>
#include <stdlib.h>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;


#define _E_ 0x1000
#define _G_ 0x2000
#define _M_ 0x3000
#define _I_ 0x4000
#define _Y_ 0x5000
#define _X_ 0x6000
#define _J_ 0x7000
#define _S_ 0x8000
#define _R_ 0x9000
#define _A_ 0xa000
#define _F_ 0xb000
#define _O_ 0xc000

#define _b_ 0x40
#define _v_ 0x50
#define _w_ 0x60
#define _z_ 0x70
#define _a_ 0x80
#define _p_ 0x90

#define _INVALID_OP_ 0x44444
#define _MODreg_ 0x99999

#define _Eb_ 0x1040
#define _Ev_ 0x1050
#define _Ew_ 0x1060
#define _Gb_ 0x2040
#define _Gv_ 0x2050
#define _Gw_ 0x2060
#define _Gz_ 0x2070
#define _Mw_ 0x3060
#define _Ma_ 0x3080
#define _Mp_ 0x3090
#define _Ib_ 0x4040
#define _Iv_ 0x4050
#define _Iw_ 0x4060
#define _Iz_ 0x4070
#define _Yb_ 0x5040
#define _Yv_ 0x5050
#define _Yz_ 0x5070
#define _Xb_ 0x6040
#define _Xv_ 0x6050
#define _Xz_ 0x6070
#define _Jb_ 0x7040
#define _Jz_ 0x7070
#define _Sw_ 0x8060
#define _Rv_ 0x9050
#define _Ap_ 0xa090
#define _Fv_ 0xb050
#define _Ob_ 0xc040
#define _Ov_ 0xc050

#define _AL_ 0x10
#define _CL_ 0x11
#define _DL_ 0x12
#define _BL_ 0x13
#define _AH_ 0x14
#define _CH_ 0x15
#define _DH_ 0x16
#define _BH_ 0x17
#define _AX_ 0x18
#define _CX_ 0x19
#define _DX_ 0x1a
#define _BX_ 0x1b
#define _SP_ 0x1c
#define _BP_ 0x1d
#define _SI_ 0x1e
#define _DI_ 0x1f
#define _ES_ 0x20
#define _CS_ 0x21
#define _SS_ 0x22
#define _DS_ 0x23
#define _GS_ 0x24
#define _FS_ 0x25


typedef struct _prefixTable
{
	int prefix_code;
	int prefix_group;
	char* prefix_text;
}prefixTable, *PprefixTable;

prefixTable g_prefixTable[14] = {
	{ 0xf0, 1, "LOCK" },
	{ 0xf2, 1, "REPNE" },
	{ 0xf2, 1, "REPNZ" },
	{ 0xf3, 1, "REP" },
	{ 0xf3, 1, "REPE" },
	{ 0xf3, 1, "REPZ" },
	{ 0x2e, 2, "CS:" },
	{ 0x36, 2, "SS:" },
	{ 0x3e, 2, "DS:" },
	{ 0x26, 2, "ES:" },
	{ 0x64, 2, "FS:" },
	{ 0x65, 2, "GS:" },
	{ 0x66, 3, "" },
	{ 0x67, 4, "" }
};

typedef struct _opCodeTable
{
	int op_code;
	bool op_modRM;
	int	op_operands[3];
	char* op_text;

}opCodeTable, *PopCodeTable;

opCodeTable g_opCodeTable_oneByte[256] = {
	{ 0x00, true, { _Eb_, _Gb_, _INVALID_OP_ }, "ADD" },
	{ 0x01, true, { _Ev_, _Gv_, _INVALID_OP_ }, "ADD" },
	{ 0x02, true, { _Gb_, _Eb_, _INVALID_OP_ }, "ADD" },
	{ 0x03, true, { _Gb_, _Eb_, _INVALID_OP_ }, "ADD" },
	{ 0x04, false, { _AL_, _Ib_, _INVALID_OP_ }, "ADD" },
	{ 0x05, false, { _AX_, _Iz_, _INVALID_OP_ }, "ADD" },
	{ 0x06, false, { _ES_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x07, false, { _ES_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x08, true, { _Eb_, _Gb_, _INVALID_OP_ }, "OR" },
	{ 0x09, true, { _Ev_, _Gv_, _INVALID_OP_ }, "OR" },
	{ 0xa, true, { _Gb_, _Eb_, _INVALID_OP_ }, "OR" },
	{ 0xb, true, { _Gv_, _Ev_, _INVALID_OP_ }, "OR" },
	{ 0xc, false, { _AL_, _Ib_, _INVALID_OP_ }, "OR" },
	{ 0xd, false, { _AX_, _Iz_, _INVALID_OP_ }, "OR" },
	{ 0xe, false, { _CS_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0xf, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "" },
	{ 0x10, true, { _Eb_, _Gb_, _INVALID_OP_ }, "ADC" },
	{ 0x11, true, { _Ev_, _Gv_, _INVALID_OP_ }, "ADC" },
	{ 0x12, true, { _Gb_, _Eb_, _INVALID_OP_ }, "ADC" },
	{ 0x13, true, { _Gv_, _Ev_, _INVALID_OP_ }, "ADC" },
	{ 0x14, false, { _AL_, _Ib_, _INVALID_OP_ }, "ADC" },
	{ 0x15, false, { _AX_, _Iz_, _INVALID_OP_ }, "ADC" },
	{ 0x16, false, { _SS_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x17, false, { _SS_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x18, true, { _Eb_, _Gb_, _INVALID_OP_ }, "SBB" },
	{ 0x19, true, { _Ev_, _Gv_, _INVALID_OP_ }, "SBB" },
	{ 0x1a, true, { _Gb_, _Eb_, _INVALID_OP_ }, "SBB" },
	{ 0x1b, true, { _Gv_, _Ev_, _INVALID_OP_ }, "SBB" },
	{ 0x1c, false, { _AL_, _Ib_, _INVALID_OP_ }, "SBB" },
	{ 0x1d, false, { _AX_, _Iz_, _INVALID_OP_ }, "SBB" },
	{ 0x1e, false, { _DS_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x1f, false, { _DS_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x20, true, { _Eb_, _Gb_, _INVALID_OP_ }, "AND" },
	{ 0x21, true, { _Ev_, _Gv_, _INVALID_OP_ }, "AND" },
	{ 0x22, true, { _Gb_, _Eb_, _INVALID_OP_ }, "AND" },
	{ 0x23, true, { _Gv_, _Ev_, _INVALID_OP_ }, "AND" },
	{ 0x24, false, { _AL_, _Ib_, _INVALID_OP_ }, "AND" },
	{ 0x25, false, { _AX_, _Iz_, _INVALID_OP_ }, "AND" },
	{ 0x26, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "" },
	{ 0x27, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "DAA" },
	{ 0x28, true, { _Eb_, _Gb_, _INVALID_OP_ }, "SUB" },
	{ 0x29, true, { _Ev_, _Gv_, _INVALID_OP_ }, "SUB" },
	{ 0x2a, true, { _Gb_, _Eb_, _INVALID_OP_ }, "SUB" },
	{ 0x2b, true, { _Gv_, _Ev_, _INVALID_OP_ }, "SUB" },
	{ 0x2c, false, { _AL_, _Ib_, _INVALID_OP_ }, "SUB" },
	{ 0x2d, false, { _AX_, _Iz_, _INVALID_OP_ }, "SUB" },
	{ 0x2e, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "" },
	{ 0x2f, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "DAS" },
	{ 0x30, true, { _Eb_, _Gb_, _INVALID_OP_ }, "XOR" },
	{ 0x31, true, { _Ev_, _Gv_, _INVALID_OP_ }, "XOR" },
	{ 0x32, true, { _Gb_, _Eb_, _INVALID_OP_ }, "XOR" },
	{ 0x33, true, { _Gv_, _Ev_, _INVALID_OP_ }, "XOR" },
	{ 0x34, false, { _AL_, _Ib_, _INVALID_OP_ }, "XOR" },
	{ 0x35, false, { _AX_, _Iz_, _INVALID_OP_ }, "XOR" },
	{ 0x36, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "" },
	{ 0x37, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "AAA" },
	{ 0x38, true, { _Eb_, _Gb_, _INVALID_OP_ }, "CMP" },
	{ 0x39, true, { _Ev_, _Gv_, _INVALID_OP_ }, "CMP" },
	{ 0x3a, true, { _Gb_, _Eb_, _INVALID_OP_ }, "CMP" },
	{ 0x3b, true, { _Gv_, _Ev_, _INVALID_OP_ }, "CMP" },
	{ 0x3c, false, { _AL_, _Ib_, _INVALID_OP_ }, "CMP" },
	{ 0x3d, false, { _AX_, _Iz_, _INVALID_OP_ }, "CMP" },
	{ 0x3e, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "" },
	{ 0x3f, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "AAS" },
	{ 0x40, false, { _AX_, _INVALID_OP_, _INVALID_OP_ }, "INC" },
	{ 0x41, false, { _CX_, _INVALID_OP_, _INVALID_OP_ }, "INC" },
	{ 0x42, false, { _DX_, _INVALID_OP_, _INVALID_OP_ }, "INC" },
	{ 0x43, false, { _BX_, _INVALID_OP_, _INVALID_OP_ }, "INC" },
	{ 0x44, false, { _SP_, _INVALID_OP_, _INVALID_OP_ }, "INC" },
	{ 0x45, false, { _BP_, _INVALID_OP_, _INVALID_OP_ }, "INC" },
	{ 0x46, false, { _SI_, _INVALID_OP_, _INVALID_OP_ }, "INC" },
	{ 0x47, false, { _DI_, _INVALID_OP_, _INVALID_OP_ }, "INC" },
	{ 0x48, false, { _AX_, _INVALID_OP_, _INVALID_OP_ }, "DEC" },
	{ 0x49, false, { _CX_, _INVALID_OP_, _INVALID_OP_ }, "DEC" },
	{ 0x4a, false, { _DX_, _INVALID_OP_, _INVALID_OP_ }, "DEC" },
	{ 0x4b, false, { _BX_, _INVALID_OP_, _INVALID_OP_ }, "DEC" },
	{ 0x4c, false, { _SP_, _INVALID_OP_, _INVALID_OP_ }, "DEC" },
	{ 0x4d, false, { _BP_, _INVALID_OP_, _INVALID_OP_ }, "DEC" },
	{ 0x4e, false, { _SI_, _INVALID_OP_, _INVALID_OP_ }, "DEC" },
	{ 0x4f, false, { _DI_, _INVALID_OP_, _INVALID_OP_ }, "DEC" },
	{ 0x50, false, { _AX_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x51, false, { _CX_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x52, false, { _DX_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x53, false, { _BX_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x54, false, { _SP_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x55, false, { _BP_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x56, false, { _SI_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x57, false, { _DI_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x58, false, { _AX_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x59, false, { _CX_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x5a, false, { _DX_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x5b, false, { _BX_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x5c, false, { _SP_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x5d, false, { _BP_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x5e, false, { _SI_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x5f, false, { _DI_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x60, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "PUSHAD" },
	{ 0x61, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "POPAD" },
	{ 0x62, true, { _Gv_, _Ma_, _INVALID_OP_ }, "BOUND" },
	{ 0x63, true, { _Ew_, _Gw_, _INVALID_OP_ }, "ARPL" },
	{ 0x64, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "" },
	{ 0x65, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "" },
	{ 0x66, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "" },
	{ 0x67, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "" },
	{ 0x68, false, { _Iz_, _INVALID_OP_, _INVALID_OP_ }, "PUSH" },
	{ 0x69, true, { _Gv_, _Ev_, _Iz_ }, "IMUL" },
	{ 0x6a, false, { _Ib_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x6b, true, { _Gv_, _Ev_, _Ib_ }, "IMUL" },
	{ 0x6c, false, { _Yb_, _DX_, _INVALID_OP_ }, "INS" },
	{ 0x6d, false, { _Yz_, _DX_, _INVALID_OP_ }, "INS" },
	{ 0x6e, false, { _DX_, _Xb_, _INVALID_OP_ }, "OUTS" },
	{ 0x6f, false, { _DX_, _Yz_, _INVALID_OP_ }, "OUTS" },
	{ 0x70, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JO" },
	{ 0x71, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JNO" },
	{ 0x72, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JB" },
	{ 0x73, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JNB" },
	{ 0x74, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JZ" },
	{ 0x75, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JNZ" },
	{ 0x76, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JBE" },
	{ 0x77, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JNBE" },
	{ 0x78, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JS" },
	{ 0x79, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JNS" },
	{ 0x7a, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JP" },
	{ 0x7b, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JNP" },
	{ 0x7c, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JL" },
	{ 0x7d, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JNL" },
	{ 0x7e, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JLE" },
	{ 0x7f, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JNLE" },
	{ 0x80, true, { _MODreg_, _Eb_, _Ib_ }, "" },
	{ 0x81, true, { _MODreg_, _Ev_, _Iz_ }, "" },
	{ 0x82, true, { _MODreg_, _Eb_, _Ib_ }, "" },
	{ 0x83, true, { _MODreg_, _Ev_, _Ib_ }, "" },
	{ 0x84, true, { _Eb_, _Gb_, _INVALID_OP_ }, "TEST" },
	{ 0x85, true, { _Ev_, _Gv_, _INVALID_OP_ }, "TEST" },
	{ 0x86, true, { _Eb_, _Gb_, _INVALID_OP_ }, "XCHG" },
	{ 0x87, true, { _Ev_, _Gb_, _INVALID_OP_ }, "XCHG" },
	{ 0x88, true, { _Eb_, _Gb_, _INVALID_OP_ }, "MOV" },
	{ 0x89, true, { _Ev_, _Gv_, _INVALID_OP_ }, "MOV" },
	{ 0x8a, true, { _Gb_, _Eb_, _INVALID_OP_ }, "MOV" },
	{ 0x8b, true, { _Gv_, _Ev_, _INVALID_OP_ }, "MOV" },
	{ 0x8c, true, { _Ev_, _Sw_, _INVALID_OP_ }, "MOV" },
	{ 0x8d, true, { _Gv_, _M_, _INVALID_OP_ }, "LEA" },
	{ 0x8e, true, { _Sw_, _Ew_, _INVALID_OP_ }, "MOV" },
	{ 0x8f, true, { _Ev_, _INVALID_OP_, _INVALID_OP_ }, "POP" },
	{ 0x90, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "NOP" },
	{ 0x91, false, { _CX_, _AX_, _INVALID_OP_ }, "XCHG" },
	{ 0x92, false, { _DX_, _AX_, _INVALID_OP_ }, "XCHG" },
	{ 0x93, false, { _BX_, _AX_, _INVALID_OP_ }, "XCHG" },
	{ 0x94, false, { _SP_, _AX_, _INVALID_OP_ }, "XCHG" },
	{ 0x95, false, { _BP_, _AX_, _INVALID_OP_ }, "XCHG" },
	{ 0x96, false, { _SI_, _AX_, _INVALID_OP_ }, "XCHG" },
	{ 0x97, false, { _DI_, _AX_, _INVALID_OP_ }, "XCHG" },
	{ 0x98, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "CWDE" },
	{ 0x99, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "CDQ" },
	{ 0x9a, false, { _Ap_, _INVALID_OP_, _INVALID_OP_ }, "CALL" },
	{ 0x9b, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "WAIT" },
	{ 0x9c, false, { _Fv_, _INVALID_OP_, _INVALID_OP_ }, "PUSHFD" },
	{ 0x9d, false, { _Fv_, _INVALID_OP_, _INVALID_OP_ }, "POPFD" },
	{ 0x9e, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "SAHF" },
	{ 0x9f, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "LAHF" },
	{ 0xa0, false, { _AL_, _Ob_, _INVALID_OP_ }, "MOV" },
	{ 0xa1, false, { _AX_, _Ov_, _INVALID_OP_ }, "MOV" },
	{ 0xa2, false, { _Ob_, _AL_, _INVALID_OP_ }, "MOV" },
	{ 0xa3, false, { _Ov_, _AX_, _INVALID_OP_ }, "MOV" },
	{ 0xa4, false, { _Yb_, _Xb_, _INVALID_OP_ }, "MOVS" },
	{ 0xa5, false, { _Yv_, _Xv_, _INVALID_OP_ }, "MOVS" },
	{ 0xa6, false, { _Xb_, _Yb_, _INVALID_OP_ }, "CMPS" },
	{ 0xa7, false, { _Xv_, _Yv_, _INVALID_OP_ }, "CMPS" },
	{ 0xa8, false, { _AL_, _Ib_, _INVALID_OP_ }, "TEST" },
	{ 0xa9, false, { _AX_, _Iz_, _INVALID_OP_ }, "TEST" },
	{ 0xaa, false, { _Yv_, _AL_, _INVALID_OP_ }, "STOS" },
	{ 0xab, false, { _Yv_, _AX_, _INVALID_OP_ }, "STOS" },
	{ 0xac, false, { _AL_, _Xb_, _INVALID_OP_ }, "LODS" },
	{ 0xad, false, { _AX_, _Xv_, _INVALID_OP_ }, "LODS" },
	{ 0xae, false, { _AL_, _Yb_, _INVALID_OP_ }, "SCAS" },
	{ 0xaf, false, { _AX_, _Yv_, _INVALID_OP_ }, "SCAS" },
	{ 0xb0, false, { _AL_, _Ib_, _INVALID_OP_ }, "MOV" },
	{ 0xb1, false, { _CL_, _Ib_, _INVALID_OP_ }, "MOV" },
	{ 0xb2, false, { _DL_, _Ib_, _INVALID_OP_ }, "MOV" },
	{ 0xb3, false, { _BL_, _Ib_, _INVALID_OP_ }, "MOV" },
	{ 0xb4, false, { _AH_, _Ib_, _INVALID_OP_ }, "MOV" },
	{ 0xb5, false, { _CH_, _Ib_, _INVALID_OP_ }, "MOV" },
	{ 0xb6, false, { _DH_, _Ib_, _INVALID_OP_ }, "MOV" },
	{ 0xb7, false, { _BH_, _Ib_, _INVALID_OP_ }, "MOV" },
	{ 0xb8, false, { _AX_, _Iv_, _INVALID_OP_ }, "MOV" },
	{ 0xb9, false, { _CX_, _Iv_, _INVALID_OP_ }, "MOV" },
	{ 0xba, false, { _DX_, _Iv_, _INVALID_OP_ }, "MOV" },
	{ 0xbb, false, { _BX_, _Iv_, _INVALID_OP_ }, "MOV" },
	{ 0xbc, false, { _SP_, _Iv_, _INVALID_OP_ }, "MOV" },
	{ 0xbd, false, { _BP_, _Iv_, _INVALID_OP_ }, "MOV" },
	{ 0xbe, false, { _SI_, _Iv_, _INVALID_OP_ }, "MOV" },
	{ 0xbf, false, { _DI_, _Iv_, _INVALID_OP_ }, "MOV" },
	{ 0xc0, true, { _MODreg_, _Eb_, _Ib_ }, "" },
	{ 0xc1, true, { _MODreg_, _Ev_, _Ib_ }, "" },
	{ 0xc2, false, { _Iw_, _INVALID_OP_, _INVALID_OP_ }, "RETN" },
	{ 0xc3, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "RETN" },
	{ 0xc4, true, { _Gz_, _Mp_, _INVALID_OP_ }, "LES" },
	{ 0xc5, true, { _Gz_, _Mp_, _INVALID_OP_ }, "LDS" },
	{ 0xc6, true, { _MODreg_, _Eb_, _Iz_ }, "" },
	{ 0xc7, true, { _MODreg_, _Ev_, _Iz_ }, "" },
	{ 0xc8, false, { _Iw_, _Ib_, _INVALID_OP_ }, "ENTER" },
	{ 0xc9, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "LEAVE" },
	{ 0xca, false, { _Iw_, _INVALID_OP_, _INVALID_OP_ }, "RETN" },
	{ 0xcb, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "RETN" },
	{ 0xcc, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "INT 3" },
	{ 0xcd, false, { _Ib_, _INVALID_OP_, _INVALID_OP_ }, "INT" },
	{ 0xce, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "INT 0" },
	{ 0xcf, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "IRETD" },
	{ 0xd0, true, { _MODreg_, _Eb_, 1 }, "" },
	{ 0xd1, true, { _MODreg_, _Ev_, 1 }, "" },
	{ 0xd2, true, { _MODreg_, _Eb_, _CL_ }, "" },
	{ 0xd3, true, { _MODreg_, _Ev_, _CL_ }, "" },
	{ 0xd4, false, { _Ib_, _INVALID_OP_, _INVALID_OP_ }, "AAM" },
	{ 0xd5, false, { _Ib_, _INVALID_OP_, _INVALID_OP_ }, "AAD" },
	{ 0xd6, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "SALC" },
	{ 0xd7, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "XLAT" },
	{ 0xd8, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "ESC" },
	{ 0xd9, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "ESC" },
	{ 0xda, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "ESC" },
	{ 0xdb, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "ESC" },
	{ 0xdc, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "ESC" },
	{ 0xdd, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "ESC" },
	{ 0xde, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "ESC" },
	{ 0xdf, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "ESC" },
	{ 0xe0, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "LOOPNZ" },
	{ 0xe1, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "LOOPZ" },
	{ 0xe2, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "LOOP" },
	{ 0xe3, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JECXZ" },
	{ 0xe4, false, { _AL_, _Ib_, _INVALID_OP_ }, "IN" },
	{ 0xe5, false, { _AX_, _Ib_, _INVALID_OP_ }, "IN" },
	{ 0xe6, false, { _Ib_, _AL_, _INVALID_OP_ }, "OUT" },
	{ 0xe7, false, { _Ib_, _AX_, _INVALID_OP_ }, "OUT" },
	{ 0xe8, false, { _Jz_, _INVALID_OP_, _INVALID_OP_ }, "CALL" },
	{ 0xe9, false, { _Jz_, _INVALID_OP_, _INVALID_OP_ }, "JMP" },
	{ 0xea, false, { _Ap_, _INVALID_OP_, _INVALID_OP_ }, "JMP" },
	{ 0xeb, false, { _Jb_, _INVALID_OP_, _INVALID_OP_ }, "JMP" },
	{ 0xec, false, { _AL_, _DX_, _INVALID_OP_ }, "IN" },
	{ 0xed, false, { _AX_, _DX_, _INVALID_OP_ }, "IN" },
	{ 0xee, false, { _DX_, _AL_, _INVALID_OP_ }, "OUT" },
	{ 0xef, false, { _DX_, _AX_, _INVALID_OP_ }, "OUT" },
	{ 0xf0, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "LOCK:" },
	{ 0xf1, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "INT 1" },
	{ 0xf2, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "REPNZ:" },
	{ 0xf3, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "REP:" },
	{ 0xf4, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "HLT" },
	{ 0xf5, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "CMC" },
	{ 0xf6, true, { _MODreg_, _Eb_, _Ib_ }, "" },
	{ 0xf7, true, { _MODreg_, _Ev_, _Iz_ }, "" },
	{ 0xf8, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "CLC" },
	{ 0xf9, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "STC" },
	{ 0xfa, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "CLI" },
	{ 0xfb, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "STI" },
	{ 0xfc, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "CLD" },
	{ 0xfd, false, { _INVALID_OP_, _INVALID_OP_, _INVALID_OP_ }, "STD" },
	{ 0xfe, true, { _MODreg_, _Eb_, _INVALID_OP_ }, "" },
	{ 0xff, true, { _MODreg_, _Ev_, _INVALID_OP_ }, "" }
};

typedef struct _ModRM_reg
{
	char* Mod_reg_r8;
	char* Mod_reg_r16;
	char* Mod_reg_r32;
	char* Mod_reg_mm;
	char* Mod_reg_xmm;
}ModRM_reg, *PModRM_reg;

ModRM_reg ModRM_REG_Table[8] = {
	{ "AL", "AX", "EAX", "MM0", "XMM0" },
	{ "CL", "CX", "ECX", "MM1", "XMM1" },
	{ "DL", "DX", "EDX", "MM2", "XMM2" },
	{ "BL", "BX", "EBX", "MM3", "XMM3" },
	{ "AH", "SP", "ESP", "MM4", "XMM4" },
	{ "CH", "BP", "EBP", "MM5", "XMM5" },
	{ "DH", "SI", "ESI", "MM6", "XMM6" },
	{ "BH", "DI", "EDI", "MM7", "XMM7" }
};
//[N]=ModRM Byte%64/8 reg


typedef struct _ModRM_rm
{
	char* Mod_rm;
	int disp;
	bool sib;
}ModRM_rm, *PModRM_rm;

ModRM_rm modRM_rm_Table[3][8] = {
	{
		{ "EAX", 0, false },
		{ "ECX", 0, false },
		{ "EDX", 0, false },
		{ "EBX", 0, false },
		{ "", 0, true },
		{ "[", 4, false },
		{ "ESI", 0, false },
		{ "EDI", 0, false }
	},
	{
		{ "[EAX", 1, false },
		{ "[ECX", 1, false },
		{ "[EDX", 1, false },
		{ "[EBX", 1, false },
		{ "", 1, true },
		{ "[EBP", 1, false },
		{ "[ESI", 1, false },
		{ "[EDI", 1, false }
	},
	{
		{ "[EAX", 4, false },
		{ "[ECX", 4, false },
		{ "[EDX", 4, false },
		{ "[EBX", 4, false },
		{ "", 4, true },
		{ "[EBP", 4, false },
		{ "[ESI", 4, false },
		{ "[EDI", 4, false }
	}
};
//[N1]=ModRM Byte/64
//[N2]=ModRM Byte%8


char* sibREG[8] = {
	"EAX", "ECX", "EDX", "EBX", "ESP", "", "ESI", "EDI"
};
//[N]=SIB byte%8 base


char* sibScale[4][8] = {
	{ "[EAX+", "[ECX+", "[EDX+", "[EBX+", "[", "[EBP+", "[ESI+", "[EDI+" },
	{ "[EAX*2+", "[ECX*2+", "[EDX*2+", "[EBX*2+", "[", "[EBP*2+", "[ESI*2+", "[EDI*2+" },
	{ "[EAX*4+", "[ECX*4+", "[EDX*4+", "[EBX*4+", "[", "[EBP*4+", "[ESI*4+", "[EDI*4+" },
	{ "[EAX*8+", "[ECX*8+", "[EDX*8+", "[EBX*8+", "[", "[EBP*8+", "[ESI*8+", "[EDI*8+" }
};
//[N1]=SIB byte/64
//[N2]=SIB byte%64/8



void disassembleProc(FILE *pFile)
{
	vector<char*> outPutChain;
	BYTE codeRead;
	BYTE modRead;
	BYTE sibRead;
	bool _32bit_operand = true;
	//32 bit = true / 16 bit = false / default is set to be 32 bit
	bool _32bit_address = true;
	bool _regOnly = false;
	bool _rmOnly = false;
	int _operand1 = 4;
	int _operand2 = 4;
	int _operand3 = 4;
	int segOverride = 0;
	bool _segOverride = false;
	bool _isMod = false;
	bool _isSIB = false;
	DWORD _immRead = 0;
	char num2string[128];

prefixJump:

	fread_s(&codeRead, 1, 1, 1, pFile);
	outPutChain.push_back(g_opCodeTable_oneByte[codeRead].op_text);


	if ((g_opCodeTable_oneByte[codeRead].op_operands[0] == _INVALID_OP_) && (g_opCodeTable_oneByte[codeRead].op_operands[1] == _INVALID_OP_) && (g_opCodeTable_oneByte[codeRead].op_operands[2] == _INVALID_OP_))
	{
		switch (codeRead)
		{
		case 0x66:
		{
					 _32bit_operand = false;
					 //outPutChain.push_back(" ");
					 goto prefixJump;
		}
		case 0x67:
		{
					 _32bit_address = false;
					 //outPutChain.push_back(" ");
					 goto prefixJump;
		}
		case 0x0f:
		{
					 //outPutChain.push_back(" ");
					 goto prefixJump;
		}
		case 0x2e:case 0x3e:case 0x26:case 0x36:
		{
					  segOverride = codeRead;
					  _segOverride = true;
					  //outPutChain.push_back(" ");
					  goto prefixJump;
		}
		case 0xf0:case 0xf2:case 0xf3:
		{
					  outPutChain.push_back(" ");
					  goto prefixJump;
		}
		default:
		{
				   for (int x = 0; x < outPutChain.size(); x++)
				   {
					   cout << outPutChain[x];
				   }
				   cout << endl;
				   return;
		}
		}

	}
	if (g_opCodeTable_oneByte[codeRead].op_modRM)
	{
		outPutChain.push_back(" ");
		fread_s(&modRead, 1, 1, 1, pFile);
		_isMod = true;
		if ((modRead / 0x40) < 3)
		{
			if (modRM_rm_Table[modRead / 0x40][modRead % 8].sib)
			{
				fread_s(&sibRead, 1, 1, 1, pFile);
				_isSIB = true;
			}
		}
		else
		{
			if (g_opCodeTable_oneByte[codeRead].op_operands[0] & _b_ == _b_)
			{
				outPutChain.push_back(ModRM_REG_Table[modRead % 8].Mod_reg_r8);
				outPutChain.push_back(",");
				outPutChain.push_back(ModRM_REG_Table[modRead % 0x40 / 8].Mod_reg_r8);
				goto earlyPrint;
			}
			if (_32bit_operand)
			{
				outPutChain.push_back(ModRM_REG_Table[modRead % 8].Mod_reg_r32);
				outPutChain.push_back(",");
				outPutChain.push_back(ModRM_REG_Table[modRead % 0x40 / 8].Mod_reg_r32);
				goto earlyPrint;
			}
			else
			{
				outPutChain.push_back(ModRM_REG_Table[modRead % 8].Mod_reg_r16);
				outPutChain.push_back(",");
				outPutChain.push_back(ModRM_REG_Table[modRead % 0x40 / 8].Mod_reg_r16);
				goto earlyPrint;
			}
		}
	}

	for (int p = 0; p < 3; p++)
	{
		if (g_opCodeTable_oneByte[codeRead].op_operands[p] == _INVALID_OP_)
		{
			for (int x = 0; x < outPutChain.size(); x++)
			{
				cout << outPutChain[x];
			}
			cout << endl;
			return;
		}
		outPutChain.push_back(" ");
		if (g_opCodeTable_oneByte[codeRead].op_operands[p] > 0xf && g_opCodeTable_oneByte[codeRead].op_operands[p] < 0x18)
		{
			outPutChain.push_back(ModRM_REG_Table[g_opCodeTable_oneByte[codeRead].op_operands[p] % 8].Mod_reg_r8);
		}
		else if (g_opCodeTable_oneByte[codeRead].op_operands[p]>0x17 && g_opCodeTable_oneByte[codeRead].op_operands[p] < 0x20)
		{
			if (_32bit_operand)
			{
				outPutChain.push_back(ModRM_REG_Table[g_opCodeTable_oneByte[codeRead].op_operands[p] % 8].Mod_reg_r32);
			}
			else
			{
				outPutChain.push_back(ModRM_REG_Table[g_opCodeTable_oneByte[codeRead].op_operands[p] % 8].Mod_reg_r16);
			}
		}
		else if (g_opCodeTable_oneByte[codeRead].op_operands[p] > 0x1f && g_opCodeTable_oneByte[codeRead].op_operands[p] < 0x26)
		{
			switch (g_opCodeTable_oneByte[codeRead].op_operands[p])
			{
			case _DS_:
			{
						 outPutChain.push_back("DS");
						 break;
			}
			case _CS_:
			{
						 outPutChain.push_back("CS");
						 break;
			}
			case _ES_:
			{
						 outPutChain.push_back("ES");
						 break;
			}
			case _SS_:
			{
						 outPutChain.push_back("SS");
						 break;
			}
			case _GS_:
			{
						 outPutChain.push_back("GS");
						 break;
			}
			case _FS_:
			{
						 outPutChain.push_back("FS");
						 break;
			}
			default:
				break;
			}
		}
		else
		{
			switch (g_opCodeTable_oneByte[codeRead].op_operands[p] & 0xff00)
			{
			case _E_:
			{
						if (g_opCodeTable_oneByte[codeRead].op_operands[p] & 0xff == _b_)
						{
							outPutChain.push_back("BYTE PTR ");
							if (segOverride)
							{
								for (int q = 0; q < 14; q++)
								{
									if (g_prefixTable[q].prefix_code == segOverride)
									{
										outPutChain.push_back(g_prefixTable[q].prefix_text);
									}
								}
							}
							else
							{
								outPutChain.push_back("DS:");
							}

							if (modRead / 0x40 > 2)
							{
								outPutChain.push_back(ModRM_REG_Table[modRead % 8].Mod_reg_r8);
							}
							else
							{
								if (_isSIB)
								{
									outPutChain.push_back(sibScale[sibRead / 64][sibRead % 64 / 8]);
									outPutChain.push_back(sibREG[sibRead % 8]);
								}
								outPutChain.push_back(modRM_rm_Table[modRead / 64][modRead % 8].Mod_rm);
								switch (modRM_rm_Table[modRead / 64][modRead % 8].disp)
								{
								case 0:
								{
										  outPutChain.push_back("]");
										  break;
								}
								case 1:
								{
										  BYTE readByte;
										  fread_s(&readByte, 1, 1, 1, pFile);
										  if (readByte & 0x80)
										  {
											  outPutChain.push_back("-");
											  readByte = ~readByte + 1;
										  }
										  _itoa(readByte, num2string, 16);
										  outPutChain.push_back(num2string);
										  break;
								}
								case 2:
								{
										  WORD readWord;
										  fread_s(&readWord, 2, 2, 1, pFile);
										  if (readWord & 0x8000)
										  {
											  outPutChain.push_back("-");
											  readWord = ~readWord + 1;
										  }
										  _itoa(readWord, num2string, 16);
										  outPutChain.push_back(num2string);
										  break;
								}
								case 4:
								{
										  DWORD readDword;
										  fread_s(&readDword, 2, 2, 1, pFile);
										  if (readDword & 0x80000000)
										  {
											  outPutChain.push_back("-");
											  readDword = ~readDword + 1;
										  }
										  _itoa(readDword, num2string, 16);
										  outPutChain.push_back(num2string);
										  break;
								}
								default:
									break;
								}
								outPutChain.push_back("]");
							}

						}
						else if (g_opCodeTable_oneByte[codeRead].op_operands[p] & 0xff == _v_)
						{
							if (_32bit_operand)
							{
								outPutChain.push_back("DWORD PTR ");
							}
							else
							{
								outPutChain.push_back("WORD PTR ");
							}
							if (segOverride)
							{
								for (int q = 0; q < 14; q++)
								{
									if (g_prefixTable[q].prefix_code == segOverride)
									{
										outPutChain.push_back(g_prefixTable[q].prefix_text);
									}
								}
							}
							else
							{
								outPutChain.push_back("DS:");
							}

							if (modRead / 0x40 > 2)
							{
								if (_32bit_operand)
								{
									outPutChain.push_back(ModRM_REG_Table[modRead % 8].Mod_reg_r32);
								}
								else
								{
									outPutChain.push_back(ModRM_REG_Table[modRead % 8].Mod_reg_r16);
								}

							}
							else
							{
								if (_isSIB)
								{
									outPutChain.push_back(sibScale[sibRead / 64][sibRead % 64 / 8]);
									outPutChain.push_back(sibREG[sibRead % 8]);
								}
								outPutChain.push_back(modRM_rm_Table[modRead / 64][modRead % 8].Mod_rm);
								switch (modRM_rm_Table[modRead / 64][modRead % 8].disp)
								{
								case 0:
								{
										  outPutChain.push_back("]");
										  break;
								}
								case 1:
								{
										  BYTE readByte;
										  fread_s(&readByte, 1, 1, 1, pFile);
										  if (readByte & 0x80)
										  {
											  outPutChain.push_back("-");
											  readByte = ~readByte + 1;
										  }
										  _itoa(readByte, num2string, 16);
										  outPutChain.push_back(num2string);
										  break;
								}
								case 2:
								{
										  WORD readWord;
										  fread_s(&readWord, 2, 2, 1, pFile);
										  if (readWord & 0x8000)
										  {
											  outPutChain.push_back("-");
											  readWord = ~readWord + 1;
										  }
										  _itoa(readWord, num2string, 16);
										  outPutChain.push_back(num2string);
										  break;
								}
								case 4:
								{
										  DWORD readDword;
										  fread_s(&readDword, 2, 2, 1, pFile);
										  if (readDword & 0x80000000)
										  {
											  outPutChain.push_back("-");
											  readDword = ~readDword + 1;
										  }
										  _itoa(readDword, num2string, 16);
										  outPutChain.push_back(num2string);
										  break;
								}
								default:
									break;
								}
								outPutChain.push_back("]");
							}
							break;
						}
						else
						{
							outPutChain.push_back("WORD PTR ");
							if (segOverride)
							{
								for (int q = 0; q < 14; q++)
								{
									if (g_prefixTable[q].prefix_code == segOverride)
									{
										outPutChain.push_back(g_prefixTable[q].prefix_text);
									}
								}
							}
							else
							{
								outPutChain.push_back("DS:");
							}
							if (modRead / 0x40 > 2)
							{
								outPutChain.push_back(ModRM_REG_Table[modRead % 8].Mod_reg_r16);
							}
							else
							{
								if (_isSIB)
								{
									outPutChain.push_back(sibScale[sibRead / 64][sibRead % 64 / 8]);
									outPutChain.push_back(sibREG[sibRead % 8]);
								}
								outPutChain.push_back(modRM_rm_Table[modRead / 64][modRead % 8].Mod_rm);
							}
							outPutChain.push_back("]");
							break;
						}
			}
			case _G_:
			{
						if (g_opCodeTable_oneByte[codeRead].op_operands[p] & 0xff == _b_)
						{
							outPutChain.push_back(ModRM_REG_Table[modRead % 64 / 8].Mod_reg_r8);
						}
						else
						{
							if (_32bit_operand)
							{
								outPutChain.push_back(ModRM_REG_Table[modRead % 64 / 8].Mod_reg_r32);
							}
							else
							{
								outPutChain.push_back(ModRM_REG_Table[modRead % 64 / 8].Mod_reg_r16);
							}
						}
						break;
			}
			case _I_:
			{
						switch (g_opCodeTable_oneByte[codeRead].op_operands[p] & 0xff)
						{
						case _b_:
						{
									BYTE readByte;
									fread_s(&readByte, 1, 1, 1, pFile);
									if (readByte & 0x80)
									{
										outPutChain.push_back("-");
										readByte = ~readByte + 1;
									}
									_itoa(readByte, num2string, 16);
									outPutChain.push_back(num2string);
									break;
						}
						case _w_:
						{
									WORD readWord;
									fread_s(&readWord, 2, 2, 1, pFile);
									if (readWord & 0x8000)
									{
										outPutChain.push_back("-");
										readWord = ~readWord + 1;
									}
									_itoa(readWord, num2string, 16);
									outPutChain.push_back(num2string);
									break;
						}
						case _v_:
						{
									if (_32bit_operand)
									{
										DWORD readDword;
										fread_s(&readDword, 4, 4, 1, pFile);
										if (readDword & 0x80000000)
										{
											outPutChain.push_back("-");
											readDword = ~readDword + 1;
										}
										_itoa(readDword, num2string, 16);
										outPutChain.push_back(num2string);
									}
									else
									{
										WORD readWord;
										fread_s(&readWord, 2, 2, 1, pFile);
										if (readWord & 0x8000)
										{
											outPutChain.push_back("-");
											readWord = ~readWord + 1;
										}
										_itoa(readWord, num2string, 16);
										outPutChain.push_back(num2string);
									}
									break;
						}
						case _z_:
						{
									if (!_32bit_operand)
									{
										WORD readWord;
										fread_s(&readWord, 2, 2, 1, pFile);
										if (readWord & 0x8000)
										{
											outPutChain.push_back("-");
											readWord = ~readWord + 1;
										}
										_itoa(readWord, num2string, 16);
										outPutChain.push_back(num2string);
									}
									else
									{
										DWORD readDword;
										fread_s(&readDword, 4, 4, 1, pFile);
										if (readDword & 0x80000000)
										{
											outPutChain.push_back("-");
											readDword = ~readDword + 1;
										}
										_itoa(readDword, num2string, 16);
										outPutChain.push_back(num2string);
									}
									break;
						}
						default:
							break;
						}
						break;

			}
			case _O_:
			{
						if (_32bit_address)
						{
							DWORD readDword;
							fread_s(&readDword, 4, 4, 1, pFile);
						}
						break;
			}
			case _J_:
			{
						if (g_opCodeTable_oneByte[codeRead].op_operands[p] & 0xff == _b_)
						{
							BYTE readByte;
							fread_s(&readByte, 1, 1, 1, pFile);
							outPutChain.push_back("SHORT EIP");
							if (readByte & 0x80)
							{
								outPutChain.push_back("-");
								readByte = ~readByte + 1;
							}
							else
							{
								outPutChain.push_back("+");
							}
							_itoa(readByte, num2string, 16);
							outPutChain.push_back(num2string);
						}
						else
						{
							if (!_32bit_operand)
							{
								WORD readWord;
								fread_s(&readWord, 2, 2, 1, pFile);
								if (readWord & 0x8000)
								{
									readWord = ~readWord + 1;
								}
								_itoa(readWord, num2string, 16);
								outPutChain.push_back(num2string);
							}
							else
							{
								DWORD readDword;
								fread_s(&readDword, 4, 4, 1, pFile);
								if (readDword & 0x80000000)
								{
									readDword = ~readDword + 1;
								}
								_itoa(readDword, num2string, 16);
								outPutChain.push_back(num2string);
							}
						}
						break;
			}
			case _X_:
			{
						if (g_opCodeTable_oneByte[codeRead].op_operands[p] & 0xff == _b_)
						{
							outPutChain.push_back("BYTE PTR DS:[");
							if (_32bit_address)
							{
								outPutChain.push_back("ESI]");
							}
							else
							{
								outPutChain.push_back("SI]");
							}
						}
						else
						{
							if (_32bit_address)
							{
								if (_32bit_operand)
								{
									outPutChain.push_back("DWORD PTR DS:[ESI]");
								}
								else
								{
									outPutChain.push_back("WORD PTR DS:[ESI]");
								}
							}
							else
							{
								if (_32bit_operand)
								{
									outPutChain.push_back("DWORD PTR DS:[SI]");
								}
								else
								{
									outPutChain.push_back("WORD PTR DS:[SI]");
								}
							}
						}
						break;
			}
			case _Y_:
			{
						if (g_opCodeTable_oneByte[codeRead].op_operands[p] & 0xff == _b_)
						{
							outPutChain.push_back("BYTE PTR ES:[");
							if (_32bit_address)
							{
								outPutChain.push_back("EDI]");
							}
							else
							{
								outPutChain.push_back("DI]");
							}
						}
						else
						{
							if (_32bit_address)
							{
								if (_32bit_operand)
								{
									outPutChain.push_back("DWORD PTR ES:[EDI]");
								}
								else
								{
									outPutChain.push_back("WORD PTR ES:[EDI]");
								}
							}
							else
							{
								if (_32bit_operand)
								{
									outPutChain.push_back("DWORD PTR ES:[DI]");
								}
								else
								{
									outPutChain.push_back("WORD PTR ES:[DI]");
								}
							}
						}
						break;
			}
			default:
				break;
			}
		}
		if (p < 2 && (g_opCodeTable_oneByte[codeRead].op_operands[p + 1] != _INVALID_OP_))
		{
			outPutChain.push_back(",");
		}
	}

earlyPrint:

	for (int x = 0; x < outPutChain.size(); x++)
	{
		cout << outPutChain[x];
	}
	cout << endl;
	return;
}


int _tmain(int argc, char* argv[])
{

	if (argc < 2)
	{
		return 0;
	}
	FILE *pFile;
	fopen_s(&pFile, (char*)argv[1], "rb");

	fseek(pFile, 0, SEEK_END);
	int endTest = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	int currentTest = 0;
	while (ftell(pFile) < endTest)
	{
		disassembleProc(pFile);
		currentTest = ftell(pFile);
	}


	return 0;
}

