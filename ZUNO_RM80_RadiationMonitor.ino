/*****************************************************************************
* Z-Wave enabled Radiation, Temperature and Humidity Monitor with LCD Display
******************************************************************************
* This is the main Z-Uno application file. The project requires the following 
* hardware components:
*   - Radiation monitor
*     RM-60/70/80 radiation monitor from AWARE Electronics. Connected to the
*     external interrupt 'ZUNO_EXT_ZEROX'.
*   - Temperature/humidity sensor
*     DHT22 temp/hum sensor, 10kOhm resistor. Controlled via a single pin using
*     the ZUNO_DHT module.
*   - LCD display
*     Adafruit Sharp 1.3" Memory LCD Display. Controlled via the SPI interface.
*   - Control buttons
*     2 simple press buttons connected to 2 DIOs.
* 
* In addition to the functions related to the above hardware components, the 
* application provides the following features:
*   - Radiation data, temperature and humidity can be reported every minutes
*     via Z-Wave
*   - Radiation events can be indicated via the Z-Uno user LED
*   - Status information can be reported via the USB attached serial interface
*   - Configuration options: Z-Wave reporting, LED activity and status 
*     reporting via serial interface can individually be enabled or disabled.
*     Configurations are stored in the EEPROM and restored after power cycling.
* 
* The following files are part of this project:
*   - ZUno_RM80_RadiationMonitor.ino - Main Z-Uno project file (this file!)
*   - DataLog24H.h        - 24 hour data logging module, header file
*   - DataLog24H.cpp      - 24 hour data logging module, implementation file
*   - SharpMemoryLCD.h    - Sharp Memory LCD module, header file
*   - SharpMemoryLCD.cpp  - Sharp Memory LCD module, implementation file
*   - LICENSE             - License information
*
* Copyright (C) 2017 Andreas Drollinger
*****************************************************************************
* See the file "LICENSE" for information on usage and redistribution
* of this file, and for a DISCLAIMER OF ALL WARRANTIES.
\****************************************************************************/


/**** Module inclusion, project settings ****/

// Include the different modules
#include "ZUNO_DHT.h"
#include "DataLog24H.h"
#include "EEPROM.h"
#include "SharpMemoryLCD.h"

// The radiation is always logged during 24 hours. The temperature and humidity
// logging can be disabled to reduce memory space (set the following 
// definitions to 1).
#define LOG24H_TEMPERATURE 1
#define LOG24H_HUMIDITY 1

// Pin definitions, LED uses the Z-Uno user LED
#define LED_PIN 13
#define DHTPIN 9 
#define PIN_BUTTON1 6
#define PIN_BUTTON2 5


/**** Z-Wave channel setup and sleep mode definition ****/

ZUNO_SETUP_CHANNELS(
  ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_GENERAL_PURPOSE_VALUE,
                         1,
                         SENSOR_MULTILEVEL_SIZE_TWO_BYTES,
                         SENSOR_MULTILEVEL_PRECISION_ZERO_DECIMALS, RadEventGet1M),
  ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_GENERAL_PURPOSE_VALUE,
                         1,
                         SENSOR_MULTILEVEL_SIZE_TWO_BYTES,
                         SENSOR_MULTILEVEL_PRECISION_ZERO_DECIMALS, RadEventGet5M),
  ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_GENERAL_PURPOSE_VALUE,
                         1,
                         SENSOR_MULTILEVEL_SIZE_TWO_BYTES,
                         SENSOR_MULTILEVEL_PRECISION_ZERO_DECIMALS, RadEventGet1H),
  ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_GENERAL_PURPOSE_VALUE,
                         1,
                         SENSOR_MULTILEVEL_SIZE_TWO_BYTES,
                         SENSOR_MULTILEVEL_PRECISION_ZERO_DECIMALS, RadEventGet24H),
  ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_TEMPERATURE, SENSOR_MULTILEVEL_SCALE_CELSIUS, 
                         SENSOR_MULTILEVEL_SIZE_TWO_BYTES, SENSOR_MULTILEVEL_PRECISION_ONE_DECIMAL, TemperatureGet1M),
  ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_RELATIVE_HUMIDITY, SENSOR_MULTILEVEL_SCALE_PERCENTAGE_VALUE,
                         SENSOR_MULTILEVEL_SIZE_TWO_BYTES, SENSOR_MULTILEVEL_PRECISION_ZERO_DECIMALS, HumidityGet1M)
);

