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


#ifndef WIDGETPROPS_H
#define WIDGETPROPS_H

#include "ui_widget_properties.h"
#include "widget_info.h"
#include <map>
#include <vector>
#include <string>

namespace GUIEditor
{
	class CWidgetInfoTree;

	class CWidgetProperties : public QWidget, public Ui::WidgetProperties
	{
		Q_OBJECT

	public:
		CWidgetProperties( QWidget *parent = NULL );
		~CWidgetProperties();
		void setupWidgetInfo( CWidgetInfoTree *tree );

	private Q_SLOTS:
		void onListSelectionChanged( int i );
		
		/// Removes widget from the list
		void onRemoveWButtonClicked();

		/// Removes widget property from the list
		void onRemovePButtonClicked();

	private:
		/// Builds the widget list
		void buildWidgetList();

		/// Builds the property list for the currently selected widget
		void setPropsOf( const char *name );

		CWidgetInfoTree *tree;
	};
}

#endif
