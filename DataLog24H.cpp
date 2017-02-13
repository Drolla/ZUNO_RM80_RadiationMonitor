/*****************************************************************************
* Z-Wave enabled Radiation, Temperature and Humidity Monitor with LCD Display
******************************************************************************
* 24 hour data logging class. It logs integer values in 3 different intervals:
*   - 1 minute interval during 5 minutes
*   - 5 minute interval during the next 55 minutes -> 1 hour in total
*   - 1 hour interval during the next 23 hours -> 24 hours in total
* Mean values are calculated for the last 5 minutes, last hour and last 24 
* hours.
*
* Copyright (C) 2017 Andreas Drollinger
*****************************************************************************
* See the file "LICENSE" for information on usage and redistribution
* of this file, and for a DISCLAIMER OF ALL WARRANTIES.
\****************************************************************************/

#include "DataLog24H.h"

// Get mean values
int DataLog24H::GetMean1M() {
	return Mean1M;}
int DataLog24H::GetMean5M() {
	return Mean5M;}
int DataLog24H::GetMean1H() {
	return Mean1H;}
int DataLog24H::GetMean24H() {
	return Mean24H;}

// Get the data arrays
int *DataLog24H::GetData() {
	return Data;}
int *DataLog24H::GetData1M() {
	return Data;}
int *DataLog24H::GetData5M() {
	return Data+5;}
int *DataLog24H::GetData1H() {
	return Data+5+11;}

// Constructor - Set all data to IntNA (not available). Reset the interval
// counter.
DataLog24H::DataLog24H() {
	for (byte i=0; i<5+11+23; i++)
		Data[i]=IntNA;
	Mean1M=Mean5M=Mean1H=Mean24H=IntNA;
	Counter=0;
}

// AddData - Log a new data value. This method should be called exactly once
// per minute.
// Each time a value is added the 5 1-minute interval values are shifted by 
// one position and the mean value is calculated. Every 5 minutes this mean 
// value is shifted into the 5-minute interval array. The mean value of this
// array is shifted into the 1-hour interval array every hour.
void DataLog24H::AddData(int NewData1M) {
	Counter++; // Increment the interval counter

	Mean1M=NewData1M; // The provided value is considered as 1M mean value
	
	// Shift the new value into the 1-minute interval array
	int ValOld;
	int Mean=0;
	int ValNew=Mean1M;
	for (byte i=0; i<5; i++) {
		ValOld=Data[i];
		Data[i]=ValNew;
		Mean+=ValOld;
		ValNew=ValOld;
	}
	
	// Update the 5-minute mean value. Skip this update if less than 5 minutes 
	// are registered. Skip updating the 5-minute interval array if the time is 
	// not a multiple of 5 minutes.
	if (Counter<6)
		return;
	Mean/=5;
	Mean5M=Mean;
	if (Counter%5!=1)
		return;

	// Every 5 minutes, shift mean value from the 1-minute interval array into 
	// the 5-minute interval array
	ValNew=Mean; // Mean value from the 1st 5 minutes
	for (byte i=5; i<5+11; i++) {
		ValOld=Data[i];
		Data[i]=ValNew;
		Mean+=ValOld;
		ValNew=ValOld;
	}
	
	// Update the 1-hour mean value. Skip this update if less than 1 hour
	// is registered. Skip updating the 1-hour interval array if the time is 
	// not a multiple of 1 hour.
	if (Counter<5*12+1)
		return;
	Mean/=12;
	Mean1H=Mean;
	if (Counter%(5*12)!=1)
		return;

	// Every hour, shift mean value from the 5-minute interval array into the 
	// 1-hour interval array
	ValNew=Mean; // Mean value from the 1st hour
	for (byte i=5+11; i<5+11+23; i++) {
		ValOld=Data[i];
		Data[i]=ValNew;
		Mean+=ValOld;
		ValNew=ValOld;
	}

	// Update the 24-hours mean value. Skip this update if less than 24 hours
	// are registered.
	if (Counter<5*12*24+1)
		return;
	Mean/=24;
	Mean24H=Mean;
}
