//parser.h

#ifndef _DATAFILE_H
#define _DATAFILE_H

#include "string.h"
#include "stdio.h"

#include "iostream"

#include "charlines.h"
#include "2dmatrix.h"



class CParser
{
public:

         CParser();
        ~CParser();

public:

	bool ReadFile(const char* p_FileName);

	bool ParseData();

        bool GetMatrix(int m_Index,
                       C2DMatrix* p_Dest,
                       int m_RawFrom,
                       int m_NRaws,
                       int m_ColumnFrom,
                       int m_NColumns);

        bool GetMatrix(int m_Index,
                       long double** pp_Matrix,
                       int  m_RawFrom,
                       int  m_NRaws,
                       int  m_ColumnFrom,
                       int  m_NColumns);

        bool GetNMatrices(int& m_NMatrices);

        bool GetMatrixDimensions(int m_Index,
                                 int &m_NRaws,
                                 int &m_NColumns);


private:

	void DeleteCharLines(CCharLines* p_Head);
	void DeleteDataBlocks(C2DMatrix* p_Head);


	void ShowCharLines(CCharLines* p_Head);
	void ShowDataBlocks(C2DMatrix* p_Head);

	bool SetIndex();

        bool ParseLine(CCharLines* p_Line, C2DMatrix* p_Numbers);

        bool ParseWord(CCharLines* p_Word, C2DMatrix* p_Numbers);

	bool ParseNumber(const char* string, long double& var);

	bool  CheckNumber(const char m_Char);
	bool  CheckDelimiter(const char m_Char);
	bool  CheckReturn(const char m_Char);

private:
	CCharLines* p_CharLinesHead;
	C2DMatrix* p_DataBlocksHead;
};

#endif
