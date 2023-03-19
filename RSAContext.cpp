#include "BigIntegerLibrary.hh"
#include <iostream>
#include <iomanip>

using namespace std;



//a structure of data and functions to work on it.
class RSAConxtext{
	private: BigUnsigned p;//a large prime p
	private: BigUnsigned q;//a large prime q
	private: BigUnsigned e;//the encryption key
	private: BigUnsigned n; // modulus of RSA, n = p*q;
	private: BigUnsigned phi_n; //(p - 1)* (q - 1); // phi n
	private: bool hasChanged = true;
	private: BigUnsigned d; //decryption key calculated from e,  = modinv(e, phi_n);
	
	private: void recalculate(){
		n = p * q;
		phi_n = (p - 1)* (q - 1);
		//to do, error check. 
		//de mod phi(n) = 1 may not exist. This happens when phi(n) is not relativly prime to d*e
		try{
		d = modinv(e, phi_n);
		} catch(const char* c){
			cout << "error with d in RSA context, [ " << c << endl;
		}
		hasChanged = false;
	};
	
	public: bool setP(BigUnsigned primeP){
		//error checking
		if(primeP == q){
			//cout << "RSAConxtext.setP primes are the same: throwing error" << endl;
			//throw "RSAConxtext.setP primes are the same: aborting program";
			return false;
		}
		p = primeP;
		hasChanged = true;
		
		return true; //the value has changed.
	}
	public: BigUnsigned getP(){
		return p;
	}
	
	public: bool setQ(BigUnsigned primeQ){
		//error checking
		if(p == primeQ){
			//cout << "RSAConxtext.setQ primes are the same: throwing error" << endl;
			//throw "RSAConxtext.setQ primes are the same: aborting program";
			return false;
		}
		q = primeQ;
		hasChanged = true;
		
		return true; //the value has changed.
	}
	public: BigUnsigned getQ(){
		return q;
	}
	
	public: bool sete(BigUnsigned encryptionKey){
		
		phi_n = (p - 1)* (q - 1);

		if(encryptionKey > phi_n || gcd(phi_n, encryptionKey) != 1){
			return false;
		}

		try{
			d = modinv(encryptionKey, phi_n);
		} catch(const char* c){
			cout << "error with d in RSA context, [ " << c << endl;
			return false;
		}

		e = encryptionKey;
		hasChanged = true;
		

		//to-do error check, not all e are valid in RSA or even all Ps and Qs
		return true;
	}
	public: BigUnsigned gete(){
		return e;
	}
	public: BigUnsigned getd(){
		if(hasChanged)
			recalculate();
		
		return d;
	}
	
	public: BigUnsigned getn(){
		if(hasChanged)
			recalculate();
		return n;
	}
	public: BigUnsigned getphin(){
		if(hasChanged)
			recalculate();
		return phi_n;
	}
	
	public: void printContext(){
		if(hasChanged)
			recalculate();
		cout << endl;
		cout << "var   p: [ " << setw(203) << p << endl;
		cout << "var   q: [ " << setw(203) << q << endl;
		cout << "var   n: [ " << setw(203) << n<< endl;
		cout << "v phi n: [ " << setw(203) <<  phi_n<< endl;
		cout << "var   e: [ " << setw(203) << e << endl;
		cout << "var   d: [ " << setw(203) << d<< endl;
		//cout << "var    : [ " << setw(203) <<  endl;
		cout << endl;
	}

	public: BigUnsigned encryptBlock(BigUnsigned plainTextBlock){
		if(hasChanged)
			recalculate();
		return modexp(plainTextBlock, p, n);
	}

	public: BigUnsigned decryptBlock(BigUnsigned cipherTextBlock){
		if(hasChanged)
			recalculate();
		return modexp(cipherTextBlock, q, n);
	}

	




};