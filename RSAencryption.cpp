//This program computes the trigraph code for a message using the RSA algorithm.
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <cstddef>
#include <math.h>
#include <sstream>
#include <time.h>
#include <iomanip>
#include <chrono>
#include <time.h>

#include "BigIntegerLibrary.hh"   //this external library handles big unsigned integers for us
#include "timer.cpp"              //this external library handles test timing for us
#include "RSAcontext.cpp"         //written by us, it holds the structure of information for RSA encryption and methods to do it


const int PLAINTEXTREADSIZE  = 200; //m < n -- Computer Security Practices and Principles
                                    //prime generation happens in another process, and the file of primes are required before running this program
                                    //the two lowest primes are ~10^100. 10 ^200 in base 8 is around 226 octal digits
const int BLOCKWRITESIZE = 450;     // 10^200 *10^ 200 = 10^402. 10^402 in base 8 is around 446 octal digits. 


using namespace std;


//function prototypes
vector<BigUnsigned> getPrime(); // gets prime numbers from generated prime file
RSAConxtext         select_P_Q_E(vector<BigUnsigned>);// will return vector of primes in this specific order
vector<char>        getMessageFromFile(); //Reads plaintext file and puts all characters into vector
void                encryptMessage(vector<char>, RSAConxtext); // reads vector of chars in plain text and writes cipher text file
void                decryptMessage(vector<char> originaltext, RSAConxtext RSAinfo);
BigUnsigned         convertCharsToBigUnsigned(string input);//helper funcion to convert string of chars (which are base 256) to BigUnsigned
string              convertBigUnsignedToBase256(BigUnsigned);
void                testPrimeLengthTiming(vector<char>, vector<BigUnsigned> Primes);
void                testMessageLengthTiming(vector<char>, vector<BigUnsigned> Primes);

string inline       to_string(vector<char> msg){ string m; for(int i =0; i < msg.size(); i++) m.append(1,msg[i]); return m;}//inline function to convert a char vector and returns a string
    

//Main program method
int main(){
    srand(time(0));                          //seed the time for random selections later
    vector<BigUnsigned>    primes;           //a list of primes read from an existing file
    RSAConxtext            PQE;              //RSA p q and e data structure
    vector<char>           plaintext;        //a vector of plaintext characters read from the file
    
    
    
    //primes are generated from primegeneration process and stroed in a file. Source code is available in primegeneration.cc
    primes = getPrime(); // all prime numbers from file have been recieved
    if (primes.empty()) {
        return 9999;
    }
	
	
    PQE = select_P_Q_E(primes); // prime numbers selected for p,q,e
    plaintext = getMessageFromFile(); // reads file for plaintext and stores in plaintext vector

    encryptMessage(plaintext,PQE); //write encrypt to file.
    decryptMessage(plaintext, PQE);//reads from the file and decrypts
    
    testPrimeLengthTiming(plaintext, primes);//runs test suite for prime length timing
    testMessageLengthTiming(plaintext, primes);//runs test suite for message length timing

    return 0;
}

/// @brief Attempts to read primes-sorted.dat file and store them into a vector of big unsigned. It does not check for primeness
/// @return a vector of numbers read from file
vector<BigUnsigned> getPrime() {
    
    vector<BigUnsigned> primes;                       //location to store primes
    ifstream inputFile("primes-sorted.dat");          //name of the file for prime storage
    
    if (inputFile.is_open() == false) {
        printf("file primes-sorted.dat failed to open");
		printf("please run prime generation");
        return primes;
    }
    
	cout << "loading primes from file" << endl;
	string c;                                         // temp storage for file reads

    //while(primes.size() < 20){  //testing memory problems.
	//read every entry and convert it to a number to be stored in a bigunsigned variable
    while (inputFile) {
        inputFile >> c;
		primes.push_back(stringToBigUnsigned(c));
    }
	
	cout << "loaded [ " << primes.size() << " ] primes from file" << endl;
    inputFile.close();
    return primes;
}

