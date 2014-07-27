// Object Viewer Qt - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
// Copyright (C) 2010  Winch Gate Property Limited
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef TILE_EDITOR_MAIN_WINDOW_H
#define TILE_EDITOR_MAIN_WINDOW_H

#include "ui_tile_editor_main_window.h"

#include "nel/misc/types_nl.h"
#include "nel/misc/sheet_id.h"

#include <QtGui/QUndoStack>
#include <QSignalMapper>

namespace Ui {
    class TileEditorMainWindow;
}

class TileModel;
class TileItemDelegate;

class TileEditorMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit TileEditorMainWindow(QWidget *parent = 0);
    ~TileEditorMainWindow();

	QUndoStack *getUndoStack() { return m_undoStack; }	

public Q_SLOTS:
	void save();
	void saveAs();

private Q_SLOTS:
	void onActionAddTile(bool triggered);
	void onActionDeleteTile(bool triggered);
	void onActionReplaceImage(bool triggered);
	void onActionDeleteImage(bool triggered);
	
	void onTileSetAdd();
	void onTileSetDelete();
	void onTileSetEdit();
	void onTileSetUp();
	void onTileSetDown();
	
	void onLandAdd();
	void onLandRemove();
	void onLandEdit();
	void onLandRowChanged( int row );

	void onResetVegetation();
	void onChooseVegetation();

	void onChooseTexturePath();

	void changeActiveTileSet(const QModelIndex &newIndex, const QModelIndex &oldIndex);
	void onZoomFactor(int level);

private:
	void onActionAddTile(int tabId);
	void onActionDeleteTile(int tabId);
	void onActionDeleteImage(int tabId);
	void onActionReplaceImage(int tabId);


	TileModel* createTileModel();
	QListView* getListViewByTab( int tab ) const;

	Ui::TileEditorMainWindow *m_ui;
	QUndoStack *m_undoStack;
	QMenu *m_rotationMenu;
	
	QMenu *m_tileDisplayMenu;
	QMenu *m_tileEditorMenu;

	QMenu *m_zoomMenu;
	QActionGroup *m_zoomActionGroup;
	QSignalMapper *m_zoomSignalMapper;

	TileItemDelegate *m_tileItemDelegate;

	QList< TileModel* > m_tileModels;

	QString m_texturePath;

	enum Tabs
	{
		TAB_128 = 0,
		TAB_256 = 1,
		TAB_TRANSITION = 2,
		TAB_DISPLACEMENT = 3,
		TAB_DETAILS = 4
	};

	QString m_fileName;
};

#endif // TILE_EDITOR_MAIN_WINDOW_H
