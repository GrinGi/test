#include <stdlib.h>
#include <ctime>

class CAbase
{
public:
  CAbase()
    : Ny(10)
    , Nx(10)
    , nochanges(false)
  {
      ResetWorldSize( Nx , Ny, 1 );
  }
  CAbase( int nx, int ny )
    : Ny(ny)
    , Nx(nx)
    , nochanges(false)
  {
      ResetWorldSize( Nx , Ny, 1 );
  }

  void ResetWorldSize( int nx, int ny, bool del = 0 );

  int GetNy() { return Ny; }
  int GetNx() { return Nx; }
  int GetColor(int x, int y) { return worldColor[y*(Nx+2)+x]; }
  void SetColor( int x, int y, int c);
  void SetColorEvo( int x, int y, int c);
  void SetAlive( int x, int y, int i );
  void SetAliveEvo( int x, int y, int i );
  int IsAlive( int x, int y ) { return world[y*(Nx+2)+x]; }
  bool IsNotChanged() { return nochanges; }
  int CellEvolutionLife( int x, int y );
  int CellEvolutionRAND( int x, int y );
  int CellEvolutionErosion( int x, int y );
  int CellEvolutionMoving( int x, int y );
  int CellEvolutionUnlimitedLife( int x, int y );
  int CellEvolutionLimitedLife( int x, int y );
  int CellEvolutionFractalLife( int x, int y );
  int CellEvolutionLifeColor( int x, int y );
  void WorldEvolutionLife(int mode = 0);
  void WorldEvolutionLifeColor();
  void PrepareFieldSnake();
  bool CreateFood();
  int CellEvolutionSnake( int x, int y );
  void WorldEvolutionSnake();
  void TurnSnake( int dir );
  int GetMaxAround( int x, int y, int start );
  // ---
  int CellEvolutionTwoLiquids( int x, int y );
  int CellEvolutionDiffusionMargolus( int x, int y, bool t );
  void WorldEvolutionMargolus();
  // ---
  ~CAbase()
  {
  }
private:
  int Ny;
  int Nx;
  int *world;
  int *worldNew;
  int *worldColor;
  int *worldColorNew;
  bool nochanges;
};

inline void CAbase::SetAlive( int x, int y, int i ) // Set number i into cell with coordinates x,y in current universe
{
  world[y*(Nx+2)+x] = i;
}

inline void CAbase::SetAliveEvo( int x, int y, int i ) // Set number i into cell with coordinates x,y in evolution universe
{
  worldNew[y*(Nx+2)+x] = i;
}

inline void CAbase::SetColor( int x, int y, int c) //Set color c into cell with coordinates x,y in current color universe
{
    worldColor[y*(Nx+2)+x] = c;
}

inline void CAbase::SetColorEvo( int x, int y, int c) // Set color c into cell with coordinates x,y in evolution color universe
{
    worldColorNew[y*(Nx+2)+x] = c;
}


//
// Game "Life" rules:
//
// Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
// Any live cell with two or three live neighbours lives on to the next generation.
// Any live cell with more than three live neighbours dies, as if by overpopulation.
// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
//
//
inline int CAbase::CellEvolutionLife( int x, int y ) // Classic Life. Evolution rules for every cell. Changing only cell (x, y) for every step
{
  int n_sum = 0;
  int x1(0), y1(0);
  for( int ix = -1; ix <= 1; ix++ ) {
    for( int iy = -1; iy <= 1; iy++ ) {
      if( ix == 0 && iy == 0 ) continue;
      x1 = x + ix;
      y1 = y + iy;
      if( x1 < 1 ) x1 = Nx;
      if( x1 > Nx ) x1 = 1;
      if( y1 < 1 ) y1 = Ny;
      if( y1 > Ny ) y1 = 1;
      if( IsAlive( x1, y1 ) == 1 ) n_sum++;
    }
  }
  if( IsAlive( x, y ) == 1 ) {
    if( n_sum == 2 || n_sum == 3 ) SetAliveEvo( x, y, 1 );
    else SetAliveEvo( x, y, 0 );
  }
  else {
    if( n_sum == 3 ) SetAliveEvo( x, y, 1 );
  }
  return 0;
}