/// @brief fills a [RSAcontext] with random primes and e
/// @param a pre-checked list of primes
/// @return a vector of numbers read from file
RSAConxtext select_P_Q_E(vector<BigUnsigned> primes) {
    RSAConxtext PQE;  //data structure to hold primes and keys
    
    // continiuosly draw primes till the RSAContext class accepts them as valid.
    while (!PQE.setP(primes.at(rand() % primes.size())))
        ; // nop to keep drawing
    while (!PQE.setQ(primes.at(rand() % primes.size())))
        ; // nop to keep drawing

    //fill e with a random number
    //d is a calcuation from e
    BigUnsigned e;
    do
    {
        // cout << "setting e" << endl;
        e = (unsigned)rand() | 3;
    } while (!PQE.sete(e));

    /*
    //manual prime and e selection process
    while (flag == 0) {
        printf("please input element number 0-999 for prime p \n");
        cin >> x;
        printf("please input element number 0-999 for prime q that is not equal to p: \n");
        cin >> y;
        printf("please input element number for e that is not equal to p or q \n");
        cin >> z;
        if (x > -1 && x <= 999) {
            if (y > -1 && y <= 999 && y != x) {
                if (z > -1 && z < 999 && z != x && z != y) {
                    flag = 1;
                }
            }
        }
        else {
            cout << "numbers for input do not follow parameters \n";
        }
    }
    */

    return PQE;
}

/// @brief reads hard coded file for a message to be encrypted
/// @return  a vector of all the characters read from the file
vector<char> getMessageFromFile(){
    vector<char> plaintext;                      //the storage for the entire message
    ifstream inputFile("plaintext-sent.txt");    //file stread to read from
    
    cout << "reading plaintext from file" << endl;
    //grab each character one by one to ensure we don't ignore whitespace or other characters
    char c;
    while(inputFile.get(c)){
        plaintext.push_back(c);
    }  
    inputFile.close();

    cout << "File read [ " << plaintext.size() << " ] characters" << endl;

    //Iterate through the plaintext vector to calculate and place in padding needed for block encrpytion
    while(plaintext.size() % PLAINTEXTREADSIZE != 0){
        plaintext.push_back(' ');
    }
    
    cout << "padding to [ " << plaintext.size() << " ] characters" << endl;
    return plaintext;
}

/// @brief accepts plain text and writes it (as blocks) to a file
/// @param plainText the message to be encrypted. It's size should already be padded to a multiple of the block size
/// @param RSAinfo //the context needed to encrpyt the data
void encryptMessage(vector<char> plainText, RSAConxtext RSAinfo) {
    int blocksNeeded = (plainText.size() / PLAINTEXTREADSIZE);//integer division to figure out how many blocks
    string block[blocksNeeded];//trying to use a vector of chars, gives memory allocation errors because of vector's internals
    ofstream outFile("CipherText.txt");//destination of the plaintextblocks

    //the plaintext should have already been padded
    if( plainText.size() % PLAINTEXTREADSIZE != 0){
        cout << "encryptMessage, plaintext isn't padded correctly, throwing error " << endl;
        throw "encryptMessage, plaintext isn't padded correctly, throwing error ";
    }
    
    outFile << blocksNeeded << endl;
    cout << "encrypting message" << endl <<endl;

    //loop through all blocks needed
    for (int i = 0; i < blocksNeeded; i++)
    {   
        //for each block, construct the message
        cout << "writing block [ " << i << endl;
        for (int j = 0; j < PLAINTEXTREADSIZE; j++)
        {
            block[i].append(1,plainText.at((i * PLAINTEXTREADSIZE) + j));
        }
        
        //convert to base 10 and encrypt
        BigUnsigned base10 = convertCharsToBigUnsigned(block[i]);
        base10 = RSAinfo.encryptBlock(base10);

        //padd cipher block character to length -- this adds leading 0s
        string cipherBlock = convertBigUnsignedToBase256(base10);
        while (cipherBlock.size()%BLOCKWRITESIZE != 0)
            cipherBlock.insert(cipherBlock.begin(), (char)0 );
            //cipherBlock.append(1,' ');
        outFile << cipherBlock;

    }

    cout << "file closed"<< endl;
    outFile.close();
    return;
}


/// @brief decypts CipherText.txt and compares results to [original text]
/// @param originalText what the decryption should be
/// @param RSAinfo modulus and keys for RSA
void decryptMessage(vector<char> originalText, RSAConxtext RSAinfo) {
    ifstream inFile("Ciphertext.txt",ios::binary);
    string cipherMessage;
    string plainMessage = to_string(originalText);
    int cipherBlockAmount;

    cout << endl << "opening cipher text file..." << endl;
    inFile >> cipherBlockAmount;//read the amount of cipher blocks we are expected to get     
    inFile.get();//don't leave newline char hanging
    

    for(int i=0; i < cipherBlockAmount * BLOCKWRITESIZE-12; i ++){
    //for(int i=0; i < cipherBlockAmount * BLOCKWRITESIZE; i ++){
        cipherMessage.append(1,inFile.get());
    }
    

    //cout << "reading ciphermessage size [ " << cipherMessage.size() << endl;
    cout << "reading cipher blocks [ " << cipherBlockAmount << endl;


    //decrypte the message block by block
    string cipherBlock;
    for(int i =0; i < cipherBlockAmount; i++){
        cout << "decryping block [ " << i << endl;
        cipherBlock.clear();
        for(int j =0; j< BLOCKWRITESIZE; j++){
            cipherBlock.append(1,cipherMessage[(i*BLOCKWRITESIZE)+j]);
        }

        //convert the message into base10 that BigUnsigned can use
        BigUnsigned BlockBase10 = convertCharsToBigUnsigned(cipherBlock);
        BlockBase10 = RSAinfo.decryptBlock(BlockBase10);
        cipherBlock = convertBigUnsignedToBase256(BlockBase10);
        cipherMessage.append(cipherBlock);
    }

    if(plainMessage.compare(cipherMessage)){
        cout << "cipher decryption returned original" << endl;
    }
    else
        cout << "cipher decryption failed" << endl;

}


