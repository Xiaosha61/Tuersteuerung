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

enum SIGNALSTATE
{
	activate,  //0
	inactivate //1
};

DoorControl::DoorControl() : door_if(false, true)
{
	door_if.SecondLevelInit();
}

DoorControl::~DoorControl()
{
	door_if.quit_doorcontrol_flag = true;
}

void DoorControl::Set_All_Input()
{
	door_if.DIO_Read(&inputSignal); //channel --> inputSignal
	if ((inputSignal & 1) == 0)
	{
		S1 = activate;
	}
	else
	{
		S1 = inactivate;
	}
	if ((inputSignal & 2) == 0)
	{
		S2 = activate;
	}
	else
	{
		S2 = inactivate;
	}
	if ((inputSignal & 4) == 0)
	{
		E1 = activate;
	}
	else
	{
		E1 = inactivate;
	}
	if ((inputSignal & 8) == 0)
	{
		E2 = activate;
	}
	else
	{
		E2 = inactivate;
	}
	if ((inputSignal & 16) == 0)
	{
		X1 = activate;
	}
	else
	{
		X1 = inactivate;
	}
	if ((inputSignal & 32) == 0)
	{
		X2 = activate;
	}
	else
	{
		X2 = inactivate;
	}
	if ((inputSignal & 64) == 0)
	{
		X3 = activate;
	}
	else
	{
		X3 = inactivate;
	}
	if ((inputSignal & 128) == 0)
	{
		LS1 = activate;
	}
	else
	{
		LS1 = inactivate;
	}
	if ((inputSignal & 256) == 0)
	{
		LS2 = activate;
	}
	else
	{
		LS2 = inactivate;
	}
	if ((inputSignal & 512) == 0)
	{
		BE = activate;
	}
	else
	{
		BE = inactivate;
	}
	if ((inputSignal & 1024) == 0)
	{
		B = activate;
	}
	else
	{
		B = inactivate;
	}
}

void DoorControl::set_mode() // determine mode according to [s1,s2]
{
	mode = 2 * S1 + S2;
}

int DoorControl::get_mode()
{
	return mode;
}

// 改变自动机的工作模式(Wechsel von verschiedenen Betrieben)
void DoorControl::ModeSwitch()
{
	Set_All_Input(); // channels --> [x1 x2 x3 s1 s2 ls1 ls2 BE B ]
	set_mode();		 // [s1,s2] -> mode

	switch (mode)
	{
	case PST_AUS:
		ProzessAusMode();
		break;
	case Reparatur:
		ReparaturMode();
		break;
	case Hand:
		HandMode();
		break;
	case Automatik:
		AutomatikMode();
		break;
	default:
		Fehler();
		break;
	}
}

void DoorControl::AutomatikMode()
{
	while (mode == Automatik)
	{

		Set_All_Input();
		set_mode();
	}
}

void DoorControl::HandMode()
{
}

void DoorControl::ReparaturMode()
{
}

void DoorControl::ProzessAusMode()
{
}

void DoorControl::Fehler()
{
}

void DoorControl::CloseDoor()
{
	door_if.DIO_Write(6); // door is being closed.
}

void DoorControl::OpenDoor()
{
	door_if.DIO_Write(1); // door is being opened.
}

void DoorControl::run()
{
	door_if.DIO_Write(0);
	while (1)
	{
		//door_if.StartTimer(0.2);
		// channels -> inputSignal
		//Read_S1_S2_Signal();
		//AutomatikMode();
		Set_All_Input(); // channels --> inputSignal --> X1 X2....
		if (X1 != 1)
		{
			cout << "X1=" << X1 << endl;
		}
		ModeSwitch(); // determine mode and switch.

		if (door_if.quit_doorcontrol_flag)
		{
			return;
		}
	}
}

/* If "show_ui" of class DoorInterface is active use "External Tools" -> run in xterm"
 * to execute from Eclipse IDE */
int main(int argc, char *argv[])
{

	// ... insert your class initialisation and loop code here ...
	// example start:
	DoorControl control;

	control.run();

	return 0;
}
