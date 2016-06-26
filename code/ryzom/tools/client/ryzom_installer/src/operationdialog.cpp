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
#include "operationdialog.h"
#include "downloader.h"
#include "profilesdialog.h"
#include "configfile.h"
#include "config.h"
#include "profilesmodel.h"
#include "utils.h"
#include "nel/misc/path.h"

#include "filescopier.h"
#include "filesextractor.h"
#include "filescleaner.h"

#include "seven_zip.h"

#if defined(Q_OS_WIN32) && defined(QT_WINEXTRAS_LIB)
#include <QtWinExtras/QWinTaskbarProgress>
#include <QtWinExtras/QWinTaskbarButton>
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

COperationDialog::COperationDialog(QWidget *parent):QDialog(parent), m_aborting(false), m_operation(OperationNone)
{
	setupUi(this);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

#if defined(Q_OS_WIN32) && defined(QT_WINEXTRAS_LIB)
	m_button = new QWinTaskbarButton(this);
#endif

	// downloader
	m_downloader = new CDownloader(this, this);

	connect(m_downloader, SIGNAL(downloadPrepared()), SLOT(onDownloadPrepared()));
	connect(m_downloader, SIGNAL(downloadDone()), SLOT(onDownloadDone()));

	connect(operationButtonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(onAbortClicked()));

	// operations
	connect(this, SIGNAL(prepare()), SLOT(onProgressPrepare()));
	connect(this, SIGNAL(init(qint64, qint64)), SLOT(onProgressInit(qint64, qint64)));
	connect(this, SIGNAL(start()), SLOT(onProgressStart()));
	connect(this, SIGNAL(stop()), SLOT(onProgressStop()));
	connect(this, SIGNAL(progress(qint64, QString)), SLOT(onProgressProgress(qint64, QString)));
	connect(this, SIGNAL(success(qint64)), SLOT(onProgressSuccess(qint64)));
	connect(this, SIGNAL(fail(QString)), SLOT(onProgressFail(QString)));
	connect(this, SIGNAL(done()), SLOT(onDone()));

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

COperationDialog::~COperationDialog()
{
}

void COperationDialog::setOperation(OperationType operation)
{
	m_operation = operation;
}

void COperationDialog::setUninstallComponents(const SComponents &components)
{
	m_removeComponents = components;
}

void COperationDialog::processNextStep()
{
	if (operationShouldStop())
	{
		reject();
		return;
	}

	switch (m_operation)
	{
		case OperationMigrate:
		case OperationInstall:
			processInstallNextStep();
			break;

		case OperationUpdateProfiles:
			processUpdateProfilesNextStep();
			break;

		case OperationUninstall:
			processUninstallNextStep();
			break;

		default:
			break;
	}
}

void COperationDialog::processInstallNextStep()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	// default profile
	const CProfile &configuration = config->getProfile();

	// long operations are done in a thread
	OperationStep step = config->getInstallNextStep();

	switch(step)
	{
		case DownloadData:
		downloadData();
		break;

		case ExtractDownloadedData:
		QtConcurrent::run(this, &COperationDialog::extractDownloadedData);
		break;

		case DownloadClient:
		downloadClient();
		break;

		case ExtractDownloadedClient:
		QtConcurrent::run(this, &COperationDialog::extractDownloadedClient);
		break;

		case CopyDataFiles:
		QtConcurrent::run(this, &COperationDialog::copyDataFiles);
		break;

		case CopyProfileFiles:
		QtConcurrent::run(this, &COperationDialog::copyProfileFiles);
		break;

		case CleanFiles:
		QtConcurrent::run(this, &COperationDialog::cleanFiles);
		break;

		case ExtractBnpClient:
		QtConcurrent::run(this, &COperationDialog::extractBnpClient);
		break;

		case CopyInstaller:
		QtConcurrent::run(this, &COperationDialog::copyInstaller);
		break;

		case UninstallOldClient:
		uninstallOldClient();
		break;

		case CreateProfile:
		createDefaultProfile();
		break;

		case CreateDesktopShortcut:
		createClientDesktopShortcut(0);
		break;

		case CreateMenuShortcut:
		createClientMenuShortcut(0);
		break;

		case CreateAddRemoveEntry:
		createAddRemoveEntry();
		break;

		case Done:
		accept();
		break;

		default:
		// cases already managed in main.cpp
		qDebug() << "Shouldn't happen, step" << step;
		break;
	}
}

