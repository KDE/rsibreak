/*
    SPDX-FileCopyrightText: 2009-2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BREAKCONTROL_H
#define BREAKCONTROL_H

#include <QWidget>

class QLabel;
class QPushButton;
class QVBoxLayout;

class BreakControl : public QWidget
{
    Q_OBJECT

public:
    BreakControl( QWidget*, Qt::WindowType );
    void setText( const QString& );
    void showMinimize( bool show );
    void showLock( bool show );
    void showPostpone(bool arg1);

protected:
    void paintEvent( QPaintEvent *event ) override;

private slots:
    void slotCenterIt();
    void slotLock();

signals:
    void skip();
    void lock();
    void postpone();

private:
    QLabel* m_textLabel;
    QPushButton* m_skipButton;
    QPushButton* m_lockButton;
    QVBoxLayout* m_vbox;
    QPushButton* m_postponeButton;
};

#endif // BREAKCONTROL_H
