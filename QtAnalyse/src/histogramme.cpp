
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
#include <histogramme.h>

Histogram::Histogram( const QString &title, const QColor &symbolColor ):
    QwtPlotHistogram( title )
{
    setStyle( QwtPlotHistogram::Columns );

    setColor( symbolColor );
}

void Histogram::setColor( const QColor &color )
{
    QColor c = color;
    c.setAlpha( 180 );
    setBrush( QBrush( c ) );
}

void Histogram::setValues( uint numValues, const double *values )
{
    QVector<QwtIntervalSample> samples( numValues );
    for ( uint i = 0; i < numValues; i++ )
    {
        QwtInterval interval( double( i ), i + 1.0 );
        interval.setBorderFlags( QwtInterval::ExcludeMaximum );

        samples[i] = QwtIntervalSample( values[i], interval );
    }

    setData( new QwtIntervalSeriesData( samples ) );
}

void Histogram::setValues(QVector<double> *values ) 
{
    min = *std::min_element(values->constBegin(), values->constEnd());
    max = *std::max_element(values->constBegin(), values->constEnd());
    mean = std::accumulate(std::begin(*values), std::end(*values), 0)/static_cast<double>(values->size());
    
    double sq_sum = std::inner_product(values->begin(), values->end(), values->begin(), 0.0);
    stdev = std::sqrt(sq_sum / values->size() - mean * mean);

    QVector<double> HistoY(max+1,0.0);
    QVector<double> HistoX;


    for (const auto& item : *values) {
        HistoY[item]++;
    }	
    
    QVector<QwtIntervalSample> samples( HistoY.size() );
     
    for ( uint i = 0; i < HistoY.size(); i++ )
    {
        QwtInterval interval( double( i -0.25), i + 0.25 );
        interval.setBorderFlags( QwtInterval::ExcludeMaximum );

        samples[i] = QwtIntervalSample( HistoY.at(i), interval );
    }

    setData( new QwtIntervalSeriesData( samples ) );
}