void COperationDialog::updateAddRemoveComponents()
{
	QStringList serversToUpdate;

	QStringList profilesToDelete;
	QStringList profilesToAdd;

	CConfigFile *config = CConfigFile::getInstance();

	foreach(const CProfile &profile, config->getProfiles())
	{
		// append all new profiles
		profilesToAdd << profile.id;
	}

	foreach(const CProfile &profile, config->getBackupProfiles())
	{
		// append all old profiles
		profilesToDelete << profile.id;

		// remove profiles that didn't exist
		profilesToAdd.removeAll(profile.id);
	}

	const CServer &defaultServer = config->getServer();

	foreach(const CProfile &profile, config->getProfiles())
	{
		const CServer &server = config->getServer(profile.server);

		QString serverDirectory = server.getDirectory();

		// check if Ryzom is installed in new server directory
		if (server.id != defaultServer.id && !config->isRyzomInstalledIn(serverDirectory) && serversToUpdate.indexOf(server.id) == -1)
		{
			serversToUpdate << server.id;
		}

		// remove profiles that still exist
		profilesToDelete.removeAll(profile.id);
	}

	// update components to remove
	m_removeComponents.profiles << profilesToDelete;
	m_removeComponents.installer = false;
	m_removeComponents.downloadedFiles = false;

	// update components to add
	m_addComponents.profiles << profilesToAdd;
	m_addComponents.servers << serversToUpdate;
	m_addComponents.installer = false;
	m_addComponents.downloadedFiles = false;
}

void COperationDialog::processUpdateProfilesNextStep()
{
	// for "update profiles" operations, we set installer to false when components are updated,
	// since we're not using this variable
	if (m_addComponents.installer && m_removeComponents.installer)
	{
		updateAddRemoveComponents();
	}
	// TODO: check all servers are downloaded
	// TODO: delete profiles directories that are not used anymore
	// TODO: create shortcuts


	if (!m_removeComponents.profiles.isEmpty())
	{
		// delete profiles in another thread
		QtConcurrent::run(this, &COperationDialog::deleteComponentsProfiles);
		return;
	}

	if (!m_addComponents.profiles.isEmpty())
	{
		// add profiles in another thread
		QtConcurrent::run(this, &COperationDialog::addComponentsProfiles);
		return;
	}

	if (!m_addComponents.servers.isEmpty())
	{
		CConfigFile *config = CConfigFile::getInstance();
		const CServer &defaultServer = config->getServer();

		// servers files to download/update
		foreach(const QString &serverId, m_addComponents.servers)
		{
			const CServer &server = config->getServer(serverId);

			// data
			if (!config->areRyzomDataInstalledIn(server.getDirectory()))
			{
				QString dataFile = config->getInstallationDirectory() + "/" + server.dataDownloadFilename;

				// archive already downloaded
				if (QFile::exists(dataFile))
				{
					// make server current
					m_currentServerId = server.id;

					// uncompress it
					QtConcurrent::run(this, &COperationDialog::extractDownloadedData);
					return;
				}

				// data download URLs are different, can't copy data from default server
				if (server.dataDownloadUrl != defaultServer.dataDownloadUrl)
				{
					// download it
					downloadData();
					return;
				}

				// same data used

				// copy them
				// TODO
				return;
			}

			// client
			if (!config->isRyzomClientInstalledIn(server.getDirectory()))
			{
				// client download URLs are different, can't copy client from default server
				if (server.clientDownloadUrl == defaultServer.clientDownloadUrl)
				{
					if (QFile::exists(""))
						downloadData();
					return;
				}
			}
			else
			{
				QString clientFile = config->getInstallationDirectory() + "/" + config->expandVariables(server.clientDownloadFilename);
			}
		}
	}

	updateAddRemoveEntry();
}

