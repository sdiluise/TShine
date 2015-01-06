#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_


#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>


using std::vector;
using std::string;

/**
Alias is defined as well

namespace StringUtils = STR_UTILS;

*/

namespace STR_UTILS
{



static struct IsLonger{
		//longer TString must appear before in a std::sort ordering
		bool operator()(TString s1, TString s2){ return ( s1.Length() > s2.Length() ); }
} CompLengthDecr;

static struct IsShorter{
		//longer TString must appear before in a std::sort ordering
		bool operator()(TString s1, TString s2){ return ( s1.Length() < s2.Length() ); }
} CompLengthIncr;


inline vector<TString> Tokenize(TString line,TString delim){

	vector<TString> list;
    if(line.IsNull() || line.IsWhitespace()) return list;

	const char *delimiters = delim.Data();//":";

    TObjArray* Strings = line.Tokenize(delimiters);

    if(Strings->GetEntriesFast()) {
    	TIter iString(Strings);
    	TObjString* os=0;
    	Int_t j=0;
    	while ((os=(TObjString*)iString())) {
    		j++;

    		//cout<<os->GetString().Data()<<endl;

    		list.push_back(os->GetString());
    	}
    	delete os;
    }

    delete Strings;

	return list;

}


inline vector<string> TokenizeStr(string line,string delim){


	std::vector<TString> TS = Tokenize(TString(line),TString(delim));

	std::vector<string> list;

	for(int i=0; i< TS.size(); ++i){
		//list.push_back( string(TS[i].Data()) );
		list.push_back( string(TS[i]) );
	}

    return list;
}



inline int Duplicates( std::vector<TString> list){

	int N=0;

	for(int i1=0; i1< list.size()-1; ++i1){
		for(int i2=i1+1; i2< list.size();++i2){
			if(list.at(i1)==list.at(i2)) N++;
		}
	}

	return N;
}


inline bool StringIsPresent(std::vector<TString> &list, TString &s){

	for(int i=0;i<list.size();++i){
		if( list.at(i)==s ) return 1;
	}

	return 0;
}


template <typename T>
  string ToString ( T Number )
  {
     std::ostringstream ss;
     ss << Number;
     return ss.str();
  }



template <typename N>
  N ToNumber ( const string &Text )
   {
     std::istringstream ss(Text);
     N result;
     return ss >> result ? result : 0;
   }




inline TString Name(TString str1,TString sep,TString str2="") {

	TString name=str1;

	name+=sep; name+=str2;

	return name;

}

inline TString Name(TString str1,TString sep1,TString str2,TString sep2, TString str3) {

	TString name=Name(str1,sep1,str2);

	name+=sep2; name+=str3;

	return name;

}

inline TString Name(TString str1,TString sep1,TString str2,TString sep2, TString str3, TString sep3, TString str4) {

	TString name=Name(str1,sep1,str2,sep2,str3);

	name+=sep3; name+=str4;


	return name;

}

inline TString SplitLineLabel(TString top, TString bottom){

	TString label = "";

	if(bottom.IsWhitespace()) return top;

	label = Name("#splitline{",top,"}{",bottom,"}");

	return label;
}

inline TString Title(TString tit1,TString sep,TString tit2) {

	TString title=tit1;

	title+=sep; title+=tit2;

	return title;

}

inline TString TitleXYZ(TString tit,TString titx,TString tity="",TString titz="") {


	TString title=tit;

	title+=";"; title+=titx;

	if(!tity.IsWhitespace() ){
		title+=";"; title+=tity;
	}

	if(!titz.IsWhitespace() ){
		title+=";"; title+=titz;
	}


	return title;
}


inline TString AppendToTitleXYZ(TString tit,TString sep, TString tit2) {

	/**
	 *  Use this to add a string to a title in the format
	 *  title;xx;yy;zz
	 *  since a string added to this title will be shown to
	 *  along with the last axis label which is set
	 *
	 *  the resuting string of the method is
	 *
	 *  title sep tit2;xx;yy;zz
	 *
	 */

	TString str="";

	std::vector<TString> vec=STR_UTILS::Tokenize(tit,";");

	int n=vec.size();

	if(n==0) return tit2;

	if(n>=1){
		str=STR_UTILS::Name(vec.at(0),sep,tit2);
	}


	if(n>=2){
		str=STR_UTILS::TitleXYZ(str,vec.at(1));
	}

	if(n>=3){
		str=STR_UTILS::TitleXYZ(str,vec.at(2));
	}

	if(n>=4){
		str=STR_UTILS::TitleXYZ(str,vec.at(3));
	}

	return str;
}

inline TString PrependToTitleXYZ(TString tit,TString sep, TString tit2) {

	/**
	*  Use this to add a string to a title in the format
	*  title;xx;yy;zz
	*  since a string added to this title will be shown to
	*  along with the last axis label which is set
	*
	*  the resuting string of the method is
	*
	*  tit2 sep title;xx;yy;zz
	*
	*/


	TString str="";

	std::vector<TString> vec=STR_UTILS::Tokenize(tit,";");

	int n=vec.size();

	if(n==0) return tit2;

	if(n>=1){
		str=STR_UTILS::Name(tit2,sep,vec.at(0));
	}


	if(n>=2){
		str=STR_UTILS::TitleXYZ(str,vec.at(1));
	}

	if(n>=3){
		str=STR_UTILS::TitleXYZ(str,vec.at(2));
	}

	if(n>=4){
		str=STR_UTILS::TitleXYZ(str,vec.at(3));
	}

	return str;
}



}/// STR_UTILS


namespace StringUtils = STR_UTILS;

#endif
