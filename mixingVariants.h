#ifndef GRIDGENERATOR_H
#define GRIDGENERATOR_H

#include "modelEntities.h"
#include <vector>
#include <memory>

//алгоритмы генерации сеток: https://www.pvsm.ru/python/42274

class Randomizer
{
public:
    static int GetValue(int nMin, int nMax);
};


//базовый класс вариантов генерации сетки
class MixingVariantBase
{
public:
    virtual void Invoke(DifficultLvlBase* pCurrentLvl) = 0;
    virtual ~MixingVariantBase(){}
};

// 1 Транспонирование всей таблицы — столбцы становятся строками и наоборот (transposing)
class TranspositionVariant : public MixingVariantBase
{
public:
    void Invoke(DifficultLvlBase* pCurrentLvl) override;
};

// 2 Обмен двух строк в пределах одного района
class SwapRowsSmallVariant : public MixingVariantBase
{
public:
    void Invoke(DifficultLvlBase* pCurrentLvl) override;
};

// 3 Обмен двух столбцов в пределах одного района
class SwapColumnsSmallVariant : public MixingVariantBase
{
public:
    void Invoke(DifficultLvlBase* pCurrentLvl) override;
};

// 4 Обмен двух районов по горизонтали
class SwapRowsAreaVariant : public MixingVariantBase
{
public:
    void Invoke(DifficultLvlBase* pCurrentLvl) override;
};

// 5 Обмен двух районов по вертикали
class SwapColumnsAreaVariant : public MixingVariantBase
{
public:
    void Invoke(DifficultLvlBase* pCurrentLvl) override;
};



// генератор судоку
class SudokuGenerator
{
public:
    //nMaxAttemptsCnt - max кол-во попыток
    explicit SudokuGenerator(int nMaxAttemptsCnt = 4);

    void Invoke(DifficultLvlBase* pCurrentLvl);

private:
    void InitGenerationVariants();

    //примение алгоритмов смешивания значений
    void MixOriginalGrid(DifficultLvlBase* pCurrentLvl);

    //скрытие некоторых ячеек игровой сетки
    void MakeInvisibleSomeCells(DifficultLvlBase* pCurrentLvl);

    //скрытие рандомной строки и столбца
    void MakeInvisibleRandomRowAndColumn(DifficultLvlBase* pCurrentLvl);

    std::vector<std::unique_ptr<MixingVariantBase>> m_levels;

    int m_nMaxAttemptsCnt;
};

#endif // GRIDGENERATOR_H
