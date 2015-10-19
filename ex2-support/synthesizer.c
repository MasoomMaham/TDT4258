#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

double _radPerCircle = 3.14159265359*2;		//Declaration of a global variabel which is used to create a full sine wave by 							  letting the wave go a full round in the "unit circle"(2pi). 
int _frequency = 2000;				//Declaration of the frequency the sound wave should use. 
int _sampleRate = 44100;			//Declaration of the samplingrate. How many times should the sound wave be 							  sampled, higher samplingrate the better the sound becomes. 
int calculateSine(double radian);		//Declaration of function calculateSine(), which calculate a sine value in a point.

void setSineOscillator(int sampleRate)		//Function used to set the samplingrate.
{
	_sampleRate = sampleRate;		//Set the value of _sampleRate to the one passed to the parameter of the function.
}

void setFrequency(int _valueFrequency)		//Function used to set the frequency.
{
	_frequency = _valueFrequency;		//Set the value of _valueFrequency to the one passed to the parameter of the 							  function.
}

double powerOfInt(double base, int degree) 	//Function used to calculate the power of a number.
{
	double powerOfBase = base;		//Creating a variable which contains the number to calculate the power for. 
	for(int powerTimes = 0; powerTimes < degree; powerTimes++)//Loop calculating the power of a number by looping degree times.
	{
		powerOfBase *= base;		//Calculating the power by multiplying base degree times and storing it in 							  powerOfBase
	}
	
	return powerOfBase;			//Returning the power of the number we wanted to calculate.
}

int getNext(int sampleNumberInSeconds)//Function used to generate an angle to create the next sound wave in a point at the x-axis. 
{
	int samplePerOccilation = (_sampleRate/_frequency);	
	double depthIntoOccilations = (samplePerOccilation % sampleNumberInSeconds) / samplePerOccilation;
	return calculateSine(depthIntoOccilations * _radPerCircle);   //Call function to calculate a sine value for the angle.
}

int calculateSine(double radian)		 //Function to calculate the sine value of an angle by use of taylor approximation.
{
	double sineApprox = radian - (powerOfInt(radian,3)/(3*2*1)) + (powerOfInt(radian,5)/(5*4*3*2*1)) - (powerOfInt(radian,7)/(7*6*5*4*3*2*1));				 //Calculating and storing the sine value in a variable. 
	return (int)sineApprox;			 //Returning the sine value as an int because the DAC only operates on that type.
}


int playMusic(int wavePoint, int volume)	//Function used to play the music to the DAC, and call the calculating functions.
{	
	return getNext(wavePoint)*volume;	//Calls the function getNext with an int as argument. The int is here thought of 							  as a value of time on the x-axis. Returning the point on the sound wave to be 						  played
}
