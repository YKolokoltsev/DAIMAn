//datafile.cpp

//#define DEBUG

#include "parser.h"

CParser::CParser()
{
	p_CharLinesHead = NULL;
	p_DataBlocksHead = NULL;
}

CParser::~CParser()
{		
	if(p_CharLinesHead != NULL)
	{
		DeleteCharLines(p_CharLinesHead);
		p_CharLinesHead = NULL;
	}

	if(p_DataBlocksHead != NULL)
	{
		DeleteDataBlocks(p_DataBlocksHead);
		p_DataBlocksHead = NULL;
	}
}

void CParser::DeleteCharLines(CCharLines* p_Head)
{
	if(p_Head == NULL) return;

	CCharLines *p_Current = NULL, 
	           *p_Next = NULL;

	p_Current = p_Head;

	while(p_Current != NULL)
	{
		p_Next = p_Current->p_Next;
		delete p_Current;
		p_Current = p_Next;
	}

	return;
}

void CParser::DeleteDataBlocks(C2DMatrix* p_Head)
{
	if(p_Head == NULL) return;

	C2DMatrix *p_Current = NULL,
		  *p_Next = NULL;

	p_Current = p_Head;

	while(p_Current != NULL)
	{
		p_Next = p_Current->p_Next;
		delete p_Current;
		p_Current = p_Next;
	}

	return;
}

void CParser::ShowCharLines(CCharLines* p_Head)
{
	if(p_Head == NULL) return;

	CCharLines *p_Current = p_Head;

	while(p_Current != NULL)
	{
		p_Current->ShowData();	
		p_Current = p_Current->p_Next;
	}

	return;
}

void CParser::ShowDataBlocks(C2DMatrix* p_Head)
{
	if(p_Head == NULL) return;

	C2DMatrix *p_Current = p_Head;

	int i = 0;

	while(p_Current != NULL)
	{
		printf("m_BlockNum: %d	",p_Current->m_BlockNum);
		printf("m_IndexNum: %d\n", p_Current->m_IndexNum);
		p_Current->ShowMatrix();
		p_Current = p_Current->p_Next;
		if(p_Current != NULL) printf("\n\n");
		i++;
	}

	return;
}

bool CParser::ReadFile(const char* p_FileName)
{

	if(p_FileName == NULL) return false;

	FILE* p_DataFile = NULL;

	if((p_DataFile = fopen(p_FileName,"r")) == NULL)
	{
		printf("Error opening file.\n");
		return false;
	}

	const int m_NumItems = 1024*100;
	char * p_DataBigBlock = new char[m_NumItems];
	if(p_DataBigBlock == NULL) return false;
	memset(p_DataBigBlock,'\0',m_NumItems*sizeof(char));

	int m_NumItemsRed = 0;
	bool         m_RetVal = true;

	int i = 0,
	             j = 0;

	DeleteCharLines(p_CharLinesHead);
	p_CharLinesHead = NULL;

	p_CharLinesHead = new CCharLines();
	if(p_CharLinesHead == NULL) m_RetVal = false;

	CCharLines *p_Current = p_CharLinesHead;
	CCharLines *p_Last = NULL;

        while((feof(p_DataFile) == 0) && m_RetVal)
	{
		m_NumItemsRed = fread(p_DataBigBlock,sizeof(char),m_NumItems,p_DataFile);

		if(ferror(p_DataFile) != 0)
		{
			m_RetVal = false;
			break;
		}

		i = 0; 
		j = 0;

		while( (i < m_NumItemsRed) && m_RetVal)
		{
			if(CheckReturn(p_DataBigBlock[i]))
			{
				
				if( !(i <= j) )
				{
					if(!p_Current->AppendData(i - j, &p_DataBigBlock[j]))
					{
						m_RetVal = false;
						break;
					}
				}

				j = i + 1;

				p_Current->p_Next = new CCharLines();

				if(p_Current->p_Next == NULL)
				{
					m_RetVal = false;
					break;
				}

				p_Last = p_Current;
				p_Current = p_Current->p_Next;
			}

			i++;
		}

                if( (j < m_NumItemsRed) && m_RetVal )
		{
                        p_Current->AppendData(m_NumItemsRed - (j + 1), &p_DataBigBlock[j]);
		}	
	}

	if(!m_RetVal)
	{
		DeleteCharLines(p_CharLinesHead);
		p_CharLinesHead = NULL;
	}

	fclose(p_DataFile);

   //     delete p_Last->p_Next;
   //     p_Last->p_Next = NULL;

	delete[] p_DataBigBlock;

	return m_RetVal;
}

