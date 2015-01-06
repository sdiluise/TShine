/**
 *
 *
 * MessageMgr.h
 *
 *  Created on: Nov 10, 2013
 *      Author: Silvestro di Luise
 *
 *      Simple Functions and Macros to handle Log/Warning/Error Messages
 *
 *
 */


#ifndef MESSAGEMGR_H_
#define MESSAGEMGR_H_

#include <iostream>
#include <string>


using namespace std;



#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)




#ifdef DEBUG
#define D(x) do { std::cerr << x <<std::endl; } while (0)
#else
#define D(x)
#endif





namespace MSG{


static string WARN=" ## WARNING ## "; //!
static string ERR=" !!! ERROR !!! "; //!

using std::cout;
using std::endl;




template<typename T1>
 void LOG(T1 a){
	 cout<<a<<endl;
 }

template<typename T1,typename T2>
 void LOG(T1 a,T2 b){
	 cout<<a<<""<<b<<endl;
 }


template<typename T1,typename T2, typename T3>
 void LOG(T1 a,T2 b, T3 c){
	 cout<<a<<""<<b<<""<<c<<endl;
 }


template<typename T1,typename T2,typename T3, typename T4>
 void LOG(T1 a,T2 b, T3 c, T4 d){
	 cout<<a<<""<<b<<""<<c<<""<<d<<std::endl;
 }


template<typename T1, typename T2,typename T3, typename T4, typename T5>
 void LOG(T1 a, T2 b, T3 c, T4 d, T5 e){
	 cout<<a<<""<<b<<""<<c<<""<<d<<""<<e<<endl;
 }


template<typename T1, typename T2,typename T3, typename T4, typename T5, typename T6>
 void LOG(T1 a, T2 b, T3 c, T4 d, T5 e, T6 f){
	 cout<<a<<""<<b<<""<<c<<""<<d<<""<<e<<""<<f<<endl;
 }


template<typename T1, typename T2,typename T3, typename T4, typename T5, typename T6, typename T7>
 void LOG(T1 a, T2 b, T3 c, T4 d, T5 e, T6 f, T7 g){
	 cout<<a<<""<<b<<""<<c<<""<<d<<""<<e<<""<<f<<""<<g<<endl;
 }


template<typename T1>
 void WARNING(T1 a){
	 cout<<WARN; LOG(a);
 }

template<typename T1,typename T2>
 void WARNING(T1 a,T2 b){
	 cout<<WARN; LOG(a,b);
 }

template<typename T1, typename T2,typename T3>
 void WARNING(T1 a, T2 b, T3 c){
	 cout<<WARN; LOG(a,b,c);
 }

template<typename T1, typename T2,typename T3, typename T4>
 void WARNING(T1 a, T2 b, T3 c, T4 d){
	 cout<<WARN; LOG(a,b,c,d);
 }

template<typename T1, typename T2,typename T3, typename T4, typename T5>
 void WARNING(T1 a, T2 b, T3 c, T4 d, T5 e){
	 cout<<WARN; LOG(a,b,c,d,e);
 }


template<typename T1, typename T2,typename T3, typename T4, typename T5, typename T6>
 void WARNING(T1 a, T2 b, T3 c, T4 d, T5 e, T6 f){
	 cout<<WARN; LOG(a,b,c,d,e,f);
 }

template<typename T1, typename T2,typename T3, typename T4, typename T5, typename T6, typename T7>
 void WARNING(T1 a, T2 b, T3 c, T4 d, T5 e, T6 f, T7 g){
	 cout<<WARN; LOG(a,b,c,d,e,f,g);
 }


template<typename T1>
 void ERROR(T1 a){
	 cout<<ERR; LOG(a);
 }

template<typename T1,typename T2>
 void ERROR(T1 a,T2 b){
	 cout<<ERR; LOG(a,b);
 }

template<typename T1, typename T2,typename T3>
 void ERROR(T1 a, T2 b, T3 c){
	 cout<<ERR; LOG(a,b,c);
 }

template<typename T1, typename T2,typename T3, typename T4>
 void ERROR(T1 a, T2 b, T3 c, T4 d){
	 cout<<ERR; LOG(a,b,c,d);
 }

template<typename T1, typename T2,typename T3, typename T4, typename T5>
 void ERROR(T1 a, T2 b, T3 c, T4 d, T5 e){
	 cout<<ERR; LOG(a,b,c,d,e);
 }

template<typename T1, typename T2,typename T3, typename T4, typename T5, typename T6>
 void ERROR(T1 a, T2 b, T3 c, T4 d, T5 e, T6 f){
	 cout<<ERR; LOG(a,b,c,d,e,f);
 }


template<typename T1, typename T2,typename T3, typename T4, typename T5, typename T6, typename T7>
 void ERROR(T1 a, T2 b, T3 c, T4 d, T5 e, T6 f, T7 g){
	 cout<<ERR; LOG(a,b,c,d,e,f,g);
 }

}//MSG


//not readable for rootcint
// #define MSG_WHERE MSG::LOG(__FILE__,"::",__PRETTY_FUNCTION__," line: ",__LINE__);







#define KIMI_LOG_INTERNAL(level,EXPR)           \
  if(kimi::Logger::loggingEnabled(level))       \
  {                                             \
    std::ostringstream os;                      \
    os << EXPR;                                 \
    kimi::Logger::logMessage(level ,os.str());  \
  }                                             \
  else (void) 0

#define KIMI_LOG(THELEVEL,EXPR)                 \
  KIMI_LOG_INTERNAL(kimi::Logger::LEVEL_ ## THELEVEL,EXPR)

#define KIMI_ERROR(EXPR)   KIMI_LOG(ERROR,EXPR)
#define KIMI_VERBOSE(EXPR) KIMI_LOG(VERBOSE,EXPR)
#define KIMI_TRACE(EXPR)   KIMI_LOG(TRACE,EXPR)
#define KIMI_INFO(EXPR)    KIMI_LOG(INFO,EXPR)
#define KIMI_PROFILE(EXPR) KIMI_LOG(TRACE,EXPR)

// Use KIMI_PRIVATE for sensitive tracing
//#if defined(_DEBUG)
#  define KIMI_PRIVATE(EXPR) KIMI_LOG(PRIVATE,EXPR)
// #else
// #  define KIMI_PRIVATE(EXPR) (void)0
// #endif

#endif /* MESSAGEMGR_H_ */
