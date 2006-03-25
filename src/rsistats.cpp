/* This file is part of the KDE project
   Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <qgrid.h>
#include <qlabel.h>

#include <kdebug.h>
#include <klocale.h>

#include "rsistats.h"
#include "rsiglobals.h"

class RSIStatItem
{
  public:
    RSIStatItem( const QString &description )
    {
      m_description = description;
      m_value = QVariant(0);
    }
    RSIStatItem() : m_value( QVariant(0) ) {}
    ~RSIStatItem() {}

    QString getDescription() const { return m_description; }

    QVariant getValue() const { return m_value; }
    void setValue( QVariant v ) { m_value = v; }

    void addDerivedItem( RSIStat stat ) { m_derived += stat; }
    QValueList<RSIStat> getDerivedItems() const { return m_derived; }

  private:
    QVariant m_value;
    QString m_description;

    /** Contains a list of RSIStats which depend on *this* item. */
    QValueList< RSIStat > m_derived;
};

RSIStats *RSIStats::m_instance = 0;

RSIStats::RSIStats()
{
    kdDebug() << "RSIStats::RSIStats() entered" << endl;

    m_statistics[TOTAL_TIME] = RSIStatItem(i18n("Total recorded time"));
    m_labels[TOTAL_TIME] = new QLabel(0);

    m_statistics[ACTIVITY] = RSIStatItem(i18n("Total time of activity"));
    m_labels[ACTIVITY] = new QLabel(0);

    m_statistics[IDLENESS] = RSIStatItem(i18n("Total time being idle"));
    m_labels[IDLENESS] = new QLabel(0);

    m_statistics[TINY_BREAKS] = RSIStatItem(i18n("Total amount of tiny breaks"));
    m_statistics[TINY_BREAKS].addDerivedItem( PAUSE_SCORE );
    m_labels[TINY_BREAKS] = new QLabel(0);

    m_statistics[TINY_BREAKS_SKIPPED] = RSIStatItem(i18n("Number of skipped tiny breaks"));
    m_statistics[TINY_BREAKS_SKIPPED].addDerivedItem( PAUSE_SCORE );
    m_labels[TINY_BREAKS_SKIPPED] = new QLabel(0);

    m_statistics[BIG_BREAKS] = RSIStatItem(i18n("Total amount of big breaks"));
    m_statistics[BIG_BREAKS].addDerivedItem( PAUSE_SCORE );
    m_labels[BIG_BREAKS] = new QLabel(0);

    m_statistics[BIG_BREAKS_SKIPPED] = RSIStatItem(i18n("Number of skipped big breaks"));
    m_statistics[BIG_BREAKS_SKIPPED].addDerivedItem( PAUSE_SCORE );
    m_labels[BIG_BREAKS_SKIPPED] = new QLabel(0);

    // FIXME: Find better name
    m_statistics[PAUSE_SCORE] = RSIStatItem(i18n("Pause score"));
    m_labels[PAUSE_SCORE] = new QLabel(0);

    updateLabels();
    reset();
}

RSIStats::~RSIStats()
{
    kdDebug() << "RSIStats::~RSIStats() entered" << endl;

    QMap<RSIStat,QLabel *>::Iterator it;
    for ( it = m_labels.begin() ; it != m_labels.end(); ++it )
    {
      delete it.data();
      it.data() = 0;
    }

    delete m_instance;
}

RSIStats *RSIStats::instance()
{
  if ( !m_instance )
  {
      m_instance = new RSIStats();
  }

  return m_instance;
}

void RSIStats::reset()
{
    kdDebug() << "RSIStats::reset() entered" << endl;

    QMap<RSIStat,RSIStatItem>::ConstIterator it;
    for( it = m_statistics.begin(); it != m_statistics.end(); ++it )
      m_statistics[ it.key() ].setValue( 0 );
}


void RSIStats::increaseStat( RSIStat stat, int delta )
{
    // kdDebug() << "RSIStats::increaseStat() entered" << endl;

    QVariant v = m_statistics[stat].getValue();

    if ( v.type() == QVariant::Int )
      m_statistics[stat].setValue( v.toInt() + delta );
    else
      m_statistics[stat].setValue( v.toDouble() + (double)delta );

    updateStat( stat );
}

void RSIStats::updateDependentStats( RSIStat stat )
{
    // kdDebug() << "RSIStats::updateDependentStats" << endl;

    QValueList<RSIStat> stats = m_statistics[ stat ].getDerivedItems();
    QValueList<RSIStat>::ConstIterator it;
    for( it = stats.begin() ; it != stats.end(); ++it )
    {
      switch( *it )
      {
        case PAUSE_SCORE:
        {
          /*
                            tiny_skip + 2*big_skip
            score = 100 - -------------------------- * 100
                          tiny_breaks + 2*big_breaks
          */

          double skipped = m_statistics[ TINY_BREAKS_SKIPPED ].getValue().toDouble() + 2 * m_statistics[ BIG_BREAKS_SKIPPED ].getValue().toDouble();
          double total = m_statistics[ TINY_BREAKS ].getValue().toDouble() + 2 * m_statistics[ BIG_BREAKS ].getValue().toDouble();

          if ( total > 0 )
            m_statistics[ *it ].setValue( 100 - ( ( skipped / total ) * 100 ) );
          else
            m_statistics[ *it ].setValue( 0 );

          updateStat( *it );
          break;
        }
        default: ;// nada
      }
    }
}

void RSIStats::updateStat( RSIStat stat )
{
    // kdDebug() << "RSIStats::updateStat() entered" << endl;

    updateDependentStats( stat );
    updateLabel( stat );
}

void RSIStats::updateLabel( RSIStat stat )
{
    // kdDebug() << "RSIStats::updateLabel() entered" << endl;

    QLabel *l = m_labels[ stat ];

    switch ( stat )
    {
      // integer values representing a time
      case TOTAL_TIME:
      case ACTIVITY:
      case IDLENESS:
         l->setText( RSIGlobals::formatSeconds( m_statistics[ stat ].getValue().toInt() ) );
         break;

      // plain integer values
      case TINY_BREAKS:
      case TINY_BREAKS_SKIPPED:
      case BIG_BREAKS:
      case BIG_BREAKS_SKIPPED:
        l->setText( QString::number( m_statistics[ stat ].getValue().toInt() ) );
        break;

      // doubles
      case PAUSE_SCORE:
        l->setText( QString::number( m_statistics[ stat ].getValue().toDouble() ) );
        break;
      default: ; // nada
    }

    // some stats need a %
    if ( stat == PAUSE_SCORE )
      l->setText( l->text() + "%" );
}

void RSIStats::updateLabels()
{
    kdDebug() << "RSIStats::updateLabels()" << endl;

    QMapConstIterator<RSIStat,RSIStatItem> it;
    for ( it = m_statistics.begin() ; it != m_statistics.end() ; ++it )
      updateLabel( it.key() );
}

QVariant RSIStats::getStat( RSIStat stat ) const
{
    kdDebug() << "RSIStats::getStat() entered" << endl;

    return m_statistics[ stat ].getValue();
}

QLabel *RSIStats::getLabel( RSIStat stat ) const
{
    kdDebug() << "RSIStats::getFormattedStat() entered" << endl;

    return m_labels[ stat ];
}

QString RSIStats::getDescription( RSIStat stat ) const
{
    kdDebug() << "RSIStats::getDescription() entered" << endl;

    return m_statistics[stat].getDescription();
}
