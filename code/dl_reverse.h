#pragma once
#ifndef DSY_DELAY_RVRS_H
#define DSY_DELAY_RVRS_H
#include <stdlib.h>
#include <stdint.h>
namespace daisysp
{

template<typename T, size_t max_size>
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
            diff_ = 0;
            delay_rev_ = 1;
            fade_time_ = 1;

            for(size_t i = 0; i < max_size; i++)
            {
                line_[i] = T(0);
            }

            playing_ = false;
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

        }

        inline const T ReadRev() const
        {

        }
        

    private:
        float fade_pos_;
        size_t write_ptr_;
        size_t read_ptr1_;
        size_t read_ptr2_;
        size_t diff_;
        size_t delay_rev_;
        size_t fade_time_;
        T line_[max_size];
        bool playing_;
        bool fading_;
};
};
#endif