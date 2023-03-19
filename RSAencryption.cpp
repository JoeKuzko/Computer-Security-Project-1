//This program computes the trigraph code for a message using the RSA algorithm.
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
//#include <string.h>
#include <cstddef>
#include <math.h>
#include <sstream>
#include <time.h>
#include <iomanip>
#include "BigIntegerLibrary.hh"
#include <chrono>
#include "RSAcontext.cpp"

const int PLAINTEXTREADSIZE  = 200; //m < n -- Computer Security Practices and Principles
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
vector<char>        getBlockAt(vector<char>, int, int); //returns the block at i, seperated by blockSize
string              to_string(vector<char> msg){ string m; for(int i =0; i < msg.size(); i++) m.append(1,msg[i]); return m;}
    

//timing class from https://www.learncpp.com/cpp-tutorial/timing-your-code/
class Timer
{
private:
    // Type aliases to make accessing nested type easier
    using Clock = std::chrono::steady_clock;
    using Second = std::chrono::duration<double, std::ratio<1> >;

    std::chrono::time_point<Clock> m_beg{ Clock::now() };

public:

    void reset()
    {
        m_beg = Clock::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<Second>(Clock::now() - m_beg).count();
    }
}; 
    
	

	

//Main program method
int main(){
    vector<BigUnsigned>    primes;            //a list of primes read from an existing file
    RSAConxtext            PQE;               //RSA p q and e data structure
    vector<char>           plaintext;
    vector<char>           ciphertext;
    
    

    primes = getPrime(); // all prime numbers from file have been recieved
    if (primes.empty()) {
        return 9999;
    }
	
	
    PQE = select_P_Q_E(primes); // prime numbers selected for p,q,e
    plaintext = getMessageFromFile(); // reads file for plaintext and stores in plaintext vector

    //encryptMessage(plaintext,PQE); //write encrypt to file.
    cout << "!!!encryptMessage disabled to work on Decrypt" << endl;
    //decryptMessage(plaintext, PQE);
    cout << "!!!decrypt testing disable to work on timing" << endl;

    testPrimeLengthTiming(plaintext, primes);

    /*
    //enciphered_code = encipher(trigraph_code, e, n);
    //codeToText(enciphered_code);

    //cipher_code = textToCode();
    //deciphered_code = decipher(cipher_code, d, n);
    original_message = orgPlaintext(deciphered_code);

    ofstream outFile("plaintext-recieved.txt");
    for (int i = 0; i < original_message.size(); i++) {
        outFile << original_message[i];
    }
    outFile.close();
    */

    return 0;
}

//function to get prime numbers from file. 
vector<BigUnsigned> getPrime() {
    vector<BigUnsigned> primes;
    ifstream inputFile("primes-sorted.dat");
    if (inputFile.is_open() == false) {
        printf("file primes-sorted.dat failed to open");
		printf("please run prime generation");
        return primes;
    }
    
	cout << "loading primes from file" << endl;
	string c; // for getting a prime in file

    while(primes.size() < 20){  //testing memory problems.
	//while (inputFile) {
		inputFile >> c;
		primes.push_back(stringToBigUnsigned(c));
		//cout << "prime : " << primes.size() << " [ " << primes[0] << endl;
    }
	
	cout << "loaded [ " << primes.size() << " ] primes from file" << endl;
    inputFile.close();
    return primes;
}

