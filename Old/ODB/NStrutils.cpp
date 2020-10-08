
#include "NStrutils.h"

namespace nstd {

void mswap(char& a,char& b){
	char c(a); a=b;b=c;
}

/* A utility function to reverse a string  */
void mreverse(char* str, unsigned int length){
	unsigned int start = 0;
	unsigned int end = length -1;
	while (start < end){
		mswap(*(str+start), *(str+end));
		start++;
		end--;
	}
}

char* mitoa(long int num, char* str, int base){
	unsigned int i = 0;
	boolean isNegative = false;
	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0){
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}
	// In standard itoa(), negative numbers are handled only with 
	// base 10. Otherwise numbers are considered unsigned.
	if (num < 0 && base == 10){
		isNegative = true;
		num = -num;
	}
	// Process individual digits
	while (num != 0){
		long int rem = num % base;
		str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
		num = num/base;
	}
	// If number is negative, append '-'
	if (isNegative) str[i++] = '-';
	str[i] = '\0'; // Append string terminator
	// Reverse the string
	mreverse(str, i);
	return str;
}

char* mftoa(char* str,uint8_t maxlen,float val,uint8_t prec){
	unsigned long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
	char* ret=str;
	for(int i=0;i<maxlen-1;i++)str[i]=' ';
	str[maxlen]='\0';
	if (prec>9) prec=9;
	long heiltal = (long)val;
	mitoa(heiltal, str, 10);
	if (prec>0) {
		while (*str != '\0') str++;
		*str++ = '.';
		unsigned long desimal = abs((long)((val - heiltal) * p[prec]));
		mitoa(desimal, str, 10);
	}
	return ret;
}

}

