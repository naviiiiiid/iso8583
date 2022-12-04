#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <Utils.hpp>

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>


#include <fstream>
#include <vector>


namespace isolib
{
  int fromHex(char c)
  {
    switch(c)
    {
      case '0':
        return 0;
      case '1':
        return 1;
      case '2':
        return 2;
      case '3':
        return 3;
      case '4':
        return 4;
      case '5':
        return 5;
      case '6':
        return 6;
      case '7':
        return 7;
      case '8':
        return 8;
      case '9':
        return 9;
      case 'a':
      case 'A':
        return 10;
      case 'b':
      case 'B':
        return 11;
      case 'c':
      case 'C':
        return 12;
      case 'd':
      case 'D':
        return 13;
      case 'e':
      case 'E':
        return 14;
      case 'f':
      case 'F':
        return 15;
      default:
        throw std::invalid_argument("Not a hex char");
    }
  }

  std::string readFixedField(std::istringstream& iss, size_t length)
  {
    std::string result(length, ' ');
    iss.read(&result[0], length);
    if (!iss.good() || iss.gcount() != static_cast<std::streamsize>(length))
    {
      throw std::runtime_error("Error while reading field content");
    }
    return result;
  }

  std::string readVarField(std::istringstream& iss, size_t headerLength)
  {
    if (headerLength > 4)
      throw std::invalid_argument("readVarField: length specifier is too long");

    char buff[4];
    iss.read(buff, headerLength);
    if (!iss.good() || iss.gcount() != static_cast<std::streamsize>(headerLength))
    {
      throw std::runtime_error("readVarField: error while reading length specifier");
    }

    size_t length = 0;
    for(size_t i = 0; i < headerLength; i++)
    {
      const int digit = buff[i] - '0';
      if (digit < 0 || digit > 9)
        throw std::runtime_error("The length specifier contains invalid characters");
      length = (length * 10) + digit;
    }

      return readFixedField(iss, length);

  }

  size_t getNumberOfDigits(size_t number)
  {
    size_t digits = 1;

    while (number /= 10)
    {
      digits++;
    }

    return digits;
  }

  void validateMessageType(const std::string& mt)
  {
    if (mt.size() != 4)
      throw std::runtime_error("Message type has wrong length, should be 4");
    auto allDigits = std::all_of(mt.begin(), mt.end(), [](const char& c) {return c >= '0' && c <= '9';});
    if (!allDigits)
      throw std::runtime_error("Message type can only contain digits");
  }

  std::string pad_right(std::string const &str, size_t s, char ch) {
        if (str.size() < s)
            return str + std::string(s - str.size(), ch);
        else
            return str;
    }

  std::string pad_left(std::string const &str, size_t s, char ch) {
        if (str.size() < s)
            return std::string(s - str.size(), ch) + str;
        else
            return str;
    }

  std::string sendAndRecieve(const std::string& messageRequest, const std::string& ip, int port) {

      int sock = 0;
      struct sockaddr_in serv_addr;

      char buffer[1024] = {0};
      if ((
                  sock = socket(AF_INET, SOCK_STREAM, 0)
          ) < 0) {
          printf("\n Socket creation error \n");
          return "-1";
      }

      serv_addr.
              sin_family = AF_INET;
      serv_addr.
              sin_port = htons(port);

      if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
          printf("\nInvalid address/ Address not supported \n");
          return "-1";
      }

      if (connect(sock,
                  (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
          printf("\nConnection Failed \n");
          return "-1";
      }


      send(sock, messageRequest.c_str(), strlen(messageRequest.c_str()),
           0);
      read(sock, buffer,
           1024);

      printf("Message From Server :  %s\n", buffer);

      return std::string(buffer);

  }

  std::string readFromFile(const std::string& pathFile) {

      std::ifstream myfile(pathFile);

      if (myfile.good()) {
          /*Read data using streambuffer iterators.*/
          std::vector<char> buf((std::istreambuf_iterator<char>(myfile)), (std::istreambuf_iterator<char>()));

          /*str_buf holds all the data including whitespaces and newline .*/
          std::string str_buf(buf.begin(), buf.end());

          myfile.close();

          return str_buf;
      }
  }
}