ZUNO_SETUP_SLEEPING_MODE(ZUNO_SLEEPING_MODE_SLEEPING);


/**** Temperature/humidity sensor ****/

DHT dht(DHTPIN, DHT22); // Sensor object


/**** Radiation, temperature and humidity measurements -
                    data logging, Z-Wave getter functions ****/

// Radiation measurements
unsigned int NbrRadEvents; // Counts number of radiation events per time interval)
unsigned char RadEventOccured; // Will be set by the radiation event interrupt routine
DataLog24H RadEventDataLog; // Radiation data logging 24h

unsigned long NextLogTime; // Next log time (interval is 60 seconds)
#define IntervalMs 60000

// Z-Uno/Z-Wave getter functions, returns 0 if the value is N/A
int RadEventGet1M() {
	int Mean=RadEventDataLog.GetMean1M();
	return (Mean<0 ? 0 : Mean);}
int RadEventGet5M() {
	int Mean=RadEventDataLog.GetMean5M();
	return (Mean<0 ? 0 : Mean);}
int RadEventGet1H() {
	int Mean=RadEventDataLog.GetMean1H();
	return (Mean<0 ? 0 : Mean);}
int RadEventGet24H() {
	int Mean=RadEventDataLog.GetMean24H();
	return (Mean<0 ? 0 : Mean);}

// Temperature: Data logging and getter functions - Implementation depends on
//              the data logging configuration
#if LOG24H_TEMPERATURE
	DataLog24H TemperatureDataLog;
	int TemperatureGet1M() {
		int Mean=TemperatureDataLog.GetMean1M();
		return (Mean==IntNA ? 0 : Mean);}
	int TemperatureGet1MInt() {
		return TemperatureDataLog.GetMean1M(); }
	int TemperatureGet24HInt() {
		return TemperatureDataLog.GetMean24H(); }
#else
	int Temperature=IntNA;
	int TemperatureGet1M() {
		return (Temperature==IntNA ? 0 : Temperature);}
	int TemperatureGet1MInt() {
		return Temperature; }
	int TemperatureGet24HInt() {
		return IntNA; }
#endif

// Humidity: Data logging and getter functions - Implementation depends on
//           the data logging configuration
#if LOG24H_HUMIDITY
	DataLog24H HumidityDataLog;
	int HumidityGet1M() {
		int Mean=HumidityDataLog.GetMean1M();
		return (Mean==IntNA ? 0 : Mean);}
	int HumidityGet1MInt() {
		return HumidityDataLog.GetMean1M(); }
	int HumidityGet24HInt() {
		return HumidityDataLog.GetMean24H(); }
#else
	int Humidity=IntNA;
	int HumidityGet1M() {
		return (Humidity==IntNA ? 0 : Humidity);}
	int HumidityGet1MInt() {
		return Humidity; }
	int HumidityGet24HInt() {
		return IntNA; }
#endif


/**** Status print to serial interface ****/

// SerialPrintA - Prints an array of integer values
void SerialPrintA(int *Array, unsigned char Size) {
  for (unsigned char i=0; i<Size; i++) {
    if (Array[i]>=0)
      Serial.print(Array[i]);
    else
      Serial.print("..");
    Serial.print(" ");
  }
  Serial.print("  ");
}

