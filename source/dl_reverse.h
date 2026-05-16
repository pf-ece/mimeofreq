/** dl_reverse.h | Reverse delay line.
 *  Author: Patrick F. (pf-ece)
 *  Modification of code originally written by Adam Fulford.
 * 
 *  Copyright 2026 pf-ece
 *  Copyright 2021 Adam Fulford
 *  MIT License
 */

#pragma once
#ifndef DSY_DL_REVERSE_H
#define DSY_DL_REVERSE_H
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
namespace daisysp
{
template <typename T, size_t max_size>
class DLReverse
{
    public:
        DLReverse() {}
        ~DLReverse() {}

        void Init() {Reset();}

        void Reset()
        {
            fade_pos_ = 0.0f;

            write_ptr_ = 0;
            read_ptr1_ = 0;
            read_ptr2_ = 0;
            loop_pos_ = 0;
            delay_rev_ = 50000;
            fade_time_ = 48000;

            for(size_t i = 0; i < max_size; i++)
            {
                line_[i] = T(0);
            }

            playing_ptr2_ = false;
            fading_ = false;
        }

        inline void SetDelayRev(size_t delay)
        {
            delay_rev_ = delay < max_size ? delay : max_size - 1;
        }

        inline void SetDelayRev(float delay)
        {
        int32_t int_delay = static_cast<int32_t>(delay);
        delay_rev_ = static_cast<size_t>(int_delay) < max_size ? int_delay
                                                           : max_size - 1;
        }

        inline void Write(const T sample)
        {
            line_[write_ptr_] = sample;
            write_ptr_       = (write_ptr_ + 1 + max_size) % max_size;

            loop_pos_        = (loop_pos_ + 1 + delay_rev_) % delay_rev_;

            read_ptr1_ = (read_ptr1_ - 1 + max_size) % max_size;
            read_ptr2_ = (read_ptr2_ - 1 + max_size) % max_size;

            if(loop_pos_ > (delay_rev_ - fade_time_ - 1)) {
                if(!fading_) {
                    fading_ = true;
                    
                    if(!playing_ptr2_) {
                       read_ptr2_ = write_ptr_ - 1; 
                    }
                    else {
                       read_ptr1_ = write_ptr_ - 1;
                    }
                }
            }

            if(fading_) {
                if(!playing_ptr2_) {
                    fade_pos_ = fade_pos_ + (1.0f / fade_time_);
                    if (fade_pos_ > 1.0f) {
                        fade_pos_ = 1.0f;
                        fading_ = false;
                        playing_ptr2_ = true;
                    }
                }
                else {
                    fade_pos_ = fade_pos_ - (1.0f / fade_time_);
                    if (fade_pos_ < 0.0f) {
                        fade_pos_ = 0.0f;
                        fading_ = false;
                        playing_ptr2_ = false;
                    }
                }
            }
        }

        inline const T ReadRev() const
        {
            T read1 = line_[read_ptr1_];
            T read2 = line_[read_ptr2_];
            float scalar_1;
            float scalar_2;

            // Hann window
            scalar_1 = sinf(fade_pos_ * ((float)M_PI * 0.5f));
            scalar_2 = sinf((1.0f - fade_pos_) * ((float)M_PI * 0.5f));

            float sample = (read2 * scalar_1) + (read1 * scalar_2);

            return sample;
        }
        

    private:
        float fade_pos_;
        size_t write_ptr_;
        size_t read_ptr1_;
        size_t read_ptr2_;
        size_t loop_pos_;
        size_t delay_rev_;
        size_t fade_time_;
        T line_[max_size];
        bool playing_ptr2_;
        bool fading_;
};
};
#endif