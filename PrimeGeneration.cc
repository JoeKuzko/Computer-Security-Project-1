// Sample program demonstrating the use of the Big Integer Library.

// Standard libraries
#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <fstream>

// `BigIntegerLibrary.hh' includes all of the library headers.
#include "BigIntegerLibrary.hh"

using namespace std;
static int candidateCounter =0; // how many random numbers the bignumber generator has made

//raises x to a power and returns results. Input must be greater than 0.
BigInteger powerBigInt(BigInteger x, int power){
	if (power <=0){
		throw "this helper functions need more fuctionality!, sincerly powerBigInt function";
	}
	BigInteger result = x;
	for (int i =1; i < power; i ++)
	{
		result *= x;
	}

	return result;
}

/* miller-roban is a test for compositness of a number. Is probably prime does this test with a base 2. It's likely the numbers are prime, but not definitally
*/
bool isProbablyPrime(BigInteger x){
	//the test does not work with numbers under 3
	if(x.getMagnitude() < BigUnsigned(3))
	{
		cout << "isProbablyPrime testing must be bigger than 3";
		throw"isProbablyPrime testing must be bigger than 3";
	}
	//the number must be positive
	if(x.getSign() != BigInteger::Sign::positive){
		cout << "isProbablyPrime function doesn't currently support negatives!";
		throw "isProbablyPrime function doesn't currently support negatives!";
	}
		

	/////////////////////////////////////////
	//step 1, n-1 = 2^k * m such that n is the number tested, k is computed and m is based from k
	BigUnsigned n(x.getMagnitude()),    //this is the number we want to test for primeness
				k(1),    //largest whole number exponent
				m;    //calculated based on k
	BigUnsigned 	stepOneDivisor(2); // this how many factors of multiple we removed from (n-1)
	
	//loop to pull out factors
	while((n-1) % stepOneDivisor == 0){
		stepOneDivisor *= 2;
		k+=1;
	}

	//readd in the last "extra factor"
	stepOneDivisor /= 2;
	k -= 1; // we need to subtract 1 because current division results in a non whole number
			//To-Do, there might be a bug, if n = 4, ((n-1) % 2 ==1, but then k-=1 sets k to 0...

	//calculate m
	m = (n-1) / stepOneDivisor;
		
	///////////////////////////////////////
	//step 2;
	//test b0 = a^m mod n,   for some aribitary a
	//if b0 == 1 or -1 mod n, then it's likely prime
	//
	//if (bi = (bi-1) ^a mod n) == 1, then composite. If bi == -1, then likely prime.
	//that is for b2, b2 = b1^2 mod n. 
	//		if b2 is 1 then composite
	//		if b2 is -1 then likely prime
	//		repeat this test for how many factors we pulled out of (n-1)
	BigInteger a = 2;	//choose a: 1<a<n-1// this inital test uses a power of 2
	BigInteger b;		//
	
	// base test
	//FYI b is unsigned, so b will never be negative
	//because it's unsigned, b mod n == -1 will alway be false, so compare b == n-1
	b = modexp(a, m, n);
	if(b == 1 || b == n-1){
		//cout << "prime found! [ " << n << endl;
		return true;
	}
	
	//find b_i such that b_i = B_i-1 ^ 2 mod n
	//repeat k time. k is from (2^k) above
	for(BigUnsigned i = 1; i < k && (b != 1 || b != n-1); i++)
	{
		b = modexp(b, 2, n);
	}

	if (b == n-1)
		return true;//it's prime
	else
		return false;
}

