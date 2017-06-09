//charlines.h

#ifndef _CHARLINES_H
#define _CHARLINES_H

#include "string.h"
#include "stdio.h"

class CCharLines
{		
public:
	CCharLines();
	~CCharLines();

public:
	//Initialize a string with the new data. The old string stored in 
	//the p_CharLine will be deleted
	bool         AssignData(int m_NSymbols, const char* p_Data);
	//The same, but here we can replace p_CharData with a m_NSymbols 
	//symbols beginnig  m_NFrom symbol of the p_Data
	bool	     AssignData(CCharLines* p_Data, int m_NFrom, int m_NSymbols);

	//This will add m_NSymbols to the end of the string
	bool         AppendData(int m_NSymbols, const char* p_Data);

	//Possible to empty all the string from outside call
	void         DeleteData();

	//Here I don't use '\0' symbol, so a string may containe a lot of them
	//If it happens - this function will show just a furst part of the string - ugly standart of printf();
	//Don't want to modify a string by the deleting of the all '\0' symbols that could happen there.
	bool         ShowData();

	//Important! The p_DestArray should be initialized before calling of this function
	//and have a length m_NSymbols. If this not happen - that the function fail
	bool         GetData(char* p_Dest, int m_NFrom, int m_NSymbols);

	//The nubers can be from 1 to m_NLine symbols - I haven't reached a standart it such 
	//questions yet, so - better look at the code of this function
	bool 	     GetSymbol(char& m_Symbol, int m_NSymbol);

	//Returns the total number of symbols (including '\0')
	int GetLineSize();
public:
	//This is for the possebility to make a FIFO from this CCharLines classes.
	CCharLines*  p_Next;

private:
	//Data
	char*        p_CharLine;
	//Total number of the symbols (chars)
	int m_NLineSymbols;
};

#endif
