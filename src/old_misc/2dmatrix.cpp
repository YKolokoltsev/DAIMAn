//2dmatrix.cpp

#include "2dmatrix.h"

//Not all the functions here are well optimized... Here I
//wrote a code, that will be stable... not fast. - Best for parsing of small files.
//Nice Idea to add in this class CCharLine FIFO, to allow search of the tables
//over it's titles.

C2DMatrix::C2DMatrix()
{
	p_Next = NULL;
        p_Name = NULL;

	pp_Matrix = NULL;
	m_NColumns = 0;
	m_NRaws = 0;

	m_BlockNum = 0;
	m_IndexNum = 0;

}

C2DMatrix::~C2DMatrix()
{
	DeleteMatrix();
        if(p_Name != NULL) delete[] p_Name;
}

bool C2DMatrix::AppendMatrixRaw( int m_Columns, long double* p_DataRaw )
{
	if((p_DataRaw == NULL) || (m_Columns == 0)) return false;

	if((m_Columns != m_NColumns) && (m_NColumns != 0) ) return false;

	long double * p_NewDataRaw = NULL;
			
	if((p_NewDataRaw = new long double [m_Columns]) == NULL) return false;

	if(memcpy(p_NewDataRaw,p_DataRaw,m_Columns*sizeof(long double))== NULL)
	{
		delete[] p_NewDataRaw;
		return false;
	}

	long double ** pp_NewDataBlock = NULL;

	if((pp_NewDataBlock = new long double* [m_NRaws + 1]) == NULL)
	{
		delete[] p_NewDataRaw;
		return false;
	}

	if(memcpy(pp_NewDataBlock,pp_Matrix,m_NRaws*sizeof(long double*)) == NULL)
	{
		delete[] p_NewDataRaw;
		delete[] pp_NewDataBlock;
		return false;
	}

	delete[] pp_Matrix;

	pp_NewDataBlock[m_NRaws] = p_NewDataRaw;
	pp_Matrix = pp_NewDataBlock;
	m_NRaws = m_NRaws + 1;
	m_NColumns = m_Columns;

	return true;

}

bool C2DMatrix::AppendMatrixRaw(C2DMatrix* p_Matrix)
{
	if(p_Matrix == NULL) return false;

	if(m_NColumns != 0)
		if(p_Matrix->GetNColumns() != m_NColumns) return false;

	if(p_Matrix->GetNRaws() <= 0) return false;
	
	int m_NewNRaws = p_Matrix->GetNRaws() + m_NRaws;
	int m_NewNColumns = p_Matrix->GetNColumns();
	long double** pp_NewMatrix = NULL;
	
	if(!MemAlloc(pp_NewMatrix,m_NewNRaws,m_NewNColumns)) return false;

	bool m_RetVal = true;

	if((m_NRaws != 0) && (m_NColumns != 0))
	if(!GetMatrix(pp_NewMatrix,0,m_NRaws,0,m_NColumns))
		m_RetVal = false;

	if(m_RetVal)
	if(!p_Matrix->GetMatrix(&pp_NewMatrix[m_NRaws], 0, p_Matrix->GetNRaws(), 0, m_NewNColumns))
		m_RetVal = false;

	if(!m_RetVal)
	{
		MemRelease(pp_NewMatrix,m_NewNRaws,m_NewNColumns);
	}
	else
	{
		DeleteMatrix();
		pp_Matrix = pp_NewMatrix;
		m_NRaws = m_NewNRaws;
		m_NColumns = m_NewNColumns;
	}

	return m_RetVal;
}

void C2DMatrix::DeleteMatrix()
{
	MemRelease(pp_Matrix,m_NRaws,m_NColumns);

	m_NRaws = 0;
	m_NColumns = 0;

	return;
}

void C2DMatrix::ShowMatrix()
{
	if((pp_Matrix == NULL) || (m_NRaws == 0) || (m_NColumns == 0)) return;

	for(int i = 0; i < m_NRaws; i++)
	{
		for(int j = 0; j < m_NColumns; j++)
		{
			if(pp_Matrix[i][j] >= 0)
			{
				printf(" %LE ",pp_Matrix[i][j]);
			}
			else
			{
				printf("%LE ", pp_Matrix[i][j]);
			}
		}

		printf("\n");
	}
	
	return;
}

