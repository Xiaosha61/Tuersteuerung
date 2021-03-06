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
		determineDoorCurrentState(); // determine doorCurrentState according to current X1x2x3
		switch (doorCurrentState)
		{
		case ZTZu:
			if ((!LS1) || (!LS2) || (!BE) || (!B) == 1) //object detected.
			{
				OpenDoorCarefully();
			}
			break;

		case ZTOeffnen:
			OpenDoorCarefully();
			break;

		case ZTAuf:
			for (int i = 0; i < 100; i++) // sleep 10s.
			{
				door_if.StartTimer(0.1);
			}
			doorPreviousState = doorCurrentState;
			doorCurrentState = ZTSchliessen;
			CloseDoorCarefully();
			break;

		case ZTSchliessen:
			while (X2 || X3 == 1) // still not closed.
			{
				if ((!LS1) || (!LS2) || (!BE) || (!B) == 1) // object detected.
				{
					doorPreviousState = doorCurrentState;
					doorCurrentState = ZTOeffnen;
					OpenDoorCarefully();
					break;
				}
				CloseDoorCarefully();
			}
		}

		set_mode();
		if (door_if.quit_doorcontrol_flag)
		{
			break;
		}
	}
}

void DoorControl::determineDoorCurrentState()
{
	if (X1 == 0 && X2 == 1 && X3 == 1)
	{
		doorCurrentState = ZTAuf;
	}
	if (X1 == 1 && X2 == 0 && X3 == 0)
	{
		doorCurrentState = ZTZu;
	}
	if (X1 == 1 && X2 == 1 && X3 == 1)
	{
		if (doorPreviousState == ZTAuf)
		{
			doorCurrentState = ZTSchliessen;
		}
		else if (doorPreviousState == ZTZu || doorPreviousState == ZTSchliessen)
		{
			doorCurrentState = ZTOeffnen;
		}
		else
		{
			Fehler();
		}
	}
}

void DoorControl::HandMode()
{
	while (mode == Hand)
	{

		Set_All_Input();
		set_mode();
	}
}

void DoorControl::ReparaturMode()
{
	while (mode == Reparatur)
	{

		Set_All_Input();
		set_mode();
	}
}

void DoorControl::ProzessAusMode()
{
	while (mode == PST_AUS)
	{

		Set_All_Input();
		set_mode();
	}
}

void DoorControl::Fehler()
{
}

void DoorControl::CloseDoorCarefully()
{
	while (1)
	{
		Set_All_Input();
		if ((!LS1) || (!LS2) || (!BE) || (!B) == 1) //object detected.
		{
			OpenDoorCarefully();
			doorPreviousState = ZTSchliessen;
			doorCurrentState = ZTOeffnen;
			break;
		}
		if (X1 == 1 && X2 == 0 && X3 == 0) // door is completely closed.
		{
			door_if.DIO_Write(0); // stop the LED and Motor
			break;
		}
		CloseDoor();
	}
}

void DoorControl::CloseDoor()
{
	door_if.DIO_Write(6); // keep LED and motor on.
}

void DoorControl::OpenDoorCarefully()
{
	while (1)
	{
		Set_All_Input();
		if (X1 == 0 && X2 == 1 && X3 == 1) // door is completely opened
		{
			door_if.DIO_Write(0); // stop Motor
			doorPreviousState = ZTOeffnen;
			doorCurrentState = ZTAuf;
			break;
		}
		OpenDoor();
	}
}

void DoorControl::OpenDoor()
{
	door_if.DIO_Write(1); // keep motor on
}

void DoorControl::InitializeDoor()
{
	Set_All_Input(); // should have read ffff if it's simulating.

	while ((X2 || X3 == 1) || (X1 != 1))
	// only when X1X2X3 = 100(door is completely closed), end the loop.
	//otherwise, it's not a good start point.
	{
		CloseDoor();
		Set_All_Input();
	}
	door_if.DIO_Write(0);
}

void DoorControl::run()
{
	door_if.DIO_Write(0); //Ruhelagen
	doorCurrentState = ZTZu;
	doorPreviousState = ZTZu;
	InitializeDoor(); // so that the door is really closed.

	Set_All_Input(); // channels --> inputSignal --> X1 X2....
	ModeSwitch();	// determine mode and switch.
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
