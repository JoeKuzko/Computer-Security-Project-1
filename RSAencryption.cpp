//This program computes the trigraph code for a message using the RSA algorithm.
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstddef>
#include <math.h>
#include <sstream>
#include <time.h>
#include "BigIntegerLibrary.hh"

using namespace std;


//function prototypes

vector<BigUnsigned> getPrime(); // gets prime numbers from generated prime file
vector<BigUnsigned> select_P_Q_E(vector<BigUnsigned>);// will return vector of primes in this specific order
vector<char>        storeMessage(vector<char>); //Reads plaintext file and puts all characters into vector
vector<BigUnsigned> getTrigraphCode(char[], vector<BigUnsigned>); //reads every 3 chars and converts to trigraph code
vector<BigUnsigned> encipher(vector<BigUnsigned>, BigUnsigned, BigUnsigned);// enciphers trigraph code
void                codeToText(vector<BigUnsigned>); // takes the enciphered code and splits it up into ascii and creates file
vector<BigUnsigned> textToCode(); // reads file created with ciphertext and converts back to cipher code
vector<BigUnsigned> decipher(vector<BigUnsigned>, BigUnsigned, BigUnsigned); // takes the cipher code and deciphers it back to the trigraph code
vector<char>        orgPlaintext(vector<BigUnsigned>); // converts deciphered code into the orginal plaintext


//Main program method
int main(){
    vector<BigUnsigned>    primes;
    vector<BigUnsigned>    PQE;
    vector<char>           plaintext;
    vector<char>           ciphertext;
    vector<BigUnsigned>    trigraph_code;
    vector<BigUnsigned>    enciphered_code;
    vector<BigUnsigned>    cipher_code;
    vector<BigUnsigned>    deciphered_code;
    vector<char>           original_message;

    primes = getPrime(); // all prime numbers from file have been recieved
    if (primes.empty()) {
        return 9999;
    }

    PQE = select_P_Q_E(primes); // prime numbers selected for p,q,e

    BigUnsigned p = PQE[0];
    BigUnsigned q = PQE[1];
    BigUnsigned n = p*q ; // modulus
    BigUnsigned totient = (p - 1)* (q - 1); // phi n
    BigUnsigned e = PQE[2]; // encryption key
    BigUnsigned d = modinv(e, totient); // decryption key

    plaintext = storeMessage(plaintext); // reads file for plaintext and stores in plaintext vector

    //Iterate through the plaintext vector to calculate and place in padding
    int size = plaintext.size();
    int numPadding = 0;
    if(size % 3 != 0){
        numPadding = 3 - (size % 3);
        int i = 0;
        while(i < numPadding){
            plaintext.push_back('\0');
            i++;
        }
    }

    //creating trigraph and recieving trigraph code
    //kept plan of splitting message into blocks of 3 chars each
    int A,B,C; //Place holders in vector
    A = 0, B = 1,C = 2; 
    int num = plaintext.size()/3;
    for(int i = 0; i < num; i++){
        char trigraph[] = {plaintext[A], plaintext[B], plaintext[C]};
        trigraph_code = getTrigraphCode(trigraph, trigraph_code);
        A = A + 3;
        B = B + 3;
        C = C + 3;
    }
    
    enciphered_code = encipher(trigraph_code, e, n);
    codeToText(enciphered_code);

    cipher_code = textToCode();
    deciphered_code = decipher(cipher_code, d, n);
    original_message = orgPlaintext(deciphered_code);

    ofstream outFile("plaintext-recieved.txt");
    for (int i = 0; i < original_message.size(); i++) {
        outFile << original_message[i];
    }
    outFile.close();
    
    return 0;
}

//function to get prime numbers from file. 
vector<BigUnsigned> getPrime() {
    vector<BigUnsigned> primes;
    ifstream inputFile("primes-sorted.dat");
    if (inputFile.is_open() == false) {
        printf("file primes-sorted.dat failed to open");
        return primes;
    }
    string c; // for getting line in file
    while (getline(inputFile, c)) {
        string final_num = "";
        for (int i = 0; i < c.size(); i++) {
            if (c[i] != ' ') {
                final_num = final_num + c[i];
            }
        }
        BigUnsigned prime = stringToBigUnsigned(final_num);
        primes.push_back(prime);
    }
    inputFile.close();
    return primes;
}

//function to select prime numbers for p,q,e
vector<BigUnsigned> select_P_Q_E(vector<BigUnsigned> prime_numbers) {
    vector<BigUnsigned> PQE;
    int x, y, z;//element holders
    int flag = 0;
    while (flag == 0) {
        printf("please input element number 0-998 for prime p \n");
        cin >> x;
        printf("please input element number 0-998 for prime q that is not equal to p: \n");
        cin >> y;
        printf("please input element number for e that is not equal to p or q \n");
        cin >> z;
        if (x > -1 && x < 999) {
            if (y > -1 && y < 999 && y != x) {
                if (z > -1 && z < 999 && z != x && z != y) {
                    flag = 1;
                }
            }
        }
        else {
            cout << "numbers for input do not follow parameters \n";
        }
    }
    PQE.push_back(prime_numbers[x]);
    PQE.push_back(prime_numbers[y]);
    PQE.push_back(prime_numbers[z]);
    return PQE;
}

//Function to store the input file into a vector of strings
vector<char> storeMessage(vector<char> trigraph){

    ifstream inputFile("plaintext-sent.txt");

    char c;
    while(inputFile.get(c)){
        trigraph.push_back(c);
    }
  
    inputFile.close();
    return trigraph;
}

//Function to convert plaintext trigraphs into trigraph code
vector <BigUnsigned> getTrigraphCode(char plaintext[3], vector<BigUnsigned> tricode){
    BigUnsigned num1, num2, num3, sum;
    if(plaintext[1] == '\0'){ //sets termination char to A which is 0
        plaintext[1] = ' ';
        plaintext[2] = ' ';
    }
    if(plaintext[2] == '\0'){
        plaintext[2] = ' ';
    }

    num1 = int(plaintext[0] - int(' '));
    num2 = int(plaintext[1] - int(' '));
    num3 = int(plaintext[2] - int(' '));
    sum = (num1 * 94 * 94) + (num2 * 94) + num3;

    tricode.push_back(sum);
    return tricode;
}

//Function to encipher the trigraph code
vector<BigUnsigned> encipher(vector<BigUnsigned> trigraph, BigUnsigned e, BigUnsigned n){
    vector<BigUnsigned> enciphered_code;

    for(int i = 0; i < trigraph.size(); i++){
        BigUnsigned val = modexp(trigraph[i], e, n);
        enciphered_code.push_back(val);
    }
    return enciphered_code;
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

//function to read and convert ciphertext into ciphercode
vector<BigUnsigned> textToCode() {
    vector<BigUnsigned> ciphercode;
    ifstream inFile("CipherText.txt");

    string text;
    while (getline(inFile, text)) {
        string temp = "";
        for (int i = 0; i < text.size(); i++) {
            string bucket = "";
            unsigned char num = text[i];
            int num1 = int(num) - ' ';
            if (num1 < 10) {
                if (num1 == 0) {
                    bucket = "00";
                }
                else {
                    bucket = to_string(0) + to_string(num1);
                }
            }
            else {
                bucket = to_string(num1);
            }
            temp = temp + bucket;
        }
        ciphercode.push_back(stringToBigUnsigned(temp));
    }

    inFile.close();
    return ciphercode;
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
