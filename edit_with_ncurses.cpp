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
		,int start_row
		,int end_row
		,int const selection_row
		,int const selection_col
		)
{
	assert( end_row >= start_row );
	erase();

	int const width = 8;
	int const table_rows = table.table.size();

	attron(A_REVERSE);
	print_row(
			 0
			,0
			,width
			,-1
			,-1
			,table.table.at(0) );
	attroff(A_REVERSE);

	if(start_row >= table.table.size()) {
		start_row = table.table.size();
	}
	if(end_row >= table.table.size()) {
		end_row = table.table.size();
	}



	int y = 1;

	for( int r = start_row ; r < end_row ; ++r ) {
		++y;
		std::vector<Cell> row = table.table.at(r);
		if( r >= table_rows ) {
			return;
		}

		for( int c = 0 ; c < row.size() ; ++c ) {
			int const x = c*width;
			bool const is_highlighted
				= (
				(r == selection_row)
				&&
				(c == selection_col) );
			//printw( "\n(%d,%d)(%d,%d)\n" , r , c , y , x );
			print_cell(
					 y
					,x
					,width
					,row.at(c)
					,is_highlighted
					);
		}


	}
}




void
edit_selected_cell(
		Cell * cell
		)
{
	// TODO
}


void
ensure_less_than(
	 int * x
	,int const v
	)
{
	assert(x);
	if( *x >= v ) {
		*x = v-1;
	}
}



void
ensure_greater_than(
	 int * x
	,int const v
	)
{
	assert(x);
	if( *x <= v ) {
		*x = v+1;
	}
}


Table
edit_with_ncurses(Table table)
{
	initscr();
	noecho();
	raw();
	keypad(stdscr,true);


	int selection_row  = 1;
	int selection_col = 1;
	// TODO selecting and editing out of bounds

	int display_y = 0;



	for( int ch = ERR;
		ch != 3; // ^c
		ch = getch()
	   ) {

		bool do_edit_selected_cell = false;



		switch(ch) {
			case 'k': selection_row--; break;
			case 'j': selection_row++; break;
			case 'h': selection_col--; break;
			case 'l': selection_col++; break;
		}

		ensure_greater_than( &selection_row , -1 );
		ensure_greater_than( &selection_col , -1 );


		ensure_less_than( &selection_row , table.table.size() );
		ensure_less_than( &selection_col , table.table.at(selection_row).size());

		if( do_edit_selected_cell ) {
			edit_selected_cell( &table.at( selection_row , selection_col ) );
		}

		int const row_start = 0;
		int const row_end   = INT_MAX;

		print_table_rows(
				table
				,row_start
				,row_end
				,selection_row
				,selection_col
				);


		printw( "\nselection:%d %d\n" , selection_row , selection_col );
		refresh();
	}


	//printw("\n%s\n" , table.at(0,0).str.c_str()) ;
	//printw("\n%s\n" , table.at(2,0).str.c_str()) ;


	endwin();
	flushinp();

	return table;
}
