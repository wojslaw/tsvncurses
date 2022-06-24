#include "edit_with_ncurses.hpp"


// TODO:
// TODO limit number of columns on screen
// TODO only display selected columns to select
// scroll screen!
// ensure no more than 7-8 characters are displayed
// std::map , from string   to column id with matching header
// edit cells: s, S
// moving cells ,rows, columns around
// adding cells,rows, columns
//

int CELLWIDTH = 8;
static bool SHOW_DOUBLE  = false;
static bool SHOW_INT  = false;
static bool FOLLOW_CURSOR_WITH_WINDOW  = true;



int int_smaller(
	 int const a
	,int const b)
{
	if(a < b) {
		return a;
	}
	return b;
}


int int_greater(
	 int const a
	,int const b)
{
	if(a > b) {
		return a;
	}
	return b;
}








void wprint_cell(
		 WINDOW * w
		,int const y
		,int const x
		,int const width
		,Cell cell
		,bool const is_highlighted
		)
{
	std::string displayed_cell;
	displayed_cell.resize(width + 1);

	wmove(w,y,x);
	if(is_highlighted) {
		wattron(w,A_BOLD);
	}
	char stringbuffer[CELLWIDTH] = {0};
	strncpy((stringbuffer) , cell.str.c_str(), CELLWIDTH-1);
	wprintw( w, "%7s\t" , stringbuffer );

	if(is_highlighted) {
		wattroff(w,A_BOLD);
	}
}

















void
wprint_row(
		 WINDOW * w
		,int const start_y
		,int const start_x
		,int const width
		,int const col_start
		,int const col_end
		,bool const is_highlighted
		,const std::vector<Cell> & vec_cells
	 )

{

	int const display_y = start_y;
	int display_x = start_x;

	int col = 0;
	assert(col_end < (int)vec_cells.size());

	for(int c = col_start ;
			c < col_end ;
			++c ) {
		const Cell & cell = vec_cells.at(c);
		wprint_cell(w,display_y,display_x,width,cell,is_highlighted);
		display_x += width;
		col++;
	}

}












	void
