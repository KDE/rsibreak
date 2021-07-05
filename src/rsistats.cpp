/*
    SPDX-FileCopyrightText: 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "rsistats.h"
#include "rsistatitem.h"

#include <QDateTime>
#include <QLocale>

#include <KLocalizedString>

RSIStats::RSIStats()
        : m_doUpdates( false )
{
    m_statistics.insert( TOTAL_TIME,
                         new RSIStatItem( i18n( "Total recorded time" ) ) );
    m_statistics[TOTAL_TIME]->addDerivedItem( ACTIVITY_PERC );

    m_statistics.insert( ACTIVITY,
                         new RSIStatItem( i18n( "Total time of activity" ) ) );
    m_statistics[ACTIVITY]->addDerivedItem( ACTIVITY_PERC );
    m_statistics[ACTIVITY]->addDerivedItem( ACTIVITY_PERC_MINUTE );
    m_statistics[ACTIVITY]->addDerivedItem( ACTIVITY_PERC_HOUR );
    m_statistics[ACTIVITY]->addDerivedItem( ACTIVITY_PERC_6HOUR );

    m_statistics.insert( IDLENESS,
                         new RSIStatItem( i18n( "Total time being idle" ) ) );
    m_statistics[IDLENESS]->addDerivedItem( ACTIVITY_PERC_MINUTE );
    m_statistics[IDLENESS]->addDerivedItem( ACTIVITY_PERC_HOUR );
    m_statistics[IDLENESS]->addDerivedItem( ACTIVITY_PERC_6HOUR );

    m_statistics.insert( ACTIVITY_PERC,
                         new RSIStatItem( i18n( "Percentage of activity" ), 0 ) );

    m_statistics.insert( ACTIVITY_PERC_MINUTE,
                         new RSIStatBitArrayItem( i18n( "Percentage of activity last minute" ),
                                                  QVariant( 0 ), 60 ) );
    m_statistics.insert( ACTIVITY_PERC_HOUR,
                         new RSIStatBitArrayItem( i18n( "Percentage of activity last hour" ),
                                                  QVariant( 0 ), 3600 ) );
    m_statistics.insert( ACTIVITY_PERC_6HOUR,
                         new RSIStatBitArrayItem( i18n( "Percentage of activity last 6 hours" ),
                                                  QVariant( 0 ), 6 * 3600 ) );

    m_statistics.insert( MAX_IDLENESS,
                         new RSIStatItem( i18n( "Maximum idle period" ) ) );
    m_statistics[MAX_IDLENESS]->addDerivedItem( IDLENESS );

    m_statistics.insert( CURRENT_IDLE_TIME,
                         new RSIStatItem( i18n( "Current idle period" ) ) );

    m_statistics.insert( IDLENESS_CAUSED_SKIP_TINY,
                         new RSIStatItem( i18n( "Number of skipped short breaks (idle)" ) ) );

    m_statistics.insert( IDLENESS_CAUSED_SKIP_BIG,
                         new RSIStatItem( i18n( "Number of skipped long breaks (idle)" ) ) );

    m_statistics.insert( TINY_BREAKS,
                         new RSIStatItem( i18n( "Total number of short breaks" ) ) );
    m_statistics[TINY_BREAKS]->addDerivedItem( PAUSE_SCORE );
    m_statistics[TINY_BREAKS]->addDerivedItem( LAST_TINY_BREAK );

    m_statistics.insert( TINY_BREAKS_SKIPPED,
                         new RSIStatItem( i18n( "Number of skipped short breaks (user)" ) ) );
    m_statistics[TINY_BREAKS_SKIPPED]->addDerivedItem( PAUSE_SCORE );

    m_statistics.insert( TINY_BREAKS_POSTPONED,
                         new RSIStatItem( i18n( "Number of postponed short breaks (user)" ) ) );

    QDateTime dt( QDate( -1, -1, -1 ), QTime( -1, -1, -1 ) );
    m_statistics.insert( LAST_TINY_BREAK,
                         new RSIStatItem( i18n( "Last short break" ), dt ) );

    m_statistics.insert( BIG_BREAKS,
                         new RSIStatItem( i18n( "Total number of long breaks" ) ) );
    m_statistics[BIG_BREAKS]->addDerivedItem( PAUSE_SCORE );
    m_statistics[BIG_BREAKS]->addDerivedItem( LAST_BIG_BREAK );

    m_statistics.insert( BIG_BREAKS_SKIPPED,
                         new RSIStatItem( i18n( "Number of skipped long breaks (user)" ) ) );
    m_statistics[BIG_BREAKS_SKIPPED]->addDerivedItem( PAUSE_SCORE );

    m_statistics.insert( BIG_BREAKS_POSTPONED,
                         new RSIStatItem( i18n( "Number of postponed long breaks (user)" ) ) );

    m_statistics.insert( LAST_BIG_BREAK,
                         new RSIStatItem( i18n( "Last long break" ), dt ) );

    m_statistics.insert( PAUSE_SCORE, new RSIStatItem( i18n( "Pause score" ), 100 ) );

    // initialise labels
    for ( int i = 0; i < STAT_COUNT; ++i ) {
        QLabel *l = new QLabel( 0 );
        const QString whatsThis = getWhatsThisText( static_cast<RSIStat>(i) );
        l->setWhatsThis( whatsThis );
        m_statistics[i]->getDescription()->setWhatsThis( whatsThis );
        m_labels << l;
    }

    // initialise statistics
    reset();
}

RSIStats::~RSIStats()
{
    qDeleteAll(m_labels);

    for ( int i = 0; i < STAT_COUNT; ++i ) {
        RSIStatItem *item = m_statistics[i];
        QLabel *l = item->getDescription();
        delete l;
        delete item;
    }
}

void RSIStats::reset()
{
    for ( int i = 0; i < STAT_COUNT; ++i ) {
        m_statistics[ i ]->reset();
        updateStat( static_cast<RSIStat>(i), /* update derived stats */ false );
    }
}

