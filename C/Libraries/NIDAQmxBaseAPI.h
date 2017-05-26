/*********************************************************************
*
* Beschreibung:
*		API für die DAQmxBase*() Funktionen zur einfachen Bedienung
*		des National Instruments USB-6008 Gerätes
*
* Autor:
*		Johannes Pfeffer
*
* Anweisungen für den Betrieb:
*		1. Dem Compiler müssen die optionen "-lnidaqmxbase" und "-lrt"
* 		übergeben werden.
*		2. ERST das Programm starten, dann den Versuchsstand einschalten,
*		die analogen Ausgänge könnten vor Programmstart noch auf Spannung
*		geschaltet sein.
*
*********************************************************************/

namespace NIUSB6008 {

	/*
	* Setzt die Bits data (z.B. 0xFF) auf dem digitalen Port portNr.
	*
	* Es stehen 2 digitale Ports zur Verfügung ('0' und '1')
	* Port 0 ist 8 Bit und Port 1 ist 4 Bit breit.
	*/
	void writeDigitalPort(uInt8 data, char portNr);

	/*
	* Liest die Daten des digitalen Ports portNr ein. 
	* Das Ergebnis des Lesevorganges ist der Rückgabewert der Funktion.
	*
	* Es stehen 2 digitale Ports zur Verfügung ('0' und '1')
	* Port 0 ist 8 Bit und Port 1 ist 4 Bit breit.
	*/
	uInt32 readDigitalPort(char portNr);

	/*
	* Liest die Spannung auf dem analogen Eingang eingangNr ein.
	* Das Ergebnis des Lesevorganges ist der Rückgabewert der Funktion.
	*
	* Es stehen 8 analoge Eingänge zur Verfügung ('1' bis '7')
	*/
	float64 readAnalog(char eingangNr);

	/*
	* Gibt die Spannung spannung auf dem analogen Ausgang ausgangNr aus.
	* Es stehen 2 analoge Ausgänge zur Verfügung ('0' und '1')
	*/
	void writeAnalog(char ausgangNr, float64 data);

	/*
	* Alle Ausgänge werden auf Null gesetzt und das Gerät
	* in den Ausgangszustand gebracht. Sollte ganz am Ende 
	* des Programms stehen.
	*/
	void USB6008_Close();
	
	/*
	* Alle Ausgänge werden auf Null gesetzt.
	*/
	void resetAllOutputs();

	/*
	* Fehlerroutine: Der Fehlerbuffer wird ausgegeben.
	* Die Konstante exitBeiFehler = 0|1 legt fest ob sich das Programm bei
	* einem Fehler beenden soll.
	*/
	const int exitBeiFehler = 1; 
	int DAQmxErrChk(int32 rueckgabeWert);
	
}
