// 09/21/2016 - CS 5600 - Meine, Joel
// Homework #1 - Min-Max Pruning

// Cannibals and Missionaries

#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

int sumC = 3;	/* Total number of cannibals */
int sumM = 3;	/* Total number of missionaries */

typedef struct {
	vector<int> this_side; /* This side of the river bank. */
	vector<vector<int>> that_side; /* All possible moves from current side of river bank. */
	int that_side_size; /* The number of possible moves from current side of river bank. */
	bool checked; /* Has this move already been checked? */
} move_state;

int moves_size; /* The number of moves returned by moves. */
int legal_size; /* The number of legal moves from the current position. */
bool end_move; /* Has the final move been reached? */
int checked_count; /* The number of moves to reach the end goal. */
vector<move_state> move_states_west; /* The move state objects for West side of river. */
vector<move_state> move_states_east; /* The move state objects for East side of river. */
int move_states_size_west; /* The number of move state objects saved in move states for West side of river. */
int move_states_size_east; /* The number of move state objects saved in move states for West side of river. */
vector<vector<int>> solution; /* The move sequence to reach the end goal. */

/* Given the current state of each river bank, return all possible next moves. */
vector<vector<int>> moves(vector<int> m)
{
	moves_size = 0; /* Reset */
	int numberC = m[0];	/* Number of cannibals */
	int numberM = m[1];	/* Number of missionaries */
	int boat = m[2];	/* Side of river bank that boat in on; West=1 East=0 */

	int westC;	/* Number of cannibals on West side of river bank */
	int eastC;	/* Number of cannibals on East side of river bank */
	int westM;	/* Number of missionaries on West side of river bank */
	int eastM;	/* Number of missionaries on East side of river bank */

	vector<vector<int>> move_options;	/* Result of the input move based on cannibals, missionaries, and boat */

	/* Is boat on West side of river bank? */
	if (boat == 1)
	{
		westC = numberC; eastC = sumC - westC;
		westM = numberM; eastM = sumC - westM;
		boat = 0;
		
		/* Send one cannibal and no missionaries. */
		westC = westC - 1; /* Remove one cannibal from West river bank. */
		eastC = eastC + 1; /* Add one cannibal to East river bank. */
		if (westC >= 0 && eastC <= sumC) /* Is this a valid move? */
		{
			move_options.push_back({eastC,eastM,boat});
			moves_size++;
		}
		westC = westC + 1;
		eastC = eastC - 1;

		/* Send two cannibals and no missionaries. */
		westC = westC - 2; /* Remove two cannibals from West river bank. */
		eastC = eastC + 2; /* Add two cannibals to East river bank. */
		if (westC >= 0 && eastC <= sumC) /* Is this a valid move? */
		{
			move_options.push_back({eastC,eastM,boat});
			moves_size++;
		}
		westC = westC + 2;
		eastC = eastC - 2;

		/* Send no cannibals and one missionary. */
		westM = westM - 1; /* Remove one missionary from West river bank. */
		eastM = eastM + 1; /* Add one missionary to East river bank. */
		if (westM >= 0 && eastM <= sumM) /* Is this a valid move? */
		{
			move_options.push_back({eastC,eastM,boat});
			moves_size++;
		}
		westM = westM + 1;
		eastM = eastM - 1;

		/* Send no cannibals and two missionaries. */
		westM = westM - 2; /* Remove two missionaries from West river bank. */
		eastM = eastM + 2; /* Add two missionaries to East river bank. */
		if (westM >= 0 && eastM <= sumM) /* Is this a valid move? */
		{
			move_options.push_back({eastC,eastM,boat});
			moves_size++;
		}
		westM = westM + 2;
		eastM = eastM - 2;

		/* Send one cannibal and one missionary. */
		westC = westC - 1; westM = westM - 1; /* Remove one cannibal and one missionary from West river bank. */
		eastC = eastC + 1; eastM = eastM + 1; /* Add one cannibal and one missionary to East river bank. */
		if (westC >= 0 && eastC <= sumC && westM >= 0 && eastM <= sumM) /* Is this a valid move? */
		{
			move_options.push_back({eastC,eastM,boat});
			moves_size++;
		}
		westC = westC + 1; westM = westM + 1;
		eastC = eastC - 1; eastM = eastM - 1;
	}
	/* If no, then move boat to West side of river bank. */
	else
	{
		eastC = numberC; westC = sumC - eastC;
		eastM = numberM; westM = sumM - eastM;
		boat = 1;

		/* Send one cannibal and no missionaries. */
		westC = westC + 1; /* Add one cannibal to West river bank. */
		eastC = eastC - 1; /* Remove one cannibal from East river bank. */
		if (eastC >= 0 && westC <= sumC) /* Is this a valid move? */
		{
			move_options.push_back({westC,westM,boat});
			moves_size++;
		}
		westC = westC - 1;
		eastC = eastC + 1;

		/* Send two cannibals and no missionaries. */
		westC = westC + 2; /* Add two cannibals to West river bank. */
		eastC = eastC - 2; /* Remove two cannibals from East river bank. */
		if (eastC >= 0 && westC <= sumC) /* Is this a valid move? */
		{
			move_options.push_back({westC,westM,boat});
			moves_size++;
		}
		westC = westC - 2;
		eastC = eastC + 2;

		/* Send no cannibals and one missionary. */
		westM = westM + 1; /* Add one missionary to West river bank. */
		eastM = eastM - 1; /* Remove one missionary from East river bank. */
		if (eastM >= 0 && westM <= sumM) /* Is this a valid move? */
		{
			move_options.push_back({westC,westM,boat});
			moves_size++;
		}
		westM = westM - 1;
		eastM = eastM + 1;

		/* Send no cannibals and two missionaries. */
		westM = westM + 2; /* Add two missionaries to West river bank. */
		eastM = eastM - 2; /* Remove two missionaries from East river bank. */
		if (eastM >= 0 && westM <= sumM) /* Is this a valid move? */
		{
			move_options.push_back({westC,westM,boat});
			moves_size++;
		}
		westM = westM - 2;
		eastM = eastM + 2;

		/* Send one cannibal and one missionary. */
		westC = westC + 1; westM = westM + 1; /* Add one cannibal and one missionary to West river bank. */
		eastC = eastC - 1; eastM = eastM - 1; /* Remove one cannibal and one missionary from East river bank. */
		if (eastC >= 0 && westC <= sumC && eastM >= 0 && westM <= sumM) /* Is this a valid move? */
		{
			move_options.push_back({westC,westM,boat});
			moves_size++;
		}
		westC = westC - 1; westM = westM - 1;
		eastC = eastC + 1; eastM = eastM + 1;
	}
	return(move_options);
}