void RSIStats::increaseStat( RSIStat stat, int delta )
{
    QVariant v = m_statistics[stat]->getValue();

    if ( v.type() == QVariant::Int )
        m_statistics[stat]->setValue( v.toInt() + delta );
    else if ( v.type() == QVariant::Double )
        m_statistics[stat]->setValue( v.toDouble() + ( double )delta );
    else if ( v.type() == QVariant::DateTime )
        m_statistics[stat]->setValue(
            QDateTime( v.toDateTime() ).addSecs( delta ) );

    updateStat( stat );
}

void RSIStats::setStat( RSIStat stat, const QVariant &val, bool ifmax )
{
    QVariant v = m_statistics[stat]->getValue();

    if ( !ifmax ||
            ( v.type() == QVariant::Int && val.toInt() > v.toInt() ) ||
            ( v.type() == QVariant::Double && val.toDouble() > v.toDouble() ) ||
            ( v.type() == QVariant::DateTime && val.toDateTime() > v.toDateTime() ) )
        m_statistics[stat]->setValue( val );

    // WATCH OUT: IDLENESS is derived from MAX_IDLENESS and needs to be
    // updated regardless if a new value is set.
    updateStat( stat );
}

void RSIStats::updateDependentStats( RSIStat stat )
{
    const QList<RSIStat> &stats = m_statistics[ stat ]->getDerivedItems();
    for ( int i = 0 ; i < stats.count(); ++i ) {
        RSIStat it = stats.at( i );
        switch (( it ) ) {
        case PAUSE_SCORE: {
            double a = m_statistics[ TINY_BREAKS_SKIPPED ]->getValue().toDouble();
            double b = m_statistics[ BIG_BREAKS_SKIPPED ]->getValue().toDouble();
            double c = m_statistics[ IDLENESS_CAUSED_SKIP_TINY ]->getValue().toDouble();
            double d = m_statistics[ IDLENESS_CAUSED_SKIP_BIG ]->getValue().toDouble();

            RSIGlobals *glbl = RSIGlobals::instance();
            double ratio = ( double )( glbl->intervals()[BIG_BREAK_DURATION] ) /
                           ( double )( glbl->intervals()[TINY_BREAK_DURATION] );

            double skipped = a - c + ratio * ( b - d );
            skipped = skipped < 0 ? 0 : skipped;

            double total = m_statistics[ TINY_BREAKS ]->getValue().toDouble();
            total += ratio * m_statistics[ BIG_BREAKS ]->getValue().toDouble();

            if ( total > 0 )
                m_statistics[ it ]->setValue( 100 - (( skipped / total ) * 100 ) );
            else
                m_statistics[ it ]->setValue( 0 );

            updateStat( it );
            break;
        }

        case IDLENESS: {
            increaseStat( IDLENESS );
            break;
        }

        case ACTIVITY_PERC: {
            /*
                                            seconds of activity
                activity_percentage =  100 - -------------------
                                                total seconds
            */

            double activity = m_statistics[ ACTIVITY ]->getValue().toDouble();
            double total = m_statistics[TOTAL_TIME]->getValue().toDouble();

            if ( total > 0 )
                m_statistics[ it ]->setValue(( activity / total ) * 100 );
            else
                m_statistics[ it ]->setValue( 0 );

            updateStat( it );
            break;
        }

        case ACTIVITY_PERC_MINUTE:
        case ACTIVITY_PERC_HOUR:
        case ACTIVITY_PERC_6HOUR: {
            if ( stat == ACTIVITY )
                static_cast<RSIStatBitArrayItem *>( m_statistics[it] )->setActivity();
            else
                static_cast<RSIStatBitArrayItem *>( m_statistics[it] )->setIdle();

            updateStat( it );
            break;
        }

        case LAST_BIG_BREAK: {
            setStat( LAST_BIG_BREAK, QDateTime::currentDateTime() );
            break;
        }

        case LAST_TINY_BREAK: {
            setStat( LAST_TINY_BREAK, QDateTime::currentDateTime() );
            break;
        }

        default:
            ;// nada
        }
    }
}

