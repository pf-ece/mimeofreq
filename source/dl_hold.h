/** dl_hold.h | Looper configured for use as a hold mode found on delay pedals.
 *  Author: Patrick F. (pf-ece)
 *  Modification of 'looper.h' by Electrosmith, Corp.
 * 
 *  Copyright 2026 pf-ece
 *  Copyright 2020 Electrosmith, Corp.
 *  MIT License
 * 
 *  /// BUGS & ISSUES /// POSSIBLE ADDITIONS /// & OTHER VERY FUN THINGS TO NOTE ///
 *  - Pressing down on hold toggle long enough will introduce unexpected input signal.
 *  - Noise on overdubs (may be related to delay time knob setting speed).
 *  - More comments.
 */

#pragma once
#ifndef DSY_DL_HOLD_H
#define DSY_DL_HOLD_H
#include "daisysp.h"
#include "daisy_seed.h"
namespace daisysp
{
class DLHold
{
    public:
        DLHold() {}
        ~DLHold() {}

        void Init(float *mem, size_t size) {
            buff_size_ = size;
            buff_ = mem;

            InitBuff();

            state_ = State::EMPTY;
            win_idx_ = 0;  
            inc_size_ = 1.0f;
        }

        inline void Clear() { state_ = State::EMPTY; }

        float Process(const float in) {
            float sig = 0.0f;
            float inc;

            inc = state_ == State::EMPTY || state_ == State::REC_FIRST
                ? 1.f
                : inc_size_;
            win_ = WindowVal(win_idx_ * kWindowFactor);

            switch(state_) {
                case State::EMPTY:
                    sig = 0.0f;
                    pos_ = 0;
                    rec_size_ = 0;
                    break;
                case State::REC_FIRST:
                    sig = 0.0f;
                    Write(pos_, in * win_);
                    if(win_idx_ < kWindowSamps - 1)
                        win_idx_ += 1;
                    rec_size_ = pos_;
                    pos_ += inc;
                    if(pos_ > buff_size_ - 1) {
                        state_ = State::PLAYING;
                        rec_size_ = pos_ - 1;
                        pos_ = 0;
                    }
                    break;
                case State::PLAYING:
                    sig = Read(pos_);

                    if(win_idx_ < kWindowSamps - 1) {
                        Write(pos_, sig + in * (1.0f - win_));
                        win_idx_ += 1;
                    }

                    pos_ += inc;
                    if(pos_ > (float)rec_size_ - 1) {
                        pos_ = 0.0f;
                    }
                    else if(pos_ < 0.0f) {
                        pos_ = (float)rec_size_ - 1;
                    }
                    break;
                case State::REC_DUB:
                    sig = Read(pos_);

                    Write(pos_, (in * win_) + sig);

                    if(win_idx_ < kWindowSamps - 1) {
                        win_idx_ += 1;
                    }

                    pos_ += inc;
                    if(pos_ > (float)rec_size_ - 1) {
                        pos_ = 0.0f;
                    }
                    else if(pos_ < 0.0f) {
                        pos_ = (float)rec_size_ - 1;
                    }
                    break;
                default: break;
            }
            return sig;
        }

        inline void TrigRecord() {
            switch(state_) {
                case State::EMPTY:
                pos_ = 0.0f;
                rec_size_ = 0;
                state_ = State::REC_FIRST;
                break;
                case State::REC_FIRST:
                case State::REC_DUB: state_ = State::PLAYING; break;
                case State::PLAYING: state_ = State::REC_DUB; break;
                default: state_ = State::EMPTY; break;
            }
            win_idx_ = 0;
        }

        inline const bool Recording() const
        {
            return state_ == State::REC_DUB || state_ == State::REC_FIRST;
        }

        void SetIncrementSize(float inc_set) { inc_size_ = inc_set; }

        float Lerp(float start, float end, float t) {
            return start + (end - start) * t;
        }

        inline void SetPlaybackSpeed(float del_time) {
            constexpr float kMinSpeed = 0.5f;
            constexpr float kMaxSpeed = 2.0f;

            float speed;
            if(del_time <= 0.5f) {
                speed = Lerp(kMinSpeed, 1.0f, del_time / 0.5f);
            }
            else {
                speed = Lerp(1.0f, kMaxSpeed, (del_time - 0.5f) / 0.5f);
            }

            SetIncrementSize(speed);
            }

    private:
        static constexpr int kWindowSamps = 1200;
        static constexpr float kWindowFactor = (1.0f / kWindowSamps);
        
        void InitBuff() { std::fill(&buff_[0], &buff_[buff_size_ - 1], 0); }
        
        inline const float Read(size_t pos) const { return buff_[pos]; }

        inline void Write(size_t pos, float val) { buff_[pos] = val; }

        float WindowVal(float in) { return sin(HALFPI_F * in); }

        enum class State
        {
            EMPTY,
            REC_FIRST,
            PLAYING,
            REC_DUB
        };

        float *buff_;
        float inc_size_;
        float pos_, win_;
        size_t buff_size_;
        size_t rec_size_;
        size_t win_idx_;
        State state_;
};
};
#endif