/// @brief helper funcion to convert string of chars (which are base 256) to BigUnsigned
/// @param input charater in base 256 to convert to bigunsigned base 10
/// @return base 10 representation of the input
BigUnsigned convertCharsToBigUnsigned(string input)
{
	BigUnsigned multiple = 1;       //multiple = digit_i = 256^i
    BigUnsigned tally = 0;          //total of intermediate addition

    //loop through every character and add to tally
    for(int i= 0; i < input.size(); i++){
        tally += multiple * (unsigned) input.at(i); 
        multiple *= 256;
    }

    return tally;
}

/// @brief //helper funcion to convert number to chars (which are base 256) 
/// @param input a nubumer
/// @return a base 256 representation of the input
string convertBigUnsignedToBase256(BigUnsigned input)
{
	string text;
    char temp;
    
    while(input>0){
        temp = (char) (input % 256).toInt(); //there are faster ways to do this, but it's not worth the typing time
        input.bitShiftRight(input, 8);

        text.append(1, temp);
    }
    

    return text;

	
}

/// @brief helper function to count the binary digits needed to represent the number
/// @return amount of binary digits needed for the input
unsigned getLengthInBits (BigUnsigned input){
    unsigned count = 0;
    while (input>0){
        input = input >> 1;
        count++;
    }

    return count;

}

//
void testMessageLengthTiming(vector<char> message, vector<BigUnsigned> primes){
    int testRounds = 40;                                    //how many rounds of testing to do
    int seed = 665;                                         //a seed for randomness to make testing repeatable
    srand(seed);
    string msg;                                             //the plain text message stored as a string
    const string filename= "testMsgLengthTiming.csv";       //the name of the file to store test results
    ofstream outFile(filename);                             //the file stream from the [filename]
    RSAConxtext RSA;                                        //data structure to hold primes and keys during RSA
    BigUnsigned msgBase10;                                  //plaintext stored in base 10 BigUnsigned format
    Timer clock;                                            //timer class to measure 
    Timer totalTime;                                        //timer to measure total for personal reasons
    totalTime.reset();

    double encryptionTime;                                  //how long encryption took -- filled by timer class
    double decryptionTime;                                  //how long decryption took
    BigUnsigned preLoadedCipher;                            //used to eliminate memory allocation time from encryption trials
    
    //read the entire message into a string for formatting reasons
    for(int i =0; i< message.size(); i++){
        msg.append(1,message[i]);
    }


    cout << "testing message length timing "<< endl << endl;
    cout << "doing [ " << testRounds << " ] rounds" << endl;
    cout << "using random seed [ " << seed << endl;
    
    //fill RSA information 
    RSA = select_P_Q_E(primes);
    RSA.printContext();

    cout << "setting fileheader" << endl;

    outFile << "prime one length (bin digits), " << getLengthInBits(RSA.getP()) << endl;
    outFile << "prime two length (bin digits), " << getLengthInBits(RSA.getQ()) << endl;
    outFile << "modulus - n length (bin Digits), " << getLengthInBits(RSA.getn()) << endl;
    outFile << "phi(n) length (bin digits), " << getLengthInBits(RSA.getphin()) << endl;
    outFile << "random seed used, " << seed << endl;

    outFile << "message length (bin digits), encryption time (seconds), decryption time (seconds)" << endl;

    //this is the test loop
    //it 
    for (int i = 0; i < testRounds; i++)
    {
        string testMessage;                //this holds a substring of the message to be tested on
        unsigned messageRoundSize;         //how big the message will be this round of testing
        unsigned messageRoundOffset;       //the offset from the begining of the message to start grabbing a substring

        //randomly grab a sub-string and check it's not outof bounds
        do{
            messageRoundSize = (rand() % PLAINTEXTREADSIZE) ;
            messageRoundOffset = (rand() % PLAINTEXTREADSIZE) ;
        }while (!messageRoundSize && messageRoundOffset + messageRoundSize >= msg.size());

        //make a copy of the sub-string so we can text on it            
        for(int i =0; i < messageRoundSize; i ++){
            testMessage.append(1,msg[i+messageRoundOffset]);
        }
        msgBase10 = convertCharsToBigUnsigned(testMessage);//RSA is done on numbers, so convert the charater into a useful base 10 BigUnsigned

        //report incremental progress of testing
        unsigned reportAfterRound = testRounds/10;
        if (!(i%reportAfterRound))
        {
            cout << "timer since start [ " << totalTime.elapsed() << " ] on test round [ " << i << endl;
        }

        //measure encryption times
        clock.reset();
        RSA.encryptBlock(msgBase10);
        encryptionTime = clock.elapsed();

        //measure decryption times
        preLoadedCipher = RSA.encryptBlock(msgBase10); // we want to remove memory allocation time from the test
        clock.reset();
        RSA.decryptBlock(preLoadedCipher);
        decryptionTime = clock.elapsed();

        //write result to file
        outFile << testMessage.size() << ", " << encryptionTime << ", " << decryptionTime << endl;
    }
    outFile.close();

    cout << "testing done. " << endl;
    cout << "total elapsed time [ " << totalTime.elapsed() << endl;
    cout << "results located in " << filename << endl;

    return;


}

