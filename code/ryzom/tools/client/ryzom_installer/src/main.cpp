// Ryzom - MMORPG Framework <http://dev.ryzom.com/projects/ryzom/>
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

#include "stdpch.h"
#include "mainwindow.h"
#include "configfile.h"
#include "migratedialog.h"
#include "installdialog.h"
#include "uninstalldialog.h"
#include "operationdialog.h"
#include "utils.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef QT_STATICPLUGIN

#include <QtPlugin>

#if defined(Q_OS_WIN32)
	Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#elif defined(Q_OS_MAC)
	Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
#elif defined(Q_OS_UNIX)
	Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

	Q_IMPORT_PLUGIN(QICOPlugin)

#endif

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

// copy all specified files from current directory to destination directory
bool copyInstallerFiles(const QStringList &files, const QString &destination)
{
	QString path = QApplication::applicationDirPath();

	foreach(const QString &file, files)
	{
		// convert to absolute path
		QString srcPath = path + "/" + file;
		QString dstPath = destination + "/" + file;

		if (QFile::exists(srcPath))
		{
			if (QFile::exists(dstPath))
			{
				if (!QFile::remove(dstPath))
				{
					qDebug() << "Unable to delete" << dstPath;
				}
			}

			if (!QFile::copy(srcPath, dstPath))
			{
				qDebug() << "Unable to copy" << srcPath << "to" << dstPath;

				return false;
			}
		}
	}

	return true;
}

#ifdef Q_OS_WIN
class CCOMHelper
{
	bool m_mustUninit;

public:
	CCOMHelper()
	{
		// to fix the bug with QFileDialog::getExistingDirectory hanging under Windows
		m_mustUninit = SUCCEEDED(CoInitialize(NULL));
	}

	~CCOMHelper()
	{
		// only call CoUninitialize if CoInitialize succeeded
		if (m_mustUninit) CoUninitialize();
	}
};
#endif

int main(int argc, char *argv[])
{
#if defined(_MSC_VER) && defined(_DEBUG)
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

#ifdef Q_OS_WIN
	CCOMHelper comHelper;
#endif

	NLMISC::CApplicationContext appContext;

	QApplication app(argc, argv);

	QApplication::setApplicationName("Ryzom");
	QApplication::setApplicationVersion(RYZOM_VERSION);
	QApplication::setWindowIcon(QIcon(":/icons/ryzom.ico"));

	QLocale locale = QLocale::system();

	// load application translations
	QTranslator localTranslator;
	if (localTranslator.load(locale, "ryzom_installer", "_", ":/translations"))
	{
		QApplication::installTranslator(&localTranslator);
	}

	// load Qt default translations
	QTranslator qtTranslator;
	if (qtTranslator.load(locale, "qtbase", "_", ":/translations"))
	{
		QApplication::installTranslator(&qtTranslator);
	}

	// instanciate ConfigFile
	CConfigFile config;
	OperationStep step = config.load() ? config.getInstallNextStep():DisplayNoServerError;

	if (step == DisplayNoServerError)
	{
		QMessageBox::critical(NULL, QApplication::tr("Error"), QApplication::tr("Unable to find ryzom_installer.ini"));
		return 1;
	}

#if defined(Q_OS_WIN) && !defined(_DEBUG)
	// under Windows, Ryzom Installer should always be copied in TEMP directory
	QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

	// check if launched from TEMP directory
	if (step == Done && !QApplication::applicationDirPath().startsWith(tempPath))
	{
		// try to delete all temporary installers
		QDir tempDir(tempPath);

		QStringList filter;
		filter << "ryzom_installer_*";

		QStringList dirs = tempDir.entryList(filter, QDir::Dirs);

		foreach(const QString &dir, dirs)
		{
			QDir(dir).removeRecursively();
		}

		tempPath += QString("/ryzom_installer_%1").arg(QDateTime::currentMSecsSinceEpoch());

		// copy installer and required files to TEMP directory
		if (copyInstallerFiles(config.getInstallerRequiredFiles(), tempPath))
		{
			QString tempFile = tempPath + "/" + QFileInfo(QApplication::applicationFilePath()).fileName();

			// launch copy in TEMP directory with same arguments
			if (QProcess::startDetached(tempFile, QApplication::arguments())) return 0;
		}
	}
#endif

	// use product name from ryzom_installer.ini
	if (!config.getProductName().isEmpty()) QApplication::setApplicationName(config.getProductName());

	// define commandline arguments
	QCommandLineParser parser;
	parser.setApplicationDescription(QApplication::tr("Instalation and launcher tool for Ryzom"));
	parser.addHelpOption();
	parser.addVersionOption();

	// root, username and password are optional because they can be saved in settings file
	QCommandLineOption uninstallOption(QStringList() << "u" << "uninstall", QApplication::tr("Uninstall"));
	parser.addOption(uninstallOption);

	QCommandLineOption silentOption(QStringList() << "s" << "silent", QApplication::tr("Silent mode"));
	parser.addOption(silentOption);

	// process the actual command line arguments given by the user
	parser.process(app);

	if (parser.isSet(uninstallOption))
	{
		SComponents components;

		// add all servers by default
		for (int i = 0; i < config.getServersCount(); ++i)
		{
			components.servers << config.getServer(i).id;
		}

		// show uninstall wizard dialog if not in silent mode
		if (!parser.isSet(silentOption))
		{
			CUninstallDialog dialog;

			dialog.setSelectedComponents(components);

			// TODO: check real return codes from Uninstallers
			if (!dialog.exec()) return 1;

			components = dialog.getSelectedCompenents();
		}

		COperationDialog dialog;

		dialog.setOperation(OperationUninstall);
		dialog.setUninstallComponents(components);

		// TODO: set all components to uninstall

		return dialog.exec() ? 0 : 1;
	}

	if (step == ShowMigrateWizard)
	{
		CMigrateDialog dialog;

		if (!dialog.exec()) return 1;

		step = config.getInstallNextStep();
	}
	else if (step == ShowInstallWizard)
	{
		CInstallDialog dialog;

		if (!dialog.exec()) return 1;

		step = config.getInstallNextStep();
	}
	
	if (step != Done)
	{
		COperationDialog dialog;
		dialog.setOperation(config.getSrcServerDirectory().isEmpty() ? OperationInstall:OperationMigrate);

		if (!dialog.exec()) return 1;

		step = config.getInstallNextStep();

		if (step == Done)
		{
#if defined(Q_OS_WIN) && !defined(_DEBUG)
			// restart Installer, so it could be copied in TEMP and allowed to update itself
			if (QProcess::startDetached(QApplication::applicationFilePath(), QApplication::arguments())) return 0;
#endif
		}
	}

	CMainWindow mainWindow;
	mainWindow.show();

	return QApplication::exec();
}
