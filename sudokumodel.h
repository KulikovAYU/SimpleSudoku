#ifndef SUDOKUMODEL_H
#define SUDOKUMODEL_H

#include "modelEntities.h"
#include <QObject>
#include <QAbstractTableModel >
#include <QVector>
#include <memory>
#include <vector>

//http://itnotesblog.ru/note.php?id=159



//Сама игра (механика)
class SudokuGame
{
public:
    void Create(std::unique_ptr<DifficultLvlBase> pLevel);

    const std::unique_ptr<DifficultLvlBase>& GetLevel() const {return m_pLevel;}

    QString GetName() const{return "Sudoku";}


    bool GetItemValue(int nRow, int nCol, QString& outValue) const;
    void SetItemValue(int nRow, int nCol, const QString& value);
    bool IsSolvedCell(int nRow, int nCol) const;
    bool IsAutoGenCell(int nRow, int nCol) const;
    bool IsVisibilityCellLocked(int nRow, int nCol) const;


    bool IsCellChangingLocked(int nRow, int nCol) const;
    bool IsRequiredValue(int nRow, int nCol, const QString& value) const;
    void SetState(int nRow, int nCol,CellState state);

    bool IsFinished() const;

    void Save(const QString& strPath) const;

    Cell* GetPlayingCell(int nRow, int nCol) const;

private:
    mutable std::unique_ptr<DifficultLvlBase> m_pLevel;
};



//фабрика для генерации уровня
class LevelsFactory
{
public:
    static  std::unique_ptr<DifficultLvlBase> Create(const QString& strDifficultName);
    static QVector<QString> GetLevelsName();
    static std::vector<std::unique_ptr<DifficultLvlBase>>& GetAvailableLevels();
};


//класс, отвечающий за логику генерации
//сетки судоку
class LevelBuilder
{
public:
    void MakeLevel(DifficultLvlBase* pCurrentLvl);

private:
    //генерация основной базовой сетки
    void CreateBaseGrid(DifficultLvlBase* pCurrentLvl);
    //перетасовка сетки
    void MixGrid(DifficultLvlBase* pCurrentLvl);
};

class SudokuModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    SudokuModel();

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex& index, int role ) const override;
    bool setData( const QModelIndex& index, const QVariant& value, int role ) override;

    int GetDifficultValue() const;
    void SetDifficult(const QString& strDifficultName);

    void NewGame();

    bool IsGameFinished() const;

    QString GetGameName() const;

    void SaveGame(const QString& strPath) const {m_Game.Save(strPath);}

private:

    SudokuGame m_Game;
};
#endif // SUDOKUMODEL_H