//
// Unlimited growth case
//
// The cell becomes alive if at least one of its neighbors is alive.
// The cell, once alive, remains alive forever.
//
inline int CAbase::CellEvolutionUnlimitedLife( int x, int y ) // unlimited Life. Evolution rules for every cell. Changing only cell (x, y) for every step
{
  int n_sum = 0;
  int x1(0), y1(0);
  for( int ix = -1; ix <= 1; ix++ ) {
    for( int iy = -1; iy <= 1; iy++ ) {
      if( ix == 0 && iy == 0 ) continue;
      x1 = x + ix;
      y1 = y + iy;
      if( x1 < 1 ) x1 = Nx;
      if( x1 > Nx ) x1 = 1;
      if( y1 < 1 ) y1 = Ny;
      if( y1 > Ny ) y1 = 1;
      if( IsAlive( x1, y1 ) == 1 ) n_sum++;
    }
  }
  if( IsAlive( x, y ) == 1 )
     SetAliveEvo( x, y, 1 );
  else
    if( n_sum > 0 ) SetAliveEvo( x, y, 1 );

  return 0;
}

//
// Limited growth case with Moore's neighborhood(northWestCell, northCell, northEastCell, westCell, eastCell, southWestCell, southCell, southEastCell).
//
// The cell becomes alive if ONLY one of its neighbors is alive.
// The cell, once alive, remains alive forever.
//
inline int CAbase::CellEvolutionLimitedLife( int x, int y ) // limited Life. Evolution rules for every cell. Changing only cell (x, y) for every step
{
  int n_sum = 0;
  int x1(0), y1(0);
  for( int ix = -1; ix <= 1; ix++ ) {
    for( int iy = -1; iy <= 1; iy++ ) {
      if( ix == 0 && iy == 0 ) continue;
      x1 = x + ix;
      y1 = y + iy;
      if( x1 < 1 ) x1 = Nx;
      if( x1 > Nx ) x1 = 1;
      if( y1 < 1 ) y1 = Ny;
      if( y1 > Ny ) y1 = 1;
      if( IsAlive( x1, y1 ) == 1 ) n_sum++;
    }
  }
  if( IsAlive( x, y ) == 1 )
     SetAliveEvo( x, y, 1 );
  else
    if( n_sum == 1 ) SetAliveEvo( x, y, 1 );

  return 0;
}

//
// Limited growth case with von Neumann neighborhood(northCell, westCell, eastCell, southCell).
//
// The cell becomes alive if ONLY one of its neighbors is alive.
// The cell, once alive, remains alive forever.
//
inline int CAbase::CellEvolutionFractalLife( int x, int y ) // limited fractal Life. Evolution rules for every cell. Changing only cell (x, y) for every step
{
  int n_sum = 0;
  int x1(0), y1(0);
  for( int ix = -1; ix <= 1; ix++ ) {
      if( ix == 0 ) continue;
      x1 = x + ix;
      if( x1 < 1 ) x1 = Nx;
      if( x1 > Nx ) x1 = 1;
      if( IsAlive( x1, y ) == 1 ) n_sum++;
    }
  for( int iy = -1; iy <= 1; iy++ ) {
      if( iy == 0 ) continue;
      y1 = y + iy;
      if( y1 < 1 ) y1 = Ny;
      if( y1 > Ny ) y1 = 1;
      if( IsAlive( x, y1 ) == 1 ) n_sum++;
    }

  if( IsAlive( x, y ) == 1 )
     SetAliveEvo( x, y, 1 );
  else
    if( n_sum == 1 ) SetAliveEvo( x, y, 1 );

  return 0;
}