bool CParser::ParseData()
{
	if(p_CharLinesHead == NULL) return false;
	CCharLines *p_CurrentCharLine = p_CharLinesHead;

	DeleteDataBlocks(p_DataBlocksHead);
	p_DataBlocksHead = NULL;
	C2DMatrix* p_CurrentDataBlock = NULL;
	p_CurrentDataBlock = new C2DMatrix();

	if(p_CurrentDataBlock == NULL) return false;
	p_DataBlocksHead = p_CurrentDataBlock;

	C2DMatrix* p_Numbers = NULL;

	bool	     m_Success = true;
	bool         m_RetVal = true;

	while((p_CurrentCharLine != NULL) && m_RetVal)
	{
		m_Success = true;

		if(p_CurrentCharLine->GetLineSize() != 0)
		{
			if((p_Numbers = new C2DMatrix()) == NULL)
			{
				m_RetVal = false;
				break;
			}

			m_Success = ParseLine(p_CurrentCharLine, p_Numbers);

			if(m_Success)
			{
				p_Numbers->TransposeMatrix();

				if( (p_CurrentDataBlock->GetNColumns() != p_Numbers->GetNColumns()) &&
				    (p_CurrentDataBlock->GetNColumns() != 0) )
				{
					p_CurrentDataBlock->p_Next = new C2DMatrix();
					if(p_CurrentDataBlock->p_Next == NULL)
					{
						m_RetVal = false;
						break;
					}

					p_CurrentDataBlock = p_CurrentDataBlock->p_Next;

					if(!p_CurrentDataBlock->AppendMatrixRaw(p_Numbers))
					{
						m_RetVal = false;
						break;
					}
				}
				else
				{
					if(!p_CurrentDataBlock->AppendMatrixRaw(p_Numbers))
					{
						m_RetVal = false;
						break;
					}
				}
			}

			delete p_Numbers;
			p_Numbers = NULL;
		}
		else
		{
			m_Success = false;
		}

		if(!m_Success)
		{
			p_CurrentDataBlock->p_Next = new C2DMatrix();
			if(p_CurrentDataBlock->p_Next == NULL)
			{
				m_RetVal = false;
				break;
			}
			p_CurrentDataBlock = p_CurrentDataBlock->p_Next;
		}

		p_CurrentCharLine = p_CurrentCharLine->p_Next;
	}
	
        if(!m_RetVal)
	{
		DeleteDataBlocks(p_DataBlocksHead);
		p_DataBlocksHead = NULL;
	}
	else
	{
		m_RetVal = SetIndex();
	}

    return m_RetVal;
}




bool CParser::ParseLine(CCharLines* p_Line, C2DMatrix* p_Numbers)
{
	if((p_Line == NULL) || (p_Numbers == NULL)) return false;

	if(p_Numbers->GetNColumns() > 1) return false;

	CCharLines* p_WordsHead = new CCharLines();
	if(p_WordsHead == NULL) return false;
	CCharLines* p_WordsCurrent = p_WordsHead;

	char m_CurSymbol = '\0';
	bool m_RetVal = true;

	for(int i = 0; i <= p_Line->GetLineSize(); i++)
	{
		m_CurSymbol = '\0';
		p_Line->GetSymbol(m_CurSymbol,i);

		if( CheckDelimiter(m_CurSymbol) )
		{
			p_WordsCurrent->p_Next = new CCharLines();
			if(p_WordsCurrent->p_Next == NULL)
			{
				m_RetVal = false;
				break;
			}

			if(p_WordsCurrent->GetLineSize() != 0)
			{
				//Here you can add an unparsed numbers work-out
				if(!ParseWord(p_WordsCurrent, p_Numbers))
				{
					printf("This word is unparseble: \n");
					p_WordsCurrent->ShowData();
				};
			}

			p_WordsCurrent = p_WordsCurrent->p_Next;
		
		}
		else
		{
			if(!p_WordsCurrent->AppendData(1,&m_CurSymbol))
			{
				m_RetVal = false;
				break;
			}
		}

		if(!m_RetVal) break;
		
	}


	DeleteCharLines(p_WordsHead);

	if((p_Numbers->GetNColumns() == 0) || (p_Numbers->GetNRaws() == 0))
		m_RetVal = false;

	return m_RetVal;
}

