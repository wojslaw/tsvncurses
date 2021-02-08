#include "edit_with_ncurses.hpp"


// TODO:
// std::map , from string   to column id with matching header
// edit cells: s, S
// moving cells ,rows, columns around
// adding cells,rows, columns

int CELLWIDTH = 8;


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
		,std::vector<Cell> vec_cells
	 )

{

	int const display_y = start_y;
	int display_x = start_x;




	int col = 0;

	for(Cell cell : vec_cells ) {
		print_cell(display_y,display_x,width,cell,is_highlighted);
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
			,width
			,width
			,false
			,table.table.at(0) );
	attroff(A_REVERSE);

	for( int c = 0; c < (int)table.table.at(0).size() ; ++c ) {
		
		mvprintw( 1 , (1+c) * width , "%4d" , c );
	}

	if(start_row >= (int)table.table.size()) {
		start_row = table.table.size();
	}
	if(end_row >= (int)table.table.size()) {
		end_row = table.table.size();
	}



	int y = 1;

	for( int r = start_row ; r < end_row ; ++r ) {
		++y;
		std::vector<Cell> row = table.table.at(r);
		if( r >= table_rows ) {
			return;
		}

		mvprintw( y , 0 , "%4d" , r );


		for( int c = 0 ; c < (int)row.size() ; ++c ) {
			int const x = (1+c)*width;
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
		 WINDOW * w
		,Cell * cell
		)
{
	assert(w);
	assert(cell);

	wmove(w,0,0);
	wprintw(w, "%s\n" , cell->str.c_str() );
	echo();
	noraw();

	char buffer[0x100] = {0};


	wprintw(w, "input new value:\n"  );
	wgetnstr(w,buffer,0xff);

	cell->str = std::string(buffer);
	cell->parse_all();

	noecho();
	raw();

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



	WINDOW * window_edit = newwin(
			4
			,80
			,table.table.size() + 8
			,10
			);



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
			case 's': do_edit_selected_cell = true; break;
		}

		ensure_greater_than( &selection_row , -1 );
		ensure_greater_than( &selection_col , -1 );


		ensure_less_than( &selection_row , table.table.size() );
		ensure_less_than( &selection_col , table.table.at(selection_row).size());


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

		Cell cell = table.at(selection_row , selection_col);
		printw( "%s\n" ,cell.str.c_str());
		if( cell.is_double ) {
			printw( "%f\n" ,cell.as_double);
		}
		if( cell.is_int ) {
			printw( "%d\n" ,cell.as_int);
		}


		if( do_edit_selected_cell ) {
			assert(window_edit);
			edit_selected_cell( window_edit ,  &table.at( selection_row , selection_col ) );

			print_table_rows(
					table
					,row_start
					,row_end
					,selection_row
					,selection_col
					);
		}



		refresh();
		wrefresh(window_edit);

		move( selection_row+2 , (selection_col+1) * CELLWIDTH );

	}


	//printw("\n%s\n" , table.at(0,0).str.c_str()) ;
	//printw("\n%s\n" , table.at(2,0).str.c_str()) ;


	endwin();
	flushinp();

	return table;
}
