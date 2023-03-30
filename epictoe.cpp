// Epic TicTacToe.cpp
// Written by David Whittaker

#include <iostream>
#include <cassert>

// Found color codes here https://stackoverflow.com/questions/4053837/colorizing-text-in-the-console-with-c
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[94m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define BRIGHT_RED "\x1B[91m"
#define CYAN "\x1B[36m"
#define END_COLOR "\033[0m"
#define MAX_PLAYERS 2
#define MINE_HIDDEN 1
#define MINE_FOUND 2


using namespace std;

class Board {
public:
    /**
    * Sets up main board
    * params:
    *  m - number of rows
    *  n - number of cols
    *  k - number of spaces to win
    *  mines - number of mines
    **/
    // TODO - think of ways to split up this class
    Board(int m, int n, int k, int mines) {
        board = new int*[m];
        mine_locations = new int*[m];
        for (int i = 0; i < m; i++) {
            board[i] = new int[n];
            mine_locations[i] = new int[n];
        }
        this->m = m;
        this->n = n;
        this->mines = mines;
        this->win = k;
        init_board();
    }

    ~Board() {
        // free memory
        for (int i = 0; i < m; i++) {
            delete[] board[i];
            delete[] mine_locations[i];
        }
        delete[] board;
        delete[] mine_locations;
    }

    /**
    * Initializes all spaces to 0, sets up mines
    **/
    void init_board() {
        int mine_count = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                board[i][j] = 0;
                mine_locations[i][j] = 0;
            }            
        }
        for (int i = 0; i < mines; i++) {
            while (true) {
                int row = rand() % m;
                int col = rand() % n;
                if (!mine_locations[row][col]) {
                    mine_locations[row][col] = MINE_HIDDEN;
                    // cout << "setting mine at location (" << row << ", " << col << ")\n";
                    break;
                }
            }
        }
    }

    /**
    * Prints the current board with M for found mines
    **/
    void print_board() {  
        print_top();
        for (int i = 0; i < m; i++) {
            cout << BLUE << char('A' + i) << YELLOW << " |" << END_COLOR;
            for (int j = 0; j < n; j++) {
                if (mine_locations[i][j] == MINE_FOUND) {
                    cout << RED << " M" << END_COLOR;
                }
                else {
                    switch (board[i][j]) {
                    case 0:
                        cout << "  ";
                        break;
                    case 1:
                        cout << BRIGHT_RED << " X" << END_COLOR;
                        break;
                    case 2:
                        cout << GREEN << " O" << END_COLOR;
                        break;
                    }
                }
                if (j < n) cout << YELLOW << " |" << END_COLOR;
            }
            cout << YELLOW << "\n  +" << END_COLOR;
            if (i < m - 1) {
                for (int i = 0; i < n - 1; i++) {
                    cout << YELLOW << "---+" << END_COLOR;
                }
                cout << YELLOW << "---+\n" << END_COLOR;
            }
        }
        print_line();
    }

    /**
    * Sets a mark on the board
    * params:
    *  m - row number (0-based)
    *  n - column number (0-based)
    *  player - the player number (1 or 2)
    **/
    void set_mark(int m, int n, int player) {
        board[m][n] = player;
    }

    /**
    * Checks whether a player has won the game
    * params:
    *  player - the player number (1 or 2)
    **/
    bool check_win(int player) {
        //check cols
        int count;
        for (int i = 0; i < m; i++) {
            count = 0;
            for (int j = 0; j < n; j++) {
                if (board[i][j] == player) {
                    count++;
                    if (count == win) return true;
                }
                else {
                    count = 0;
                }
            }
        }
        //check rows       
        for (int j = 0; j < n; j++) {
            count = 0;
            for (int i = 0; i < m; i++) {
                if (board[i][j] == player) {
                    count++;
                    if (count == win) return true;
                }
                else {
                    count = 0;
                }
            }
        }
        //check forward diags: 
        for (int i = 0; i <= m - win; i++) {
            count = 0;
            for (int j = 0; j <= n - win; j++) {
                if (board[i][j] == player) {
                    // check next win diagonals
                    for (int k = 0; k < win; k++) {
                        if (board_val(i + k, j + k) == player) {
                            count++;
                            if (count == win) return true;
                        }
                        else {
                            count = 0;
                            break;
                        }
                    }
                }
                else {
                    count = 0;
                }
            }
        }
        //check reverse diags: 
        for (int i = 0; i <= m - win; i++) {
            count = 0;
            for (int j = n; j >= win - 1; j--) {
                if (board[i][j] == player) {
                    // check next win diagonals
                    for (int k = 0; k < win; k++) {
                        if (board_val(i + k, j - k) == player) {
                            count++;
                            if (count == win) return true;
                        }
                        else {
                            count = 0;
                            break;
                        }
                    }
                }
                else {
                    count = 0;
                }
            }
        }
        return false;
    }

    /**
    * Determines whether there are any spaces remaining on the board 
    **/
    bool no_more_moves() {
        int count = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (board[i][j] > 0 || mine_locations[i][j] == MINE_FOUND) {
                    count++;
                }
            }
        }
        return count == m * n;
    }

    /**
    * Gets the number of rows
    **/
    int get_num_rows() {
        return this->m;
    }

    /**
    * Gets the number of columns
    **/
    int get_num_cols() {
        return this->n;
    }

    /**
    * Determines if a mine is visible to the user at a location
    *  params:
    *  row - row number (0-based)
    *  col - column number (0-based)
    **/
    bool is_exploded_mine_at(int row, int col) {
        return mine_locations[row][col] == MINE_FOUND;
    }

    /**
    * Determines if there is a not-yet-visible mine at a location
    *  params:
    *  row - row number (0-based)
    *  col - column number (0-based)
    **/
    bool is_unexploded_mine_at(int row, int col) {
        return mine_locations[row][col] == MINE_HIDDEN;
    }

    /**
    * Sets the mine at a location to visible
    *  params:
    *  row - row number (0-based)
    *  col - column number (0-based)
    **/
    void explode_mine_at(int row, int col) {
        mine_locations[row][col] = MINE_FOUND;
    }

    /**
    * Gets the mark (if any) of a player at a location
    *  params:
    *  row - row number (0-based)
    *  col - column number (0-based)
    **/
    int get_mark_at(int row, int col) {
        return board[row][col];
    }