/* Connections from one move state to the next move state. */
move_state hop(move_state input)
{
	int position = input.this_side[2]; /* Side of river from input. */
	vector<vector<int>> paths = input.that_side; /* The possible moves from current position. */
	int paths_count = input.that_side_size; /* The number of possible moves from current position. */
	vector<int> this_side, that_side; /* The sides being compared. */
	move_state output; /* Switch to move object matching selected path. */
	int goback = 0; /* Return to previous move state at dead end. */
	if (position == 1)
	{
		/* The input move state is on the West side. */
		for (int i = 0; i < paths_count; i++) /* For every possible move from West to East... */
		{
			that_side = paths[i]; /* Evaluate a possible move from possible moves. */
			for (int j = 0; j < move_states_size_east; j++) /* For every possible move from East to West... */
			{
				this_side = move_states_east[j].this_side; /* Return origin move point from East side. */
				if (that_side[0] == this_side[0] && that_side[1] == this_side[1])
					goback = j; /* Save the move on East being checked in case a return is needed. */;
				if (that_side[0] == this_side[0] && that_side[1] == this_side[1] && move_states_east[j].checked != true) /* If a connection is found from West to East... */
				{
					solution.push_back(input.this_side); /* Add move state to solution. */
					for (int k = 0; k < move_states_size_west; k++)
					{
						if (move_states_west[k].this_side == input.this_side)
							move_states_west[k].checked = true; /* Indicate that move state has been checked. */
					}
					checked_count++; /* Increment the check counter. */
					output.this_side = that_side; /* Move to the next state. */
					output.that_side = move_states_east[j].that_side;
					output.that_side_size = move_states_east[j].that_side_size;
					output.checked = false;
					goto done;
				}
			}
		}
		/* No possible move from West to East is detected. */
		for (int i = 0; i < move_states_size_west; i++)
		{
			if (input.this_side == move_states_west[i].this_side)
			{
				solution.push_back(input.this_side); /* Add move state to solution. */
				checked_count++; /* Increment the check counter. */
				move_states_west[i].checked = true; /* Close current move state. */
				move_states_east[goback].checked = false; /* Open return path to previous move state. */
				output = move_states_east[goback];
				goto done;
			}
		}
	}
	else
	{
		/* The input move state is on the East side. */
		for (int i = 0; i < paths_count; i++) /* For every possible move from East to West... */
		{
			that_side = paths[i]; /* Evaluate a possible move from possible moves. */
			for (int j = 0; j < move_states_size_west; j++) /* For every possible move from West to East... */
			{
				this_side = move_states_west[j].this_side; /* Return origin move point from West side. */
				if (that_side[0] == this_side[0] && that_side[1] == this_side[1])
					goback = j; /* Save the move on West being checked in case a return is needed. */;
				if (that_side[0] == this_side[0] && that_side[1] == this_side[1] && move_states_west[j].checked != true) /* If a connection is found from East to West... */
				{
					solution.push_back(input.this_side); /* Add move state to solution. */
					for (int k = 0; k < move_states_size_east; k++)
					{
						if (move_states_east[k].this_side == input.this_side)
							move_states_east[k].checked = true; /* Indicate that move state has been checked. */
					}
					checked_count++; /* Increment the check counter. */
					output.this_side = that_side; /* Move to the next state. */
					output.that_side = move_states_west[j].that_side;
					output.that_side_size = move_states_west[j].that_side_size;
					output.checked = false;
					goto done;
				}
			}
		}
		/* No possible move from East to West is detected. */
		for (int i = 0; i < move_states_size_east; i++)
		{
			if (input.this_side == move_states_east[i].this_side)
			{
				solution.push_back(input.this_side); /* Add move state to solution. */
				checked_count++; /* Increment the check counter. */
				move_states_east[i].checked = true; /* Close current move state. */
				move_states_west[goback].checked = false; /* Open return path to previous move state. */
				output = move_states_west[goback];
				goto done;
			}
		}
	}
done: return(output);
}