//
void testPrimeLengthTiming(vector<char> message, vector<BigUnsigned>primes){
    int testRounds = 30;                                  //amount of rounds to test
    int seed = 665;                                       //seed to randomly repeat the test
    srand(seed);
    string msg;                                           //input message represented as a string data type
    const string filename= "testPrimeLengthTiming.csv";   //file to write results to
    ofstream outFile(filename);                           //out stream of file
    RSAConxtext RSA;                                      //context  of information to do RSA
    BigUnsigned msgBase10;                                //a base 10 representation of the message
    Timer clock;                                          //a timer object to measure
    Timer totalTime;                                      // timer object to measure total time for personal use
    totalTime.reset();

    double encryptionTime;                                //time in seconds it took to encrypt
    double decryptionTime;                                //time in seconds it took to decrypt
    BigUnsigned preLoadedCipher;                          //a place to hold a ciphertext (inbase 10) to eliminate measuring memory allocation time


    //read a single plaintext block into msg to test on.
    for(int i =0; i< PLAINTEXTREADSIZE; i++){
        msg.append(1,message[i]);
        
    }
    cout <<" begining prime length testing " << endl << endl;

    cout << "message to encrypt [ " << msg << endl;
    
    msgBase10 = convertCharsToBigUnsigned(msg);

    cout << "doing [ " << testRounds << " ] rounds" << endl;
    cout << "using random seed [ " << seed << endl;
    cout << "setting fileheader" << endl;

    //write the csv cell header
    outFile << "message length (char), " << msg.size() <<endl;
    outFile << "random seed used, " << seed << endl;
    outFile << "prime one length (bin digits), prime two length (bin digits), ";
    outFile << "modulus - n length (bin Digits, phi(n) length (bin digits), ";
    outFile << "encryption time (seconds), decryption time (seconds)" << endl;

    

    for (int i = 0; i < testRounds; i++)
    {

        RSA = select_P_Q_E(primes);

        preLoadedCipher = RSA.encryptBlock(msgBase10); // we want to remove memory allocation time from the test
        
        //report incrementially though the testing
        unsigned reportAfterRound = testRounds/10;
        if (!(i%reportAfterRound))
        {
            cout << "timer since start [ " << totalTime.elapsed() << " ] on test round [ " << i << endl;
        }

        //test encryption
        clock.reset();
        RSA.encryptBlock(msgBase10);
        encryptionTime = clock.elapsed();

        //test decryption
        clock.reset();
        RSA.decryptBlock(preLoadedCipher);
        decryptionTime = clock.elapsed();

        //store results
        outFile << getLengthInBits(RSA.getP()) << ", " << getLengthInBits(RSA.getQ()) << ", ";
        outFile << getLengthInBits(RSA.getn()) << ", " << getLengthInBits(RSA.getphin()) << ",";
        outFile << encryptionTime << ", " << decryptionTime << endl;
    }
    outFile.close();

    cout << "testing done. " << endl;
    cout << "total elapsed time [ " << totalTime.elapsed() << endl;
    cout << "results located in " << filename << endl;

    return;

}