private:
    int** board;
    int** mine_locations;
    int m, n, win, mines = 0;

    // prints full divider line
    void print_line() {
        for (int j = 0; j < n - 1; j++) {
            cout << YELLOW << "---+" << END_COLOR;
        }
        cout << YELLOW << "---+\n" << END_COLOR;
    }

    // prints top of grid
    void print_top() {        
        cout << "   ";
        for (int j = 0; j < n; j++) {
            cout << BLUE << " " << char('a' + j) << "  " << END_COLOR;
        }
        cout << YELLOW << "\n  +" << END_COLOR;        
        print_line();
    }

    // provides a safe way to get the value of the board at a position without worrying about buffer overflow
    int board_val(int i, int j) {
        if (i >= m || j >= n) {
            return 0;
        }
        return board[i][j];
    }
};

// test cases
void test_game() {
    auto game = new Board(8, 8, 4, 0);
    //column win
    game->set_mark(1, 4, 1);
    game->set_mark(2, 4, 1);
    game->set_mark(3, 4, 1);
    game->set_mark(4, 4, 1);
    game->print_board();
    assert(game->check_win(1) == true);
    game->init_board();
    //row win
    game->set_mark(7, 0, 2);
    game->set_mark(7, 1, 2);
    game->set_mark(7, 2, 2);
    game->set_mark(7, 3, 2);
    game->print_board();
    assert(game->check_win(2) == true);
    //forward diag win
    game->init_board();
    game->set_mark(4, 4, 2);
    game->set_mark(5, 5, 2);
    game->set_mark(6, 6, 2);
    game->set_mark(7, 7, 2);
    game->print_board();
    assert(game->check_win(2) == true);
    //reverse diag win
    game->init_board();
    game->set_mark(4, 7, 2);
    game->set_mark(5, 6, 2);
    game->set_mark(6, 5, 2);
    game->set_mark(7, 4, 2);
    game->print_board();
    assert(game->check_win(2) == true);
}

/**
* Gets the initial board dimensions and parameters from the user and sanitizes
**/
Board* setup_board() {
    int rows = 0, cols = 0, win = 0, mines = 0;
    bool valid_input = false;

    while (!valid_input) {
        cout << "Enter the number of rows, columns, spaces in a row to win, and mines (separated by spaces, e.g, 3 3 3 0): " << END_COLOR;
        if (cin >> rows >> cols >> win >> mines) {
            if (rows < 3 || rows > 26) {
                cout << RED << "Invalid input. The number of rows must be at least 3 but not more than 26.\n" << END_COLOR;
            }
            else if (cols < 3 || cols > 26) {
                cout << RED << "Invalid input. The number of columns must be at least 3 but not more than 26.\n" << END_COLOR;
            }
            else if (win < 1) {
                cout << RED << "Invalid input. The number of spaces to win must be at least 1.\n" << END_COLOR;
            }
            else if (win > rows || win > cols) {
                cout << RED << "Invalid input. The number of spaces to win cannot be larger than the number of rows or columns.\n" << END_COLOR;
            }
            else if (mines > rows * cols) {
                cout << RED << "Invalid input. The number of mines must be less than the number of spaces on the board.\n" << END_COLOR;
            }
            else {
                valid_input = true;
            }
        }
        else
        {
            cout << RED << "Invalid input. Please enter 4 integers separated by spaces, e.g., 3 3 3 0.\n" << END_COLOR;
            cin.clear();
            cin.ignore();
            while ((getchar()) != '\n');
        }
    }
    return new Board(rows, cols, win, mines);
}