wprint_table_rows(
		 WINDOW * w
		,const Table & table
		,int row_start
		,int row_end
		,int col_start
		,int col_end
		,int const selection_row
		,int const selection_col
		)
{
	assert(col_start >= 0);
	assert(row_start >= 0);
	assert(w);
	assert( row_end >= row_start );
	assert( col_end >= col_start );
	werase(w);

	int const width = CELLWIDTH;
	int const table_rows = table.table.size();


	assert(row_start < (int)table.table.size());
	row_start = int_greater( row_start , 0);
	row_end = int_greater( row_end , 0);
	row_start = int_smaller( row_start ,(table.table.size()-1) );
	row_end = int_smaller( row_end ,(table.table.size()-1) );


	int y = 0; //for ncurses
	int x = 0;
	for( int r = row_start ; r <= row_end ; ++r ) { // note the <= : I had huge trouble because this used to be 'r<row_end', and I was wondering why it doesn't show the last row. but this also requires being vigilant about the size vs index
		y = r - row_start;
		const std::vector<Cell> & row = table.table.at(r);
		if( r >= table_rows ) {
			return;
		}

		wmove(w,y,0);

		assert(col_end >= col_start);
		col_start = int_greater(col_start, 0);
		col_end = int_greater(col_end, 0);
		col_start = int_smaller(col_start, row.size()-1);
		col_end = int_smaller(col_end, row.size()-1);

		for( int c = col_start ; c <= col_end ; ++c ) {
			x = (c-col_start)*width;
			bool const is_highlighted
				= (
				(r == selection_row)
				&&
				(c == selection_col) );
			wprint_cell(
					 w
					,y
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







Table
edit_with_ncurses(Table table)
{
	initscr();
	noecho();
	raw();
	keypad(stdscr,true);


	int selection_row = 0;
	int selection_col = 0;
	// TODO selecting and editing out of bounds


	int const count_displayable_table_rows = (LINES-20); // 20 is just a random number that hopefully lets the windows underneath appear
	int const count_displayable_table_cols = ((COLS / CELLWIDTH) - 2); // leave some space on the right to ensure it displays correctly

	/* int const count_displayable_table_rows = 2; */
	/* int const count_displayable_table_cols = 2; */

	//add a little padding:
	int const windowsize_table_rows = 2+count_displayable_table_rows;
	int const windowsize_table_cols = 2+(count_displayable_table_cols*CELLWIDTH);


	// WINDOW *newwin(
	// int nlines, int ncols,
	// int begin_y, int begin_x);

	WINDOW * window_table = newwin( // TODO
			 windowsize_table_rows
			,windowsize_table_cols
			,0
			,0
			);

	WINDOW * window_edit = newwin(
			 4
			,80
			,count_displayable_table_rows
			,10
			);

	WINDOW * window_cellview = newwin( // TODO
			 8
			,80
			,count_displayable_table_rows + 4
			,10
			);


	// allow offset, so that I can move the window around
	int row_start = 0;
	int col_start = 0;



	for( int ch = ERR;
		ch != 3; // ^c
		ch = getch()
	   ) {


		bool do_edit_selected_cell = false;

		switch(ch) {
			case 'k':  selection_row--; break;
			case 'j':  selection_row++; break;
			case 'h':  selection_col--; break;
			case 'l':  selection_col++; break;
			case 'K':  case KEY_UP:    row_start--; break;
			case 'J':  case KEY_DOWN:  row_start++; break;
			case 'H':  case KEY_LEFT:  col_start--; break;
			case 'L':  case KEY_RIGHT: col_start++; break;
			case 's':  do_edit_selected_cell = true; break;
			// gG
			case 'g':
					   {
						   selection_row = 0;
						   selection_col = 0;
						   row_start = 0;
						   col_start = 0;
					   } break;
			case 'G':
					   {
						   selection_row = table.rows;
						   selection_col = table.cols;
						   row_start = table.rows - count_displayable_table_rows;
						   col_start = table.cols - count_displayable_table_cols;
					   } break;
		}


		int const row_end   = (row_start + count_displayable_table_rows) - 1 ;
		int const col_end   = (col_start + count_displayable_table_cols) - 1 ;

		row_start     = int_greater( row_start , 0 );
		col_start     = int_greater( col_start , 0 );
		selection_row = int_greater( selection_row , 0 );
		selection_col = int_greater( selection_col , 0 );

		selection_row = int_smaller(
				selection_row
				, (table.table.size())-1 );
		selection_col = int_smaller(
				selection_col ,
				(table.table.at(selection_row).size())-1 );
		row_start = int_smaller(row_start ,(table.table.size())-1);
		col_start = int_smaller(col_start ,(table.table.at(selection_row).size())-1);

		// edit cell
		if( do_edit_selected_cell ) {
			assert(window_edit);
			edit_selected_cell( window_edit ,  &table.at( selection_row , selection_col ) );
			ch = ERR;
		}





		//cellview
		werase(window_cellview);
		wmove(window_cellview,0,0);
		wprintw(window_cellview, "\nselection:%d %d" , selection_row , selection_col );
		wprintw(window_cellview, " (offset:%d %d\n" , row_start , col_start );
		wprintw(window_cellview, "(row: %s)\n(col: %s)\n\n"
				, table.at(selection_row,0).c_str() 
				, table.at(0,selection_col).c_str() );

		Cell cell = table.at(selection_row , selection_col);
		wprintw(window_cellview, "%s\n" ,cell.str.c_str());
		if( SHOW_DOUBLE &&  cell.is_double ) {
			wprintw(window_cellview, "%f\n" ,cell.as_double);
		}
		if( SHOW_INT && cell.is_int ) {
			wprintw(window_cellview, "%d\n" ,cell.as_int);
		}




		// table
		werase(window_table);
		wprint_table_rows(
				 window_table
				,table
				,row_start
				,row_end
				,col_start
				,col_end
				,selection_row
				,selection_col
				);



		refresh();
		wrefresh(window_edit);
		wrefresh(window_table);
		wrefresh(window_cellview);

		move( (selection_row-row_start) , (selection_col-col_start) * CELLWIDTH ); // TODO move relative to window_table

	}




	endwin();
	flushinp();

	return table;
}
