#include <functional>
#include <memory>
#include <string>

#include <DataElement.hpp>
#include <DataElementFactory.hpp>
#include <IsoType.hpp>

#include <single_include/nlohmann/json.hpp>
#include <plog/Log.h>

using namespace isolib;
using namespace isolib::isoFactory;


CreationMap ISOFactoryStruct::creationMap_;
IsotypeMap ISOFactoryStruct::IsotypeMap_;

std::unique_ptr<DataElementBase> ISOFactoryStruct::create(const std::string& id) {

    const auto &it = creationMap_.find(id);

    if (it == std::end(creationMap_))
        throw std::runtime_error("There is no data element with Id " + id);
    else
        return std::move(it->second);
}

void ISOFactoryStruct::createMap(const std::string& pathConfigFile) {

   nlohmann::json jsonObj = nlohmann::json::parse(readFromFile(pathConfigFile));

    std::map<std::string, LengthType> LengthTypeMap;
    LengthTypeMap["Variable"] = LengthType::Variable;
    LengthTypeMap["Fixed"] = LengthType::Fixed;


    std::map<std::string, isolib::PaddingType > paddingTypeMap;
    paddingTypeMap["Left"] = PaddingType::Left;
    paddingTypeMap["Right"] = PaddingType::Right;
    paddingTypeMap["None"] = PaddingType::None;

    for (auto &el : jsonObj["isoType"].items()) {

        const std::string &typeName = el.key();
        std::string paddingType = el.value()["paddingType"];

        std::string pCharStr = (el.value()["pChar"]);
        char const *ca = pCharStr.c_str();
        char padChar = ca[0];

        std::string pattern = el.value()["pattern"];

        auto IsotypeRet =  std::make_shared<Isotype>(typeName, paddingTypeMap[paddingType], padChar, pattern);
        IsotypeMap_[typeName] = IsotypeRet;
      }

    PLOGD << "Create ISO Type Successfully ";

    for (auto &el : jsonObj["dataElements"].items()) {

        std::string dataElement = el.value()["dataElement"];
        std::string lengthType = el.value()["lengthType"];
        std::string maxLength = el.value()["maxLength"];

        std::unique_ptr<DataElementBase> ret;
        const std::string &de = el.key();

        ret = std::make_unique<DataElement>(IsotypeMap_[dataElement], LengthTypeMap[lengthType], std::stoi(maxLength));
        ret->setName(de);
        creationMap_[de] = std::move(ret);

    }

    PLOGD << "Create DateElement in ISO Version Config Successfully ";
}



