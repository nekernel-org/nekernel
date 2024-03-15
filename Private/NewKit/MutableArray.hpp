/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */
#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>

#define TRY_FIND_NODE(NAME, NODE)                                              \
    auto *NAME = NODE;                                                         \
    while (NAME)                                                               \
    {                                                                          \
        if (NAME->m_Index == Index)                                            \
            return NAME->m_Val;                                                \
        NAME = NAME->m_Next;                                                   \
    }



#define TRY_FIND_NODE2(NAME, NODE)                                             \
    auto *NAME = NODE;                                                         \
    while (NAME)                                                               \
    {                                                                          \
        if (NAME->m_Index == Index)                                            \
            return Ref<T>{NAME->m_Val};                                        \
        NAME = NAME->m_Next;                                                   \
    }



#define TRY_REMOVE_NODE(NODE)                                                  \
    if (NODE && NODE->m_Index == Index)                                        \
    {                                                                          \
        NODE->m_Used = false;                                                  \
        NODE->m_Index = 0;                                                     \
                                                                               \
        return true;                                                           \
    }



// FIXME: this is a shitty algorithm, which is consumer hungry.
// Remove and occurences of that, and remove that class.
namespace HCore
{
template <typename T> class MutableArray;

template <typename T, T _PlaceHolderValue> class NullableMutableArray;

template <typename T> class MutableLinkedList
{
  public:
    T m_Val;
    SizeT m_Index{0};
    Boolean m_Used{false};

    MutableLinkedList *m_Prev{nullptr};
    MutableLinkedList *m_Next{nullptr};
};

template <typename T, T _PlaceHolderValue> class NullableMutableArray
{
  public:
    // explicit this.
    explicit NullableMutableArray() : m_FirstNode(new MutableLinkedList<T>()) {}

    /*
     * We free all the nodes allocated by the array
     * and store the next one inside "NextIt"
     */

    virtual ~NullableMutableArray()
    {
        auto *It = m_FirstNode;
        MutableLinkedList<T> *NextIt = nullptr;

        while (It)
        {
            NextIt = It->m_Next;
            delete It;

            It = NextIt;
        }
    }

    NullableMutableArray &operator=(const NullableMutableArray &) = default;
    NullableMutableArray(const NullableMutableArray &) = default;

    operator bool() { return Count() > 1; }

  public:
    T operator[](const SizeT &Index) const
    {
        TRY_FIND_NODE(first, m_FirstNode);
        TRY_FIND_NODE(last, m_LastNode);

        return _PlaceHolderValue;
    }

    SizeT Count() const { return m_NodeCount; }

  public:
    Boolean Remove(const SizeT &Index)
    {
        TRY_REMOVE_NODE(m_FirstNode);
        TRY_REMOVE_NODE(m_LastNode);

        return false;
    }

    Boolean Add(const T val)
    {
        auto *iterationNode = m_FirstNode;
        MUST_PASS(iterationNode);

        while (iterationNode)
        {
            if (!iterationNode->m_Used)
            {
                iterationNode->m_Val = val;
                iterationNode->m_Index = 0;

                iterationNode->m_Used = true;

                ++m_NodeCount;

                return true;
            }

            iterationNode = iterationNode->m_Next;
        }

        return false;
    }

  private:
    /* Avoid useless lookups */
    MutableLinkedList<T> *m_LastNode{nullptr};
    MutableLinkedList<T> *m_FirstNode{nullptr};

    /* Number of nodes inside of this dynamic array. */
    HCore::SizeT m_NodeCount{0};

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

    HCORE_COPY_DEFAULT(MutableArray)

  public:
    Boolean Add(const T val)
    {
        auto *iterationNode = m_FirstNode;
        MUST_PASS(iterationNode);

        while (iterationNode)
        {
            if (!iterationNode->m_Used)
            {
                iterationNode->m_Val = val;
                iterationNode->m_Index = 0;

                iterationNode->m_Used = true;

                ++m_NodeCount;

                return true;
            }

            iterationNode = iterationNode->m_Next;
        }

        return false;
    }

  public:
    Ref<T> operator[](const SizeT &Index) const
    {
        TRY_FIND_NODE2(first, m_FirstNode);
        TRY_FIND_NODE2(last, m_LastNode);

        return {};
    }

    SizeT Count() const { return m_NodeCount; }

    bool Contains(T &value) noexcept
    {
        MutableLinkedList<T> *first = m_FirstNode;

        while (first)
        {
            if (first->m_Val == value && first->m_Used)
                return true;

            first = first->m_Next;
        }

        return false;
    }

  private:
    /* Avoid useless lookups */
    MutableLinkedList<T> *m_LastNode{nullptr};
    MutableLinkedList<T> *m_FirstNode{nullptr};

    /* Number of nodes inside of this dynamic array. */
    HCore::SizeT m_NodeCount{0};
};
} // namespace HCore
