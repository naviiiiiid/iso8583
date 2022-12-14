#pragma once

#include <map>
#include <memory>
#include <string>
#include <sstream>

#include <Utils.hpp>
#include "IsoType.hpp"

#define UNUSED(expr) do { (void)(expr); } while (0)

namespace isolib
{
  constexpr size_t MaxAllowedDataElementLength = 999;
  enum class LengthType : char { Fixed, Variable };


  class DataElementBase
  {
  public:
    DataElementBase(const std::string& name): _name(name) {};

    DataElementBase(): DataElementBase("_unnamed_") {};

    virtual ~DataElementBase() {};

    virtual std::string toString() const = 0;

    virtual void parse(std::istringstream& iss) = 0;

    virtual void setValue(const std::string& val) = 0;

    void setName(const std::string& newName)
    {
      _name = newName;
    }

    std::string getName() const
    {
      return _name;
    }

    friend bool operator==(const DataElementBase& lhs, const DataElementBase& rhs)
    {
      return lhs.compare(rhs);
    }

    friend bool operator!=(const DataElementBase& lhs, const DataElementBase& rhs)
    {
      return !(lhs == rhs);
    }
  private:
    virtual bool compare(const DataElementBase& other) const = 0;
  private:
    std::string _name;
  };


  //template <typename Isotype>
  class DataElement: public DataElementBase
  {
  public:
    DataElement( std::shared_ptr<Isotype> isotype, const std::string& name = "_unnamed_",
      const std::string val = "",
      LengthType lType = LengthType::Variable,
      size_t maxLength = MaxAllowedDataElementLength):
        DataElementBase(name),
        _value(val),
        _lengthType(lType),
        _maxLength(maxLength)
    {
        this->isotype = isotype;

       if (!val.empty())
      {
           try {
               this->isotype->validate(val);
           }
           catch (std::exception &ex) {
               std::cout << getName();
               throw ex;
           }
        if (_value.size() > _maxLength)
          throw std::invalid_argument("Data element " + getName() + " has a maximum length of " + std::to_string(_maxLength));
      }
    }

    DataElement(std::shared_ptr<Isotype> isotype , const std::string& val, LengthType lType, size_t maxLength = MaxAllowedDataElementLength):
      DataElement( isotype  , "_unnamed_", val, lType, maxLength)
    {}

    DataElement(std::shared_ptr<Isotype> isotype ,LengthType lType, size_t maxLength = MaxAllowedDataElementLength):
      DataElement( isotype , "_unnamed_", "", lType, maxLength)
    {}

    void setValue(const std::string& newVal) override
    {
        this->isotype->validate(newVal);
      if (newVal.size() > _maxLength)
        throw std::invalid_argument("Maximum length allowed for data element " + getName() + " is " + std::to_string(_maxLength));
      _value = newVal;
    }

    std::string getValue() const
    {
      return _value;
    }

    virtual std::string toString() const override
    {
      if (_lengthType == LengthType::Variable)
      {
        const auto headerPrefixSize = getNumberOfDigits(_maxLength) - getNumberOfDigits(_value.size());
        std::string ret(headerPrefixSize, '0');
        ret += std::to_string(_value.size());
        ret += _value;

        return ret;
      }
      else
      {
        std::string ret(_maxLength - _value.size(), this->isotype->getPadChar());
        switch(this->isotype->getPaddingType())
        {
          case PaddingType::Left:
            return ret + _value;
          case PaddingType::Right:
            return _value + ret;
          case PaddingType::None:
            return _value;
          default:
            throw std::runtime_error("IsoType of data element " + getName() + " has invalid padding type");
        }
      }
    }

    virtual void parse(std::istringstream& iss) override
    {
      if (_lengthType == LengthType::Variable)
      {
        _value = readVarField(iss, getNumberOfDigits(_maxLength));
      }
      else
      {
        _value = readFixedField(iss, _maxLength);
      }

      try
      {
          this->isotype->validate(_value);
      }
      catch(const std::invalid_argument& e)
      {
        throw std::runtime_error("While parsing input for data element " + getName() + ": " + e.what());
      }
    }

  private:
    virtual bool compare(const DataElementBase& other) const override
    {
      auto rhs = dynamic_cast<const DataElement*>(&other);
      if (rhs)
      {
        if (_lengthType != rhs->_lengthType)
          return false;
        if (_maxLength != rhs->_maxLength)
          return false;
        if (_value != rhs->_value)
          return false;
        // The name is not considered a fundamental attribute of a data element
        return true;
      }

      return false;
    }

  private:
    std::string _value;
    LengthType _lengthType;
    size_t _maxLength;


    //

       std::shared_ptr<Isotype> isotype;

  };



    class DataElementComposite final : public DataElementBase
  {
  public:
    DataElementComposite(const std::string& name):
      DataElementBase(name)
    {};

    DataElementComposite() = default;

    ~DataElementComposite() {};

    std::string toString() const override
    {
      std::ostringstream os;
      for(const auto& de : _children)
      {
        os << de.second->toString();
      }

      return os.str();
    }

    void setValue(const std::string& newVal) override
    {
      UNUSED(newVal);
      throw std::runtime_error("Composite data element " + getName() + " does not support setValue operation");
    }

    void parse(std::istringstream& iss) override
    {
        try {
            for (auto &child : _children) {
                child.second->parse(iss);
            }
        }catch (std::exception &e){
            std::cout << e.what();
        }
    }

    DataElementComposite& add(int pos, std::unique_ptr<DataElementBase> de)
    {
      _children[pos] = std::move(de);
      return *this;
    }

    size_t erase(int pos)
    {
      return _children.erase(pos);
    }

  private:
    virtual bool compare(const DataElementBase& other) const override
    {
      const auto rhs = dynamic_cast<const DataElementComposite*>(&other);
      if (rhs)
      {
        if (this->_children.size() != rhs->_children.size())
          return false;

        for (const auto& de : _children)
        {
          try
          {
            if (*de.second != *rhs->_children.at(de.first))
              return false;
          }
          catch(std::out_of_range& e)
          {
            return false;
          }
        }

        return true;
      }

      return false;
    }

  private:
    std::map<int, std::unique_ptr<DataElementBase>> _children;
  };

  class DataElementDecorator : public DataElementBase
  {
  public:
    DataElementDecorator(std::unique_ptr<DataElementBase>&& deb, const std::string& name = "_unnamed_"):
      DataElementBase(name),
      _child(std::move(deb))
    {}

    virtual ~DataElementDecorator() {}

    virtual void setValue(const std::string& newVal) override
    {
      try
      {
        _child->setValue(newVal);
      }
      catch(const std::exception& e)
      {
        throw std::runtime_error("During setValue call on decorated data element " + getName() + ": " + e.what());
      }
    }
  protected:
    std::unique_ptr<DataElementBase> _child;
  };
}