void COperationDialog::processUninstallNextStep()
{
	CConfigFile *config = CConfigFile::getInstance();

	if (!m_removeComponents.servers.isEmpty())
	{
		QtConcurrent::run(this, &COperationDialog::deleteComponentsServers);
	}
	else if (!m_removeComponents.profiles.isEmpty())
	{
		QtConcurrent::run(this, &COperationDialog::deleteComponentsProfiles);
	}
	else if (m_removeComponents.downloadedFiles)
	{
		QtConcurrent::run(this, &COperationDialog::deleteComponentsDownloadedFiles);
	}
	else if (m_removeComponents.installer)
	{
		QtConcurrent::run(this, &COperationDialog::deleteComponentsInstaller);
	}
	else
	{
		// done
		accept();
	}
}

void COperationDialog::showEvent(QShowEvent *e)
{
#if defined(Q_OS_WIN32) && defined(QT_WINEXTRAS_LIB)
	m_button->setWindow(windowHandle());
#endif

	e->accept();

	processNextStep();
}

void COperationDialog::closeEvent(QCloseEvent *e)
{
	if (e->spontaneous())
	{
		e->ignore();

		onAbortClicked();
	}
}

void COperationDialog::onAbortClicked()
{
	if (m_downloader->isDownloading())
	{
		if (!m_downloader->supportsResume())
		{
			QMessageBox::StandardButton res = QMessageBox::question(this, tr("Confirmation"), tr("Warning, this server doesn't support resume! If you stop download now, you won't be able to resume it later.\nAre you sure to abort download?"));

			if (res != QMessageBox::Yes) return;
		}
	}

	QMutexLocker locker(&m_abortingMutex);
	m_aborting = true;
}

void COperationDialog::onDownloadPrepared()
{
	// actually download the file
	m_downloader->getFile();
}

void COperationDialog::onDownloadDone()
{
	renamePartFile();

	emit done();
}

void COperationDialog::onProgressPrepare()
{
	operationProgressBar->setFormat(tr("%p% (%v/%m KiB)"));

	operationProgressBar->setMinimum(0);
	operationProgressBar->setMaximum(0);
	operationProgressBar->setValue(0);

	operationLabel->setText(m_currentOperation);
}

void COperationDialog::onProgressInit(qint64 current, qint64 total)
{
	operationProgressBar->setMinimum(0);
	operationProgressBar->setMaximum(total / 1024);
	operationProgressBar->setValue(current / 1024);

#if defined(Q_OS_WIN32) && defined(QT_WINEXTRAS_LIB)
	m_button->progress()->setMinimum(0);
	m_button->progress()->setMaximum(total / 1024);
	m_button->progress()->setValue(current / 1024);
#endif
}

void COperationDialog::onProgressStart()
{
#if defined(Q_OS_WIN32) && defined(QT_WINEXTRAS_LIB)
	m_button->progress()->show();
#endif
}

void COperationDialog::onProgressStop()
{
#if defined(Q_OS_WIN32) && defined(QT_WINEXTRAS_LIB)
	m_button->progress()->hide();
#endif

	reject();
}

void COperationDialog::onProgressProgress(qint64 current, const QString &filename)
{
	operationProgressLabel->setText(m_currentOperationProgressFormat.arg(filename));

	operationProgressBar->setValue(current / 1024);

#if defined(Q_OS_WIN32) && defined(QT_WINEXTRAS_LIB)
	m_button->progress()->setValue(current / 1024);
#endif
}

void COperationDialog::onProgressSuccess(qint64 total)
{
	operationProgressBar->setValue(total / 1024);

#if defined(Q_OS_WIN32) && defined(QT_WINEXTRAS_LIB)
	m_button->progress()->hide();
#endif
}

void COperationDialog::onProgressFail(const QString &error)
{
	QMessageBox::critical(this, tr("Error"), error);
}

void COperationDialog::onDone()
{
	processNextStep();
}

void COperationDialog::downloadData()
{
	CConfigFile *config = CConfigFile::getInstance();

	const CServer &server = config->getServer(m_currentServerId);

	m_currentOperation = tr("Download data required by server %1").arg(server.name);
	m_currentOperationProgressFormat = tr("Downloading %1...");

	m_downloader->prepareFile(config->expandVariables(server.dataDownloadUrl), config->getInstallationDirectory() + "/" + config->expandVariables(server.dataDownloadFilename) + ".part");
}

