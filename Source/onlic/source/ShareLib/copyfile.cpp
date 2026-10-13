
#include "ShareLib.h"

using namespace std;
/*
typedef basic_ifstream<char, char_traits<char> > ifstream;
typedef basic_ofstream<char, char_traits<char> > ofstream;
typedef basic_ifstream<wchar_t, char_traits<wchar_t> > wifstream;
typedef basic_ofstream<wchar_t, char_traits<wchar_t> > wofstream;
*/
int copyfile(char *srcFile, char *dstFile)
{
	 ifstream src(srcFile,ios_base::in | ios_base::binary);
     if (!src.is_open()) return -1;

     ofstream dest(dstFile,ios_base::out | ios_base::trunc | ios_base::binary);
	 if (!dest.is_open()) return -2;

     char buf[10000];
     int nread;

     for (;;) {
          src.read(buf,10000);
          nread=src.gcount();
          if (nread==0) break;
          dest.write(buf,nread);
     }
     src.close();
     dest.close();
	 return 0;
}

