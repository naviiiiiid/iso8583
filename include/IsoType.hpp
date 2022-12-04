#pragma once

#include <string>
#include <regex>
#include <stdexcept>

#include <Utils.hpp>
#include <fstream>

    class Isotype {


        isolib::PaddingType paddingType;
    public:
        isolib::PaddingType getPaddingType() const {
            return paddingType;
        }

        char getPadChar() const {
            return padChar;
        }

        const std::string &getPattern() const {
            return pattern;
        }

        const std::string &getTypeName() const {
            return typeName;
        }


    private:
        char padChar{};
        std::string pattern;
        std::string typeName;


    public :

        Isotype(std::string
        typeName,
        isolib::PaddingType paddingType,
        char padChar, std::string
        pattern) {

            this->typeName = typeName;
            this->paddingType = paddingType;
            this->padChar = padChar;
            this->pattern = pattern;
        }

        ~Isotype() {

        }

        void validate(const std::string &str) {
            std::regex rgx(this->pattern);
            if (!std::regex_match(str, rgx))
                throw std::invalid_argument(str + " is not a valid " + "" + " IsoType");
        }

    };
