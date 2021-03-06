/****************************************************************/
/* Parallel Combinatorial BLAS Library (for Graph Computations) */
/* version 1.2 -------------------------------------------------*/
/* date: 10/06/2011 --------------------------------------------*/
/* authors: Aydin Buluc (abuluc@lbl.gov), Adam Lugowski --------*/
/****************************************************************/
/*
Copyright (c) 2011, Aydin Buluc

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _SP_MAT_H
#define _SP_MAT_H

#include <iostream>
#include <vector>
#include <utility>
#include "CombBLAS.h"
#include "SpDefs.h"
#include "promote.h"
#include "LocArr.h"


// Forward declaration (required since a friend function returns a SpTuples object)
template <class IU, class NU>	
class SpTuples;


/**
 ** The abstract base class for all derived sequential sparse matrix classes
 ** Contains no data members, hence no copy constructor/assignment operator
 ** Uses static polymorphism through curiously recurring templates (CRTP)
 ** Template parameters: IT (index type), NT (numerical type), DER (derived class type)
 **/
template < class IT, class NT, class DER >
class SpMat
{
public:
	//! Standard destructor, copy ctor and assignment are generated by compiler, they all do nothing !
	//! Default constructor also exists, and does nothing more than creating Base<Derived>() and Derived() objects
	//! One has to call one of the overloaded create functions to get an nonempty object
	void Create(const vector<IT> & essentials)
	{
		static_cast<DER*>(this)->CreateImpl(essentials);
	}

	void Create(IT size, IT nRow, IT nCol, tuple<IT, IT, NT> * mytuples)
	{
		static_cast<DER*>(this)->CreateImpl(size, nRow, nCol, mytuples);
	}	
	
	SpMat< IT,NT,DER >  operator() (const vector<IT> & ri, const vector<IT> & ci) const;
	
	template <typename SR>
	void SpGEMM( SpMat< IT,NT,DER > & A, SpMat< IT,NT,DER > & B, bool isAT, bool isBT);

	// ABAB: A semiring elementwise operation with automatic type promotion is required for completeness (should cover +/- and .* ?)
	// ABAB: A neat version of ConvertNumericType should be in base class (an operator SpMat<NIT,NNT,NDER>())

	void Split( SpMat< IT,NT,DER > & partA, SpMat< IT,NT,DER > & partB); 
	void Merge( SpMat< IT,NT,DER > & partA, SpMat< IT,NT,DER > & partB); 

	Arr<IT,NT> GetArrays() const
	{
		return static_cast<const DER*>(this)->GetArrays();
	}
	vector<IT> GetEssentials() const
	{
		return static_cast<const DER*>(this)->GetEssentials();
	}

	void Transpose()
	{
		static_cast<DER*>(this)->Transpose();
	}

	bool operator== (const SpMat< IT,NT,DER > & rhs) const; 
		
	ofstream& put(ofstream& outfile) const;
	ifstream& get(ifstream& infile);
	
	bool isZero() const { return static_cast<const DER*>(this)->isZero(); }
	IT getnrow() const { return static_cast<const DER*>(this)->getnrow(); }
	IT getncol() const { return static_cast<const DER*>(this)->getncol(); }
	IT getnnz() const  { return static_cast<const DER*>(this)->getnnz(); }

protected:

	template < typename UIT, typename UNT, typename UDER >
	friend ofstream& operator<< (ofstream& outfile, const SpMat< UIT,UNT,UDER > & s);	

	template < typename UIT, typename UNT, typename UDER >
	friend ifstream& operator>> (ifstream& infile, SpMat< UIT,UNT,UDER > & s);

	//! Returns a pointer to SpTuples, in order to avoid temporaries
	//! It is the caller's responsibility to delete the returned pointer afterwards
	template< class SR, class NUO, class IU, class NU1, class NU2, class DER1, class DER2 >
	friend SpTuples< IU, NUO > *
	MultiplyReturnTuples (const SpMat< IU, NU1, DER1 > & A, const SpMat< IU, NU2, DER2 > & B, bool isAT, bool isBT, bool clearA, bool clearB);

};

#include "SpMat.cpp"	
#endif

