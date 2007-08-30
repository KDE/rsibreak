/*
   Copyright (C) 2007 Tom Albers <tomalbers@kde.nl>

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

#ifndef RSIBREAK_H
#define RSIBREAK_H

#include <plasma/applet.h>
#include <QTimer>

namespace Plasma { class Svg; class Label; }

class RSIBreak : public Plasma::Applet
{
    Q_OBJECT
public:
    RSIBreak( QObject *parent, const QVariantList &args );
    ~RSIBreak();

    QSizeF contentSize() const;
    void paintInterface( QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         const QRect &contentsRect );
private slots:
    void slotUpdate();

private:
    QTimer*   m_timer;
    QString   m_text;
    QString   m_icon;
    QSize     m_size;
    Plasma::Svg* m_theme;
    Plasma::Label* m_h1;
    Plasma::Label* m_h2;
    Plasma::Label* m_h3;
    Plasma::Label* m_t1;
    Plasma::Label* m_t2;
    Plasma::Label* m_t3;
    int	      m_heightPerLine;
};

K_EXPORT_PLASMA_APPLET( rsibreak, RSIBreak )

#endif
