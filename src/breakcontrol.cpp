/*
    SPDX-FileCopyrightText: 2009-2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "breakcontrol.h"

#include <QApplication>
#include <QIcon>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScreen>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <QHBoxLayout>

BreakControl::BreakControl(QWidget *parent, Qt::WindowType type)
    : QWidget(parent, type)
{
    m_vbox = new QVBoxLayout;
    m_textLabel = new QLabel(this);
    m_textLabel->setAlignment(Qt::AlignHCenter);

    QWidget *hbox = new QWidget(this);
    QHBoxLayout *hboxHBoxLayout = new QHBoxLayout(hbox);
    hboxHBoxLayout->setContentsMargins(0, 0, 0, 0);
    hboxHBoxLayout->setSpacing(25);

    m_skipButton = new QPushButton(i18n("Skip Break"), hbox);
    hboxHBoxLayout->addWidget(m_skipButton);
    QSize sizeSkip(m_skipButton->size());
    m_skipButton->setIcon(QIcon::fromTheme("dialog-cancel"));
    m_skipButton->setFixedHeight(sizeSkip.height());
    connect(m_skipButton, &QPushButton::clicked, this, &BreakControl::skip);

    m_postponeButton = new QPushButton(i18n("Postpone Break"), hbox);
    hboxHBoxLayout->addWidget(m_postponeButton);
    QSize sizePostpone(m_postponeButton->size());
    m_postponeButton->setIcon(QIcon::fromTheme("go-next"));
    m_postponeButton->setFixedHeight(sizePostpone.height());
    connect(m_postponeButton, &QPushButton::clicked, this, &BreakControl::postpone);

    m_lockButton = new QPushButton(i18n("Lock Screen"), hbox);
    hboxHBoxLayout->addWidget(m_lockButton);
    QSize sizeLock(m_skipButton->size());
    m_lockButton->setFixedHeight(sizeLock.height());
    m_lockButton->setIcon(QIcon::fromTheme("system-lock-screen"));
    connect(m_lockButton, &QPushButton::clicked, this, &BreakControl::slotLock);

    m_vbox->addWidget(m_textLabel);
    m_vbox->addWidget(hbox);

    setLayout(m_vbox);

    connect(qApp, &QGuiApplication::screenAdded, this, &BreakControl::slotCenterIt);
    connect(qApp, &QGuiApplication::screenRemoved, this, &BreakControl::slotCenterIt);

    slotCenterIt();
}

void BreakControl::slotCenterIt()
{
    const QRect r(QGuiApplication::primaryScreen()->geometry());

    const QPoint center(r.width() / 2 - sizeHint().width() / 2, r.y());
    move(center);
}

void BreakControl::slotLock()
{
    emit lock();
}

void BreakControl::setText(const QString &text)
{
    m_textLabel->setText(text);
}

void BreakControl::showMinimize(bool show)
{
    m_skipButton->setVisible(show);
}

void BreakControl::showLock(bool show)
{
    m_lockButton->setVisible(show);
}

void BreakControl::showPostpone(bool show)
{
    m_postponeButton->setVisible(show);
}

void BreakControl::paintEvent(QPaintEvent *event)
{
    if (event->type() == QEvent::Paint) {
        int margin = 3;
        QPainterPath box;
        box.moveTo(rect().topLeft());
        box.lineTo(rect().bottomLeft());
        box.lineTo(rect().bottomRight());
        box.lineTo(rect().topRight());
        box.closeSubpath();

        QColor highlight = palette().highlight().color();
        highlight.setAlphaF(0.7);

        QPen pen(highlight);
        pen.setWidth(margin);

        QPainter painter(this);
        painter.setPen(pen);
        painter.drawPath(box);
    }
}
