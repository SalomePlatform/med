#include "MEDMEM_SkyLineArray.hxx"
#include "utilities.h"

using namespace std;
using namespace MEDMEM;

MEDSKYLINEARRAY::MEDSKYLINEARRAY(): _count(0), _length(0),
				_index((int*)NULL),_value((int*)NULL)
{
  MESSAGE("Constructeur MEDSKYLINEARRAY sans parametre");
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const MEDSKYLINEARRAY &myArray):
			        _count(myArray._count),_length(myArray._length),
				_index(_count+1),_value(_length)
{
	BEGIN_OF("MEDSKYLINEARRAY(const MEDSKYLINEARRAY &)");
	memcpy(_index,myArray._index,sizeof(int)*(_count+1));
	memcpy(_value,myArray._value,sizeof(int)*_length);
	END_OF("MEDSKYLINEARRAY(const MEDSKYLINEARRAY &)");
}

MEDSKYLINEARRAY::~MEDSKYLINEARRAY()
{
  MESSAGE("Destructeur ~MEDSKYLINEARRAY");

  //if (_index != NULL) delete [] _index;
  //if (_value != NULL) delete [] _value;
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const int count, const int length):
                                _count(count), _length(length),
                                _index(_count+1),_value(_length)
{
	MESSAGE("Constructeur MEDSKYLINEARRAY(count="<<count<<", length="<<length<<") avec parametres");
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const int count, const int length,
				 const int* index, const int* value,bool shallowCopy):
                                _count(count), _length(length)
{
// 	MESSAGE("Constructeur MEDSKYLINEARRAY(count="<<count<<", length="<<length<<") avec parametres");
		if(shallowCopy)
	  {
	    _index.setShallowAndOwnership(index);
	    _value.setShallowAndOwnership(value);
	  }
	else
	  {
	    _index.set(_count+1,index);
	    _value.set(_length,value);
	  }
}

//  void MEDSKYLINEARRAY::setMEDSKYLINEARRAY( const int count , const int length, int* index , int* value )
//  {
//    MESSAGE("void MEDSKYLINEARRAY::setMEDSKYLINEARRAY(count, length, index, value)");
//    _count  = count  ;
//    _length = length ;

//    //if (_index != NULL) delete [] _index;
//    //if (_value != NULL) delete [] _value;

//  	  _index.set(index);
//  	  _value.set(value);
//  }
