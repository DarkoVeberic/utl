#include <vector>
#include <iostream>

using namespace std;


class Sudoku {
public:
  typedef vector<char> CType;
  Sudoku(const char init[9][9]);
  bool Solve();
  void Candidates(const int ind, bool cand[10]);
  void Dump(const int index = -1);
private:
  CType fArray;
};


Sudoku::Sudoku(const char init[9][9])
{
  fArray.reserve(9*9);
  for (int i = 0; i < 9; ++i)
    for (int j = 0; j < 9; ++j)
      fArray.push_back(init[i][j]);
}


bool
Sudoku::Solve()
{
  int ind;
  for (ind = 0; ind < 9*9; ++ind)
    if (!fArray[ind])
      break;

  if (ind == 9*9) {

    cout << "found solution:\n";
    Dump();

    return true;

  } else {

    bool cand[10] = { 0,  1,1,1,  1,1,1,  1,1,1 };
    Candidates(ind, cand);

    Sudoku s(*this);
    for (int i = 1; i <= 9; ++i)
      if (cand[i]) {
        s.fArray[ind] = i;
        s.Solve();
      }
    //cout << "nothing found for position " << ind << endl;
    return false;
  }
}


void
Sudoku::Candidates(const int ind, bool cand[10])
{
  const int row = ind / 9;
  const int end = 9*(row + 1);
  for (int i = 9*row; i < end; ++i) {
    const int entry = fArray[i];
    if (entry)
      cand[entry] = false;
  }

  const int col = ind % 9;
  for (int i = col; i < 9*9; i += 9) {
    const int entry = fArray[i];
    if (entry)
      cand[entry] = false;
  }

  const int start = 9*3*(row/3) + 3*(col/3);
  for (int i = 0; i < 3; ++i) {
    const int off = start + 9*i;
    for (int j = 0; j < 3; ++j) {
      const int entry = fArray[off + j];
      if (entry)
        cand[entry] = false;
    }
  }
}


void
Sudoku::Dump(const int index)
{
  int ind = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      for (int k = 0; k < 3; ++k) {
        if (k)
          cout << " | ";
        for (int m = 0; m < 3; ++m) {
          const int ent = fArray[ind];
          if (ent)
            cout << ent;
          else
            cout << (ind == index ? 'x' : '.');
          ++ind;
        }
      }
      cout << '\n';
    }
    if (i < 2)
      cout << "----+-----+----\n";
  }
}


int
main()
{
  const char init[9][9] = {
    { 0,8,0,  0,0,0,  0,5,1 },
    { 5,0,0,  0,0,7,  0,0,0 },
    { 0,0,4,  0,2,9,  0,0,0 },

    { 0,6,1,  0,7,0,  0,2,0 },
    { 0,0,0,  8,0,5,  0,0,0 },
    { 0,5,0,  0,6,0,  3,8,0 },

    { 0,0,0,  3,4,0,  8,0,0 },
    { 0,0,0,  7,0,0,  0,0,5 },
    { 6,2,0,  0,0,0,  0,3,0 }
  };
  Sudoku s(init);
  cout << "start with:\n";
  s.Dump();
  cout << '\n';
  s.Solve();

  return 0;
}
