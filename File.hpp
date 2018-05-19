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

#ifndef __FILE_HPP__
#define __FILE_HPP__

#include <IIo.hpp>

DEFINE_CLASS(File);

class CFile :
	public IFile,
	public CEnableSharedPtr<CFile>
{
public:
	CFile(const CConstStringPtr &path,
		  IFile::FilePerm perm = IFile::RW);
	virtual ~CFile(void);

	virtual CConstStringPtr Map(void) const;

	virtual IWriterPtr Write(const CConstStringPtr &data,
							 const OnWriteFailFn &onFail,
							 const OnWrittenFn &onWritten);

	virtual IReaderPtr Read(const OnReadFn &onRead,
							const OnReadFailFn &onFail);

private:
	int mFd;
};

#endif /* __FILE_HPP__ */