bool C2DMatrix::TransposeMatrix()
{
	if((pp_Matrix == NULL) || (m_NRaws == 0) || (m_NColumns == 0)) return false;

	int m_NewNRaws = m_NColumns,
		     m_NewNColumns = m_NRaws;

	long double** pp_NewMatrix = NULL;

	if(!MemAlloc(pp_NewMatrix,m_NColumns,m_NRaws)) return false;

	for(int i = 0; i < m_NRaws; i++)
		for(int j = 0; j < m_NColumns; j++)
			pp_NewMatrix[j][i] = pp_Matrix[i][j];

	DeleteMatrix();

	pp_Matrix = pp_NewMatrix;
	m_NColumns = m_NewNColumns;
	m_NRaws = m_NewNRaws;

	return true;
}

bool C2DMatrix::SetMatrix(long double** pp_Src, int m_NewNRaws, int m_NewNColumns)
{
	if(pp_Src == NULL) return false;
        if((m_NewNRaws == 0) || (m_NewNColumns == 0)) return false;

	long double** pp_NewMatrix = NULL;
	if(!MemAlloc(pp_NewMatrix,m_NewNRaws,m_NewNColumns)) return false;

	bool m_RetVal = true;

	for (int i = 0; i < m_NewNRaws; i++)
	{
                if((memcpy(pp_NewMatrix[i],pp_Src[i],m_NewNColumns*sizeof(long double))) == NULL)
		{
			m_RetVal = false;
			break;
		}
	}

	if(!m_RetVal)
	{
		MemRelease(pp_NewMatrix,m_NewNRaws,m_NewNColumns);
		return false;
	}
	else
	{
		DeleteMatrix();
		pp_Matrix = pp_NewMatrix;
		m_NColumns = m_NewNColumns;
		m_NRaws = m_NewNRaws;
	}

	return m_RetVal;
}

bool C2DMatrix::GetMatrix(long double ** pp_Dest, int m_RawFrom,
						  int m_Raws,
						  int m_ColumnFrom,
						  int m_Columns)
{
	if(pp_Dest == NULL) return false;

	if((m_Raws == 0) || ((m_RawFrom + m_Raws) > m_NRaws)) return false;

	if((m_Columns == 0) || ((m_ColumnFrom + m_Columns) > m_NColumns)) return false;

	for(int i = 0; i < m_Raws; i++)
	if((memcpy(pp_Dest[i], &pp_Matrix[i + m_RawFrom][m_ColumnFrom],m_Columns*sizeof(long double))) == NULL)
	{
		return false;
	}

	return true;
}

bool C2DMatrix::GetMatrix(C2DMatrix* p_Dest, int m_RawFrom,
					  int m_Raws,
					  int m_ColumnFrom,
					  int m_Columns)
{
	if(p_Dest == NULL) return false;
	
	if((m_Raws == 0) || ((m_RawFrom + m_Raws) > m_NRaws)) return false;

	if((m_Columns == 0) || ((m_ColumnFrom + m_Columns) > m_NColumns)) return false;

	long double** pp_Src = NULL;
	if(!MemAlloc(pp_Src,m_Raws,m_Columns)) return false;

	if(!GetMatrix(pp_Src,m_RawFrom,m_Raws,m_ColumnFrom,m_Columns))
	{
		MemRelease(pp_Src,m_Raws,m_Columns);
		return false;
	}
	

	bool m_RetVal = p_Dest->SetMatrix(pp_Src, m_Raws, m_Columns);

	MemRelease(pp_Src,m_Raws,m_Columns);

	return m_RetVal;
}