// SerialPrintStatus - Prints status information
void SerialPrintStatus() {
  Serial.print("Temp="); 
  Serial.print(0.1*TemperatureGet1MInt());
  Serial.print("*C\tHum="); 
  Serial.print(HumidityGet1MInt());
  Serial.print("%\t");
  Serial.print(RadEventDataLog.GetMean1M());
  Serial.print("\t");
  Serial.print(RadEventDataLog.GetMean5M());
  Serial.print("\t");
  Serial.print(RadEventDataLog.GetMean1H());
  Serial.print("\t");
  Serial.print(RadEventDataLog.GetMean24H());
  Serial.print("\t");
  SerialPrintA(RadEventDataLog.GetData1M(),5);
  SerialPrintA(RadEventDataLog.GetData5M(),11);
  SerialPrintA(RadEventDataLog.GetData1H(),23);
  Serial.println("");
  Serial.flush();
}

/**** Configurations ****/

// Configurations are stored in a single byte. Each bit of it corresponds to a
// configuration feature. The configuration byte is stored after each update in
// the EEPROM (address 0). After power up/reset this configuration value is 
// restored.

// Configuration byte with default value (all features are enabled).
byte Config=7;

// Macros to get for the different features the configuration state
#define CONFIG_ZWAVE_REPORT (Config&1)
#define CONFIG_LED_REPORT (Config&2)
#define CONFIG_SERIAL_REPORT (Config&4)

// ReadConfig - Read the configuration from the EEPROM
void ReadConfig() {
	// Read the configuration and the checksum
	byte StoredConfig=EEPROM.read(0);
	byte StoredCheckSum=EEPROM.read(1);

	// Ignore the configuration if the checksum is incorrect
	if ((StoredConfig^StoredCheckSum)!=0xff)
		return;

	// Restore the configuration
	Config=StoredConfig;
}

// WriteConfig - Store the configuration and its checksum in the EEPROM. The
// checksum corresponds to the configuration value with all bits inverted.
void WriteConfig() {
	EEPROM.update(0,Config);
	EEPROM.update(1,~Config);
}

// ToggleConfig - Toggle the bit of configuration number defined by ConfigNbr.
// Store the updated configuration value in the EEPROM after an update. The 
// return value indicates if the configuration menu has to be left (exit selected).
bool ToggleConfig(byte ConfigNbr) {
	// Check if the config menu has to be left ('exit' selected)
	if (ConfigNbr==3)
		return 1;

	// The configuration number is bigger than 3 -> should not happen, ignore it
	else if (ConfigNbr>3)
		return 0;

	// Toggle the bit of the given configuration number and store the new 
	// config value in the EEPROM.
	Config^=(1<<ConfigNbr);
	WriteConfig();
	return 0;
}

/**** Display outputs ****/

// Create the display LCD object
SharpMemoryLCD LCD=SharpMemoryLCD();

// writeFit_Label - Integer writeFit function extended with a 2 character 
// label. The 2 label characters are written in small characters in 2 rows.
xy_t writeFit_Label (xy_t x0,xy_t y0,xy_t x1,char *label,int Val, unsigned char DecimalPos=0) {
	// Write the 2 label characters (no string length check is performed!)
	LCD.writeChrS(x0,y0,*label++);
	x0=LCD.writeChrS(x0,y0+8,*label);
	x0++;
	
	// Write the integer. Write a '-' if the value is invalid.
	if (Val==IntNA)
		x0=LCD.writeFit(x0,y0,x1,"-",SMLCD_WRITE_TIGHT|SMLCD_WRITE_CENTERY);
	else
		x0=LCD.writeFit(x0,y0,x1,int2Str(Val,DecimalPos),SMLCD_WRITE_TIGHT|SMLCD_WRITE_CENTERY);
	return x0; // Return next character position
}

// vLineDotted - Vertical doted line
void vLineDotted (xy_t x, xy_t y0, xy_t y1) {
	for (xy_t y=y0;y<=y1;y+=2) // Set the pixel every 2nd row
		LCD.setPixel(x,y);
}

