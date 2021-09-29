#ifndef SUDOKUSOLVECHECKER_H
#define SUDOKUSOLVECHECKER_H

#include "modelEntities.h"



class SudokuSolveChecker
{
public:
    explicit SudokuSolveChecker(const DifficultLvlBase* pCurrentLvl) :m_pCurrentLvl(pCurrentLvl){}

    bool IsSolved() const;

private:
    bool IsRowsOk() const;

    bool IsColumnsOk() const;

    bool IsRegionsOk() const;

    bool IsEmptyOrExistValueInContainer(std::vector<QString>& buffer, const QString& value) const;

   const DifficultLvlBase* m_pCurrentLvl;

};

#endif // SUDOKUSOLVECHECKER_H
