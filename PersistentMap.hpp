/** -*-c++-*-
 *
 *  Hash array mapped tries (HAMT) as introduced by Phil Bagwell:
 *  Implementation of the PersistentMap class.
 *
 *  Copyright 2012  Olaf Delgado-Friedrichs
 *
 */


#ifndef ODF_PERSISTENTMAP_HPP
#define ODF_PERSISTENTMAP_HPP 1

#include "hash_trie.hpp"

namespace odf
{
namespace hash_trie
{

// ----------------------------------------------------------------------------
// A leaf node for maps - holds a single key,value pair
// ----------------------------------------------------------------------------

template<typename Key, typename Val>
struct MapLeaf : public Node<Key, Val>
{
    typename Node<Key, Val>::ValPtr  typedef ValPtr;
    typename Node<Key, Val>::NodePtr typedef NodePtr;

    MapLeaf(hashType const hash, Key const key, ValPtr const value)
        : hash_(hash),
          key_(key),
          value_(value)
    {
    }

    ~MapLeaf() {}

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
        return NodePtr();
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
        return NodePtr(new MapLeaf(*this));
    }
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
        : root_()
    {
    }

    size_t size() const
    {
        return root_ ? root_->size() : 0;
    }

    ValPtr get(Key const key) const
    {
        return root_ ? root_->get(0, hashFunc(key), key) : ValPtr();
    }

    Val getVal(Key const key, Val const notFound) const
    {
        ValPtr vp = get(key);
        if (vp)
            return *vp;
        else
            return notFound;
    }

    PersistentMap const insert(Key const key, Val const val) const
    {
        hashType hash = hashFunc(key);
        NodePtr leaf(new MapLeaf<Key, Val>(hash, key, ValPtr(new Val(val))));
        if (not root_)
        {
            return PersistentMap(leaf);
        }
        else
        {
            ValPtr current = root_->get(0, hash, key);
            if (not current or *current != val)
                return PersistentMap(root_->insert(0, hash, leaf));
            else
                return *this;
        }
    }

    PersistentMap const remove(Key const key) const
    {
        hashType hash = hashFunc(key);
        if (root_ and root_->get(0, hash, key))
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
        ss << "PersistentMap(" <<
            (root_ ? root_->asString() : "{}") << ")";
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

} // namespace hash_trie
} // namespace odf

#endif // !ODF_PERSISTENTMAP_HPP
