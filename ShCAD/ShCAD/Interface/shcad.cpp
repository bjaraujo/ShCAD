
/*--
**
**   This file is part of the ShCAD project, a 2D CAD Program
**
**    Copyright (C) 2019, Seungho Ha  (sh.ha1018@gmail.com)
**
**
**   This program is free software; you can redistribute it and/or modify it
**   under the terms of the GNU Lesser General Public License as published by
**   the Free Software Foundation; either version 2.1 of the License, or (at
**   your option) any later version.
**
**   This program is distributed in the hope that it will be useful, but
**   WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
**   General Public License for more details.
**
**   You should have received a copy of the GNU Lesser General Public License
**   along with this program; if not, write to the Free Software Foundation,
**   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
**
**
--*/

#include "shcad.h"
#include "ShMenuBar.h"
#include <qmdiarea.h>
#include <qdockwidget.h>
#include <qtoolbar.h>
#include "ShRibbonMenu.h"
#include "ShStatusBar.h"
#include "Dock\ShCommandDock.h"
#include "Singleton Pattern\ShChangeManager.h"
ShCAD::ShCAD(QWidget *parent)
	: QMainWindow(parent){

	
	this->menuBar = new ShMenuBar(this);
	this->setMenuBar(this->menuBar);
	
	this->mdiArea = new QMdiArea;
	this->setCentralWidget(this->mdiArea);
	this->mdiArea->setDocumentMode(true);

	this->ribbon = new ShRibbonMenu(150, this);
	this->ribbon->setWindowTitle("RibbonBar");
	this->addToolBar(this->ribbon);

	this->statusBar = new ShStatusBar(this);
	this->setStatusBar(this->statusBar);

	this->commandDock = new ShCommandDock(this);
	this->commandDock->setWindowTitle("Command");
	this->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, this->commandDock);


	ShChangeManager *manager = ShChangeManager::GetInstance();
	manager->Register(this->statusBar);
	manager->Register(this->commandDock);



	this->dock = new QDockWidget(this);
	this->dock->installEventFilter(this);
	this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, this->dock);
	

	QToolBar *temp2 = new QToolBar(this);
	this->addToolBarBreak();
	this->addToolBar(temp2);

	
}

ShCAD::~ShCAD(){
	

	
}

#include "ShGraphicView.h"
#include "Singleton Pattern\ShWidgetManager.h"
void ShCAD::NewActionClicked() {
	
	
	
	ShGraphicView *newWidget = new ShGraphicView(this->mdiArea);
	newWidget->setMinimumSize(400, 400);

	this->mdiArea->addSubWindow(newWidget,Qt::WindowFlags::enum_type::SubWindow);
	newWidget->show();
	
	ShWidgetManager::GetInstance()->SetActivatedWidget(newWidget);
	
}





#include <QEvent>
#include <QResizeEvent>
bool ShCAD::eventFilter(QObject *obj, QEvent *event) {
	
	if (event->type() == QEvent::Resize && obj == this->dock) {
		QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
		qDebug("Dock Resized (New Size) - Width: %d Height: %d",
			resizeEvent->size().width(),
			resizeEvent->size().height());
		
		
		this->resizeDocks({ this->dock }, { resizeEvent->size().width() }, 
			Qt::Orientation::Horizontal);

		this->resizeDocks({ this->dock }, { resizeEvent->size().height() }, Qt::Orientation::Vertical);
	}
	
	return QWidget::eventFilter(obj, event);
}

/*
void ShCAD::ShowContextMenu(const QPoint &pos) {

	QMenu contextMenu(tr("Context menu"), this);

	QAction action1("Remove Data Point", this);
	connect(&action1, SIGNAL(triggered()), this, SLOT(TestCustomContextMenu()));
	contextMenu.addAction(&action1);

	contextMenu.exec(mapToGlobal(pos));

}

#include <qmessagebox.h>
void ShCAD::TestCustomContextMenu() {
	QMessageBox box;
	box.exec();

}
*/