void COperationDialog::extractDownloadedData()
{
	CConfigFile *config = CConfigFile::getInstance();

	const CServer &server = config->getServer(m_currentServerId);

	m_currentOperation = tr("Extract data files required by server %1").arg(server.name);
	m_currentOperationProgressFormat = tr("Extracting %1...");

	CFilesExtractor extractor(this);
	extractor.setSourceFile(config->getInstallationDirectory() + "/" + server.dataDownloadFilename);
	extractor.setDestinationDirectory(server.getDirectory());

	if (extractor.exec())
	{
	}
	else
	{
	}

	emit done();
}

void COperationDialog::downloadClient()
{
	CConfigFile *config = CConfigFile::getInstance();

	const CServer &server = config->getServer(m_currentServerId);

	m_currentOperation = tr("Download client required by server %1").arg(server.name);
	m_currentOperationProgressFormat = tr("Downloading %1...");

	m_downloader->prepareFile(config->expandVariables(server.clientDownloadUrl), config->getInstallationDirectory() + "/" + config->expandVariables(server.clientDownloadFilename) + ".part");
}

void COperationDialog::extractDownloadedClient()
{
	CConfigFile *config = CConfigFile::getInstance();

	const CServer &server = config->getServer(m_currentServerId);

	m_currentOperation = tr("Extract client files required by server %1").arg(server.name);
	m_currentOperationProgressFormat = tr("Extracting %1...");

	CFilesExtractor extractor(this);
	extractor.setSourceFile(config->getInstallationDirectory() + "/" + config->expandVariables(server.clientDownloadFilename));
	extractor.setDestinationDirectory(server.getDirectory());

	if (extractor.exec())
	{
	}
	else
	{
	}

	emit done();
}

void COperationDialog::copyDataFiles()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer(m_currentServerId);

	m_currentOperation = tr("Copy data files required by server %1").arg(server.name);
	m_currentOperationProgressFormat = tr("Copying %1...");

	QStringList serverFiles;
	serverFiles << "cfg";
	serverFiles << "data";
	serverFiles << "examples";
	serverFiles << "patch";
	serverFiles << "unpack";

	CFilesCopier copier(this);
	copier.setSourceDirectory(config->getSrcServerDirectory());
	copier.setDestinationDirectory(server.getDirectory());
	copier.setIncludeFilter(serverFiles);

	if (copier.exec())
	{
	}
	else
	{
	}

	emit done();
}

void COperationDialog::copyProfileFiles()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	// default profile
	const CProfile &profile = config->getProfile();

	m_currentOperation = tr("Copy old profile to new location");
	m_currentOperationProgressFormat = tr("Copying %1...");

	QStringList profileFiles;
	profileFiles << "cache";
	profileFiles << "save";
	profileFiles << "user";
	profileFiles << "screenshots";
	profileFiles << "client.cfg";
	profileFiles << "*.log";

	CFilesCopier copier(this);
	copier.setSourceDirectory(config->getSrcProfileDirectory());
	copier.setDestinationDirectory(profile.getDirectory());
	copier.setIncludeFilter(profileFiles);

	if (copier.exec())
	{
	}
	else
	{
	}

	emit done();
}

void COperationDialog::extractBnpClient()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	m_currentOperation = tr("Extract client to new location");
	m_currentOperationProgressFormat = tr("Extracting %1...");

	QString destinationDirectory = server.getDirectory();

	CFilesExtractor extractor(this);
	extractor.setSourceFile(config->getSrcServerClientBNPFullPath());
	extractor.setDestinationDirectory(destinationDirectory);
	extractor.exec();

	QString upgradeScript = destinationDirectory + "/upgd_nl.";

#ifdef Q_OS_WIN
	upgradeScript += "bat";
#else
	upgradeScript += "sh";
