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

#ifndef __DIRENT_HPP__
#define __DIRENT_HPP__

#include <IIo.hpp>

DEFINE_CLASS(Dirent);

class CDirent :
	public IDirent
{
public:
	CDirent(const CConstStringPtr &path);
	CDirent(const CConstStringPtr &baseName,
			const CConstStringPtr &dirName);

private:
	IDirent::CDirInfoPtr GetDirInfo(const CConstStringPtr &path);

private:
	virtual IFolderPtr _ToFolder(void) const;
	virtual IFilePtr _ToFile(void) const;
};

#endif /* __DIRENT_HPP__ */

