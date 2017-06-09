//charlines.cpp

#include "charlines.h"


CCharLines::CCharLines()
{
	p_CharLine = NULL;
	m_NLineSymbols = 0;
	p_Next = NULL;
}

CCharLines::~CCharLines()
{
	DeleteData();
}

bool CCharLines::AssignData(int m_NSymbols, const char* p_Data)
	{
	if((p_Data == NULL) || (m_NSymbols == 0)) return false;

	DeleteData();

	p_CharLine = new char[m_NSymbols];

	if(p_CharLine == NULL) return false;

	memset(p_CharLine, '\0', m_NSymbols);

	if(memcpy(p_CharLine,p_Data,m_NSymbols*sizeof(char)) == NULL)
	{
		delete[] p_CharLine;
		p_CharLine = NULL;
		return false;
	}

	m_NLineSymbols = m_NSymbols;

	return true;
}

bool CCharLines::AssignData(CCharLines* p_Data, int m_NFrom, int m_NSymbols)
{
	if(p_Data == NULL) return false;

	if((m_NFrom > p_Data->GetLineSize()) || 
	   (m_NSymbols == 0) || ( (m_NFrom + m_NSymbols) > p_Data->GetLineSize()) ) return false;

	char* p_NewCharLine = new char[m_NSymbols];
	if(p_NewCharLine == NULL) return false;

	if(!p_Data->GetData(p_NewCharLine, m_NFrom, m_NSymbols))
	{
		delete[] p_NewCharLine;
		return false;
	}

	DeleteData();
	p_CharLine = p_NewCharLine;
	m_NLineSymbols = m_NSymbols;

	return true;
}

bool CCharLines::AppendData(int m_NSymbols, const char* p_Data)
{
	if((p_Data == NULL) || (m_NSymbols == 0)) return false;

	if((p_CharLine == NULL) || (m_NLineSymbols == 0))
		return AssignData(m_NSymbols, p_Data);

	const int m_NewNSymbols = m_NLineSymbols + m_NSymbols;
	char *p_NewCharLine = NULL;

	p_NewCharLine = new char[m_NewNSymbols];
	if(p_NewCharLine == NULL) return false;

	memset(p_NewCharLine,'\0', m_NewNSymbols);
			
	if (memcpy(&p_NewCharLine[0],p_CharLine,m_NLineSymbols*sizeof(char)) == NULL)
	{
		delete[] p_NewCharLine;
		return false;
	}

	if (memcpy(&p_NewCharLine[m_NLineSymbols],p_Data,m_NSymbols*sizeof(char)) == NULL)
	{
		delete[] p_NewCharLine;
		return false;
	}

	delete[] p_CharLine;
	p_CharLine = p_NewCharLine;
	m_NLineSymbols = m_NewNSymbols;

	return true;
}

bool CCharLines::ShowData()
{
	//This function will partially fail, if inside of the Data exist '\0'

	if(p_CharLine == NULL) return false;

	char * p_String = new char[m_NLineSymbols + 1];

	if(p_String == NULL) return false;

	p_String[m_NLineSymbols] = '\0';
	memcpy(p_String, p_CharLine, m_NLineSymbols);

	printf("%s \n", p_String);

	delete[] p_String;
	return true;
}

bool CCharLines::GetData(char* p_Dest, int m_NFrom, int m_NSymbols)
{
	if(p_Dest == NULL) return false;

	if( (m_NFrom > m_NLineSymbols) || 
	    (m_NSymbols == 0) || ((m_NFrom + m_NSymbols) > m_NLineSymbols) ) return false;

	if(memcpy(p_Dest, &p_CharLine[m_NFrom], m_NSymbols*sizeof(char)) == NULL) return false;

	return true;
}

bool CCharLines::GetSymbol(char& m_Symbol, int m_NSymbol)
{
	if(m_NSymbol >= m_NLineSymbols) return false;
	m_Symbol = p_CharLine[m_NSymbol];
	return true;
}

int CCharLines::GetLineSize()
{
	return m_NLineSymbols;
}


void CCharLines::DeleteData()
{
	if(p_CharLine == NULL) return;

	delete[] p_CharLine;

	p_CharLine = NULL;
	m_NLineSymbols = 0;
	return;
}
