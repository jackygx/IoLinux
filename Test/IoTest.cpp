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

#include <EasyCpp.hpp>
#include <Test/Test.hpp>
#include <Io.hpp>

static const char *gFolderOutByName[] = {
	"1",
	"1.1.txt",
	"1.2.txt",
	"1.3.txt",
	"2",
	"2.1.txt",
	"2.2.txt",
	"2.3.txt",
	"3",
	"3.1.txt",
	"3.2.txt",
	"3.3.txt",
};

static inline void PrintFolderByName(const IFolderPtr &folder, uint32_t &i)
{
	folder->Sort(IFolder::BY_NAME)->ForEach([&](const IDirentPtr &dir) {
		CHECK(i < ARRAY_SIZE(gFolderOutByName),
			  "More dirent than expected. expect: %d, actually: %d",
			  ARRAY_SIZE(gFolderOutByName), i);

		CHECK(dir->GetBaseName() == gFolderOutByName[i],
			  "Dirent mismatch: expect: %s, actually: %s",
			  gFolderOutByName[i], dir->GetBaseName()->ToCStr());

		++i;

		if (dir->IsFolder()) {
			PrintFolderByName(dir->ToFolder(), i);
		}
	});
}

static inline void PrintFolderBySize(const IFolderPtr &folder, uint32_t &i)
{
	uint32_t size = 0;

	folder->Sort(IFolder::BY_SIZE)->ForEach([&](const IDirentPtr &dir) {
		CHECK(dir->GetSize() >= size,
			  "Sort by size fail, last size: %d, current size: %lu",
			  size, dir->GetSize());

		size = dir->GetSize();

		++i;

		if (dir->IsFolder()) {
			PrintFolderBySize(dir->ToFolder(), i);
		}
	});
}

static void DirentTest(const CConstStringPtr &app)
{
	auto dir(app->Split("/")->Reverse()->First([](const CConstStringPtr &) {
		/* Does nothing */
	})->Rest([](const CConstStringPtr &path) {
		return CreateDirent(path ?
							CConstStringPtr(path, "/DirentTestFolder") :
							CConstStringPtr("./DirentTestFolder"));
	}));

	TEST_STEP(1, 1, "Prepare", "Input should be a folder") {
		CHECK(dir->IsFolder(), "Input is not a folder");
	}

	TEST_STEP(1, 2, "Test sort folder by name", "Should work") {
		uint32_t i = 0;

		PrintFolderByName(dir->ToFolder(), i);

		CHECK(i == ARRAY_SIZE(gFolderOutByName),
			  "Not all dirents are iteratored. Expect: %d, actually: %d",
			  ARRAY_SIZE(gFolderOutByName), i);
	}

	TEST_STEP(1, 3, "Test sort folder by size", "Should work") {
		uint32_t i = 0;

		PrintFolderBySize(dir->ToFolder(), i);

		CHECK(i == ARRAY_SIZE(gFolderOutByName),
			  "Not all dirents are iteratored. Expect: %d, actually: %d",
			  ARRAY_SIZE(gFolderOutByName), i);
	}
}

void TEST_CASE_ENTRY(void)
{
	CConstStringPtr path(IException::GetSymbol());
	DO_TEST(DirentTest, path);
}

