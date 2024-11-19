#include "pch.h"
#include "CppUnitTest.h"
#include "TreeExt.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace My00classexcercises2024libTests
{
	struct GameObjectMock : public TreeExt<GameObjectMock> {
		std::string name;
	};

	TEST_CLASS(TreeExtTest)
	{
	public:

		TEST_METHOD(TestHierarchyWorksFine)
		{
			//Arrange
			GameObjectMock tree;
			tree.name = "root";
			
			//Act
			auto& child1 = tree.emplaceChild();
			child1.name = "child1";

			auto& child11 = child1.emplaceChild();
			child11.name = "child11";

			auto& child2 = tree.emplaceChild();
			child2.name = "child2";

			auto& child21 = child2.emplaceChild();
			child21.name = "child21";
			
			//Assert
			Assert::AreEqual(tree.children().size(), static_cast<size_t>(2));
			Assert::AreEqual(tree.children().front().children().size(), static_cast<size_t>(1));
			Assert::AreEqual(tree.children().back().children().size(), static_cast<size_t>(1));

			Assert::AreEqual(tree.name, std::string("root"));
			Assert::AreEqual(tree.children().front().name, std::string("child1"));
			Assert::AreEqual(tree.children().back().name, std::string("child2"));
		}
	};
}
