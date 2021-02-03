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
	int row_start = 0;
	int row_end = 0;
	if( argc == 3 ) {
		row_start = strtol(argv[1],0,0);
		row_end   = strtol(argv[2],0,0);
	}




	LinesVector larr = lines_from_file(stdin);
	printf( "Lines: %zd\n" , larr.size() );

	Table t = Table(larr);
	//t.fprint(stdout);


	if(row_start > 0 && row_end > 0) {
		t.fprint_row(stdout,0);
		assert( row_start <= row_end );
		assert( (size_t)row_start  < t.table.size() );
		assert( (size_t)row_end    < t.table.size() );
		for( int w = row_start; w <= row_end ; ++w ) {
			t.fprint_row(stdout,w);
		}
	} else {
		t.fprint(stdout);
	}

}
