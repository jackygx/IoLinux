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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

#include "Folder.hpp"
#include "Dirent.hpp"
#include "File.hpp"

//#define DEBUG_FOLDER

#ifdef DEBUG_FOLDER
#define FOLDER_DEBUG(...) TRACE_INFO(__VA_ARGS__, EOS)
#else
#define FOLDER_DEBUG(...)
#endif

CFolder::CFolder(const CConstStringPtr &path) :
	mPath(path),
	mChildListIter(InitEntity())
{
	/* Does nothing */
}

IFolderPtr CFolder::CreateFolder(const CConstStringPtr &name)
{
	FOLDER_DEBUG("Create a folder: ", name);

	return Find(name)->Then([&](const IDirentPtr &dir) {
		FOLDER_DEBUG("Folder ", name, " exists");

		/* Folder exists */
		if (dir->IsFolder()) {
			return dir->ToFolder();
		/* It is a file. Overwritten? Throw error? */
		} else {
			throw E("Cannot create folder. File exist: ", name);
		}
	})->Catch([&](void) {
		CConstStringPtr path(mPath, "/", name);

		FOLDER_DEBUG("Creating folder: ", path);

		if (0 != mkdir(path->ToCStr(), 0755)) {
			throw E("Fail to mkdir: ", path, ", err: ", DEC(errno));
		}

		CFolderPtr ret(path);
		mChildListIter = InitEntity();

		return ret;
	});
}

IFilePtr CFolder::CreateFile(const CConstStringPtr &name)
{
	FOLDER_DEBUG("Create a file: ", name);

	return Find(name)->Then([&](const IDirentPtr &dir) {
		FOLDER_DEBUG("File ", name, " exists");

		/* File exists */
		if (!dir->IsFolder()) {
			return dir->ToFile();
		/* It is a file. Overwritten? Throw error? */
		} else {
			throw E("Cannot create file. Folder exist: ", name);
		}
	})->Catch([&](void) {
		CConstStringPtr path(mPath, "/", name);

		FOLDER_DEBUG("Creating File ", path);
		CFilePtr ret(path);

		mChildListIter = InitEntity();

		return ret;
	});
}

void CFolder::Delete(const CConstStringPtr &name)
{
	FOLDER_DEBUG("Delete: ", name);

	Find(name)->Then([&](const IDirentPtr &dir) {
		CConstStringPtr path(mPath, "/", name);

		if (dir->IsFolder()) {
			FOLDER_DEBUG("Deleteing folder: ", path);
			rmdir(path->ToCStr());
		} else {
			FOLDER_DEBUG("Deleteing file: ", path);
			unlink(path->ToCStr());
		}

		mChildListIter = InitEntity();
	});
}

IDirentPtr CFolder::ToDirent(void) const
{
	return CDirentPtr(mPath);
}

void CFolder::FirstEntity(void)
{
	FOLDER_DEBUG("Goto 1st Dirent");
	mChildListIter->First();
}

void CFolder::NextEntity(void)
{
	FOLDER_DEBUG("Goto next Dirent");
	mChildListIter->Next();
}

bool CFolder::IsLastEntity(void) const
{
	FOLDER_DEBUG(mChildListIter->End() ? "" : "Not", " last Dirent");
	return mChildListIter->End();
}

IDirentPtr CFolder::GetEntity(void)
{
	return mChildListIter->Get([&](const CDirentPtr &child) {
		return child;
	});
}

CFolder::ChildList::IteratorPtr CFolder::InitEntity(void)
{
	DIR *dir = opendir(mPath->ToCStr());
	if (NULL == dir) {
		throw E("Fail to open dir: ", mPath, " error: ", DEC(errno));
	}

	struct dirent *dirent;

	while (NULL != (dirent = readdir(dir))) {
		/* Bypass . and .. */
		if (('.' == dirent->d_name[0]) &&
			(('\0' == dirent->d_name[1]) ||
			 (('.' == dirent->d_name[1]) &&
			  ('\0' == dirent->d_name[2])))) {
			continue;
		}

		mChildList.PushBack(CDirentPtr(mPath, CConstStringPtr(dirent->d_name)));
	}

	closedir(dir);
	return mChildList.Iter();
}

void CFolder::SortEntity(IFolder::SortType type)
{
	switch (type) {
	case IFolder::BY_NAME:
	default:
		FOLDER_DEBUG("Sort entity by name");
		mChildListIter->Sort([&](const CDirentPtr &left, const CDirentPtr &right) {
			return left->GetBaseName() < right->GetBaseName();
		});
		break;

	case IFolder::BY_SIZE:
		FOLDER_DEBUG("Sort entity by size");
		mChildListIter->Sort([&](const CDirentPtr &left, const CDirentPtr &right) {
			return left->GetSize() < right->GetSize();
		});
		break;
	}
}

