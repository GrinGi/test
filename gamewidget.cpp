#include <QMessageBox>
#include <QTimer>
#include <QMouseEvent>
#include <QDebug>
#include <QRectF>
#include <QPainter>
#include <qmath.h>
#include "gamewidget.h"
#include "QTime"


GameWidget::GameWidget(QWidget *parent) :
    QWidget(parent),
    timer(new QTimer(this)),
    timerColor(new QTimer(this)),
    generations(-1),
    ca1(),
    universeSize(50),
    universeMode(0),
    cellMode(0)
{
    timer->setInterval(300);
    timerColor->setInterval(50);
    m_masterColor = "#000";
    ca1.ResetWorldSize(universeSize, universeSize);
    connect(timer, SIGNAL(timeout()), this, SLOT(newGeneration()));
    connect(timerColor, SIGNAL(timeout()), this, SLOT(newGenerationColor()));

}

GameWidget::~GameWidget()
{

}

void GameWidget::startGame(const int &number)  // Start Game
{
    generations = number;
    timer->start();
}

void GameWidget::stopGame()  //Stop Game
{
    timer->stop();
    timerColor->stop();
}

void GameWidget::clear() // Clear game field
{
    for(int k = 1; k <= universeSize; k++) {
        for(int j = 1; j <= universeSize; j++) {
            ca1.SetAlive(j,k,0);
        }
    }
    gameEnds(true);
    ca1.ResetWorldSize(universeSize,universeSize);
    update();

}

int GameWidget::cellNumber()  // Number of the cells in one row
{
    return universeSize;
}

void GameWidget::setCellNumber(const int &s) // Set number of the cells in one row
{
    universeSize = s;
    ca1.ResetWorldSize(s,s);
    update();
}

void GameWidget::setMode(const int &m) //Set evolution mode
{
    if( m == 7 ) ca1.PrepareFieldSnake();
        universeMode = m;
    if( m == 9 ) universeMode = m;
}

void GameWidget::setCellMode(const int &m) //Set cell mode
{
    cellMode = m;
}

void GameWidget::fillRandom() //Fill current universe with random color
{
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    for(int k = 1; k <= universeSize; k++) {
        for(int j = 1; j <= universeSize; j++) {
            ca1.SetAlive(j,k,1);
            ca1.SetColor(j,k,qrand() % 12);
        }
    }

    update();
}

void GameWidget::goRandom() // Update current universe with Cyclic cellular automaton
{
    timerColor->start();
}

QString GameWidget::dump() // Dump of current universe
{
    char temp;
    QString master = "";
    for(int k = 1; k <= universeSize; k++) {
        for(int j = 1; j <= universeSize; j++) {
            if(ca1.IsAlive(j,k) == 1) {
                temp = '*';
            } else {
                temp = 'o';
            }
            master.append(temp);
        }
        master.append("\n");
    }
    return master;
}

void GameWidget::setDump(const QString &data) // Set current universe from it's dump
{
    int current = 0;
    for(int k = 1; k <= universeSize; k++) {
        for(int j = 1; j <= universeSize; j++) {
           if(data[current] == '*') ca1.SetAlive(j,k,1);
            current++;
        }
        current++;
    }
    ca1.WorldEvolutionLife(universeMode);
    update();
}

int GameWidget::interval() // Interval between generations
{
    return timer->interval();
}

void GameWidget::setInterval(int msec) // Set interval between generations
{
    timer->setInterval(msec);
}

void GameWidget::newGeneration()  // Start the evolution of universe and update the game field
{
    if(generations < 0)
        generations++;

    if( universeMode == 7)
        ca1.WorldEvolutionSnake();
    else if( universeMode == 9 )
        ca1.WorldEvolutionMargolus();
    else
        ca1.WorldEvolutionLife(universeMode);

    update();

    if(ca1.IsNotChanged()) {
        QMessageBox::information(this,
                                 tr("Game lost sense"),
                                 tr("The End. Now game finished because all the next generations will be the same."),
                                 QMessageBox::Ok);
        stopGame();
        gameEnds(true);
        return;
    }

    generations--;
    if(generations == 0) {
        stopGame();
        gameEnds(true);
        QMessageBox::information(this,
                                 tr("Game finished."),
                                 tr("Iterations finished."),
                                 QMessageBox::Ok,
                                 QMessageBox::Cancel);
    }
}

void GameWidget::newGenerationColor() // Start the evolution of universe and update the game field for "Cyclic cellular automata" mode
{
    if(generations < 0)
        generations++;

    ca1.WorldEvolutionLifeColor();
    update();

//    if(ca1.IsNotChanged()) {
//        QMessageBox::information(this,
//                                 tr("Game lost sense"),
//                                 tr("The End. Now game finished because all the next generations will be the same."),
//                                 QMessageBox::Ok);
//        stopGame();
//        gameEnds(true);
//        return;
//    }

    generations--;
    if(generations == 0) {
        stopGame();
        gameEnds(true);
        QMessageBox::information(this,
                                 tr("Game finished."),
                                 tr("Iterations finished."),
                                 QMessageBox::Ok,
                                 QMessageBox::Cancel);
    }
}

void GameWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    paintGrid(p);
    paintUniverse(p);
}

