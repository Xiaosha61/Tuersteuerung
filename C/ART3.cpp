#include "Libraries/NIDAQmxBase.h"			// Standard NI-Bibliothek
#include "Libraries/NIDAQmxBaseAPI.h"			// Angepasste API
#include "ART3.h"

#include <stdio.h>
#include <iostream>
#include <signal.h>
#include <time.h>
#include <ncurses.h>

// hier speichern wir die E/A-Kanaele für die Simulation
static long channels = 0;

void Init_Ncurses() {
	initscr(); // initialisiere die curses Bibliothek
	keypad(stdscr, TRUE); // schalte das Tastatur-Mapping ein
	raw(); // take input chars one at a time, no wait for \n
	curs_set(0); //Cursor ausschalten
	noecho(); // echo input
	nodelay(stdscr, TRUE);
	start_color();
	init_pair(1, COLOR_BLUE, COLOR_YELLOW);
	init_pair(2, COLOR_RED, COLOR_YELLOW);
}

void showSim() {
	//Cursorposition merken
	int old_x, old_y;
	getsyx(old_y, old_x);

	attron(COLOR_PAIR(1));
	mvprintw(0, 1, " 00 01 02 03 04 05 06 07 08 09 10 11      ");

	move(1, 1);
	for (int i=0; i<12; i++)
		printw(" %ld ", (channels>>i)&1);
	printw("     ");

	attron(COLOR_PAIR(2));
	mvprintw(0, 42, "   00 01 02 03 04 05 06 07 ");

	mvprintw(1, 42, "  ");
	for (int i=12; i<20; i++)
		printw(" %ld ", (channels>>i)&1);
	printw(" ");
	attroff(COLOR_PAIR(2));

	// Cursor zurücksetzen
	if (old_y <2)
		old_y =2;
	move(old_y, old_x);
	refresh();
}

void SwitchBitByKey(int c) {
	int chan, cbit;

	// mvprintw(10, 10, "%i ", c); //Debug
	if (c >= 265 && c <= 276) {
		chan = c-265;
		cbit = (channels>>chan)&1; // ist der Channel z.Zt. gesetzt?
		if (cbit)
			channels = channels ^ (0x1L<<chan);
		else
			channels = channels | (0x1L<<chan);
	}

	// Shift
	if (c>=80 && c<=83) {
	//	mvprintw(10, 10, "Shift1"); //Debug
		chan = c-80+12;
		cbit = (channels>>chan)&1; // ist der Channel z.Zt. gesetzt?
		if (cbit)
			channels = channels ^ (0x1L<<chan);
		else
			channels = channels | (0x1L<<chan);
	}
	if (c>=277 &&c<=288) {
	//	mvprintw(10, 10, "Shift2"); //Debug
		chan = c-277+12;
		cbit = (channels>>chan)&1; // ist der Channel z.Zt. gesetzt?
		if (cbit)
			channels = channels ^ (0x1L<<chan);
		else
			channels = channels | (0x1L<<chan);
	}

	showSim();
}

void DIO_End() {
	if (SIMULATION)
		endwin();
}

int StartTimer(double seconds) {

	// Der Teil vor dem Komma
	int vorDemKomma = (int)seconds;

	// Der Teil nach dem Komma in Nanosekunden
	int nachDemKomma = (int)((seconds - (double)vorDemKomma) * 1000000000) ;

	// Speichert den aktuellen Stand des Timers
	int akt_s, akt_ns = 0;

	// Datenstruktur zur näheren Klassifikation eines „Signalereignisses“
	struct sigevent ereignis;

	// Der Timer (ID des Timers)
	timer_t wecker_id;

	// cout << "s: " << vorDemKomma << "\n";
	// cout << "ns:" << nachDemKomma << "\n";
	// cout << "seconds:" << seconds << "\n";

	// wecker_sett: 	nimmt Startzeitpunkt und die Wiederholunsperiode auf
	// wecker_status:	nimmt die aktuelle Position des Timers auf
	struct itimerspec wecker_sett, wecker_status;

	// Dauer der ersten Runde nach starten des Timers
	wecker_sett.it_value.tv_sec = vorDemKomma; // Sekunden
	wecker_sett.it_value.tv_nsec = nachDemKomma; // Nanosekunden

	// Dauer der nachfolgenden Runden (beides 0: Timer läuft nur einmal)
	wecker_sett.it_interval.tv_sec = 0; // Sekunden
	wecker_sett.it_interval.tv_nsec = 0; //Nanosekunden

	// Das Signalereignis wird initialisiert und dann ausgeschaltet
	ereignis.sigev_value.sival_int = 0; // initialisieren
	ereignis.sigev_notify = SIGEV_NONE; // ausschalten


	// Den Timer erstellen
	timer_create(CLOCK_REALTIME, &ereignis, &wecker_id);

	// In wecker_sett gespeicherte Einstellungen anwenden
	timer_settime(wecker_id, 0, &wecker_sett, NULL);

	// Abfragen, ob die Zeit bereits abgelaufen ist
	do {
		timer_gettime(wecker_id, &wecker_status);
		akt_s = wecker_status.it_value.tv_sec;
		akt_ns = wecker_status.it_value.tv_nsec;
		// Debug
		// printw("Zeitpunkt: s.ns %i.%i\n",akt_s,akt_ns);
	} while (akt_s > 0 || akt_ns > 0);

	// Timer löschen
	timer_delete(wecker_id);
	if (SIMULATION) {
		SwitchBitByKey(getch());
	}

	return 0;
}