/* This does the miller robin test (for compositness), but looks at the first 168 primes. if it fails any time, it's not actually a prime.
    technically the testing does not garantee prime, but will serve for the projects purposes
	https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test#Testing_against_small_sets_of_bases talks about proof of primeness with miller robin
*/
bool isDefinitalyPrime(BigUnsigned n){
	if(n < BigUnsigned(3))
		throw"isProbablyPrime testing must be bigger than 3";

	/////////////////////////////////////////
	//step 1, n-1 = 2^k * m
	BigUnsigned k(1),    //largest whole number exponent
				m;    //calculated based on k
	

	//we should test against primes under the ln(n). The ln(2^665) is less than 461. this represent testing against ln of 10^200
	const int knownPrimes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67,
						 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 
						 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 
						 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 
						 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 
						 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 
						 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 
						 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 
						 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 
						 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 
						 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 
						 947, 953, 967, 971, 977, 983, 991, 997};//this is a list of known primes from wolfram alpha
	const int knownPrimesSize = 168;
	
	BigUnsigned 	stepOneDivisor(2); // this is our divisor for step 1, // a^k, such that a is known prime
	bool isComposite = false;

	//now we test the input number against our list of known primes
	for (int j = 0; j < knownPrimesSize && !isComposite; j++)
	{
		//reset variables
		stepOneDivisor=knownPrimes[j];
		k=1;

		//pull out the factors of the known prime in (n-1)
		while ((n - 1) % stepOneDivisor == 0)
		{
			// cout<< "divsor : " << stepOneDivisor << endl;
			stepOneDivisor *= knownPrimes[j];
			k += 1;
		}


		stepOneDivisor /= knownPrimes[j];//undo the "extra" multiple we pulled out.
		k -= 1; // we need to subtract 1 because current division results in a non whole number
		

		// calculate m
		m = (n - 1) / stepOneDivisor;

		///////////////////////////////////////
		// step 2;
		// b0 = a^m mod n,   (bi = bi-1) ^2
		//
		//test b0 = a^m mod n,   for some aribitary a
		//if b0 == 1 or -1 mod n, then it's likely prime
		//tests after this are for bi
		//if (bi = (bi-1) ^a mod n) == 1, then composite. If bi == -1, then likely prime.
		//that is for b2, b2 = b1^2 mod n. 
		//		if b2 is 1 then composite
		//		if b2 is -1 then likely prime
		//		repeat this test for how many factors we pulled out of (n-1)
		BigInteger a = knownPrimes[j]; // choose a: 1<a<n-1
		BigInteger b;	  //

		// base test
		// FYI b is unsigned, so b will never be negative
		// b mod n == -1 will alway be false, so compare b == n-1
		b = modexp(a, m, n);
		if (b == 1 || b == n - 1)
		{
			//we can stop the loop, but the rest of known primes need testing .
			continue;
		}

		// find b_i such that b_i = B_i-1 ^ a mod n
		// a is from the known primelist, but could be an arbitrary number
		// repeat k time. k is from (2^k) above
		for (BigUnsigned i = 1; i < k && (b != 1 || b != n - 1); i++)
		{
			b = modexp(b, 2, n);
		}

		if (b == n - 1)
			continue;
		if(b == 1)
			isComposite = true;
	}

	return !isComposite;//return the flag, but remember that it looks for compositness.
	
}

BigUnsigned genRandomPrimeCandidate(){
	candidateCounter++;//this counter is personal use
	//2^333 is  10^100 -- 101 decimal digits
	//2^665 is ~10^200
	BigUnsigned x = 1;
	int r = (rand()%333)+333;// this is between 333 and 665
	//int r = (rand()%20) +20; // a much smaller shift to test with.
	
	x = x << r;//shift x to enlarge it.
	x += rand();
	x = x | 3;//ensure x is odd and above 3

	return x;
}

/*sort the vector supplied*/
vector<BigUnsigned> sortPrimes(vector<BigUnsigned> toSort){
		
		//sort vector of primes
		bool hasChanged = false;
		BigUnsigned tempHolder; //used to sawp
		do{
			//simple bubble sort because it's fast to write out. and fast enough with 1k primes to sort
			hasChanged = false;
			for (size_t i = 0; i < toSort.size()-1; i++)
			{
				if(toSort.at(i)>toSort.at(i+1)){
					tempHolder = toSort.at(i+1);
					toSort[i+1] = toSort[i];
					toSort[i] = tempHolder;
					hasChanged= true;
				}
			}
			
		}while (hasChanged);

		return toSort;

}

int main() {
	vector<BigUnsigned> primes;//an array to store our found primes;
	int primesToFind = 1000;//how many prime numbers the program should find.
	ofstream MyFile("primes-unsorted.dat");//file write to store found information

	srand(665);
	cout << "seed: 665\n";
	cout << "hello world. Let's make some primes\n";
	cout << "primesToFind : [ " << primesToFind << " ]\n";
	
	//this will loop till [primesToFind] number of miller-rabin tested primes are in the list
	while(primes.size()<primesToFind){		
		BigUnsigned candidate = genRandomPrimeCandidate();
		if (isProbablyPrime(candidate)){
			//if something passes the probably prime test, we can input it to further test primeness
			if(!isDefinitalyPrime(candidate))
				cout << "found fake prime [ " << candidate << endl;
			else{
				//add the number to out list and file of primes
				MyFile <<setw(202) << candidate << endl;
				cout << "prime" <<setw(3)<<primes.size() <<":[ " << setw(202) << candidate << " ]\n";
				primes.push_back(candidate);
			}
		}
	}

		MyFile.close();

		/*write another file of sorted primes*/
		MyFile.open("primes-sorted.dat");
		primes = sortPrimes(primes);

		//write our now sorted primes.
		for (size_t i = 0; i < primes.size() - 1; i++){
			MyFile << setw(202) << primes.at(i) << endl;
		}

	cout << "searched [ " << candidateCounter << " ] candidates\n";

	return 0;
}