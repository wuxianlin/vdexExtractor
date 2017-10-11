/*

   vdexExtractor
   -----------------------------------------

   Anestis Bechtsoudis <anestis@census-labs.com>
   Copyright 2017 by CENSUS S.A. All Rights Reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#ifndef _DEX_INSTRUCTION_H_
#define _DEX_INSTRUCTION_H_

#include "log.h"
#include "dex_instruction_list.h"

typedef enum {
  kPackedSwitchSignature = 0x0100,
  kSparseSwitchSignature = 0x0200,
  kArrayDataSignature = 0x0300,
} Signatures;

typedef enum {
#define INSTRUCTION_ENUM(opcode, cname, p, f, i, a, v) cname = (opcode),
  DEX_INSTRUCTION_LIST(INSTRUCTION_ENUM)
  RSUB_INT_LIT16 = RSUB_INT,
} Code;

typedef enum {
  k10x,  // op
  k12x,  // op vA, vB
  k11n,  // op vA, #+B
  k11x,  // op vAA
  k10t,  // op +AA
  k20t,  // op +AAAA
  k22x,  // op vAA, vBBBB
  k21t,  // op vAA, +BBBB
  k21s,  // op vAA, #+BBBB
  k21h,  // op vAA, #+BBBB00000[00000000]
  k21c,  // op vAA, thing@BBBB
  k23x,  // op vAA, vBB, vCC
  k22b,  // op vAA, vBB, #+CC
  k22t,  // op vA, vB, +CCCC
  k22s,  // op vA, vB, #+CCCC
  k22c,  // op vA, vB, thing@CCCC
  k32x,  // op vAAAA, vBBBB
  k30t,  // op +AAAAAAAA
  k31t,  // op vAA, +BBBBBBBB
  k31i,  // op vAA, #+BBBBBBBB
  k31c,  // op vAA, thing@BBBBBBBB
  k35c,  // op {vC, vD, vE, vF, vG}, thing@BBBB (B: count, A: vG)
  k3rc,  // op {vCCCC .. v(CCCC+AA-1)}, meth@BBBB

  // op {vC, vD, vE, vF, vG}, meth@BBBB, proto@HHHH (A: count)
  // format: AG op BBBB FEDC HHHH
  k45cc,

  // op {VCCCC .. v(CCCC+AA-1)}, meth@BBBB, proto@HHHH (AA: count)
  // format: AA op BBBB CCCC HHHH
  k4rcc,  // op {VCCCC .. v(CCCC+AA-1)}, meth@BBBB, proto@HHHH (AA: count)

  k51l,  // op vAA, #+BBBBBBBBBBBBBBBB
} Format;

typedef enum {
  kIndexUnknown = 0,
  kIndexNone,               // has no index
  kIndexTypeRef,            // type reference index
  kIndexStringRef,          // string reference index
  kIndexMethodRef,          // method reference index
  kIndexFieldRef,           // field reference index
  kIndexFieldOffset,        // field offset (for static linked fields)
  kIndexVtableOffset,       // vtable offset (for static linked methods)
  kIndexMethodAndProtoRef,  // method and a proto reference index (for invoke-polymorphic)
  kIndexCallSiteRef,        // call site reference index
} IndexType;

typedef enum {
  kBranch              = 0x0000001,  // conditional or unconditional branch
  kContinue            = 0x0000002,  // flow can continue to next statement
  kSwitch              = 0x0000004,  // switch statement
  kThrow               = 0x0000008,  // could cause an exception to be thrown
  kReturn              = 0x0000010,  // returns, no additional statements
  kInvoke              = 0x0000020,  // a flavor of invoke
  kUnconditional       = 0x0000040,  // unconditional branch
  kAdd                 = 0x0000080,  // addition
  kSubtract            = 0x0000100,  // subtract
  kMultiply            = 0x0000200,  // multiply
  kDivide              = 0x0000400,  // division
  kRemainder           = 0x0000800,  // remainder
  kAnd                 = 0x0001000,  // and
  kOr                  = 0x0002000,  // or
  kXor                 = 0x0004000,  // xor
  kShl                 = 0x0008000,  // shl
  kShr                 = 0x0010000,  // shr
  kUshr                = 0x0020000,  // ushr
  kCast                = 0x0040000,  // cast
  kStore               = 0x0080000,  // store opcode
  kLoad                = 0x0100000,  // load opcode
  kClobber             = 0x0200000,  // clobbers memory in a big way (not just a write)
  kRegCFieldOrConstant = 0x0400000,  // is the third virtual register a field or literal constant (vC)
  kRegBFieldOrConstant = 0x0800000,  // is the second virtual register a field or literal constant (vB)
  kExperimental        = 0x1000000,  // is an experimental opcode
} Flags;

typedef enum {
  kVerifyNone               = 0x0000000,
  kVerifyRegA               = 0x0000001,
  kVerifyRegAWide           = 0x0000002,
  kVerifyRegB               = 0x0000004,
  kVerifyRegBField          = 0x0000008,
  kVerifyRegBMethod         = 0x0000010,
  kVerifyRegBNewInstance    = 0x0000020,
  kVerifyRegBString         = 0x0000040,
  kVerifyRegBType           = 0x0000080,
  kVerifyRegBWide           = 0x0000100,
  kVerifyRegC               = 0x0000200,
  kVerifyRegCField          = 0x0000400,
  kVerifyRegCNewArray       = 0x0000800,
  kVerifyRegCType           = 0x0001000,
  kVerifyRegCWide           = 0x0002000,
  kVerifyArrayData          = 0x0004000,
  kVerifyBranchTarget       = 0x0008000,
  kVerifySwitchTargets      = 0x0010000,
  kVerifyVarArg             = 0x0020000,
  kVerifyVarArgNonZero      = 0x0040000,
  kVerifyVarArgRange        = 0x0080000,
  kVerifyVarArgRangeNonZero = 0x0100000,
  kVerifyRuntimeOnly        = 0x0200000,
  kVerifyError              = 0x0400000,
  kVerifyRegHPrototype      = 0x0800000,
  kVerifyRegBCallSite       = 0x1000000
} VerifyFlag;

static int const kInstructionSizeInCodeUnits[] = {
#define INSTRUCTION_SIZE(opcode, c, p, format, i, a, v) \
  (((opcode) == NOP) ? -1 : \
   (((format) >= k10x) && ((format) <= k10t)) ?  1 : \
   (((format) >= k20t) && ((format) <= k22c)) ?  2 : \
   (((format) >= k32x) && ((format) <= k3rc)) ?  3 : \
   (((format) >= k45cc) && ((format) <= k4rcc)) ? 4 : \
    ((format) == k51l) ?  5 : -1),
  DEX_INSTRUCTION_LIST(INSTRUCTION_SIZE)
};

Code dexInstr_getOpcode(uint16_t *code_ptr) {
  uint16_t inst_data = code_ptr[0];
  return (inst_data & 0xFF);
}

uint32_t dexInstr_SizeInCodeUnitsComplexOpcode(uint16_t *code_ptr)
{
  // Handle special NOP encoded variable length sequences.
  switch (*code_ptr) {
    case kPackedSwitchSignature:
      return (4 + code_ptr[1] * 2);
    case kSparseSwitchSignature:
      return (2 + code_ptr[1] * 4);
    case kArrayDataSignature: {
      uint16_t element_size = code_ptr[1];
      uint32_t length = code_ptr[2] | (((uint32_t)code_ptr[3]) << 16);
      // The plus 1 is to round up for odd size and width.
      return (4 + (element_size * length + 1) / 2);
    }
    default:
      if ((*code_ptr & 0xFF) == 0) {
        return 1;  // NOP.
      } else {
        LOGMSG(l_FATAL, "Error when decoding complex opcode");
        exit(EXIT_FAILURE);
      }
  }
}

// Returns the size (in 2 byte code units) of this instruction.
uint32_t dexInstr_SizeInCodeUnits(uint16_t *code_ptr)
{
  int result = kInstructionSizeInCodeUnits[dexInstr_getOpcode(code_ptr)];
  if (result < 0) {
    return dexInstr_SizeInCodeUnitsComplexOpcode(code_ptr);
  } else {
    return result;
  }
}

#endif
