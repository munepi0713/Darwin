#ifndef GARNET__MD5__H__
#define GARNET__MD5__H__

#include <cmath>


#ifdef _MSC_VER
typedef unsigned __int64 uint64_t;
typedef unsigned __int32 uint32_t;
#else
#include <stdint.h>
#endif


//r specifies the per-round shift amounts
static const unsigned int  MD5_r[64] = {
		7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
		5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
		4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
		6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

static const unsigned int  MD5_k[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,

    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 
    
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

struct MD5Hash
{
	union {
		uint64_t  u64[2]; // lo -> hi
		uint32_t  u32[4]; // lo -> hi
	};

	bool operator < (const MD5Hash& a)
	{
		return (this->u64[1] < a.u64[1]) || ((this->u64[1] == a.u64[1]) && (this->u64[0] < a.u64[0]));
	}
	bool operator == (const MD5Hash& a)
	{
		return (this->u64[1] == a.u64[1]) && (this->u64[0] == a.u64[0]);
	}
};

// 
class MD5
{
public :
	MD5()
	{
		//Use binary integer part of the sines of integers (Radians) as constants:
        /*
		for (int i = 0; i < 64; i++)
		{
			_k[i] = (int)(fabs(sin(i + 1.0)) * pow(2.0, 32.0));
			fprintf(stderr, "0x%08x ", _k[i]);
		}
        */
        _r = MD5_r;
        _k = MD5_k;

		init();
	}
	void  init()
	{
		_length = 0;
		_h[0] = 0x67452301;
		_h[1] = 0xEFCDAB89;
		_h[2] = 0x98BADCFE;
		_h[3] = 0x10325476;
	}
	void  write(const unsigned char* data, size_t size)
	{
		for (int i = 0; i < (int)size; i++)
		{
			_buffer[_length & 0x3f] = data[i];
			_length++;
			if ( (_length & 0x3f) == 0 ) update();
		}
	}
	void  finalize()
	{
		unsigned int lenlo = _length << 3;
		unsigned int lenhi = _length >> 29;
		const unsigned int one  = 0x80;
		const unsigned int zero = 0;
		write((unsigned char*)&one, 1);
		while ( (_length & 0x3f) != 56 )
		{
			write((unsigned char*)&zero, 1);
		}
		write((unsigned char*)&lenlo, 4);
		write((unsigned char*)&lenhi, 4);
	}
	const char* get()
	{
		const unsigned char* h = (const unsigned char*)_h;
		sprintf(_md5string,
				"%02x%02x%02x%02x%02x%02x%02x%02x"
				"%02x%02x%02x%02x%02x%02x%02x%02x",
				(int)h[ 0], (int)h[ 1], (int)h[ 2], (int)h[ 3],
				(int)h[ 4], (int)h[ 5], (int)h[ 6], (int)h[ 7],
				(int)h[ 8], (int)h[ 9], (int)h[10], (int)h[11],
				(int)h[12], (int)h[13], (int)h[14], (int)h[15]);

		return _md5string;
	}
	MD5Hash  getInt()
	{
		MD5Hash  ret;
		ret.u32[0] = _h[0];
		ret.u32[1] = _h[1];
		ret.u32[2] = _h[2];
		ret.u32[3] = _h[3];
		return ret;
	}

private :
	char          _md5string[33];
	unsigned char _buffer[64];
	unsigned int         _length;
	unsigned int         _h[4];
	//static const unsigned int  _r[64];
	//static const unsigned int  _k[64];
	const unsigned int* _r;
	const unsigned int* _k;

	void  update()
	{
		unsigned int* w = (unsigned int*)_buffer;

		//Initialize hash value for this chunk:
		unsigned int a = _h[0];
		unsigned int b = _h[1];
		unsigned int c = _h[2];
		unsigned int d = _h[3];

		//Main loop:
		unsigned int f, g, temp;
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define ROTATE()\
		temp = d;\
		d    = c;\
		c    = b;\
		b   += ROTATE_LEFT(a + f + _k[i] + w[g], _r[i]);\
		a    = temp
		for (int i = 0; i < 16; i++)
		{
			f = (b & c) | (~b & d);
			g = i;
			ROTATE();
		}
		for (int i = 16; i < 32; i++)
		{
			f = (d & b) | (~d & c);
			g = (5 * i + 1) & 0xf;
			ROTATE();
		}
		for (int i = 32; i < 48; i++)
		{
			f = b ^ c ^ d;
			g = (3 * i + 5) & 0xf;
			ROTATE();
		}
		for (int i = 48; i < 64; i++)
		{
			f = c ^ (b | ~d);
			g = (7 * i) & 0xf;
			ROTATE();
		}

		//Add this chunk's hash to result so far:
		_h[0] += a;
		_h[1] += b;
		_h[2] += c;
		_h[3] += d;
	}
};

#if 0
//r specifies the per-round shift amounts
const unsigned int  MD5::_r[64] = {
		7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
		5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
		4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
		6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

const unsigned int  MD5::_k[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,

    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 
    
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};
#endif

#endif//GARNET__MD5__H__