int StartTimerWithISR(double seconds, void (*handler)(int)) {

	// Der Teil vor dem Komma
	int vorDemKomma = (int)seconds;

	// Der Teil nach dem Komma in Nanosekunden
	int nachDemKomma = (int)((seconds - (double)vorDemKomma) * 1000000000) ;

	// Datenstruktur zur näheren Klassifikation eines „Signalereignisses“
	struct sigevent isr_ereignis;

	// Datenstruktur zur Anmeldung von Signalbehandlungen
	struct sigaction isr_sig_aktion;

	// Der Timer (ID des Timers)
	timer_t isr_wecker_id;

	// wecker_sett: 	nimmt Startzeitpunkt und die Wiederholunsperiode auf
	// wecker_status:	nimmt die aktuelle Position des Timers auf
	struct itimerspec isr_wecker_sett;

	// signal declarations
	//sigset_t newmask,oldmask;

	// Dauer der ersten Runde nach starten des Timers
	isr_wecker_sett.it_value.tv_sec = vorDemKomma; // Sekunden
	isr_wecker_sett.it_value.tv_nsec = nachDemKomma; // Nanosekunden

	// Dauer der nachfolgenden Runden (beides 0: Timer läuft nur einmal)
	isr_wecker_sett.it_interval.tv_sec = vorDemKomma; // Sekunden
	isr_wecker_sett.it_interval.tv_nsec = nachDemKomma; // Nanosekunden

	// Das Signalereignis wird initialisiert und dann das zu sendene Signal eingestellt
	isr_ereignis.sigev_value.sival_int = 0; // spezifiziert einen anwendungsdefinierten Wert, der bei der
	// Auslieferung übergeben wird
	isr_ereignis.sigev_signo = SIGUSR1; // dieses Signal senden
	isr_ereignis.sigev_notify = SIGEV_SIGNAL; // Signalisieren einschalten

	sigemptyset(&isr_sig_aktion.sa_mask); // Signalset erstellen
	isr_sig_aktion.sa_handler = handler; // diese Funktion aufrufen wenn das Signal kommt
	isr_sig_aktion.sa_flags = 0; // keine zusätzlichen Flags

	sigaction(SIGUSR1, &isr_sig_aktion, 0);

	// Den Timer erstellen
	timer_create(CLOCK_REALTIME, &isr_ereignis, &isr_wecker_id);

	// In wecker_sett gespeicherte Einstellungen anwenden
	timer_settime(isr_wecker_id, 0, &isr_wecker_sett, NULL);

	return 1;
}

char WaitForUserInput(void) {
	char Taste = -1;

	while (Taste == -1)
		Taste = getch();
	return Taste;
}

void DIO_Init(void) {
	if (!SIMULATION)
		NIUSB6008::resetAllOutputs();
	else {
		Init_Ncurses();
		showSim();
	}
}

void DIO_Read(long int *in_channels) {

	if (!SIMULATION){
		*in_channels = NIUSB6008::readDigitalPort('0');
		*in_channels = *in_channels + (NIUSB6008::readDigitalPort('1') << 8);
	}
	else if (SIMULATION)
		*in_channels = channels & 0x0000FFFFL;

}

void DIO_Write(long int out_channels) {
	if (!SIMULATION) {

		float64 ao[2] = { 0, 0 };

		//out_channels = out_channels && 0x03L;

		if (out_channels & 0x1)
			ao[0] = 5.00;
		else
			ao[0] = 0.00;

		if (out_channels & 0x2)
			ao[1] = 5.00;
		else
			ao[1] = 0.00;

		//printf("ao0: %f - ao1: %f\n", ao[0], ao[1]); //Debug
		NIUSB6008::writeAnalog('0', ao[0]);
		NIUSB6008::writeAnalog('1', ao[1]);

	} else if (SIMULATION) {
		channels = channels & 0x00000FFFL;
		channels = channels | ((out_channels&0xFF)<<12);
		showSim();
	}

}
