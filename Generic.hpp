#ifndef GENERIC_HPP
#define GENERIC_HPP

#include <iostream>

#include <SFML/System.hpp>
#include <SFML/Main.hpp>

struct Timer {
private:
    sf::Clock clock;
    sf::Time elapsedTime;

public:
    Timer() :
        elapsedTime(sf::Time::Zero) {}

    bool getTimerApproval(int, bool);

};

namespace Utilities {
template <typename T, typename R>
void listOn(T& OriginalElement, R& ReferencingVector) {
    ReferencingVector.emplace_back(&(OriginalElement));
}
}


#endif // GENERIC_HPP
