#include "edit_with_ncurses.hpp"





void print_cell(
		 int const y
		,int const x
		,int const width
		,Cell cell
		,bool const is_highlighted
		)
{
	std::string displayed_cell;
	displayed_cell.resize(width + 1);

	move(y,x);
	/* for(int i = 0; i < width; ++i ) { */
	/* 	displayed_cell[i] = cell.str[i]; */
	/* } */
	if(is_highlighted) {
		attron(A_BOLD);
	}
	printw( "%s\t" , cell.str.c_str() );
	if(is_highlighted) {
		attroff(A_BOLD);
	}
}




void
print_row(
		 int const start_y
		,int const start_x
		,int const width
		,bool const is_highlighted
		,int const selection_col
		,std::vector<Cell> vec_cells
	 )

{

	int const display_y = start_y;
	int display_x = start_x;




	int col = 0;

	for(Cell cell : vec_cells ) {
		bool const is_col_highlighted = (
				is_highlighted
				);
		print_cell(display_y,display_x,width,cell,true);
		display_x += width;
		col++;
	}

}




void
print_table_rows(
		 Table table
		,int const start_row
		,int const end_row
		,int const selection_row
		,int const selection_col
		)
{
	assert( end_row >= start_row );
	erase();

	int const table_rows = table.table.size();

	attron(A_REVERSE);
	print_row(
			 0
			,0
			,8
			,-1
			,-1
			,table.table.at(0) );
	attroff(A_REVERSE);


	for( int i = 1 ; i < (end_row - start_row) ; ++i ) {
		int const row = start_row + i;
		if( row >= table_rows ) {
			return;
		}
		
		print_row (
				 i
				,0
				,8
				,true // (selection_row == row)
				,selection_col
				,table.table.at(row) );
	}
}



Table
edit_with_ncurses(Table table)
{
	initscr();
	noecho();
	raw();


	int selection_row  = 2;
	int selection_col = 0;

	int display_y = 0;

	print_table_rows(
			table
			,selection_row
			,selection_row + 4
			,selection_row
			,selection_col
			);

	refresh();


	printw("\n%s\n" , table.at(0,0).str.c_str()) ;
	printw("\n%s\n" , table.at(2,0).str.c_str()) ;

	getch();

	endwin();
	flushinp();

	return table;
}
