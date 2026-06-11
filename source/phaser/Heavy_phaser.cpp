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

#include "Heavy_phaser.hpp"

#include <new>

#define Context(_c) static_cast<Heavy_phaser *>(_c)


/*
 * C Functions
 */

extern "C" {
  HV_EXPORT HeavyContextInterface *hv_phaser_new(double sampleRate) {
    // allocate aligned memory
    void *ptr = hv_malloc(sizeof(Heavy_phaser));
    // ensure non-null
    if (!ptr) return nullptr;
    // call constructor
    new(ptr) Heavy_phaser(sampleRate);
    return Context(ptr);
  }

  HV_EXPORT HeavyContextInterface *hv_phaser_new_with_options(double sampleRate,
      int poolKb, int inQueueKb, int outQueueKb) {
    // allocate aligned memory
    void *ptr = hv_malloc(sizeof(Heavy_phaser));
    // ensure non-null
    if (!ptr) return nullptr;
    // call constructor
    new(ptr) Heavy_phaser(sampleRate, poolKb, inQueueKb, outQueueKb);
    return Context(ptr);
  }

  HV_EXPORT void hv_phaser_free(HeavyContextInterface *instance) {
    // call destructor
    Context(instance)->~Heavy_phaser();
    // free memory
    hv_free(instance);
  }
} // extern "C"







/*
 * Class Functions
 */

Heavy_phaser::Heavy_phaser(double sampleRate, int poolKb, int inQueueKb, int outQueueKb)
    : HeavyContext(sampleRate, poolKb, inQueueKb, outQueueKb) {
  numBytes += sLine_init(&sLine_qeXKzbxc);
  numBytes += sPhasor_init(&sPhasor_3hdjiZHf, sampleRate);
  numBytes += sLine_init(&sLine_LXlaf5cj);
  numBytes += sDel1_init(&sDel1_6u9lQDNR);
  numBytes += sRPole_init(&sRPole_3K9bHEOE);
  numBytes += sDel1_init(&sDel1_t53bVmLz);
  numBytes += sRPole_init(&sRPole_rXmkpJhH);
  numBytes += sDel1_init(&sDel1_ajfnNLAo);
  numBytes += sRPole_init(&sRPole_KNL9fPp8);
  numBytes += sDel1_init(&sDel1_oIsdt3Bx);
  numBytes += sRPole_init(&sRPole_a7PQTxT6);
  numBytes += sDel1_init(&sDel1_Lzwnd3ZM);
  numBytes += sRPole_init(&sRPole_umCaYVMA);
  numBytes += sDel1_init(&sDel1_qxR8FvVS);
  numBytes += sRPole_init(&sRPole_rIJQd0fD);
  numBytes += sDel1_init(&sDel1_JUQf4K4j);
  numBytes += sRPole_init(&sRPole_0uGDZeXd);
  numBytes += sDel1_init(&sDel1_lHoRuRrs);
  numBytes += sRPole_init(&sRPole_cKtfvEsk);
  numBytes += cExpr_init(&cExpr_XpcK23bH, &Heavy_phaser::cExpr_XpcK23bH_evaluate);
  
}

Heavy_phaser::~Heavy_phaser() {
  cExpr_free(&cExpr_XpcK23bH);
}

HvTable *Heavy_phaser::getTableForHash(hv_uint32_t tableHash) {
  return nullptr;
}

void Heavy_phaser::scheduleMessageForReceiver(hv_uint32_t receiverHash, HvMessage *m) {
  switch (receiverHash) {
    case 0xAB90C33D: { // depth
      mq_addMessageByTimestamp(&mq, m, 0, &cReceive_jow7NXaD_sendMessage);
      break;
    }
    case 0x6AA9FD2E: { // rate
      mq_addMessageByTimestamp(&mq, m, 0, &cReceive_eJu4EkNt_sendMessage);
      break;
    }
    default: return;
  }
}

