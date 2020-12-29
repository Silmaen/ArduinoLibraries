//
// Created by damien.lachouette on 11/12/2020.
//

#pragma once
#include <Arduino.h>

namespace network {

    class MacAddress {
    public:
        MacAddress() =default;
        ~MacAddress() =default;
        MacAddress(const MacAddress&) =default;
        MacAddress(MacAddress&&) =default;
        MacAddress& operator=(const MacAddress&) =default;
        MacAddress& operator=(MacAddress&&) =default;
        explicit MacAddress(const uint8_t* m ){
            for(uint8_t i=0;i<6;++i)mac[i]=m[i];
        }
        const uint8_t* toBytes(){return mac;}

        // operator
        bool operator==(const MacAddress& o)const{
            for(uint8_t i=0;i<6;++i){
                if (mac[i]!=o.mac[i]) return false;
            }
            return true;
        }
        bool operator!=(const MacAddress& o)const{
            for(uint8_t i=0;i<6;++i){
                if (mac[i]!=o.mac[i]) return true;
            }
            return false;
        }

        virtual size_t printTo(Print& p) const;
        String toString()const;
    private:
        uint8_t mac[6];

    };

} // namespace network
