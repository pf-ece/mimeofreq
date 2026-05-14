#include "daisysp.h"
#include "daisy_seed.h"

// Set maximum delay time
#define MAX_DELAY static_cast<size_t>(48000)

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

// ADC function declarations
void initADC();
void procADC();

// Switch function declarations
void initSwitch();
void procSwitch();

// Declare DelayLine with MAX_DELAY number of samples.
static DelayLine<float, MAX_DELAY> del_l, del_r;

static OnePole lpf;

Switch del_switch;

Switch del_reverse_switch;

// Bypass initializations
bool del_bypass = true;

// Mode initializations
bool del_reverse = false;

static void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    float dry_l, dry_r, wet_l, wet_r;

    procADC();
    procSwitch();

    // Set Delay time (samples)
    float time_N = lpf.Process(delay_time) * MAX_DELAY;
    del_l.SetDelay(time_N);
    del_r.SetDelay(time_N);

    for(size_t i = 0; i < size; i++)
    {
        // Check if delay effect is bypassed
        if(del_bypass) {
            out[0][i] = in[0][i];
            out[1][i] = in[1][i]; 
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
}