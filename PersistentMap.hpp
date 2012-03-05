/** -*-c++-*-
 *
 *  Hash array mapped tries (HAMT) as introduced by Phil Bagwell.
 *
 *  Copyright 2012  Olaf Delgado-Friedrichs
 *
 */


#ifndef ODF_HASH_TRIE_HPP
#define ODF_HASH_TRIE_HPP 1

#include <stdint.h>
#include <vector>
#include <sstream>

#include <boost/smart_ptr.hpp>


namespace odf
{
namespace hash_trie
{

typedef uint32_t hashType;
typedef uint8_t  indexType;


// ----------------------------------------------------------------------------
// Bit counting and manipulation functions.
// ----------------------------------------------------------------------------

indexType masked(hashType const n, indexType const shift)
{
    return (n >> shift) & 0x1f;
}

indexType bitCount(hashType n)
{
    n -= (n >> 1) & 0x55555555;
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    n = (n & 0x0f0f0f0f) + ((n >> 4) & 0x0f0f0f0f);
    n += n >> 8;
    return (n + (n >> 16)) & 0x3f;
}

indexType indexForBit(hashType const bitmap, hashType const bit)
{
    return bitCount(bitmap & (bit - 1));
}

hashType maskBit(hashType const n, indexType const shift)
{
    return 1 << masked(n, shift);
}


// ----------------------------------------------------------------------------
// Array copying with small modifications
// ----------------------------------------------------------------------------

template<typename T>
T const* arrayUpdate(T const* source, int const len, int const pos, T const val)
{
    T* copy = new T[len];
    for (int i = 0; i < len; ++i)
        copy[i] = (i == pos) ? val : source[i];
    return copy;
}

template<typename T>
T const* arrayInsert(T const* source, int const len, int const pos, T const val)
{
    T* copy = new T[len + 1];
    for (int i = 0; i < pos; ++i)
        copy[i] = source[i];
    copy[pos] = val;
    for (int i = pos; i < len; ++i)
        copy[i+1] = source[i];
    return copy;
}

template<typename T>
T const* arrayRemove(T const* source, int const len, int const pos)
{
    T* copy = new T[len - 1];
    for (int i = 0; i < pos; ++i)
        copy[i] = source[i];
    for (int i = pos+1; i < len; ++i)
        copy[i-1] = source[i];
    return copy;
}


// ----------------------------------------------------------------------------
// We need some forward declarations for hash trie node type
// ----------------------------------------------------------------------------

template<typename Key, typename Val> class Leaf;
template<typename Key, typename Val> class CollisionNode;
template<typename Key, typename Val> class BitmappedNode;

// ----------------------------------------------------------------------------
// The interface for nodes of the hash trie data structure
// ----------------------------------------------------------------------------

template<typename Key, typename Val>
struct Node
{
    typename boost::shared_ptr<Val>     typedef ValPtr;
    typename boost::intrusive_ptr<Node> typedef NodePtr;

    virtual size_t size() const = 0;

    virtual bool isLeaf() const = 0;

    virtual ValPtr get(indexType const shift,
                       hashType  const hash,
                       Key       const key) const = 0;

    virtual NodePtr insert(indexType const shift,
                           hashType  const hash,
                           NodePtr   const leaf) const = 0;

    virtual NodePtr remove(indexType const shift,
                           hashType  const hash,
                           Key       const key) const = 0;

    virtual Key const& key() const {};

    virtual std::string asString() const = 0;

    friend void intrusive_ptr_add_ref(Node const* const p)
    {
        ++p->counter_;
    }

    friend void intrusive_ptr_release(Node const* const p)
    {
        if (--p->counter_ == 0)
            delete p;
    }

protected:
    Node()
        : counter_(0)
    {
    }

    Node(Node const&)
        : counter_(0)
    {
    }

    virtual ~Node() {};

    Node& operator=(Node const&)
    {
        return *this;
    }

private:
    mutable size_t counter_;
};

// ----------------------------------------------------------------------------
// We use an empty node to avoid checking for null pointers
// ----------------------------------------------------------------------------

template<typename Key, typename Val>
struct EmptyNode : public Node<Key, Val>
{
    typename Node<Key, Val>::ValPtr  typedef ValPtr;
    typename Node<Key, Val>::NodePtr typedef NodePtr;

    EmptyNode() {}

    ~EmptyNode() {}

    size_t size() const { return 0; }

    bool isLeaf() const { return true; }

    ValPtr get(indexType const shift,
               hashType  const hash,
               Key       const key) const
    {
        return ValPtr();
    }

    NodePtr insert(indexType const shift,
                   hashType  const hash,
                   NodePtr   const leaf) const
    {
        return leaf;
    }

