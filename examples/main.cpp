#include <iostream>
#include <DataElementFactory.hpp>
#include <IsoMessage.hpp>
#include <random>
#include <single_include/nlohmann/json.hpp>
#define green "\033[1;32m"
#define reset "\033[0m"

using namespace isolib;
using namespace isolib::isoFactory;


#include  <IsoType.hpp>
#include <messgaeGenerator.hpp>
#include <memory>
#include <condition_variable>
#include <plog/Log.h> // Step1: include the headers
#include <plog/Initializers/RollingFileInitializer.h>

enum request{
    parse,
    generate,
    help,
    send
};

static std::map<std::string, request> requestMap;

std::string generator(int argc, char **pString, char **pString1);

void sender(const std::string& stringIP , const std::string& stringPort , const std::string& stringMessage);
static void Initialize(){

    requestMap["-parse"] = parse;
    requestMap["-generate"] = generate;
    requestMap["-help"] = help;
    requestMap["-send"] = send;
}

static void parser(int argc, char **argv, char **envp) {

    try {

        std::cout << green << "-----------------------Parser ---------------" << "\n";

        ISOFactoryStruct::createMap(std::string(argv[2]));
        std::string message = std::string(argv[3]);
        IsoMessage<ISOFactoryStruct> msg((message.substr(4, 4)));
        msg.read(message.substr(4, message.length()-4));

        std::cout << "[MessageType]=" << message.substr(4, 4) << std::endl;
        std::cout << "[BitMap]=" << message.substr(8, 32) << std::endl;

       for (size_t i : msg.getAllFields()) {
           std::cout << "[BIT" << i << "]=" << msg.getField(i)->toString() << std::endl;
       }

        std::cout << green << "-------------------------------------------" << "\n";
        std::cout << reset;

    }
    catch (const std::exception &e) {
        PLOGD << "Exception caught while assembling msg1: " << e.what();
    }

}

std::string generator(int argc, char **pString, char **pString1) {

    try {

        std::cout << green << "-----------------------Generator ---------------" << "\n";

        ISOFactoryStruct::createMap(std::string(pString[2]));

        nlohmann::json jsonObj = nlohmann::json::parse(readFromFile(std::string(pString[3])));
        std::map<std::string, std::map<std::string, std::vector<std::string>>> tempObj;
        for (auto &re : jsonObj["formatElement"].items()) {
            std::map<std::string, std::vector<std::string>> tempMap;

            auto item = re.value()["values"].get<std::vector<std::string>>();
            tempMap.insert({"values", item});

            auto item2 = re.value()["types"].get<std::vector<std::string>>();
            tempMap.insert({"types", item2});

            tempObj.insert({re.key(), tempMap});
        }

        auto messageGeneratorRef = std::make_shared<messageGenerator>(tempObj);
        std::map<std::string, std::string> res = messageGeneratorRef->generate();

        IsoMessage<ISOFactoryStruct> msg1(res["1"]);
        res.erase("1");

        for (const auto &ret:res) {
            msg1.getField(atoi(ret.first.c_str()))->setValue(ret.second);
        }

        std::string messageGenerated = isolib::pad_left(std::to_string(msg1.write().length()), 4, '0') + msg1.write();

        std::cout << "message Generated : " << messageGenerated << std::endl;

        PLOGD << "message Generated : " << messageGenerated ;

        std::cout << green << "-------------------------------------------" << "\n";
        return messageGenerated;
    }
    catch (const std::exception &e) {
        PLOGD << "Exception caught while assembling msg1: " << e.what();
        return "";
    }
}

void sender(const std::string& stringIP , const std::string& stringPort , const std::string& stringMessage) {

    PLOGD << stringIP << stringPort <<  " message ->" << stringMessage;

    PLOGD << "message <-"<< sendAndRecieve(stringMessage, stringIP, atoi(stringPort.c_str()));
}

void showHelp() {

    std::cout << green << "-----------------------Message Parser & Generator ---------------" << "\n" <<
              "-help or -h For Help" << "\n" <<
              "-parse " << "\n" <<
              "\t Please Enter \"ISOVersionFileConfig(json) message\" " <<
              "\n" << "\t For Example : " << "iso8583_v1998 010012145252415552454545454..... " << "\n"<<
              "-generate " << "\n" <<
              "\t Please Enter \"ISOVersionFileConfig(json) formatElement(.json) [ip port]\" " <<
              "\n" << "\t For Example : " << "iso8583_v1998 formatElement 0.0.0.0 0000 " << "\n";
}

int main(int argc, char **argv, char **envp) {


    plog::init(plog::debug, "ISOSimulatorLog"); // Step2: initialize the logger

    PLOGD << "******************************************************************************";

    std::string message;

    Initialize();

    if(argc > 1) {

        std::string key = argv[1];
        auto iterator = requestMap.find(key);

        if (iterator != requestMap.end()) {

            switch (iterator->second) {
                case parse: {
                    PLOGD << "parse ->"<<argv;
                    parser(argc, argv, envp);
                    break;
                }
                case generate: {
                    PLOGD << "generator ->" << argv;
                    message = generator(argc, argv, envp);
                    if (argc > 4) {
                        sender(argv[4], argv[5], message);
                    }
                    break;
                }
                case help: {
                    showHelp();
                    break;
                }
                case send: {
                    PLOGD << "sender ->"<<argv;
                    sender(argv[2]  ,argv[3] ,  argv[4]);
                    break;
                }
                default: {
                    showHelp();
                }
            }
        } else {
            showHelp();
        }
    }
    std::cout << "-----------------------------------------------------" << reset << "\n";

    PLOGD << "******************************************************************************";

    return 0;
}



