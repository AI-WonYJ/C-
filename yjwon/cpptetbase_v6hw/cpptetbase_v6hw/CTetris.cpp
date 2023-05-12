#include "CTetris.h"

using namespace std;

///**************************************************************
/// static member variables and functions
///**************************************************************

Matrix *** CTetris::setOfColorBlockObjects = NULL;

void CTetris::init(int **setOfColorBlockArrays, int nTypes, int nDegrees) {
  if (setOfBlockObjects != NULL) // already allocated?
    deinit();

  numTypes = nTypes;
  numDegrees = nDegrees;

  setOfBlockObjects = new Matrix**[numTypes]; // allocate 1d array of pointers to Matrix pointer arrays

  for (int t = 0; t < numTypes; t++)
    setOfBlockObjects[t] = new Matrix*[numDegrees]; // allocate 1d array of Matrix pointers

  for (int t = 0; t < numTypes; t++) {
    int *array = setOfColorBlockArrays[numDegrees * t];
    int idx, size;
    for (idx = 0; array[idx] != -1 ; idx++); // find the element of -1 in array[]
    for (size = 0; size*size < idx; size++); // comupte the square root of idx
    wallDepth = (size > wallDepth ? size : wallDepth);
    for (int d = 0; d < numDegrees; d++) { // allocate matrix objects
      int *array2 = new int[size*size+1];
      int k; 
      for (k = 0; k < size*size; k++)
        array2[k] = setOfColorBlockArrays[numDegrees * t + d][k];
      array2[k] = -1;
      setOfBlockObjects[t][d] = new Matrix(array2, size, size);
      delete[] array2;
      //cout << *setOfBlockObjects[t][d] << endl;
    }
  }
}



void CTetris::deinit(void) {
  // Tetris::deinit(); // call superclass' function

  for (int t = 0; t < numTypes; t++)
    for (int d = 0; d < numDegrees; d++)
      delete setOfColorBlockObjects[t][d]; // deallocate matrix objects

  for (int t = 0; t < numTypes; t++)
    delete [] setOfColorBlockObjects[t]; // deallocate 1d array of Matrix pointers

  delete []  setOfColorBlockObjects; // deallocate 1d array of pointers to Matrix pointer arrays

  setOfColorBlockObjects = NULL;
}

///**************************************************************
/// dynamic member variables and functions
///**************************************************************

/// constructors and destructor

CTetris::CTetris(int cy, int cx) : Tetris(cy, cx) {
  iCScreen = new Matrix(iScreen);
  oCScreen = new Matrix(oScreen);
  currCBlk = NULL;
}

CTetris::~CTetris() {
  delete iCScreen;
  delete oCScreen;
}

/// mutators
TetrisState CTetris::accept(char key) {
  // TetrisState _state = Tetris::accept(key); 
  if (state == TetrisState::Finished){
    cout << 3<< endl;
    return state;
    }

  else if (state == TetrisState::NewBlock) {
  
    int idx = key - '0';
    if (idx < 0 || idx >= numTypes) {
      cout << "Tetris::accept: wrong block index!" << endl;
      return state = TetrisState::NewBlock;
    }

    state = TetrisState::Running;

    // select a new block
    type = idx;
    degree = 0;
    top = 0; 
    left = cols/2 - wallDepth/2;

    cout << type << endl;

    // init variables for screen refresh with the new block
    cout << 1 << endl;
    currCBlk = setOfBlockObjects[type][degree];
    Matrix *tempBlk = iCScreen->clip(top, left, top + currCBlk->get_dy(), left + currCBlk->get_dx());
    Matrix *tempBlk2 = tempBlk->add(currCBlk);
    delete tempBlk;
    cout << 1 << endl;

    // update oScreen before conflict test
    oCScreen->paste(iCScreen, 0, 0);
    oCScreen->paste(tempBlk2, top, left);
    cout << 1 << endl;
    if (tempBlk2->anyGreaterThan(1)) // exit the game
      cout << 4<< endl;
      state = TetrisState::Finished;
    delete tempBlk2;

    return state; // = Running or Finished
  }
  else if (state == TetrisState::Running) {
    cout << 3<< endl;

    state = TetrisState::Running;
    bool touchDown = false;
    Matrix *tempBlk, *tempBlk2;

    switch (key) { // perform the requested action
      case 'a': left--; break;
      case 'd': left++; break;
      case 'w': 
        degree = (degree + 1) % numDegrees; 
        currCBlk = setOfBlockObjects[type][degree]; 
        break;
      case 's': top++; break;
      case ' ': 
        while (true) {
          top++;
          tempBlk = iCScreen->clip(top, left, top + currCBlk->get_dy(), left + currCBlk->get_dx());
          tempBlk2 = tempBlk->add(currCBlk);
          delete tempBlk;
          if (tempBlk2->anyGreaterThan(1)) {
            delete tempBlk2;
            break;
          }
          delete tempBlk2;
        }
        break;
      default: cout << "Tetris::accept: wrong key input" << endl;
    }

    tempBlk = iCScreen->clip(top, left, top + currCBlk->get_dy(), left + currCBlk->get_dx());
    tempBlk2 = tempBlk->add(currCBlk);
    delete tempBlk;
    if (tempBlk2->anyGreaterThan(1)) {
      switch (key) { // undo the requested action
        case 'a': left++; break;
        case 'd': left--; break;
        case 'w': 
          degree = (degree + 3) % numDegrees; 
          currCBlk = setOfBlockObjects[type][degree]; 
          break;
        case 's': top--; touchDown = true; break;
        case ' ': top--; touchDown = true; break;
      }
      delete tempBlk2;
      tempBlk = iCScreen->clip(top, left, top + currCBlk->get_dy(), left + currCBlk->get_dx());
      tempBlk2 = tempBlk->add(currCBlk);    
      delete tempBlk;
    }

    // update oScreen
    oCScreen->paste(iCScreen, 0, 0);
    oCScreen->paste(tempBlk2, top, left);
    delete tempBlk2;

    if (touchDown) {
      oCScreen = deleteFullLines(oCScreen, currCBlk, top, wallDepth);
      iCScreen->paste(oCScreen, 0, 0);
      state = TetrisState::NewBlock;
    }

    return state; // = Running or NewBlock
  }
  
  return state; // not reachable
}