void RSIStats::updateStat( RSIStat stat, bool updateDerived )
{
    if ( updateDerived )
        updateDependentStats( stat );

    if ( m_doUpdates )
        updateLabel( stat );
}

void RSIStats::updateLabel( RSIStat stat )
{
    QLabel *l = m_labels[ stat ];
    double v;

    switch ( stat ) {
        // integer values representing a time
    case TOTAL_TIME:
    case ACTIVITY:
    case IDLENESS:
    case MAX_IDLENESS:
    case CURRENT_IDLE_TIME:
        l->setText( RSIGlobals::instance()->formatSeconds(
                        m_statistics[ stat ]->getValue().toInt() ) );
        break;

        // plain integer values
    case TINY_BREAKS:
    case TINY_BREAKS_SKIPPED:
    case TINY_BREAKS_POSTPONED:
    case IDLENESS_CAUSED_SKIP_TINY:
    case BIG_BREAKS:
    case BIG_BREAKS_SKIPPED:
    case BIG_BREAKS_POSTPONED:
    case IDLENESS_CAUSED_SKIP_BIG:
        l->setText( QString::number(
                        m_statistics[ stat ]->getValue().toInt() ) );
        break;

        // doubles
    case PAUSE_SCORE:
        v = m_statistics[ stat ]->getValue().toDouble();
        setColor( stat, QColor(( int )( 255 - 2.55 * v ), ( int )( 1.60 * v ), 0 ) );
        l->setText( QString::number(
                        m_statistics[ stat ]->getValue().toDouble(), 'f', 1 ) );
        break;
    case ACTIVITY_PERC:
    case ACTIVITY_PERC_MINUTE:
    case ACTIVITY_PERC_HOUR:
    case ACTIVITY_PERC_6HOUR:
        v = m_statistics[stat]->getValue().toDouble();
        setColor( stat, QColor(( int )( 2.55 * v ), ( int )( 160 - 1.60 * v ), 0 ) );
        l->setText( QString::number(
                        m_statistics[ stat ]->getValue().toDouble(), 'f', 1 ) );
        break;

        // datetimes
    case LAST_BIG_BREAK:
    case LAST_TINY_BREAK: {
        QTime when( m_statistics[ stat ]->getValue().toTime() );
        when.isValid() ? l->setText( when.toString() )
        : l->clear();
        break;
    }

    default:
        ; // nada
    }

    // some stats need a %
    if ( stat == PAUSE_SCORE || stat == ACTIVITY_PERC || stat == ACTIVITY_PERC_MINUTE ||
            stat == ACTIVITY_PERC_HOUR || stat == ACTIVITY_PERC_6HOUR )
        l->setText( l->text() + '%' );
}

