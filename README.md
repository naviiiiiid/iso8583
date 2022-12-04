# Iso8583lib 
dynamic C++ library for generate and parse iso8583 in any standard ! Since I didn't find many "generic" solutions in C++ I decided
to give it a shot and try to implement one.

-----------------------Message Parser & Generator ---------------
-help or -h For Help
-parse
         Please Enter "ISOVersionFileConfig(json) message"
         For Example : iso8583_v1998 010012145252415552454545454.....
-generate
         Please Enter "ISOVersionFileConfig(json) formatElement(.json) [ip port]"
         For Example : iso8583_v1998 formatElement 0.0.0.0 0000
---------------------------------------------------------------------------

./ISOSimulator -parse iso8583_v2003.json "0159020000000000......."

---------------------------------------------------------------------------

./ISOSimulator -generate iso8583_v2003.json transaction1.json 127.0.0.1 8080

---------------------------------------------------------------------------
