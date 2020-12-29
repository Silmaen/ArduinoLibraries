//
// Created by damien.lachouette on 11/12/2020.
//
#include "MacAddress.h"
#include <StreamString.h>
namespace network{

    size_t  MacAddress::printTo(Print& p) const{
        size_t n = 0;
        for(uint8_t i=0;i<6;++i) {
            constexpr char pszNibbleToHex[] = {"0123456789ABCDEF"};
            n += p.print(pszNibbleToHex[mac[i]>>4u]);
            n += p.print(pszNibbleToHex[mac[i] & 0x0FU]);
            if (i != 5)
                n += p.print(':');
        }
        return n;
    }

    String MacAddress::toString()const {
        StreamString sstr;
        sstr.reserve(17); // 4 bytes with 3 chars max + 3 dots + nullterm, or '(IP unset)'
        printTo(sstr);
        return sstr;
    }
};