// WriteCompact - Write an integer value rounded in a compact form. Values 
// between 1000 and 9999 (for example 1234) are written like this:  -> 1.2k
// Values are written in normal small size characters if values are outside of
// this range (including negative values) or if a decimal position is defined.
void WriteCompact (xy_t x, xy_t y, int Val, unsigned char DecimalPos) {
	if (DecimalPos!=0 || Val<1000 || Val>=10000) { // Normal writing
		LCD.writeS(x,y,int2Str(Val,DecimalPos),SMLCD_WRITE_TIGHT);
	} else { // Short writing (1.2k)
		x=LCD.writeChrS(x,y,'0'+(Val/1000),SMLCD_WRITE_TIGHT);
		x=LCD.writeChrS(x,'.',SMLCD_WRITE_TIGHT);
		x=LCD.writeChrS(x,y,'0'+((Val/100)%10),SMLCD_WRITE_TIGHT);
		x=LCD.writeChrS(x,'k',SMLCD_WRITE_TIGHT);
	}
}

// setPixel3x3 - Set the pixels in a 3x3 dot area
void setPixel3x3 (xy_t x, xy_t y) {
	for (byte dx=0;dx<3;dx++)
		for (byte dy=0;dy<3;dy++)
			LCD.setPixel(x+dx-1,y+dy-1);
}

// DrawGraph - Draw a 24-hour integer value graph between display row y0 and y1 
// using the full display size. The integer value array needs to have the 
// format of the DataLog24 class (5*1min, 11*5min, 23*1hour).
// Optionally X-axis labels and a value decimal limiter can be defined. To 
// reduce stack usage all options are provided via a single byte.
// The graph is automatically scaled to the available window height.

// DrawGraph options (X label to be displayed, optional decimal position)
#define DRAWGRAPH_OPTION_WITHXLABEL 0x80
#define DRAWGRAPH_OPTION_DECIMALPOS(Pos) (Pos)

void DrawGraph (xy_t y0, xy_t y1, char *Title, int *Data, unsigned char Option) {
	// Display the graph title if the title string is not empty
	if (*Title!=0) {
		LCD.writeS(20,y0,Title,SMLCD_WRITE_TIGHT);
		y0+=8; // Reduce the graph area by the amount taken by the title
	}
	
	// Display optionally the X labels
	if (Option&DRAWGRAPH_OPTION_WITHXLABEL) {
		LCD.writeS(91,y1-6,"0",SMLCD_WRITE_TIGHT);
		LCD.writeS(76,y1-6,"5M",SMLCD_WRITE_TIGHT);
		LCD.writeS(58,y1-6,"1H",SMLCD_WRITE_TIGHT);
		LCD.writeS(17,y1-6,"24H",SMLCD_WRITE_TIGHT);
		y1-=8;
	}
	
	// Draw the graph rectangle and the graph subdivisions
	LCD.rect(17,y0,95,y1);
	vLineDotted(85,y0,y1); // Separation between 1M and 5M
	vLineDotted(63,y0,y1); // Separation between 5M and 1H

	// Determine the minumum and maximum values (used to scale the graph)
	int Min, Max;
	Min=Max=Data[0];
	for (byte i=1; i<5+11+23; i++) {
		if (Data[i]!=IntNA) { // Ignore the value if it is invalid/undefined
			if (Min==IntNA)
				Min=Max=Data[i];
			else {
				Min=min(Min,Data[i]);
				Max=max(Max,Data[i]);
			}
		}
	}
	
	// Don't display the graph if no values are available
	if (Min==IntNA)
		return;
	// Ensure a minimum scale (to avoid Max=Min, which leads to div by zero)
	if (Max-Min<5)
		Max=Min+5;

	// Y labels: Min and Max graph values
	WriteCompact(0,y0+2,Max,Option&0x0f);
	WriteCompact(0,y1-6,Min,Option&0x0f);

	// Graph scale factor = (y1-y0)/(Max-Min). To avoid floating point numbers 
	// and quantization effects the scale factor is multiplied by 256 (<<8) and
	// the scaled value divided again by 256 (>>8).
	int Mul=((y1-y0)<<8)/(Max-Min);
	
	// Draw all points of the graph (3x3 pixels), connect neighbor values by a 
	// vertical line.
	unsigned char x=94; // Graph start position (right display side)
	xy_t yl, y=xy_tNA;
	for (byte i=0; i<5+11+23; i++) {
		yl=y; // Store previous value
		if (Data[i]==IntNA) { // Value is invalid)
			y=xy_tNA;
		} else {
			// Calculate the 3x3 pixel position
			y=y1-((Mul*(Data[i]-Min))>>8);
			setPixel3x3(x,y);
			// Draw the vertical line to the previous pixel if this one was valid
			if (yl!=xy_tNA)
				LCD.line(x+1,yl,x+1,y);
		}
		x-=2;
	}
}

