#include "pch.h"
#include "CppUnitTest.h"

#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\StringHash.h"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\StringHash.cpp"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\Merchant.h"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\Merchant.cpp"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\Platform.h"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\Platform.cpp"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\TCProcessor.h"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\TCProcessor.cpp"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\Vital\Vital.h"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\Vital\Vital.cpp"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\Vital\TSYSCofInfo.h"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\Vital\TSYSCofInfo.cpp"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\Vital\CofInfo.h"
#include "..\..\..\Source\SpawnApp\SpawnApp\Processor\Vital\CofInfo.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TCProcessorTests
{
	TEST_CLASS(TCProcessorTests)
	{
	public:
		TCProcessor subject;

		TEST_METHOD(TestMethod_assingPlatform_IsTrue_ForPlatform_M)
		{
			bool result = subject.assignPlatform("M");
			Assert::IsTrue(result);
		}

		TEST_METHOD(TestMethod_runTransaction_IsC101_ForPlatform_M)
		{
			char transID[10] = { "123456" };
			subject.transID = transID;

			bool result = subject.assignPlatform("M");
			Assert::IsTrue(result);

			StringHash data;
			data.put("action", "sale");
			data.put("mediatype", "mc");
			data.put("store", "y");
			StringHash response;

			subject.runTransaction(data, response);
			Assert::AreEqual(1, response.size());

			const char* expectedValue = "C101";
			const char* actualValue = response.get("CMI");
			Assert::AreEqual(expectedValue, actualValue);
		}

		TEST_METHOD(TestMethod_runTransaction_IsC01_ForPlatform_M)
		{
			char transID[10] = { "123456" };
			subject.transID = transID;

			bool result = subject.assignPlatform("M");
			Assert::IsTrue(result);

			StringHash data;
			data.put("action", "sale");
			data.put("mediatype", "mc");
			data.put("store", "y");
			data.put("cardonfile", "y");
			StringHash response;

			subject.runTransaction(data, response);
			Assert::AreEqual(1, response.size());

			const char* expectedValue = "C101";
			const char* actualValue = response.get("CMI");
			Assert::AreEqual(expectedValue, actualValue);
		}
	};
}
