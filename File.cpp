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

#ifdef DEBUG_FILE_MMAP
#define MMAP_DEBUG(...) TRACE_INFO(__VA_ARGS__)
#else
#define MMAP_DEBUG(...)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

#include "File.hpp"

CFile::CFile(const CConstStringPtr &path, IFile::FilePerm perm) :
	mFd(-1)
{
	const char *_path = path->ToCStr();

	if (IFile::RO == perm) {
		mFd = open(_path, O_RDONLY);

		if (mFd < 0) {
			throw E("Fail to open: ", path, ", err: ", DEC(errno));
		}
	} else {
		mFd = open(_path, O_RDWR);

		if (mFd < 0) {
			/* Create it */
			if (errno == 2) {
				mFd = creat(_path, 0644);

				if (mFd < 0) {
					throw E("Fail to create: ", path, ", err: ", DEC(errno));
				}
			} else {
				throw E("Fail to open: ", path, ", err: ", DEC(errno));
			}
		}
	}
}

CFile::~CFile(void)
{
	if (mFd >= 0) {
		close(mFd);
		mFd = -1;
	}
}

CConstStringPtr CFile::Map(void) const
{
	struct stat stat;
	if (fstat(mFd, &stat) < 0) {
		throw E("Fail to get stat, err: ",DEC(errno));
	}

	int size = (uint32_t)stat.st_size;
	MMAP_DEBUG("File size: ", DEC(size), EOS);

	char *buf = (char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, mFd, 0);
	if (NULL == buf) {
		throw E("Fail to mmap, err: ",DEC(errno));
	}

	MMAP_DEBUG("Map file to: ", HEX(buf), ", fd: ", DEC(fd),
			   ", size: ", DEC(size), EOS);

	const CMemPtr mem(buf, [size](char *buf) {
		MMAP_DEBUG("Unmap file from: ", HEX(buf),
				   ", fd: ", DEC(fd), ", size: ", DEC(size), EOS);
		munmap(buf, size);
	});

	return CConstStringPtr(mem, size);
}

IWriterPtr CFile::Write(const CConstStringPtr &data,
						const OnWriteFailFn &onFail,
						const OnWrittenFn &onWritten)
{
	uint32_t size = data->GetSize();
	if (size != ::write(mFd, data->ToCStr(), size)) {
		if (onFail) {
			onFail();
		}
	} else {
		if (onWritten) {
			onWritten();
		}
	}

	return Share();
}

IReaderPtr CFile::Read(const OnReadFn &onRead,
					   const OnReadFailFn &onFail)
{
	int size;
	uint32_t capacity;

	do {
		CStringPtr data;
		capacity = data->GetCapacity();
		size = ::read(mFd, data, capacity);

		if (size < 0) {
			if (onFail) {
				onFail();
			}
			return Share();
		} else {
			data->SetSize(size);
			onRead(data);
		}
	} while (size == (int)capacity);

	return Share();
}