// LCD display layout: Overview
void LCD_ShowOverview() {
	LCD.writeS(0,0,"Radiation(ev/s):",SMLCD_WRITE_TIGHT);
	LCD.rect(0,8,95,46);

	writeFit_Label(3,11,50,"1M:",RadEventDataLog.GetMean1M(),0);

	writeFit_Label(52,11,94,"1H:",RadEventDataLog.GetMean1H(),0);
	writeFit_Label(3,29,50,"5M:",RadEventDataLog.GetMean5M(),0);
	writeFit_Label(52,29,94,"1D:",RadEventDataLog.GetMean24H(),0);

	LCD.writeS(0,50,"Temp(C):",SMLCD_WRITE_TIGHT);
	LCD.rect(0,58,55,95);
	writeFit_Label(3,61,52,"1M:",TemperatureGet1MInt(),1);
	writeFit_Label(3,78,52,"1D:",TemperatureGet24HInt(),1);

	LCD.writeS(60,50,"Hum(%):",SMLCD_WRITE_TIGHT);
	LCD.rect(58,58,95,95);
	writeFit_Label(61,61,94,"1M:",HumidityGet1MInt(),0);
	writeFit_Label(61,78,94,"1D:",HumidityGet24HInt(),0);
}

// LCD display layout: Radiation graph
void LCD_ShowGraphRad() {
	DrawGraph(0,95,"Rad (ev/s):",RadEventDataLog.GetData(),DRAWGRAPH_OPTION_WITHXLABEL);
}

// LCD display layout: Temperature graph (only if temperature logging is enabled)
#if LOG24H_TEMPERATURE
	void LCD_ShowGraphTemp() {
		DrawGraph(0,95,"Temp (C):",TemperatureDataLog.GetData(),DRAWGRAPH_OPTION_WITHXLABEL|DRAWGRAPH_OPTION_DECIMALPOS(1));
	}
#endif

// LCD display layout: Combined radiation and temperature graphs (only if 
// temperature logging is enabled)
#if LOG24H_TEMPERATURE
	void LCD_ShowGraphRadTemp() {
		DrawGraph(0,40,"Rad (ev/s):",RadEventDataLog.GetData(),0);
		DrawGraph(43,95,"Temp (C):",TemperatureDataLog.GetData(),DRAWGRAPH_OPTION_WITHXLABEL|DRAWGRAPH_OPTION_DECIMALPOS(1));
	}
#endif

// LCD display layout: Humidity graph (only if humidity logging is enabled)
#if LOG24H_HUMIDITY
	void LCD_ShowGraphHum() {
		DrawGraph(0,95,"Hum (%):",HumidityDataLog.GetData(),DRAWGRAPH_OPTION_WITHXLABEL);
	}
#endif

