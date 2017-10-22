#include <iostream>
#include "CAbase.h"
using namespace std;

// Function to draw current state of CA
void Draw( CAbase ca )
{
  for( unsigned int j = 0; j < ca.GetNx()+2; j++ ) {
    for( unsigned int i = 0; i < ca.GetNy()+2; i++ ) {
      if( ca.IsAlive(i, j) == 0 ) cout<<"  ";
      if( ca.IsAlive(i, j) == 1 ) cout<<"* ";
      if( ca.IsAlive(i, j) == -1 ) cout<<". ";
    }
    cout<<"\n";
  }
}

int main() {

  CAbase newCA(20,20);			// Create CA object with constructor
  newCA.ResetWorldSize(30,30);		// Change universe size
  cout<<" Nh: "<<newCA.GetNy()<<";   Nw: "<<newCA.GetNx()<<"\n";
  // Set example
  newCA.SetAlive(2,3,1);
  newCA.SetAlive(2,4,1);
  newCA.SetAlive(2,5,1);
  newCA.SetAlive(3,4,1);
  newCA.SetAlive(3,5,1);
  newCA.SetAlive(4,4,1);
  newCA.SetAlive(5,4,1);
  newCA.SetAlive(6,4,1);
  newCA.SetAlive(7,4,1);
  newCA.SetAlive(7,7,1);
  newCA.SetAlive(7,8,1);
  newCA.SetAlive(8,8,1);
  newCA.SetAlive(9,8,1);
  newCA.SetAlive(10,8,1);
  newCA.SetAlive(6,8,1);
  newCA.SetAlive(7,6,1);
  newCA.SetAlive(7,5,1);
  newCA.SetAlive(5,5,1);

  newCA.SetAlive(20,5,1);
  newCA.SetAlive(20,6,1);
  newCA.SetAlive(20,7,1);
  newCA.SetAlive(19,7,1);
  newCA.SetAlive(18,6,1);
  // ---
  int input = 1;
  do {
    Draw( newCA );
    bool do_menu(true);
    do{
      // Console menu realization
      cout<<" 0. Exit\n 1. Evolve\n 2. Set alive cell\n 3. Reprint\n 4. Resize\n";
      cin>>input;
      switch( input ) {			// Check menu settings
        case 0:
          do_menu = false;		// 0: Exit
	  break;
        case 1:
          newCA.WorldEvolutionLife();	// 1: Start evolution
          do_menu = false;		// and exit from menu to redraw universe
	  break;
        case 2:
	  int x, y;			// 2. Take coordinates from keyboard,
	  cout<<" X= "; cin>>x;
	  cout<<" Y= "; cin>>y;
	  newCA.SetAlive(x,y,1);	// set cell as alive
	  break;			// and go to menu again
        case 3:
          do_menu = false;		// 3. Exit from menu to redraw universe
          break;
        case 4:
          int nx, ny;			// 4. Take new universe size from keyboard,
          cout<<" Nx= "; cin>>nx;
          cout<<" Ny= "; cin>>ny;
          newCA.ResetWorldSize(nx,ny);	// set new universe size
          do_menu = false;		// and exit from menu to redraw universe
          break;
        default:
	  cout<<"Wrong option. Try Another.\n";
      }
    }while( do_menu );
  }while( input != 0 );

  return 0;
}
