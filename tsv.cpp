#include "tsv.hpp"







// IDEA todo eventually: read line from from file 1 by 1, instead of first the whole file
// but that's a "maybe eventually"
// because I want this parser to first ensure that the whole file is read properly, and only then 

// IDEA maybe never because it's not important: configurable row and column delimiter. but the again, it shouldn't be too difficult to implement

// IDEA ncurses display, show a range of columns and rows, maybe edit
// or better yet: instead of editing, it could spit out a script to edit those lines, or a diff?

LinesVector
lines_from_file(FILE * f)
{
	LinesVector lines_array;


	ssize_t retval = 0;
	char * lineptr = 0;
	size_t n = 0;
	while(true) {
		lineptr = 0;
		n = 0;
		retval = getline(&lineptr, &n, f);
		if(retval == -1) {
			return lines_array;
		}
		lineptr[retval-1] = '\0';
		lines_array.emplace_back(std::string(lineptr));
		free(lineptr);
	}

}





void
lines_fprint_to_file(
		 FILE * f
		,const LinesVector larr
		)
{
	for( const std::string & str : larr ) {
		fputs( str.c_str() , f );
		fputc(DELIMITER_ROW,f);
	}
}





// 





bool
Cell::is_cell_proper(void)
const
{
	// check, to see if there are any forbidden characters: tabs, newlines, nullbytes
	// maybe it would make sense to only allow printable characters?
	// or maybe not: if I checked for isprint(), then UTF-8 stuff would signal an error
	for( const char c : str  ) {
		if( c == DELIMITER_CELL ) {
			return false;
		}
		if( c == DELIMITER_ROW ) {
			return false;
		}
		if( c == '\0' ) {
			return false;
		}
	}
	// ensure that the cell is not both a pointer and normal value
	if( (is_int || is_double || str.size() !=0)
			&&
			(is_ptr_to_double || is_ptr_to_int) ) {
		return false;
	}

	if( is_ptr_to_double && (ptr_to_double == 0) ) {
		return false;
	}
	if( is_ptr_to_int && (ptr_to_int == 0) ) {
		return false;
	}

	if( !is_ptr_to_double && (ptr_to_double != 0) ) {
		return false;
	}
	if( !is_ptr_to_int && (ptr_to_int != 0) ) {
		return false;
	}

	return true;
}




Cell&
Table::at(
		size_t const row
		, size_t const col
		)
{
	return table.at(row).at(col);
}

void Table::fprint(FILE * f) const {
	for( const auto & row : table ) {
		for( const auto & cell : row ) {
			fputs(cell.str.c_str() , f);
			fputc(DELIMITER_CELL , f);
		}
		fputc(DELIMITER_ROW , f);
	}
}




















std::vector< Cell >
vector_of_cells_from_line_string(
		std::string line
		)
{
	std::vector< Cell > vec_cells;
	const char * strptr  = line.c_str();
	const char * const strptr_end  = line.c_str() + line.size() - 0;
	const char * strptr_next_delimiter = strchrnul(strptr , DELIMITER_CELL) ;
	while(
			strptr_next_delimiter != 0
			&&
			strptr < strptr_end
			&&
			strptr != 0
			) {
		const size_t length_of_string = ( strptr_next_delimiter - strptr);

		vec_cells.emplace_back(Cell(strptr , length_of_string));


		strptr = strptr_next_delimiter + 1;
		strptr_next_delimiter = strchrnul(strptr , DELIMITER_CELL) ;

	}
	return vec_cells;
}




Table::Table(
		const LinesVector & linevec
		)
{
	//for cutting on tabs: strchr
	size_t count_rows = 0;
	table.reserve(linevec.size());
	for(std::string line_of_input : linevec) {
		table.emplace_back(vector_of_cells_from_line_string(line_of_input));
		++count_rows;
	}

	printf( "Table rows: %zd\n" , count_rows  );
}




Table table_from_file(
		FILE * f
		)
{
	LinesVector linevec = lines_from_file(f);
	return Table(linevec);
}





void
Table::fprint_row (
		FILE * f
		,int r
) const {
	for( auto const & cell : table.at(r) ) {
		fputs(cell.as_str() , f);
		fputc(DELIMITER_CELL,f);
	}
	fputc(DELIMITER_ROW,f);
}





















bool
Cell::is_str(
		void
	  )
	const
{
	return (str.size() > 0);
}


const char *
Cell::as_str(
		void
		)
	const
{
	return str.c_str();
}


bool
Cell::parse_as_int(
		void
		)
{
	if(str.size() == 0){
		return false;
	}
	errno = 0;
	as_int = strtol(str.c_str(),0,0);
	if(errno == 0) {
		// I think it wouldn't be logical to signal errors in here, at least for now,
		// just say it wasn't parsed
		is_int = true;
	}
	return is_int;
}


bool
Cell::parse_as_double(void)
{
	if(str.size() == 0){
		return false;
	}
	errno = 0;
	as_double = strtod(str.c_str(),0);
	if(errno == 0) {
		// I think it wouldn't be logical to signal errors in here, at least for now,
		// just say it wasn't parsed
		is_double = true;
	}
	return is_double;
}




Cell::Cell(
		const char * s)
{
	str = std::string(s);
	assert(is_cell_proper());
	parse_as_int();
	parse_as_double();
}




Cell::Cell(
		const char * strptr , size_t length_of_string
	)
{
	str.resize(length_of_string);
	strncpy(&str[0] //char *dest
			,strptr //const char *src
			,length_of_string  ////size_t n);
	);
	assert(is_cell_proper());
}