#endif

	if (QFile::exists(upgradeScript))
	{
		QProcess process;

		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		env.insert("RYZOM_CLIENT", QDir::toNativeSeparators(destinationDirectory + "/" + server.clientFilename));
		env.insert("UNPACKPATH", QDir::toNativeSeparators(destinationDirectory + "/unpack"));
		env.insert("ROOTPATH", QDir::toNativeSeparators(destinationDirectory));
		env.insert("STARTUPPATH", "");
		process.setProcessEnvironment(env);

		// permissions to execute script
		QFileDevice::Permissions permissions;
		permissions |= QFileDevice::ExeOther;
		permissions |= QFileDevice::ExeOwner;
		permissions |= QFileDevice::ExeUser;
		permissions |= QFileDevice::ReadOther;
		permissions |= QFileDevice::ReadOwner;
		permissions |= QFileDevice::ReadUser;
		permissions |= QFileDevice::WriteOwner;

		if (!QFile::setPermissions(upgradeScript, permissions))
		{
			qDebug() << "Unable to set executable flag to" << upgradeScript;
		}

		process.start(upgradeScript);

		while (process.waitForFinished())
		{
			qDebug() << "waiting";
		}
	}

	emit done();
}

void COperationDialog::copyInstaller()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	m_currentOperation = tr("Copy installer to new location");
	m_currentOperationProgressFormat = tr("Copying %1...");

	QString destinationDirectory = config->getInstallationDirectory();

	// rename old client to installer
	QString newInstallerFilename = server.installerFilename;

	if (!newInstallerFilename.isEmpty())
	{
		QString oldInstallerFullPath = QApplication::applicationFilePath();
		QString newInstallerFullPath = config->getInstallationDirectory() + "/" + newInstallerFilename;

		if (!QFile::exists(newInstallerFullPath))
		{
			QStringList filter;
			filter << "msvcp100.dll";
			filter << "msvcr100.dll";

			CFilesCopier copier(this);
			copier.setIncludeFilter(filter);
			copier.addFile(oldInstallerFullPath);
			copier.setSourceDirectory(config->getSrcServerDirectory().isEmpty() ? QApplication::applicationDirPath():config->getSrcServerDirectory());
			copier.setDestinationDirectory(config->getInstallationDirectory());
			copier.exec();

			// copied file
			oldInstallerFullPath = config->getInstallationDirectory() + "/" + QFileInfo(oldInstallerFullPath).fileName();

			// rename old filename if different
			if (oldInstallerFullPath != newInstallerFullPath)
			{
				QFile::rename(oldInstallerFullPath, newInstallerFullPath);
			}
		}

		// create menu directory if defined
		QString path = config->getMenuDirectory();

		if (!path.isEmpty())
		{
			QDir dir;

			if (!dir.mkpath(path))
			{
				qDebug() << "Unable to create directory" << path;
			}
		}

		// create installer link in menu
		QString executable = newInstallerFullPath;
		QString shortcut = config->getInstallerMenuLinkFullPath();
		QString desc = "Ryzom Installer";

		createLink(executable, shortcut, "", "", desc);
	}

	emit done();
}

void COperationDialog::uninstallOldClient()
{
#ifdef Q_OS_WIN

#ifdef Q_OS_WIN64
	// use WOW6432Node in 64 bits (64 bits OS and 64 bits Installer) because Ryzom old installer was in 32 bits
	QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Ryzom", QSettings::NativeFormat);
#else
	QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Ryzom", QSettings::NativeFormat);
#endif

	// check if Ryzom 2.1.0 is installed
	if (settings.contains("UninstallString"))
	{
		QString uninstaller = settings.value("UninstallString").toString();

		if (!uninstaller.isEmpty() && QFile::exists(uninstaller))
		{
			QMessageBox::StandardButtons button = QMessageBox::question(this, tr("Uninstall old client"), tr("An old version of Ryzom has been detected on this system, would you like to uninstall it to save space disk?"));

			if (button == QMessageBox::Yes)
			{
				CConfigFile::getInstance()->setShouldUninstallOldClient(true);

				QDesktopServices::openUrl(QUrl::fromLocalFile(uninstaller));
			}
			else
			{
				// don't ask this question anymore
				CConfigFile::getInstance()->setShouldUninstallOldClient(false);
			}
		}
	}
#endif

	emit done();
}

