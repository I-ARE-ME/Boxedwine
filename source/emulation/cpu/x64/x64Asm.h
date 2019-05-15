#ifndef __X64ASM_H__
#define __X64ASM_H__

#ifdef BOXEDWINE_X64

#include "x64Data.h"

#define CPU_OFFSET_ES_ADDRESS (U32)(offsetof(CPU, seg[ES].address))
#define CPU_OFFSET_CS_ADDRESS (U32)(offsetof(CPU, seg[CS].address))
#define CPU_OFFSET_SS_ADDRESS (U32)(offsetof(CPU, seg[SS].address))
#define CPU_OFFSET_DS_ADDRESS (U32)(offsetof(CPU, seg[DS].address))
#define CPU_OFFSET_FS_ADDRESS (U32)(offsetof(CPU, seg[FS].address))
#define CPU_OFFSET_GS_ADDRESS (U32)(offsetof(CPU, seg[GS].address))

#define CPU_OFFSET_ES_NEG_ADDRESS (U32)(offsetof(x64CPU, negSegAddress[ES]))
#define CPU_OFFSET_CS_NEG_ADDRESS (U32)(offsetof(x64CPU, negSegAddress[CS]))
#define CPU_OFFSET_SS_NEG_ADDRESS (U32)(offsetof(x64CPU, negSegAddress[SS]))
#define CPU_OFFSET_DS_NEG_ADDRESS (U32)(offsetof(x64CPU, negSegAddress[DS]))
#define CPU_OFFSET_FS_NEG_ADDRESS (U32)(offsetof(x64CPU, negSegAddress[FS]))
#define CPU_OFFSET_GS_NEG_ADDRESS (U32)(offsetof(x64CPU, negSegAddress[GS]))

#define CPU_OFFSET_ES (U32)(offsetof(CPU, seg[ES].value))
#define CPU_OFFSET_CS (U32)(offsetof(CPU, seg[CS].value))
#define CPU_OFFSET_SS (U32)(offsetof(CPU, seg[SS].value))
#define CPU_OFFSET_DS (U32)(offsetof(CPU, seg[DS].value))
#define CPU_OFFSET_FS (U32)(offsetof(CPU, seg[FS].value))
#define CPU_OFFSET_GS (U32)(offsetof(CPU, seg[GS].value))

#define CPU_OFFSET_MEM (U32)(offsetof(x64CPU, memOffset))
#define CPU_OFFSET_NEG_MEM (U32)(offsetof(x64CPU, negMemOffset))
#define CPU_OFFSET_OP_PAGES (U32)(offsetof(x64CPU, eipToHostInstruction))

#define CPU_OFFSET_EIP (U32)(offsetof(x64CPU, eip.u32))

class X64Asm : public X64Data {
public:  
    X64Asm(x64CPU* cpu);

    X64Asm* parent;

    void translateRM(U8 rm, bool checkG, bool checkE, bool isG8bit, bool isE8bit, U8 immWidth);
    void writeOp(bool isG8bit=false);

    void setImmediate8(U8 value);
    void setImmediate16(U16 value);
    void setImmediate32(U32 value);

    void xlat();
    void pushCpuOffset16(U32 offset);
    void pushCpuOffset32(U32 offset);
    void popSeg(U8 seg, U8 bytes);
    void daa();
    void aam(U8 ib);
    void salc();
    void incReg(U8 reg, bool isRegRex, U8 bytes);
    void decReg(U8 reg, bool isRegRex, U8 bytes);
    void pushReg16(U8 reg, bool isRegRex);
    void popReg16(U8 reg, bool isRegRex);
    void pushReg32(U8 reg, bool isRegRex);
    void pushw(U16 value);
    void popw(U8 rm);
    void pushd(U32 value);
    void popd(U8 rm);
    void popReg32(U8 reg, bool isRegRex);
    void pushA16();
    void popA16();
    void pushA32();
    void popA32();
    void pushfw();
    void pushfd();
    void popfw();
    void popfd();
    void pushE16(U8 rm);
    void pushE32(U8 rm);
    void jumpConditional(U8 condition, U32 eip);
    void jcxz(U32 eip, bool ea16);
    void loop(U32 eip, bool ea16);
    void loopz(U32 eip, bool ea16);
    void loopnz(U32 eip, bool ea16);
    void jumpTo(U32 eip);
    void jmp(bool big, U32 sel, U32 offset, U32 oldEip);
    void call(bool big, U32 sel, U32 offset, U32 oldEip);
    void retn16(U32 bytes);
    void retn32(U32 bytes);
    void retf(U32 big, U32 bytes);
    void iret(U32 big, U32 oldEip);
    void signalIllegalInstruction(int code);
    void syscall(U32 opLen, U32 eip);
    void int98(U32 opLen);
    void int99(U32 opLen);
    void writeToEFromCpuOffset(U8 rm, U32 offset, U8 fromBytes, U8 toBytes);    
    void writeToRegFromMemAddress(U8 seg, U8 reg, bool isRegRex, U32 disp, U8 bytes);
    void writeToMemAddressFromReg(U8 seg, U8 reg, bool isRegRex, U32 disp, U8 bytes);
    void writeToMemFromValue(U64 value, U8 reg2, bool isReg2Rex, S8 reg3, bool isReg3Rex, U8 reg3Shift, S32 displacement, U8 bytes, bool translateToHost);
    void setSeg(U8 seg, U8 rm);
    void loadSeg(U8 seg, U8 rm);
    void writeXchgEspEax();
    void writeXchgSpAx();
    void bswapEsp();
    void bswapSp();
    void string(bool hasSi, bool hasDi, bool ea16);
    void writeToRegFromValue(U8 reg, bool isRexReg, U64 value, U8 bytes);
    void enter(bool big, U32 bytes, U32 level);
    void leave(bool big);
    void callE(bool big, U8 rm);
    void callFar(bool big, U8 rm);
    void jmpE(bool big, U8 rm);
    void jmpFar(bool big, U8 rm);    
    void lsl(bool big, U8 rm);
    void lar(bool big, U8 rm);
    void invalidOp(U32 op);
    void cpuid();
    void setNativeFlags(U32 flags, U32 mask);
    void write32Buffer(U8* buffer, U32 value); 
    void write16Buffer(U8* buffer, U16 value); 
    void pushNativeFlags();
    void popNativeFlags();
    void logOp(U32 eip);
    bool tmp1InUse;
    bool tmp2InUse;
    bool tmp3InUse;

#ifdef __TEST
    void addReturnFromTest();
#endif
private:
    U8 getTmpReg();
    void releaseTmpReg(U8 reg);
    bool isTmpReg(U8 reg);

