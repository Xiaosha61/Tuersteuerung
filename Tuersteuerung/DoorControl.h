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
	Automatik  //3, s1=1&&s2=1
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

	void Set_All_Input();
	void set_mode();
	int get_mode();
	void ModeSwitch();

	void ProzessAusMode();
	void ReparaturMode();
	void AutomatikMode();
	void HandMode();

	void determineDoorCurrentState();

	void OpenDoor();
	void OpenDoorCarefully();
	void CloseDoor();
	void CloseDoorCarefully();
	void InitializeDoor();
	void getDoorCurrentState();

	void Fehler();
	void run();

  private:
	DoorInterface door_if;
	int doorCurrentState;
	int doorPreviousState;

	int inputSignal = 0;
	int mode;
	int S1, S2, E1, E2, X1, X2, X3, LS1, LS2, BE, B;
	//加上friend，来让外部的函数访问inputSignal
};

#endif // DOORCONTROL_HH