//Cyclic cellular automata with von Neumann neighborhood(northCell, westCell, eastCell, southCell).
//
//The cell can be in 12 states (Number of colors), which we will number with the numbers 0, 1, ..., 11.
//We say that the state m is next to the state k if m + 1 = k (mod n).
//A cell from the state m goes to the next state k if at least one of the neighboring cells has the state k.
//
//
inline int CAbase::CellEvolutionLifeColor( int x, int y ) // Cyclic cellular automaton
{
  int n_sum = 0;
  int nextColor = (GetColor( x, y ) + 1) % 12;
  int x1(0), y1(0);
  for( int ix = -1; ix <= 1; ix++ ) {
      if( ix == 0 ) continue;
      x1 = x + ix;
      if( x1 < 1 ) x1 = Nx;
      if( x1 > Nx ) x1 = 1;
      if( GetColor( x1, y ) == nextColor ) n_sum++;
    }
  for( int iy = -1; iy <= 1; iy++ ) {
      if( iy == 0 ) continue;
      y1 = y + iy;
      if( y1 < 1 ) y1 = Ny;
      if( y1 > Ny ) y1 = 1;
      if( GetColor( x, y1 ) == nextColor ) n_sum++;
    }

  if( n_sum > 0 )
      SetColorEvo( x, y, nextColor );
  else
      SetColorEvo( x, y, GetColor( x, y) );

  return 0;
}

// Evolution of cell for noise generator
// Calculate new state of every cell by formula (NEW) = (CENTER) AND (UP) XOR (DOWN) XOR (RIGHT) XOR (LEFT) XOR (CENTER)
// Create 'random' noise combinations from any input sample created by user
inline int CAbase::CellEvolutionRAND( int x, int y )
{
    bool alive = (IsAlive(x, y)==1);		// (CENTER)
    int c(0);
    y+1 > Ny ? c = 1 : c = y + 1;		// check border
    alive &= (IsAlive(x, c)==1);		// AND (UP)
    y-1 < 1 ? c = Ny : c = y - 1;		// check border
    alive ^= (IsAlive(x, c)==1);		// XOR (DOWN)
    x+1 > Nx ? c = 1 : c = x + 1;		// check border
    alive ^= (IsAlive(c, y)==1);		// XOR (RIGHT)
    x-1 < 1 ? c = Nx : c = x - 1;		// check border
    alive ^= (IsAlive(c, y)==1);		// XOR (LEFT)
    alive ^= (IsAlive(x, y)==1);		// XOR (CENTER)
    if( alive ) SetAliveEvo( x, y, 1 );		// Evolve cell
    else SetAliveEvo( x, y, 0 );
  return 0;
}

