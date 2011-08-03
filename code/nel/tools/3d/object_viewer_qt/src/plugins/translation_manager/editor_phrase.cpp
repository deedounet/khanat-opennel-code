// Translation Manager Plugin - OVQT Plugin <http://dev.ryzom.com/projects/nel/>
// Copyright (C) 2010  Winch Gate Property Limited
// Copyright (C) 2011  Emanuel Costea <cemycc@gmail.com>
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

// Nel includes
#include "nel/misc/path.h"
#include "nel/misc/diff_tool.h"

// Qt includes
#include <QtGui/QErrorMessage>
#include <QtCore/qfileinfo.h>
#include <QtGui/QMessageBox>
#include <QtGui/QCloseEvent>

// Project includes
#include "editor_phrase.h"
#include "translation_manager_constants.h"

using namespace std;

namespace Plugin {

void CEditorPhrase::open(QString filename)
{
	vector<STRING_MANAGER::TPhrase> phrases;
	if(readPhraseFile(filename.toStdString(), phrases, false))
	{
		text_edit = new QTextEdit();
		// read the file content
		QFile file(filename);
		QTextStream in(&file);
		// set the file content to the text edit
		QString content = in.readAll();
		text_edit->setText(content); 
		// window settings
		setCurrentFile(filename);
        setAttribute(Qt::WA_DeleteOnClose);
        setWidget(text_edit);
		editor_type = Constants::ED_PHRASE;
		current_file = filename;
	} else {
        QErrorMessage error;
        error.showMessage("This file is not a phrase file.");
        error.exec();                             
    }
}

void CEditorPhrase::activateWindow()
{
	showMaximized();
}

void CEditorPhrase::save()
{
	QFile file(current_file);
	QTextStream out(&file);
	out<<text_edit->toPlainText();
	setCurrentFile(current_file);
}

void CEditorPhrase::saveAs(QString filename)
{
	QFile file(filename);
	QTextStream out(&file);
	out<<text_edit->toPlainText();
	current_file = filename;
	setCurrentFile(current_file);
}

void CEditorPhrase::closeEvent(QCloseEvent *event)
{
    if(isWindowModified())
    {
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) 
        {
                case QMessageBox::Save:
                    save();
                    event->accept();
                    close();
                    break;
                case QMessageBox::Discard:
                    event->accept();
                    close();
                    break;
                case QMessageBox::Cancel:
                    event->ignore();
                    break;
                default:
                    break;
        }
    } else {
        event->accept();
        close();
    }
}

}