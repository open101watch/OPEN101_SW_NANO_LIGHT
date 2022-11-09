// cryptography.h
#ifndef CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_H

// #include <iostream>
#include <math.h>
// using namespace std;

// class CRYPTOGRAPHY
// {

//     public:
//       // char *gen_MD5_hash(byte data);
//       // char *gen_SHA256_hash(byte data);
//       unsigned int bin2hex(char *digits);
//       long dec2bin(long n);
//       int bin2dec(long long n);

// };



/////////////////////////////////////////////////


class CRYPTOGRAPHY{
  public:
    // char *gen_MD5_hash(byte data){
    //   // Output char length => 32


    // }

    // char *gen_SHA256_hash(byte data){
    //   // Output char length => 64


    // }

    unsigned int bin2hex(char *digits){
      unsigned int res=0;
      while(*digits)
        res = (res<<1)|(*digits++ -'0');
      return res;
    }


    long dec2bin(long n) {
        int remainder; 
        long binary = 0, i = 1;
       
        while(n != 0) {
            remainder = n%2;
            n = n/2;
            binary= binary + (remainder*i);
            i = i*10;
        }
        return binary;
    }

    int bin2dec(long long n) {
      int dec = 0, i = 0, rem;

      while (n!=0) {
        rem = n % 10;
        n /= 10;
        dec += rem * pow(2, i);
        ++i;
      }

      return dec;
    }
};

#endif