//
// Moving and interacting objects.
//
// There are five types of cells:
//
// DEAD cell with state 3.  Non-moving
// LEFT cell with state 4.  Moving to the left
// RIGHT cell with state 6. Moving to the right
// UP cell with state 2.    Moving up
// DOWN cell with state 8.  Moving down
//
// Each cell is reflected from the grid borders
// Moving cells can move DEAD cells
//
inline int CAbase::CellEvolutionMoving( int x, int y )
{
    if(IsAlive(x,y)!=3)
        SetAliveEvo( x, y, 0 );
    else
        SetAliveEvo( x, y, 3);

    if(x == Nx && IsAlive(x,y) == 6)  // border right
        SetAliveEvo(Nx,y,4);

    if(x == 1 && IsAlive(x,y) == 4) // border left
        SetAliveEvo(1,y,6);

    if(y == 1 && IsAlive(x,y) == 2) // border up
        SetAliveEvo(x,1,8);

    if(y == Ny && IsAlive(x,y) == 8) // border dowm
        SetAliveEvo(x,Ny,2);


    if(IsAlive(x,y) != 3 && IsAlive(x-1,y) == 6)  // move right
        SetAliveEvo( x, y, 6);

    if(IsAlive(x,y) != 3 && IsAlive(x+1,y) == 4) // move left
        SetAliveEvo( x, y, 4);

    if(IsAlive(x,y) != 3 && IsAlive(x,y+1) == 2) // move up
        SetAliveEvo( x, y, 2);

    if(IsAlive(x,y) != 3 && IsAlive(x,y-1) == 8) // move down
        SetAliveEvo( x, y, 8);


    if(IsAlive(x,y) == 3 && IsAlive(x-1,y) == 6)  // change direction move right
        SetAliveEvo( x-1, y, 4);

    if(IsAlive(x-1,y) == 3 && IsAlive(x,y) == 4) // change direction move left
        SetAliveEvo( x, y, 6);

    if(IsAlive(x,y-1) == 3 && IsAlive(x,y) == 2) // change direction move up
        SetAliveEvo( x, y, 8);

    if(IsAlive(x,y) == 3 && IsAlive(x,y-1) == 8) // change direction move down
        SetAliveEvo( x, y-1, 2);


    if(x != (Nx+1) && IsAlive(x,y) == 0 && IsAlive(x-1,y) == 3 && IsAlive(x-2,y) == 6){  // move dead cell right
        SetAliveEvo( x-1, y, 0);
        SetAliveEvo( x, y, 3);
    }

    if(x != 1 && IsAlive(x-2,y) == 0 && IsAlive(x-1,y) == 3 && IsAlive(x,y) == 4){  // move dead cell left
        SetAliveEvo( x-1, y, 0);
        SetAliveEvo( x-2, y, 3);
    }

    if(y != 1 && IsAlive(x,y-2) == 0 && IsAlive(x,y-1) == 3 && IsAlive(x,y) == 2){  // move dead cell up
        SetAliveEvo( x, y-1, 0);
        SetAliveEvo( x, y-2, 3);
    }

    if(y != (Ny+1) && IsAlive(x,y) == 0 && IsAlive(x,y-1) == 3 && IsAlive(x,y-2) == 8){  // move dead cell dowm
        SetAliveEvo( x, y-1, 0);
        SetAliveEvo( x, y, 3);
    }


    if(IsAlive(x-1,y) == 6 && IsAlive(x,y) == 4){ //fly through each other left right
        SetAliveEvo( x-1, y, 4);
        SetAliveEvo( x, y, 6);
    }

    if(IsAlive(x,y-1) == 8 && IsAlive(x,y) == 2){ //fly through each other up down
        SetAliveEvo( x, y-1, 2);
        SetAliveEvo( x, y, 8);
    }

    return 0;
}

