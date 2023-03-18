all: RSAencryption 


#RSAencryption is the program to encrpyt / decrypt primes.
#to-do add BigIntLib makefile dependency
RSAencryption: RSAencryption.cpp 
	g++ -std=c++11 -g -w -Wall RSAencryption.cpp BigInteger.o BigIntegerAlgorithms.o BigIntegerUtils.o BigUnsigned.o BigUnsignedInABase.o -o RSAencryption

		
#program to fiddle with stuff -- not for public consumption
#to-do add BigIntLib makefile dependency
FooFiddle: FooFiddle.cpp 
	g++ -g -w -Wall FooFiddle.cpp BigInteger.o BigIntegerAlgorithms.o BigIntegerUtils.o BigUnsigned.o BigUnsignedInABase.o -o FooFiddle

#list of objects for BigIntCreates
BigIntLib: BigUnsigned.cc BigInteger.cc BigIntegerAlgorithms.cc BigUnsignedInABase.cc
	g++ -c -w -O2 -Wall -Wextra -pedantic BigUnsigned.cc
	g++ -c -w -O2 -Wall -Wextra -pedantic BigInteger.cc
	g++ -c -w -O2 -Wall -Wextra -pedantic BigIntegerAlgorithms.cc
	g++ -c -w -O2 -Wall -Wextra -pedantic BigUnsignedInABase.cc
	g++ -c -w -O2 -Wall -Wextra -pedantic BigIntegerUtils.cc

clean: 
	  $(RM) RSAencryption