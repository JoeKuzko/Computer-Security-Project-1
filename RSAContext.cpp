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
	private: BigUnsigned totient; //(p - 1)* (q - 1); // phi n
	private: bool hasChanged = true;
	private: BigUnsigned d; //decryption key calculated from e,  = modinv(e, totient);
	
	private: void recalculate(){
		n = p * q;
		totient = (p - 1)* (q - 1);
		//to do, error check. 
		//de mod phi(n) = 1 may not exist. This happens when phi(n) is not relativly prime to d*e
		d = modinv(e, totient);
		
		hasChanged = false;
	};
	
	public: bool setP(BigUnsigned primeP){
		//error checking
		if(p==q){
			cout << "RSAConxtext.setP primes are the same: throwing error" << endl;
			throw "RSAConxtext.setP primes are the same: aborting program";
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
		if(p==q){
			cout << "RSAConxtext.setQ primes are the same: throwing error" << endl;
			throw "RSAConxtext.setQ primes are the same: aborting program";
		}
		q = primeQ;
		hasChanged = true;
		
		return true; //the value has changed.
	}
	public: BigUnsigned getQ(){
		return q;
	}
	
	public: bool sete(BigUnsigned encryptionKey){
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
		return totient;
	}
	
	public: void printContext(){
		if(hasChanged)
			recalculate();
		cout << endl;
		cout << "var   p: [ " << setw(203) << p << endl;
		cout << "var   q: [ " << setw(203) << q << endl;
		cout << "var   n: [ " << setw(203) << n<< endl;
		cout << "v phi n: [ " << setw(203) <<  totient<< endl;
		cout << "var   e: [ " << setw(203) << e << endl;
		cout << "var   d: [ " << setw(203) << d<< endl;
		cout << "var    : [ " << setw(203) <<  endl;
		cout << endl;
	
	}
};