void COperationDialog::cleanFiles()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	m_currentOperation = tr("Clean obsolete files");
	m_currentOperationProgressFormat = tr("Deleting %1...");

	CFilesCleaner cleaner(this);
	cleaner.setDirectory(server.getDirectory());
	cleaner.exec();

	emit done();
}

bool COperationDialog::createDefaultProfile()
{
	CConfigFile *config = CConfigFile::getInstance();

	CServer server = config->getServer();

	m_currentOperation = tr("Create default profile");

	CProfile profile;

	profile.id = "0";
	profile.name = QString("Ryzom (%1)").arg(server.name);
	profile.server = server.id;
	profile.comments = "Default profile created by Ryzom Installer";
	profile.desktopShortcut = false;
	profile.menuShortcut = false;

#ifdef Q_OS_WIN32
	QStringList paths;

	// desktop

	// Windows XP
	paths << "C:/Documents and Settings/All Users/Desktop";
	// since Windows Vista
	paths << "C:/Users/Public/Desktop";
	// new location
	paths << QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

	foreach(const QString &path, paths)
	{
		if (QFile::exists(path + "/Ryzom.lnk")) profile.desktopShortcut = true;
	}

	paths.clear();

	// start menu

	// Windows XP
	paths << "C:/Documents and Settings/All Users/Start Menu/Programs";
	// since Windows Vista
	paths << "C:/ProgramData/Microsoft/Windows/Start Menu/Programs";
	// new location
	paths << QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);

	foreach(const QString &path, paths)
	{
		if (QFile::exists(path + "/Ryzom/Ryzom.lnk")) profile.menuShortcut = true;
	}
#endif

	config->addProfile(profile);
	config->save();

	emit done();

	return true;
}

bool COperationDialog::createClientDesktopShortcut(const QString &profileId)
{
	CConfigFile *config = CConfigFile::getInstance();

	const CProfile &profile = config->getProfile(profileId);
	const CServer &server = config->getServer(profile.server);

	m_currentOperation = tr("Create desktop shortcut for profile %1").arg(profile.id);

#ifdef Q_OS_WIN32
	if (profile.desktopShortcut)
	{
		QString executable = profile.getClientFullPath();
		QString shortcut = profile.getClientDesktopLinkFullPath();
		QString workingDir = server.getDirectory();

		QString arguments = QString("--profile %1").arg(profile.id);

		// append custom arguments
		if (!profile.arguments.isEmpty()) arguments += QString(" %1").arg(profile.arguments);

		createLink(executable, shortcut, arguments, workingDir, profile.comments);
	}
#endif

	emit done();

	return true;
}

bool COperationDialog::createClientMenuShortcut(const QString &profileId)
{
	CConfigFile *config = CConfigFile::getInstance();

	const CProfile &profile = config->getProfile(profileId);
	const CServer &server = config->getServer(profile.server);

	m_currentOperation = tr("Create menu shortcut for profile %1").arg(profile.id);

#ifdef Q_OS_WIN32
	if (profile.menuShortcut)
	{
		QString executable = profile.getClientFullPath();
		QString shortcut = profile.getClientMenuLinkFullPath();
		QString workingDir = server.getDirectory();

		QString arguments = QString("--profile %1").arg(profile.id);

		// append custom arguments
		if (!profile.arguments.isEmpty()) arguments += QString(" %1").arg(profile.arguments);

		createLink(executable, shortcut, arguments, workingDir, profile.comments);
	}
#endif

	emit done();

	return true;
}