// LCD display layout: Combined temperature and humidity graphs (only if data 
// logging is enabled for both paramters)
#if LOG24H_TEMPERATURE && LOG24H_HUMIDITY
	void LCD_ShowGraphTempHum() {
		DrawGraph(0,40,"Temp (C):",TemperatureDataLog.GetData(),DRAWGRAPH_OPTION_DECIMALPOS(1));
		DrawGraph(43,95,"Hum (%):",HumidityDataLog.GetData(),DRAWGRAPH_OPTION_WITHXLABEL);
	}
#endif

// LCD display layout: Show the configuration page, indicate the configuration 
// selection
void LCD_ShowConfig(byte ActiveConfig) {
	// Write the configuration parameters (+exit item)
	LCD.writeS(0,0,"Configurations",SMLCD_WRITE_TIGHT);
	LCD.writeS(10,20,"Z-Wave report",SMLCD_WRITE_TIGHT);
	LCD.writeS(10,31,"LED",SMLCD_WRITE_TIGHT);
	LCD.writeS(10,42,"Serial report",SMLCD_WRITE_TIGHT);
	LCD.writeS(10,53,"Exit",SMLCD_WRITE_TIGHT);

	// Indicate the current configuration selection
	LCD.writeS(0,20+ActiveConfig*11,">>",SMLCD_WRITE_TIGHT);

	// Indicate what configuration is enabled/disabled
	for (byte c=0; c<3; c++) {
		LCD.circle(91,24+c*11,4);
		if ((Config>>c)&1) // Configuration is enabled
			LCD.circle(91,24+c*11,2);
	}
}

// Manage the different display layouts. The current display layout is defined 
// by 'DisplayMode' in the following way:
// * DisplayMode<0x80
//      DisplayMode selects one of the "normal" display layouts
// * DisplayMode>=0x80
//      The configuration layout is selected. The 7 LSB of select a specific 
//      configuration option.

byte DisplayMode=0;

// LCD_Show - Show a certain display layout in function of an active button
// state (1=button 1, 2=button 2, 0=no button)
void LCD_Show(byte ActiveButton) {
	// Clear the display buffer
	LCD.clear();

	// A "normal" display layout is selected and button 1 activated -> Select
	// the next "normal" display layout. The max number of "normal" layouts
	// depends on the availability of the temperature and humidity graphs
	if (!(DisplayMode&0x80) && ActiveButton==1) {
		DisplayMode++;
		DisplayMode=DisplayMode%(2+2*LOG24H_TEMPERATURE+1*LOG24H_HUMIDITY+
		                         1*(LOG24H_TEMPERATURE || LOG24H_HUMIDITY));

	// A "normal" display layout is selected and button 2 activated -> Switch
	// to the config layout
	} else if (!(DisplayMode&0x80) && ActiveButton==2) {
		DisplayMode=0x80; // Config layout, 1st configuration value

	// The config layout is selected, and button 2 activated -> chose the next 
	// configuration option
	} else if ((DisplayMode&0x80) && ActiveButton==2) {
		DisplayMode++;
		DisplayMode=0x80+(DisplayMode%4); // 4 configuration options including 'exit'

	// The config layout is selected, and button 1 activated -> toggle the
	// configuraiton option, and switch back to the first 'normal' layout if
	// 'exit' is selected
	} else if ((DisplayMode&0x80) && ActiveButton==1) {
		if (ToggleConfig(DisplayMode&0x7f))
			DisplayMode=0x00;
	}

	// Display the active layout
	if (!(DisplayMode&0x80)) { // A 'normal' layout is selected
		switch (DisplayMode) {
			case 0:
				LCD_ShowOverview();
				break;
			case 1:
				LCD_ShowGraphRad();
				break;
			#if LOG24H_TEMPERATURE
			case 2:
				LCD_ShowGraphTemp();
				break;
			case 3:
				LCD_ShowGraphRadTemp();
				break;
			#endif
			#if LOG24H_HUMIDITY
			case 2+2*LOG24H_TEMPERATURE:
				LCD_ShowGraphHum();
				break;
			#endif
			#if LOG24H_TEMPERATURE && LOG24H_HUMIDITY
			case 5:
				LCD_ShowGraphTempHum();
				break;
			#endif
		}
	} else { // The config layout is selected
		LCD_ShowConfig(DisplayMode&0x7f);
	}
	
	// Update the physical display
	LCD.update();
}