    NodePtr remove(indexType const shift,
                   hashType  const hash,
                   Key       const key) const
    {
        return NodePtr(new EmptyNode());
    }

    std::string asString() const
    {
        return "{}";
    }
};


// ----------------------------------------------------------------------------
// A leaf node holds a single key,value pair
// ----------------------------------------------------------------------------

template<typename Key, typename Val>
struct Leaf : public Node<Key, Val>
{
    typename Node<Key, Val>::ValPtr  typedef ValPtr;
    typename Node<Key, Val>::NodePtr typedef NodePtr;

    Leaf(hashType const hash, Key const key, ValPtr const value)
        : hash_(hash),
          key_(key),
          value_(value)
    {
    }

    ~Leaf() {}

    size_t size() const { return 1; }

    bool isLeaf() const { return true; }

    ValPtr get(indexType const shift,
               hashType  const hash,
               Key       const key) const
    {
        return key == key_ ? value_ : ValPtr();
    }

    NodePtr insert(indexType const shift,
                   hashType  const hash,
                   NodePtr   const leaf) const
    {
        if (key_ == leaf->key())
        {
            return leaf;
        }
        else
        {
            NodePtr base;
            if (hash_ == hash)
                base = NodePtr(new CollisionNode<Key, Val>(hash));
            else
                base = NodePtr(new BitmappedNode<Key, Val>());

            return base
                ->insert(shift, hash_, clone())
                ->insert(shift, hash, leaf);
        }
    }

    NodePtr remove(indexType const shift,
                   hashType  const hash,
                   Key       const key) const
    {
        return NodePtr(new EmptyNode<Key, Val>());
    }

    Key const& key() const { return key_; }


    std::string asString() const
    {
        std::stringstream ss;
        ss << key_ << " -> " << *value_.get();
        return ss.str();
    }

private:
    hashType const hash_;
    Key const key_;
    ValPtr const value_;

    NodePtr const clone() const
    {
        return NodePtr(new Leaf(*this));
    }
};

// ----------------------------------------------------------------------------
// A collision node holds several leaf nodes with equal hash codes.
// ----------------------------------------------------------------------------

template<typename Key, typename Val>
struct CollisionNode : public Node<Key, Val>
{
    typename Node<Key, Val>::ValPtr  typedef ValPtr;
    typename Node<Key, Val>::NodePtr typedef NodePtr;

    typedef std::vector<NodePtr> Bucket;

    CollisionNode(hashType const hash)
        : hash_(hash),
          bucket_()
    {
    }

    size_t size() const { return bucket_.size(); }

    bool isLeaf() const { return true; }

    ValPtr get(indexType const shift,
               hashType  const hash,
               Key       const key) const
    {
        for (typename Bucket::const_iterator iter = bucket_.begin();
             iter != bucket_.end();
             ++iter)
        {
            if ((*iter)->key() == key)
                return (*iter)->get(shift, hash, key);
        }
        return ValPtr();
    }

    NodePtr insert(indexType const shift,
                   hashType  const hash,
                   NodePtr   const leaf) const
    {
        if (hash != hash_)
        {
            return NodePtr(new BitmappedNode<Key, Val>())
                ->insert(shift, hash_, clone())
                ->insert(shift, hash, leaf);
        }
        else
        {
            Bucket newBucket = bucketWithout(leaf->key());
            newBucket.push_back(leaf);
            return NodePtr(new CollisionNode(hash, newBucket));
        }
    }

    NodePtr remove(indexType const shift,
                   hashType  const hash,
                   Key       const key) const
    {
        assert(size() >= 2);
        if (size() == 2)
        {
            if (bucket_.at(0)->key() != key)
                return bucket_.at(0);
            else
                return bucket_.at(1);
        }
        else
        {
            return NodePtr(new CollisionNode(hash, bucketWithout(key)));
        }
    }

    std::string asString() const
    {
        std::stringstream ss;
        ss << "<";
        for (typename Bucket::const_iterator iter = bucket_.begin();
             iter != bucket_.end();
             ++iter)
        {
            if (iter != bucket_.begin())
                ss << " | ";
            ss << (*iter)->asString();
        }
        ss << ">";
        return ss.str();
    }
        
private:
    hashType const hash_;
    Bucket const bucket_;

    CollisionNode(hashType const hash, Bucket const bucket)
        : hash_(hash),
          bucket_(bucket)
    {
    }

    NodePtr const clone() const
    {
        return NodePtr(new CollisionNode(*this));
    }

    Bucket bucketWithout(Key const key) const
    {
        Bucket result;
        for (typename Bucket::const_iterator iter = bucket_.begin();
             iter != bucket_.end();
             ++iter)
        {
            if ((*iter)->key() != key)
                result.push_back(*iter);
        }
        return result;
    }
};

// ----------------------------------------------------------------------------
// An array node provides room for up to 32 child nodes with direct access
// ----------------------------------------------------------------------------

template<typename Key, typename Val>
struct ArrayNode : public Node<Key, Val>
{
    typename Node<Key, Val>::ValPtr  typedef ValPtr;
    typename Node<Key, Val>::NodePtr typedef NodePtr;