// Evolution of cell for erosion
// Function emulates ground erosion process - the action of surface processes that remove ground from some location on the Earth's crust
// Cell will be removed if it does not have all 3 neighbour at any side
// Cell will stay alive if at one or more cell at every side of this cell (left, right, up, down) is alive
inline int CAbase::CellEvolutionErosion( int x, int y )
{
    bool alive = (IsAlive(x, y)==1);
    int c(0), d(0);
    if( !alive ) {			// Check if cell is not alive, stay it the same and finish its evolution
        SetAliveEvo( x, y, 0 );
        return 0;
    }
    alive = false;			// Check UP neighbours
    y+1 > Ny ? c = 1 : c = y + 1;
    x-1 < 1 ? d = Nx : d = x - 1;
    alive |= (IsAlive(d, c)==1);
    alive |= (IsAlive(x, c)==1);
    x+1 > Nx ? d = 1 : d = x + 1;
    alive |= (IsAlive(d, c)==1);
    if( !alive ) {			// Delete cell if there are no neighbours and finish its evolution
        SetAliveEvo( x, y, 0 );
        return 0;
    }
    alive = false;			// Check LEFT neighbours
    y+1 > Ny ? c = 1 : c = y + 1;
    x-1 < 1 ? d = Nx : d = x - 1;
    alive |= (IsAlive(d, c)==1);
    alive |= (IsAlive(d, y)==1);
    y-1 < 1 ? c = Ny : c = y - 1;
    alive |= (IsAlive(d, c)==1);
    if( !alive ) {			// Delete cell if there are no neighbours and finish its evolution
        SetAliveEvo( x, y, 0 );
        return 0;
    }
    alive = false;			// Check DOWN neighbours
    y-1 < 1 ? c = Ny : c = y - 1;
    x+1 > Nx ? d = 1 : d = x + 1;
    alive |= (IsAlive(d, c)==1);
    alive |= (IsAlive(x, c)==1);
    x-1 < 1 ? d = Nx : d = x - 1;
    alive |= (IsAlive(d, c)==1);
    if( !alive ) {			// Delete cell if there are no neighbours and finish its evolution
        SetAliveEvo( x, y, 0 );
        return 0;
    }
    alive = false;			// Check RIGHT neighbours
    y-1 < 1 ? c = Ny : c = y - 1;
    x+1 > Nx ? d = 1 : d = x + 1;
    alive |= (IsAlive(d, c)==1);
    alive |= (IsAlive(d, y)==1);
    y+1 > Ny ? c = 1 : c = y + 1;
    alive |= (IsAlive(d, c)==1);
    if( !alive ) {			// Delete cell if there are no neighbours and finish its evolution
        SetAliveEvo( x, y, 0 );
        return 0;
    }
    SetAliveEvo( x, y, 1 );		// Leave cell alive, if there are neibours at every side
  return 0;
}

// Run evolution for every cell of universe array
// Type of evolution is determined by 'mode'
// After full evolution, new state of of universe have to be copied into current state
inline void CAbase::WorldEvolutionLife( int mode )
{
  for( int ix = 1; ix <= Nx; ix++ ) {                   // Universe evolution for every cell
    for( int iy = 1; iy <= Ny; iy++ ) {
        switch(mode){
        case 0:
            CellEvolutionLife( ix, iy );
            break;
        case 1:
            CellEvolutionUnlimitedLife( ix, iy );
            break;
        case 2:
            CellEvolutionLimitedLife( ix, iy );
            break;
        case 3:
            CellEvolutionFractalLife( ix, iy );
            break;
        case 4:
            CellEvolutionRAND( ix, iy );
            break;
        case 5:
            CellEvolutionErosion( ix, iy );
            break;
        case 6:
            CellEvolutionMoving( ix, iy );
        case 8:
            CellEvolutionTwoLiquids( ix, iy );
        }
    }
  }
  nochanges = true;
  for( int ix = 1; ix <= Nx; ix++ ) {           // Copy new state to current universe
    for( int iy = 1; iy <= Ny; iy++ ) {
      if( world[iy*(Nx+2)+ix] != worldNew[iy*(Nx+2)+ix] ) nochanges = false;
      world[iy*(Nx+2)+ix] = worldNew[iy*(Nx+2)+ix];
    }
  }                                 // if nochanges == true, there are no evolution and we reached final stare of universe
}

//
// Run evolution for every cell of universe array in "Cyclic cellular automata" mode
// After full evolution, new state of of universe have to be copied into current state
//
inline void CAbase::WorldEvolutionLifeColor()
{
    for( int ix = 1; ix <= Nx; ix++ ) {             // Universe evolution for every cell
      for( int iy = 1; iy <= Ny; iy++ ) {
          CellEvolutionLifeColor( ix, iy );
      }
    }
    for( int ix = 1; ix <= Nx; ix++ ) {         // Copy new state to current universe
      for( int iy = 1; iy <= Ny; iy++ ) {
        worldColor[iy*(Nx+2)+ix] = worldColorNew[iy*(Nx+2)+ix];
      }
    }
}