int Heavy_phaser::getParameterInfo(int index, HvParameterInfo *info) {
  if (info != nullptr) {
    switch (index) {
      case 0: {
        info->name = "depth";
        info->hash = 0xAB90C33D;
        info->type = HvParameterType::HV_PARAM_TYPE_PARAMETER_IN;
        info->minVal = 0.0f;
        info->maxVal = 1.0f;
        info->defaultVal = 0.5f;
        break;
      }
      case 1: {
        info->name = "rate";
        info->hash = 0x6AA9FD2E;
        info->type = HvParameterType::HV_PARAM_TYPE_PARAMETER_IN;
        info->minVal = -2.0f;
        info->maxVal = 1.0f;
        info->defaultVal = -0.522879f;
        break;
      }
      default: {
        info->name = "invalid parameter index";
        info->hash = 0;
        info->type = HvParameterType::HV_PARAM_TYPE_PARAMETER_IN;
        info->minVal = 0.0f;
        info->maxVal = 0.0f;
        info->defaultVal = 0.0f;
        break;
      }
    }
  }
  return 2;
}



/*
 * Send Function Implementations
 */


void Heavy_phaser::cExpr_XpcK23bH_sendMessage(HeavyContextInterface *_c, int letIn, const HvMessage *m) {
  cMsg_0X0y13ff_sendMessage(_c, 0, m);
}

float Heavy_phaser::cExpr_XpcK23bH_evaluate(const float* args) {
  	return hv_pow_f(10 , ((float)(args[0])));
}

void Heavy_phaser::cMsg_0X0y13ff_sendMessage(HeavyContextInterface *_c, int letIn, const HvMessage *const n) {
  HvMessage *m = nullptr;
  m = HV_MESSAGE_ON_STACK(2);
  msg_init(m, 2, msg_getTimestamp(n));
  msg_setElementToFrom(m, 0, n, 0);
  msg_setFloat(m, 1, 20.0f);
  sLine_onMessage(_c, &Context(_c)->sLine_qeXKzbxc, 0, m, NULL);
}

void Heavy_phaser::cMsg_KlHXQjnU_sendMessage(HeavyContextInterface *_c, int letIn, const HvMessage *const n) {
  HvMessage *m = nullptr;
  m = HV_MESSAGE_ON_STACK(2);
  msg_init(m, 2, msg_getTimestamp(n));
  msg_setElementToFrom(m, 0, n, 0);
  msg_setFloat(m, 1, 20.0f);
  sLine_onMessage(_c, &Context(_c)->sLine_LXlaf5cj, 0, m, NULL);
}

void Heavy_phaser::cReceive_eJu4EkNt_sendMessage(HeavyContextInterface *_c, int letIn, const HvMessage *m) {
  cExpr_onMessage(_c, &Context(_c)->cExpr_XpcK23bH, 0, m, &cExpr_XpcK23bH_sendMessage);
}

void Heavy_phaser::cReceive_jow7NXaD_sendMessage(HeavyContextInterface *_c, int letIn, const HvMessage *m) {
  cMsg_KlHXQjnU_sendMessage(_c, 0, m);
}



/*
 * Code for expr~ implementation
 * Write out the generic implementation code
 */

 // per class code

 // per object code


/*
 * Context Process Implementation
 */

