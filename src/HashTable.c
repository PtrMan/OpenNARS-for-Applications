#include "HashTable.h"

Concept *HashTable_Get(HashTable *hashtable, Term *key)
{
    TERM_HASH_TYPE hash = Term_Hash(key) % CONCEPTS_MAX;
    VMItem *item = hashtable->HT[hash];
    for(; item!=NULL; item=item->next)
    {
        if(Term_Equal(&item->value->term, key))
        {
            return item->value;
        }
    }
    return NULL;
}

void HashTable_Set(HashTable *hashtable, Concept *c)
{
    //Check if item already exists in hashtable, if yes return
    VMItem *item = hashtable->HT[c->term_hash];
    bool empty = item == NULL;
    if(!empty)
    {
        for(; item->next!=NULL; item=item->next)
        {
            if(Term_Equal(&item->value->term, &c->term))
            {
                return;
            }
        }
    }
    //Retrieve recycled VMItem from the stack and set its value to c
    VMItem *popped = Stack_Pop(&hashtable->VMStack);
    popped->value = c;
    popped->next = NULL;
    //Case1: HT at hash was empty so add recycled item at HT[c->term_hash]
    if(empty)
    {
        hashtable->HT[c->term_hash] = popped;
    }
    //Case2: HT at hash not empty so add recycled item at end of the chain of HT[c->term_hash]
    else
    {
        assert(item != NULL, "VMItem should not be null!");
        item->next = popped;
    }
}

void HashTable_Delete(HashTable *hashtable, Concept *c)
{
    VMItem *item = hashtable->HT[c->term_hash];
    VMItem *previous = NULL;
    //If there is only one item set HT[c->term_hash] to NULL and push back the VMItem to stack for recycling
    if(item->next == NULL)
    {
        hashtable->HT[c->term_hash] = NULL;
        Stack_Push(&hashtable->VMStack, item);
        return;
    }
    //If there is more than 1 item, we have to remove the item from chain, relinking previous to next
    for(; item!=NULL; previous=item, item=item->next)
    {
        //item found?
        if(Term_Equal(&item->value->term, &c->term))
        {
            //remove item and return
            if(previous == NULL)
            {
                hashtable->HT[c->term_hash] = item->next;
            }
            else
            {
                previous->next = item->next;
            }
            Stack_Push(&hashtable->VMStack, item);
            return;
        }
    }
    assert(false, "HashTable deletion failed, item was not found!");
}

void HashTable_Init(HashTable *hashtable)
{
    hashtable->VMStack = (Stack) {0};
    for(int i=0; i<CONCEPTS_MAX; i++)
    {
        Stack_Push(&hashtable->VMStack, &hashtable->storage[i]);
        hashtable->HT[i] = NULL;
    }
}