//
// Creation or re-creation new current and evolution universe with default value (0 for non-border cell and -1 for border cell)
//
//
inline void CAbase::ResetWorldSize( int nx, int ny, bool del )
{
  Nx = nx;
  Ny = ny;
  if( !del ) {
    delete[] world;
    delete[] worldNew;
    // Color
    delete[] worldColor;
    delete[] worldColorNew;
    // Color
  }
  world = new int[(Ny+2)*(Nx+2)+1];
  worldNew = new int[(Ny+2)*(Nx+2)+1];
  // Color
  worldColor = new int[(Ny+2)*(Nx+2)+1];
  worldColorNew = new int[(Ny+2)*(Nx+2)+1];
  // Color
  for( int i = 0; i <= (Ny+2)*(Nx+2); i++ ) {
    if( (i < (Nx+2)) || (i >= (Ny+1)*(Nx+2)) || (i%(Nx+2) == 0) || (i%(Nx+2) == (Nx+1)) ) {
	world[i] = -1;
	worldNew[i] = -1;
    // Color
    worldColor[i] = -1;
    worldColorNew[i] = -1;
    // Color
    }
    else {
	world[i] = 0;
	worldNew[i] = 0;
    // Color
    worldColor[i] = 12;
    worldColorNew[i] = 12;
    // Color
    }
  }
}

/* Snake rules
 * Head: move directions:
 * 0 8 0
 * 4 0 6
 * 0 2 0
 * Tail: (n+10), (n+10-1), (n+10-2), ...
 * Food: 5
 * Border: -1
 * Evolution: change only 'this' cell (except snake growing case)
 */

// Prepare field for Snake game. Create snake and food
inline void CAbase::PrepareFieldSnake()
{
    srand( time(NULL) );
    ResetWorldSize( Nx, Ny, 1 );
    // Start position
    int startX = int(Nx/2);
    int startY = Ny - 7;
    SetAlive(startX, startY, 8);    // Head
    SetAlive(startX, startY+1, 12); // Tail with 3 cells
    SetAlive(startX, startY+2, 11);
    SetAlive(startX, startY+3, 10);
    CreateFood();
}

// Create one food cell on the field
inline bool CAbase::CreateFood()
{
    int maxIt(Nx*Ny);   // Maximal number of shots to prevent endless loops
    for( int i = 1; i < maxIt; i++ ) {
        int x = int(rand()%Nx + 1); // 'Random' coordinates with stdlib::rand()
        int y = int(rand()%Ny + 1); //TODO try other randomizers
        if( IsAlive(x, y) == 0 ) {
            SetAlive(x, y, 5);      // Set food cell
            SetAliveEvo(x, y, 5);
            return true;            // End loop and function if success
        }
    }
    return false;                   // End function if failure
}

