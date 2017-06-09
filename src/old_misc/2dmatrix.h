//datablocks.h
//In this class is possible to safely store a 2D matrix, retreve it, and make some matrix operations....
//This class is easy extendable, but here I use some rules. To increase a  safety and stability of this class
//all the manipulations with the data executes thrue the buffer memory.
//So, first we copy all the data to the buffer, making anything, and is all pessed well - writing the matrices.

#ifndef _DATABLOCKS_H
#define _DATABLOCKS_H

#include "string.h"
#include "stdio.h"

class C2DMatrix
{
public:
	C2DMatrix();
       ~C2DMatrix();

	//Add one new raw to the exiting matrix, will fail if the raw is not the same size 
	//(if  m_Coluns are bigger or smaller than m_NColumns ) with the current matrix.
	//But, if the current matrix are empty, this raw will be the first raw.
	bool AppendMatrixRaw(int m_Columns, long double* p_DataRaw);

	//This function allows to append a matrix p_Matrix to the end of the existing, 
	//Also fails if m_NColumns are not the same. If the matrix are empty,
	//after this function it will be a copy of p_Matrix
	bool AppendMatrixRaw(C2DMatrix* p_Matrix);
	
	//Used to initialize whall the matrix with pp_Src. This function will delete all previous data
	//stored in the pp_Matrix.
	bool SetMatrix(long double** pp_Src, int m_NewNRaws, int m_NewNColumns);
	
	//Matrix Multiplication: p_Dest = ThsMatrix*p_R. This function will kill all previous 
	//data in the p_Dest.
	bool MultiplyR(C2DMatrix* p_Dest, C2DMatrix* p_R);

	//A complex check, that for 100% says that this matrix is empty.
	bool IsEmpty(){return ((m_NColumns == 0) ||(m_NRaws == 0) || (pp_Matrix == NULL));}

	//Will fail if the matrix are empty
	bool TransposeMatrix();

	//Important! Befure using of this function you should previously alloc pp_dest array.
	//The best way of such initialization are implemented in the MemAlloc function (see below).
	//Can be used with a vector.
	bool GetMatrix(long double** pp_Dest, int m_RawFrom,
					      int m_Raws,
					      int m_ColumnFrom,
					      int m_Columns);
	//This is more safe method of the retreving data from the matrix to another matrix.
	bool GetMatrix(C2DMatrix* p_Dest, int m_RawFrom,
					  int m_Raws,
					  int m_ColumnsFrom,
					  int m_Columns);
	bool GetElement(int m_Raw, int m_Column, long double& m_Element);

        //Prints all matrix on console.
	void ShowMatrix();

        bool RemoveRaw(int m_Raw);

	//This will return the values of the private variables outside.
	int GetNRaws(){ return m_NRaws; }
	int GetNColumns(){ return m_NColumns; }

        //Kill all the data in matrix (also used by destructor)
        void DeleteMatrix();

        bool SetName(const char* p_NewName, int m_Length);
        char* GetNamePointer();
        long double ** GetMatrix(){return pp_Matrix;}

protected:
	//Before allocating, pp_Dest should have a NULL value (to avoid killing of the live pointer)
        bool MemAlloc(long double** &pp_Dest, int m_Raws, int m_Columns);
	//Delete pp_Dest array, and assign it to NULL
	bool MemRelease(long double** &pp_Dest, int m_Raws, int m_Columns);


public:
	//To have a possebility of FIFO creation
	C2DMatrix* p_Next;
	int m_BlockNum; //Should increase after one blank line in the input file
	int m_IndexNum; //Should increase after two or more blanks in the input file

protected:
	//Data stored in this class
	long double** pp_Matrix;
        char * p_Name;
	int m_NColumns;
	int m_NRaws;
};

#endif
