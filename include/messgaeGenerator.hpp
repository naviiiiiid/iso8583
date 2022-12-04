#pragma once

#include <string>
#include <regex>
#include <stdexcept>

#include <Utils.hpp>
#include <fstream>


class messageGenerator {

private:
    std::map<std::string, std::map<std::string, std::vector<std::string>>> formatElements;

public :
    messageGenerator(std::map<std::string, std::map<std::string, std::vector<std::string>>> formatElements) {
        this->formatElements = formatElements;

    }

    ~messageGenerator() {

    }

    std::map<std::string, std::string> generate() {
        std::map<std::string, std::string> temp;

        for (auto rec:this->formatElements) {
            std::string values;
            int counter = 0;
            for (const auto &recn:rec.second["values"]) {
                std::regex rgx(R"(\[(.*)\(([0-9hmsYMD]*)\)\])");
                std::smatch matches;
                if (std::regex_search(recn, matches, rgx)) {
                    if (matches[1] == "random") {
                        std::random_device r;
                        std::default_random_engine e1(r());
                        std::uniform_int_distribution<int> uniform_dist(1, 1000000);
                        int traceNumber = uniform_dist(e1);
                        std::string trace = std::to_string(traceNumber);
                        trace = isolib::pad_left(trace, atoi(matches[2].str().c_str()), '0');
                        values.append(trace);
                        PLOGD << "random->" << trace ;
                    }
                    if (matches[1] == "input") {
                        std::string input;
                        std::cout << rec.second["types"][counter] << ": ";
                        std::cin >> input;
                        values.append(input);
                        PLOGD << "input->" << input ;
                    }
                    if (matches[1] == "time") {
                         time_t now = time(0);
                         tm *ltm = localtime(&now);
                         std::string timeFormat = matches[2].str().c_str();

                         if (timeFormat == "h") {
                             values.append(isolib::pad_left(std::to_string(ltm->tm_hour), 2, '0'));
                         }
                         if (timeFormat == "m") {
                             values.append(isolib::pad_left(std::to_string(ltm->tm_min), 2, '0'));
                         }
                         if (timeFormat == "s") {
                             values.append(isolib::pad_left(std::to_string(ltm->tm_sec), 2, '0'));
                         }

                        PLOGD << "time->" << values ;
                    }
                    if (matches[1] == "date") {
                         time_t now = time(0);
                         tm *ltm = localtime(&now);
                         std::string dateFormat = matches[2].str().c_str();

                         if (dateFormat == "M") {
                             std::string month = std::to_string(ltm->tm_mon+1);
                             month = isolib::pad_left(month, 2, '0');
                             values.append(month);
                         }
                         if (dateFormat == "D") {
                             std::string day = std::to_string(ltm->tm_mday);
                             day = isolib::pad_left(day, 2, '0');
                             values.append(day);
                         }
                         if (dateFormat == "Y") {
                         }
                        PLOGD << "date->" << values ;
                    }
                } else {
                    values.append(recn);
                    PLOGD << "else->" << recn ;
                }
                counter++;
            }
            temp.insert({rec.first, values});
            PLOGD << "temp.insert->" << values ;
        }
        return temp;
    }

};