bool COperationDialog::createAddRemoveEntry()
{
	CConfigFile *config = CConfigFile::getInstance();

	const CServer &server = config->getServer();

	QString oldInstallerFilename = server.clientFilenameOld;
	QString newInstallerFilename = server.installerFilename;

	if (!oldInstallerFilename.isEmpty() && !newInstallerFilename.isEmpty())
	{
		QString oldInstallerFullPath = config->getSrcServerDirectory() + "/" + oldInstallerFilename;
		QString newInstallerFullPath = config->getInstallationDirectory() + "/" + newInstallerFilename;

		if (QFile::exists(newInstallerFullPath))
		{
#ifdef Q_OS_WIN
			QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Ryzom", QSettings::NativeFormat);

			QStringList versionTokens = QString(RYZOM_VERSION).split('.');
			QString nativeFullPath = QDir::toNativeSeparators(newInstallerFullPath);

			settings.setValue("Comments", "");
			settings.setValue("DisplayIcon", nativeFullPath + ",0");
			settings.setValue("DisplayName", QApplication::applicationName());
			settings.setValue("DisplayVersion", RYZOM_VERSION);
			settings.setValue("EstimatedSize", getDirectorySize(config->getInstallationDirectory()));
			settings.setValue("InstallDate", QDateTime::currentDateTime().toString("Ymd"));
			settings.setValue("InstallLocation", config->getInstallationDirectory());
			settings.setValue("MajorVersion", versionTokens[0].toInt());
			settings.setValue("MinorVersion", versionTokens[1].toInt());
			settings.setValue("NoModify", 0);
			settings.setValue("NoRemove", 0);
			settings.setValue("NoRepair", 0);
			if (!config->getProductPublisher().isEmpty()) settings.setValue("Publisher", config->getProductPublisher());
			settings.setValue("QuietUninstallString", nativeFullPath + " -u -s");
			settings.setValue("UninstallString", nativeFullPath + " -u");
			if (!config->getProductUpdateUrl().isEmpty()) settings.setValue("URLUpdateInfo", config->getProductUpdateUrl());
			if (!config->getProductAboutUrl().isEmpty()) settings.setValue("URLInfoAbout", config->getProductAboutUrl());
			if (!config->getProductHelpUrl().isEmpty()) settings.setValue("HelpLink", config->getProductHelpUrl());
			//	ModifyPath
#endif
		}
	}

	emit done();

	return true;
}

bool COperationDialog::updateAddRemoveEntry()
{
	CConfigFile *config = CConfigFile::getInstance();

	const CServer &server = config->getServer();

	QString oldInstallerFilename = server.clientFilenameOld;
	QString newInstallerFilename = server.installerFilename;

	if (!oldInstallerFilename.isEmpty() && !newInstallerFilename.isEmpty())
	{
		QString oldInstallerFullPath = config->getSrcServerDirectory() + "/" + oldInstallerFilename;
		QString newInstallerFullPath = config->getInstallationDirectory() + "/" + newInstallerFilename;

		if (QFile::exists(newInstallerFullPath))
		{
#ifdef Q_OS_WIN
			QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Ryzom", QSettings::NativeFormat);
			QStringList versionTokens = QApplication::applicationVersion().split('.');

			settings.setValue("DisplayVersion", QApplication::applicationVersion());
			settings.setValue("EstimatedSize", getDirectorySize(config->getInstallationDirectory()));
			settings.setValue("MajorVersion", versionTokens[0].toInt());
			settings.setValue("MinorVersion", versionTokens[1].toInt());
#endif
		}
	}

	return true;
}

bool COperationDialog::deleteAddRemoveEntry()
{
#ifdef Q_OS_WIN
	QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Ryzom", QSettings::NativeFormat);
	settings.remove("");
#endif

	emit done();

	return true;
}

void COperationDialog::deleteComponentsServers()
{
	m_currentOperation = tr("Delete client files");
	m_currentOperationProgressFormat = tr("Deleting %1...");

	emit prepare();
	emit init(0, m_removeComponents.servers.size());
	emit start();

	CConfigFile *config = CConfigFile::getInstance();

	int i = 0;

	foreach(const QString &serverId, m_removeComponents.servers)
	{
		if (operationShouldStop())
		{
			emit stop();
			return;
		}

		const CServer &server = config->getServer(serverId);

		emit progress(i++, server.name);

		QString path = server.getDirectory();

		if (!path.isEmpty())
		{
			QDir dir(path);

			if (dir.exists() && !dir.removeRecursively())
			{
				emit fail(tr("Unable to delete files for client %1").arg(server.name));
				return;
			}
		}
	}

	emit success(m_removeComponents.servers.size());

	// clear list of all servers to uninstall
	m_removeComponents.servers.clear();

	emit done();
}

