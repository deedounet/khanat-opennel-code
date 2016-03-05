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
#include "configfile.h"
#include "utils.h"

#include "nel/misc/path.h"

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

const CServer NoServer;
const CProfile NoProfile;

CConfigFile *CConfigFile::s_instance = NULL;

CConfigFile::CConfigFile(QObject *parent):QObject(parent), m_defaultServer(0), m_defaultProfile(0), m_use64BitsClient(false)
{
	s_instance = this;

	m_defaultConfigPath = QApplication::applicationDirPath() + "/installer.ini";
	m_configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/installer.ini";
}

CConfigFile::~CConfigFile()
{
	s_instance = NULL;
}

bool CConfigFile::load()
{
	return load(m_configPath) || load(m_defaultConfigPath);
}

bool CConfigFile::load(const QString &filename)
{
	QSettings settings(filename, QSettings::IniFormat);

	settings.beginGroup("common");
	m_language = settings.value("language").toString();
	m_srcDirectory = settings.value("source_directory").toString();
	m_installationDirectory = settings.value("installation_directory").toString();
	m_use64BitsClient = settings.value("use_64bits_client").toBool();
	settings.endGroup();

	settings.beginGroup("servers");
	int serversCount = settings.value("size").toInt();
	m_defaultServer = settings.value("default").toInt();
	settings.endGroup();
	
	m_servers.resize(serversCount);

	for(int i = 0; i < serversCount; ++i)
	{
		CServer &server = m_servers[i];

		settings.beginGroup(QString("server_%1").arg(i));

		server.id = settings.value("id").toString();
		server.name = settings.value("name").toString();
		server.displayUrl = settings.value("display_url").toString();
		server.dataDownloadUrl = settings.value("data_download_url").toString();
		server.dataDownloadFilename = settings.value("data_download_filename").toString();
		server.dataCompressedSize = settings.value("data_compressed_size").toULongLong();
		server.dataUncompressedSize = settings.value("data_uncompressed_size").toULongLong();
		server.clientDownloadUrl = settings.value("client_download_url").toString();
		server.clientDownloadFilename = settings.value("client_download_filename").toString();
#if defined(Q_OS_WIN)
		server.clientFilename = settings.value("client_filename_windows").toString();
#elif defined(Q_OS_MAC)
		server.clientFilename = settings.value("client_filename_osx").toString();
#else
		server.clientFilename = settings.value("client_filename_linux").toString();
#endif
		server.comments = settings.value("comments").toString();

		settings.endGroup();
	}

	settings.beginGroup("profiles");
	int profilesCounts = settings.value("size").toInt();
	m_defaultProfile = settings.value("default").toInt();
	settings.endGroup();
	
	m_profiles.resize(profilesCounts);

	for(int i = 0; i < profilesCounts; ++i)
	{
		CProfile &profile = m_profiles[i];

		settings.beginGroup(QString("profile_%1").arg(i));

		profile.id = settings.value("id").toInt();
		profile.name = settings.value("name").toString();
		profile.account = settings.value("account").toString();
		profile.server = settings.value("server").toString();
		profile.executable = settings.value("executable").toString();
		profile.arguments = settings.value("arguments").toString();
		profile.comments = settings.value("comments").toString();

		settings.endGroup();
	}

	return !m_servers.isEmpty();
}

bool CConfigFile::save() const
{
	QSettings settings(m_configPath, QSettings::IniFormat);

	settings.beginGroup("common");
	settings.setValue("language", m_language);
	settings.setValue("source_directory", m_srcDirectory);
	settings.setValue("installation_directory", m_installationDirectory);
	settings.setValue("use_64bits_client", m_use64BitsClient);
	settings.endGroup();

	settings.beginGroup("servers");
	settings.setValue("size", m_servers.size());
	settings.setValue("default", m_defaultServer);
	settings.endGroup();

	for(int i = 0; i < m_servers.size(); ++i)
	{
		const CServer &server = m_servers[i];

		settings.beginGroup(QString("server_%1").arg(i));

		settings.setValue("id", server.id);
		settings.setValue("name", server.name);
		settings.setValue("display_url", server.displayUrl);
		settings.setValue("data_download_url", server.dataDownloadUrl);
		settings.setValue("data_download_filename", server.dataDownloadFilename);
		settings.setValue("data_compressed_size", server.dataCompressedSize);
		settings.setValue("data_uncompressed_size", server.dataUncompressedSize);
		settings.setValue("client_download_url", server.clientDownloadUrl);
		settings.setValue("client_download_filename", server.clientDownloadFilename);
#if defined(Q_OS_WIN)
		settings.setValue("client_filename_windows", server.clientFilename);
#elif defined(Q_OS_MAC)
		settings.setValue("client_filename_osx", server.clientFilename);
#else
		settings.setValue("client_filename_linux", server.clientFilename);
#endif
		settings.setValue("comments", server.comments);

		settings.endGroup();
	}

	settings.beginGroup("profiles");
	settings.setValue("size", m_profiles.size());
	settings.setValue("default", m_defaultProfile);
	settings.endGroup();

	for(int i = 0; i < m_profiles.size(); ++i)
	{
		const CProfile &profile = m_profiles[i];

		settings.beginGroup(QString("profile_%1").arg(i));

		settings.setValue("id", profile.id);
		settings.setValue("name", profile.name);
		settings.setValue("account", profile.account);
		settings.setValue("server", profile.server);
		settings.setValue("executable", profile.executable);
		settings.setValue("arguments", profile.arguments);
		settings.setValue("comments", profile.comments);

		settings.endGroup();
	}

	return true;
}