int Heavy_phaser::process(float **inputBuffers, float **outputBuffers, int n) {
  while (hLp_hasData(&inQueue)) {
    hv_uint32_t numBytes = 0;
    ReceiverMessagePair *p = reinterpret_cast<ReceiverMessagePair *>(hLp_getReadBuffer(&inQueue, &numBytes));
    hv_assert(numBytes >= sizeof(ReceiverMessagePair));
    scheduleMessageForReceiver(p->receiverHash, &p->msg);
    hLp_consume(&inQueue);
  }

  sendBangToReceiver(0xDD21C0EB); // send to __hv_bang~ on next cycle
  const int n4 = n & ~HV_N_SIMD_MASK; // ensure that the block size is a multiple of HV_N_SIMD

  // temporary signal vars
  hv_bufferf_t Bf0, Bf1, Bf2, Bf3, Bf4;

  // input and output vars
  hv_bufferf_t O0;
  hv_bufferf_t I0;

  // declare and init the zero buffer
  hv_bufferf_t ZERO; __hv_zero_f(VOf(ZERO));

  hv_uint32_t nextBlock = blockStartTimestamp;
  for (int n = 0; n < n4; n += HV_N_SIMD) {

    // process all of the messages for this block
    nextBlock += HV_N_SIMD;
    while (mq_hasMessageBefore(&mq, nextBlock)) {
      MessageNode *const node = mq_peek(&mq);
      node->sendMessage(this, node->let, node->m);
      mq_pop(&mq);
    }

    // load input buffers
    __hv_load_f(inputBuffers[0]+n, VOf(I0));

    // zero output buffers
    __hv_zero_f(VOf(O0));

    // process all signal functions
    __hv_line_f(&sLine_qeXKzbxc, VOf(Bf0));
    __hv_phasor_f(&sPhasor_3hdjiZHf, VIf(Bf0), VOf(Bf0));
    __hv_var_k_f(VOf(Bf1), 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
    __hv_sub_f(VIf(Bf0), VIf(Bf1), VOf(Bf1));
    __hv_abs_f(VIf(Bf1), VOf(Bf1));
    __hv_var_k_f(VOf(Bf0), 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f);
    __hv_sub_f(VIf(Bf1), VIf(Bf0), VOf(Bf0));
    __hv_var_k_f(VOf(Bf1), 6.283185307179586f, 6.283185307179586f, 6.283185307179586f, 6.283185307179586f, 6.283185307179586f, 6.283185307179586f, 6.283185307179586f, 6.283185307179586f);
    __hv_mul_f(VIf(Bf0), VIf(Bf1), VOf(Bf1));
    __hv_mul_f(VIf(Bf1), VIf(Bf1), VOf(Bf0));
    __hv_mul_f(VIf(Bf1), VIf(Bf0), VOf(Bf2));
    __hv_mul_f(VIf(Bf2), VIf(Bf0), VOf(Bf0));
    __hv_var_k_f(VOf(Bf3), 0.007833333333333f, 0.007833333333333f, 0.007833333333333f, 0.007833333333333f, 0.007833333333333f, 0.007833333333333f, 0.007833333333333f, 0.007833333333333f);
    __hv_var_k_f(VOf(Bf4), -0.166666666666667f, -0.166666666666667f, -0.166666666666667f, -0.166666666666667f, -0.166666666666667f, -0.166666666666667f, -0.166666666666667f, -0.166666666666667f);
    __hv_fma_f(VIf(Bf2), VIf(Bf4), VIf(Bf1), VOf(Bf1));
    __hv_fma_f(VIf(Bf0), VIf(Bf3), VIf(Bf1), VOf(Bf1));
    __hv_var_k_f(VOf(Bf3), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    __hv_add_f(VIf(Bf1), VIf(Bf3), VOf(Bf3));
    __hv_var_k_f(VOf(Bf1), 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
    __hv_mul_f(VIf(Bf3), VIf(Bf1), VOf(Bf1));
    __hv_line_f(&sLine_LXlaf5cj, VOf(Bf3));
    __hv_mul_f(VIf(Bf1), VIf(Bf3), VOf(Bf3));
    __hv_del1_f(&sDel1_6u9lQDNR, VIf(I0), VOf(Bf1));
    __hv_mul_f(VIf(I0), VIf(Bf3), VOf(Bf0));
    __hv_sub_f(VIf(Bf1), VIf(Bf0), VOf(Bf0));
    __hv_neg_f(VIf(Bf3), VOf(Bf1));
    __hv_rpole_f(&sRPole_3K9bHEOE, VIf(Bf0), VIf(Bf1), VOf(Bf1));
    __hv_del1_f(&sDel1_t53bVmLz, VIf(Bf1), VOf(Bf0));
    __hv_mul_f(VIf(Bf1), VIf(Bf3), VOf(Bf1));
    __hv_sub_f(VIf(Bf0), VIf(Bf1), VOf(Bf1));
    __hv_neg_f(VIf(Bf3), VOf(Bf0));
    __hv_rpole_f(&sRPole_rXmkpJhH, VIf(Bf1), VIf(Bf0), VOf(Bf0));
    __hv_del1_f(&sDel1_ajfnNLAo, VIf(Bf0), VOf(Bf1));
    __hv_mul_f(VIf(Bf0), VIf(Bf3), VOf(Bf0));
    __hv_sub_f(VIf(Bf1), VIf(Bf0), VOf(Bf0));
    __hv_neg_f(VIf(Bf3), VOf(Bf1));
    __hv_rpole_f(&sRPole_KNL9fPp8, VIf(Bf0), VIf(Bf1), VOf(Bf1));
    __hv_del1_f(&sDel1_oIsdt3Bx, VIf(Bf1), VOf(Bf0));
    __hv_mul_f(VIf(Bf1), VIf(Bf3), VOf(Bf1));
    __hv_sub_f(VIf(Bf0), VIf(Bf1), VOf(Bf1));
    __hv_neg_f(VIf(Bf3), VOf(Bf0));
    __hv_rpole_f(&sRPole_a7PQTxT6, VIf(Bf1), VIf(Bf0), VOf(Bf0));
    __hv_del1_f(&sDel1_Lzwnd3ZM, VIf(Bf0), VOf(Bf1));
    __hv_mul_f(VIf(Bf0), VIf(Bf3), VOf(Bf0));
    __hv_sub_f(VIf(Bf1), VIf(Bf0), VOf(Bf0));
    __hv_neg_f(VIf(Bf3), VOf(Bf1));
    __hv_rpole_f(&sRPole_umCaYVMA, VIf(Bf0), VIf(Bf1), VOf(Bf1));
    __hv_del1_f(&sDel1_qxR8FvVS, VIf(Bf1), VOf(Bf0));
    __hv_mul_f(VIf(Bf1), VIf(Bf3), VOf(Bf1));
    __hv_sub_f(VIf(Bf0), VIf(Bf1), VOf(Bf1));
    __hv_neg_f(VIf(Bf3), VOf(Bf0));
    __hv_rpole_f(&sRPole_rIJQd0fD, VIf(Bf1), VIf(Bf0), VOf(Bf0));
    __hv_del1_f(&sDel1_JUQf4K4j, VIf(Bf0), VOf(Bf1));
    __hv_mul_f(VIf(Bf0), VIf(Bf3), VOf(Bf0));
    __hv_sub_f(VIf(Bf1), VIf(Bf0), VOf(Bf0));
    __hv_neg_f(VIf(Bf3), VOf(Bf1));
    __hv_rpole_f(&sRPole_0uGDZeXd, VIf(Bf0), VIf(Bf1), VOf(Bf1));
    __hv_del1_f(&sDel1_lHoRuRrs, VIf(Bf1), VOf(Bf0));
    __hv_mul_f(VIf(Bf1), VIf(Bf3), VOf(Bf1));
    __hv_sub_f(VIf(Bf0), VIf(Bf1), VOf(Bf1));
    __hv_neg_f(VIf(Bf3), VOf(Bf3));
    __hv_rpole_f(&sRPole_cKtfvEsk, VIf(Bf1), VIf(Bf3), VOf(Bf3));
    __hv_add_f(VIf(Bf3), VIf(I0), VOf(Bf3));
    __hv_var_k_f(VOf(Bf1), 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
    __hv_mul_f(VIf(Bf3), VIf(Bf1), VOf(Bf1));
    __hv_add_f(VIf(Bf1), VIf(O0), VOf(O0));

    // save output vars to output buffer
    __hv_store_f(outputBuffers[0]+n, VIf(O0));
  }

  blockStartTimestamp = nextBlock;

  return n4; // return the number of frames processed

}

int Heavy_phaser::processInline(float *inputBuffers, float *outputBuffers, int n4) {
  hv_assert(!(n4 & HV_N_SIMD_MASK)); // ensure that n4 is a multiple of HV_N_SIMD

  // define the heavy input buffer for 1 channel(s)
  float **const bIn = &inputBuffers;

  // define the heavy output buffer for 1 channel(s)
  float **const bOut = &outputBuffers;

  int n = process(bIn, bOut, n4);
  return n;
}

int Heavy_phaser::processInlineInterleaved(float *inputBuffers, float *outputBuffers, int n4) {
  hv_assert(n4 & ~HV_N_SIMD_MASK); // ensure that n4 is a multiple of HV_N_SIMD

  // define the heavy input buffer for 1 channel(s), uninterleave
  float *const bIn = inputBuffers;

  // define the heavy output buffer for 1 channel(s)
  float *const bOut = outputBuffers;

  int n = processInline(bIn, bOut, n4);

  

  return n;
}
