/*
 * Praktikum MRT2 
 * ART3 Türsteuerung
 * Institut für Automatisierungstechnik
 * letztes Update Mai 2017
 * Autor: M.Herhold
 */

#ifndef DOORCONTROL_HH
#define DOORCONTROL_HH

#include "ncurses.h"
#include "DoorInterface.h"


class DoorControl {
public:
	DoorControl();
	~DoorControl();
	
	
	int inputSignal;
	//int S;
	void run();
	void setInput(int *eingang);
	void setOutput(int ausgang);
	int getSignal();
private:
	DoorInterface door_if;
	
	
};


#endif // DOORCONTROL_HH
