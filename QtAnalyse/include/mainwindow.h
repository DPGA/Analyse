#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qcustomplot.h>
//#include "myplots.h"
#include "myplotsqwt.h"
#include "decodeframe.h"
#include <QProgressBar>

const int MAX_ASM = 16;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public DecodeFrame
{
    Q_OBJECT
    QWidget* m_mainWindow;

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private slots :
	void ReadFile_lib(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd);
	void CloseTab(int index);
	void DisplayChannel(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd);
    void DisplayallChannel(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd);
	void Handlebutton();
	void on_actionOpen_triggered();
//	void on_actionChannel_triggered();


	void on_actionTimestamp_triggered();

	void on_actionOpen_Multi_file_triggered();

    void on_actionOne_Channel_triggered();

    void on_actionAll_triggered();

    void on_actionAll_Files_and_Channels_triggered();
    
    void on_actionCounter_and_trigger_triggered();

private:
	void ReadallChannel(int16_t FileIndex,int16_t FeId,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd);
	void FileInfo(QString FileName,QTextEdit *fileDesc,MyPlotsQwt *Plots);
	void DisplayTimeStamp(MyPlotsQwt *Plots);
	void MeanStDev(QVector<double> Y,QVector<double> *Mean,QVector<double> *Stdev);
    void DisplayallHeader(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd);
    QGroupBox *TaballHeader(int16_t FileIndex,int16_t FeId,int16_t Cell,int64_t NbEvts);
    void ReadallHeader(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd);
	QGroupBox *TabChannel(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvts);
	QGroupBox *TaballChannel(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvts);	  
	Ui::MainWindow *ui;

   QTextEdit *TextEdit[MAX_ASM];
 //  MyPlots *PlotTstp[MAX_ASM];
	MyPlotsQwt *PlotsQwtTstp[MAX_ASM];
	MyPlotsQwt *PlotChannel;
	MyPlotsQwt *PlotSroutCh;
	MyPlotsQwt *PlotMeanCh;
	MyPlotsQwt *PlotStdDevCh;
	MyPlotsQwt *PlotCellCh[5];
	MyPlotsQwt *PlotCellMean;
	//MyPlotsQwt *PlotCh[24];
	QVector <MyPlotsQwt *> PlotCh;
	
	QLCDNumber  *Num [6*MAX_ASM];
	double CptTrame = 0;
	QTabWidget *onglets;
	QString Filename;
	QString dirPath;
	uint64_t NbRecords;
	QStringList _fnames;
	QPushButton *showCh;
	
};

#endif // MAINWINDOW_H
