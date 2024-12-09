#include "pch.h"
#include "CppUnitTest.h"
#include "TreeExt.h"
#include <sstream>
using namespace std;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace My00classexcercises2024libTests
{

	stringstream ss;

	class GameObjectMock : public TreeExt<GameObjectMock> {
	public:
		std::string name;
		explicit GameObjectMock(const std::string& name) : name(name) { ss << "CTR(" << name << ')' << endl; }
		GameObjectMock(const GameObjectMock& other) : name(other.name) { ss << "COPY(" << name << ')' << endl; }
		GameObjectMock(GameObjectMock&& other) noexcept : name(std::move(other.name)) { ss << "MOVE(" << name << ')' << endl; }
		GameObjectMock& operator=(const GameObjectMock& other) { name = other.name; ss << "ASSIGN(" << name << ')' << endl; return *this; }
		GameObjectMock& operator=(GameObjectMock&& other) noexcept { name = std::move(other.name); ss << "MOVEASSIGN(" << name << ')' << endl; return *this; }
		virtual ~GameObjectMock() { ss << "DTR(" << name << ')' << endl; }
	};

	TEST_CLASS(TreeExtTest)
	{
	public:

		TEST_METHOD_INITIALIZE(Init)
		{
			ss.str("");
		}

		TEST_METHOD(TestHierarchyWorksFine)
		{
			//Arrange
			GameObjectMock tree("root");
			
			//Act
			auto& child1 = tree.emplaceChild("child1");
			auto& child11 = child1.emplaceChild("child11");
			auto& child111 = child11.emplaceChild("child111");
			auto& child2 = tree.emplaceChild("child2");
			auto& child21 = child2.emplaceChild("child21");
			
			//Assert
			Assert::AreEqual(tree.children().size(), static_cast<size_t>(2));
			Assert::AreEqual(tree.children().front().children().size(), static_cast<size_t>(1));
			Assert::AreEqual(tree.children().back().children().size(), static_cast<size_t>(1));

			Assert::AreEqual(tree.name, std::string("root"));
			Assert::AreEqual(tree.children().front().name, std::string("child1"));
			Assert::AreEqual(tree.children().back().name, std::string("child2"));
		}

		TEST_METHOD(TestRemove)
		{
			//Arrange
			GameObjectMock tree("root");
			auto& child1 = tree.emplaceChild("child1");
			auto& child11 = child1.emplaceChild("child11");
			auto& child111 = child11.emplaceChild("child111");
			auto& child2 = tree.emplaceChild("child2");

			//Act
			child1.removeChild(child11);

			//Assert
			Assert::AreEqual(child1.children().size(), static_cast<size_t>(0));
			Assert::AreEqual(ss.str(), string("CTR(root)\nCTR(child1)\nCTR(child11)\nCTR(child111)\nCTR(child2)\nDTR(child11)\nDTR(child111)\n"));
		}

		TEST_METHOD(TestMove)
		{
			//Arrange
			GameObjectMock tree("root");
			auto& child1 = tree.emplaceChild("child1");
			auto& child11 = child1.emplaceChild("child11");
			auto& child111 = child11.emplaceChild("child111");
			auto& child2 = tree.emplaceChild("child2");

			//Act
			child11.moveTo(child2);

			//Assert
			Assert::AreEqual(child1.children().size(), static_cast<size_t>(0));
			Assert::AreEqual(ss.str(), string("CTR(root)\nCTR(child1)\nCTR(child11)\nCTR(child111)\nCTR(child2)\n"));
		}
	};
}
