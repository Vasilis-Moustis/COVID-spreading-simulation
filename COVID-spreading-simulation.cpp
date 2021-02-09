/*
we gonna assume the city centre . Each civilian comes from a place across the city.
We gonna track people movements in the city centre.
The method is similar to a backtracking algorithm. We take the steps from the centre to an edge of the city centre's map
and we will assume the came from the same direction. 
*/
 
#include <fstream>			//ofstream	<<
#include <iostream>			// cout		<<
#include <iomanip>			// setw
#include <time.h>	    	// time
#include <list> 
#include <iterator>
#include <bits/stdc++.h> 
#include <stack>

using namespace std;		// srand, rand

#define MAX INT_MAX 

const int DIM = 40; //change for a bigger or smaller map diameter
enum StepDirection	{ N, NE, E, SE, S, SW, W, NW };	// compass points
int matrix[DIM][DIM] = {0}; //passes per point on the map
int startI = DIM/2 , startJ = DIM/2;    //variables to indicate the centre of a map
//For StartI,StartJ, removal of comment needed in line 455 - 456

int GlobalCounter = 0; //used by civilianSimulation()
const int NumberOfCivilians = 15; // change to alter population!!
int GlobalStepCounter[NumberOfCivilians] = {0}; //steps of each civilian

bool Infected[NumberOfCivilians] = {false}; //Infection status of each civilian
int infectionPossibility;//possibility of infection if in close contact with infected civilian (given by user)

//////////////////////////////////////////////////////////////////////////
//Note that we dont use every function in our classes, we have them most in
//case we need some extra tools.
//////////////////////////////////////////////////////////////////////////
//declaring our linked list class
//CAUTION!!!!
//-append() is for struct mynode and classicNodeAppend() for struct node
//-traverseNprintLL() is for struct mynode and printList() for struct node

struct node
{
    int data;
    node *next;
};

struct mynode
{
    node *data;
    mynode *next;
};


class linked_list
{
private:
    node *head,*tail;
public:
    linked_list()
    {
        head = NULL;
        tail = NULL;
    }
};


/* Given a reference (pointer to pointer) 
to the head of a list and an int, inserts 
a new node on the front of the list. */
void push(node** head_ref, int new_data)  
{  
    /* 1. allocate node */
    node* new_node = new node(); 
  
    /* 2. put in the data */
    new_node->data = new_data;  
  
    /* 3. Make next of new node as head */
    new_node->next = (*head_ref);  
  
    /* 4. move the head to point to the new node */
    (*head_ref) = new_node;  
}  
  
/* Given a node prev_node, insert a new node after the given  
prev_node */
void insertAfter(node* prev_node, int new_data)  
{  
    /*1. check if the given prev_node is NULL */
    if (prev_node == NULL)  
    {  
        cout<<"the given previous node cannot be NULL";  
        return;  
    }  
  
    /* 2. allocate new node */
    node* new_node = new node(); 
  
    /* 3. put in the data */
    new_node->data = new_data;  
  
    /* 4. Make next of new node as next of prev_node */
    new_node->next = prev_node->next;  
  
    /* 5. move the next of prev_node as new_node */
    prev_node->next = new_node;  
}  

void classicNodeAppend(node** head_ref, int new_data)  
{  
    /* 1. allocate node */
    node* new_node = new node(); 
  
    node *last = *head_ref; /* used in step 5*/
  
    /* 2. put in the data */
    new_node->data = new_data;  
  
    /* 3. This new node is going to be  
    the last node, so make next of  
    it as NULL*/
    new_node->next = NULL;  
  
    /* 4. If the Linked List is empty, 
    then make the new node as head */
    if (*head_ref == NULL)  
    {  
        *head_ref = new_node;  
        return;  
    }  
  
    /* 5. Else traverse till the last node */
    while (last->next != NULL)  
        last = last->next;  
  
    /* 6. Change the next of last node */
    last->next = new_node;  
    return;  
}  
  
/* Given a reference (pointer to pointer) to the head  
of a list holder (linked list of linked lists) and an node(the head of each linked list), 
appends a new node at the end */
void append(mynode** head_ref, node new_data)  
{  
    /* 1. allocate node */
    mynode* new_node = new mynode(); 
  
    mynode *last = *head_ref; /* used in step 5*/
  
    /* 2. put in the data */
    new_node->data = &new_data;  
  
    /* 3. This new node is going to be  
    the last node, so make next of  
    it as NULL*/
    new_node->next = NULL;  
  
    /* 4. If the Linked List is empty, 
    then make the new node as head */
    if (*head_ref == NULL)  
    {  
        *head_ref = new_node;  
        return;  
    }  
  
    /* 5. Else traverse till the last node */
    while (last->next != NULL)  
        last = last->next;  
  
    /* 6. Change the next of last node */
    last->next = new_node;  
    return;  
}  
  
