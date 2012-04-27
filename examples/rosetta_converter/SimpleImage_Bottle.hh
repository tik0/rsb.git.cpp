#pragma once

#include <rosetta/api.h>
#include "support.hh"

namespace tutorial {
namespace rosetta_converter {

class SimpleImage {
public:
    boost::uint32_t width() { return this->_width; }
    boost::uint32_t height() { return this->_height; }

    boost::uint32_t _width;
    boost::uint32_t _height;
};

}
}

namespace rosetta {

template <>
boost::uint64_t
packedSize<MechanismBottle> (const tutorial::rosetta_converter::SimpleImage& /*sourceVar9644*/) {
    return  (8 + (4 + (4 + (4 + 4))));

}
template <>
boost::uint64_t
pack<MechanismBottle> (const tutorial::rosetta_converter::SimpleImage& sourceVar9653 ,
                       std::vector<unsigned char>&                     destinationVar9654 ,
                       boost::uint64_t                                 startVar9655 ,
                       boost::uint64_t                                 /*endVar9656*/) {
    boost::uint64_t offsetVar9657 = startVar9655;
    offsetVar9657 = (offsetVar9657
                     + writeInteger<32>(destinationVar9654, 256, offsetVar9657));
    offsetVar9657 = (offsetVar9657
                     + writeInteger <32>(destinationVar9654, 2, offsetVar9657));

    offsetVar9657 = (offsetVar9657
                     + (writeInteger<32>(destinationVar9654, 1, offsetVar9657)
                        + writeInteger<32>(destinationVar9654, sourceVar9653._width, (offsetVar9657 + 4))));

    offsetVar9657 = (offsetVar9657
                     + (writeInteger<32>(destinationVar9654, 1, offsetVar9657)
                        + writeInteger<32>(destinationVar9654, sourceVar9653._height, (offsetVar9657 + 4))));

    return (offsetVar9657 - startVar9655);
}

template <>
boost::uint64_t
unpack<MechanismBottle> (const std::vector<unsigned char>&         sourceVar9659,
                         tutorial::rosetta_converter::SimpleImage& destinationVar9660,
                         boost::uint64_t                           startVar9661,
                         boost::uint64_t                           /*endVar9662*/) {
    boost::uint64_t offsetVar9663 = startVar9661;
    offsetVar9663 = (offsetVar9663 + 8);

    std::vector<unsigned char> temp;
    offsetVar9663 = (offsetVar9663 + 4);
    boost::uint64_t g9667 = readBytes(sourceVar9659, temp, offsetVar9663);
    offsetVar9663 = (offsetVar9663 + g9667);

    offsetVar9663 = (offsetVar9663 + 4);
    boost::uint64_t g9665 = readInteger<32>(sourceVar9659, destinationVar9660._width, offsetVar9663);
    offsetVar9663 = (offsetVar9663 + g9665);

    std::vector<unsigned char> temp2;
    offsetVar9663 = (offsetVar9663 + 4);
    boost::uint64_t g9668 = readBytes(sourceVar9659, temp, offsetVar9663);
    offsetVar9663 = (offsetVar9663 + g9668);

    offsetVar9663 = (offsetVar9663 + 4);
    boost::uint64_t g9666 = readInteger<32>(sourceVar9659, destinationVar9660._height, offsetVar9663);
    offsetVar9663 = (offsetVar9663 + g9666);

    return (offsetVar9663 - startVar9661);
}

}
