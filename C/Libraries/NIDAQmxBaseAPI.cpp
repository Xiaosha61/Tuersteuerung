#include "NIDAQmxBase.h"			// Standard NI-Bibliothek
#include "NIDAQmxBaseAPI.h"			// Angepasste API
#include <stdio.h>
#include <stdlib.h>

namespace NIUSB6008 {

	int DAQmxErrChk(int32 rueckgabeWert) { 	//Fehlerbehandlung
		char         errBuff[2048];
	
		if( DAQmxFailed(rueckgabeWert) ) { 
			DAQmxBaseGetExtendedErrorInfo (errBuff, 2048);
			printf ("DAQmxBase Error %ld: %s\n", rueckgabeWert, errBuff);
		
			DAQmxBaseResetDevice ("Dev1");
		
			if (exitBeiFehler) {
				printf("\nACHTUNG: Programm wurde wegen eines Fehlers beendet!\n");
				exit(1);
			}
			return 1;
		}
		return 0; 
	}

	void CloseDevice(){
	
		resetAllOutputs();
		DAQmxErrChk(DAQmxBaseResetDevice ("Dev1"));  // Gerät in Ausgangszustand versetzen
	// printf("\nAlle Tasks gelöscht!\n");
	}

	void writeDigitalPort(uInt8 dig_out_daten, char portNr = '0') {  //Default Port = 0
	
		TaskHandle	taskHandle = 0;
	
	// Channel parameters
		char        chan[10] = "Dev1/port";

	// Write parameters
		uInt8      w_data [1];
		int32       written;
	
		chan[9] = portNr;  // Vervollständige den String mit der Portnummer
	
	// Create Digital Output Channel and Task
		DAQmxErrChk (DAQmxBaseCreateTask ("", &taskHandle));
		DAQmxErrChk ( DAQmxBaseCreateDOChan(taskHandle,chan,"",DAQmx_Val_ChanForAllLines) );

	// Start Task (configure port)
	//DAQmxErrChk (DAQmxBaseStartTask (taskHandle));

	//  Write dig_out_daten to port(s)
	//  Only 1 sample per channel supported for static DIO
	//  Autostart ON
	
		w_data[0] = dig_out_daten;

	// printf("Data to write: 0x%X\n", w_data[0]);  // Debug

		DAQmxErrChk (DAQmxBaseWriteDigitalU8(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,w_data,&written,NULL));
		DAQmxErrChk (DAQmxBaseStopTask (taskHandle));
	}

	uInt32 readDigitalPort(char portNr = '0'){		//Default Port = 0
	// Task parameters
		TaskHandle  taskHandle = 0;

	// Channel parameters
		char  chan[10] = "Dev1/port";

	// Read parameters
		uInt32      r_data [1];
		int32       read;
	
	// Vervollständige den String mit der Portnummer
		chan[9] = portNr;
	
	// Create Digital Input (DI) Task and Channel
		DAQmxErrChk (DAQmxBaseCreateTask ("", &taskHandle));
		DAQmxErrChk (DAQmxBaseCreateDIChan(taskHandle,chan,"",DAQmx_Val_ChanForAllLines));

	// Start Task (configure port)
		DAQmxErrChk (DAQmxBaseStartTask (taskHandle));

	// Read from port
		DAQmxErrChk (DAQmxBaseReadDigitalU32(taskHandle,1,10.0,DAQmx_Val_GroupByChannel,r_data,1,&read,NULL));

	// printf("Data read: 0x%X\n", r_data[0]); //Debug
	
		DAQmxErrChk (DAQmxBaseStopTask (taskHandle));
	
		return r_data[0];
	}

	float64 readAnalog(char eingangNr = '0'){	//Default Eingang = 0
	// Task parameters
		TaskHandle  taskHandle = 0;
	
	// Channel parameters
		char        chan[8] = "Dev1/ai";
		float64     min = 0.0;
		float64     max = 5.0;

	// Timing parameters
		uInt64      samplesPerChan = 1;

	// Data read parameters
	
		float64		data;
		int32       pointsToRead = 1;
		int32       pointsRead;
		float64     timeout = 10.0;

		chan[7] = eingangNr;
	
		DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
		DAQmxErrChk (DAQmxBaseCreateAIVoltageChan(taskHandle,chan,"",DAQmx_Val_RSE,min,max,DAQmx_Val_Volts,NULL));
		DAQmxErrChk (DAQmxBaseStartTask(taskHandle));
		DAQmxErrChk (DAQmxBaseReadAnalogF64(taskHandle,pointsToRead,timeout,DAQmx_Val_GroupByChannel,&data,samplesPerChan,&pointsRead,NULL));

	// printf ("Acquired reading: %f\n", data);		//Debug
	
		DAQmxErrChk (DAQmxBaseStopTask (taskHandle));
	
		return data;
	}

	void writeAnalog(char ausgangNr = '0', float64 data = 5.0){	//Default Ausgang = 0, Spannung = 5.0 Volt
	// Task parameters
		TaskHandle  taskHandle = 0;

	// Channel parameters
		char        chan[8] = "Dev1/ao";
		float64     min = 0.0;
		float64     max = 5.0;

	// Timing parameters
		uInt64      samplesPerChan = 1;

	// Data write parameters
		int32       pointsWritten;
		float64     timeout = 10.0;

		chan[7] = ausgangNr;
	
		DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
		DAQmxErrChk (DAQmxBaseCreateAOVoltageChan(taskHandle,chan,"",min,max,DAQmx_Val_Volts,NULL));
		DAQmxErrChk (DAQmxBaseStartTask(taskHandle));

		DAQmxErrChk (DAQmxBaseWriteAnalogF64(taskHandle,samplesPerChan,0,timeout,DAQmx_Val_GroupByChannel,&data,&pointsWritten,NULL));
	
		DAQmxErrChk (DAQmxBaseStopTask (taskHandle));
	}

	void resetAllOutputs(){ 		//Setzt alle Ausgänge auf 0
		writeDigitalPort(0xFF,'0');
		writeDigitalPort(0x0F,'0');
	
		writeAnalog('0',0.0);
		writeAnalog('1',0.0);
	}
}

