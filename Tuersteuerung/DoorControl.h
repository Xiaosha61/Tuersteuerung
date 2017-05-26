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

enum MODE
{
	PST_AUS,   //0
	Reparatur, //1
	Hand,	  //2
	Automatik  //3
};

enum DOORSTATE
{
	ZTAuf,		 //0
	ZTZu,		 //1
	ZTOeffnen,   //2
	ZTSchliessen //3
};

class DoorControl
{
  public:
	DoorControl();
	~DoorControl();

	void Read_S1_S2_Signal();
	void set_S1_S2_Signal();
	int get_S1_S2_Signal();
	void ModeSwitch();
	void AutomatikMode();
	void OpenDoorAndWait();
	int TuerCloseBeurteilen();
	void CloseDoorWithCondition();
	int IfThereIsSth();
	void HandMode();
	void ReparaturMode();
	void ProzessAusMode();
	void CloseDoor();
	void OpenDoor();
	void Fehler();
	void run();

  private:
	DoorInterface door_if;
	DOORSTATE doorCurrentState;
	DOORSTATE doorPreviousState;

	int inputSignal = 0;
	MODE mode;
	int S1, S2, E1, E2, X1, X2, X3, LS1, LS2, BE, B;
	//加上friend，来让外部的函数访问inputSignal
};

#endif // DOORCONTROL_HH
