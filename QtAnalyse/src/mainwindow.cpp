#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <frame.h>
#include <arpa/inet.h>
//#include "qcustomplot.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
//#include "myplots.h"
#include <cmath>
#include "decodeframe.h"
#include "dialogchcell.h"
#include "histogramme.h"
#include "myplotsqwt.h"

QGroupBox *MainWindow::TaballHeader(int16_t FileIndex,int16_t FeId,int16_t Cell,int64_t NbEvts)
{
	QGroupBox *groupboxCh  = new QGroupBox("");
	QGridLayout* LayoutPageCh = new QGridLayout;
	QFont serifFont("Times", 6);
    for (int k=0;k<8;++k) {
		PlotCh << new MyPlotsQwt("Ch ",false);
		PlotCh.back()->setAxisFont(QwtPlot::xBottom,serifFont);
		PlotCh.back()->setAxisFont(QwtPlot::yLeft,serifFont);
		LayoutPageCh->addWidget(PlotCh.back(),k/4,k%4);
	}
	groupboxCh->setLayout(LayoutPageCh);
	ui->onglets->addTab(groupboxCh,"HeaderFeId 0x"+QString::number(FeId,16));
	return groupboxCh;
}


QGroupBox *MainWindow::TaballChannel(int16_t FileIndex,int16_t FeId,int16_t Cell,int64_t NbEvts)
{
	QGroupBox *groupboxCh  = new QGroupBox("");
	QGridLayout* LayoutPageCh = new QGridLayout;
	QFont serifFont("Times", 6);
	for (int k=0;k<24;++k) {
        PlotCh << new MyPlotsQwt("NbAsm histo ",false);
		PlotCh.back()->setAxisFont(QwtPlot::xBottom,serifFont);
		PlotCh.back()->setAxisFont(QwtPlot::yLeft,serifFont);
		LayoutPageCh->addWidget(PlotCh.back(),k/4,k%4);
	}
	groupboxCh->setLayout(LayoutPageCh);
	ui->onglets->addTab(groupboxCh,"FeId 0x"+QString::number(FeId,16));
	return groupboxCh;
}

QGroupBox *MainWindow::TabChannel(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvts)
{
	QGroupBox *groupbox  = new QGroupBox("");
	QFormLayout *FormLayout = new QFormLayout();
	
	QGroupBox *choicegroupbox  = new QGroupBox("");
	choicegroupbox->setMaximumWidth(400);
	
	// Choic Channel and cell 
	QGridLayout *GridLayout = new QGridLayout();
	GridLayout->addWidget(new QLabel("Channel"),0,0);
	QComboBox *Chcombobox  = new QComboBox();
	for (int i=0;i<24;i++) Chcombobox->addItem(QString::number(i,10));
	Chcombobox->setCurrentText(QString::number(Ch,10));
	GridLayout->addWidget(Chcombobox,0,1);
	GridLayout->addWidget(new QLabel("\tCell"),0,2);
	QComboBox *Cellcombobox  = new QComboBox();
	for (int i=2;i<1022;i++) Cellcombobox->addItem(QString::number(i,10));
	Cellcombobox->setCurrentText(QString::number(Cell,10));
	GridLayout->addWidget(Cellcombobox,0,3);
	showCh = new QPushButton("Show");
	GridLayout->addWidget(showCh,0,4);
	choicegroupbox->setLayout(GridLayout);
	FormLayout->addRow(choicegroupbox);
	
	QGridLayout* LayoutPage = new QGridLayout;
	PlotChannel = new MyPlotsQwt("Ch ",false);
	LayoutPage->addWidget(PlotChannel,0,0,1,0);
	PlotSroutCh = new MyPlotsQwt("Ch ",false);
	LayoutPage->addWidget(PlotSroutCh,1,0,1,0);
	PlotMeanCh = new MyPlotsQwt("Mean",false);
	LayoutPage->addWidget(PlotMeanCh,2,0,1,0);
	PlotStdDevCh = new MyPlotsQwt("StdDev",false);
	LayoutPage->addWidget(PlotStdDevCh,3,0);
	PlotCellMean = new MyPlotsQwt("CellMean",false);
	LayoutPage->addWidget(PlotCellMean,3,1);
	
	FormLayout->addRow(LayoutPage);
	
	QGridLayout* LayoutHPage4 = new QGridLayout;
	for (int i=0;i<5;i++) {
		PlotCellCh[i] = new MyPlotsQwt("Cell ",false);
		LayoutHPage4->addWidget(PlotCellCh[i],4,i);
	}
	FormLayout->addRow(LayoutHPage4);
	groupbox->setLayout(FormLayout);
	connect(showCh,SIGNAL(pressed()),this,SLOT(Handlebutton()));
	ui->onglets->addTab(groupbox,QString::number(FileIndex,10));
	return(groupbox);
}