bool CParser::ParseWord(CCharLines* p_Word, C2DMatrix* p_Numbers)
{
	if((p_Word == NULL) || (p_Numbers == NULL)) return false;

	if((p_Word->GetLineSize() <= 0) || (p_Numbers->GetNColumns() > 1)) return false;

	long double m_OutNumber = 0;

	char * p_Buf = NULL;
	if((p_Buf = new char [p_Word->GetLineSize() + 1]) == NULL) return false;
	memset(p_Buf,0,(p_Word->GetLineSize() + 1)*sizeof(char));
	bool m_RetVal = true;
	p_Word->GetData(p_Buf,0,p_Word->GetLineSize());

	if(!ParseNumber(p_Buf, m_OutNumber))
	{
		m_RetVal = false;
	}
	else
	{
		if(!p_Numbers->AppendMatrixRaw(1,&m_OutNumber)) 
			m_RetVal = false;
	}

	delete[] p_Buf;
	return m_RetVal;
}

bool CParser::CheckNumber(const char m_Char)
{
	char m_Nums[] = "0123456789.,eE-+";
	int  m_Length = 16;
	
	for(int i = 0; i < m_Length; i++)
		if(m_Char == m_Nums[i]) return true;
	
	return false;
}

bool CParser::CheckDelimiter(const char m_Char)
{
	//Look at the ASCII table on http://www.cplusplus.com/doc/ascii.html
	char m_Delimiters[] = "\x00\x09\x3B\x20\x7F";
	int m_Length = 5;

	for (int i = 0; i < m_Length; i++)
		if(m_Char == m_Delimiters[i]) return true;
	
	return false;
}

bool CParser::CheckReturn(const char m_Char)
{
	char p_Returns[] = "\n\r\0";
	int  m_Length = 3;

	for (int i = 0; i < m_Length; i++)
		if(m_Char == p_Returns[i]) return true;
	
	return false;
}

bool CParser::ParseNumber(const char* string, long double& var)
{

    bool minus(false);                // true if number is negative
    bool eminus(false);                // true if exponent is negative
    bool sign_awaited(true);
    int f = -1;                       // A number of digits after floating point.
                // When f < 0 we are still parsing an integer part of a number.
    int exp = 0;
    int dig = 0;                            // A number of decimal digits
    int edig = 0;                            // A number of decimal digits in exponent (after 'E')
    long double x = 0;
    char c;                                        // A current character 

    while ((c = *string++),(c != 0))
    {
        if (c == ' ' || c == '\t') continue;

        if (sign_awaited && (c == '+' || c == '-')) 
        {
            minus = (c != '+');
            sign_awaited = false;
            continue;
        }
        if (f < 0 && (c == '.' || c == ',')) 
        {
            f = 0;
            sign_awaited = false;
            continue; 
        }
        if (c == 'e' || c == 'E') 
        {
            sign_awaited = true;
            while ((c = *string++),(c != 0))
            {
                if (c == ' ' || c == '\t') continue;
                if (sign_awaited && (c == '+' || c == '-')) 
                {
                    eminus = (c != '+');
                    sign_awaited = false;
                    continue;
                }
                if (c < '0' || c > '9') return false;
                exp = exp*10 + (c - '0');
                ++edig;
            }
            if (edig == 0) return false;
            if (eminus) exp = -exp;
            break;
        }

        if (c < '0' || c > '9') return false;
        sign_awaited = false;
        ++dig;
        if (f >= 0)
        {
            ++f;
            if (dig >= 25) continue; 
                // All digits after 25th are not significant.
        }
        x = x*10 + (c - '0');
    }

    if (dig == 0) return false;
    f = (f < 0) ? -exp : f-exp;
    while (f < 0) 
    {
        x *= 10;
        ++f; 
    }
    while (f > 0)
    {
        x /= 10;
        --f;
    }
    var = minus ? -x : x;

    return true;
}