void RSIStats::updateLabels()
{
    if ( !m_doUpdates )
        return;

    for ( int i = 0; i < STAT_COUNT; ++i ) {
        updateLabel( static_cast<RSIStat>(i) );
    }
}

QVariant RSIStats::getStat( RSIStat stat ) const
{
    return m_statistics[ stat ]->getValue();
}

QLabel *RSIStats::getLabel( RSIStat stat ) const
{
    return m_labels[ stat ];
}

QLabel *RSIStats::getDescription( RSIStat stat ) const
{
    return m_statistics[stat]->getDescription();
}

QString RSIStats::getWhatsThisText( RSIStat stat ) const
{
    switch ( stat ) {
    case TOTAL_TIME:
        return i18n( "This is the total time RSIBreak has been running." );
    case ACTIVITY:
        return i18n( "This is the total amount of time you used the "
                     "keyboard or mouse." );
    case IDLENESS:
        return i18n( "This is the total amount of time you did not use "
                     "the keyboard or mouse." );
    case ACTIVITY_PERC:
        return i18n( "This is a percentage of activity, based on the "
                     "periods of activity vs. the total time RSIBreak has been running. "
                     "The color indicates the level of your activity. When the color is "
                     "close to full red it is recommended you lower your work pace." );
    case MAX_IDLENESS:
        return i18n( "This is the longest period of inactivity measured "
                     "while RSIBreak has been running." );
    case TINY_BREAKS:
        return i18n( "This is the total number of short breaks" );
    case LAST_TINY_BREAK:
        return i18n( "This is the time of the last finished short break. "
                     "The color of this text gradually turns from green to red, "
                     "indicating when you can expect the next tiny break." );
    case TINY_BREAKS_SKIPPED:
        return i18n( "This is the total number of short breaks "
                     "which you skipped." );
    case TINY_BREAKS_POSTPONED:
        return i18n( "This is the total number of short breaks "
                     "which you postponed." );
    case IDLENESS_CAUSED_SKIP_TINY:
        return i18n( "This is the total number of short breaks "
                     "which were skipped because you were idle." );
    case BIG_BREAKS:
        return i18n( "This is the total number of long breaks." );
    case LAST_BIG_BREAK:
        return i18n( "This is the time of the last finished long break."
                     "The color of this text gradually turns from green to red,"
                     "indicating when you can expect the next big break." );
    case BIG_BREAKS_SKIPPED:
        return i18n( "This is the total number of long breaks "
                     "which you skipped." );
    case BIG_BREAKS_POSTPONED:
        return i18n( "This is the total number of long breaks "
                     "which you postponed." );
    case IDLENESS_CAUSED_SKIP_BIG:
        return i18n( "This is the total number of long breaks "
                     "which were skipped because you were idle." );
    case PAUSE_SCORE:
        return i18n( "This is an indication of how well you behaved "
                     "with the breaks. It decreases every time you skip a break." );
    case CURRENT_IDLE_TIME:
        return i18n( "This is the current idle time." );
    case ACTIVITY_PERC_MINUTE:
        return i18n( "This is a percentage of activity during the last minute. "
                     "The color indicates the level of your activity. When the color is "
                     "close to full red it is recommended you lower your work pace." );
    case ACTIVITY_PERC_HOUR:
        return i18n( "This is a percentage of activity during the last hour. "
                     "The color indicates the level of your activity. When the color is "
                     "close to full red it is recommended you lower your work pace." );
    case ACTIVITY_PERC_6HOUR:
        return i18n( "This is a percentage of activity during the last 6 hours. "
                     "The color indicates the level of your activity. When the color is "
                     "close to full red it is recommended you lower your work pace." );
    default:
        ;
    }

    return QString();
}

void RSIStats::setColor( RSIStat stat, const QColor &color )
{
    QPalette normal;
    normal.setColor( QPalette::Active, QPalette::WindowText, color );
    m_statistics[ stat ]->getDescription()->setPalette( normal );
    m_labels[ stat ]->setPalette( normal );
}

void RSIStats::doUpdates( bool b )
{
    m_doUpdates = b;
    if ( m_doUpdates )
        updateLabels();
}