void MainWindow::Handlebutton()
{
	qInfo() << "handle button";
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    DecodeFrame(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

// Tab 1 with header file info
    QGroupBox *groupbox1  = new QGroupBox("");
    QGridLayout* LayoutVerticalPage1 = new QGridLayout;
    for (int i=0;i<12;i++) {
       TextEdit[i] = new QTextEdit;
       LayoutVerticalPage1->addWidget(TextEdit[i],i/4,i%4);
     }
     groupbox1->setLayout(LayoutVerticalPage1);
     ui->onglets->addTab(groupbox1,"Header File");
     
// Tab 2 Timestamp analysis   
	QGroupBox *groupbox2  = new QGroupBox("");
	QGridLayout* LayoutVerticalPage2 = new QGridLayout;
	for (int i=0;i<12;i++) {
		PlotsQwtTstp[i] = new MyPlotsQwt("FeId 0x",false);
		LayoutVerticalPage2->addWidget(PlotsQwtTstp[i],i/4,i%4);
	}
	groupbox2->setLayout(LayoutVerticalPage2);
	ui->onglets->addTab(groupbox2,"TimeStamp");
	
// Tab 3 Display one channel 
//	QGroupBox *groupbox3 = TabChannel();
	
/*	QGroupBox *groupbox3  = new QGroupBox("");
	
	QFormLayout *FormLayout = new QFormLayout;
	QGridLayout* LayoutPage3 = new QGridLayout;
	PlotChannel = new MyPlotsQwt("Ch ",false);
	LayoutPage3->addWidget(PlotChannel,0,0,1,0);
	PlotMeanCh = new MyPlotsQwt("Mean",false);
	LayoutPage3->addWidget(PlotMeanCh,1,0,1,0);
	PlotStdDevCh = new MyPlotsQwt("StdDev",false);
	LayoutPage3->addWidget(PlotStdDevCh,2,0);
	PlotCellMean = new MyPlotsQwt("CellMean",false);
	LayoutPage3->addWidget(PlotCellMean,2,1);
	
	FormLayout->addRow(LayoutPage3);
	
	QGridLayout* LayoutHPage4 = new QGridLayout;
	for (int i=0;i<5;i++) {
		PlotCellCh[i] = new MyPlotsQwt("Cell ",false);
		LayoutHPage4->addWidget(PlotCellCh[i],4,i);
	}
	FormLayout->addRow(LayoutHPage4);
	groupbox3->setLayout(FormLayout);
	*/
//	ui->onglets->addTab(groupbox3,"Channel");
	ui->onglets->setTabsClosable(true);
	// Les 2 premiers onglets ne sont pas fermable
	QTabBar *tabBar = ui->onglets->findChild<QTabBar *>();
	tabBar->setTabButton(0, QTabBar::RightSide, 0);
	tabBar->setTabButton(0, QTabBar::LeftSide, 0);
	tabBar->setTabButton(1, QTabBar::RightSide, 0);
	tabBar->setTabButton(1, QTabBar::LeftSide, 0);
	
	connect(ui->onglets,SIGNAL(tabCloseRequested(int)),this,SLOT(CloseTab(int)));
}

void MainWindow::CloseTab(int index)
{
	ui->onglets->removeTab(index);
}


void MainWindow::DisplayallChannel(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd)
{
	bool Exist = false;
	if (!SetFile(&_fnames[FileIndex].toStdString()[0])) {
		qInfo() << "File not found : " << Filename; 
		return;
	}
	S_HeaderFile *HdrFile = GetHeaderFile();
	
	for (int i=2;i<ui->onglets->count();++i) {
		if (ui->onglets->tabText(i) == "FeId 0x"+QString::number(HdrFile->FrontEndId,16)) {
			Exist=true;
			ui->onglets->setCurrentIndex(i);
			break;
		}
	}
	if (!Exist) {
		TaballChannel(FileIndex,HdrFile->FrontEndId,Cell,NbEvtsBegin);
		ui->onglets->setCurrentIndex(ui->onglets->count());
	}
	ReadallChannel(FileIndex,Ch,Cell,NbEvtsBegin,NbEvtsEnd);
}


void MainWindow::DisplayallHeader(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd)
{
    bool Exist = false;
    if (!SetFile(&_fnames[FileIndex].toStdString()[0])) {
        qInfo() << "File not found : " << Filename;
        return;
    }
    S_HeaderFile *HdrFile = GetHeaderFile();

    for (int i=2;i<ui->onglets->count();++i) {
        if (ui->onglets->tabText(i) == "Header FeId 0x"+QString::number(HdrFile->FrontEndId,16)) {
            Exist=true;
            ui->onglets->setCurrentIndex(i);
            break;
        }
    }
    if (!Exist) {
        TaballHeader(FileIndex,HdrFile->FrontEndId,Cell,NbEvtsBegin);
        ui->onglets->setCurrentIndex(ui->onglets->count());
    }
    ReadallHeader(FileIndex,Ch,Cell,NbEvtsBegin,NbEvtsEnd);
}

void MainWindow::DisplayChannel(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd)
{
	qInfo() <<"Displaychannel";
	bool Exist = false;
	for (int i=2;i<ui->onglets->count();++i) {
		if (ui->onglets->tabText(i) == QString::number(FileIndex,10)) {
			Exist=true;
			ui->onglets->setCurrentIndex(i);
			break;
		}
	}
		
	if (!Exist) {
		TabChannel(FileIndex,Ch,Cell,NbEvtsBegin);
		ui->onglets->setCurrentIndex(ui->onglets->count());
	}
	if ((Ch >=0) && (Ch<24))
		ReadFile_lib(FileIndex,Ch,Cell,NbEvtsBegin,NbEvtsEnd);
}


void MainWindow::ReadallHeader(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd)
{
    S_HeaderFile *HdrFile = GetHeaderFile();

    QFileInfo Infofile(_fnames[FileIndex]);
    dirPath = Infofile.filePath(); // Path vers le fichier

    qint64 SizeFile = Infofile.size();
    unsigned long NbRecs = SizeFile/(sizeof(S_HeaderFrame)+(HdrFile->NbSamples*2+4)*4+4);

    uint16_t NbSamplesHdrFile = HdrFile->NbSamples;
//	ui->onglets->setTabText(2, "FeId 0x"+QString::number(HdrFile->FrontEndId,16));
//    for (int i=0;i<NbSamplesHdrFile;i++) NoCell << i;

    QProgressDialog progress("Task in progress...\n" + _fnames[FileIndex], "Cancel", 0, NbEvtsEnd - NbEvtsBegin, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMaximum(NbRecs);
    int index=0;
    uint16_t *p;
//	uint16_t NbPLotCh = PlotCh.size();
//	for (int i=0;i<24;i++)	PlotCh[NbPLotCh-24+i]->clearCurve();
    QVector <double> HistoChannel;


    QList <unsigned long long> NbFrameAsm;
    QList <unsigned long long> NbFrameAmc;
    QList <unsigned long long> tstpAsm;
    do {
        progress.setValue(index++);
        p = NextPacket();
        int FeID = GetFeId() - 0x10;

        unsigned short nbSamples = GetNbSamples();
        NbFrameAsm << GetNbFrameAsm();
        NbFrameAmc << GetNbFrameAmc();
        tstpAsm << GetTimeStpAsm();

        if ((FeID >= 0) && (FeID <= 0xf)) {
            for (int j=0;j<4;++j) {
                uint16_t *pCh = GetChannel(j);
                unsigned short Channel = GetCh();
                HistoChannel.push_back(Channel);
                unsigned short Srout = GetSrout();

                if (nbSamples != NbSamplesHdrFile) {
                    qInfo() << "Error Nb Samples, Header_file = " << NbSamplesHdrFile << " Header frame = " << nbSamples;
                    break;
                }
                if ((Channel < 0) && (Channel >23)){
                    qInfo() << "Error Channel = " << hex << Channel;
                    break;
                }
               int IndexSrout=Srout;

            }
        }
        else qInfo() << "Error FrontEnd Id = " <<  FeID;
    } while (p!=NULL);
//    for (int i=0;i<24;i++)	PlotCh[i]->replot();
    qInfo() << "End File";

    qSort(NbFrameAsm.begin(),NbFrameAsm.end(),qLess<unsigned long long>());
    qSort(NbFrameAmc.begin(),NbFrameAmc.end(),qLess<unsigned long long>());
    qSort(tstpAsm.begin(),tstpAsm.end(),qLess<unsigned long long>());

    QList<unsigned long long>::const_iterator iter;
    QList<unsigned long long> lostFrameAsm;
    QVector<double> diff;
    QList<unsigned long long> temps;
    QVector<double> difftemps;
    QVector<double> x;
    bool first = true;
    unsigned long long loop = 0;
    unsigned long long boucle = 0;
    for (iter = NbFrameAsm.begin(); iter != NbFrameAsm.end()-1;iter++) {
       if  (((*iter)+1) != *(iter+1)) {
           lostFrameAsm << (*iter)+1;
           temps << tstpAsm.at(boucle);
           x.push_back(boucle);
       }
       boucle++;
    }

    for (iter = lostFrameAsm.begin(); iter != lostFrameAsm.end()-1;iter++) {
        diff.push_back((*(iter+1)) - (*iter));
    }

    for (iter = temps.begin(); iter != temps.end()-1;iter++) {
        difftemps.push_back((*(iter+1)) - (*iter));
    }

    PlotCh.at(0)->setHisto(&diff);
    PlotCh.at(0)->replot();
    PlotCh.at(1)->setHisto(&HistoChannel);
    PlotCh.at(1)->replot();
    for (const auto &i:diff) qDebug() << "diff " << i;




}

void MainWindow::ReadallChannel(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd)
{
	QVector<double> NoCell;
    QVector<double> Amp;
	QVector<double> ySrout(1024,0);
	
	S_HeaderFile *HdrFile = GetHeaderFile();
	
	QFileInfo Infofile(_fnames[FileIndex]);
	dirPath = Infofile.filePath(); // Path vers le fichier
	
	qint64 SizeFile = Infofile.size();
	unsigned long NbRecs = SizeFile/(sizeof(S_HeaderFrame)+(HdrFile->NbSamples*2+4)*4+4);
	
	uint16_t NbSamplesHdrFile = HdrFile->NbSamples;
//	ui->onglets->setTabText(2, "FeId 0x"+QString::number(HdrFile->FrontEndId,16));
	for (int i=0;i<NbSamplesHdrFile;i++) NoCell << i;
	
	QProgressDialog progress("Task in progress...\n" + _fnames[FileIndex], "Cancel", 0, NbEvtsEnd - NbEvtsBegin, this);
   progress.setWindowModality(Qt::WindowModal);
   progress.setMaximum(NbRecs);
	int index=0;
	uint16_t *p;
	uint16_t NbPLotCh = PlotCh.size();
	for (int i=0;i<24;i++)	PlotCh[NbPLotCh-24+i]->clearCurve();
	do {
		progress.setValue(index++);
		p = NextPacket();
		int FeID = GetFeId() - 0x10;

		unsigned short nbSamples = GetNbSamples();
		
		if ((FeID >= 0) && (FeID <= 0xf)) {
			for (int j=0;j<4;++j) {
				uint16_t *pCh = GetChannel(j);
            unsigned short Channel = GetCh();
				unsigned short Srout = GetSrout();

				if (nbSamples != NbSamplesHdrFile) {
					qInfo() << "Error Nb Samples, Header_file = " << NbSamplesHdrFile << " Header frame = " << nbSamples;
					break;
				}
				if ((Channel < 0) && (Channel >23)){
					qInfo() << "Error Channel = " << hex << Channel;
					break;
				}
			   int IndexSrout=Srout;
				for(int samples=0;samples<nbSamples;samples++) {
					unsigned int d = ntohs(pCh[samples+2]) & 0xFFF;
					Amp << (double) (d);
					ySrout[IndexSrout] = (double) d;
					IndexSrout = (IndexSrout+1) % 1024;
				}

				PlotCh[NbPLotCh-24+Channel]->setData(NoCell,ySrout,Qt::green);
//				PlotCh[NbPLotCh-24+Channel]->setData(NoCell,Amp,Qt::green);

				Amp.clear();
			}
		}
		else qInfo() << "Error FrontEnd Id = " <<  FeID;     
	} while (p!=NULL);

	for (int i=0;i<24;i++)	PlotCh[i]->replot();
	qInfo() << "End File";
	
}

void MainWindow::ReadFile_lib(int16_t FileIndex,int16_t Ch,int16_t Cell,int64_t NbEvtsBegin,int64_t NbEvtsEnd)
{
	uint32_t events=0;
	qInfo() << "Channel " << QString::number(Ch,10) << "  Cell " << QString::number(Cell,10);
	if ((Ch == -1) && (Cell == -1)) return;
	if ((Ch <0) && ( Ch>23)) return;
	if ((Cell<0) && (Cell> 1023)) return;
   
	QVector<double> DataCell[5];
	QVector<double> x;
	QVector<double> y;
	QVector<double> yCell,xCell;
	QVector<double> HistoCell,HistoX;
	QVector<double> HistoCellm2,HistoCellm1,HistoCellp1,HistoCellp2;
	QVector<double> yrescale(1024,0);
	QVector<double> xSrout;
	QVector<double> HistoSrout;//(1024,0);
	QVector<QVector<double>> lSamples;
	QVector<double> Mean,Stddev;
	
	for (int k=0;k<1024;k++) xSrout.push_back((double) k);
   QProgressDialog progress("Task in progress...", "Cancel", 0, NbEvtsEnd - NbEvtsBegin, this);
   progress.setWindowModality(Qt::WindowModal);
	
	for (int loop=0;loop < 4096;loop++) {
		HistoCell.push_back(0.0);
		HistoCellm2.push_back(0.0);
		HistoCellm1.push_back(0.0);
		HistoCellp1.push_back(0.0);
		HistoCellp2.push_back(0.0);
		HistoX.push_back(loop);
	}

	x.reserve(1024);
	y.reserve(1024); 
	
    if (!SetFile(&_fnames[FileIndex].toStdString()[0])) {
		qInfo() << "File not found : " << Filename; 
		return;
	}
	qInfo() << "file name " << _fnames[0];
	
	PlotChannel->clearCurve();
	PlotMeanCh->clearCurve();
	PlotStdDevCh->clearCurve();
	PlotSroutCh->clearCurve();
	for (int i=0;i<5;i++) {
		PlotCellCh[i]->clearCurve();
		PlotCellCh[i]->setCenterPlotMean();
	}
	PlotCellMean->clearCurve();
	
   PlotChannel->setAxisScale( QwtPlot::xBottom,0,1023 );
   PlotSroutCh->setAxisScale( QwtPlot::xBottom,0,1023 );
   PlotMeanCh->setAxisScale( QwtPlot::xBottom,0,1023 );
   PlotStdDevCh->setAxisScale( QwtPlot::xBottom,0,1023 );
	PlotCellMean->setAxisScale( QwtPlot::xBottom,Cell-3,Cell+3);
	
	PlotChannel->addTitle("Ch " + QString::number(Ch,10));
	PlotSroutCh->addTitle("Srout Ch " + QString::number(Ch,10));
	
	for (int evt=0;evt<NbEvtsBegin;evt++) NextPacket();
	for(int evt=NbEvtsBegin;evt<NbEvtsEnd;evt++) {
		progress.setValue(evt);
		NextPacket();
		int FeID = GetFeId() - 0x10;
		unsigned long  trigthor = GetCptTriggerThor();
		QString str = QString::number(trigthor,10);
//		qInfo() << "trig= " << trigthor << " " << hex << GetCptTriggerThor() << dec;
		CptTrame++;
		unsigned short nbSamples = GetNbSamples();
	//	qInfo() << "Frond Id = " << hex << FeID + 0x10 << endl;
	//	qInfo() << "NbSamples = " << nbSamples << " len = " << (nbSamples+2)*2<< endl;
		if ((FeID >= 0) && (FeID <= 0xf)) {
			for (int j=0;j<4;++j) {
				uint16_t *pCh = GetChannel(j);
            unsigned short Channel = GetCh();
//				qInfo() 	<< "ch = " << Channel << "Sample 0 " << hex << ntohs(pCh[2]);

				unsigned short Srout = GetSrout();
				
				
				
            if (Ch == Channel) {
					events++;
					qInfo() << "srout " << Srout;
					//HistoSrout[Srout]++;
					HistoSrout.push_back(Srout);
					x.clear();
					y.clear();
					unsigned short SroutLoop = Srout;
                            //for (int i=0;i<1024;i++) x.push_back((double) i);
					for(int samples=0;samples<nbSamples;samples++) {
//						if (nbSamples == 0x400) x.push_back((double) samples);
//						else x.push_back((double) SroutLoop);
						x.push_back((double) samples);
//						xSrout[samples] = (double) SroutLoop;
						unsigned int d = ntohs(pCh[samples+2]) & 0xFFF;
						y.push_back((double) (d));
						yrescale[SroutLoop] = d;

						if (nbSamples == 0x400) {
							if (samples == Cell-2) DataCell[0].push_back(d);
							if (samples == Cell-1) DataCell[1].push_back(d);
							if (samples == Cell+1) DataCell[3].push_back(d);
							if (samples == Cell+2) DataCell[4].push_back(d);
							if (samples == Cell) {	
								DataCell[2].push_back(d);
								yCell.push_back((double) (d));
								xCell.push_back((double) (samples));
								HistoCell[d]++;
							}
						}
						else {
							if (SroutLoop == Cell-2) DataCell[0].push_back(d);
							if (SroutLoop == Cell-1) DataCell[1].push_back(d);
							if (SroutLoop == Cell+1) DataCell[3].push_back(d);
							if (SroutLoop == Cell+2) DataCell[4].push_back(d);
							if (SroutLoop == Cell) { 
								DataCell[2].push_back(d);
								yCell.push_back((double) (d));
								xCell.push_back((double) (SroutLoop));//samples
								HistoCell[d]++;
							}
						}
						SroutLoop = (SroutLoop+1) % 1024;
					}

					PlotChannel->setData(x,y,Qt::green);
					PlotSroutCh->setData(xSrout,yrescale,Qt::green);
					if (nbSamples == 0x400) {
						lSamples.push_back(y);
					}
					else {
						lSamples.push_back(yrescale);
					}

				}
			}

		}
       
	}

   PlotChannel->replot();
   PlotSroutCh->replot();
	qInfo() << "nb event " << events;

	QVector<double> tempC(lSamples[0].size());
	QVector<double> tempR(lSamples.size());

	QVector<QVector<double>> CellSamples(lSamples[0].size(),QVector<double>(lSamples.size()));
	
	for (int loop=0;loop<lSamples.size();loop++) {
		QVector<double> Evt = lSamples.at(loop);

		for (int samples=0;samples < Evt.size();samples++) {
			CellSamples[samples][loop] = Evt[samples];
		}
	}

	QVector<double> meanX;
	for (int loop=0;loop<CellSamples.size();loop++) {
        MeanStDev(CellSamples.at(loop),&Mean,&Stddev);
//        qInfo() << loop << "  : mean = " << Mean.at(loop) << "stddev " << Stddev.at(loop);
        meanX.push_back(loop);
    }
    
	PlotMeanCh->setData(meanX,Mean,Qt::blue);
	PlotMeanCh->replot();

	PlotStdDevCh->setData(meanX,Stddev);
	PlotStdDevCh->replot();

	for (int i=0;i<4;i++) {
		PlotCellCh[i]->addTitle("Cell " + QString::number(Cell-2+i,10));
		PlotCellCh[i]->setHisto(&DataCell[i],Qt::yellow);
		PlotCellCh[i]->setMarker("Mean",8);
	}
	
	QVector<double> cella;
	for (int k=0;k<1024;k++) cella.push_back(k);
	PlotCellCh[4]->addTitle("SrOut");
	PlotCellCh[4]->setHisto(&HistoSrout,Qt::yellow);
	//PlotCellCh[4]->setData(cella,HistoSrout,Qt::red);
		
	PlotCellMean->setData(xCell,yCell,Qt::red,QwtPlotCurve::Dots);
	PlotCellMean->replot();

	qInfo() << " Fin ............. "<< endl;
	for (int i=0;i<1024;i++) qInfo() << i << "  " << HistoSrout[i];
	
}
void MainWindow::MeanStDev(QVector<double> Y,QVector<double> *Mean,QVector<double> *Stdev)
{
    double mean = std::accumulate(std::begin(Y), std::end(Y), 0)/static_cast<double>(Y.size());
    double sq_sum = std::inner_product(Y.begin(), Y.end(), Y.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / Y.size() - mean * mean);

    Stdev->push_back(stdev);
    Mean->push_back(mean);
}


MainWindow::~MainWindow()
{
    //for (int i=0;i<MAX_ASM;i++) if (TextEdit[i] !=NULL) delete TextEdit[i];
    delete ui;
}

void MainWindow::FileInfo(QString FileName,QTextEdit *fileDesc,MyPlotsQwt *Plots)
{
	QStringList ModeStr = (QStringList() << "HEADER" << "RAWDATA" << "ALL");
	
	QFileInfo Infofile(FileName);
	dirPath = Infofile.filePath(); // Path vers le fichier
	
	qint64 SizeFile = Infofile.size();
	qInfo() << "Size File " << SizeFile;
   
	SetFile(dirPath.toStdString().c_str());
	S_HeaderFile *HdrFile = GetHeaderFile();
	
	fileDesc->clear();
	fileDesc->append("<b>" + Infofile.fileName() +"</b>" );
	fileDesc->append("Mode File \t\t " + ModeStr.at(HdrFile->ModeFile));
	fileDesc->append("Frontend Id \t 0x" + QString::number(HdrFile->FrontEndId,16));
	fileDesc->append("Nb Samples \t " + QString::number(HdrFile->NbSamples,10));
	unsigned long NbRecs = SizeFile/(sizeof(S_HeaderFrame)+(HdrFile->NbSamples*2+4)*4+4);
	fileDesc->append("Nb Rec (1/2 Drs)\t " + QString::number(NbRecs,10));
	//Le calcul de perte de trame ne marche qu'en mode piedestaux ?? todo
   //fileDesc->append("Frame(s) Lost \t " + QString::number(GetCptTriggerThor()-(GetNbFrameAsm()/6),10));
	
	ExtractHeader();
    
	QVector<uint64_t> vector = QVector<uint64_t>::fromStdVector(GetAllTstpAsm());
    
	S_ErrorFrame ErrFrame = GetErrFrame();
	fileDesc->append("________________________________");
	fileDesc->append("Nb Err Start Of Frame	" + QString::number(ErrFrame.ErrSoF,10));
	fileDesc->append("Nb Err CafeDeca 		" + QString::number(ErrFrame.ErrCafeDeca,10));
	fileDesc->append("Nb Err bobo		" + QString::number(ErrFrame.ErrBobo,10));
	fileDesc->append("Nb Err Soc			" + QString::number(ErrFrame.ErrSoc,10));
	fileDesc->append("Nb Err Crc			" + QString::number(ErrFrame.ErrCrc,10));
	fileDesc->append("Nb Err EoF			" + QString::number(ErrFrame.ErrEoF,10));
	fileDesc->append("Nb Err TT			" + QString::number(ErrFrame.ErrTT,10));
	Plots->addTitle("FeId " + QString::number(HdrFile->FrontEndId,16));
//	DisplayTimeStamp(Plots);


}


void MainWindow::DisplayTimeStamp(MyPlotsQwt *Plots)
{
	QVector<uint64_t> TimeStampAsm      = QVector<uint64_t>::fromStdVector(GetAllTstpAsm());
	QVector<uint64_t> TimeStampThor     = QVector<uint64_t>::fromStdVector(GetAllTstpThor());
	QVector<uint64_t> TimeStampThorAsm  = QVector<uint64_t>::fromStdVector(GetAllTstpThorAsm());
	QVector<bool> 	  ErrorTT           = QVector<bool>::fromStdVector(GetAllTTError());

	QVector<double> x;
	QVector<double> ydiff;
	Plots->clearCurve();
	
	for (int j=0;j<TimeStampThor.size();j++) {
		int temp = TimeStampAsm.at(j) - TimeStampThor.at(j);
		if (j<=30) 
        qInfo() << "T=" << (unsigned long) TimeStampThor.at(j)  << " A=" << TimeStampAsm.at(j) << " " << temp;
		x.push_back(j);
		ydiff << (double) abs(temp);
	}
	
	Plots->setHisto(&ydiff);
	Plots->setMarker("Mean");
}

void MainWindow::on_actionOpen_triggered()
{
    QStringList ModeStr = (QStringList() << "HEADER" << "RAWDATA" << "ALL");
    QString file = QFileDialog::getOpenFileName(this, tr("Ouvrir Raw Data ..."),"/MyFile",tr(" (*.bin)"));
    _fnames.clear();
    _fnames.push_back(file);
    if(file.isEmpty()) {
        // Erreur ...
        return;
    }
}


void MainWindow::on_actionTimestamp_triggered()
{

}

void MainWindow::on_actionOpen_Multi_file_triggered()
{
    QFileDialog w;
    w.setNameFilters(QStringList() << "*.bin" << "*.err");
    w.setFileMode(QFileDialog::AnyFile);
    w.setOption(QFileDialog::DontUseNativeDialog,true);
    w.setViewMode(QFileDialog::Detail);
    
    QListView *l = w.findChild<QListView*>("listView");
    if (l) {
      l->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    QTreeView *t = w.findChild<QTreeView*>();
    if (t) {
      t->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    w.exec();
    _fnames = w.selectedFiles();
    if (_fnames.size() >12) {
        QMessageBox msgBox;
        msgBox.setText("The Number of file do not execed 12.");
        msgBox.exec();
        return;
    }
    
// if (PlotChannel) PlotChannel->clearCurve();
//	PlotMeanCh->clearCurve();
//	PlotStdDevCh->clearCurve();

    for (int i=0;i< _fnames.size();i++) {
      FileInfo(_fnames.at(i),TextEdit[i],PlotsQwtTstp[i]);
	}
}

void MainWindow::on_actionOne_Channel_triggered()
{
    DialogChCell dcc(m_mainWindow);
    dcc.setFile(_fnames);
    dcc.setModal(true);
    connect(&dcc, SIGNAL(dataAvailable(int16_t,int16_t,int16_t,int64_t,int64_t)), this, SLOT(DisplayChannel(int16_t,int16_t,int16_t,int64_t,int64_t)));
    dcc.exec();
}

void MainWindow::on_actionAll_triggered()
{
    DialogChCell dcc(m_mainWindow,false);
    dcc.setFile(_fnames);
    dcc.setModal(true);
    connect(&dcc, SIGNAL(dataAvailable(int16_t,int16_t,int16_t,int64_t,int64_t)), this, SLOT(DisplayallChannel(int16_t,int16_t,int16_t,int64_t,int64_t)));
    dcc.exec();
}

void MainWindow::on_actionAll_Files_and_Channels_triggered()
{
    for (int i=0;i< _fnames.size();i++) {
        DisplayallChannel(i,0,0,0,0);

    }    
}

void MainWindow::on_actionCounter_and_trigger_triggered()
{
    for (int i=0;i< _fnames.size();i++) {
//        DisplayallChannel(i,0,0,0,0);
          DisplayallHeader(i,0,0,0,0);
    }
}
