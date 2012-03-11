/** -*-c++-*-
 *
 *  Hash array mapped tries (HAMT) as introduced by Phil Bagwell:
 *  Implementation of the PersistentSet class.
 *
 *  Copyright 2012  Olaf Delgado-Friedrichs
 *
 */


#ifndef ODF_PERSISTENTSET_HPP
#define ODF_PERSISTENTSET_HPP 1

#include "hash_trie.hpp"

namespace odf
{
namespace hash_trie
{

// ----------------------------------------------------------------------------
// A leaf node for sets - holds a single key,value pair
// ----------------------------------------------------------------------------

template<typename Key>
struct SetLeaf : public Node<Key, bool>
{
    typename Node<Key, bool>::ValPtr  typedef ValPtr;
    typename Node<Key, bool>::NodePtr typedef NodePtr;

    SetLeaf(hashType const hash, Key const key)
        : hash_(hash),
          key_(key)
    {
    }

    ~SetLeaf() {}

    size_t size() const { return 1; }

    bool isLeaf() const { return true; }

    ValPtr get(indexType const shift,
               hashType  const hash,
               Key       const key) const
    {
        return ValPtr(key == key_);
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
                base = NodePtr(new CollisionNode<Key, bool>(hash));
            else
                base = NodePtr(new BitmappedNode<Key, bool>());

            return base
                ->insert(shift, hash_, clone())
                ->insert(shift, hash, leaf);
        }
    }

    NodePtr remove(indexType const shift,
                   hashType  const hash,
                   Key       const key) const
    {
        return NodePtr();
    }

    Key const& key() const { return key_; }


    std::string asString() const
    {
        std::stringstream ss;
        ss << key_;
        return ss.str();
    }

private:
    hashType const hash_;
    Key const key_;

    NodePtr const clone() const
    {
        return NodePtr(new SetLeaf(*this));
    }
};

// ----------------------------------------------------------------------------
// The driver class
// ----------------------------------------------------------------------------

template<typename Key, hashType (*hashFunc)(Key const)>
class PersistentSet
{
public:
    typename Node<Key, bool>::NodePtr typedef NodePtr;

    PersistentSet()
        : root_()
    {
    }

    size_t size() const
    {
        return root_ ? root_->size() : 0;
    }

    bool contains(Key const key) const
    {
        return root_ && root_->get(0, hashFunc(key), key);
    }

    PersistentSet const insert(Key const key) const
    {
        hashType hash = hashFunc(key);
        NodePtr leaf(new SetLeaf<Key>(hash, key));
        if (not root_)
            return PersistentSet(leaf);
        else if (not root_->get(0, hash, key))
            return PersistentSet(root_->insert(0, hash, leaf));
        else
            return *this;
    }

    PersistentSet const remove(Key const key) const
    {
        hashType hash = hashFunc(key);
        if (root_ and root_->get(0, hash, key))
            return PersistentSet(root_->remove(0, hash, key));
        else
            return *this;
    }

    std::string asString() const
    {
        std::stringstream ss;
        ss << "PersistentSet(" <<
            (root_ ? root_->asString() : "{}") << ")";
        return ss.str();
    }

private:
    PersistentSet(NodePtr const root)
        : root_(root)
    {
    }

    NodePtr root_;
};


template<typename Key, hashType (*hashFunc)(Key const)>
std::ostream& operator<<(std::ostream& out,
                         PersistentSet<Key, hashFunc> const& set)
{
    out << set.asString();
    return out;
}

} // namespace hash_trie
} // namespace odf

#endif // !ODF_PERSISTENTSET_HPP
