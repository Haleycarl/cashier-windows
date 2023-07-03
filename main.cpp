#include<unistd.h>
#include<vector>
#include"include/curses.h"
#include<string>
#include"logicfunc.h"
#include<ctime>
// Handling input
Logic cashier;
time_t now = time(0);
std::string dt = ctime(&now);

struct productinfo{
	std::string name;
	std::string code;
	std::string price;
};

int main()
{
	//create new window on the left
	initscr();
	int row,col;
	char operation;
	operation_menu:
	curs_set(0);
	getmaxyx(stdscr,row,col);
	WINDOW * operation_window = newwin(row*.50,col*.50,row*.25,col*.25);
	box(operation_window,0,0);
	wattron(operation_window,A_BOLD);
	mvwprintw(operation_window,0,1,"Simple Cashier System");
	wattroff(operation_window,A_BOLD);
	mvwprintw(operation_window,1,1,"Please Select Operation");
	mvwprintw(operation_window,2,1,"I = Manual Input Mode");
	mvwprintw(operation_window,3,1,"E = Edit Mode");
	mvwprintw(operation_window,4,1,"D = Delete Mode");
	mvwprintw(operation_window,5,1,"X = Exit");
	refresh();
	wrefresh(operation_window);
	operation = wgetch(operation_window);
	delwin(operation_window);
//	fprintf(stderr,"recieved input %c\n",operation); // For Scan Mode
	if(operation == 'i' || operation == 'I'){
		cashier.clearevery();
		std::vector<productinfo> products;
		int i_row,i_col,l_row,l_col;
		refresh();
		std::string input;
		// Basic windows for scan mode
//		fprintf(stderr,"Doing scan mode\n");
		scan_input:
		WINDOW * listing_window = newwin(0,col*.50,0,0);
		getmaxyx(listing_window,l_row,l_col);
		scrollok(listing_window,TRUE);
		box(listing_window,0,0);
		mvwprintw(listing_window,0,1,"List");
		for(int i = 0 ; i < products.size() ; i++ ){
			wmove(listing_window,1+i*2,1);
			wattron(listing_window,A_BOLD);
			wprintw(listing_window,"%s",products[i].code.c_str());
			wattroff(listing_window,A_BOLD);
			wmove(listing_window,2+i*2,1);
			wprintw(listing_window,"%s",products[i].name.c_str());
			wmove(listing_window,2+i*2,l_col-products[i].price.size()-1);
			wprintw(listing_window,"%s",products[i].price.c_str());
		}
		wattroff(listing_window,A_BOLD);

		WINDOW * input_window = newwin(row*.50,col*.50,0,col*.50);
		box(input_window,0,0);
		wattron(input_window,A_BOLD);
		mvwprintw(input_window,0,1,"Input");
		wattroff(input_window,A_BOLD);
		mvwprintw(input_window,1,1,"Insert mode, Insert the code in the Input window");
		mvwprintw(input_window,2,1,"Type your product code to see the product name");
		mvwprintw(input_window,3,1,"press X once you are done,");
		mvwprintw(input_window,4,1,"press W for payment process");
		mvwprintw(input_window,7,1,"Code: %s",cashier.prodcode().c_str());
		mvwprintw(input_window,8,1,"Name: %s",cashier.prodname().c_str());
		mvwprintw(input_window,9,1,"Price: %f", cashier.prodprice());
		// mvwprintw(input_window,1,1,dt);

		WINDOW * total_window = newwin(row*.50,col*.50,row*.50,col*.50);
		box(total_window,0,0);
		wattron(total_window,A_BOLD);
		mvwprintw(total_window,0,1,"Total");
		wattroff(total_window,A_BOLD);
		mvwprintw(total_window,1,1,"Date: %s",dt.c_str());
		mvwprintw(total_window,2,1,"Total: %f",cashier.prodtotal());

		// Refreshing the windows
		curs_set(1);
		refresh();
		wrefresh(listing_window);
		wrefresh(total_window);
		wrefresh(input_window);
		getmaxyx(input_window,i_row,i_col);
		wmove(input_window,2,2);
		char scaninput[90];
		mvwprintw(input_window,i_row-2,1,"Scan: ");
		mvwscanw(input_window,i_row-2,7,"%s",scaninput);
		input = std::string(scaninput);
		if(input[0] == 'x' or input[0] == 'X'){ // if input is x then exit back to operation mode
			products.clear();
			goto operation_menu;
		}
		if(input[0] == 'w' or input[0] == 'W'){
			delwin(input_window);
			delwin(total_window);
			delwin(listing_window);
			int p_row,p_col;
			float payment;
			WINDOW * pay_window = newwin(row*.50,col*.50,row*.25,col*.25);
			getmaxyx(pay_window,p_row,p_col);
			box(pay_window,0,0);
			mvwprintw(pay_window,0,1,"Payment");
			mvwprintw(pay_window,1,1,"Please enter the amount you want to pay");
			mvwprintw(pay_window,2,1,"Current Date: %s",dt.c_str());
			mvwprintw(pay_window,4,1,"Items: %d",products.size());
			mvwprintw(pay_window,5,1,"Total: %f",cashier.prodtotal());
			mvwprintw(pay_window,6,1,"Change: ");
			char c_payment[80];
			p_repeat:
			mvwprintw(pay_window,p_row-2,1,"Payment:          ");
			wrefresh(pay_window);
			mvwscanw(pay_window,p_row-2,10,"%s",c_payment);
			payment = atof(c_payment);
			if(payment < cashier.prodtotal()){
				mvwprintw(pay_window,p_row-3,1,"Payment is too low please pay higher price");
				goto p_repeat;
			}
			mvwprintw(pay_window,6,1,"Change: %f",payment-cashier.prodtotal());
			wattron(pay_window,A_BLINK);
			mvwprintw(pay_window,p_row-2,1,"-Press anything to continue-");
			wattroff(pay_window,A_BLINK);
			products.clear();
			cashier.clearevery();
			wrefresh(pay_window);
			wgetch(pay_window);
		}
		// call for class of cashier where you scan things
		// from logic.h
		// Continuation later ^-^
		cashier.scan(input);
		if(cashier.prodname() != ""){
			struct productinfo info;
			info.code = cashier.prodcode();
			info.name = cashier.prodname();
			info.price = std::to_string(cashier.prodprice());
			products.push_back(info);
		}
		goto scan_input;
	}
	if(operation == 'e' || operation == 'E'){
		int e_row, e_col;
		curs_set(1);
		e_repeat:
		refresh();
//		fprintf(stderr,"Doing Editting mode\n");
		WINDOW * editting_window = newwin(row*.5,col*.5,row*.25,col*.25);
		box(editting_window,0,0);
		getmaxyx(editting_window,e_row,e_col);
		mvwprintw(editting_window,0,1,"Editting Mode");
		mvwprintw(editting_window,1,1,"Welcome to editting mode, Please enter the code you want to scan");
		mvwprintw(editting_window,2,1,"Press x if you want to exit Editting mode");
		wmove(editting_window,1,1);
		wrefresh(editting_window);
		std::string code,name,price;
		char scaninput[256];
		mvwprintw(editting_window,e_row-2,1,"Code:                          ");
		mvwscanw(editting_window,e_row-2,7,"%s",scaninput);
		code = std::string(scaninput);
		if(code[0] == 'x' or code[0] == 'X'){ // check if x then return to operation menu
			goto operation_menu;
		}
		mvwprintw(editting_window,e_row-2,1,"Name:                      ");
		mvwscanw(editting_window,e_row-2,7,"%s",scaninput);
		name = std::string(scaninput);
		price_in:
		mvwprintw(editting_window,e_row-2,1,"Price:                   ");
		mvwscanw(editting_window,e_row-2,8,"%s",scaninput);
		price = std::string(scaninput);
		// check if price is a number
		for(char a : price){
			if(!(a<=57 or a >= 48 or a==46)){
				attron(A_BOLD);
				mvwprintw(editting_window,2,1,"Price is not a number Please try again");
				attroff(A_BOLD);
				goto price_in;
			}
		}
		cashier.scan(code);
		if(cashier.prodname() != ""){ // check if theres a duplicate
			char decide;
			noecho();
			curs_set(0);
			mvwprintw(editting_window,e_row-2,1,"Duplicate detected, do you want to overwrite?[y/n]: ");
			decide:
			decide = wgetch(editting_window);
			if(decide == 'y' or decide == 'Y'){
				cashier.erase(code);
			}
			else if(decide == 'n' or decide == 'N'){
				echo();
				curs_set(1);
				goto e_repeat;	
			}
			else{
				mvwprintw(editting_window,e_row-2,1,"Decision not valid try again");
				goto decide;
			}
		}
		cashier.insert(code,name,price);
		echo();
		curs_set(1);
		goto e_repeat;
	}
	if(operation == 'd' || operation == 'D'){
		int d_row, d_col;
		curs_set(1);
		std::string input;
		char dinput[80];
//		fprintf(stderr, "Doing Delete mode\n");
		WINDOW * deleting_window = newwin(row*.5,col*.5,row*.25,col*.25);
		box(deleting_window,0,0);
		getmaxyx(deleting_window,d_row,d_col);
		wattron(deleting_window,A_BOLD);
		mvwprintw(deleting_window,0,1,"Deleting Mode");
		wattroff(deleting_window,A_BOLD);
		mvwprintw(deleting_window,1,1,"Welcome to delete mode.");
		mvwprintw(deleting_window,2,1,"please insert the code of the product you want to delete");
		mvwprintw(deleting_window,3,1,"type x and enter if you want to exit");
		wrefresh(deleting_window);
		d_repeat:
		mvwprintw(deleting_window,d_row-2,1,"Code: ");
		mvwscanw(deleting_window,d_row-2,7,"%s",dinput);
		if(dinput[0] == 'x' or dinput[0] == 'X')
			goto operation_menu;
		input = std::string(dinput);
		cashier.erase(input);
		goto d_repeat;
	}
	if(operation == 'x' || operation == 'X'){
		refresh();
		curs_set(1);
		echo();
		endwin();
		return 0;
	}
	endwin();
	goto operation_menu;
}
