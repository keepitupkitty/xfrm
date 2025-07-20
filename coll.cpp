#include <gtest/gtest.h>
#include <string.h>
#include <wchar.h>

extern "C" {
  size_t icu_strxfrm(char* dest, const char* src, std::size_t count);
  size_t icu_wcsxfrm(wchar_t *dest, const wchar_t* src, std::size_t count);
}

struct coll_data {
	const char* a;
	const char* b;
	int result;
};

struct coll_wide_data {
	const wchar_t* a;
	const wchar_t* b;
	int result;
};


static int sign (int a)
{
	if (a < 0)
		return -1;
	if (a > 0)
		return 1;
	return 0;
}

static void test_strxfrm(const coll_data* coll) {
	for (unsigned int i = 0; coll[i].a != NULL; ++i) {
		int result = 0;
		char sortKeyA[100], sortKeyB[100];
		icu_strxfrm(sortKeyA, coll[i].a, 100);
		icu_strxfrm(sortKeyB, coll[i].b, 100);
		result = sign(strcmp(sortKeyA, sortKeyB));
		ASSERT_EQ(result, coll[i].result);
	}
}

static void test_wcsxfrm(const coll_wide_data* coll) {
	for (unsigned int i = 0; coll[i].a != NULL; ++i) {
		int result = 0;
		wchar_t sortKeyA[100], sortKeyB[100];
		icu_wcsxfrm(sortKeyA, coll[i].a, 100);
		icu_wcsxfrm(sortKeyB, coll[i].b, 100);
		result = sign(wcscmp(sortKeyA, sortKeyB));
		ASSERT_EQ(result, coll[i].result);
	}
}

TEST(strxfrm, unicode) {
	const coll_data coll[] = {
		{ "", "", 0 },
		{ "test", "test", 0 },
		{ "tester", "test", 1 },
		{ "tEst", "test", 1 },
		{ "test", "tester", -1 },
		{ "täst", "täst", 0 },
		{ "tast", "täst", -1 },
		{ "tbst", "täst", 1 },
		{ "tbst", "tæst", 1 },
		{ "täst", "tÄst", -1 },
		{ "tBst", "tÄst", 1 },
		{ "tBst", "täst", 1 },
		{ "taest", "tæst", -1 },
		{ "tafst", "tæst", 1 },
		{ "taa", "täa", -1 },
		{ "tab", "täb", -1 },
		{ "tad", "täd", -1 },
		{ "tae", "täe", -1 },
		{ "taf", "täf", -1 },
		{ "cote", "coté", -1 },
		{ "coté", "côte", -1 },
		{ "côte", "côté", -1 },
		{ NULL, NULL, 0 },
	};

	test_strxfrm(coll);
}

TEST(wcsxfrm, unicode) {
	const coll_wide_data coll[] = {
		{ L"", L"", 0 },
		{ L"test", L"test", 0 },
		{ L"tester", L"test", 1 },
		{ L"tEst", L"test", 1 },
		{ L"test", L"tester", -1 },
		{ L"täst", L"täst", 0 },
		{ L"tast", L"täst", -1 },
		{ L"tbst", L"täst", 1 },
		{ L"tbst", L"tæst", 1 },
		{ L"täst", L"tÄst", -1 },
		{ L"tBst", L"tÄst", 1 },
		{ L"tBst", L"täst", 1 },
		{ L"taest", L"tæst", -1 },
		{ L"tafst", L"tæst", 1 },
		{ L"taa", L"täa", -1 },
		{ L"tab", L"täb", -1 },
		{ L"tad", L"täd", -1 },
		{ L"tae", L"täe", -1 },
		{ L"taf", L"täf", -1 },
		{ L"cote", L"coté", -1 },
		{ L"coté", L"côte", -1 },
		{ L"côte", L"côté", -1 },
		{ NULL, NULL, 0 },
	};

	test_wcsxfrm(coll);
}
