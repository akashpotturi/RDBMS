
#include "BlockAccess.h"
#include <stdlib.h>
#include <cstring>
#include <iostream>
using namespace std;
RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {
    // get the previous search index of the relation relId from the relation cache
    // (use RelCacheTable::getSearchIndex() function)
    RecId prevRecId;
    RelCacheTable::getSearchIndex(relId,&prevRecId);
    // let block and slot denote the record id of the record being currently checked
    int block,slot;
    RelCatEntry buf;
    // if the current search index record is invalid(i.e. both block and slot = -1)
    if (prevRecId.block == -1 && prevRecId.slot == -1)
    {
        // (no hits from previous search; search should start from the
        // first record itself)

        // get the first record block of the relation from the relation cache
        // (use RelCacheTable::getRelCatEntry() function of Cache Layer)
        RelCacheTable::getRelCatEntry(relId, &buf);
        block = buf.firstBlk;
        slot = 0;

        // block = first record block of the relation
        // slot = 0
    }
    else
    {
        // (there is a hit from previous search; search should start from
        // the record next to the search index record)
        block = prevRecId.block;
        slot = prevRecId.slot+1;
        // block = search index's block
        // slot = search index's slot + 1
    }

    /* The following code searches for the next record in the relation
       that satisfies the given condition
       We start from the record id (block, slot) and iterate over the remaining
       records of the relation
    */
    while (block != -1)
    {
        /* create a RecBuffer object for block (use RecBuffer Constructor for
           existing block) */
        RecBuffer buffer(block);
        struct HeadInfo head;
        buffer.getHeader(&head);
        int a = head.numAttrs;
        Attribute record[a];
        buffer.getRecord(record,slot);
        unsigned char *slotmap = (unsigned char*)malloc(sizeof(unsigned char)*head.numSlots);
        buffer.getSlotMap(slotmap);
        // get the record with id (block, slot) using RecBuffer::getRecord()
        // get header of the block using RecBuffer::getHeader() function
        // get slot map of the block using RecBuffer::getSlotMap() function

        if(slot>=head.numSlots)// If slot >= the number of slots per block(i.e. no more slots in this block)
        {
            // update block = right block of block
            block = head.rblock;
            slot = 0;
            // update slot = 0
            continue;  // continue to the beginning of this while loop
        }

        // if slot is free skip the loop
        if(slotmap[slot] == SLOT_UNOCCUPIED)// (i.e. check if slot'th entry in slot map of block contains SLOT_UNOCCUPIED)
        {
            // increment slot and continue to the next record slot
            slot++;
            continue;
        }
        

        // compare record's attribute value to the the given attrVal as below:
        /*
            firstly get the attribute offset for the attrName attribute
            from the attribute cache entry of the relation using
            AttrCacheTable::getAttrCatEntry()
        */
       AttrCatEntry buf;
       AttrCacheTable::getAttrCatEntry(relId,attrName,&buf);
        /* use the attribute offset to get the value of the attribute from
           current record */
        Attribute attr2 = record[buf.offset];

        int cmpVal = compareAttrs(attr2, attrVal, buf.attrType);  // will store the difference between the attributes
        // set cmpVal using compareAttrs()

        /* Next task is to check whether this record satisfies the given condition.
           It is determined based on the output of previous comparison and
           the op value received.
           The following code sets the cond variable if the condition is satisfied.
        */
        if (
            (op == NE && cmpVal != 0) ||    // if op is "not equal to"
            (op == LT && cmpVal < 0) ||     // if op is "less than"
            (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
            (op == EQ && cmpVal == 0) ||    // if op is "equal to"
            (op == GT && cmpVal > 0) ||     // if op is "greater than"
            (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
        ) {
            /*
            set the search index in the relation cache as
            the record id of the record that satisfies the given condition
            (use RelCacheTable::setSearchIndex function)
            */
            RecId rec;
            rec.block = block;
            rec.slot = slot;
            RelCacheTable::setSearchIndex(relId,&rec);
            return rec;
        }

        slot++;
    }

    // no record in the relation with Id relid satisfies the given condition
    return RecId{-1, -1};
}

int BlockAccess::renameRelation(char oldName[ATTR_SIZE], char newName[ATTR_SIZE]){
    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    Attribute newRelationName;    // set newRelationName with newName
    strcpy(newRelationName.sVal,newName);
    char name[ATTR_SIZE];
    strcpy(name,RELCAT_ATTR_RELNAME);
    // search the relation catalog for an entry with "RelName" = newRelationName
    RecId recId = BlockAccess::linearSearch(RELCAT_RELID,name,newRelationName,EQ);
    if(recId.block != -1 && recId.slot != -1)return E_RELEXIST;
    // If relation with name newName already exists (result of linearSearch
    //                                               is not {-1, -1})
    //    return E_RELEXIST;


    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    Attribute oldRelationName;    // set oldRelationName with oldName
    strcpy(oldRelationName.sVal,oldName);
    recId = BlockAccess::linearSearch(RELCAT_RELID,name,oldRelationName,EQ);
    if(recId.block == -1 && recId.slot == -1)return E_RELNOTEXIST;

    // search the relation catalog for an entry with "RelName" = oldRelationName

    // If relation with name oldName does not exist (result of linearSearch is {-1, -1})
    //    return E_RELNOTEXIST;

    /* get the relation catalog record of the relation to rename using a RecBuffer
       on the relation catalog [RELCAT_BLOCK] and RecBuffer.getRecord function
    */
    RecBuffer buf(RELCAT_BLOCK);
    Attribute reco[RELCAT_NO_ATTRS];
    buf.getRecord(reco,recId.slot);
    

    strcpy(reco[RELCAT_REL_NAME_INDEX].sVal,newName);
    /* update the relation name attribute in the record with newName.
       (use RELCAT_REL_NAME_INDEX) */
    // set back the record value using RecBuffer.setRecord
    buf.setRecord(reco,recId.slot);

    /*
    update all the attribute catalog entries in the attribute catalog corresponding
    to the relation with relation name oldName to the relation name newName
    */
    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    int numAttr = (int)reco[RELCAT_NO_ATTRIBUTES_INDEX].nVal;
    Attribute val;
    strcpy(val.sVal,oldName);
    /* reset the searchIndex of the attribute catalog using
       RelCacheTable::resetSearchIndex() */
    for(int i = 0;i<numAttr;i++)
    {
        recId = linearSearch(ATTRCAT_RELID,name,val,EQ);
        RecBuffer b(recId.block);
        b.getRecord(reco,recId.slot);
        strcpy(reco[ATTRCAT_REL_NAME_INDEX].sVal,newName);
        b.setRecord(reco,recId.slot);
    }
    //for i = 0 to numberOfAttributes :
    //    linearSearch on the attribute catalog for relName = oldRelationName
    //    get the record using RecBuffer.getRecord
    //
    //    update the relName field in the record to newName
    //    set back the record using RecBuffer.setRecord

    return SUCCESS;
}

int BlockAccess::renameAttribute(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE]) {

    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
    RelCacheTable::resetSearchIndex(RELCAT_RELID);

    Attribute relNameAttr;    // set relNameAttr to relName
    strcpy(relNameAttr.sVal,relName);
    char name[ATTR_SIZE];
    strcpy(name,ATTRCAT_ATTR_RELNAME);
    RecId recId = BlockAccess::linearSearch(RELCAT_RELID,name,relNameAttr,EQ);
    if(recId.block == -1 && recId.slot == -1)return E_RELNOTEXIST;
    // Search for the relation with name relName in relation catalog using linearSearch()
    // If relation with name relName does not exist (search returns {-1,-1})
    //    return E_RELNOTEXIST;
    /* reset the searchIndex of the attribute catalog using
       RelCacheTable::resetSearchIndex() */
    
    /* declare variable attrToRenameRecId used to store the attr-cat recId
    of the attribute to rename */
    Attribute attrCatEntryRecord[ATTRCAT_NO_ATTRS];
    RecId attrToRenameRecId{-1, -1};
    AttrCacheEntry attr;
    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    // AttrCacheTable::getAttrCatEntry(recId,)
    /* iterate over all Attribute Catalog Entry record corresponding to the
       relation to find the required attribute */
    while (true) {
        // linear search on the attribute catalog for RelName = relNameAttr
        recId = BlockAccess::linearSearch(ATTRCAT_RELID,name,relNameAttr,EQ);
        if(recId.block == -1 && recId.slot == -1)break;
        // if there are no more attributes left to check (linearSearch returned {-1,-1})
        //     break;

        /* Get the record from the attribute catalog using RecBuffer.getRecord
          into attrCatEntryRecord */
        RecBuffer r(recId.block);
        r.getRecord(attrCatEntryRecord,recId.slot);
        if(!strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,oldName))
        {
            attrToRenameRecId.block = recId.block;
            attrToRenameRecId.slot = recId.slot;
        }
        if(!strcmp(attrCatEntryRecord[1].sVal,newName))
        {
            return E_ATTREXIST;
        }


        // if attrCatEntryRecord.attrName = oldName
        //     attrToRenameRecId = block and slot of this record

        // if attrCatEntryRecord.attrName = newName
        //     return E_ATTREXIST;
    }

    // if attrToRenameRecId == {-1, -1}
    //     return E_ATTRNOTEXIST;
    if(attrToRenameRecId.block == -1 && attrToRenameRecId.slot == -1)return E_ATTRNOTEXIST;
    RecBuffer rec(attrToRenameRecId.block);
    Attribute record[ATTRCAT_NO_ATTRS];
    rec.getRecord(record,attrToRenameRecId.slot);
    strcpy(record[ATTRCAT_ATTR_NAME_INDEX].sVal,newName);
    rec.setRecord(record,attrToRenameRecId.slot);

    // Update the entry corresponding to the attribute in the Attribute Catalog Relation.
    /*   declare a RecBuffer for attrToRenameRecId.block and get the record at
         attrToRenameRecId.slot */
    //   update the AttrName of the record with newName
    //   set back the record with RecBuffer.setRecord

    return SUCCESS;
}
int BlockAccess::insert(int relId, Attribute *record) {
    // get the relation catalog entry from relation cache
    // ( use RelCacheTable::getRelCatEntry() of Cache Layer)
    RelCatEntry relcatEntry;
    RelCacheTable::getRelCatEntry(relId,&relcatEntry);

    int blockNum = /* first record block of the relation (from the rel-cat entry)*/relcatEntry.firstBlk;

    // rec_id will be used to store where the new record will be inserted
    RecId rec_id = {-1, -1};

    int numOfSlots = /* number of slots per record block */relcatEntry.numSlotsPerBlk;
    int numOfAttributes = /* number of attributes of the relation */relcatEntry.numAttrs;

    int prevBlockNum = /* block number of the last element in the linked list = -1 */-1;

    /*
        Traversing the linked list of existing record blocks of the relation
        until a free slot is found OR
        until the end of the list is reached
    */
    while (blockNum != -1) {
        // create a RecBuffer object for blockNum (using appropriate constructor!)
        RecBuffer rec(blockNum);
        struct HeadInfo head;
        rec.getHeader(&head);
        // unsigned char *slotmap = (unsigned char*)malloc(sizeof(unsigned char)*head.numSlots);
        unsigned char slotmap[head.numSlots];
        rec.getSlotMap(slotmap);

        // get header of block(blockNum) using RecBuffer::getHeader() function

        // get slot map of block(blockNum) using RecBuffer::getSlotMap() function

        // search for free slot in the block 'blockNum' and store it's rec-id in rec_id
        // (Free slot can be found by iterating over the slot map of the block)
        /* slot map stores SLOT_UNOCCUPIED if slot is free and
           SLOT_OCCUPIED if slot is occupied) */
        int freeslot = -1;
        for(int i = 0;i<head.numSlots;i++)
        {
            if(slotmap[i] == SLOT_UNOCCUPIED)
            {
                freeslot = i;
                break;
            }
        }
        if(freeslot != -1)
        {
            rec_id.block = blockNum;
            rec_id.slot = freeslot;
            break;
        }
        /* if a free slot is found, set rec_id and discontinue the traversal
           of the linked list of record blocks (break from the loop) */
        else
        {
            prevBlockNum = blockNum;
            blockNum = head.rblock;
        }
        /* otherwise, continue to check the next block by updating the
           block numbers as follows:
              update prevBlockNum = blockNum
              update blockNum = header.rblock (next element in the linked
                                               list of record blocks)
        */
    }
    if(rec_id.block == -1 && rec_id.slot == -1)//  if no free slot is found in existing record blocks (rec_id = {-1, -1})
    {
        // if relation is RELCAT, do not allocate any more blocks
        //     return E_MAXRELATIONS;
        char relcatName[ATTR_SIZE];
        strcpy(relcatName,RELCAT_RELNAME);
        if(!strcmp(relcatName,relcatEntry.relName))return E_MAXRELATIONS;

        // Otherwise,
        // get a new record block (using the appropriate RecBuffer constructor!)
        RecBuffer block;
        int ret = block.getBlockNum();
        // get the block number of the newly allocated block
        // (use BlockBuffer::getBlockNum() function)
        // let ret be the return value of getBlockNum() function call
        if (ret == E_DISKFULL) {
            return E_DISKFULL;
        }

        // Assign rec_id.block = new block number(i.e. ret) and rec_id.slot = 0
        rec_id.block = ret;
        rec_id.slot = 0;
        struct HeadInfo head;
        head.blockType = REC;
        head.pblock = -1;
        head.lblock = -1;
        head.rblock = -1;
        head.numEntries = 0;
        head.numSlots = numOfSlots;
        head.numAttrs = numOfAttributes;
        block.setHeader(&head);

        /*
            set the header of the new record block such that it links with
            existing record blocks of the relation
            set the block's header as follows:
            blockType: REC, pblock: -1
            lblock
                  = -1 (if linked list of existing record blocks was empty
                         i.e this is the first insertion into the relation)
                  = prevBlockNum (otherwise),
            rblock: -1, numEntries: 0,
            numSlots: numOfSlots, numAttrs: numOfAttributes
            (use BlockBuffer::setHeader() function)
        */
        // unsigned char *slotmap = (unsigned char*)malloc(sizeof(unsigned char)*head.numSlots);
        unsigned char slotmap[head.numSlots];
        for(int i = 0;i<head.numSlots;i++)
        {
            slotmap[i] = SLOT_UNOCCUPIED;
        }
        block.setSlotMap(slotmap);
        /*
            set block's slot map with all slots marked as free
            (i.e. store SLOT_UNOCCUPIED for all the entries)
            (use RecBuffer::setSlotMap() function)
        */

        if(prevBlockNum != -1)// if prevBlockNum != -1
        {
            // create a RecBuffer object for prevBlockNum
            RecBuffer buf(prevBlockNum);
            // get the header of the block prevBlockNum and
            struct HeadInfo prevhead;
            buf.getHeader(&prevhead);
            prevhead.rblock = rec_id.block;
            // update the rblock field of the header to the new block
            // number i.e. rec_id.block
            buf.setHeader(&prevhead);
            // (use BlockBuffer::setHeader() function)
        }
        // else
        else
        {
            // update first block field in the relation catalog entry to the
            // new block (using RelCacheTable::setRelCatEntry() function)
            relcatEntry.firstBlk = rec_id.block;
            RelCacheTable::setRelCatEntry(relId,&relcatEntry);
        }

        // update last block field in the relation catalog entry to the
        // new block (using RelCacheTable::setRelCatEntry() function)
        relcatEntry.lastBlk = rec_id.block;
        RelCacheTable::setRelCatEntry(relId,&relcatEntry);
    }

    // create a RecBuffer object for rec_id.block
    RecBuffer buffer(rec_id.block);
    buffer.setRecord(record,rec_id.slot);
    // insert the record into rec_id'th slot using RecBuffer.setRecord())
    // unsigned char *slotmap = (unsigned char*)malloc(sizeof(unsigned char)*numOfSlots);
    unsigned char slotmap[numOfSlots];
    buffer.getSlotMap(slotmap);
    slotmap[rec_id.slot] = SLOT_OCCUPIED;
    buffer.setSlotMap(slotmap);
    /* update the slot map of the block by marking entry of the slot to
       which record was inserted as occupied) */
    // (ie store SLOT_OCCUPIED in free_slot'th entry of slot map)
    // (use RecBuffer::getSlotMap() and RecBuffer::setSlotMap() functions)
    struct HeadInfo header;
    buffer.getHeader(&header);
    header.numEntries = header.numEntries + 1;
    buffer.setHeader(&header);
    // increment the numEntries field in the header of the block to
    // which record was inserted
    // (use BlockBuffer::getHeader() and BlockBuffer::setHeader() functions)
    relcatEntry.numRecs = relcatEntry.numRecs+1;
    RelCacheTable::setRelCatEntry(relId,&relcatEntry);
    // Increment the number of records field in the relation cache entry for
    // the relation. (use RelCacheTable::setRelCatEntry function)

    return SUCCESS;
}
/*
NOTE: This function will copy the result of the search to the `record` argument.
      The caller should ensure that space is allocated for `record` array
      based on the number of attributes in the relation.
*/
int BlockAccess::search(int relId, Attribute *record, char attrName[ATTR_SIZE], Attribute attrVal, int op) {
    // Declare a variable called recid to store the searched record
    RecId recId;

    /* search for the record id (recid) corresponding to the attribute with
    attribute name attrName, with value attrval and satisfying the condition op
    using linearSearch() */
    recId = BlockAccess::linearSearch(relId,attrName,attrVal,op);
    if(recId.block == -1 && recId.slot == -1)return E_NOTFOUND;

    // if there's no record satisfying the given condition (recId = {-1, -1})
    //    return E_NOTFOUND;
    RecBuffer rec(recId.block);
    rec.getRecord(record,recId.slot);
    /* Copy the record with record id (recId) to the record buffer (record)
       For this Instantiate a RecBuffer class object using recId and
       call the appropriate method to fetch the record
    */

    return SUCCESS;
}
int BlockAccess::deleteRelation(char relName[ATTR_SIZE]) {
    char relcatname[ATTR_SIZE];
    char attrcatname[ATTR_SIZE];
    strcpy(relcatname,RELCAT_RELNAME);
    strcpy(attrcatname,ATTRCAT_RELNAME);
    if((!strcmp(relName,relcatname))||(!strcmp(relName,attrcatname)))return E_NOTPERMITTED;
    // if the relation to delete is either Relation Catalog or Attribute Catalog,
    //     return E_NOTPERMITTED
        // (check if the relation names are either "RELATIONCAT" and "ATTRIBUTECAT".
        // you may use the following constants: RELCAT_NAME and ATTRCAT_NAME)

    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
    RelCacheTable::resetSearchIndex(RELCAT_RELID);

    Attribute relNameAttr; // (stores relName as type union Attribute)
    // assign relNameAttr.sVal = relName
    strcpy(relNameAttr.sVal,relName);

    //  linearSearch on the relation catalog for RelName = relNameAttr
    char rnameattr[ATTR_SIZE];
    strcpy(rnameattr,RELCAT_ATTR_RELNAME);
    RecId recId = BlockAccess::linearSearch(RELCAT_RELID,rnameattr,relNameAttr,EQ);
    if(recId.slot == -1 && recId.block == -1)return E_RELNOTEXIST;
    // if the relation does not exist (linearSearch returned {-1, -1})
    //     return E_RELNOTEXIST
    RecBuffer rbuffer(recId.block);
    Attribute relCatEntryRecord[RELCAT_NO_ATTRS];
    rbuffer.getRecord(relCatEntryRecord,recId.slot);
    /* store the relation catalog record corresponding to the relation in
       relCatEntryRecord using RecBuffer.getRecord */
    int blk = (int)relCatEntryRecord[RELCAT_FIRST_BLOCK_INDEX].nVal;
    int nAttr = (int)relCatEntryRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal;
    /* get the first record block of the relation (firstBlock) using the
       relation catalog entry record */
    /* get the number of attributes corresponding to the relation (numAttrs)
       using the relation catalog entry record */

    /*
     Delete all the record blocks of the relation
    */
    // for each record block of the relation:
    //     get block header using BlockBuffer.getHeader
    //     get the next block from the header (rblock)
    //     release the block using BlockBuffer.releaseBlock
    //
    //     Hint: to know if we reached the end, check if nextBlock = -1
    while(blk!=-1)
    {
        RecBuffer buffer(blk);
        HeadInfo head;
        buffer.getHeader(&head);
        blk = head.rblock;
        buffer.releaseBlock();
    }

    /***
        Deleting attribute catalog entries corresponding the relation and index
        blocks corresponding to the relation with relName on its attributes
    ***/

    // reset the searchIndex of the attribute catalog
    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    int numberOfAttributesDeleted = 0;

    while(true) {
        RecId attrCatRecId;
        // attrCatRecId = linearSearch on attribute catalog for RelName = relNameAttr
        attrCatRecId = BlockAccess::linearSearch(ATTRCAT_RELID,rnameattr,relNameAttr,EQ);
        if(attrCatRecId.block == -1 && attrCatRecId.slot == -1)break;
        // if no more attributes to iterate over (attrCatRecId == {-1, -1})
        //     break;

        numberOfAttributesDeleted++;
        RecBuffer rec(attrCatRecId.block);
        HeadInfo header;
        rec.getHeader(&header);

        // create a RecBuffer for attrCatRecId.block
        // get the header of the block
        // get the record corresponding to attrCatRecId.slot

        // declare variable rootBlock which will be used to store the root
        // block field from the attribute catalog record.
        int rootBlock = /* get root block from the record */header.rblock;
        // (This will be used later to delete any indexes if it exists)

        // Update the Slotmap for the block by setting the slot as SLOT_UNOCCUPIED
        // Hint: use RecBuffer.getSlotMap and RecBuffer.setSlotMap
        unsigned char *slotmap = (unsigned char*)malloc(sizeof(unsigned char)*header.numSlots);
        rec.getSlotMap(slotmap);
        slotmap[attrCatRecId.slot] = SLOT_UNOCCUPIED;
        rec.setSlotMap(slotmap);
        /* Decrement the numEntries in the header of the block corresponding to
           the attribute catalog entry and then set back the header
           using RecBuffer.setHeader */
        header.numEntries = header.numEntries-1;

        /* If number of entries become 0, releaseBlock is called after fixing
           the linked list.
        */
        if (header.numEntries == 0/*   header.numEntries == 0  */) {
            /* Standard Linked List Delete for a Block
               Get the header of the left block and set it's rblock to this
               block's rblock
            */
            int lblk = header.lblock;
            RecBuffer Lblk(lblk);
            HeadInfo lhead;
            Lblk.getHeader(&lhead);
            lhead.rblock = header.rblock;
            Lblk.setHeader(&lhead);
            // create a RecBuffer for lblock and call appropriate methods

            if (header.rblock != -1 /* header.rblock != -1 */) {
                /* Get the header of the right block and set it's lblock to
                   this block's lblock */
                int rblk = header.rblock;
                RecBuffer Rblk(rblk);
                HeadInfo rhead;
                Rblk.getHeader(&rhead);
                rhead.lblock = header.lblock;
                Rblk.setHeader(&rhead);
                // create a RecBuffer for rblock and call appropriate methods

            } else {
                // (the block being released is the "Last Block" of the relation.)
                /* update the Relation Catalog entry's LastBlock field for this
                   relation with the block number of the previous block. */
                RelCatEntry relcatEntry;
                RelCacheTable::getRelCatEntry(RELCAT_RELID,&relcatEntry);
                relcatEntry.lastBlk = attrCatRecId.block;
                RelCacheTable::setRelCatEntry(RELCAT_RELID,&relcatEntry);
            }

            // (Since the attribute catalog will never be empty(why?), we do not
            //  need to handle the case of the linked list becoming empty - i.e
            //  every block of the attribute catalog gets released.)
            rec.releaseBlock();
            // call releaseBlock()
        }

        // (the following part is only relevant once indexing has been implemented)
        // if index exists for the attribute (rootBlock != -1), call bplus destroy
        // if (rootBlock != -1) {
        //     // delete the bplus tree rooted at rootBlock using BPlusTree::bPlusDestroy()
        //     BPlusTree::bPlusDestroy(rootBlock);
        // }
    }

    /*** Delete the entry corresponding to the relation from relation catalog ***/
    // Fetch the header of Relcat block
    HeadInfo relcathead;
    RecBuffer relcatblk(recId.block);
    relcatblk.getHeader(&relcathead);
    relcathead.numEntries = relcathead.numEntries-1;
    relcatblk.setHeader(&relcathead);
    unsigned char *slotmap = (unsigned char*)malloc(sizeof(unsigned char)*relcathead.numSlots);\
    relcatblk.getSlotMap(slotmap);
    slotmap[recId.slot] = SLOT_UNOCCUPIED;
    relcatblk.setSlotMap(slotmap);

    /* Decrement the numEntries in the header of the block corresponding to the
       relation catalog entry and set it back */

    /* Get the slotmap in relation catalog, update it by marking the slot as
       free(SLOT_UNOCCUPIED) and set it back. */

    /*** Updating the Relation Cache Table ***/
    RelCatEntry relcatentry;
    RelCacheTable::getRelCatEntry(RELCAT_RELID,&relcatentry);
    relcatentry.numRecs = relcatentry.numRecs-1;
    RelCacheTable::setRelCatEntry(RELCAT_RELID,&relcatentry);
    /** Update relation catalog record entry (number of records in relation
        catalog is decreased by 1) **/
    // Get the entry corresponding to relation catalog from the relation
    // cache and update the number of records and set it back
    // (using RelCacheTable::setRelCatEntry() function)
    RelCacheTable::getRelCatEntry(ATTRCAT_RELID,&relcatentry);
    relcatentry.numRecs = relcatentry.numRecs-numberOfAttributesDeleted;
    RelCacheTable::setRelCatEntry(ATTRCAT_RELID,&relcatentry);
    /** Update attribute catalog entry (number of records in attribute catalog
        is decreased by numberOfAttributesDeleted) **/
    // i.e., #Records = #Records - numberOfAttributesDeleted

    // Get the entry corresponding to attribute catalog from the relation
    // cache and update the number of records and set it back
    // (using RelCacheTable::setRelCatEntry() function)

    return SUCCESS;
}
/*
NOTE: the caller is expected to allocate space for the argument `record` based
      on the size of the relation. This function will only copy the result of
      the projection onto the array pointed to by the argument.
*/
int BlockAccess::project(int relId, Attribute *record) {
    // get the previous search index of the relation relId from the relation
    // cache (use RelCacheTable::getSearchIndex() function)
    RecId prevRecId;
    RelCacheTable::getSearchIndex(relId,&prevRecId);
    // declare block and slot which will be used to store the record id of the
    // slot we need to check.
    int block, slot;

    /* if the current search index record is invalid(i.e. = {-1, -1})
       (this only happens when the caller reset the search index)
    */
    if (prevRecId.block == -1 && prevRecId.slot == -1)
    {
        // (new project operation. start from beginning)

        // get the first record block of the relation from the relation cache
        // (use RelCacheTable::getRelCatEntry() function of Cache Layer)
        RelCatEntry relcatentry;
        RelCacheTable::getRelCatEntry(relId,&relcatentry);
        block = relcatentry.firstBlk;
        slot = 0;
        // block = first record block of the relation
        // slot = 0
    }
    else
    {
        // (a project/search operation is already in progress)
        block = prevRecId.block;
        slot = prevRecId.slot+1;
        // block = previous search index's block
        // slot = previous search index's slot + 1
    }


    // The following code finds the next record of the relation
    /* Start from the record id (block, slot) and iterate over the remaining
       records of the relation */
    while (block != -1)
    {
        RecBuffer buf(block);
        HeadInfo head;
        buf.getHeader(&head);
        // unsigned char *slotmap = (unsigned char*)malloc(sizeof(unsigned char)*head.numSlots);
        unsigned char slotmap[head.numSlots];
        buf.getSlotMap(slotmap);
        // create a RecBuffer object for block (using appropriate constructor!)

        // get header of the block using RecBuffer::getHeader() function
        // get slot map of the block using RecBuffer::getSlotMap() function

        if(/* slot >= the number of slots per block*/slot>=head.numSlots)
        {
            // (no more slots in this block)
            // update block = right block of block
            block = head.rblock;
            slot = 0;
            // update slot = 0
            // (NOTE: if this is the last block, rblock would be -1. this would
            //        set block = -1 and fail the loop condition )
        }
        else if (/* slot is free */slotmap[slot] == SLOT_UNOCCUPIED)
        { // (i.e slot-th entry in slotMap contains SLOT_UNOCCUPIED)
            slot++;
            // increment slot
        }
        else {
            // (the next occupied slot / record has been found)
            break;
        }
    }

    if (block == -1){
        // (a record was not found. all records exhausted)
        return E_NOTFOUND;
    }

    // declare nextRecId to store the RecId of the record found
    RecId nextRecId{block, slot};

    // set the search index to nextRecId using RelCacheTable::setSearchIndex

    RelCacheTable::setSearchIndex(relId,&nextRecId);
    /* Copy the record with record id (nextRecId) to the record buffer (record)
       For this Instantiate a RecBuffer class object by passing the recId and
       call the appropriate method to fetch the record
    */
    RecBuffer blk(block);
    blk.getRecord(record,slot);
    return SUCCESS;
}