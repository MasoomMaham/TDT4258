#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

double _radPerCircle = 3.14159265359*2;
int _frequency = 2000;
int _sampleRate = 44100;

int calculateSine(double radian);

void setSineOscillator(int sampleRate)
{
	_sampleRate = sampleRate;
}

void setFrequency(int _valueFrequency)
{
	_frequency = _valueFrequency;
}

double powerOfInt(double base, int degree)
{
	double powerOfBase = base;
	for(int powerTimes = 0; powerTimes < degree; powerTimes++)
	{
		powerOfBase *= base;
	}
	
	return powerOfBase;
}

int getNext(int sampleNumberInSeconds)
{
	*GPIO_PA_DOUT = 0x0000000;
	int samplePerOccilation = (_sampleRate/_frequency);
	double depthIntoOccilations = (samplePerOccilation % sampleNumberInSeconds) / samplePerOccilation;
	return (int)calculateSine(depthIntoOccilations * _radPerCircle);
}

int calculateSine(double radian)
{
	double sineApprox = radian - (powerOfInt(radian,3)/(3*2*1)) + (powerOfInt(radian,5)/(5*4*3*2*1)) - (powerOfInt(radian,7)/(7*6*5*4*3*2*1));
	return (int)sineApprox;
}

int playMusic()
{	
	*GPIO_PA_DOUT = 0x0000000;
	return getNext(3) * 30000;
}
