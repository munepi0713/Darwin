
#include <limits>
#include <vector>
#include <map>

//#include <smmintrin.h>


static
__inline void Noise16(float* ret, int x, int y)
{
#if 0
	// ’x‚¢
	__m128i m_1, m_1376312589, m_7fffffff, m_15731, m_789221;
	m_1          = _mm_set1_epi32(1);
	m_1376312589 = _mm_set1_epi32(1376312589);
	m_7fffffff   = _mm_set1_epi32(0x7fffffff);
	m_15731      = _mm_set1_epi32(15731);
	m_789221     = _mm_set1_epi32(789221);

	__m128  m_1f, m_1073741824f;
	m_1f          = _mm_set1_ps(1.f);
	m_1073741824f = _mm_set1_ps(1073741824.f);

	/*
	__declspec(align(16)) int n[4];
	n[0] = (x - 1) + (y - 1) * 57;
	n[1] = (x - 1) + (y    ) * 57;
	n[2] = (x - 1) + (y + 1) * 57;
	n[3] = (x - 1) + (y + 2) * 57;
	*/

	__m128i m_a, m_n, m_t;
	//m_a = _mm_load_si128((__m128i*)&n);
	m_a.m128i_u32[0] = (x - 1) + (y - 1) * 57;
	m_a.m128i_u32[1] = (x - 1) + (y    ) * 57;
	m_a.m128i_u32[2] = (x - 1) + (y + 1) * 57;
	m_a.m128i_u32[3] = (x - 1) + (y + 2) * 57;

	__m128  m_nf;

	for (int i = 0; i < 4; i++)
	{
		m_n = _mm_xor_si128(_mm_slli_epi32(m_a, 13), m_a);

#if 1
		m_t = _mm_mullo_epi16(m_n, m_n);
		m_t = _mm_mullo_epi16(m_t, m_15731);
		m_t = _mm_add_epi32(m_t, m_789221);
		m_n = _mm_mullo_epi16(m_t, m_n);
#elif 0
		m_t.m128i_u32[0] = m_n.m128i_u32[0] * m_n.m128i_u32[0] * 15731;
		m_t.m128i_u32[1] = m_n.m128i_u32[1] * m_n.m128i_u32[1] * 15731;
		m_t.m128i_u32[2] = m_n.m128i_u32[2] * m_n.m128i_u32[2] * 15731;
		m_t.m128i_u32[3] = m_n.m128i_u32[3] * m_n.m128i_u32[3] * 15731;
		m_t = _mm_add_epi32(m_t, m_789221);
		m_n.m128i_u32[0] = m_t.m128i_u32[0] * m_n.m128i_u32[0];
		m_n.m128i_u32[1] = m_t.m128i_u32[1] * m_n.m128i_u32[1];
		m_n.m128i_u32[2] = m_t.m128i_u32[2] * m_n.m128i_u32[2];
		m_n.m128i_u32[3] = m_t.m128i_u32[3] * m_n.m128i_u32[3];
#else
		_mm_store_si128((__m128i*)&n, m_n);
		n[0] = n[0] * (n[0] * n[0] * 15731 + 789221);
		n[1] = n[1] * (n[1] * n[1] * 15731 + 789221);
		n[2] = n[2] * (n[2] * n[2] * 15731 + 789221);
		n[3] = n[3] * (n[3] * n[3] * 15731 + 789221);
		m_n = _mm_load_si128((__m128i*)&n);
#endif

		m_n = _mm_add_epi32(m_n, m_1376312589);
		m_n = _mm_and_si128(m_n, m_7fffffff);
		m_nf = _mm_cvtepi32_ps(m_n);
		m_nf = _mm_div_ps(m_nf, m_1073741824f);
		m_nf = _mm_sub_ps(m_1f, m_nf);
		_mm_storeu_ps(ret, m_nf);
		ret += 4;

		m_a = _mm_add_epi32(m_a, m_1);
	}
#elif 1
	int a[4], n[4];
	a[0] = (x - 1) + (y - 1) * 57;
	a[1] = (x - 1) + (y    ) * 57;
	a[2] = (x - 1) + (y + 1) * 57;
	a[3] = (x - 1) + (y + 2) * 57;

	for (int i = 0; i < 4; i++)
	{
		n[0] = (a[0]<<13) ^ a[0];
		n[1] = (a[1]<<13) ^ a[1];
		n[2] = (a[2]<<13) ^ a[2];
		n[3] = (a[3]<<13) ^ a[3];

		ret[0] = (float)( 1.0 - ( (n[0] * (n[0] * n[0] * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		ret[1] = (float)( 1.0 - ( (n[1] * (n[1] * n[1] * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		ret[2] = (float)( 1.0 - ( (n[2] * (n[2] * n[2] * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		ret[3] = (float)( 1.0 - ( (n[3] * (n[3] * n[3] * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

		a[0]++;
		a[1]++;
		a[2]++;
		a[3]++;

		ret += 4;
	}
#else
	int o, p, q, r;
	o = (x - 1) + (y - 1) * 57;
	p = (x - 1) + (y    ) * 57;
	q = (x - 1) + (y + 1) * 57;
	r = (x - 1) + (y + 2) * 57;
	for (int i = 0; i < 4; i++)
	{
		int n0, n1, n2, n3;
		n0 = (o<<13) ^ o;
		*ret++ = (float)( 1.0 - ( (n0 * (n0 * n0 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n1 = (p<<13) ^ p;
		*ret++ = (float)( 1.0 - ( (n1 * (n1 * n1 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n2 = (q<<13) ^ q;
		*ret++ = (float)( 1.0 - ( (n2 * (n2 * n2 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n3 = (r<<13) ^ r;
		*ret++ = (float)( 1.0 - ( (n3 * (n3 * n3 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

		o++;
		p++;
		q++;
		r++;
	}
#endif
}

class PerlinNoise
{
public :
	inline float Noise(int x, int y)
	{
		int n;
		n = x + y * 57;
		n = (n<<13) ^ n;
		return (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
	}
	inline void Noise16(float* ret, int x, int y)
	{
#if 1
		int o, p, q, r;
		o = (x - 1) + (y - 1) * 57;
		p = (x - 1) + (y    ) * 57;
		q = (x - 1) + (y + 1) * 57;
		r = (x - 1) + (y + 2) * 57;
		for (int i = 0; i < 4; i++)
		{
			int n0, n1, n2, n3;
			n0 = (o<<13) ^ o;
			*ret++ = (float)( 1.0 - ( (n0 * (n0 * n0 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
			n1 = (p<<13) ^ p;
			*ret++ = (float)( 1.0 - ( (n1 * (n1 * n1 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
			n2 = (q<<13) ^ q;
			*ret++ = (float)( 1.0 - ( (n2 * (n2 * n2 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
			n3 = (r<<13) ^ r;
			*ret++ = (float)( 1.0 - ( (n3 * (n3 * n3 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

			o++;
			p++;
			q++;
			r++;
		}
#elif 0
		int n, o, p, q, r;
		o = (x - 1) + (y - 1) * 57;
		n = (o<<13) ^ o;
		ret[ 0] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		p = o + 57;
		n = (p<<13) ^ p;
		ret[ 1] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		q = p + 57;
		n = (q<<13) ^ q;
		ret[ 2] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		r = q + 57;
		n = (r<<13) ^ r;
		ret[ 3] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

		o = o + 1;
		n = (o<<13) ^ o;
		ret[ 4] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		p = p + 1;
		n = (p<<13) ^ p;
		ret[ 5] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		q = q + 1;
		n = (q<<13) ^ q;
		ret[ 6] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		r = r + 1;
		n = (r<<13) ^ r;
		ret[ 7] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

		o = o + 1;
		n = (o<<13) ^ o;
		ret[ 8] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		p = p + 1;
		n = (p<<13) ^ p;
		ret[ 9] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		q = q + 1;
		n = (q<<13) ^ q;
		ret[10] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		r = r + 1;
		n = (r<<13) ^ r;
		ret[11] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

		o = o + 1;
		n = (o<<13) ^ o;
		ret[12] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		p = p + 1;
		n = (p<<13) ^ p;
		ret[13] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		q = q + 1;
		n = (q<<13) ^ q;
		ret[14] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		r = r + 1;
		n = (r<<13) ^ r;
		ret[15] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
#else
		int n;
		n = (x - 1) + (y - 1) * 57;
		n = (n<<13) ^ n;
		ret[ 0] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x - 1) + (y + 0) * 57;
		n = (n<<13) ^ n;
		ret[ 1] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x - 1) + (y + 1) * 57;
		n = (n<<13) ^ n;
		ret[ 2] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x - 1) + (y + 2) * 57;
		n = (n<<13) ^ n;
		ret[ 3] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

		n = (x + 0) + (y - 1) * 57;
		n = (n<<13) ^ n;
		ret[ 4] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x + 0) + (y + 0) * 57;
		n = (n<<13) ^ n;
		ret[ 5] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x + 0) + (y + 1) * 57;
		n = (n<<13) ^ n;
		ret[ 6] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x + 0) + (y + 2) * 57;
		n = (n<<13) ^ n;
		ret[ 7] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

		n = (x + 1) + (y - 1) * 57;
		n = (n<<13) ^ n;
		ret[ 8] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x + 1) + (y + 0) * 57;
		n = (n<<13) ^ n;
		ret[ 9] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x + 1) + (y + 1) * 57;
		n = (n<<13) ^ n;
		ret[10] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x + 1) + (y + 2) * 57;
		n = (n<<13) ^ n;
		ret[11] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

		n = (x + 2) + (y - 1) * 57;
		n = (n<<13) ^ n;
		ret[12] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x + 2) + (y + 0) * 57;
		n = (n<<13) ^ n;
		ret[13] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x + 2) + (y + 1) * 57;
		n = (n<<13) ^ n;
		ret[14] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		n = (x + 2) + (y + 2) * 57;
		n = (n<<13) ^ n;
		ret[15] = (float)( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
#endif
	}

	inline float Interpolate(float a, float b, float x)
	{
		float ft, f;
		ft = x * 3.1415927f;
		f = (1 - cos(ft)) * .5f;

		return  a*(1-f) + b*f;
	}
	inline float SmoothedNoise1(int x, int y)
	{
		float corners, sides, center;
		corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
		sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8;
		center  =  Noise(x, y) / 4;
		return corners + sides + center;
	}
	float InterpolatedNoise(float x, float y)
	{
		int   integer_X    = int(x);
		float fractional_X = x - integer_X;

		int   integer_Y    = int(y);
		float fractional_Y = y - integer_Y;

		float v1, v2, v3, v4;
		v1 = SmoothedNoise1(integer_X,     integer_Y);
		v2 = SmoothedNoise1(integer_X + 1, integer_Y);
		v3 = SmoothedNoise1(integer_X,     integer_Y + 1);
		v4 = SmoothedNoise1(integer_X + 1, integer_Y + 1);

		float i1, i2;
		i1 = Interpolate(v1 , v2 , fractional_X);
		i2 = Interpolate(v3 , v4 , fractional_X);

		return Interpolate(i1 , i2 , fractional_Y);
	}

	float generate2d(int x, int y, float wavelength, float persistence, float amplitude, float noisiness = 0.1)
	{
		float total = 0;
		float delta = 0;
		int   i = 0;
		//printf("wavelength = %f\n", wavelength);
		//printf("persistence = %f\n", persistence);
		//printf("amplitude = %f\n", amplitude);
		for (;;)
		{
			float frequency  = (1 << i) / wavelength;
			float a = amplitude * pow(persistence, i);
			if ( a < amplitude * noisiness ) break;
			delta = InterpolatedNoise(x * frequency, y * frequency) * a;
			total += delta;
			i++;
			if ( i == 8 ) break;
			//printf("amp=%f, %d, ", amplitude, i);
		}
		//printf("\n");
		return total;
	}

	void  generate2d_x(unsigned char* pixel, int sx, int ex, int y, float wavelength, float persistence, float amplitude, float noisiness = 0.1, float* temp = 0)
	{
#if 0
		std::vector<float> noise(ex);
#elif 0
		float* noise = (float*)malloc(sizeof(float) * ex);
		memset(noise, 0, sizeof(float) * ex);
#else
		float* noise;
		if ( temp ) noise = temp;
		else 		noise = (float*)malloc(sizeof(float) * ex);
		memset(noise, 0, sizeof(float) * ex);
#endif

		float a = amplitude;
		float threshold = amplitude * noisiness;
		for (int i = 0; i < 8 && a >= threshold; i++)
		{
			//if ( a < threshold ) break;

			float frequency  = (1 << i) / wavelength;

			int   integer_Y    = int(y * frequency);
			float fractional_Y = (y * frequency) - integer_Y;
			float fY = (1.f - cos(fractional_Y * 3.1415927f)) * .5f;

			float* pn = noise;
			for (int x = sx; x < ex; x++)
			{
				int   integer_X    = int(x * frequency);
				float fractional_X = (x * frequency) - integer_X;

				float n[16];
				Noise16(n, integer_X, integer_Y);

				float v1, v2, v3, v4;
				v1 = (n[0x0] + n[0x8] + n[0x2] + n[0xA]) / 16 + (n[0x1] + n[0x9] + n[0x4] + n[0x6]) / 8 + n[0x5] / 4;
				v2 = (n[0x4] + n[0xC] + n[0x6] + n[0xE]) / 16 + (n[0x5] + n[0xD] + n[0x8] + n[0xA]) / 8 + n[0x9] / 4;
				v3 = (n[0x1] + n[0x9] + n[0x3] + n[0xB]) / 16 + (n[0x2] + n[0xA] + n[0x5] + n[0x7]) / 8 + n[0x6] / 4;
				v4 = (n[0x5] + n[0xD] + n[0x7] + n[0xF]) / 16 + (n[0x6] + n[0xE] + n[0x9] + n[0xB]) / 8 + n[0xA] / 4;

				float i1, i2;
				float fX = (1.f - cos(fractional_X * 3.1415927f)) * .5f;
				i1 = v1 * (1.f - fX) + v2 * fX;
				i2 = v3 * (1.f - fX) + v4 * fX;

				float delta;
				delta = (i1 * (1.f - fY) + i2 * fY) * a;
				*pn++ += delta;
			}
			a *= persistence;
		}
		for (int x = sx; x < ex; x++)
		{
			int temp;
			temp = (int)(noise[x]);
			if ( temp > 255 ) temp = 255;
			if ( temp < 0 ) temp = 0;
			pixel[x] = (unsigned char)temp;
		}
#if 0
		free(noise);
#else
		if ( !temp ) free(noise);
#endif
	}

	void  generate2d_xy(unsigned char* img, int stride, int width, int height, float wavelength, float persistence, float amplitude, float noisiness = 0.1)
	{
		std::vector<float> noise;

		unsigned char* pixel = img;

		for (int y = 0; y < height; y++)
		{
			noise.clear();
			noise.assign(width, 0.0);

			for (int i = 0; i < 8; i++)
			{
				float frequency  = (1 << i) / wavelength;
				float a = amplitude * pow(persistence, i);
				if ( a < amplitude * noisiness ) break;

				int   integer_Y    = int(y * frequency);
				float fractional_Y = (y * frequency) - integer_Y;
				float fY = (1 - cos(fractional_Y * 3.1415927f)) * .5f;

				for (int x = 0; x < width; x++)
				{
					int   integer_X    = int(x * frequency);
					float fractional_X = (x * frequency) - integer_X;
#if 0
					float n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, nA, nB, nC, nD, nE, nF;
					n0 = Noise(integer_X - 1, integer_Y - 1);
					n1 = Noise(integer_X - 1, integer_Y    );
					n2 = Noise(integer_X - 1, integer_Y + 1);
					n3 = Noise(integer_X - 1, integer_Y + 2);
					n4 = Noise(integer_X    , integer_Y - 1);
					n5 = Noise(integer_X    , integer_Y    );
					n6 = Noise(integer_X    , integer_Y + 1);
					n7 = Noise(integer_X    , integer_Y + 2);
					n8 = Noise(integer_X + 1, integer_Y - 1);
					n9 = Noise(integer_X + 1, integer_Y    );
					nA = Noise(integer_X + 1, integer_Y + 1);
					nB = Noise(integer_X + 1, integer_Y + 2);
					nC = Noise(integer_X + 2, integer_Y - 1);
					nD = Noise(integer_X + 2, integer_Y    );
					nE = Noise(integer_X + 2, integer_Y + 1);
					nF = Noise(integer_X + 2, integer_Y + 2);
					float v1, v2, v3, v4;
					v1 = (n0 + n8 + n2 + nA) / 16 + (n1 + n9 + n4 + n6) / 8 + n5 / 4;
					v2 = (n4 + nC + n6 + nE) / 16 + (n5 + nD + n8 + nA) / 8 + n9 / 4;
					v3 = (n1 + n9 + n3 + nB) / 16 + (n2 + nA + n5 + n7) / 8 + n6 / 4;
					v4 = (n5 + nD + n7 + nF) / 16 + (n6 + nE + n9 + nB) / 8 + nA / 4;
#else
					float n[16];
					Noise16(n, integer_X, integer_Y);

					float v1, v2, v3, v4;
					v1 = (n[0x0] + n[0x8] + n[0x2] + n[0xA]) / 16 + (n[0x1] + n[0x9] + n[0x4] + n[0x6]) / 8 + n[0x5] / 4;
					v2 = (n[0x4] + n[0xC] + n[0x6] + n[0xE]) / 16 + (n[0x5] + n[0xD] + n[0x8] + n[0xA]) / 8 + n[0x9] / 4;
					v3 = (n[0x1] + n[0x9] + n[0x3] + n[0xB]) / 16 + (n[0x2] + n[0xA] + n[0x5] + n[0x7]) / 8 + n[0x6] / 4;
					v4 = (n[0x5] + n[0xD] + n[0x7] + n[0xF]) / 16 + (n[0x6] + n[0xE] + n[0x9] + n[0xB]) / 8 + n[0xA] / 4;
#endif

					float i1, i2;
					float fX = (1 - cos(fractional_X * 3.1415927f)) * .5f;
					i1 = v1 * (1 - fX) + v2 * fX;
					i2 = v3 * (1 - fX) + v4 * fX;

					float delta;
					delta = (i1 * (1 - fY) + i2 * fY) * a;
					noise[y * width + x] += delta;
				}
			}

			for (int x = 0; x < width; x++)
			{
				int temp;
				temp = (int)(noise[x]);
				if ( temp > 255 ) temp = 255;
				if ( temp < 0 ) temp = 0;
				pixel[x] = (unsigned char)temp;
			}

			//
			pixel += stride;
		}
	}
};

namespace PicturePerfectOpenCVOperators
{
	
class NoiseV : public Operator
{
	private :

    public :
		virtual ResultType  process(const strings& tokens, PacketStore& packets)
		{
			assertNumberOfArgs(tokens, 3);

			// 
			Packet& src = packets[tokens[2]];
			Packet& dst = packets[tokens[1]];

			// Image
			IplImage* dest = dst.image;

			//printf("PerlinNoise\n");
			PerlinNoise noise;
#if 0
			noise.generate2d_xy((unsigned char*)dest->imageData, dest->widthStep, dest->width, dest->height, (float)(dest->width >> 4), (float)src.value, 256);
#else
			float* temp = new float[dest->width];
			for (int y = 0; y < dest->height; y++)
			{
				unsigned char* pixel = (unsigned char*)(dest->imageData + dest->widthStep * y);
#if 0
				noise.generate2d_x(pixel, 0, dest->width, y, (float)(dest->width >> 4), (float)packets[tokens[1]].value, 256);
#elif 1
				noise.generate2d_x(pixel, 0, dest->width, y, (float)(dest->width >> 4), (float)src.value, 256, 0.1f, temp);
#else
				for (int x = 0; x < dest->width; x++)
				{
					int temp;
					temp = (int)(noise.generate2d(x, y, (float)(dest->width >> 4), (float)packets[tokens[1]].value, 256));
					if ( temp > 255 ) temp = 255;
					if ( temp < 0 ) temp = 0;
					pixel[x] = (unsigned char)temp;
					//printf("pixel = %u\n", temp);
				}
#endif
			}
			delete[] temp;
#endif

			// Value
			dst.value = src.value;

			return RES_NEXT;
		}
};

}
