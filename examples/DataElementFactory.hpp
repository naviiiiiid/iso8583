#pragma once

#include <functional>
#include <memory>
#include <string>

#include <DataElement.hpp>

/* The intention here is to have the Iso Message receive the data element
 * factory as a template parameter assuming that the type passed to the
 * parameter will contain a createDataElement static method through which
 * all available data elements are created. This way switching from one kind
 * of data element to another is just a matter of creating a message with a
 * different factory as template parameter. This means that there's no much
 * code to be provided 'library-wise' so the factory declared here is more
 * of an isoFactory.
 */

namespace isolib
{
  namespace isoFactory
  {
    using CreationMap = std::map<std::string, std::unique_ptr<DataElementBase>>;
    using IsotypeMap = std::map<std::string, std::shared_ptr<Isotype>>;


    struct ISOFactoryStruct {
         static std::unique_ptr<DataElementBase> create(const std::string &id);

        static void createMap(const std::string& pathConfigFile);

    private:
          static CreationMap creationMap_;
          static IsotypeMap IsotypeMap_;

    };
  }
}
