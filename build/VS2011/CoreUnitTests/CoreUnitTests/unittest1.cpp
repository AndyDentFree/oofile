#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CoreUnitTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
      int i = 4;
			Assert::IsNotNull(&i);
      Assert::AreEqual(2 + 2, i);
      Assert::AreEqual(2 + 2, i, L"Optional message needs the L prefix to make a wchar literal");
		}

	};
}