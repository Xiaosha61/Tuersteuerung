#include <stdio.h>
#include <iostream>
#include <ncurses.h> // Header Datei für Bildschirmausgabe-Bibliothek (siehe 'man ncurses')
#include "ART3.h"	// Header Datei mit den Deklarationen für das E/A Geraet

/*
 * Der Simulationsmodus kann in der Headerdatei ART1.h mittels der
 * boolschen Variable SIMULATION an- oder ausgeschaltet werden!
 */

long int EingangsBits;

int Zustand, ZustandVorher;
int Betriebsart, BetriebsartVorher;
int Fehler, i;

const int ZPSTAUS = 0; // Prozessaus:  frozen
const int ZTzu = 1;
const int ZToeffnen = 2;
const int ZTauf = 3;
const int ZTwarten = 4;
const int ZTschliessen = 5;

const int BReparatur = 0;
const int BAuto = 1;
const int BHand = 2;
const int BFehler = 3;

int S1, S2, S3, E1, E2, E3, E4, X0, X1, X2, X3, X4, X5;

void Tuerkontrolle();
void AktualisiereEingangssignale();
void SchliesseTuer();
void OeffneTuer();
void Reparaturbetrieb();
void Automatikbetrieb();
void Handbetrieb();
void Abbruch();

int main()
{
	DIO_Init();
	mvprintw(4, 0, "Tuersteuerung aktiviert...");

	SchliesseTuer();
	Zustand = ZTzu;
	ZustandVorher = ZTzu;
	Fehler = 0;

	while (1)
	{
		if (Fehler == 1)
			break;
		if (getch() == 27)
			break;
		StartTimer(0.4);
		Tuerkontrolle();
	}

	DIO_End();
}

// Kontrolliert die Tuerfunktion
void Tuerkontrolle()
{
	AktualisiereEingangssignale(); //channel --> eingangsBits --> s1 s2, e1 e2, x0 x1 x2 x3 x4 x5.
	if ((S1 == 1) && (S2 == 0))
	{
		BetriebsartVorher = Betriebsart;
		Betriebsart = BReparatur;
		if (BetriebsartVorher != Betriebsart)
			mvprintw(5, 0, "Reparaturbetrieb - PST aus. ");

		Reparaturbetrieb();
	}
	if ((S1 == 0) && (S2 == 1))
	{
		BetriebsartVorher = Betriebsart;
		Betriebsart = BAuto;
		if (BetriebsartVorher != Betriebsart)
			mvprintw(5, 0, "Automatikbetrieb            ");

		Automatikbetrieb();
	}
	if ((S1 == 0) && (S2 == 0))
	{
		BetriebsartVorher = Betriebsart;
		Betriebsart = BHand;
		if (BetriebsartVorher != Betriebsart)
			mvprintw(5, 0, "Handbetrieb                 ");

		Handbetrieb();
	}
	if ((S1 == 1) && (S2 == 1))
	{
		BetriebsartVorher = Betriebsart;
		Betriebsart = BFehler;
		if (BetriebsartVorher != Betriebsart)
			mvprintw(5, 0, "Fehler - PST aus - Ende     ");

		Abbruch();
	}
	if (ZustandVorher != Zustand)
		mvprintw(6, 0, "Umstellen von %i auf %i.", ZustandVorher, Zustand);
}

// Aktualisiert die Eingangssignale
void AktualisiereEingangssignale()
{
	long int temp;
	DIO_Read(&EingangsBits);

	// S-Block
	temp = EingangsBits & 1;
	if (temp == 1)
		S1 = 1;
	else
		S1 = 0;
	temp = EingangsBits & 2;
	if (temp == 2)
		S2 = 1;
	else
		S2 = 0;

	// E-Block
	temp = EingangsBits & 4;
	if (temp == 4)
		E1 = 1;
	else
		E1 = 0;
	temp = EingangsBits & 8;
	if (temp == 8)
		E2 = 1;
	else
		E2 = 0;
	temp = EingangsBits & 16;
	if (temp == 16)
		E3 = 1;
	else
		E3 = 0;
	temp = EingangsBits & 32;
	if (temp == 32)
		E4 = 1;
	else
		E4 = 0;

	// X-Block
	temp = EingangsBits & 64;
	if (temp == 64)
		X0 = 1;
	else
		X0 = 0;
	temp = EingangsBits & 128;
	if (temp == 128)
		X1 = 1;
	else
		X1 = 0;
	temp = EingangsBits & 256;
	if (temp == 256)
		X2 = 1;
	else
		X2 = 0;
	temp = EingangsBits & 512;
	if (temp == 512)
		X3 = 1;
	else
		X3 = 0;
	temp = EingangsBits & 1024;
	if (temp == 1024)
		X4 = 1;
	else
		X4 = 0;
	temp = EingangsBits & 2048;
	if (temp == 2048)
		X5 = 1;
	else
		X5 = 0;
}
// Schliesst die Tuer mit aktivierung der Warnlampe
void SchliesseTuer()
{
	DIO_Write(6); // door is being closed
}

