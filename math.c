// math.c
/*
	Math related functions

	Written by maniek86 2022 (c) 
*/

const double PI=3.1415926535897932384650288;

double sin(double x){ 
  double answer;
  __asm__ ("fsin" :"=t" (answer) : "0" (x));
  return answer;
}

double cos(double x){
  double answer;
  __asm__ ("fcos" :"=t" (answer) : "0" (x));
  return answer;

}

double sqrt (double x) {
  double res;
  __asm__ ("fsqrt" : "=t" (res) : "0" (x));
  return res;
}
