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
#include "wizarddialog.h"
#include "profilesdialog.h"
#include "configfile.h"
#include "config.h"
#include "profilesmodel.h"

#include "filescopier.h"
#include "filesextractor.h"
#include "filescleaner.h"

#include "seven_zip.h"

#if defined(Q_OS_WIN32) && defined(QT_WINEXTRAS_LIB)
#include <QtWinExtras/QWinTaskbarProgress>
#include <QtWinExtras/QWinTaskbarButton>
#endif

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

COperationDialog::COperationDialog():QDialog(), m_aborting(false)
{
	setupUi(this);

#if defined(Q_OS_WIN32) && defined(QT_WINEXTRAS_LIB)
	m_button = new QWinTaskbarButton(this);
#endif

//	connect(resumeButton, SIGNAL(clicked()), SLOT(onResumeClicked()));
//	connect(stopButton, SIGNAL(clicked()), SLOT(onStopClicked()));

	// downloader
	m_downloader = new CDownloader(this);

	connect(m_downloader, SIGNAL(downloadPrepare()), SLOT(onProgressPrepare()));
	connect(m_downloader, SIGNAL(downloadInit(qint64, qint64)), SLOT(onProgressInit(qint64, qint64)));
	connect(m_downloader, SIGNAL(downloadStart()), SLOT(onProgressStart()));
	connect(m_downloader, SIGNAL(downloadStop()), SLOT(onProgressStop()));
	connect(m_downloader, SIGNAL(downloadProgress(qint64)), SLOT(onProgressProgress(qint64)));
	connect(m_downloader, SIGNAL(downloadSuccess(qint64)), SLOT(onProgressSuccess(qint64)));
	connect(m_downloader, SIGNAL(downloadFail(QString)), SLOT(onProgressFail(QString)));

	connect(operationButtonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(onAbortClicked()));

	// operations
	connect(this, SIGNAL(prepare()), SLOT(onProgressPrepare()));
	connect(this, SIGNAL(init(qint64, qint64)), SLOT(onProgressInit(qint64, qint64)));
	connect(this, SIGNAL(start()), SLOT(onProgressStart()));
	connect(this, SIGNAL(stop()), SLOT(onProgressStop()));
	connect(this, SIGNAL(progress(qint64, QString)), SLOT(onProgressProgress(qint64, QString)));
	connect(this, SIGNAL(success(qint64)), SLOT(onProgressSuccess(qint64)));
	connect(this, SIGNAL(fail(QString)), SLOT(onProgressFail(QString)));

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

COperationDialog::~COperationDialog()
{
}

void COperationDialog::processNextStep()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	// default profile
	const CProfile &configuration = config->getProfile();

	switch(config->getNextStep())
	{
		case CConfigFile::DisplayNoServerError:
		break;

		case CConfigFile::ShowWizard:
		break;

		case CConfigFile::DownloadData:
		downloadData();
		break;

		case CConfigFile::ExtractDownloadedData:
		// TODO
		break;

		case CConfigFile::DownloadClient:
		downloadClient();
		break;

		case CConfigFile::ExtractDownloadedClient:
		// TODO
		break;

		case CConfigFile::CopyServerFiles:
		QtConcurrent::run(this, &COperationDialog::copyServerFiles);
		break;

		case CConfigFile::CopyProfileFiles:
		QtConcurrent::run(this, &COperationDialog::copyProfileFiles);
		break;

		case CConfigFile::ExtractBnpClient:
		QtConcurrent::run(this, &COperationDialog::extractBnpClient);
		break;

		case CConfigFile::CleanFiles:
		QtConcurrent::run(this, &COperationDialog::cleanFiles);
		break;

		case CConfigFile::CreateProfile:
		createDefaultProfile();
		break;

		case CConfigFile::CreateShortcuts:
		createDefaultShortcuts();
		break;

		default:
		// cases already managed in main.cpp
		break;
	}

	m_downloader->getHtmlPageContent(config->expandVariables(server.displayUrl));
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

	close();
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
	if (!operationShouldStop()) processNextStep();
}

