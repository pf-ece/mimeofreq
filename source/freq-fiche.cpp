/** freq-fiche.cpp | Implementation file for overall Freq-Fiche project.
 *  Author: Patrick F. (pf-ece)
 *  
 *  Copyright 2026 pf-ece
 *  MIT License
 * 
 *  /// BUGS & ISSUES /// POSSIBLE ADDITIONS /// & OTHER VERY FUN THINGS TO NOTE ///
 *  - Reroute dl_hold mode to activate and deactivate in this manner:
 *      - Hold mode is ON via delay mode knob being set to "HOLD".
 *      - Press and release bypass switch for writing, overdubbing & clearing buffer.
 *      - Hold mode is OFF (and all buffers cleared) when delay mode knob is set to a different mode.
 *  - Very likely converting pedal completely into MONO.
 *  - More comments, if needed.
 */

#include "daisysp.h"
#include "daisy_seed.h"
#include "dl_hold.h"
#include "dl_reverse.h"

// Set maximum delay time
#define MAX_DELAY static_cast<size_t>(96000)

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

// Potentiometer definitions
#define POT_MIX  A0
#define POT_FB 	 A1
#define POT_TIME A2

// Bypass switch definitions
#define SWITCH_DEL D1

// Mode switch definitions
#define SWITCH_DEL_REVERSE D3
#define SWITCH_DEL_HOLD D4

// ADC channel declarations
enum AdcChannel {
	knobOne,
	knobTwo,
	knobThree,
	NUM_ADC_CHANNELS
};

static DaisySeed hw;

// Knob setting initializations
float mix = 0.5f;
float feedback_lvl = 0.5f;
float delay_time = 0.5f;

uint32_t last_press = 0; // in ms
static constexpr uint32_t kDoublePressMax = 400; // in ms

// ADC function declarations
void initADC();
void procADC();

// Switch function declarations
void initSwitch();
void procSwitch();

// Declare buffers and delay lines with MAX_DELAY number of samples.
static DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS del_l, del_r;
static DLReverse<float, MAX_DELAY> DSY_SDRAM_BSS delrev_l, delrev_r;
static float DSY_SDRAM_BSS hold_buff_l[MAX_DELAY];
static float DSY_SDRAM_BSS hold_buff_r[MAX_DELAY];

DLHold delhold_l, delhold_r;

static OnePole lpf;

Switch del_switch;

Switch del_reverse_switch;
Switch del_hold_switch;

// Bypass initializations
bool del_bypass = true;

// Mode initializations
bool del_reverse = false;
bool del_hold = false;

static void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    float dry_l, dry_r, wet_l, wet_r;

    procADC();
    procSwitch();

    // Set Delay time (samples)
    float time_N = lpf.Process(delay_time) * MAX_DELAY;
    del_l.SetDelay(time_N);
    del_r.SetDelay(time_N);
    delrev_l.SetDelayRev(time_N);
    delrev_r.SetDelayRev(time_N);
    delhold_l.SetPlaybackSpeed(lpf.Process(delay_time));
    delhold_r.SetPlaybackSpeed(lpf.Process(delay_time));


    for(size_t i = 0; i < size; i++)
    {
        // Check if delay effect is bypassed
        if(del_bypass) {
            out[0][i] = in[0][i];
            out[1][i] = in[1][i]; 
        }
        else if(del_reverse) {
            // Read dry input signal
            dry_l = in[0][i];
            dry_r = in[1][i];

            wet_l = delrev_l.ReadRev();
            wet_r = delrev_r.ReadRev();

            delrev_l.Write(dry_l + (wet_l * feedback_lvl));
            delrev_r.Write(dry_r + (wet_r * feedback_lvl));

            out[0][i] = (dry_l * (1.0f - mix)) + (wet_l * mix);
            out[1][i] = (dry_r * (1.0f - mix)) + (wet_r * mix);
        }
        else if(del_hold) {
            out[0][i] = delhold_l.Process(in[0][i]);
            out[1][i] = delhold_r.Process(in[1][i]);
        }
        else {
            // Read dry input signal
            dry_l = in[0][i];
            dry_r = in[1][i];

            // Read previous wet (delayed) signal
            wet_l = del_l.Read();
            wet_r = del_r.Read();

            // Write next delayed signal (feedback loop)
            del_l.Write(dry_l + (wet_l * feedback_lvl));
            del_r.Write(dry_r + (wet_r * feedback_lvl));

            // Mix dry/wet signals, write as output
            out[0][i] = (dry_l * (1.0f - mix)) + (wet_l * mix);
            out[1][i] = (dry_r * (1.0f - mix)) + (wet_r * mix);
        }
    }
}

int main(void)
{
    // Initializations
    // float sample_rate;
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);
    // sample_rate = hw.AudioSampleRate();

	initADC();
    initSwitch();

    del_l.Init();
    del_r.Init();

    delrev_l.Init();
    delrev_r.Init();

    delhold_l.Init(hold_buff_l, MAX_DELAY);
    delhold_r.Init(hold_buff_r, MAX_DELAY);

    lpf.Init();
    lpf.SetFilterMode(OnePole::FilterMode::FILTER_MODE_LOW_PASS);
    lpf.SetFrequency(0.001f);

    // Start callback
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

	hw.adc.Init(adcConfig, NUM_ADC_CHANNELS);
	hw.adc.Start();
}

void procADC()
{
    mix = fmap(hw.adc.GetFloat(knobOne), 0.0f, 1.0f, Mapping::LINEAR);
    feedback_lvl = fmap(hw.adc.GetFloat(knobTwo), 0.0f, 1.0f, Mapping::LINEAR);
    delay_time = fmap(hw.adc.GetFloat(knobThree), 0.05f, 1.0f, Mapping::LINEAR);
}

// Switch processing definitions
void initSwitch()
{
    del_switch.Init(SWITCH_DEL, hw.AudioSampleRate()/hw.AudioBlockSize());

    del_reverse_switch.Init(SWITCH_DEL_REVERSE, hw.AudioSampleRate()/hw.AudioBlockSize());
    del_hold_switch.Init(SWITCH_DEL_HOLD, hw.AudioSampleRate()/hw.AudioBlockSize());
}

void procSwitch()
{
    del_switch.Debounce();
    if(del_switch.RisingEdge()) {
        del_bypass = !del_bypass;
    }

    del_reverse_switch.Debounce();
    if(del_reverse_switch.RisingEdge()) {
        del_reverse = !del_reverse;
        hw.SetLed(del_reverse);
    }
    del_hold_switch.Debounce();
    if(del_hold_switch.RisingEdge()) {
        uint32_t now = System::GetNow(); // in ms
        if(now - last_press < kDoublePressMax) {
        del_hold = false;
        delhold_l.Clear();
        delhold_r.Clear();
        last_press = 0;
        }
        else {
            del_hold = true;
            delhold_l.TrigRecord();
            delhold_r.TrigRecord();
            last_press = now;
        }
    }
    if(del_hold_switch.FallingEdge() && del_hold) {
        delhold_l.TrigRecord();
        delhold_r.TrigRecord();
    }
}