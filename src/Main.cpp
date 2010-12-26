/****************************************************************************

	QFLR5 Application

	Copyright (C) 2008-2010 Andre Deperrois XFLR5@yahoo.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/


#include <QtGui/QApplication>
#include "XFLR5Application.h"
#include "MainFrame.h"
#include <QSplashScreen>
#include <QMessageBox>

CFoil *g_pCurFoil;	//make it common to all applications

int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x040600
        QGL::setPreferredPaintEngine (QPaintEngine::OpenGL);
#endif
        XFLR5Application app(argc, argv);
		return app.exec();

}

