/*
 * Copyright (c) 2018 Guo Xiang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Dirent.hpp"
#include "Folder.hpp"
#include "File.hpp"

CDirent::CDirent(const CConstStringPtr &path) :
	IDirent(GetDirInfo(path))
{
	/* Does nothing */
}

CDirent::CDirent(const CConstStringPtr &baseName,
				 const CConstStringPtr &dirName) :
	IDirent(GetDirInfo(CConstStringPtr(baseName, "/", dirName)))
{
	/* Does nothing */
}

IDirent::CDirInfoPtr CDirent::GetDirInfo(const CConstStringPtr &path)
{
	struct stat stat;

	if (0 != ::stat(path->ToCStr(), &stat)) {
		throw E("Cannot get dirent stat for ", path, ", error: ", DEC(errno));
	}

	return IDirent::CDirInfoPtr(
		path, 
		(uint64_t)stat.st_size,
		S_ISDIR(stat.st_mode) ? IDirent::FOLDER : IDirent::FILE);
}

IFolderPtr CDirent::_ToFolder(void) const
{
	return CFolderPtr(mInfo->GetPath());
}

IFilePtr CDirent::_ToFile(void) const
{
	return CFilePtr(mInfo->GetPath());
}