    void push(S32 reg, bool isRegRex, U32 value, S32 bytes);
    void pushNativeReg(U8 reg, bool isRegRex);
    void pushNativeValue64(U64 value);
    void pushNativeValue32(U32 value);
    void pushNativeValue8(U8 value);
    void popNativeReg(U8 reg, bool isRegRex);
    void orRegReg(U8 dst, bool isDstRex, U8 src, bool isSrcRex);
    void andWriteToRegFromCPU(U8 reg, bool isRegRex, U32 offset);
    void writeToMemFromReg(U8 reg1, bool isReg1Rex, U8 reg2, bool isReg2Rex, S8 reg3, bool isReg3Rex, U8 reg3Shift, S32 displacement, U8 bytes, bool translateToHost);    
    void writeToRegFromMem(U8 toReg, bool isToRegRex, U8 reg2, bool isReg2Rex, S8 reg3, bool isReg3Rex, U8 reg3Shift, S32 displacement, U8 bytes, bool translateToHost);    
    void writeToRegFromReg(U8 toReg, bool isToReg1Rex, U8 fromReg, bool isFromRegRex, U8 bytes);    
    void popReg(U8 reg, bool isRegRex, S8 bytes, bool commit);
    void syncRegsFromHost();
    void syncRegsToHost(S8 excludeReg=-1);
    void adjustStack(bool big, U8 tmpReg, S8 bytes);
    void doIf(U8 reg, bool isRexReg, U32 equalsValue, std::function<void(void)> ifBlock, std::function<void(void)> elseBlock);
    void doJmp();
    void setPF_onAL(U8 flagReg);
    void setZF_onAL(U8 flagReg);
    void setSF_onAL(U8 flagReg);

    void callHost(void* pfn);
    void callJmp(bool big, U8 rm, void* pfn);

    void translateMemory(U32 rm, bool checkG, bool isG8bit, bool isE8bit);
    void setRM(U8 rm, bool checkG, bool checkE, bool isG8bit, bool isE8bit);
    void setSib(U8 sib, bool checkBase);

    void addWithLea(U8 reg1, bool isReg1Rex, U8 reg2, bool isReg2Rex, S32 reg3, bool isReg3Rex, U8 reg3Shift, S32 displacement, U32 bytes);
    void zeroReg(U8 reg, bool isRexReg);
    void doMemoryInstruction(U8 op, U8 reg1, bool isReg1Rex, U8 reg2, bool isReg2Rex, S8 reg3, bool isReg3Rex, U8 reg3Shift, S32 displacement, U8 bytes);
    void writeHostPlusTmp(U8 rm, bool checkG, bool isG8bit, bool isE8bit, U8 tmpReg);
    U8 getRegForSeg(U8 base, U8 tmpReg);
    U8 getRegForNegSeg(U8 base, U8 tmpReg);
    void translateMemory16(U32 rm, bool checkG, bool isG8bit, bool isE8bit, S8 r1, S8 r2, S16 disp, U8 seg);    

    void setDisplacement32(U32 disp32);
    void setDisplacement8(U8 disp8);  

    void addTodoLinkJump(U32 eip);       
    void doLoop(U32 eip);
    void jmpReg(U8 reg, bool isRex);
    void jmpNativeReg(U8 reg, bool isRegRex);
    void shiftRightReg(U8 reg, bool isRegRex, U8 shiftAmount);
    void andReg(U8 reg, bool isRegRex, U32 mask);
    void writeToEFromReg(U8 rm, U8 reg, bool isRegRex, U8 bytes); // will trash current op data
    void writeToRegFromE(U8 reg, bool isRegRex, U8 rm, U8 bytes); // will trash current op data
    void getNativeAddressInRegFromE(U8 reg, bool isRegRex, U8 rm); // will trash current op data    
};
#endif
#endif