bool CParser::SetIndex()
{
	if(p_DataBlocksHead == NULL) return false;

	C2DMatrix *p_Current = p_DataBlocksHead,
		  *p_Next = NULL,
		  *p_Prev = NULL;

	int i = 0;

	p_DataBlocksHead = NULL;

	while(p_Current != NULL)
	{	
		p_Next = p_Current->p_Next;

		if(!p_Current->IsEmpty())
		{
			if(p_DataBlocksHead == NULL)
				p_DataBlocksHead = p_Current;

			if(p_Prev != NULL)
			{
				if(i < 1)
				{
					p_Current->m_IndexNum = p_Prev->m_IndexNum;
					p_Current->m_BlockNum = p_Prev->m_BlockNum + 1;
				}
				else
				{
					p_Current->m_IndexNum = p_Prev->m_IndexNum + 1;
					p_Current->m_BlockNum = 0;
				}
			}
			else
			{
				p_Current->m_BlockNum = 0;
				p_Current->m_IndexNum = 0;
			}

			i = 0;
			p_Prev = p_Current;
		}
		else
		{
			if(p_Prev != NULL)
				p_Prev->p_Next = p_Next;

			delete p_Current;
			p_Current = NULL;
			i++;
		}
		p_Current = p_Next;
	}

        return (p_DataBlocksHead != NULL);
}

bool CParser::GetMatrix(int m_Index,
                       C2DMatrix* p_Dest,
                       int m_RawFrom,
                       int m_NRaws,
                       int m_ColumnFrom,
                       int m_NColumns)
{
        if(p_DataBlocksHead == NULL) return false;

        C2DMatrix * p_Current = NULL;
        p_Current = p_DataBlocksHead;

        int i = 0;
        while(p_Current != NULL)
        {
                i = i + 1;
                if(i == m_Index)
                        return p_Current->GetMatrix(p_Dest,
                                                    m_RawFrom,
                                                    m_NRaws,
                                                    m_ColumnFrom,
                                                    m_NColumns);
                p_Current = p_Current->p_Next;
        }

        return false;

}

bool CParser::GetMatrix(int m_Index,
                       long double** pp_Matrix,
                       int  m_RawFrom,
                       int  m_NRaws,
                       int  m_ColumnFrom,
                       int  m_NColumns)
{
         if(p_DataBlocksHead == NULL) return false;

        C2DMatrix * p_Current = NULL;
        p_Current = p_DataBlocksHead;

        int i = 0;
        while(p_Current != NULL)
        {
                i = i + 1;
                if(i == m_Index)
                        return p_Current->GetMatrix(pp_Matrix,
                                                    m_RawFrom,
                                                    m_NRaws,
                                                    m_ColumnFrom,
                                                    m_NColumns);
                p_Current = p_Current->p_Next;
        }

        return false;
}

 bool CParser::GetNMatrices(int& m_NMatrices)
 {
     if(p_DataBlocksHead == NULL)
     {
         m_NMatrices = 0;
         return false;
     }

     C2DMatrix* p_Current = p_DataBlocksHead;
     int i = 0;

     while(p_Current != NULL)
     {
         i = i+1;
         p_Current = p_Current->p_Next;
     }
     m_NMatrices = i;
     return true;
 }

 bool CParser::GetMatrixDimensions(int m_Index,
                                   int &m_NRaws,
                                   int &m_NColumns)
 {
     if(p_DataBlocksHead == NULL) return false;

     C2DMatrix* p_Current = p_DataBlocksHead;
     int i = 0;

     while(p_Current != NULL)
     {
         i = i + 1;
         if(i == m_Index)
         {
             m_NRaws = p_Current->GetNRaws();
             m_NColumns = p_Current->GetNColumns();
             return true;
         }
         p_Current = p_Current->p_Next;
     }

     return false;
 }