/**
* Takes one turn - explodes mine if necessary
**/
void take_turn(Board* board, int player) {
    string space;
    bool valid_input = false;
    int row = 0, col = 0;
    while (!valid_input) {
        cin.clear();
        cin.ignore();
        cout << "Player " << player << " (" << (player == 1 ? "X" : "O") << ")  - it's your turn! Choose a space (e.g., Aa): ";
        if (cin >> space) {
            if (space.length() != 2) {
                cout << RED << "Invalid input. Please enter a valid space (e.g., Aa).\n\n" << END_COLOR;
            }
            else if (space.at(0) < 'A' || space.at(0) > 'Z') {
                cout << RED << "Invalid input. The first character should be a capital letter (e.g., A).\n\n" << END_COLOR;
            }
            else if (space.at(1) < 'a' || space.at(1) > 'z') {
                cout << RED << "Invalid input. The second character should be a lowercase letter (e.g., a).\n\n" << END_COLOR;
            }
            else if (space.at(0) - 'A' >= board->get_num_rows()) {
                cout << RED << "Invalid input. The rows only go from A to " << char(board->get_num_rows() - 1 + 'A') << ".\n\n" << END_COLOR;
            }
            else if (space.at(1) - 'a' >= board->get_num_cols()) {
                cout << RED << "Invalid input. The columns only go from a to " << char(board->get_num_cols() - 1 + 'a') << ".\n\n" << END_COLOR;
            }
            else {
                row = space.at(0) - 'A';
                col = space.at(1) - 'a';
                int player_at_mark = board->get_mark_at(row, col);

                if (board->is_exploded_mine_at(row, col)) {
                    cout << RED << "Invalid space. There's already a mine at this location!\n\n" << END_COLOR;
                }
                else if (player_at_mark > 0) {
                    cout << RED << "Invalid space. Player " << player_at_mark << " has already chosen this location!\n\n" << END_COLOR;
                }
                else {
                    valid_input = true;
                }
            }
        }
        else {
            cout << RED << "Invalid input. Please enter a valid space (e.g, Aa).\n\n" << END_COLOR;
            while ((getchar()) != '\n');
        }
    }

    if (board->is_unexploded_mine_at(row, col)) {
        cout << RED << "**BOOM!!** There's a mine at this location!\n" << END_COLOR;
        board->explode_mine_at(row, col);
    }
    else {
        board->set_mark(row, col, player);
    }
}

/**
* Main entry point for the game
**/
void run_game() {
    auto board = setup_board();
    bool player_has_won = false;
    int player = 1;
    while (!player_has_won) {
        board->print_board();
        take_turn(board, player);
        if (board->check_win(player)) {
            board->print_board();
            cout << CYAN << "*** PLAYER " << player << " WINS! ***" << END_COLOR << "\n";
            break;
        }
        if (board->no_more_moves()) {
            board->print_board();
            cout << CYAN << "*** CAT'S EYE - no more moves ***" << END_COLOR << "\n";
            break;
        }
        player++;
        if (player > MAX_PLAYERS) {
            player = 1;
        }
    }
    delete board;
}

int main()
{
    cout << BLUE << "Welcome to Epic Tic-Tac-Toe!\n" << END_COLOR;
    cout << BRIGHT_RED << "Epic Tic-Tac-Toe" << END_COLOR << " is a thrilling and explosive extension of the classic game we all know and love!";
    cout << "In this exciting twist on the traditional game, you'll face off against your opponents on a board of up to 26 rows and columns, ";
    cout << "providing a challenging and dynamic gameplay experience.But that's not all - hidden throughout the board are dangerous mines ";
    cout << "that can detonate at any moment!If you attempt to claim a space with a mine, it will explode, causing you to lose your turn and ";
    cout << "rendering that space unclaimable for the rest of the game.This adds an element of danger and excitement to the game, as you must ";
    cout << "strategically navigate the board and avoid the hidden dangers while trying to outmaneuver your opponent.\n";
    cout << "Get ready to experience the ultimate test of skill and strategy with " << BRIGHT_RED << "Epic Tic-Tac-Toe!\n\n" << END_COLOR;
    //test_game();
    while (true) {
        run_game();
        string play_again = "Y";
        cout << "\nWould you like to play again? (Y/n)? ";
        cin >> play_again;
        if (play_again == "n" || play_again == "N") {
            cout << "\nGoodbye!";
            return 0;
        }
    }  
}
