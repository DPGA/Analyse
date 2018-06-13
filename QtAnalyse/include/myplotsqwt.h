#ifndef MYPLOTSQWT_H
#define MYPLOTSQWT_H
#include <qcustomplot.h>
#include <arpa/inet.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_renderer.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_histogram.h>
#include <qwt_column_symbol.h>
#include <qwt_series_data.h>
#include <qpen.h>
#include "histogramme.h"
#include <qwt_plot_zoomer.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>


class MyWindow : public QMainWindow
{
    Q_OBJECT
public:
    MyWindow(){;};

signals:
	void close();
	
protected:
    void closeEvent(QCloseEvent *event) override;
    
};


class MyPlotsQwt :  public QwtPlot
{
    Q_OBJECT
    QWidget* m_mainWindow;

  //  QMainWindow *m_Mywindow;

public:

	explicit MyPlotsQwt(QString title,const bool zoom=false);
	~MyPlotsQwt();
	
	void setData(QVector<double> x,QVector<double>,const QColor c=Qt::red,const QwtPlotCurve::CurveStyle = QwtPlotCurve::Lines);
	void setHisto(QVector<double> *histo,const QColor c=Qt::red);
	void setHisto(Histogram *histo);
	void setCenterPlotMean();
	void replotzoom();
	void clearCurve();
	void addTitle(QString title);
	void setMarker(const QString s,const int size=10);
	
private slots:
   void ZoomGraph();
   void closeEvent(); 


protected :
	

private :
	bool eventFilter (QObject *object, QEvent *event);
	bool Zoom;
	QString Title;
	Histogram *h;
	Histogram *hzoom;
	QRubberBand *rubberBand;
	QPoint rubberOrigin;
	QVector<double> x;
	QVector<double> y;	
	MyWindow *WinZoom;
	QwtPlotZoomer* zoomer;
	MyPlotsQwt *PlotZoom;
	QwtPlotMarker *d_marker;
	bool CenterPlotMean;
	bool ZoomActived = false;
	bool FisrtDisplay= true;
	QVector<QwtPlotCurve *> curve;
	bool DisplayHisto;
};


#endif // MYPLOTS_H
