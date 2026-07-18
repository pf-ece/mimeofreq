/** freq-fiche.cpp | Implementation file for overall Freq-Fiche project.
 *  Author: Patrick F. (pf-ece)
 *  
 *  Copyright 2026 pf-ece
 *  MIT License
 */

#include "daisysp.h"
#include "daisy_seed.h"
#include "phaser/Heavy_phaser.hpp"

// Set maximum delay time
#define MAX_DELAY static_cast<size_t>(96000)

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

// Potentiometer definitions
#define POT_MIX   A0
#define POT_FB    A1
#define POT_TIME  A2
#define POT_RATE  A3
#define POT_DEPTH A4

// Bypass switch definitions
#define SWITCH_DEL D1
#define SWITCH_PHS D2

// Bypass LED definitions
#define LED_DEL D3
#define LED_PHS D4

// Combo switch definitions
#define SPDT_A D9
#define SPDT_B D10

// ADC channel declarations
enum AdcChannel {
    knobOne,
    knobTwo,
    knobThree,
    knobFour,
    knobFive,
    NUM_ADC_CHANNELS
};

static DaisySeed hw;

// Knob setting initializations
float mix = 0.5f;
float feedback_lvl = 0.5f;
float delay_time = 0.5f;
float rate = -0.522879f;
float depth = 0.5f;

// ADC function declarations
void initADC();
void procADC();

// Switch function declarations
void initSwitch();
void procSwitch();

// Effect objects
static DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS del;
static Heavy_phaser *phs = nullptr;

// Block-size intermediate arrays
static float delayed[4];
static float phased[4];

// Low-pass filters
static OnePole lpf;
static OnePole lpf_rate;
static OnePole lpf_depth;

// Switch objects
Switch del_switch;
Switch phs_switch;
Switch3 spdt;

// Bypass initializations
bool del_bypass = true;
bool phs_bypass = true;

int spdt_state = 0;

static void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    float dry, wet;

    procADC();
    procSwitch();

    // Set delay time (samples)
    float time_N = lpf.Process(delay_time) * MAX_DELAY;
    del.SetDelay(time_N);
    
    // COMBO
    if(!phs_bypass && !del_bypass) {
        switch(spdt_state) {
            case 1:
                // DELAY into PHASER
                for(size_t i = 0; i < size; i++) {
                        dry = in[0][i];
                        wet = del.Read();
                        del.Write(dry + (wet * feedback_lvl));
                        delayed[i] = (dry * (1.0f - mix)) + (wet * mix);
                }
                phs->processInline(const_cast<float*>(delayed), out[0], (int)size);
                break;
            case 2:
                // PHASER into DELAY
                phs->processInline(const_cast<float*>(in[0]), phased, (int)size);
                for(size_t i = 0; i < size; i++) {
                    dry = phased[i];
                    wet = del.Read();
                    del.Write(dry + (wet * feedback_lvl));
                    out[0][i] = (dry * (1.0f - mix)) + (wet * mix);
                }
                break;
            default:
                // PHASER into DELAY (DEFAULT)
                phs->processInline(const_cast<float*>(in[0]), phased, (int)size);
                for(size_t i = 0; i < size; i++) {
                    dry = phased[i];
                    wet = del.Read();
                    del.Write(dry + (wet * feedback_lvl));
                    out[0][i] = (dry * (1.0f - mix)) + (wet * mix);
                }
                break;
        }
    }
    // DELAY
    else if(!del_bypass) {
        for(size_t i = 0; i < size; i++) {
            // Read dry input signal
            dry = in[0][i];

            // Read previous wet (delayed) signal
            wet = del.Read();

            // Write next delayed signal (feedback loop)
            del.Write(dry + (wet * feedback_lvl));

            // Mix dry/wet signals, write as output
            out[0][i] = (dry * (1.0f - mix)) + (wet * mix);
            }
    }
    // PHASER
    else if(!phs_bypass) {
        // Phaser effect processing
        phs->processInline(const_cast<float*>(in[0]), out[0], (int)size);
    }
    // NONE
    else {
        for(size_t i = 0; i < size; i++) {
            out[0][i] = in[0][i];
        }
    }
}

int main(void)
{
    float sample_rate;
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);
    sample_rate = hw.AudioSampleRate();

    initADC();
    initSwitch();

    del.Init();

    lpf.Init();
    lpf.SetFilterMode(OnePole::FilterMode::FILTER_MODE_LOW_PASS);
    lpf.SetFrequency(0.001f);

    lpf_rate.Init();
    lpf_rate.SetFilterMode(OnePole::FilterMode::FILTER_MODE_LOW_PASS);
    lpf_rate.SetFrequency(0.001f);

    lpf_depth.Init();
    lpf_depth.SetFilterMode(OnePole::FilterMode::FILTER_MODE_LOW_PASS);
    lpf_depth.SetFrequency(0.001f);

    phs = new Heavy_phaser(sample_rate);

    phs->sendFloatToReceiver(Heavy_phaser::Parameter::In::ParameterIn::RATE, -0.301f);
    phs->sendFloatToReceiver(Heavy_phaser::Parameter::In::ParameterIn::DEPTH, 1.0f);

    hw.StartAudio(AudioCallback);

    while(1) {}
}

// ADC processing definitions
void initADC()
{
    AdcChannelConfig adcConfig[NUM_ADC_CHANNELS];
    adcConfig[knobOne].InitSingle(POT_MIX);
    adcConfig[knobTwo].InitSingle(POT_FB);
    adcConfig[knobThree].InitSingle(POT_TIME);
    adcConfig[knobFour].InitSingle(POT_RATE);
    adcConfig[knobFive].InitSingle(POT_DEPTH);

    hw.adc.Init(adcConfig, NUM_ADC_CHANNELS);
    hw.adc.Start();
}

void procADC()
{
    mix          = fmap(hw.adc.GetFloat(knobOne),   0.0f,  1.0f,  Mapping::LINEAR);
    feedback_lvl = fmap(hw.adc.GetFloat(knobTwo),   0.0f,  1.0f,  Mapping::LINEAR);
    delay_time   = fmap(hw.adc.GetFloat(knobThree), 0.05f, 1.0f,  Mapping::LINEAR);
    rate         = lpf_rate.Process(fmap(hw.adc.GetFloat(knobFour),  -2.0f, 1.0f,  Mapping::LINEAR));
    depth        = lpf_depth.Process(fmap(hw.adc.GetFloat(knobFive),  0.0f,  1.0f,  Mapping::LINEAR));

    if(!phs_bypass) {
        phs->sendFloatToReceiver(Heavy_phaser::Parameter::In::ParameterIn::RATE, rate);
        phs->sendFloatToReceiver(Heavy_phaser::Parameter::In::ParameterIn::DEPTH, depth);
    }
}

// Switch processing definitions
void initSwitch()
{
    del_switch.Init(SWITCH_DEL, hw.AudioSampleRate()/hw.AudioBlockSize());
    phs_switch.Init(SWITCH_PHS, hw.AudioSampleRate()/hw.AudioBlockSize());

    spdt.Init(SPDT_A, SPDT_B);
}

void procSwitch()
{
    del_switch.Debounce();
    if(del_switch.RisingEdge()) {
        del_bypass = !del_bypass;
    }
    phs_switch.Debounce();
    if(phs_switch.RisingEdge()) {
        phs_bypass = !phs_bypass;
    }

    spdt_state = spdt.Read();
}