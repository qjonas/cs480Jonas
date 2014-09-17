// this is the cpp file for loader.h

#include <fstream>
#include <string.h>
#include "loader.h"

using namespace std;


loader::loader( const char* uName )
{
 char* fName = new char[ 200 ];
 char* data = new char[ 2000 ];

 strcopy( fName, uName );
 char * temp = new char[2000];


 // clear new temp arrays

ifstream fin( fName );

fin.clear();


 int i;
    for( i = 0; i < 2000; i++ )
       {
        temp[i] = data[i] = '\0';
       }

 // extract data
    // prime the loop
       fin.getline( temp, 256 );
       strcat( data, temp );
       data[strlen(data)] = '\0';
    // loop
    while( fin.good() )
      {
       fin.getline( temp, 256 );
       strcat( data, temp );
       data[strlen(data)] = '\0';
      }
fin.close();

} // end of function

loader::~loader()
{
 delete[] fName;
 delete[] data:
 }

char* loader::getData()
{
 return data;
}


