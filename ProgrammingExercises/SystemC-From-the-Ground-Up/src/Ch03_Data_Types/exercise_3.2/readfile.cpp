/**
 * @file readfile.cpp
 * @brief read data from file, and output values in format of SC_DEC and SC_HEX_SM
 * @details This program provides two types of implementation ways, after comparision,
 * directly assigning file-data to SystemC value can assign hex-format data to SystemC
 * value without using "cin >> hex".
 * @date 2020-01-27 10:42:02 
 * @author Yu Zhenbo  
 * @note if you use setw() to format output, remember include iomanip.h
 */
#include <systemc>
#include <fstream>
#include <iomanip>

using namespace sc_dt;
using namespace std;

int sc_main(int argc, char *argv[]){
	sc_uint<32> sc_data[10];
	uint data = 0; ///< Using C++ native datatype as a container
	ifstream fin;
	fin.open("exercise.dat");
	if(!fin.is_open()){
		cout << "Failed to open data file!!!"<<endl;
		return 0;
	}	
	int i,j = 0;
//	fin >> hex; ///< it decides how to explain input data;

	cout << "use C++ native datatype as a container..." << endl;
	while(fin >> data){
		sc_data[i] = data;
		cout << std::setw(10)<< "DEC format: " << std::setw(10) << sc_data[i].to_string(SC_DEC, 1) << endl;
		cout << std::setw(10)<< "HEX_SM: " << std::setw(10) << sc_data[i].to_string(SC_HEX_SM,1) << endl;
		i++;
	}
	cout << endl;
	fin.close();

	cout << "directly assign file-data to SystemC datatype..."<< endl;
	fin.open("exercise.dat");
	while(fin >> sc_data[j]){
		cout << std::setw(10)<< "DEC format: " << std::setw(10) << sc_data[j].to_string(SC_DEC, 1) << endl;
		cout << std::setw(10)<< "HEX_SM: " << std::setw(10) << sc_data[j].to_string(SC_HEX_SM,1) << endl;
		i++;
	}
	cout << endl;
	fin.close();

	return 0;
}
