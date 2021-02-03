#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>

#include <string>
#include <vector>

#include <getopt.h>

#include "tsv.hpp"




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

		printf( "%s rows: %zd\n" , s , vector_table.back().table.size());
		vector_table.back().fprint(stdout);
	}


}
