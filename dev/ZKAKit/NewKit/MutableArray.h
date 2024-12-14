/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */
#pragma once

#include <CompilerKit/CompilerKit.h>
#include <NewKit/Array.h>
#include <NewKit/Defines.h>

#define TRY_FIND_NODE(NAME, NODE)  \
	auto* NAME = NODE;             \
	while (NAME)                   \
	{                              \
		if (NAME->fIndex == Index) \
			return NAME->fVal;     \
		NAME = NAME->fNext;        \
	}

#define TRY_FIND_NODE2(NAME, NODE)     \
	auto* NAME = NODE;                 \
	while (NAME)                       \
	{                                  \
		if (NAME->fIndex == Index)     \
			return Ref<T>{NAME->fVal}; \
		NAME = NAME->fNext;            \
	}

#define TRY_REMOVE_NODE(NODE)          \
	if (NODE && NODE->fIndex == Index) \
	{                                  \
		NODE->fUsed	 = false;          \
		NODE->fIndex = 0;              \
                                       \
		return true;                   \
	}

// FIXME: this is a shitty algorithm, which is consumer hungry.
// Remove and occurences of that, and remove that class.
namespace Kernel
{
	template <typename T>
	class MutableArray;

	template <typename T, T _PlaceHolderValue>
	class NullableMutableArray;

	template <typename T>
	class MutableLinkedList
	{
	public:
		T		fVal;
		SizeT	fIndex{0};
		Boolean fUsed{false};

		MutableLinkedList* fPrev{nullptr};
		MutableLinkedList* fNext{nullptr};
	};

	template <typename T, T _PlaceHolderValue>
	class NullableMutableArray
	{
	public:
		// explicit this.
		explicit NullableMutableArray()
			: fFirstNode(new MutableLinkedList<T>())
		{
		}

		/*
		 * We free all the nodes allocated by the array
		 * and store the next one inside "NextIt"
		 */

		virtual ~NullableMutableArray()
		{
			auto*				  It	 = fFirstNode;
			MutableLinkedList<T>* NextIt = nullptr;

			while (It)
			{
				NextIt = It->fNext;
				delete It;

				It = NextIt;
			}
		}

		NullableMutableArray& operator=(const NullableMutableArray&) = default;
		NullableMutableArray(const NullableMutableArray&)			 = default;

		operator bool()
		{
			return Count() > 1;
		}

	public:
		T operator[](const SizeT& Index) const
		{
			TRY_FIND_NODE(first, fFirstNode);
			TRY_FIND_NODE(last, fLastNode);

			return _PlaceHolderValue;
		}

		SizeT Count() const
		{
			return fNodeCount;
		}

	public:
		Boolean Remove(const SizeT& Index)
		{
			TRY_REMOVE_NODE(fFirstNode);
			TRY_REMOVE_NODE(fLastNode);

			return false;
		}

		Boolean Add(const T val)
		{
			auto* iterationNode = fFirstNode;
			MUST_PASS(iterationNode);

			while (iterationNode)
			{
				if (!iterationNode->fUsed)
				{
					iterationNode->fVal	  = val;
					iterationNode->fIndex = 0;

					iterationNode->fUsed = true;

					++fNodeCount;

					return true;
				}

				iterationNode = iterationNode->fNext;
			}

			return false;
		}

	private:
		/* Avoid useless lookups */
		MutableLinkedList<T>* fLastNode{nullptr};
		MutableLinkedList<T>* fFirstNode{nullptr};

		/* Number of nodes inside of this dynamic array. */
		Kernel::SizeT fNodeCount{0};

	private:
		// don't remove that
		friend MutableArray<T>;
	};

	template <typename T>
	class MutableArray : public NullableMutableArray<voidPtr, nullptr>
	{
	public:
		// explicit this.
		explicit MutableArray() = default;
		virtual ~MutableArray() = default;

		ZKA_COPY_DEFAULT(MutableArray)

	public:
		Boolean Add(const T val)
		{
			auto* iterationNode = fFirstNode;

			if (!iterationNode)
			{
				fFirstNode	  = new MutableLinkedList<T>();
				iterationNode = fFirstNode;
			}

			MUST_PASS(iterationNode);

			while (iterationNode)
			{
				if (!iterationNode->fUsed)
				{
					iterationNode->fVal	  = val;
					iterationNode->fIndex = 0;

					iterationNode->fUsed = true;

					++fNodeCount;

					return true;
				}

				iterationNode = iterationNode->fNext;
			}

			return false;
		}

	public:
		Ref<T> operator[](const SizeT& Index) const
		{
			TRY_FIND_NODE2(first, fFirstNode);
			TRY_FIND_NODE2(last, fLastNode);

			return {};
		}

		SizeT Count() const
		{
			return fNodeCount;
		}

		bool Contains(T& value) noexcept
		{
			MutableLinkedList<T>* first = fFirstNode;

			while (first)
			{
				if (first->fVal == value && first->fUsed)
					return true;

				first = first->fNext;
			}

			return false;
		}

	private:
		/* Avoid useless lookups */
		MutableLinkedList<T>* fLastNode{nullptr};
		MutableLinkedList<T>* fFirstNode{nullptr};

		/* Number of nodes inside of this dynamic array. */
		Kernel::SizeT fNodeCount{0};
	};
} // namespace Kernel