    ArrayNode(NodePtr const* progeny, size_t const size)
        : progeny_(progeny),
          size_(size)
    {
    }

    ~ArrayNode()
    {
        delete[] progeny_;
    }

    size_t size() const { return size_; }

    bool isLeaf() const { return false; }

    ValPtr get(indexType const shift,
               hashType  const hash,
               Key       const key) const
    {
        indexType i = masked(hash, shift);
        if (progeny_[i].get() != 0)
            return progeny_[i]->get(shift + 5, hash, key);
        else
            return ValPtr();
    }

    NodePtr insert(indexType const shift,
                   hashType  const hash,
                   NodePtr   const leaf) const
    {
        indexType i = masked(hash, shift);
        NodePtr oldNode = progeny_[i];
        bool const recur = oldNode.get() != 0;

        NodePtr newNode = recur ? oldNode->insert(shift+5, hash, leaf) : leaf;
        size_t added = recur ? newNode->size() - oldNode->size() : leaf->size();

        return NodePtr(new ArrayNode(arrayUpdate(progeny_, 32, i, newNode),
                                     size() + added));
    }

    NodePtr remove(indexType const shift,
                   hashType  const hash,
                   Key       const key) const
    {
        indexType i = masked(hash, shift);
        NodePtr node = progeny_[i]->remove(shift+5, hash, key);
        if (node->size() > 0)
        {
            return NodePtr(new ArrayNode(arrayUpdate(progeny_, 32, i, node),
                                         size() - 1));
        }
        else
        {
            indexType count = 0;
            for (indexType j = 0; j < 32; ++j)
            {
                if (j != i and progeny_[j].get() != 0)
                    ++count;
            }
            if (count <= 8)
            {
                NodePtr* remaining = new NodePtr[count];
                hashType bitmap = 0;
                indexType k = 0;
                for (indexType j = 0; j < 32; ++j)
                {
                    if (j != i and progeny_[j].get() != 0)
                    {
                        bitmap |= 1 << j;
                        remaining[k] = progeny_[j];
                        ++k;
                    }
                }
                return NodePtr(new BitmappedNode<Key, Val>(
                                   bitmap, remaining, size() - 1));
            }
            else
            {
                return NodePtr(new ArrayNode(
                                   arrayUpdate(progeny_, 32, i, NodePtr()),
                                   size() - 1));
            }
        }
    }

    std::string asString() const
    {
        std::stringstream ss;
        ss << "[";
        int j = 0;
        for (int i = 0; i < 32; ++i)
        {
            if (progeny_[i].get() != 0)
            {
                if (j > 0)
                    ss << ", ";
                ss << i << ": " << progeny_[i]->asString();
                ++j;
            }
        }
        ss << "]";
        return ss.str();
    }
        
private:
    NodePtr const* progeny_;
    size_t const size_;
};

// ----------------------------------------------------------------------------
// An bitmapped node contains a sparse array of child nodes
// ----------------------------------------------------------------------------

template<typename Key, typename Val>
struct BitmappedNode : public Node<Key, Val>
{
    typename Node<Key, Val>::ValPtr  typedef ValPtr;
    typename Node<Key, Val>::NodePtr typedef NodePtr;

    BitmappedNode()
        : bitmap_(0),
          progeny_(0),
          size_(0)
    {
    }

    BitmappedNode(hashType const bitmap,
                  NodePtr const* progeny,
                  size_t const size)
        : bitmap_(bitmap),
          progeny_(progeny),
          size_(size)
    {
    }

    ~BitmappedNode()
    {
        delete[] progeny_;
    }

    size_t size() const { return size_; }

    bool isLeaf() const { return false; }

    ValPtr get(indexType const shift,
               hashType  const hash,
               Key       const key) const
    {
        hashType bit = maskBit(hash, shift);
        if ((bitmap_ & bit) != 0)
        {
            indexType i = indexForBit(bitmap_, bit);
            return progeny_[i]->get(shift + 5, hash, key);
        }
        else
        {
            return ValPtr();
        }
    }

