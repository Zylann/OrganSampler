#ifndef __HEADER_UTILITY__
#define __HEADER_UTILITY__

template <typename T>
inline T clamp(T x, const T min, const T max)
{
	if(x > max)
		return max;
	else if(x < min)
		return min;
	return x;
}

inline double noteNumberToFrequency(int noteNumber)
{
	return 440.0 * pow(2.0, (noteNumber - 69.0) / 12.0);
}

inline double lerp(double a, double b, double t)
{
	return a + (b - a) * t;
}

inline double calcExponentialDecreaseFactor(double startLevel, double endLevel, unsigned long long lengthInSamples)
{
	return 1.0 + (log(endLevel) - log(startLevel)) / (lengthInSamples);
}

inline void normalizePath(std::string & str)
{
	for(unsigned int i = 0; i < str.size(); ++i)
	{
		if(str[i] == '\\')
			str[i] = '/';
	}
}

inline bool isEOL(char c)
{
	return c == '\n' || c == '\r';
}


#endif // __HEADER_UTILITY__