void GameWidget::mousePressEvent(QMouseEvent *e)
{
    emit environmentChanged(true);
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    int k = floor(e->y()/cellHeight)+1;
    int j = floor(e->x()/cellWidth)+1;

    switch (cellMode) {
    case 0:
        switch(ca1.IsAlive(j,k)){
        case 0:
            ca1.SetAlive(j,k,1);
            break;
        case 1:
            ca1.SetAlive(j,k,0);
            break;
        }
        break;
    case 1:
        switch(ca1.IsAlive(j,k)){
        case 0:
            ca1.SetAlive(j,k,3);
            break;
        case 3:
            ca1.SetAlive(j,k,0);
            break;
        }
        break;
    case 2:
        switch(ca1.IsAlive(j,k)){
        case 0:
            ca1.SetAlive(j,k,6);
            break;
        case 6:
            ca1.SetAlive(j,k,0);
            break;
        }
        break;
    case 3:
        switch(ca1.IsAlive(j,k)){
        case 0:
            ca1.SetAlive(j,k,4);
            break;
        case 4:
            ca1.SetAlive(j,k,0);
            break;
        }
        break;
    case 4:
        switch(ca1.IsAlive(j,k)){
        case 0:
            ca1.SetAlive(j,k,2);
            break;
        case 2:
            ca1.SetAlive(j,k,0);
            break;
        }
        break;
    case 5:
        switch(ca1.IsAlive(j,k)){
        case 0:
            ca1.SetAlive(j,k,8);
            break;
        case 8:
            ca1.SetAlive(j,k,0);
            break;
        }
        break;
    }

    update();
}

void GameWidget::mouseMoveEvent(QMouseEvent *e)
{
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    int k = floor(e->y()/cellHeight)+1;
    int j = floor(e->x()/cellWidth)+1;
    if(ca1.IsAlive(j,k) == 0){
        switch (cellMode) {
        case 0:
            ca1.SetAlive(j,k,1);
            update();
            break;
        case 1:
            ca1.SetAlive(j,k,3);
            update();
            break;
        case 2:
            ca1.SetAlive(j,k,6);
            update();
            break;
        case 3:
            ca1.SetAlive(j,k,4);
            update();
            break;
        case 4:
            ca1.SetAlive(j,k,2);
            update();
            break;
        case 5:
            ca1.SetAlive(j,k,8);
            update();
            break;
        }
    }
}

void GameWidget::paintGrid(QPainter &p)
{
    QRect borders(0, 0, width()-1, height()-1); // borders of the universe
    QColor gridColor = m_masterColor; // color of the grid
    gridColor.setAlpha(10); // must be lighter than main color
    p.setPen(gridColor);
    double cellWidth = (double)width()/universeSize; // width of the widget / number of cells at one row
    for(double k = cellWidth; k <= width(); k += cellWidth)
        p.drawLine(k, 0, k, height());
    double cellHeight = (double)height()/universeSize; // height of the widget / number of cells at one row
    for(double k = cellHeight; k <= height(); k += cellHeight)
        p.drawLine(0, k, width(), k);
    p.drawRect(borders);
}

void GameWidget::paintUniverse(QPainter &p)
{
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    for(int k=1; k <= universeSize; k++) {
        for(int j=1; j <= universeSize; j++) {
            if(ca1.IsAlive(j, k)!=0){
                qreal left = (qreal)(cellWidth*j-cellWidth); // margin from left
                qreal top  = (qreal)(cellHeight*k-cellHeight); // margin from top
                QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);
                if(ca1.GetColor(j,k) != 12)
                    p.fillRect(r, SetColor(ca1.GetColor(j,k))); //fill cell with brush from random mode
                else{
                    if(ca1.IsAlive(j, k) == 1)
                        p.fillRect(r, QBrush(m_masterColor)); // fill cell with brush of main color
                    else
                        p.fillRect(r, /*QBrush(m_masterColor)*/SetColor(ca1.IsAlive(j, k))); //fill cell with brush of cell type
                }
            }
        }
    }
}

QColor GameWidget::masterColor()
{
    return m_masterColor;
}

QColor GameWidget::SetColor(const int &color)
{
    QColor cellColor;

    switch (color) {
    case 0:
        cellColor = Qt::red;
        break;
    case 1:
        cellColor = Qt::darkRed;
        break;
    case 2:
        cellColor = Qt::green;
        break;
    case 3:
        cellColor = Qt::darkGreen;
        break;
    case 4:
        cellColor = Qt::blue;
        break;
    case 5:
        cellColor = Qt::darkBlue;
        break;
    case 6:
        cellColor = Qt::cyan;
        break;
    case 7:
        cellColor = Qt::darkCyan;
        break;
    case 8:
        cellColor = Qt::magenta;
        break;
    case 9:
        cellColor = Qt::darkMagenta;
        break;
    case 10:
        cellColor = Qt::yellow;
        break;
    case 11:
        cellColor = Qt::darkYellow;
        break;
    }
    return cellColor;
}

void GameWidget::setMasterColor(const QColor &color)
{
    m_masterColor = color;
    update();
}

void GameWidget::keyPressEvent(QKeyEvent *event) {
 int key=event->key();
 if (key==Qt::Key_Up ) {
     ca1.TurnSnake(8);
     ca1.WorldEvolutionSnake();
     update();
 }
 if (key==Qt::Key_Down ) {
     ca1.TurnSnake(2);
     ca1.WorldEvolutionSnake();
     update();
 }
 if (key==Qt::Key_Right ) {
     ca1.TurnSnake(4);
     ca1.WorldEvolutionSnake();
     update();
 }
 if (key==Qt::Key_Left ) {
     ca1.TurnSnake(6);
     ca1.WorldEvolutionSnake();
     update();
 }
}

void GameWidget::focusOnKeyboard()
{
    setFocusPolicy(Qt::ClickFocus);
}

