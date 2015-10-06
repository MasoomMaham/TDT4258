#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "efm32gg.h"

double _radPerCircle = PI*2;
double _frequency = 2000;
double _sampleRate = 44100;

void setSineOscillator(double sampleRate)
{
	_sampleRate = sampleRate;
}

void setFrequency(double _valueFrequency)
{
	_frequency = _valueFrequency;
}

double getNext(int sampleNumberInSeconds)
{
	double samplePerOccilation = (_sampleRate/_frequency);
	double depthIntoOccilations = fmod(sampleNumberInSeconds, samplePerOccilation) / samplePerOccilation;
	return Math.Sin(depthIntoOccilations * radPerCircle);
}

void playMusic()
{
  for(int i=0; i < sampleRate*2; i++)
  {
  	double _musicTone = getNext(i);
  	*DAC0_CH0DATA = _musicTone;
  	*DAC0_CH1DATA = _musicTone;
  }
}
