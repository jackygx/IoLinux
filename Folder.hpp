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

#ifndef __FOLDER_HPP__
#define __FOLDER_HPP__

#include <IIo.hpp>
#include "Dirent.hpp"

DEFINE_CLASS(Folder);

class CFolder :
	public IFolder
{
public:
	CFolder(const CConstStringPtr &path);

	virtual IFolderPtr CreateFolder(const CConstStringPtr &name);
	virtual IFilePtr CreateFile(const CConstStringPtr &name);
	virtual void Delete(const CConstStringPtr &name);
	virtual IDirentPtr ToDirent(void) const;

private:
	virtual void FirstEntity(void);
	virtual void NextEntity(void);
	virtual bool IsLastEntity(void) const;
	virtual IDirentPtr GetEntity(void);
	virtual void SortEntity(IFolder::SortType type);

private:
	typedef CList<CDirentPtr> ChildList;
	ChildList::IteratorPtr InitEntity(void);

private:
	CConstStringPtr mPath;
	ChildList mChildList;
	ChildList::IteratorPtr mChildListIter;
};

#endif /* __FOLDER_HPP__ */