// This function prints contents of 
// linked list starting from head  
void printList(node *node)  
{  
    while (node != NULL)  
    {  
        cout<<" "<<node->data;  
        node = node->next;  
    }  
}  

void traverseNprintLL(mynode *node)  
{  
    while (node != NULL)  
    {  
        printList(node->data);  
        node = node->next;  
    }  
}  

//////////////////////////////////////////////////////////////////////////
//declaring our array class...
//Arrays are used to translate movement of each civilian stored also in linked lists to a state
//that our eyes better understand. Tose arrays are saved in files, created with program execution.

class Array2D
{
	// This output function shows non-touched locations as + and displays
	// the step number for all locations reached. As a location may have 
	// been reached more than once, the last step number will be displayed 
	// in that case.

	friend ostream& operator << ( ostream& os, const Array2D& a )
{
	os << "  ("<< a.moveNumber << " steps)\n\n";
	
	for( int r = 0; r < DIM; r++ )
	{
		for( int c = 0; c < DIM; c++ )
		{
			switch( a.array[r][c] )
			{
				case -1:	os << "  +"; break;
				default:	os << setw(3) << a.array[r][c];
			}
		}	
		os << endl;		
	}
	return os;
}
	
public:

			Array2D();

			Array2D( const Array2D& a ); 

	bool	OnAnEdge();

	StepDirection	RandomStep(node* ll);


private:

	int array[DIM][DIM];
	int moveNumber;
	int row, col;
};

//////////////////////////////////////////////////////////////////////////
//declaring our matrix printing function
//just in case we need a conole output

void showMatrix(int table[DIM][DIM])
{
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j ++)
        {
            cout << table[i][j]<< "\t";
        }
        cout << endl;
    }
}

//////////////////////////////////////////////////////////////////////////
//declaring Array's functions...

Array2D:: Array2D()
{
	// Use the system clock to initialize the random number generator seed
	srand( (unsigned)time( NULL ) );

	// Begin the random walk in the middle position.
	row			= startI;
	col			= startJ;
	moveNumber	= 0;

	// Minus 1 indicates this location has never been reached.
	for( int r = 0; r < DIM; r++)
		for( int c = 0; c < DIM; c++ )
			array[r][c] = -1;

	// Always start at the center
	array[row][col] = 0;
}
	
Array2D:: Array2D( const Array2D& a )
{
	row			= a.row;
	col			= a.col;
	moveNumber	= a.moveNumber;

	for( int r = 0; r < DIM; r++)
		for( int c = 0; c < DIM; c++ )
			array[r][c] = a.array[r][c];
}

bool Array2D:: OnAnEdge()
{
	if( row == 0		|| col == 0		||
		row == DIM-1	|| col == DIM-1 
	  )
		return true;
	else 
		return false;
}
		
StepDirection Array2D::RandomStep(node* ll)
{
	moveNumber++;
	
	// A random integer in the range 0..7 will be used to select 
	// the direction for the step. Compass points indicate direction.

	StepDirection step = StepDirection(rand() %8);

	switch( step )
	{
		case N:		row--;			break;
		case NE:	row--;	col++;	break;
		case E:				col++;	break;
		case SE:	row++;	col++;	break;
		case S:		row++;			break;
		case SW:	row++;	col--;	break;
		case W:				col--;	break;
		case NW:	row--;	col--;	break;
	}

    //CAUTION
    //below we have a possibility that a signal is not transmitted to general factors
    //posibility is set at 1 / 15 signals
    //change the number '15' to alter the posibility
    int nullPosibility = rand() % 15; 

    nullPosibility--;
    if (nullPosibility != -1)
    {
        ll->data = step;
    }
    else
    {
        ll->data = -1;
    }
    
	array[row][col] = moveNumber;
    matrix[row][col]++;

	return step;
}

//////////////////////////////////////////////////////////////////////////
//declaring our per person function for random walking simulation

mynode* holder = NULL;

//-1 as a data value of a linked list represents that a signal was not sent correctly.
//looking at the direction from the next value we can assume the path taken by the civilian.
void fixLinkedLists(mynode *myn)
{

   while (myn != NULL)  
    {  

        node *node = myn -> data;
        node -> next;

        while(node != NULL)
        {

            if (node -> data == -1)
            {
                node-> data = node -> next -> data;
            }

            node =node -> next;
            
        }

        myn = myn->next;  

    }  


}

void Summarize_Trajectory()
{

    int maxj = 0;
    int maxi = 0;
    int max = 0;

    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j ++)
        {
            if (matrix[i][j] > max)
            {
                maxi = i;
                maxj = j;
            }
        }
        
    }

    //startI = maxi;
    //startJ = maxj;
    /*
    ATTENTION:
    we could actually have a movement from a hotspot  to another, so that the next day's sim have its coordinates as a center.
    of course the smaller the map, the quickest the spread will eventually get out of this area, as the civilization remains
    the same. Remove the above two comments to move the centre to the hotspot... it is recomended to also change the DIM(diameter)
    at the begining.
    */

}


