/**
 * @file RunTimeComp.cpp
 * @brief This program is used to compare the efficent of different datatypes, however,
 * I didn't obtain any useful result. Maybe there is something wrong in measure way.
 * @date 2020-01-27 16:37:32 
 */

#define SC_INCLUDE_FX
#include <systemc>
#include <ctime>
#include <cstdlib>

#define VAL_NUM 100000*10

using namespace std;
using namespace sc_dt;
int sc_main(int argc, char * argv[]){

	srand(time(0));

	clock_t start_point;
	clock_t end_point;
	double runtime;

	start_point = 0;
	end_point =0;
	short val_short;
	start_point = clock();
	for(int i=0; i<VAL_NUM; i++){
		val_short = (short)rand();
		val_short = val_short*val_short;
	}
	end_point = clock();
	runtime = (double)(end_point - start_point)/CLOCKS_PER_SEC;
	cout << "This program's runtime of val_short: " << runtime << endl;
	
	start_point = 0;
	end_point =0;
	int val_int;
	start_point = clock();
	for(int i=0; i<VAL_NUM; i++){
		val_int = (int)rand();
		val_int = val_int*val_int;
	}
	end_point = clock();
	runtime = (double)(end_point - start_point)/CLOCKS_PER_SEC;
	cout << "This program's runtime of val_int: " << runtime << endl;

	start_point = 0;
	end_point =0;
	unsigned val_unsigned;
	start_point = clock();
	for(int i=0; i<VAL_NUM; i++){
		val_unsigned = (unsigned)rand();
		val_unsigned = val_unsigned*val_unsigned;
	}
	end_point = clock();
	runtime = (double)(end_point - start_point)/CLOCKS_PER_SEC;
	cout << "This program's runtime of val_unsigned: " << runtime << endl;

	start_point = 0;
	end_point =0;
	long val_long;
	start_point = clock();
	for(int i=0; i<VAL_NUM; i++){
		val_long = (long)rand();
		val_long = val_long*val_long;
	}
	end_point = clock();
	runtime = (double)(end_point - start_point)/CLOCKS_PER_SEC;
	cout << "This program's runtime of val_long: " << runtime << endl;
	
	start_point = 0;
	end_point =0;
	sc_int<8> sc_int_8;
	start_point = clock();
	for(int i=0; i<VAL_NUM; i++){
		sc_int_8 = (sc_int<8>)rand();
		sc_int_8 = sc_int_8*sc_int_8;
	}
	end_point = clock();
	runtime = (double)(end_point - start_point)/CLOCKS_PER_SEC;
	cout << "This program's runtime of sc_int_8: " << runtime << endl;
	
	start_point = 0;
	end_point =0;
	sc_uint<19> sc_uint_19;
	start_point = clock();
	for(int i=0; i<VAL_NUM; i++){
		sc_uint_19 = (sc_uint<19>)rand();
		sc_uint_19 = sc_uint_19 * sc_uint_19;
	}
	end_point = clock();
	runtime = (double)(end_point - start_point)/CLOCKS_PER_SEC;
	cout << "This program's runtime of sc_uint_19: " << runtime << endl;
	
	start_point = 0;
	end_point =0;
	sc_bigint<8> sc_bigint_8;
	start_point = clock();
	for(int i=0; i<VAL_NUM; i++){
		sc_bigint_8 = (sc_bigint<8>)rand();
		sc_bigint_8 = sc_bigint_8 * sc_bigint_8;
	}
	end_point = clock();
	runtime = (double)(end_point - start_point)/CLOCKS_PER_SEC;
	cout << "This program's runtime of sc_bigint_8: " << runtime << endl;
	
	start_point = 0;
	end_point =0;
	sc_bigint<100> sc_bigint_100;
	start_point = clock();
	for(int i=0; i<VAL_NUM; i++){
		sc_bigint_100 = (sc_bigint<100>)rand();
		sc_bigint_100 = sc_bigint_100 * sc_bigint_100;
	}
	end_point = clock();
	runtime = (double)(end_point - start_point)/CLOCKS_PER_SEC;
	cout << "This program's runtime of sc_bigint_100: " << runtime << endl;
	
	start_point = 0;
	end_point =0;
	sc_fixed<12,12> sc_fixed_12_12;
	start_point = clock();
	for(int i=0; i<VAL_NUM; i++){
		sc_fixed_12_12 = (sc_fixed<12,12>)rand();
		sc_fixed_12_12 = sc_fixed_12_12 * sc_fixed_12_12;
	}
	end_point = clock();
	runtime = (double)(end_point - start_point)/CLOCKS_PER_SEC;
	cout << "This program's runtime of sc_fixed_12_12: " << runtime << endl;
	
	start_point = 0;
	end_point =0;
	sc_fixed<24,24> sc_fixed_24_24;
	start_point = clock();
	for(int i=0; i<VAL_NUM; i++){
		sc_fixed_24_24 = (sc_fixed<24,24>)rand();
		sc_fixed_24_24 = sc_fixed_24_24 * sc_fixed_24_24;
	}
	end_point = clock();
	runtime = (double)(end_point - start_point)/CLOCKS_PER_SEC;
	cout << "This program's runtime of sc_fixed_24_24: " << runtime << endl;

return 0;
}
