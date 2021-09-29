#include "mixingVariants.h"
#include <QDebug>
#include <QRandomGenerator>

int Randomizer::GetValue(int nMin, int nMax)
{
    return QRandomGenerator::global()->bounded(nMin, nMax + 1);
}

void TranspositionVariant::Invoke(DifficultLvlBase *pCurrentLvl)
{
    GameTable& gametable = pCurrentLvl->GetOriginalTable();

    for(int row = 0; row < pCurrentLvl->GetRowsCnt(); ++row)
    {
        for(int col = row; col < pCurrentLvl->GetColumnsCnt(); ++col)
        {

            Cell* pFirstCell = gametable.GetCell(row, col);
            Cell* pSecondCell = gametable.GetCell(col, row);

            if(pFirstCell && pSecondCell)
                pFirstCell->Swap(*pSecondCell);
        }
    }
}

void SwapRowsSmallVariant::Invoke(DifficultLvlBase *pCurrentLvl)
{
    int nCellCounting = pCurrentLvl->GetCellCountingRowAndColumnForRegion();

    int nArea = Randomizer::GetValue(0, nCellCounting);
    int nline1 = Randomizer::GetValue(0, nCellCounting);

    //номер 1 строки для обмена
    int nRow1 = nArea * nCellCounting + nline1;

    int nline2 = Randomizer::GetValue(0, nCellCounting);
    while (nline1 == nline2)
        nline2 = Randomizer::GetValue(0, nCellCounting);

    //номер 2 строки для обмена
    int nRow2 = nArea * nCellCounting + nline2;

    GameTable& gametable = pCurrentLvl->GetOriginalTable();

    for(int col = 0; col < pCurrentLvl->GetColumnsCnt(); ++col)
    {
        Cell* pFirstCell = gametable.GetCell(nRow1, col);
        Cell* pSecondCell = gametable.GetCell(nRow2, col);

        if(pFirstCell && pSecondCell)
            pFirstCell->Swap(*pSecondCell);
    }
}

void SwapColumnsSmallVariant::Invoke(DifficultLvlBase *pCurrentLvl)
{
    TranspositionVariant transpositionVar;
    transpositionVar.Invoke(pCurrentLvl);

    SwapRowsSmallVariant smallRowsVar;
    smallRowsVar.Invoke(pCurrentLvl);

    transpositionVar.Invoke(pCurrentLvl);
}

void SwapRowsAreaVariant::Invoke(DifficultLvlBase *pCurrentLvl)
{
    int nCellCounting = pCurrentLvl->GetCellCountingRowAndColumnForRegion();
    int nArea1 = Randomizer::GetValue(0, nCellCounting);

    int nArea2 = Randomizer::GetValue(0, nCellCounting);
    while (nArea1 == nArea2)
        nArea2 = Randomizer::GetValue(0, nCellCounting);


    GameTable& gametable = pCurrentLvl->GetOriginalTable();

    for(int i = 0; i < nCellCounting; ++i)
    {
        int nRow1 = nArea1 * nCellCounting + i;
        int nRow2 = nArea2 * nCellCounting + i;

        for(int nCol = 0; nCol < pCurrentLvl->GetColumnsCnt(); ++nCol)
        {
            Cell* pFirstCell = gametable.GetCell(nRow1, nCol);
            Cell* pSecondCell = gametable.GetCell(nRow2, nCol);

            if(pFirstCell && pSecondCell)
                pFirstCell->Swap(*pSecondCell);
        }
    }
}

void SwapColumnsAreaVariant::Invoke(DifficultLvlBase *pCurrentLvl)
{
    TranspositionVariant transpositionVar;
    transpositionVar.Invoke(pCurrentLvl);

    SwapRowsAreaVariant smallRowsVar;
    smallRowsVar.Invoke(pCurrentLvl);

    transpositionVar.Invoke(pCurrentLvl);
}

SudokuGenerator::SudokuGenerator(int nMaxAttemptsCnt) : m_nMaxAttemptsCnt(nMaxAttemptsCnt)
{
    InitGenerationVariants();
}

void SudokuGenerator::Invoke(DifficultLvlBase* pCurrentLvl)
{

    MixOriginalGrid(pCurrentLvl);
    pCurrentLvl->CopyOriginalToPlayingTable();
    MakeInvisibleSomeCells(pCurrentLvl);
}

void SudokuGenerator::InitGenerationVariants()
{
    m_levels.push_back(std::make_unique<TranspositionVariant>());
    m_levels.push_back(std::make_unique<SwapRowsSmallVariant>());
    m_levels.push_back(std::make_unique<SwapColumnsSmallVariant>());
    m_levels.push_back(std::make_unique<SwapRowsAreaVariant>());
    //m_levels.push_back(std::make_unique<SwapColumnsAreaVariant>());
}

void SudokuGenerator::MixOriginalGrid(DifficultLvlBase *pCurrentLvl)
{
    while (m_nMaxAttemptsCnt != 0) {
        int nVariant = Randomizer::GetValue(0, m_levels.size() - 1);
        qDebug()<< nVariant;
        m_levels[nVariant]->Invoke(pCurrentLvl);
        --m_nMaxAttemptsCnt;
    }
}

void SudokuGenerator::MakeInvisibleSomeCells(DifficultLvlBase *pCurrentLvl)
{

    int nMaxInvisibleCells = pCurrentLvl->GetMaxInvisibleCells();

    while (nMaxInvisibleCells > 0) {

        MakeInvisibleRandomRowAndColumn(pCurrentLvl);
        --nMaxInvisibleCells;

    }
}

void SudokuGenerator::MakeInvisibleRandomRowAndColumn(DifficultLvlBase *pCurrentLvl)
{
    while (true)
    {
        int nRow = Randomizer::GetValue(0, pCurrentLvl->GetRowsCnt() - 1);
        int nCol = Randomizer::GetValue(0, pCurrentLvl->GetColumnsCnt() - 1);

        Cell* pCell = pCurrentLvl->GetPlayingTable().GetCell(nRow, nCol);
        if(pCell && eInvisible != pCell->GetState()){
            pCell->SetState(eInvisible);
            break;
        }
    }
}
