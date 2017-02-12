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

#include "ArduinoTypes.h"

// Not applicable/available integer value
#define IntNA -32768

class DataLog24H {
	public:
		// Constructor
		DataLog24H();
		
		// AddData - Adds a new data value to the logger. This method should be
		// called once every minute
		void AddData(int NewData1M);
		
		// Get mean values
		int GetMean1M();
		int GetMean5M();
		int GetMean1H();
		int GetMean24H();
		
		// Get the integer data arrays
		int *GetData(); // All data concatenated (5*1M, 11*5M, 23*1H)
		int *GetData1M(); // Get the data for the 1M interval (5x)
		int *GetData5M(); // Get the data for the 5M interval (11x)
		int *GetData1H(); // Get the data for the 1H interval (23x)
		
	private:
		// Data array (all intervals concatenated)
		int Data[5+11+23]; // 1M[5], 5M[11], 1H[23];
		
		// Mean values
		int Mean1M, Mean5M, Mean1H, Mean24H;
		
		// Interval counter
		unsigned long Counter;
};
