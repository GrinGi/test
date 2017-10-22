#include <QTextStream>
#include <QFileDialog>
#include <QDebug>
#include <QColor>
#include <QColorDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentColor(QColor("#000")),
    game(new GameWidget(this))
{
    ui->setupUi(this);                              //Fill in the drop-down list with the types of evolution
    ui->modeControl->addItem("Classic Life");
    ui->modeControl->addItem("Unlimited Life");
    ui->modeControl->addItem("Limited Life");
    ui->modeControl->addItem("Fractal Life");
    ui->modeControl->addItem("Random");
    ui->modeControl->addItem("Erosion");
    ui->modeControl->addItem("Moving");
    ui->modeControl->addItem("Snake");
    ui->modeControl->addItem("Two Liquids");
    ui->modeControl->addItem("Diffusion Margolus");

                                                    //Fill in the drop-down list with the types of cells
    ui->cellsMode->addItem("Classic");
    ui->cellsMode->addItem("Dead");
    ui->cellsMode->addItem("Live Right");
    ui->cellsMode->addItem("Live Left");
    ui->cellsMode->addItem("Live Up");
    ui->cellsMode->addItem("Live Down");

    QPixmap icon(16, 16);
    icon.fill(currentColor);
    ui->colorButton->setIcon( QIcon(icon) );

    connect(ui->startButton, SIGNAL(clicked()), game,SLOT(startGame()));                    // Start Game with press the button "Start"
    connect(ui->stopButton, SIGNAL(clicked()), game,SLOT(stopGame()));                      // Stop Game with press the button "Stop"
    connect(ui->clearButton, SIGNAL(clicked()), game,SLOT(clear()));                        // Clear Game field with press the button "Clear"
    connect(ui->iterInterval, SIGNAL(valueChanged(int)), game, SLOT(setInterval(int)));     // Changing the interval between iteration with press the button "Generation interval"
    connect(ui->cellsControl, SIGNAL(valueChanged(int)), game, SLOT(setCellNumber(int)));   // Change the Universe Size  with press the button "Universe Size"
    connect(ui->modeControl, SIGNAL(currentIndexChanged(int)), game, SLOT(setMode(int)));   // Change the Universe Mode  with press the button "Universe Mode"
    connect(ui->cellsMode, SIGNAL(currentIndexChanged(int)), game, SLOT(setCellMode(int))); // Change the Cells Mode  with press the button "Cells Mode"
    connect(ui->modeControl, SIGNAL(activated(int)), game,SLOT(focusOnKeyboard()));         // Keyboard activation

    connect(game,SIGNAL(environmentChanged(bool)),ui->cellsControl,SLOT(setDisabled(bool))); // When one of the cell has been changed - lock the button "Universe Size"
    connect(game,SIGNAL(gameEnds(bool)),ui->cellsControl,SLOT(setEnabled(bool)));            // When game over - activate the button "Universe Size"
    connect(ui->colorButton, SIGNAL(clicked()), this, SLOT(selectMasterColor()));            // Color selection with the button "Select color for cells"
    connect(ui->randomButton, SIGNAL(clicked()), game, SLOT(fillRandom()));                  // Fill Game field with random color with press the button "Random Color"
    connect(ui->GoButton, SIGNAL(clicked()), game, SLOT(goRandom()));                        // Start Game in "Cyclic cellular automata" mode with press the button "Go"

    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveGame()));   //Save Game with press the button "Save"
    connect(ui->loadButton, SIGNAL(clicked()), this, SLOT(loadGame()));   // Load Game with press the button "Load"

    ui->mainLayout->setStretchFactor(ui->gameLayout, 8);
    ui->mainLayout->setStretchFactor(ui->setLayout, 2);
    ui->gameLayout->addWidget(game);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::saveGame()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save current game"),
                                                    QDir::homePath(),
                                                    tr("Conway's Game *.life Files (*.life)"));
    if(filename.length() < 1)
        return;
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    QString s = QString::number(game->cellNumber())+"\n";
    file.write(s.toUtf8());
    file.write(game->dump().toUtf8());
    QColor color = game->masterColor();
    QString buf = QString::number(color.red())+" "+
                  QString::number(color.green())+" "+
                  QString::number(color.blue())+"\n";
    file.write(buf.toUtf8());
    buf.clear();
    buf = QString::number(ui->iterInterval->value())+"\n";
    file.write(buf.toUtf8());
    file.close();
}

void MainWindow::loadGame()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open saved game"),
                                                    QDir::homePath(),
                                                    tr("Conway's Game Of Life File (*.life)"));
    if(filename.length() < 1)
        return;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return;
    QTextStream in(&file);

    int sv;
    in >> sv;
    ui->cellsControl->setValue(sv);

    game->setCellNumber(sv);
    QString dump="";
    for(int k=0; k != sv; k++) {
        QString t;
        in >> t;
        dump.append(t+"\n");
    }
    game->setDump(dump);

    int r,g,b; // RGB color
    in >> r >> g >> b;
    currentColor = QColor(r,g,b);
    game->setMasterColor(currentColor); // sets color of the dots
    QPixmap icon(16, 16); // icon on the button
    icon.fill(currentColor); // fill with new color
    ui->colorButton->setIcon( QIcon(icon) ); // set icon for button
    in >> r; // r will be interval number
    ui->iterInterval->setValue(r);
    game->setInterval(r);
}

void MainWindow::selectMasterColor()
{
    QColor color = QColorDialog::getColor(currentColor, this, tr("Select color of figures"));
    if(!color.isValid())
        return;
    currentColor = color;
    game->setMasterColor(color);
    QPixmap icon(16, 16);
    icon.fill(color);
    ui->colorButton->setIcon( QIcon(icon) );
}
