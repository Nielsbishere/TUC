#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace osomi {
	class BinaryHelper {
	private:
		template<class D, class I> static I toBinary(D f, unsigned int e, unsigned int m) {
			I value = f < 0 ? (I)1 << (e+m) : 0;
			f = f < 0 ? -f : f;
			D l = (D)(log2(f));
			l = l < 0 ? floor(l) - 1 : floor(l);
			value |= (I)(l + (((I)1 << (e - 1)) - 1)) << m;
			value |= (I)((f - pow(2, l)) *	pow(2, m - l));
			return value;
		}
		static inline char getChar(char c) {
			if (c < 10)return '0' + c;
			return 'A' + c - 10;

		}
		template<class I> static string toHex(I i) {
			string s(sizeof(i)*2, '0');
			I j=0;
			while (i > 0) {
				s[s.size()-1-j] = getChar(i % 16);
				i /= 16;
				j++;
			}
			return s;
		}
	public:
		static unsigned int asBinary(float f) {
			unsigned int floatExponent = 8, floatMantissa = 32 - 1 - floatExponent;
			return toBinary<float,unsigned int>(f, floatExponent, floatMantissa);
		}
		static unsigned long long asBinary(double d) {
			unsigned int doubleExponent = 11, doubleMantissa = 64 - 1 - doubleExponent;
			return toBinary<double,unsigned long long>(d, doubleExponent, doubleMantissa);
		}
		static string asHex(unsigned int i) {
			return toHex<unsigned int>(i);
		}
		static string asHex(unsigned long long l) {
			return toHex<unsigned long long> (l);
		}
		static unsigned int readUInt(ifstream &input, unsigned int end, unsigned int length){
			if (length > 4) {
				printf("Tried to write an unsigned int with a length of > 4\n");
				return 0;
			}
			if (length > end) {
				printf("Tried to get an element with end < length\n");
				return 0;
			}
			input.seekg(input.end);
			unsigned int l = (unsigned int)input.tellg();
			if (l < end + length) {
				printf("File out of bounds\n");
				return 0;
			}
			input.seekg(input.beg + end - length);
			char *buffer = new char[length];
			input.read(buffer, length);
			unsigned int i = 0;
			for (unsigned int k = 0; k < length; k++)
				i |= buffer[0] << (length - 1 - k);
			delete[] buffer;
			return i;
		}
		static void writeUInt(ofstream &output, unsigned int length, unsigned int j) {
			if (length > 4) {
				printf("Tried to write an unsigned int with a length of > 4\n");
				return;
			}
			char *buf = new char[length];
			for (int i = (int)length - 1; i >= 0; i++) {
				unsigned int b = (length - i) * 8;
				buf[i] = (j & (0xFF << b)) >> b;
			}
			output.write(buf, length);
			delete[] buf;
		}
	};
}