CConfigFile* CConfigFile::getInstance()
{
	return s_instance;
}

int CConfigFile::getServersCount() const
{
	return m_servers.size();
}

const CServer& CConfigFile::getServer(int i) const
{
	if (i < 0) i = m_defaultServer;

	if (i >= m_servers.size()) return NoServer;

	return m_servers.at(i);
}

const CServer& CConfigFile::getServer(const QString &id) const
{
	for(int i = 0; i < m_servers.size(); ++i)
	{
		if (m_servers[i].id == id) return m_servers[i];
	}

	// default server
	return getServer();
}

int CConfigFile::getProfilesCount() const
{
	return m_profiles.size();
}

CProfile CConfigFile::getProfile(int i) const
{
	if (i < 0) i = m_defaultProfile;

	if (i >= m_profiles.size()) return NoProfile;

	return m_profiles.at(i);
}

void CConfigFile::setProfile(int i, const CProfile &profile)
{
	m_profiles[i] = profile;
}

int CConfigFile::addProfile(const CProfile &profile)
{
	m_profiles.append(profile);

	return m_profiles.size()-1;
}

void CConfigFile::removeProfile(int i)
{
	m_profiles.removeAt(i);

	// TODO: decalle all profiles and move files
}

bool CConfigFile::has64bitsOS()
{
	return QSysInfo::currentCpuArchitecture() == "x86_64";
}

int CConfigFile::getDefaultProfile() const
{
	return m_defaultProfile;
}

int CConfigFile::getDefaultServer() const
{
	return m_defaultServer;
}

bool CConfigFile::isRyzomInstallerConfigured() const
{
	return m_profiles.size() > 0;
}

QString CConfigFile::getInstallationDirectory() const
{
	return m_installationDirectory;
}

void CConfigFile::setInstallationDirectory(const QString &directory)
{
	m_installationDirectory = directory;
}

QString CConfigFile::getSrcServerDirectory() const
{
	return m_srcDirectory;
}

void CConfigFile::setSrcServerDirectory(const QString &directory)
{
	m_srcDirectory = directory;
}