/**** Radiation event handler ****/

// ISR for external interrupt ZUNO_EXT_ZEROX. Used to register events from the 
// RM-60/70/80 radiation monitor.

ZUNO_SETUP_ISR_ZEROX(IntHandlerRadEvent);

void IntHandlerRadEvent() {
	NbrRadEvents++; // Increment the radiation counter
	RadEventOccured++; // Indicates to the main loop that an event happened
}

/**** Setup and loop functions ****/

void setup() {
  // Initialize specific pins
  pinMode(LED_PIN, OUTPUT); // Set LED pin as output
  pinMode(PIN_BUTTON1,INPUT_PULLUP); // Control button 1
  pinMode(PIN_BUTTON2,INPUT_PULLUP); // Control button 2

  // Initialize all modules
  dht.begin(); // DHT22 temperature/humidity sensor
  Serial.begin(); // Status information reports
  zunoExtIntMode(ZUNO_EXT_ZEROX, FALLING); // RM-60/80 input
  LCD.begin(); // LCD display

  // Read the configuration stored in the EEPROM
  ReadConfig();

  // Display the default page
  LCD_Show(false);

  // Set the 1st log time.
  NextLogTime=millis()+IntervalMs;
  
  // Initialize the radiation event counter
  NbrRadEvents=0;
  RadEventOccured=0;
}

void loop() {
	// Generate a user LED flash if this feature has been configured and if
	// an event happened
	if(CONFIG_LED_REPORT && RadEventOccured) {
		digitalWrite(LED_PIN, HIGH); // Enable the LED
		RadEventOccured=0; // Reset the event indicator
		delay(50); // Flash time
		digitalWrite(LED_PIN, LOW); // Disable the user LED again
	}

	// Update the display mode if one of the 2 buttons has been pressed
	if (digitalRead(PIN_BUTTON1)==LOW)
		LCD_Show(1);
	if (digitalRead(PIN_BUTTON2)==LOW)
		LCD_Show(2);

	// If the next log time is reached, log the new values
	if (millis()>=NextLogTime) {
		// Log the number of radiation events, reset the counter
		RadEventDataLog.AddData(NbrRadEvents);
		NbrRadEvents=0;

		// Read the temperature. Log it if temperature logging is enabled. Same
		// for the humidity.
		#if LOG24H_TEMPERATURE
			TemperatureDataLog.AddData(dht.readTemperatureC10());
		#else
			Temperature = dht.readTemperatureC10();
		#endif
		#if LOG24H_HUMIDITY
			HumidityDataLog.AddData(dht.readHumidity());
		#else
			Humidity = dht.readHumidity();
		#endif

		// Generate Z-Wave reports if configured and the if the value is available
		if (CONFIG_ZWAVE_REPORT) {
			if(RadEventDataLog.GetMean1M()>=0)
				zunoSendReport(1);
			if(RadEventDataLog.GetMean5M()>=0)
				zunoSendReport(2);
			if(RadEventDataLog.GetMean1H()>=0)
				zunoSendReport(3);
			if(RadEventDataLog.GetMean24H()>=0)
				zunoSendReport(4);
			zunoSendReport(5);
			zunoSendReport(6);
		}

		// Generate a status reports on the serial interface if this is configured
		if (CONFIG_SERIAL_REPORT)
			SerialPrintStatus();

		// Define the next log time
		NextLogTime+=IntervalMs;
	 
		// Display the new status without changing the display layout
		LCD_Show(false);
	}
	
	// Refresh regularly the physical LCD display (toggle VCOM)
	LCD.refresh();
}