void civilianSimulation(int i, node* ll) //i = civilian number   ll = memory space possition of the head of the civilian's  linked list
{

    //introduction of the civilian in the file
    ofstream out("Civilians.out", std::ios_base::app);//append

    out << "\n\nCivilian Number: " << i <<endl;
    out << "\nMove sequence: ";
 
	Array2D a;

    int totalSteps = 0;

	// Take random steps until an edge is reached. Each step is displayed.
	while( !a.OnAnEdge() )
	{
        ll->next;
		StepDirection s =		a.RandomStep(ll); // using an array 2d function
		out << s;
        totalSteps++;
	}

    GlobalStepCounter[GlobalCounter] = totalSteps; //saving steps of current civilian
    GlobalCounter++; //moving to the next one

	// Output the steps within the array. 
	out << "\n\nArray and step sequence of civilian " << i << ": " << a;

    out.close();

}

//this function print how many times a point is passed by someone through the day
void makeTheGraph()
{

    //remove comments if the program needs to print the overall table in the console
    //reminder: the overall table describe how many times a person 
    //showMatrix(matrix);

    ofstream os2;
    os2.open( "Overall.out" );
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j ++)
        {
            os2 << matrix[i][j];
            os2 << "\t";
        }
        os2 << "\n";
    }
    os2.close();

}

void makeThefixedGraph()
{

    //remove comments if the program needs to print the overall table in the console
    //reminder: the overall table describe how many times a person passed for each point 
    //showMatrix(matrix);

    ofstream fix;
    fix.open( "fixedOverall.out" );
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j ++)
        {
            fix << matrix[i][j];
            fix << "\t";
        }
        fix << "\n";
    }
    fix.close();

}

//in this function we execute simulation of each citizen
void traverseLL(mynode *node)  
{  

    int counter = 1;
    while (node != NULL)  
    {  
        civilianSimulation(counter, node->data);  
        node = node->next;  
        counter++;
    }  

    makeTheGraph();

}  

//in this function we fix the matrix table we the fixed values so we are able to call the makeThefixedGraph() function.
void Repair()
{

    int c = 0;
    int count = 0;

    for (int i = 1; i < (DIM - 1); i++)
    {
        for (int j = 1; j < (DIM - 1); j ++)
        {

            //we will give each damaged spot the MO of their visited only neighbors
            if (matrix[i][j+1] != 0)
            {
                c++;
                count += matrix[i][j+1];
            }
            
            if (matrix[i-1][j] != 0)
            {   
                c++;
                c += matrix[i-1][j];
            }

            if (matrix[i][j-1] != 0)
            {
                c++;
                count += matrix[i][j-1];
            }
            
            if (matrix[i+1][j] != 0)
            {   
                c++;
                c += matrix[i+1][j];
            }

            if (c != 0)
                matrix[i][j] = int(count/c);

        }

        c = 0;
        count = 0; 

    }

    //we also fix the linked lists
    fixLinkedLists(holder);

    makeThefixedGraph();

}

void checkPossibleConsequences(int c , int x) //c = number of civilian      x = steps of this civilian
{

    /*[I]
    Now the idea behind this is simple. We trust that each civilian came from somewhere out of the centre to the centre and back 
    to start from the same route. Route is almost one directional except from small circles int the centre and a small curve to the overall path.
    Since we understand that the civilian spent a great amount of time in the centre and the number of steps from the centre to the actual start
    is very little, we can take an actual realistic approach to check the difference of the steps. Since all civilians are signalling their steps simutaneously, similar amount of steps 
    (with the fuct the all have the same destination) will result to the fact that some of them came in close contact.
    */
    int diff;
    int P;

    for (int i = 0; i < NumberOfCivilians; i++)
    {
        
        if (i == c)
            continue;
        else
        {
            /*
                Due to [I] , it is recommended for small map to have small infection probability,
                and higher for bigger maps . You can change the diameter of the map by changing global var DIM.
            */
            diff = GlobalStepCounter[i] - x;
            if (diff >= -4 || diff <= 4)
            {
                P = rand() % 100;
                if (P < infectionPossibility)
                    Infected[i] = true;
            }

        }
        
    }

}

//this function checks if someone is possibly infected.
//if marked as infected, it activates the checkPossibleConsequences() function
void Possible_Covid_19_Infection()
{

    for (int i = 0; i < NumberOfCivilians; i++)
    {
        if (Infected[i])
            checkPossibleConsequences( i , GlobalStepCounter[i]);
    }    

}

