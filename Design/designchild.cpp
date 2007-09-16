/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>
#include <fstream>

#include "designchild.h"

DesignChild::DesignChild() 
{
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
	
	// something to see...
	//foil.SetNaca009();
	resize(1000,600);
	//foilPath=new FoilPath(foil,0.,1000.,1000.,QPoint(width()*0.1, height() / 2),QRect(0,0,width(),height()));
}

DesignChild::~DesignChild()
{
}


void DesignChild::newFile()
{
/*    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));*/
}

bool DesignChild::loadFile(const QString &fileName)
{
	std::ifstream file(fileName.toStdString().c_str());
	
	QApplication::setOverrideCursor(Qt::WaitCursor);
 	bool retVal=foil.Read(file);
	QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    return true;
}

bool DesignChild::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool DesignChild::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool DesignChild::saveFile(const QString &fileName)
{
/*    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);*/
    return true;
}

QString DesignChild::userFriendlyCurrentFile()
{
    return /*foil.GetFoilName()+*/" direct design";
}

void DesignChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}


void DesignChild::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	
	painter.drawPath(FoilPath(foil,0.,1000.,1000.,QPoint(width()*0.1, height() / 2),QRect(0,0,width(),height())));
	painter.drawPath(FoilPointPath(foil,0.,1000.,1000.,QPoint(width()*0.1, height() / 2),QRect(0,0,width(),height())));
	//painter.drawPath(FoilMidLinePath(foil,0.,1000.,1000.,QPoint(width()*0.1, height() / 2),QRect(0,0,width(),height())));
	//foil.DrawFoil(painter,0.0,1000.,1000.,QPoint(width()*0.1, height() / 2),QRect(0,0,width(),height()),false);
// 	foil.DrawMidLine(painter,1000.,1000.,QPoint(width()*0.1, height() / 2),QRect(0,0,width(),height()),false);
}



void DesignChild::documentWasModified()
{
//     setWindowModified(document()->isModified());
}

bool DesignChild::maybeSave()
{
/*    if (document()->isModified()) {
	QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MDI"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
		     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }*/
    return true;
}

void DesignChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString DesignChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
