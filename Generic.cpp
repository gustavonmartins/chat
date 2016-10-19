#include "Generic.hpp"

bool Timer::getTimerApproval(int waitingTime, bool beepOn) {

    //std::cout<<"Time elapsed is: "<<elapsedTime.asMilliseconds()<<std::endl;

    if (elapsedTime.asMilliseconds()>=waitingTime) {
        if (beepOn) {
            std::cout<<"Beep! at: "<<elapsedTime.asMilliseconds()<<std::endl;
        }
        elapsedTime=sf::Time::Zero;
        return true;
    } else {
        elapsedTime+=clock.restart();
        return false;
    }
}


