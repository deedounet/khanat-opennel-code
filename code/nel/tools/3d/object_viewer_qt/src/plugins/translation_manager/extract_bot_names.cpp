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

#include "extract_bot_names.h"


static bool	RemoveOlds = false;




namespace Plugin 
{

TCreatureInfo *ExtractBotNames::getCreature(const std::string &sheetName)
{
	CSheetId id(sheetName+".creature");

	if (Creatures.find(id) != Creatures.end())
		return &(Creatures.find(id)->second);
	else
		return NULL;
}

string ExtractBotNames::cleanupName(const std::string &name)
{
	string ret;

	for (uint i=0; i<name.size(); ++i)
	{
		if (name[i] != ' ')
			ret += name[i];
		else
			ret += '_';
	}

	return ret;
}

ucstring ExtractBotNames::cleanupUcName(const ucstring &name)
{
	ucstring ret;

	for (uint i=0; i<name.size(); ++i)
	{
		if (name[i] != ' ')
			ret += name[i];
		else
			ret += '_';
	}

	return ret;
}


/*
	Removes first and last '$'
*/
ucstring ExtractBotNames::makeGroupName(const ucstring & translationName)
{
	ucstring ret = translationName;
	if (ret.size() >= 2)
	{
		if ( *ret.begin() == ucchar('$'))
		{
			ret=ret.substr(1);
		}
		if ( *ret.rbegin() == ucchar('$'))
		{
			ret = ret.substr(0, ret.size()-1);
		}
	}
	ret = cleanupUcName(ret);
	return ret;	
}





set<string> ExtractBotNames::getGenericNames()
{
    return GenericNames;
}

map<string, TEntryInfo> ExtractBotNames::getSimpleNames()
{
    return SimpleNames;
}

void ExtractBotNames::cleanSimpleNames()
{       
    SimpleNames.clear();
}

void ExtractBotNames::cleanGenericNames()
{
    GenericNames.clear();
}

string	ExtractBotNames::removeAndStoreFunction(const std::string &fullName)
{
	string::size_type pos = fullName.find("$");
	if (pos == string::npos)
		return fullName;
	else
	{
		// extract and store the function name
		string ret;

		ret = fullName.substr(0, pos);
		string::size_type pos2 = fullName.find("$", pos+1);

		string fct = fullName.substr(pos+1, pos2-(pos+1));

		ret += fullName.substr(pos2+1);

		if (Functions.find(fct) == Functions.end())
		{
			nldebug("Adding function '%s'", fct.c_str());
			Functions.insert(fct);
		}

		return ret;
	}
}


void ExtractBotNames::addGenericName(const std::string &name, const std::string &sheetName)
{
	TCreatureInfo *c = getCreature(sheetName);
	if (!c || c->ForceSheetName || !c->DisplayName)
		return;
	
	if (SimpleNames.find(name) != SimpleNames.end())
	{
		nldebug("Name '%s' is now a generic name", name.c_str());
		GenericNames.insert(name);
		SimpleNames.erase(name);

	}
	else if (GenericNames.find(name) == GenericNames.end())
	{
		nldebug("Adding generic name '%s'", name.c_str());
		GenericNames.insert(name);
	}
}

void ExtractBotNames::addSimpleName(const std::string &name, const std::string &sheetName)
{
	TCreatureInfo *c = getCreature(sheetName);
	if (!c || c->ForceSheetName || !c->DisplayName)
		return;

	if (SimpleNames.find(name) != SimpleNames.end())
	{
		addGenericName(name, sheetName);
	}
	else if (GenericNames.find(name) != GenericNames.end())
	{
		return;
	}
	else
	{
		nldebug("Adding simple name '%s'", name.c_str());
		
		TEntryInfo ei;
		ei.SheetName = sheetName;

		SimpleNames.insert(make_pair(name, ei));
	}
}

void ExtractBotNames::setRequiredSettings(list<string> filters, string level_design_path)
{
	for (std::list<string>::iterator it = filters.begin(); it != filters.end(); ++it)
	{
		Filters.push_back(*it);
	}

	//-------------------------------------------------------------------
	// init the sheets
	CSheetId::init(false);
	const string PACKED_SHEETS_NAME = "bin/translation_tools_creature.packed_sheets";
	loadForm("creature", PACKED_SHEETS_NAME, Creatures, false, false);

	if (Creatures.empty())
	{
                loadForm("creature", PACKED_SHEETS_NAME, Creatures, true);
	}
  
}

void ExtractBotNames::extractBotNamesFromPrimitives(CLigoConfig ligoConfig)
{

	//-------------------------------------------------------------------
	// ok, ready for the real work,
	// first, read the primitives files and parse the primitives
	vector<string>	files;
	CPath::getFileList("primitive", files);


	for (uint i=0; i<files.size(); ++i)
	{
		string pathName = files[i];
		pathName = CPath::lookup(pathName);

                
                
		// check filters
		uint j=0;
		for (j=0; j<Filters.size(); ++j)
		{
			if (pathName.find(Filters[j]) != string::npos)
				break;
		}
		if (j != Filters.size())
			// skip this file
			continue;

		nlinfo("Loading file '%s'...", CFile::getFilename(pathName).c_str());
		
		CPrimitives primDoc;
		CPrimitiveContext::instance().CurrentPrimitive = &primDoc;
		loadXmlPrimitiveFile(primDoc, pathName, ligoConfig);

		// now parse the file

		// look for group template
		{
			TPrimitiveClassPredicate pred("group_template_npc");
			TPrimitiveSet result;

			CPrimitiveSet<TPrimitiveClassPredicate> ps;
			ps.buildSet(primDoc.RootNode, pred, result);

			for (uint i=0; i<result.size(); ++i)
			{
				string name;
				string countStr;
				string sheetStr;
				result[i]->getPropertyByName("name", name);
				result[i]->getPropertyByName("count", countStr);
				result[i]->getPropertyByName("bot_sheet_look", sheetStr);

				uint32 count;
				NLMISC::fromString(countStr, count);

				if (count != 0)
				{
					if (sheetStr.empty())
					{
						nlwarning("In '%s', empty sheet !", buildPrimPath(result[i]).c_str());
					}
					else
					{
						addGenericName(removeAndStoreFunction(name), sheetStr);
					}
				}
			}
		}
		// look for bot template
		{
			TPrimitiveClassPredicate pred("bot_template_npc");
			TPrimitiveSet result;

			CPrimitiveSet<TPrimitiveClassPredicate> ps;
			ps.buildSet(primDoc.RootNode, pred, result);

			for (uint i=0; i<result.size(); ++i)
			{
				string name;
				string sheetStr;
				result[i]->getPropertyByName("name", name);
				result[i]->getPropertyByName("sheet_look", sheetStr);

				if (sheetStr.empty())
				{
					// take the sheet in the parent
					result[i]->getParent()->getPropertyByName("bot_sheet_look", sheetStr);
				}

				if (sheetStr.empty())
				{
					nlwarning("In '%s', empty sheet !", buildPrimPath(result[i]).c_str());
				}
				else
				{
					addGenericName(removeAndStoreFunction(name), sheetStr);
				}
			}
		}
		// look for npc_group 
		{
			TPrimitiveClassPredicate pred("npc_group");
			TPrimitiveSet result;

			CPrimitiveSet<TPrimitiveClassPredicate> ps;
			ps.buildSet(primDoc.RootNode, pred, result);

			for (uint i=0; i<result.size(); ++i)
			{
				string name;
				string countStr;
				string sheetStr;
				result[i]->getPropertyByName("name", name);
				result[i]->getPropertyByName("count", countStr);
				result[i]->getPropertyByName("bot_sheet_client", sheetStr);

				uint32 count;
				NLMISC::fromString(countStr, count);

				if (count > 0 && sheetStr.empty())
				{
					nlwarning("In '%s', empty sheet !", buildPrimPath(result[i]).c_str());
				}
				else
				{
					if (count == 1)
					{
						addSimpleName(removeAndStoreFunction(name), sheetStr);
					}
					else if (count > 1)
					{
						addGenericName(removeAndStoreFunction(name), sheetStr);
					}
				}
			}
		}
		// look for bot 
		{
			TPrimitiveClassPredicate pred("npc_bot");
			TPrimitiveSet result;

			CPrimitiveSet<TPrimitiveClassPredicate> ps;
			ps.buildSet(primDoc.RootNode, pred, result);

			for (uint i=0; i<result.size(); ++i)
			{
				string name;
				string sheetStr;
				result[i]->getPropertyByName("name", name);
				result[i]->getPropertyByName("sheet_client", sheetStr);

				if (sheetStr.empty())
				{
					// take the sheet in the parent
					result[i]->getParent()->getPropertyByName("bot_sheet_client", sheetStr);
				}

				if (sheetStr.empty())
				{
					nlwarning("In '%s', empty sheet !", buildPrimPath(result[i]).c_str());
				}
				else
				{
					addSimpleName(removeAndStoreFunction(name), sheetStr);
				}
			}
		}
	}       
}
  
}