// Evolution rules for every cell. Changing only cell (x, y) for every step
// TODO Make rules with evolution of (x+i, y+i) cells depending on (x, y). Will it be simplier?
inline int CAbase::CellEvolutionSnake( int x, int y )
{
    int thisCell = IsAlive(x, y);                   // Cell status before the evolution
    int dir[3][3] = {0,6,0,                         // Array for faster checking of snake direction
                     8,0,2,
                     0,4,0};
    SetAliveEvo(x, y, 0);
    if( thisCell == 0 || thisCell == 10 ) {         // Case 1: can move snake hear to this cell
        for( int i = -1; i <= 1; i++ ) {            // Check is head is around and can be moved here
            if( i == 0 ) continue;
            if( IsAlive(x+i, y) == dir[1-i][1] )
                SetAliveEvo(x, y, dir[1-i][1]);
            if( IsAlive(x, y+i) == dir[1][1-i] )
                SetAliveEvo(x, y, dir[1][1-i]);
        }
        return 1;                                   // return success if cell evolved
    }
    if( (thisCell == 2)||(thisCell == 4)||(thisCell == 6)||(thisCell == 8) ) {  // Case 2: snake head
        int tailMax = GetMaxAround(x, y, 0);        // Look for first tail cell (if other tail cells are around)
        SetAliveEvo(x, y, tailMax);                 // move tail cell
        return 1;                                   // return success if cell evolved
    }
    if( thisCell > 10 ) {                           // Case 3: it is tail cell for this and next step of evolution
        for( int i = -1; i <= 1; i++ ) {            // check if snake will eat this self cell
            if( i == 0 ) continue;
            if( (IsAlive(x+i, y) == dir[1-i][1]) || (IsAlive(x, y+i) == dir[1][1-i]) ) {
                SetAliveEvo(x, y, 0);               // if yes, kill this cell
                return 0;                           // return fail signal
            }
        }
        SetAliveEvo(x, y, thisCell-1);              // if no, move snake tail
        return 1;                                   // return success if cell evolved
    }
    if( thisCell == 5 ) {                           // Case 4: it is food
        SetAliveEvo(x, y, 5);
        bool food(false);
        for( int i = -1; i <= 1; i++ ) {            // Special case: grow up snake without world evolution if food was eaten
            if( i == 0 ) continue;
            if( IsAlive(x+i, y) == dir[1-i][1] ) {  // if yes (horizontal), mark food cell as snake head
                int tailMax = GetMaxAround(x+i, y, 10);
                SetAlive(x, y, dir[1-i][1]);
                SetAlive(x+i, y, tailMax+1);
                food = CreateFood();                // create new food
                return 2;                           // return signal to prevent world evolution
            }
            if( IsAlive(x, y+i) == dir[1][1-i] ) {  // if yes (vertical), mark food cell as snake head
                int tailMax = GetMaxAround(x, y+i, 10);
                SetAlive(x, y, dir[1][1-i]);
                SetAlive(x, y+i, tailMax+1);
                food = CreateFood();                // create new food
                return 2;                           // return signal to prevent world evolution
            }
        }
        return 1;                                   // return success if cell stays the same
    }
    if( thisCell == -1 ) {                          // if it is border
        SetAliveEvo(x, y, -1);
        return 1;
    }
    return 1;
}

// World evolution for snake game with possibility to skip evolution if snake grow up
inline void CAbase::WorldEvolutionSnake()
{
    int evolution = 0;
    for( int ix = 1; ix <= Nx; ix++ ) {
      for( int iy = 1; iy <= Ny; iy++ ) {
          evolution = CellEvolutionSnake( ix, iy );
          if( evolution == 2 ) break;
      }
      if( evolution == 2 ) break;
    }
    if( evolution == 2 ) return;
    nochanges = true;
    for( int ix = 1; ix <= Nx; ix++ ) {
      for( int iy = 1; iy <= Ny; iy++ ) {
        if( world[iy*(Nx+2)+ix] != worldNew[iy*(Nx+2)+ix] ) nochanges = false;
        world[iy*(Nx+2)+ix] = worldNew[iy*(Nx+2)+ix];
      }
    }
}

// Change the direction of snake movement
inline void CAbase::TurnSnake( int dir )
{
    for( int ix = 1; ix <= Nx; ix++ ) {
      for( int iy = 1; iy <= Ny; iy++ ) {
          int thisCell = IsAlive(ix, iy);
          // check if this cell is head of snake
          if( (thisCell == 2)||(thisCell == 4)||(thisCell == 6)||(thisCell == 8) )
              // check if we trying to change direction to opposit
              if( (dir==2&&thisCell==8)||(dir==8&&thisCell==2)||(dir==4&&thisCell==6)||(dir==6&&thisCell==4) )
                  return;                   // if yes, do nothing
              else
                  SetAlive(ix, iy, dir);    // if no, change direction
      }
    }
}

inline int CAbase::GetMaxAround( int x, int y, int start )
{
    int max = start;
    for( int i = -1; i <= 1; i++ ) {
        if( i == 0 ) continue;
        if( IsAlive(x+i, y) > max ) max = IsAlive(x+i, y);
        if( IsAlive(x, y+i) > max ) max = IsAlive(x, y+i);
    }
    return max;
}

