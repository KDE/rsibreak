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

#include <qvbox.h>

#include <klocale.h>

#include "rsistatdialog.h"

RSIStatDialog::RSIStatDialog( QWidget *parent, const char *name )
  : KDialogBase( parent, name, false, i18n("Usage Statistics"), KDialogBase::Ok, KDialogBase::Ok, true )
{
  QVBox *layout = new QVBox( this );
  setMainWidget( layout );
}

RSIStatDialog::~RSIStatDialog()
{

}

void RSIStatDialog::updateStatistics()
{

}

#include "rsistatdialog.moc"
