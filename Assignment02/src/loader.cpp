// this is the cpp file for loader.h

#include <fstream>
#include <string.h>
#include "loader.h"

using namespace std;




loader::loader( const char* uName )
{
  fName = new char[ 2000 ];
  data = new char[ 2000 ];
 char * temp = new char[2000];


 strcpy( fName, uName );



 // clear new temp arrays

ifstream fin( fName );

fin.clear();


 int i;
    for( i = 0; i < 2000; i++ )
       {
        fName[i] = temp[i] = data[i] = '\0';
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
 delete[] data;

 fName = data = NULL;
 }

char* loader::getData()
{
 
 return data;
}


