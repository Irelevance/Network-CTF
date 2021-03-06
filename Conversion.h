#ifndef CONVERSION_H_INCLUDED
#define CONVERSION_H_INCLUDED

#include <string>
#include <sstream>

template<typename Number>
const std::string toString(const Number& toConvert) {

    std::stringstream stream;
    stream << toConvert;

    return stream.str();
}

#endif // CONVERSION_H_INCLUDED
