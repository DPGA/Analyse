#ifndef HISTOGRAM_H
#define HISTOGRAM_H

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
#include <stdlib.h>

class Histogram: public QwtPlotHistogram
{
public:
    Histogram( const QString &, const QColor & );

    void setColor( const QColor & );
    void setValues( uint numValues, const double * );
    void setValues(QVector<double> *values );
    double GetMin() {return min;};
    double GetMax() {return max;};
    double GetMean() {return mean;}; 
    double GetStdDev() {return stdev;};  
    
private:
		double min,max,mean,stdev;
};

#endif