void COperationDialog::addComponentsProfiles()
{
	m_currentOperation = tr("Add profiles");
	m_currentOperationProgressFormat = tr("Adding profile %1...");

	CConfigFile *config = CConfigFile::getInstance();

	foreach(const QString &profileId, m_addComponents.profiles)
	{
		CProfile &profile = config->getProfile(profileId);

		if (profile.desktopShortcut) createClientDesktopShortcut(profile.id);

		if (profile.menuShortcut) createClientMenuShortcut(profile.id);
	}
}

void COperationDialog::deleteComponentsProfiles()
{
	m_currentOperation = tr("Delete profiles");
	m_currentOperationProgressFormat = tr("Deleting profile %1...");

	emit prepare();
	emit init(0, m_removeComponents.servers.size());

	CConfigFile *config = CConfigFile::getInstance();

	int i = 0;

	foreach(const QString &profileId, m_removeComponents.profiles)
	{
		if (operationShouldStop())
		{
			emit stop();
			return;
		}

		const CProfile &profile = config->getProfile(profileId);

		emit progress(i++, profile.name);

		QString path = profile.getDirectory();

		if (!path.isEmpty())
		{
			QDir dir(path);

			if (dir.exists() && !dir.removeRecursively())
			{
				emit fail(tr("Unable to delete files for profile %1").arg(profile.name));
				return;
			}
		}

		// TODO: delete links

		// delete profile
		config->removeProfile(profileId);
	}

	emit success(m_removeComponents.profiles.size());

	// clear list of all profiles to uninstall
	m_removeComponents.profiles.clear();

	emit done();
}

void COperationDialog::deleteComponentsInstaller()
{
	m_currentOperation = tr("Delete installer");
	m_currentOperationProgressFormat = tr("Deleting %1...");

	CConfigFile *config = CConfigFile::getInstance();

	// TODO: delete installer

	deleteAddRemoveEntry();

	// delete menu
	QString path = config->getMenuDirectory();

	if (!path.isEmpty())
	{
		QDir dir(path);

		dir.removeRecursively();
	}

	// TODO:

	// reset it once it's done
	m_removeComponents.installer = false;

	emit onProgressSuccess(1);
	emit done();
}

void COperationDialog::deleteComponentsDownloadedFiles()
{
	m_currentOperation = tr("Delete downloaded files");
	m_currentOperationProgressFormat = tr("Deleting %1...");

	CConfigFile *config = CConfigFile::getInstance();

	QString path = config->getInstallationDirectory();

	QDir dir(path);

	QStringList filter;
	filter << "*.7z";
	filter << "*.bnp";
	filter << "*.zip";
	filter << "*.part";

	QStringList files = dir.entryList(filter, QDir::Files);

	foreach(const QString &file, files)
	{
		if (!QFile::remove(file))
		{
			qDebug() << "Unable to delete" << file;
		}
	}

	// reset it once it's done
	m_removeComponents.downloadedFiles = false;

	emit onProgressSuccess(1);
	emit done();
}

void COperationDialog::operationPrepare()
{
	emit prepare();
}

void COperationDialog::operationInit(qint64 current, qint64 total)
{
	emit init(current, total);
}

void COperationDialog::operationStart()
{
	emit start();
}

void COperationDialog::operationStop()
{
	emit stop();
}

void COperationDialog::operationProgress(qint64 current, const QString &filename)
{
	emit progress(current, filename);
}

void COperationDialog::operationSuccess(qint64 total)
{
	emit success(total);
}

void COperationDialog::operationFail(const QString &error)
{
	emit fail(error);
}

bool COperationDialog::operationShouldStop()
{
	QMutexLocker locker(&m_abortingMutex);

	return m_aborting;
}

void COperationDialog::renamePartFile()
{
	QString partFile = m_downloader->getFileFullPath();

	QString finalFile = partFile;
	finalFile.remove(".part");

	if (partFile != finalFile)
	{
		QFile::rename(partFile, finalFile);
	}
}
