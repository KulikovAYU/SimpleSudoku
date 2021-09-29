#include "mixingVariants.h"
#include "sudokumodel.h"
#include "sudokusolvechecker.h"
#include <QFile>
#include <QMessageBox>
#include <QSize>
#include <QTextStream>
#include <QDebug>


SudokuModel::SudokuModel()
{
    m_Game.Create(LevelsFactory::Create("4 x 4"));
}

int SudokuModel::rowCount(const QModelIndex &parent) const
{
    return  m_Game.GetLevel()->GetRowsCnt();
}

int SudokuModel::columnCount(const QModelIndex &parent) const
{
    return m_Game.GetLevel()->GetColumnsCnt();
}

QVariant SudokuModel::data(const QModelIndex &index, int role) const
{
    //Qt::BackgroundColorRole
    if (Qt::TextAlignmentRole == role)
        return Qt::AlignCenter;


    if(!index.isValid())
        return QVariant();

    int nRow = index.row();
    int nCol = index.column();


    if (Qt::BackgroundColorRole == role)
    {
        if(m_Game.IsSolvedCell(nRow, nCol))
            return  QBrush(QColor(85, 250, 20, 100));
        else if (m_Game.IsAutoGenCell(nRow, nCol))
            return  QBrush(QColor(188, 196, 230, 100));

        return QVariant();
    }


    if(Qt::EditRole != role && Qt::DisplayRole !=role)
        return QVariant();

    if( Qt::DisplayRole == role &&
            m_Game.IsVisibilityCellLocked(nRow, nCol))
    {
        return QVariant();
    }

    QString resValue;
    if(!m_Game.GetItemValue(nRow, nCol,resValue))
        return QVariant();


    return resValue;
}

bool SudokuModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

    if(!index.isValid() || role != Qt::EditRole)
        return false;

    int nRow = index.row();
    int nCol = index.column();

    //если клетка автосгенерирована или
    //решена то запрещаем измененять ее значение
    if( m_Game.IsCellChangingLocked(nRow, nCol) )
        return false;

    m_Game.SetState(nRow, nCol, eEditing);

    const QString& settingValue = value.toString();
    m_Game.SetItemValue(nRow, nCol, settingValue);

    if(m_Game.IsRequiredValue(nRow, nCol, settingValue))
        m_Game.SetState(nRow, nCol, eSolved);


    emit dataChanged(index,index);

    return true;
}

int SudokuModel::GetDifficultValue() const{return m_Game.GetLevel()->GetDifficultValue();}


void SudokuModel::SetDifficult(const QString& strDifficultName)
{
    //от пункта меню
    try {
        m_Game.Create(LevelsFactory::Create(strDifficultName));
    } catch (const std::exception& ex) {
        QMessageBox::critical(nullptr,"Ошибка", "Ошибка установки уровня");
    }
}

void SudokuModel::NewGame()
{
    //от пункта меню
    try {
        const QString& strExistGameName = m_Game.GetLevel()->GetName();
        m_Game.Create(LevelsFactory::Create(strExistGameName));
    } catch (const std::exception& ex) {
        QMessageBox::critical(nullptr,"Ошибка", "Ошибка установки уровня");
    }
}

bool SudokuModel::IsGameFinished() const{return m_Game.IsFinished();}

QString SudokuModel::GetGameName() const{return m_Game.GetName();}



std::unique_ptr<DifficultLvlBase> LevelsFactory::Create(const QString &strDifficultName)
{
    for(size_t nLvl = 0; nLvl < GetAvailableLevels().size(); ++nLvl)
    {
        //беру эталонный объект, из которого хочу создать копию
        const std::unique_ptr<DifficultLvlBase>& pCurrentTemplateObject = GetAvailableLevels().at(nLvl);
        if(pCurrentTemplateObject->GetName() == strDifficultName)
        {
            //изготавливаю из прототипа pCurrentTemplateObject новый объект
            return pCurrentTemplateObject->Clone();
        }
    }

    throw std::logic_error("unavailable level");
}

QVector<QString> LevelsFactory::GetLevelsName()
{
    //бежим по каждому из прототипов (эталонных объектов)
    //и собираем имена
    QVector<QString> levelsNames;
    for( auto& lvl : GetAvailableLevels())
        levelsNames.push_back(lvl->GetName());

    return levelsNames;
}

std::vector<std::unique_ptr<DifficultLvlBase>>& LevelsFactory::GetAvailableLevels()
{
    static std::vector<std::unique_ptr<DifficultLvlBase>> availableLevels;
    //создаю эталонные объекты - прототипы, из которых в дальнейшем
    //конструерую объекты уровней через clone()
    if(availableLevels.empty())
    {
        availableLevels.push_back(std::make_unique<DifficultLvl_4_4>());
        availableLevels.push_back(std::make_unique<DifficultLvl_9_9>());
        availableLevels.push_back(std::make_unique<DifficultLvl_16_16>());
    }

    return availableLevels;
}

