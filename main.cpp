#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <math.h>

#include <unistd.h>

#include <string>
#include <vector>

#include <getopt.h>




#include "tsv.hpp"
#include "edit_with_ncurses.hpp"

bool SHOW_TABLES = false;

int
fprint_double_hexadecimal(
		 FILE * f
		,double d
		)
{
	char sign = '+';

	if( d < 0.0 ) {
		sign = '-';
		d = -d;
	}

	double part_integer=0;
	double part_real   =0;

	part_real = modf(d ,&part_integer );

	const double percentage_base = 0x100;

	double hex_percentage = 0;
	if( part_real > 0.0 ) {
		hex_percentage = (percentage_base * part_real) ;
	}


	return fprintf(f, "%c0x%x.%x" , sign , (int)part_integer , (int)hex_percentage );
}




int main(int argc , char *argv[])
{
	std::vector< const char * > vector_filenames;

	int opt;
	while((opt = getopt(argc,argv, "")) != -1) {
	}

	// getopt reorders the argv, so that all the non-option arguments are at the end, after last option
	if( optind < argc ) {
		for( int i = optind; i < argc; ++i ) {
			vector_filenames.push_back(argv[i] );
		}
	}



	std::vector< Table > vector_table;

	for( const char * s : vector_filenames )  {
		FILE * f = fopen(s,"r");
		if( f == 0 ) {
			fprintf(stderr,"failed to open file for reading: %s" , s);
		}
		vector_table.emplace_back(table_from_file(f));
		fclose(f);

		
		if(!SHOW_TABLES) {
			continue;
		}


		printf( "%s rows: %zd\n" , s , vector_table.back().table.size());


		printf( "### print:\n" );
		for( auto const & row : vector_table.back().table ) {
			for( auto const & c : row ) {
				if(c.is_double) {
					fprint_double_hexadecimal(stdout, c.as_double );
				} else {
					printf("%s\t" , c.str.c_str() );
				}
			}
			printf("\n");
		}
		printf( "### are cells proper?\n" );
		for( auto const & row : vector_table.back().table ) {
			for( auto const & c : row ) {
				printf("%d\t" , c.is_cell_proper() );
			}
			printf("\n");
		}
	}


	Table table_after_edit = edit_with_ncurses(vector_table.at(0));

}
