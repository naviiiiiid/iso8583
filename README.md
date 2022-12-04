# iso 8583 simulator 
# transaction load  from json and iso format (87 , 93 , 2033 ) load from json !


./ISOSimulator -parse iso8583_v2003.json "0159020000000000......."
         For Example : iso8583_v1998 010012145252415552454545454.....

---------------------------------------------------------------------------

./ISOSimulator -generate iso8583_v2003.json transaction1.json 127.0.0.1 8080
         For Example : iso8583_v1998 formatElement 0.0.0.0 0000

---------------------------------------------------------------------------