// -------
// Two Liquids simple
// Sample, which demonstrate separation of two liquids with different surface tension
// 1st liquid - "0"; 2nd liquid - "1"
// Evolution: for every cell calculate sum of values in its neighbours including itself (X: {-1, 0, 1}; Y: {-1, 0, 1})
//            if sum > 5 of sum = 4 => set cell as "1", else "0"
inline int CAbase::CellEvolutionTwoLiquids( int x, int y )
{
    int n_sum = 0;
    int x1(0), y1(0);
    for( int ix = -1; ix <= 1; ix++ ) {
      for( int iy = -1; iy <= 1; iy++ ) {
        x1 = x + ix;
        y1 = y + iy;
        if( x1 < 1 ) x1 = Nx;
        if( x1 > Nx ) x1 = 1;
        if( y1 < 1 ) y1 = Ny;
        if( y1 > Ny ) y1 = 1;
        if( IsAlive( x1, y1 ) == 1 ) n_sum++;
      }
    }
    if( n_sum == 4 || n_sum > 5 )
      SetAliveEvo( x, y, 1 );
    else SetAliveEvo( x, y, 0 );
    return 0;
}

// Diffusion of two gases with Margolus neighborhood
// Margolus neighborhood in 2-dimensional case - 2x2 square block
// CA divides universe on such blocks and change every block independently
// CA works in 2 streps: 1.Margolus neighborhood starts from (X, Y); 2.Starts from (X+1, Y+1)
// 1st gas - "0", 2nd gas - "1"
// Evolution: cell values inside of every Margolus neighborhood shifts clockwise or counterclockwise
// Direction of shift is chosen randomly for every algorithm step end every Margolus neighborhood
inline int CAbase::CellEvolutionDiffusionMargolus( int x, int y, bool t )
{
    int p = int(rand()%2);
    if( t ) {
        x++;
        y++;
    }
    if( p == 0 ) {
        SetAliveEvo(x, y, IsAlive(x, y+1));
        SetAliveEvo(x, y+1, IsAlive(x+1, y+1));
        SetAliveEvo(x+1, y+1, IsAlive(x+1, y));
        SetAliveEvo(x+1, y, IsAlive(x, y));
    }
    else {
        SetAliveEvo(x, y, IsAlive(x+1, y));
        SetAliveEvo(x+1, y, IsAlive(x+1, y+1));
        SetAliveEvo(x+1, y+1, IsAlive(x, y+1));
        SetAliveEvo(x, y+1, IsAlive(x, y));
    }
}

// World evolution function for calculations with Margolus neighborhood
inline void CAbase::WorldEvolutionMargolus()
{
    srand( time(NULL) );
    for( int ix = 1; ix <= Nx-1; ix+=2 ) {          // Run 1st evolution step
        for( int iy = 1; iy <= Ny-1; iy+=2 ) {
            CellEvolutionDiffusionMargolus( ix, iy, 0 );
        }
    }
    for( int ix = 1; ix <= Nx; ix++ ) {             // Copy new state to current universe
      for( int iy = 1; iy <= Ny; iy++ ) {
        world[iy*(Nx+2)+ix] = worldNew[iy*(Nx+2)+ix];
      }
    }
    for( int ix = 1; ix <= Nx-2; ix+=2 ) {          // Run 2nd evolution step
        for( int iy = 1; iy <= Ny-2; iy+=2 ) {
            CellEvolutionDiffusionMargolus( ix, iy, 1 );
        }
    }
    nochanges = true;
    for( int ix = 1; ix <= Nx; ix++ ) {             // Copy new state to current universe
      for( int iy = 1; iy <= Ny; iy++ ) {
        if( world[iy*(Nx+2)+ix] != worldNew[iy*(Nx+2)+ix] ) nochanges = false;
        world[iy*(Nx+2)+ix] = worldNew[iy*(Nx+2)+ix];
      }
    }
}