// Oeffnet die Tuer
void OeffneTuer()
{
	DIO_Write(1);
}

// Steuert die Tuer im Automatikbetrieb
void Automatikbetrieb()
{
	switch (Zustand)
	{
	case ZPSTAUS: // frozen
		// X0 = 0 (close); X1=0(open); X4 is safetySwitch(=X0)
		if ((X0 == 1) && (X1 == 1) && (X4 == 1)) // in the middle
		{
			ZustandVorher = Zustand;
			Zustand = ZToeffnen; // is being opened
		}
		if ((X0 == 0) && (X1 == 1) && (X4 == 0)) // on the side of X0
		{
			ZustandVorher = Zustand;
			Zustand = ZTzu; // is closed
		}
		if ((X0 == 1) && (X1 == 0) && (X4 == 1)) // on the side of X1
		{
			ZustandVorher = Zustand;
			Zustand = ZTauf; // is open
		}
		break;

	case ZTzu:
		if ((X2 == 0) || (X3 == 0)) // objectDetected
		{
			ZustandVorher = Zustand;
			Zustand = ZToeffnen;
		}
		else
		{
			ZustandVorher = Zustand;
			Zustand = ZTzu;
		}
		break;

	case ZToeffnen:					//is being opened
		if ((X1 == 0) && (X0 == 1)) // is already open
		{
			ZustandVorher = Zustand;
			Zustand = ZTauf; // is open
		}
		else // still opening.
		{
			OeffneTuer();
			ZustandVorher = Zustand;
			Zustand = ZToeffnen;
		}
		break;

	case ZTauf:
		ZustandVorher = Zustand;
		Zustand = ZTwarten;
		break;

	case ZTwarten:
		StartTimer(2.0);
		ZustandVorher = Zustand;
		Zustand = ZTschliessen;
		break;

	case ZTschliessen:
		if ((X2 == 0) || (X3 == 0) || (X5 == 0)) //object detected
		{
			OeffneTuer();
			ZustandVorher = Zustand;
			Zustand = ZToeffnen;
		}
		else // nobody
		{
			if ((X0 == 0) && (X1 == 1) && (X4 == 0)) // already closed
			{
				ZustandVorher = Zustand;
				Zustand = ZTzu;
			}
			else // still closing
			{
				SchliesseTuer();
				ZustandVorher = Zustand;
				Zustand = ZTschliessen;
			}
		}
		break;
	}
}

// Steuert die Tuer im Handbetrieb
void Handbetrieb()
{
	switch (Zustand)
	{
	case ZPSTAUS:
		if ((X0 == 1) && (X1 == 1) && (X4 == 1))
		{
			ZustandVorher = Zustand;
			Zustand = ZToeffnen;
		}
		if ((X0 == 0) && (X1 == 1) && (X4 == 0))
		{
			ZustandVorher = Zustand;
			Zustand = ZTzu;
		}
		if ((X0 == 1) && (X1 == 0) && (X4 == 1))
		{
			ZustandVorher = Zustand;
			Zustand = ZTauf;
		}
		break;

	case ZTzu:
		if ((E1 == 0) || (E3 == 0))
		{
			ZustandVorher = Zustand;
			Zustand = ZToeffnen;
		}
		else
		{
			ZustandVorher = Zustand;
			Zustand = ZTzu;
		}
		break;

	case ZToeffnen:
		if ((X1 == 0) && (X0 == 1))
		{
			ZustandVorher = Zustand;
			Zustand = ZTauf;
		}
		else
		{
			OeffneTuer();
			ZustandVorher = Zustand;
			Zustand = ZToeffnen;
		}
		break;

	case ZTauf:
		if ((E2 == 0) || (E4 == 0))
		{
			ZustandVorher = Zustand;
			Zustand = ZTschliessen;
		}
		break;

	case ZTwarten:
		ZustandVorher = Zustand;
		Zustand = ZTauf;
		break;

	case ZTschliessen:
		if ((X5 == 0) || (E1 == 0) || (E3 == 0))
		{
			OeffneTuer();
			ZustandVorher = Zustand;
			Zustand = ZToeffnen;
		}
		else
		{
			if ((X0 == 0) && (X1 == 1) && (X4 == 0))
			{
				ZustandVorher = Zustand;
				Zustand = ZTzu;
			}
			else
			{
				SchliesseTuer();
				ZustandVorher = Zustand;
				Zustand = ZTschliessen;
			}
		}
		break;
	}
}

// Steuert die Tuer im Reparaturbetrieb d.h. PST_AUS
void Reparaturbetrieb()
{
	ZustandVorher = Zustand;
	Zustand = ZPSTAUS;
	DIO_Write(0);
}

// Bei Fehler wird das Programm abgebrochen
void Abbruch()
{
	ZustandVorher = Zustand;
	Zustand = ZPSTAUS;
	DIO_Write(0);
	Fehler = 1;
}