void SudokuGame::Create(std::unique_ptr<DifficultLvlBase> pLevel)
{
    m_pLevel.reset();
    m_pLevel = std::move(pLevel);
    LevelBuilder levelBuilder;
    m_pLevel->CreateNewGame(&levelBuilder);
}

bool SudokuGame::GetItemValue(int nRow, int nCol, QString &outValue) const
{
    Cell* pSelectedCell = m_pLevel->GetPlayingTable().GetCell(nRow, nCol);

    if(pSelectedCell)
    {
        outValue = pSelectedCell->GetValue();
        return true;
    }

    return false;
}

void SudokuGame::SetItemValue(int nRow, int nCol, const QString &value)
{
    Cell* pSelectedCell = GetPlayingCell(nRow, nCol);

    if(pSelectedCell)
        pSelectedCell->SetValue(value);
}

bool SudokuGame::IsSolvedCell(int nRow, int nCol) const
{
    Cell* pGameCell = GetPlayingCell(nRow, nCol);
    if(!pGameCell)
        return false;

    return  eSolved == pGameCell->GetState();
}

bool SudokuGame::IsAutoGenCell(int nRow, int nCol) const
{
    Cell* pGameCell = GetPlayingCell(nRow, nCol);
    if(!pGameCell)
        return false;

    return  eAutoGenerated == pGameCell->GetState();

}

bool SudokuGame::IsVisibilityCellLocked(int nRow, int nCol) const
{
    Cell* pGameCell = GetPlayingCell(nRow, nCol);
    if(!pGameCell)
        return false;

    return  eInvisible == pGameCell->GetState() || eUndef == pGameCell->GetState();
}

Cell *SudokuGame::GetPlayingCell(int nRow, int nCol) const
{
    return m_pLevel->GetPlayingTable().GetCell(nRow, nCol);
}

bool SudokuGame::IsCellChangingLocked(int nRow, int nCol) const
{
    Cell* pGameCell = GetPlayingCell(nRow, nCol);
    if(!pGameCell)
        return false;

    return eAutoGenerated == pGameCell->GetState() ||
            eSolved == pGameCell->GetState();
}

bool SudokuGame::IsRequiredValue(int nRow, int nCol, const QString& value) const
{
    Cell* pOriginCell = m_pLevel->GetOriginalTable().GetCell(nRow, nCol);
    if(pOriginCell)
        return value == pOriginCell->GetValue();

    return false;
}

void SudokuGame::SetState(int nRow, int nCol, CellState state)
{
    Cell* pPlayingCell = GetPlayingCell(nRow, nCol);
    if(pPlayingCell)
        pPlayingCell->SetState(state);
}

bool SudokuGame::IsFinished() const
{

    SudokuSolveChecker checker(m_pLevel.get());
    if(checker.IsSolved())
        return true;

    for(int nRow = 0; nRow < GetLevel()->GetRowsCnt(); ++nRow)
    {
        for(int nCol = 0; nCol < GetLevel()->GetColumnsCnt(); ++nCol)
        {
            Cell* pPlayingCell = GetPlayingCell(nRow, nCol);
            if(pPlayingCell && pPlayingCell->GetState() != eSolved &&
                    pPlayingCell->GetState() != eAutoGenerated)
                return false;

        }
    }

    return true;
}

void SudokuGame::Save(const QString &strPath) const
{
    QFile file(strPath);
    if(file.open(QIODevice::WriteOnly))
    {
        try {
            QTextStream out(&file);
            out << *m_pLevel;
            file.close();
        } catch (std::exception& ex) {
            qDebug() << "Error Writing to file because: " << ex.what();
        }
    }
}

void LevelBuilder::MakeLevel(DifficultLvlBase *pCurrentLvl)
{
    //Шаг 1. Взять за основу базовую сетку
    CreateBaseGrid(pCurrentLvl);

    //Шаг 2. Перетасовать сетку
    MixGrid(pCurrentLvl);
}

void LevelBuilder::CreateBaseGrid(DifficultLvlBase* pCurrentLvl)
{
    int nCellCounting = pCurrentLvl->GetCellCountingRowAndColumnForRegion();

    const GameTable& gametable = pCurrentLvl->GetOriginalTable();

    for(int row = 0; row < pCurrentLvl->GetRowsCnt(); ++row)
    {
        for(int col = 0; col < pCurrentLvl->GetColumnsCnt(); ++col)
        {
            Cell* pCell = gametable.GetCell(row, col);
            if(!pCell)
                continue;

            //волшебная формула генерации сетки
            int value = (row*nCellCounting + row/nCellCounting + col) % (nCellCounting*nCellCounting) + 1;
            pCell->SetValue(QString::number(value));
            pCell->SetState(CellState::eAutoGenerated);
        }
    }
}

void LevelBuilder::MixGrid(DifficultLvlBase* pCurrentLvl)
{
    SudokuGenerator().Invoke(pCurrentLvl);
}