    NodePtr insert(indexType const shift,
                   hashType  const hash,
                   NodePtr   const leaf) const
    {
        hashType bit = maskBit(hash, shift);
        indexType i = indexForBit(bitmap_, bit);
        indexType nrBits = bitCount(bitmap_);
        
        if ((bitmap_ & bit) == 0 && nrBits >= 16)
        {
            NodePtr* newArray = new NodePtr[32];
            size_t newSize = size() + leaf->size();
            for (int j = 0; j < 32; ++j)
            {
                hashType b = 1 << j;
                if ((bitmap_ & b) != 0)
                    newArray[j] = progeny_[indexForBit(bitmap_, b)];
            }
            newArray[masked(hash, shift)] = leaf;
            return NodePtr(new ArrayNode<Key, Val>(newArray, newSize));
        }
        else
        {
            NodePtr const* newArray;
            size_t newSize;

            if ((bitmap_ & bit) == 0)
            {
                newArray = arrayInsert(progeny_, nrBits, i, leaf);
                newSize = size() + leaf->size();
            }
            else
            {
                NodePtr oldNode = progeny_[i];
                NodePtr newNode = oldNode->insert(shift + 5, hash, leaf);
                newArray = arrayUpdate(progeny_, nrBits, i, newNode);
                newSize = size() + newNode->size() - oldNode->size();
            }
            return NodePtr(new BitmappedNode(bitmap_ | bit, newArray, newSize));
        }
    }

    NodePtr remove(indexType const shift,
                   hashType  const hash,
                   Key       const key) const
    {
        hashType bit = maskBit(hash, shift);
        indexType i = indexForBit(bitmap_, bit);
        NodePtr v = progeny_[i];
        NodePtr node = v->remove(shift + 5, hash, key);

        hashType newBitmap;
        size_t   newSize;
        NodePtr const*  newArray;

        if (node->size() > 0)
        {
            newBitmap = bitmap_;
            newSize   = size() + node->size() - v->size();
            newArray  = arrayUpdate(progeny_, bitCount(bitmap_), i, node);
        }
        else
        {
            newBitmap = bitmap_ ^ bit;
            newSize   = size() - 1;
            newArray  = arrayRemove(progeny_, bitCount(bitmap_), i);
        }

        indexType nrBits = bitCount(newBitmap);
        assert(nrBits > 0);
        if (nrBits == 1 and newArray[0]->isLeaf())
        {
            NodePtr result = newArray[0];
            delete[] newArray;
            return result;
        }
        else
        {
            return NodePtr(new BitmappedNode(newBitmap, newArray, newSize));
        }
    }

    std::string asString() const
    {
        std::stringstream ss;
        ss << "{";
        for (int i = 0; i < 32; ++i)
        {
            hashType bit = 1 << i;
            if (bitmap_ & bit)
            {
                indexType j = indexForBit(bitmap_, bit);
                if (j > 0)
                    ss << ", ";
                ss << i << ": " << progeny_[j]->asString();
            }
        }
        ss << "}";
        return ss.str();
    }
        
private:
    hashType const bitmap_;
    NodePtr const* progeny_;
    size_t const size_;
};

// ----------------------------------------------------------------------------
// The driver class
// ----------------------------------------------------------------------------

template<typename Key, typename Val, hashType (*hashFunc)(Key const)>
class PersistentMap
{
public:
    typename Node<Key, Val>::ValPtr  typedef ValPtr;
    typename Node<Key, Val>::NodePtr typedef NodePtr;

    PersistentMap()
        : root_(new EmptyNode<Key, Val>())
    {
    }

    size_t size() const
    {
        return root_->size();
    }

    ValPtr get(Key const key) const
    {
        return root_->get(0, hashFunc(key), key);
    }

    Val getVal(Key const key, Val const notFound) const
    {
        ValPtr vp = get(key);
        if (vp.get() != 0)
            return *vp;
        else
            return notFound;
    }

    PersistentMap const insert(Key const key, Val const val) const
    {
        hashType hash = hashFunc(key);
        ValPtr current = root_->get(0, hash, key);
        if (current.get() == 0 or *current != val)
        {
            NodePtr leaf(new Leaf<Key, Val>(hash, key, ValPtr(new Val(val))));
            return PersistentMap(root_->insert(0, hash, leaf));
        }
        else
        {
            return *this;
        }
    }

    PersistentMap const remove(Key const key) const
    {
        hashType hash = hashFunc(key);
        if (root_->get(0, hash, key).get() != 0)
        {
            return PersistentMap(root_->remove(0, hash, key));
        }
        else
        {
            return *this;
        }
    }

    std::string asString() const
    {
        std::stringstream ss;
        ss << "PersistentMap(" << root_->asString() << ")";
        return ss.str();
    }

private:
    PersistentMap(NodePtr const root)
        : root_(root)
    {
    }

    NodePtr root_;
};


template<typename Key, typename Val, hashType (*hashFunc)(Key const)>
std::ostream& operator<<(std::ostream& out,
                         PersistentMap<Key, Val, hashFunc> const& map)
{
    out << map.asString();
    return out;
}
}
}

#endif // !ODF_HASH_TRIE_HPP