void COperationDialog::downloadData()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	m_currentOperation = QApplication::tr("Download data required for server %1").arg(server.name);
	m_currentOperationProgressFormat = QApplication::tr("Downloading %1...");

	m_downloader->prepareFile(config->expandVariables(server.dataDownloadUrl), config->getInstallationDirectory() + "/" + config->expandVariables(server.dataDownloadFilename) + ".part");
}

void COperationDialog::downloadClient()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	m_currentOperation = QApplication::tr("Download client required for server %1").arg(server.name);
	m_currentOperationProgressFormat = QApplication::tr("Downloading %1...");

	m_downloader->prepareFile(config->expandVariables(server.clientDownloadUrl), config->getInstallationDirectory() + "/" + config->expandVariables(server.clientDownloadFilename) + ".part");
}

void COperationDialog::copyServerFiles()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	// default profile
	const CProfile &configuration = config->getProfile();

	QStringList serverFiles;
	serverFiles << "cfg";
	serverFiles << "data";
	serverFiles << "examples";
	serverFiles << "patch";
	serverFiles << "unpack";
	serverFiles << "client_default.cfg";

	m_currentOperation = QApplication::tr("Copying client files needed for server %1...").arg(server.name);
	m_currentOperationProgressFormat = QApplication::tr("Copying %1...");

	CFilesCopier copier(this);
	copier.setSourceDirectory(config->getSrcServerDirectory());
	copier.setDesinationDirectory(config->getInstallationDirectory() + "/" + server.id);
	copier.setIncludeFilter(serverFiles);

	if (copier.exec())
	{
	}
	else
	{
	}
}

void COperationDialog::copyProfileFiles()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	// default profile
	const CProfile &profile = config->getProfile();

	QStringList profileFiles;
	profileFiles << "cache";
	profileFiles << "save";
	profileFiles << "user";
	profileFiles << "screenshots";
	profileFiles << "client.cfg";
	profileFiles << "*.log";

	CFilesCopier copier(this);
	copier.setSourceDirectory(config->getSrcProfileDirectory());
	copier.setDesinationDirectory(config->getProfileDirectory() + "/" + profile.id);
	copier.setIncludeFilter(profileFiles);

	if (copier.exec())
	{
	}
	else
	{
	}
}

void COperationDialog::extractBnpClient()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	// default profile
	const CProfile &profile = config->getProfile();

	CFilesExtractor extractor(this);
	extractor.setSourceFile(config->getSrcServerClientBNPFullPath());
	extractor.setDesinationDirectory(config->getInstallationDirectory() + "/" + server.id);
	extractor.exec();
}

void COperationDialog::cleanFiles()
{
	CConfigFile *config = CConfigFile::getInstance();

	// default server
	const CServer &server = config->getServer();

	// default profile
	const CProfile &profile = config->getProfile();

	CFilesCleaner cleaner(this);
	cleaner.setDirectory(config->getInstallationDirectory() + "/" + server.id);
	cleaner.exec();
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

bool COperationDialog::createDefaultProfile()
{
	CConfigFile *config = CConfigFile::getInstance();

	CServer server = config->getServer(config->getDefaultServerIndex());

	CProfile profile;

	profile.id = 0;
	profile.executable = config->getClientFullPath();
	profile.name = QString("Ryzom (%1)").arg(server.name);
	profile.server = server.id;
	profile.comments = "Default profile created by Ryzom Installer";

#ifdef Q_OS_WIN32
	profile.desktopShortcut = QFile::exists(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/Ryzom.lnk");
#endif

	// TODO
	// profile.menuShortcut

	config->addProfile(profile);
	config->save();

	onDone();

	return true;
}

bool COperationDialog::createDefaultShortcuts()
{
	onDone();

	return true;
}