bool C2DMatrix::MultiplyR(C2DMatrix* p_Dest, C2DMatrix* p_R)
{
	if((p_Dest == NULL) || (p_R == NULL)) return false;
	if((p_R->IsEmpty()) || IsEmpty()) return false;

	//Formulae: p_Dest[i][j] = Sum(n,p_This[i][n]*p_R[n][j]);
	
	if(m_NColumns != p_R->GetNRaws()) return false;

	int m_OutNRaws = m_NRaws,
		     m_OutNColumns = p_R->GetNColumns();

	long double **pp_OutMatrix = NULL,
		    **pp_RMatrix = NULL;

	if(!MemAlloc(pp_OutMatrix, m_OutNRaws, m_OutNColumns))
		return false;
	if(!MemAlloc(pp_RMatrix,p_R->GetNRaws(),p_R->GetNColumns()))
	{
		MemRelease(pp_OutMatrix,m_OutNRaws, m_OutNColumns);
		return false;
	}

	bool m_RetVal = true;

	m_RetVal = p_R->GetMatrix(pp_RMatrix,0,p_R->GetNRaws(),0,p_R->GetNColumns());


	int i, j, n;
	long double Sum = 0;

	if(m_RetVal)
	{
	   for(i = 0; i < m_OutNRaws; i++)
		for(j = 0; j < m_OutNColumns; j++)
		{
			Sum = 0;
			for(n = 0; n < m_NColumns; n++)
				Sum += pp_Matrix[i][n]*pp_RMatrix[n][j];

			pp_OutMatrix[i][j] = Sum;
		}

	    j = m_OutNRaws;
	}

  	
	
	m_RetVal = p_Dest->SetMatrix(pp_OutMatrix,m_OutNRaws,m_OutNColumns);

	MemRelease(pp_OutMatrix,m_OutNRaws,m_OutNColumns);
	MemRelease(pp_RMatrix,p_R->GetNRaws(),p_R->GetNColumns());
	
	return m_RetVal;
}

bool C2DMatrix::MemAlloc(long double** &pp_Dest,int m_Raws, int m_Columns)
{
	if(pp_Dest != NULL) return false;
	if((m_Raws <= 0) || (m_Columns <= 0)) return false;

	pp_Dest = new long double* [m_Raws];
	if(pp_Dest == NULL) return false;

	for(int i = 0; i < m_Raws; i++)
	{
		if((pp_Dest[i] = new long double [m_Columns]) == NULL)
		{
			if(!MemRelease(pp_Dest,i,m_Columns))
				printf("Memory lieak while MemAlloc detected\n");
			return false;
		}
	}

	return true;
}

bool C2DMatrix::MemRelease(long double** &pp_Dest, int m_Raws, int m_Columns)
{
	if(pp_Dest == NULL) return false;
	if((m_Raws <= 0) || (m_Columns <= 0)) return false;

	for(int i = 0; i < m_Raws; i++)
	{
		delete[] pp_Dest[i];	
	}

	delete[] pp_Dest;
	pp_Dest = NULL;

	return true;
}

bool C2DMatrix::GetElement(int m_Raw, int m_Column, long double& m_Element)
{
	if(IsEmpty()) return false;
	if(m_Raw >= m_NRaws) return false;
	if(m_Column >= m_NColumns) return false;

	m_Element = pp_Matrix[m_Raw][m_Column];

	return true;
}

bool C2DMatrix::RemoveRaw(int m_Raw)
{
    if(m_Raw > (m_NRaws - 1)) return false;
    if(IsEmpty()) return false;

    if(m_NRaws == 1)
    {
        DeleteMatrix();
        return true;
    }

    long double ** pp_NewMatrix = NULL;

    if(!MemAlloc(pp_NewMatrix,m_NRaws-1,m_NColumns)) return false;

    bool m_RetVal = true;

    if(m_Raw != 0)
        m_RetVal = GetMatrix(pp_NewMatrix,0,m_Raw,0,m_NColumns);

    if(m_Raw != m_NRaws)
    if(m_RetVal)
        m_RetVal = GetMatrix(&pp_NewMatrix[m_Raw],m_Raw+1,m_NRaws - m_Raw,0,m_NColumns);

    if(m_RetVal)
    {
        MemRelease(pp_Matrix, m_NRaws, m_NColumns);
        pp_Matrix = pp_NewMatrix;
        m_NRaws = m_NRaws - 1;
    }
    else
    {
        MemRelease(pp_NewMatrix,m_NRaws-1,m_NColumns);
    }

    return m_RetVal;
}

bool C2DMatrix::SetName(const char* p_NewName, int m_Length)
{
    if(p_NewName == NULL) return false; if(m_Length <= 0) return false;
    if(p_Name != NULL) delete [] p_Name;
    if((p_Name = new char[m_Length + 1]) == NULL) return false;
    if(memcpy(p_Name, p_NewName,m_Length*sizeof(char)) != NULL)
    {
        p_Name[m_Length] = '\0';
        return true;
    }
    else
    {
        delete p_Name;
        p_Name = NULL;
        return false;
    }
}

char* C2DMatrix::GetNamePointer()
{
    return p_Name;
}