QString CConfigFile::getProfileDirectory() const
{
	return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString CConfigFile::getSrcProfileDirectory() const
{
	if (QFile::exists(getSrcServerDirectory() + "/client.cfg")) return getSrcServerDirectory();

	return qFromUtf8(NLMISC::CPath::getApplicationDirectory("Ryzom"));
}

bool CConfigFile::use64BitsClient() const
{
	return m_use64BitsClient;
}

void CConfigFile::setUse64BitsClient(bool on)
{
	m_use64BitsClient = on;
}

QString CConfigFile::expandVariables(const QString &str)
{
	QString res = str;

	res.replace("$TIMESTAMP", QString::number(QDateTime::currentDateTime().toTime_t()));
	res.replace("$LANG", m_language);
	res.replace("$ARCH", getClientArch());

	return res;
}

QString CConfigFile::getClientArch() const
{
#if defined(Q_OS_WIN)
	return QString("win%1").arg(m_use64BitsClient ? 64:32);
#elif defined(Q_OS_MAC)
	// only 64 bits clients under OS X, becure there not any 32 bits OS X version anymore
	return "osx";
#else
	return QString("linux%1").arg(m_use64BitsClient ? 64:32);
#endif
}

QString CConfigFile::getCurrentDirectory()
{
	return QDir::current().absolutePath();
}

QString CConfigFile::getParentDirectory()
{
	QDir current = QDir::current();
	current.cdUp();
	return current.absolutePath();
}

QString CConfigFile::getApplicationDirectory()
{
	return QApplication::applicationDirPath();
}

QString CConfigFile::getOldInstallationDirectory()
{
	// HKEY_CURRENT_USER/SOFTWARE/Nevrax/RyzomInstall/InstallId=1917716796 (string)
#if defined(Q_OS_WIN)
	// NSIS previous official installer
#ifdef Q_OS_WIN64
	// use WOW6432Node in 64 bits (64 bits OS and 64 bits Installer) because Ryzom old installer was in 32 bits
	QSettings settings("HKEY_LOCAL_MACHINE\\Software\\WOW6432Node\\Nevrax\\Ryzom", QSettings::NativeFormat);
#else
	QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Nevrax\\Ryzom", QSettings::NativeFormat);
#endif

	if (settings.contains("Ryzom Install Path"))
	{
		return QDir::fromNativeSeparators(settings.value("Ryzom Install Path").toString());
	}

	// check default directory if registry key not found
	return CConfigFile::has64bitsOS() ? "C:/Program Files (x86)/Ryzom":"C:/Program Files/Ryzom";
#elif defined(Q_OS_MAC)
	return "/Applications/Ryzom.app";
#else
	return QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.ryzom";
#endif
}

QString CConfigFile::getNewInstallationDirectory()
{
	return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
}

bool CConfigFile::isRyzomInstalledIn(const QString &directory) const
{
	// check client and data
	return isRyzomClientInstalledIn(directory) && areRyzomDataInstalledIn(directory);
}

bool CConfigFile::areRyzomDataInstalledIn(const QString &directory) const
{
	QDir dir(directory);

	// directory doesn't exist
	if (!dir.exists()) return false;

	if (!dir.cd("data") && dir.exists()) return false;

	// at least 200 BNP in data directory
	if (dir.entryList(QStringList() << "*.bnp", QDir::Files).size() < 200) return false;

	// TODO: more checks

	return true;
}

bool CConfigFile::isRyzomClientInstalledIn(const QString &directory) const
{
	QDir dir(directory);

	// directory doesn't exist
	if (!dir.exists()) return false;

	// client_default.cfg doesn't exist
	if (!dir.exists("client_default.cfg")) return false;

	if (!dir.exists(getServer().clientFilename)) return false;

	// TODO: more checks

	return true;
}

QString CConfigFile::getClientFullPath() const
{
	QString path = getProfile().executable;

	if (!path.isEmpty()) return path;

	return getInstallationDirectory() + "/" + getServer().id + "/" + getServer().clientFilename;
}

QString CConfigFile::getSrcServerClientBNPFullPath() const
{
	return QString("%1/unpack/exedll_%2.bnp").arg(getSrcServerDirectory()).arg(getClientArch());
}

CConfigFile::InstallationStep CConfigFile::getNextStep() const
{
	// get last used profile
	const CProfile &profile = getProfile();

	// get server used by it or default server
	CServer server = getServer(profile.server);

	// no or wrong profile
	if (server.id.isEmpty())
	{
		// get last used server
		server = getServer();
	}

	// no or wrong server
	if (server.id.isEmpty())
	{
		// get first server
		server = getServer(0);
	}

	// no server defined, shouldn't happen
	if (server.id.isEmpty())
	{
		return DisplayNoServerError;
	}

	// only show wizard if installation directory undefined
	if (getInstallationDirectory().isEmpty())
	{
		return ShowWizard;
	}

	QString serverDirectory = getInstallationDirectory() + "/" + server.id;

	if (getSrcServerDirectory().isEmpty())
	{
		// user decided to download files

		// downloaded files are kept in server directory
		QString dataFile = getInstallationDirectory() + "/" + server.dataDownloadFilename;
		QString clientFile = getInstallationDirectory() + "/" + server.clientDownloadFilename;

		// data are not copied
		if (!areRyzomDataInstalledIn(serverDirectory))
		{
			// when file is not finished, it has .part extension
			if (!QFile::exists(dataFile))
			{
				return DownloadData;
			}

			return ExtractDownloadedData;
		}

		if (!isRyzomClientInstalledIn(serverDirectory))
		{
			// when file is not finished, it has .part extension
			if (!QFile::exists(clientFile))
			{
				return DownloadClient;
			}

			return ExtractDownloadedClient;
		}
	}
	else
	{
		// user decided to copy files

		// selected directory contains Ryzom files (shouldn't fail)
		if (!areRyzomDataInstalledIn(getSrcServerDirectory()))
		{
			return ShowWizard;
		}

		// data are not copied
		if (!areRyzomDataInstalledIn(serverDirectory))
		{
			return CopyServerFiles;
		}

		// client is not extracted from BNP
		if (!isRyzomClientInstalledIn(serverDirectory))
		{
			if (QFile::exists(getSrcServerClientBNPFullPath()))
			{
				return ExtractBnpClient;
			}
			else
			{
				QString clientFile = getInstallationDirectory() + "/" + server.clientDownloadFilename;

				// when file is not finished, it has .part extension
				if (!QFile::exists(clientFile))
				{
					return DownloadClient;
				}

				return ExtractDownloadedClient;
			}
		}
	}

	// no default profile
	if (profile.id < 0)
	{
		return CreateProfile;
	}

	// migration profile
	if (!getSrcServerDirectory().isEmpty() && QFile::exists(getSrcProfileDirectory() + "/client.cfg") && !QFile::exists(QString("%1/%2/client.cfg").arg(getProfileDirectory()).arg(profile.name)))
	{
		return CopyProfileFiles;
	}

	if (!QFile::exists(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/Ryzom.lnk"))
	{
		// TODO: check they point to getClientFullPath()
		return CreateShortcuts;
	}

	return Done;
}
