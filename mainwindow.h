#ifndef MAINWINDOWREFACTOR_H
#define MAINWINDOWREFACTOR_H

#include "sudokumodel.h"

#include <QMainWindow>
#include <QObject>


class QGridLayout;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:
    void OnButtonClicked();
    void OnNewGameClicked();
    void OnLvlSelected();
    void OnSaveToFileClicked();

signals:


private:
    void CreateControls();
    void CreatePlayArea();
    void CreatePlayButtons();
    void CreateLvlsMenu();

    void DestroyPlayArea();
    void DestroyPlayButtons();

    QGridLayout* GetMainLayout() const;

    SudokuModel m_sudokuModel;
};

#endif // MAINWINDOWREFACTOR_H
