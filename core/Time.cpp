//
// Created by expleoene on 4/22/26.
//

#include "Time.h"



namespace BiBuild {
    void Time::UpdateTime() {
        auto nowTimePoint = std::chrono::steady_clock::now();
        Get().deltaTime = std::chrono::duration<double>(nowTimePoint - Get().lastTimePoint).count();
        Get().fDeltaTime = static_cast<float>(Get().deltaTime);
        Get().currentTime = std::chrono::duration<double>(nowTimePoint - Get().startTimePoint).count();
        Get().fCurrentTime = static_cast<float>(Get().currentTime);
        Get().lastTimePoint = nowTimePoint;
    }

    double Time::DeltaTime() {
        return Get().deltaTime;
    }

    double Time::CurrentTime() {
        return Get().currentTime;
    }

    const double* Time::DeltaTimePointer() {
        return &Get().deltaTime;
    }

    const double* Time::CurrentTimePointer() {
        return &Get().currentTime;
    }

    const float* Time::fDeltaTimePointer() {
        return &Get().fDeltaTime;
    }

    const float* Time::fCurrentTimePointer() {
        return &Get().fCurrentTime;
    }
} // BiBuild