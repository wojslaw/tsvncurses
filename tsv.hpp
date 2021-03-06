#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <string>
#include <vector>
#include <map>


#define DELIMITER_CELL '\t'
#define DELIMITER_ROW  '\n'


// TODO write table to file


typedef std::vector< std::string > LinesVector ;


LinesVector lines_from_file(FILE * );
void lines_fprint_to_file(FILE * f , const LinesVector larr);



struct Cell {
	std::string str;
	bool is_str(void) const ;
	const char * c_str(void) const { return str.c_str(); } // TODO use only c_str from these 2
	const char * as_str(void) const { return str.c_str(); }
	bool is_int = false;
	int  as_int = 0;
	bool   is_double = false;
	double as_double = 0;


	// seems like a simple way to create tables for values 
	bool     is_ptr_to_double = 0;
	double * ptr_to_double = 0;
	bool  is_ptr_to_int = 0;
	int * ptr_to_int = 0;


	bool is_cell_proper(void) const;

	bool parse_as_int() ;
	bool parse_as_double() ;

	void parse_all(void);


	Cell(const char * s) ;

	Cell(const char * strptr , size_t length_of_string) ;

};







struct Table {
	//std::string name; // TODO
	std::vector< std::vector< Cell > > table;
	std::map< const char *, int > map_of_columns ;

	int cols = 0;
	int rows = 0;

	Cell & at(size_t const row , size_t const col) ;
	void fprint(FILE * f) const ;

	Table(const LinesVector & linevec);

	void parse_all_cells(void); // TODO

	void fprint_row(FILE * , int r) const;
};


Table table_from_file(FILE * f);

