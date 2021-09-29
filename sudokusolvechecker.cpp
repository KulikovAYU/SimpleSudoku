#include "sudokusolvechecker.h"

#include <QSet>

bool SudokuSolveChecker::IsSolved() const
{
    return IsRowsOk() && IsColumnsOk() && IsRegionsOk();
}

bool SudokuSolveChecker::IsRowsOk() const
{
    for(int nRow = 0; nRow < m_pCurrentLvl->GetRowsCnt(); ++nRow)
    {
        std::vector<QString> rowsBuffer;
        for(int nCol = 0; nCol < m_pCurrentLvl->GetColumnsCnt(); ++nCol)
        {
            Cell* pCurrCell =  m_pCurrentLvl->GetPlayingTable().GetCell(nRow, nCol);
            if(!pCurrCell)
                return false;

             //проверяем только открытые ячейки
            if(pCurrCell->GetState() == eInvisible)
                 return false;

            const QString& cellValue = pCurrCell->GetValue();

            if(IsEmptyOrExistValueInContainer(rowsBuffer, cellValue))
                return false;

            rowsBuffer.push_back(cellValue);
        }
    }
    return true;
}

bool SudokuSolveChecker::IsColumnsOk() const
{
    for(int nCol = 0; nCol < m_pCurrentLvl->GetColumnsCnt(); ++nCol)
    {
        std::vector<QString> columnsBuffer;
        for(int nRow = 0; nRow < m_pCurrentLvl->GetRowsCnt(); ++nRow)
        {
            Cell* pCurrCell = m_pCurrentLvl->GetPlayingTable().GetCell(nRow, nCol);
            if(!pCurrCell)
                return false;
             //проверяем только открытые ячейки
            if(pCurrCell->GetState() == eInvisible)
                 return false;

            const QString& cellValue = pCurrCell->GetValue();

            if(IsEmptyOrExistValueInContainer(columnsBuffer, cellValue))
                return false;

            columnsBuffer.push_back(cellValue);
        }
    }

    return true;
}

bool SudokuSolveChecker::IsRegionsOk() const
{
    int nRegionsCnt = m_pCurrentLvl->GetRegionsCnt();
    const GameTable& playTable = m_pCurrentLvl->GetPlayingTable();
    //т.к. QSet содержит только уникальные значения
    //то просто заполним его.
    //если его размер будет == размеру клеток региона, то все ОК

    int nCellsInRegion = m_pCurrentLvl->GetCellCountingRowAndColumnForRegion() *
                         m_pCurrentLvl->GetCellCountingRowAndColumnForRegion();

    for(int i = 0; i < nRegionsCnt; ++i)
    {
        QSet<QString> setsOfStr;

        const QVector<Cell *>& reqRegion = m_pCurrentLvl->GetRegionByNumber(playTable, i);

        for(const Cell* pCell : reqRegion)
        {
            if(pCell)
            {
                 //проверяем только открытые ячейки
                if(pCell->GetState() == eInvisible)
                     return false;

                 setsOfStr.insert( pCell->GetValue());
            }

        }

        if(setsOfStr.size() != nCellsInRegion)
            return false;
    }

    return true;
}

bool SudokuSolveChecker::IsEmptyOrExistValueInContainer(std::vector<QString> &buffer, const QString &value) const
{
    for(const QString& currEl : buffer)
    {
        if(currEl == "")
            return true;

        if(currEl == value)
            return true;
    }

    return false;
}
