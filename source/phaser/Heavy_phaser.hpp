/**
 * Copyright (c) 2026 Enzien Audio, Ltd.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions, and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the phrase "powered by heavy",
 *    the heavy logo, and a hyperlink to https://enzienaudio.com, all in a visible
 *    form.
 * 
 *   2.1 If the Application is distributed in a store system (for example,
 *       the Apple "App Store" or "Google Play"), the phrase "powered by heavy"
 *       shall be included in the app description or the copyright text as well as
 *       the in the app itself. The heavy logo will shall be visible in the app
 *       itself as well.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef _HEAVY_CONTEXT_PHASER_HPP_
#define _HEAVY_CONTEXT_PHASER_HPP_

// object includes
#include "HeavyContext.hpp"
#include "HvSignalPhasor.h"
#include "HvSignalRPole.h"
#include "HvSignalDel1.h"
#include "HvSignalVar.h"
#include "HvSignalLine.h"
#include "HvMath.h"

class Heavy_phaser : public HeavyContext {

 public:
  Heavy_phaser(double sampleRate, int poolKb=10, int inQueueKb=2, int outQueueKb=0);
  ~Heavy_phaser();

  const char *getName() override { return "phaser"; }
  int getNumInputChannels() override { return 1; }
  int getNumOutputChannels() override { return 1; }

  int process(float **inputBuffers, float **outputBuffer, int n) override;
  int processInline(float *inputBuffers, float *outputBuffer, int n) override;
  int processInlineInterleaved(float *inputBuffers, float *outputBuffer, int n) override;

  int getParameterInfo(int index, HvParameterInfo *info) override;
  struct Parameter {
    struct In {
      enum ParameterIn : hv_uint32_t {
        DEPTH = 0xAB90C33D, // depth
        RATE = 0x6AA9FD2E, // rate
      };
    };
  };

 private:
  HvTable *getTableForHash(hv_uint32_t tableHash) override;
  void scheduleMessageForReceiver(hv_uint32_t receiverHash, HvMessage *m) override;


  /*
  * Code for expr~ implementation
  * Write out the generic header code
  */

  // per class code

  // per object code


  // static sendMessage functions
  static void cMsg_M7MJo2pu_sendMessage(HeavyContextInterface *, int, const HvMessage *);
  static void cMsg_O8bRAXxY_sendMessage(HeavyContextInterface *, int, const HvMessage *);
  static void cReceive_5oqjFb5U_sendMessage(HeavyContextInterface *, int, const HvMessage *);
  static void cReceive_a3cnplfk_sendMessage(HeavyContextInterface *, int, const HvMessage *);

  // objects
  SignalLine sLine_U8c3Yo0Z;
  SignalPhasor sPhasor_yxQNBGeu;
  SignalLine sLine_ACF2wbvz;
  SignalDel1 sDel1_bkFf1x16;
  SignalRPole sRPole_iaHeQuXe;
  SignalDel1 sDel1_l4mWbH65;
  SignalRPole sRPole_tnw1Pp7d;
  SignalDel1 sDel1_z9MlL7iD;
  SignalRPole sRPole_q0deNXaC;
  SignalDel1 sDel1_nAfabgIH;
  SignalRPole sRPole_Y5CnTFLo;
  SignalDel1 sDel1_U3jq9cYN;
  SignalRPole sRPole_k8A97lVH;
  SignalDel1 sDel1_PIQ0ApGU;
  SignalRPole sRPole_p9Ylicgn;
  SignalDel1 sDel1_Hf1p2wLy;
  SignalRPole sRPole_5zfFREnP;
  SignalDel1 sDel1_Qf4nGYdF;
  SignalRPole sRPole_3nOvOy3q;
};

#endif // _HEAVY_CONTEXT_PHASER_HPP_