/* What is the state of the opposite side of the river? */
vector<int> opposite(vector<int> o)
{
	vector<int> output = {sumC-o[0],sumM-o[1],0};
	// output[0] = sumC - o[0];
	// output[1] = sumM - o[1];
	if (o[2] == 1)
	{
		output[2] = 0;
	}
	else
		output[2] = 1;
	return(output);
}

/* Are their more cannibals than missionaries? */
bool illegal(vector<int> i)
{
	vector<int> I = opposite(i);
	if (i[0] > i[1] && i[1] != 0 || I[0] > I[1] && I[1] != 0)
		return(true);
	else
		return(false);
}

/* Have all cannibals and missionaries been safely transported? */
bool goal(vector<int> g)
{
	if (g[0] == sumC && g[1] == sumM && g[2] == 0)
		return(true);
	else
		return(false);
}

/* Is there no possible solution? */
bool no_solution(vector<vector<int>> sol)
{
	/* There is no solution when results begin moving back-and-forth between only two move states. */
	if (sol[checked_count - 1] == sol[checked_count - 3] && sol[checked_count - 2] == sol[checked_count - 4])
	{
		std::cout << "           No Solution" << std::endl;
		return(true);
	}
	else
		return(false);
}

/* Display current move state to screen. */
void print(int count)
{
	cout << setw(6) << solution[count-1][0];
	cout << setw(12) << solution[count-1][1];
	cout << setw(13) << solution[count-1][2] << endl;
}

int main()
{
	vector<int> this_side; /* This side of the river bank. */
	vector<int> current_side; /* The current side of the river bank. */
	vector<int> other_side; /* The other side of the river bank. */
	vector<vector<int>> other_sides; /* All possible next sides from the current side. */
	vector<vector<int>> legal_sides; /* All possible next sides from the current side that are legal. */
	move_state move; /* Initialize a move state object. */

	for (int B = 1; B >= 0; B--) /* For each side of the river bank... */
	{
		for (int C = sumC; C >= 0; C--) /* For every cannibal... */
		{
			for (int M = sumM; M >= 0; M--) /* For every missionary... */
			{
start1:			current_side = {C,M,B}; /* Save the current side of the river bank. */
				other_sides = moves(current_side); /* Return all possible moves from current side. */
				if (illegal(current_side) == false) /* If current side is a legal state... */
					move.this_side = current_side; /* then save current side to a move state object. */
				else
				{
					M--;
					goto start1;
				}
				for (int i = 0; i < moves_size; i++) /* For every possible move from current side... */
				{
					if (illegal(other_sides[i]) == false) /* If possible move is a legal state... */
					{
						legal_sides.push_back(other_sides[i]); /* then save as possible move. */
						legal_size++;
					}
					if (goal(other_sides[i]) == true) /* If possible move is the end goal... */
					{
						i = moves_size; /* then close list of possible moves. */
						end_move = true;
					}
				}
				move.that_side = legal_sides; /* Save possible moves to a move state object. */
				move.that_side_size = legal_size; /* Save number of possible moves to a move state object. */
				legal_sides = {};
				legal_size = 0;
				move.checked = false;
				/* Save move state object to list of move state objects. */
				if (move.that_side_size >= 2)
				{
					if (B == 1)
					{
						move_states_west.push_back(move);
						move_states_size_west++;
					}
					else
					{
						move_states_east.push_back(move);
						move_states_size_east++;
					}
				}
			}
		}
	}
	
	/* Search for the sequence of moves that leads to the end goal. */
	std::cout << "             Solution"                 << std::endl;
	std::cout << "*************************************" << std::endl;
	std::cout << " Cannibals  Missionaries  River Bank"  << std::endl;
	std::cout << "-------------------------------------" << std::endl;

	bool finish = false; /* Has the end goal been located? Yes=true No=false */
	move_state current_move = move_states_west[0], next_move;
	while (finish != true) /* While end goal has not been located... */
	{
		/* Move back and forth between West and East sides of the river. */
		next_move = hop(current_move);
		print(checked_count);
		/* End the move path search if no solution exists. */
		if (checked_count >= 4)
		{
			if (no_solution(solution) == true)
				finish = true;
		}
		/* End the move path search once end goal has been reached. */
		if (goal(next_move.this_side) == true)
		{
			finish = true;
			solution.push_back(move_states_east[0].this_side);
			print(checked_count + 1);
		}
		else
			current_move = next_move;
	}
	std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
	cout << "\n";

	system("pause");
	return 0;
}