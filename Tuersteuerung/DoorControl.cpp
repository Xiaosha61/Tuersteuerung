/*
 * Praktikum MRT2 
 * ART3 Türsteuerung
 * Institut für Automatisierungstechnik
 * letztes Update Mai 2017
 * Autor: M.Herhold
 */

#include "DoorInterface.h"
#include "DoorControl.h"
#include <iostream>
using namespace std;

DoorControl::DoorControl() : door_if(false, true)
{
	door_if.SecondLevelInit();
}

DoorControl::~DoorControl()
{
	door_if.quit_doorcontrol_flag = true;
}

void DoorControl::setInput(int* eingang) // channels -> control.inputSignal 
{

	door_if.DIO_Read(eingang);
	inputSignal = *eingang;	
}
int DoorControl::getSignal()
{
	return inputSignal;
}

void DoorControl::setOutput(int ausgang)
{
	door_if.DIO_Write(ausgang);
}

void DoorControl::run()
{
	//int i = 500000;
	door_if.DebugString("Please wait 10 Seconds for auto exit, or press 'q'.");
	/*
	while( !door_if.quit_doorcontrol_flag && i-- ){
		door_if.StartTimer(0.2);
	}
	*/
	while(1){
		door_if.DIO_Read(&inputSignal); // channels -> inputSignal
		if(inputSignal & 32 == 32){
			door_if.DIO_Write(4);
		} else {
			door_if.DIO_Write(5);
		}

		if(door_if.quit_doorcontrol_flag){
			return;
		}
	}
}



/* If "show_ui" of class DoorInterface is active use "External Tools" -> run in xterm"
 * to execute from Eclipse IDE */
int main (int argc, char *argv[])
{
	// ... insert your class initialisation and loop code here ...
	// example start:
	//int inputSignal;
	DoorControl control ;	
	//control.setOutput(2);
	control.run();

	return 0;
}