//function to select prime numbers for p,q,e
RSAConxtext select_P_Q_E(vector<BigUnsigned> prime_numbers) {
    RSAConxtext PQE;
    int x =1, y =4 , z = 13;//element holders
    int flag = 0;
    
    cout << "RSA primes using default locations at index 1 and 4 " <<endl;
    cout << "RSA using e = 13" << endl;

    /*
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

    PQE.setP(prime_numbers[x]);
    PQE.setQ(prime_numbers[y]);
    PQE.sete(prime_numbers[z]);
    return PQE;
}

//Function to store the input file into a vector of strings
vector<char> getMessageFromFile(){

    vector<char> plaintext;
    ifstream inputFile("plaintext-sent.txt");

    char c;
    while(inputFile.get(c)){
        plaintext.push_back(c);
    }
  
    inputFile.close();
    cout << "File read [ " << plaintext.size() << " ] characters" << endl;
    

    //Iterate through the plaintext vector to calculate and place in padding
    while(plaintext.size() % PLAINTEXTREADSIZE != 0){
        plaintext.push_back(' ');
    }

    
    cout << "padding to [ " << plaintext.size() << " ] characters" << endl;
    //cout << "char size [ " << plaintext.size() <<endl;
    //cout << to_string(plaintext) << endl;
    //cout << "======================end file read=============" << endl;

    return plaintext;
}



//function to turn enciphered code to ascii
void codeToText(vector<BigUnsigned> ciphercode) {
    vector<char> ciphertext;
    for (int i = 0; i < ciphercode.size(); i++) {
        string temp = bigUnsignedToString(ciphercode[i]);
        if ((temp.size() % 2) != 0) {
            temp.insert(0, 1, ' ');
        }
        int k = 1;
        int j = 0;
        while (k < temp.size()) {
            string bucket = "";
            bucket.insert(0,1,temp[j]);
            bucket = bucket + temp[k];
            ciphertext.push_back(char(stoi(bucket))+ ' ');
            j = j+2;
            k = k+2;
        }
        ciphertext.push_back('\n');
    }

    ofstream outFile("CipherText.txt");
    for (int i = 0; i < ciphertext.size(); i++) {
        outFile << ciphertext[i];
    }
    outFile.close();
}

////////////////////////////////////////////////////////
//function to read and convert plaintext into ciphertext
void encryptMessage(vector<char> plainText, RSAConxtext RSAinfo) {
    int blocksNeeded = (plainText.size() / PLAINTEXTREADSIZE);//integer division to figure out how many blocks
    if( plainText.size() % PLAINTEXTREADSIZE != 0){
        cout << "encryptMessage, plaintext isn't padded correctly, throwing error " << endl;
        throw "encryptMessage, plaintext isn't padded correctly, throwing error ";
    }

    //vector<char> block;
    ofstream outFile("CipherText.txt");//destination of the plaintextblocks
    string block[blocksNeeded];//trying to use a vector of chars, gives memory allocation errors because of vector's internals
    outFile << blocksNeeded << endl;
    
    for (int i = 0; i < blocksNeeded; i++)
    {
        // copy block size into new vector

        for (int j = 0; j < PLAINTEXTREADSIZE; j++)
        {
            //block.push_back((char)plainText.at((i * PLAINTEXTREADSIZE) + j));
            block[i].append(1,plainText.at((i * PLAINTEXTREADSIZE) + j));
        }

        // this was causing errors because of vector memory allocation
        // plainTextBlock.clear();// = vector<char>();//empty the last block
        // plainTextBlock = getBlockAt(plainText,i,PLAINTEXTREADSIZE);


        BigUnsigned base10 = convertCharsToBigUnsigned(block[i]);
        base10 = RSAinfo.encryptBlock(base10);

        string cipherBlock = convertBigUnsignedToBase256(base10);
        while (cipherBlock.size()%BLOCKWRITESIZE != 0)
            cipherBlock.append(1,' ');
        outFile << cipherBlock;

    }

    outFile.close();
    return;
}


/// @brief Look at CipherText.txt and compare decryption results to {paramater} original text
/// @param originalText what the decryption should be
/// @param RSAinfo modulus and keys for RSA
void decryptMessage(vector<char> originalText, RSAConxtext RSAinfo) {
    ifstream inFile("Ciphertext.txt",ios::binary);
    cout << "opening file..." << endl;
    string cipherMessage;

    int cipherBlockAmount;
    inFile >> cipherBlockAmount;//what about the newline characters?
    

    //cout << "debug : peek [ " << inFile.peek() << endl;
    //cout << "debug : '\\n' [ " << (int) '\n' << endl;
    inFile.get();//don't leave newline char hanging
    cout << "debug : peek [ " << inFile.peek() << endl;

    for(int i=0; i < cipherBlockAmount * BLOCKWRITESIZE-12; i ++){
    //for(int i=0; i < cipherBlockAmount * BLOCKWRITESIZE; i ++){
        cipherMessage.append(1,inFile.get());
        
        /*if(cipherMessage.back() == char_traits<char>::eof())
        {
            cout << "decryption hit end of file too early!"<< endl;
            //inFile.
        }*/
    }
    

    cout << "reading ciphermessage size [ " << cipherMessage.size() << endl;
    cout << "reading cipher blocks [ " << cipherBlockAmount << endl;


    string cipherBlock;
    for(int i =0; i < cipherBlockAmount; i++){
        cipherBlock.clear();
        for(int j =0; j< BLOCKWRITESIZE; j++){
            cipherBlock.append(1,cipherMessage[(i*BLOCKWRITESIZE)+j]);
        }
        BigUnsigned BlockBase10 = convertCharsToBigUnsigned(cipherBlock);
        BlockBase10 = RSAinfo.decryptBlock(BlockBase10);
        cout << "================== decrypted block " << endl;

        string cipherReveresed;
        for (int i=cipherBlock.size()-1; i>=0; i--){
            cipherReveresed.append(1, cipherBlock[i]);
        }
        BlockBase10 = convertCharsToBigUnsigned(cipherReveresed);
        BlockBase10 = RSAinfo.decryptBlock(BlockBase10);
        cout << "================== reverse decrypted block " << endl;

        cout << convertBigUnsignedToBase256(BlockBase10);

    }


}




