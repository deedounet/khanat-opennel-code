// Object Viewer Qt GUI Editor plugin <http://dev.ryzom.com/projects/ryzom/>
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

#include "widget_properties.h"
#include "widget_info_tree.h"
#include <qmessagebox.h>

namespace GUIEditor{
	CWidgetProperties::CWidgetProperties( QWidget *parent ) :
	QWidget( parent )
	{
		setupUi( this );
		connect( rmWButton, SIGNAL( clicked( bool ) ), this, SLOT( onRemoveWButtonClicked() ) );
		connect( rmPButton, SIGNAL( clicked( bool ) ), this, SLOT( onRemovePButtonClicked() ) );
	}

	CWidgetProperties::~CWidgetProperties()
	{
	}

	void CWidgetProperties::setupWidgetInfo( CWidgetInfoTree *tree )
	{
		this->tree = tree;
		buildWidgetList();
		onListSelectionChanged( 0 );
		connect( widgetList, SIGNAL( currentRowChanged( int ) ), this, SLOT( onListSelectionChanged( int ) ) );
	}

	void CWidgetProperties::onListSelectionChanged( int i )
	{
		if( i < 0 )
			return;
		if( i >= widgetList->count() )
			return;

		QListWidgetItem *item = widgetList->item( i );
		setPropsOf( item->text().toStdString().c_str() );
	}


	void CWidgetProperties::onRemoveWButtonClicked()
	{
		if( widgetList->count() == 0 )
			return;

		QString widgetName = widgetList->currentItem()->text();

		int reply = QMessageBox::question( this,
										tr( "Removing a widget" ),
										tr( "Are you sure you want to remove %1?" ).arg( widgetName ),
										QMessageBox::Yes | QMessageBox::Cancel
										);

		if( reply != QMessageBox::Yes )
			return;

		tree->removeNode( widgetName.toStdString() );
		widgetPropTree->clear();
		buildWidgetList();
	}

	void CWidgetProperties::onRemovePButtonClicked()
	{
		QTreeWidgetItem *item = widgetPropTree->currentItem();
		CWidgetInfoTreeNode *node = tree->findNodeByName( widgetList->currentItem()->text().toStdString() );

		if( ( item == NULL ) || ( node == NULL ) )
			return;

		std::string name = item->text( 0 ).toStdString();

		std::vector< SPropEntry >::const_iterator itr = node->getInfo().findProp( name );
		if( itr == node->getInfo().props.end() )
			return;
		
		int reply = QMessageBox::question( this,
											tr( "Removing a property" ),
											tr( "Are you sure you want to remove" ).arg( QString( name.c_str() ) ),
											QMessageBox::Yes | QMessageBox::Cancel 
											);

		if( reply != QMessageBox::Yes )
			return;

		/* 
			Remove the damned thing here
		*/

		onListSelectionChanged( widgetList->currentRow() );
	}


	void CWidgetProperties::buildWidgetList()
	{
		widgetList->clear();
		std::vector< std::string > widgetNames;
		tree->getNames( widgetNames );
		std::sort( widgetNames.begin(), widgetNames.end() );
		for( std::vector< std::string >::const_iterator itr = widgetNames.begin(); itr != widgetNames.end(); ++itr )
			widgetList->addItem( itr->c_str() );
		widgetList->setCurrentRow( 0 );
	}

	void CWidgetProperties::setPropsOf( const char *name )
	{
		CWidgetInfoTreeNode *node = tree->findNodeByName( name );
		if( node == NULL )
			return;

		widgetPropTree->clear();

		const std::vector< SPropEntry > &v = node->getInfo().props;
		for( std::vector< SPropEntry >::const_iterator itr2 = v.begin(); itr2 != v.end(); ++itr2 )
		{
			SPropEntry e = *itr2;
			QTreeWidgetItem *item = new QTreeWidgetItem;
			item->setText( 0, e.propName.c_str() );
			item->setText( 1, e.propType.c_str() );
			item->setText( 2, e.propDefault.c_str() );
			widgetPropTree->addTopLevelItem( item );
		}
	}

}
