#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QColor>
#include <QWidget>
#include "CAbase.h"

#include <QKeyEvent>

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = 0);
    ~GameWidget();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *event);

signals:
    //when one of the cell has been changed,emit this signal to lock the universeSize
    void environmentChanged(bool ok);
    //when game is over or clear is called,emit it to unlock the universeSize
    void gameEnds(bool ok);

public slots:
    void startGame(const int &number = -1); // start
    void stopGame(); // finish
    void clear(); // clear

    void focusOnKeyboard(); // keyboard for snake

    int cellNumber(); // number of the cells in one row
    void setCellNumber(const int &s); // set number of the cells in one row

    void setMode(const int &m); //set evolution mode
    void setCellMode(const int &m); //set cell mode

    int interval(); // interval between generations
    void setInterval(int msec); // set interval between generations

    QColor masterColor(); // color of the cells
    void setMasterColor(const QColor &color); // set color of the cells
    QColor SetColor(const int &color); //set color of the cells from their number

    void fillRandom(); //fill current universe with random color
    void goRandom(); // update current universe with Cyclic cellular automaton

    QString dump(); // dump of current universe
    void setDump(const QString &data); // set current universe from it's dump

private slots:
    void paintGrid(QPainter &p);
    void paintUniverse(QPainter &p);
    void newGeneration();
    void newGenerationColor();

private:
    QColor m_masterColor;
    QTimer* timer;
    QTimer* timerColor;
    int generations;
    CAbase ca1;
    int universeSize;
    int universeMode;
    int cellMode;
};

#endif // GAMEWIDGET_H