//////////////////////////////////////////////////////////////////////////
//In this section we declare our citizens
//remove or add comments AS WELL AS THE VARIABLE 'NumberOfCivilians' AT THE BEGGING to alter the civilization's size.

    node civilian0;
    node civilian1;
    node civilian2;
    node civilian3;
    node civilian4;
    node civilian5;
    node civilian6;
    node civilian7;
    node civilian8;
    node civilian9;
    node civilian10;
    node civilian11;
    node civilian12;
    node civilian13;
    node civilian14;
    //node civilian15;
    //node civilian16;
    //node civilian17;
    //node civilian18;
    //node civilian19;
    //node civilian20;
    //node civilian21;
    //node civilian22;
    //node civilian23;
    //node civilian24;
    //node civilian25;
    //node civilian26;
    //node civilian27;
    //node civilian28;
    //node civilian29;

void makeHolder()
{
    append(&holder , civilian0);
    append(&holder , civilian1);
    append(&holder , civilian2);
    append(&holder , civilian3);
    append(&holder , civilian4);
    append(&holder , civilian5);
    append(&holder , civilian6);
    append(&holder , civilian7);
    append(&holder , civilian8);
    append(&holder , civilian9);
    append(&holder , civilian10);
    append(&holder , civilian11);
    append(&holder , civilian12);
    append(&holder , civilian13);
    append(&holder , civilian14);
    //append(&holder , civilian15);
    //append(&holder , civilian16);
    //append(&holder , civilian17);
    //append(&holder , civilian18);
    //append(&holder , civilian19);
    //append(&holder , civilian20);
    //append(&holder , civilian21);
    //append(&holder , civilian22);
    //append(&holder , civilian23);
    //append(&holder , civilian24);
    //append(&holder , civilian25);
    //append(&holder , civilian26);
    //append(&holder , civilian27);
    //append(&holder , civilian28);
    //append(&holder , civilian29);
}   

//////////////////////////////////////////////////////////////////////////

//MAIN function
int main()
{

    //inroduction
	ofstream os;
	os.open( "Civilians.out" );

	os	<< "\n\n\tErgasia 1 Vasileios Moustis P17077.cpp\n\n\n"
		<< "Direction code relative to compass points : "
		<< "\n\t\t0:N, 1:NE, 2:E, 3:SE, 4:S, 5:SW, 6:W, 7:NW \n\n"; 

	os.close();

    //lets assume we have a few people as a test bench to our implementation...
    

    bool StopSimulation = false; //flag to stop the simulation
    string answear;
    int dayCounter = 1;

    //we will ask for the infection possibility...
    cout << "Please state the infection possibility if someone came in close range with an ill civilian ( 1-100 ) )\n";

    do //checking for wrong inputs
    {
        cin >> infectionPossibility;
    } while (infectionPossibility > 100 && infectionPossibility < 1);
    
    
    cout << "\n\n";

    makeHolder();

    int infected ;

    while(! StopSimulation)
    {

        //if its the first day we randomly create the first infected
        //else we check for possible other infections
        if (dayCounter > 1)
            Possible_Covid_19_Infection();
        else
        {
            infected = rand() % 30;
            Infected[infected] = true;
        }
        

        cout<< "Day " << dayCounter << " completed! Would you like to continue the simulation? \n y or n?" << endl;;

        do //checking for wrong inputs
        {
            cin >> answear;
        } while (answear != "y" && answear != "n");
        


        traverseLL(holder);

        /*
        having also the overall table all fixed up we can find spots that even though
        it has neighbors with plenty of activity, these spots are not visited by anyone.
        Because this is extremely unlike to happen(especially when we simulate with more and more
        subjects(civilians) so we gonna give them an aproximate value to the overall table, plus
        connecting them to the right place of each node(civilian). 
        */
        
        Repair();

        //every three days we are looking for errors and hotspot
        //plus we creathe a new centre if option enabled
        if ((dayCounter % 3) == 0) 
        {
            Summarize_Trajectory();
        }

        dayCounter++;
        GlobalCounter = 0;

        //we print the health status of each citizen
        for (int k = 0; k < NumberOfCivilians; k++)
            if (Infected[k])
                cout << "Civilian " << k << " has been infected..." <<endl;
            else
                cout << "Civilian " << k << " is OK..." <<endl;

            
        cout <<endl;

        //we check for flag if to end the simulation
        if (answear == "n")
            StopSimulation = true;

    }

    cout << "\nSimulation lasted :\t" << dayCounter-1 << " days\nCheck files for more information." << endl;

	return 1;
    
}

/*
Vasileios Moustis
P 17077


code sources:

http://cs.lamar.edu/
https://www.bgsu.edu/
https://stackoverflow.com/
https://www.geeksforgeeks.org/

*/




