#include "modelEntities.h"
#include "sudokumodel.h"

void DifficultLvlBase::CreateNewGame(LevelBuilder* pBuilder)
{
    Init();
    pBuilder->MakeLevel(this);
}

QVector<Cell *> DifficultLvlBase::GetRegionByNumber(const GameTable &srcTable, int numOfRegion) const
{
    int nRowCounterMin = 0;
    int nColCounterMin = 0;

    int nRowCounterMax = 0;
    int nColCounterMax = 0;


    int nCurRow = 0;
    int nCurCol = 0;
    int nCurrentRegion = -1;
    while (true) {
        ++nCurrentRegion;

        if(nCurrentRegion == numOfRegion)
        {
            nRowCounterMin = nCurRow;
            nColCounterMin = nCurCol;
            nRowCounterMax = nRowCounterMin + GetCellCountingRowAndColumnForRegion();
            nColCounterMax = nColCounterMin + GetCellCountingRowAndColumnForRegion();
            break;

        }

        nCurCol += GetCellCountingRowAndColumnForRegion();
        if(nCurCol == GetColumnsCnt())
        {
            nCurCol = 0;
            nCurRow += GetCellCountingRowAndColumnForRegion();
        }
    }


    QVector<Cell*> result;

    for(int i = nRowCounterMin; i < nRowCounterMax; ++i)
    {

        for(int j = nColCounterMin; j < nColCounterMax; ++j)
        {
            Cell* pCell = srcTable.GetCell(i, j);
            result.push_back(pCell);
        }
    }

    return result;
}

void DifficultLvlBase::Init()
{
    for(int nRow = 0; nRow < GetRowsCnt(); ++ nRow)
    {
        for(int nCol = 0; nCol < GetColumnsCnt(); ++ nCol)
        {
            Cell newCell(nRow, nCol);
            newCell.SetValue("0");
            m_originalTable.push_back(newCell);
        }
    }
}

std::unique_ptr<DifficultLvlBase> DifficultLvl_4_4::Clone() const {return std::make_unique<DifficultLvl_4_4>();}

std::unique_ptr<DifficultLvlBase> DifficultLvl_9_9::Clone() const {return std::make_unique<DifficultLvl_9_9>();}

std::unique_ptr<DifficultLvlBase> DifficultLvl_16_16::Clone() const {return std::make_unique<DifficultLvl_16_16>();}

Cell *GameTable::GetCell(int nRow, int nCol)const {

    Cell proxyCell(nRow, nCol);

    for(int n = 0; n < m_table.size(); ++n)
    {
        auto& data = m_table[n];

        if( data == proxyCell)
        {
            return &data;
        }
    }
    return nullptr;
}

void Cell::Swap(Cell &other)
{
    QString strTmp = m_value;
    m_value = other.m_value;
    other.m_value = strTmp;
}
