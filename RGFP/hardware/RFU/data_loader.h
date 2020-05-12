#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include "systemc.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Data_loader
{
	private:
		string m_filename;
		
	public:
		Data_loader
		( string filename
		)
		: m_filename(filename)
		{	
		}

		void load_data(uint8_t *dst, uint32_t size) 
		{

			ifstream fin(m_filename, ios_base::in | ios_base::binary);	
			fin.read((char *)dst, size);
			fin.close();
		}

		void extract_data(uint8_t *src, uint32_t size)
		{
			ofstream fout("extract_data.txt", ios_base::out | ios_base::binary);
			fout.write((char *)src, size);	
			fout.close();
		}
};

#endif
