// this is the header file for the shader loader class

class loader{
public:
loader( const char* uName);
~loader();

char* getData();

private:
 char* fName;
 char* data;

};
