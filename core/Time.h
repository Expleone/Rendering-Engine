//
// Created by expleoene on 4/22/26.
//

#ifndef VIEWER_TIME_H
#define VIEWER_TIME_H
#include <chrono>


namespace BiBuild {

class Time {
    double deltaTime;
    double currentTime;
    float fDeltaTime;
    float fCurrentTime;
    std::chrono::steady_clock::time_point startTimePoint;
    std::chrono::steady_clock::time_point lastTimePoint;
    Time() {
        deltaTime = 1.0/50.0;
        fDeltaTime = static_cast<float>(deltaTime);
        startTimePoint = std::chrono::steady_clock::now();
        lastTimePoint = startTimePoint;
        currentTime = 0.0;
        fCurrentTime = static_cast<float>(currentTime);
    }
    ~Time() = default;
    static Time& Get() {
        static Time time;
        return time;
    }
public:

    static void UpdateTime();
    static double DeltaTime();
    static double CurrentTime();

    static const double* DeltaTimePointer();
    static const double* CurrentTimePointer();

    static const float* fDeltaTimePointer();
    static const float* fCurrentTimePointer();
};

} // BiBuild

#endif //VIEWER_TIME_H
