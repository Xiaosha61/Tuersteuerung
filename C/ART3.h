/* 
* ART3.h deklariert Funktionen zur Benutzung des Timers und des
* E/A Geraetes.
*/

/*
 *  Simulation an- oder ausschalten
 */
const bool SIMULATION = true;

/* 
 * Startet den Timer und wartet seconds Sekunden.
 * (Richtwert: 0.1 ... 0.5 s)
 * Werte darunter fuehren je nach Rechner zum Ueberlauf.
 */
int StartTimer (double seconds);

/* 
 * Startet den Timer und programmiert ihn auf das Zeitintervall
 * seconds (in Sekunden) und ordnet dem Timer-Signal die Handler-
 * Funktion void handler(int) zu. Diese wird periodisch bei jedem
 * Timer-Ueberlauf aufgerufen und hat als Parameter einen Integer, der
 * zur Unterscheidung verschiedener Signale verwendet werden kann.
 * Dieser Parameter muss angegeben werden, auch wenn Sie ihn fuer Ihr
 * Programm nicht benoetigen. 
 */
int StartTimerWithISR (double seconds, void (*handler)(int) );

/*
 * Wartet, bis der Benutzer eine Taste + Enter drueckt.
 */
char WaitForUserInput (void);

/* 
 * Initialisiert das Geraet und setzt alle Ausgaenge auf 0 und
 * kann zu Beginn des Programms aufgerufen werden
 */
void DIO_Init (void);

/*
 * Liest die Eingabe-Kanaele der Karte.
 * Die Kanaele sind den Bits von in_channels direkt zugeordnet,
 * d.h. Kanal 0 liegt auf Bit 0 (niederwertigstes Bit), usw.
 */
void DIO_Read (long int *in_channels);

/*
 * Schreibt alle Ausgabe Kanaele auf die Digital-EA-Karte.
 * Die Kanaele sind den Bits von out_channels direkt zugeordnet,
 * d.h. Kanal 0 liegt auf Bit 0 (niederwertigstes Bit), usw.
 */
void DIO_Write (long int out_channels);

/*
 * Sollte bei Beendung des Programms ausgefuehrt werden und
 * schaltet xterm aus dem grafischen (der fuer die Simulation
 * verwendet wird), in den normalen Modus zurueck.
 */
void DIO_End(void);