//Function to decipher the ciphercode 
vector<BigUnsigned> decipher(vector<BigUnsigned> code, BigUnsigned d, BigUnsigned n){
    vector<BigUnsigned> deciphered_code;
    
    for(int i = 0; i < code.size(); i++){
        BigUnsigned val = modexp(code[i], d, n);
        deciphered_code.push_back(val);
    }
    
    return deciphered_code;
}

//Function to convert deciphered code back to plaintext
vector<char> orgPlaintext(vector<BigUnsigned> deciphered_code) {
    vector<char> org_plaintext;
    for (int i = 0; i < deciphered_code.size(); i++) {
        BigUnsigned remainder = deciphered_code[i];
        BigUnsigned quotient;
        string letter;
        int number_represent;
        char character;

        remainder.divideWithRemainder((94*94), quotient);
        letter = bigUnsignedToString(quotient + int(' '));
        number_represent = stoi(letter);
        character = char(number_represent);
        org_plaintext.push_back(character);
        remainder.divideWithRemainder((94), quotient);
        letter = bigUnsignedToString(quotient + int(' '));
        number_represent = stoi(letter);
        character = char(number_represent);
        org_plaintext.push_back(character);
        letter = bigUnsignedToString(remainder + int(' '));
        number_represent = stoi(letter);
        character = char(number_represent);
        org_plaintext.push_back(character);
        
        
    }
    return org_plaintext;
}


//helper funcion to convert string of chars (which are base 256) to BigUnsigned
BigUnsigned convertCharsToBigUnsigned(string input)
{
	BigUnsigned multiple = 1;       //the answer to digit_i = 256^i
    BigUnsigned tally = 0;          //total of intermediate addition

    for(int i= 0; i < input.size(); i++){
        tally += multiple * (unsigned) input.at(i); 
        multiple *= 256;
    }

    return tally;
	
}

//helper funcion to convert number to chars (which are base 256) 
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

//
void testPrimeLengthTiming(vector<char> message, vector<BigUnsigned>primes){
    int testRounds = 10;
    int seed = 665;
    srand(seed);
    string msg;
    const string filename= "testPrimeLengthTiming.csv";
    ofstream outFile(filename);
    RSAConxtext RSA;
    BigUnsigned msgBase10;
    Timer clock;

    cout << "testing prime length timing "<< endl;
    cout << "doing [ " << testRounds << " ] rounds" << endl;
    cout << "using random seed [ " << seed << endl;
    for(int i =0; i< PLAINTEXTREADSIZE; i++)
        msg.append(1,message[i]);
    cout << "message to encrypt [ " << msg << endl;
    

    msgBase10 = convertCharsToBigUnsigned(msg);
    
    try{
    int index = rand()%primes.size();
    RSA.setP(primes.at(index));
    
    while(!RSA.setQ(primes.at(rand()%primes.size())))
        ;//nop to keep drawing
    
    BigUnsigned e;
    
    do {
        cout << "setting e" << endl;
        e = (unsigned)rand();
        //e %= RSA.getphin() | 3;
        }
    while(!RSA.sete(e));
    }catch(const char* c) {cout << c << endl;}

    //RSA.printContext();

    outFile << "prime one length (dec digits), prime two length (dec digits), encryption time (ms), decryption time (ms)" << endl;

    double encryptionTime;
    double decryptionTime;
    BigUnsigned preLoadedCipher;
    for(int i =0; i< testRounds; i++){
        preLoadedCipher = RSA.encryptBlock(msgBase10);//we want to remove memory allocation time from the test
        cout <<"test round [ " << i;
        clock.reset();
        RSA.encryptBlock(msgBase10);
        encryptionTime=clock.elapsed();
        
        clock.reset();
        RSA.decryptBlock(preLoadedCipher);
        decryptionTime = clock.elapsed();

        outFile << std::string(BigUnsignedInABase(RSA.getP(),10)) <<',' <<std::string(BigUnsignedInABase(RSA.getQ(),10)) <<',' << encryptionTime <<',' << decryptionTime << endl;

    }
    outFile.close();

    cout << "testing done " << endl;
    cout << "results located in " << filename << endl;

    return;

}


/* vectors have strange copy constructor functionality. The 2nd loop throws a bad alloc.
// returns the block at i, seperated by blockSize
vector<char> getBlockAt(vector<char> text, int index, int blockSize)
{
    if(text.size() < (index+1) *blockSize){
        cout << "getBlockAt is trying to go out of bounds, throwing error " << endl;
        throw "getBlockAt is trying to go out of bounds, throwing error ";
    }
    
    vector<char> block;


    cout << "plain block [ " << endl << endl << to_string(block) << endl << endl;
    cout << "////////////////////////////////////////////////////////////////////////////" << endl;
}; 
*/
