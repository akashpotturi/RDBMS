
// #include "BlockAccess.h"
// #include <stdlib.h>
// #include <cstring>
// #include <iostream>
// using namespace std;
// RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {
//     // get the previous search index of the relation relId from the relation cache
//     // (use RelCacheTable::getSearchIndex() function)
//     RecId prevRecId;
//     RelCacheTable::getSearchIndex(relId,&prevRecId);
//     // let block and slot denote the record id of the record being currently checked
//     int block,slot;
//     RelCatEntry buf;
//     // if the current search index record is invalid(i.e. both block and slot = -1)
//     if (prevRecId.block == -1 && prevRecId.slot == -1)
//     {
//         // (no hits from previous search; search should start from the
//         // first record itself)

//         // get the first record block of the relation from the relation cache
//         // (use RelCacheTable::getRelCatEntry() function of Cache Layer)
//         RelCacheTable::getRelCatEntry(relId, &buf);
//         block = buf.firstBlk;
//         slot = 0;

//         // block = first record block of the relation
//         // slot = 0
//     }
//     else
//     {
//         // (there is a hit from previous search; search should start from
//         // the record next to the search index record)
//         block = prevRecId.block;
//         slot = prevRecId.slot+1;
//         // block = search index's block
//         // slot = search index's slot + 1
//     }

//     /* The following code searches for the next record in the relation
//        that satisfies the given condition
//        We start from the record id (block, slot) and iterate over the remaining
//        records of the relation
//     */
//     while (block != -1)
//     {
//         /* create a RecBuffer object for block (use RecBuffer Constructor for
//            existing block) */
//         RecBuffer buffer(block);
//         struct HeadInfo head;
//         buffer.getHeader(&head);
//         int a = head.numAttrs;
//         Attribute record[a];
//         buffer.getRecord(record,slot);
//         unsigned char *slotmap = (unsigned char*)malloc(sizeof(unsigned char)*head.numSlots);
//         buffer.getSlotMap(slotmap);
//         // get the record with id (block, slot) using RecBuffer::getRecord()
//         // get header of the block using RecBuffer::getHeader() function
//         // get slot map of the block using RecBuffer::getSlotMap() function

//         if(slot>=head.numSlots)// If slot >= the number of slots per block(i.e. no more slots in this block)
//         {
//             // update block = right block of block
//             block = head.rblock;
//             slot = 0;
//             // update slot = 0
//             continue;  // continue to the beginning of this while loop
//         }

//         // if slot is free skip the loop
//         if(slotmap[slot] == SLOT_UNOCCUPIED)// (i.e. check if slot'th entry in slot map of block contains SLOT_UNOCCUPIED)
//         {
//             // increment slot and continue to the next record slot
//             slot++;
//             continue;
//         }
        

//         // compare record's attribute value to the the given attrVal as below:
//         /*
//             firstly get the attribute offset for the attrName attribute
//             from the attribute cache entry of the relation using
//             AttrCacheTable::getAttrCatEntry()
//         */
//        AttrCatEntry buf;
//        AttrCacheTable::getAttrCatEntry(relId,attrName,&buf);
//         /* use the attribute offset to get the value of the attribute from
//            current record */
//         Attribute attr2 = record[buf.offset];

//         int cmpVal = compareAttrs(attr2, attrVal, buf.attrType);  // will store the difference between the attributes
//         // set cmpVal using compareAttrs()

//         /* Next task is to check whether this record satisfies the given condition.
//            It is determined based on the output of previous comparison and
//            the op value received.
//            The following code sets the cond variable if the condition is satisfied.
//         */
//         if (
//             (op == NE && cmpVal != 0) ||    // if op is "not equal to"
//             (op == LT && cmpVal < 0) ||     // if op is "less than"
//             (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
//             (op == EQ && cmpVal == 0) ||    // if op is "equal to"
//             (op == GT && cmpVal > 0) ||     // if op is "greater than"
//             (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
//         ) {
//             /*
//             set the search index in the relation cache as
//             the record id of the record that satisfies the given condition
//             (use RelCacheTable::setSearchIndex function)
//             */
//             RecId rec;
//             rec.block = block;
//             rec.slot = slot;
//             RelCacheTable::setSearchIndex(relId,&rec);
//             return rec;
//         }

//         slot++;
//     }

//     // no record in the relation with Id relid satisfies the given condition
//     return RecId{-1, -1};
// }

// int BlockAccess::renameRelation(char oldName[ATTR_SIZE], char newName[ATTR_SIZE]){
//     /* reset the searchIndex of the relation catalog using
//        RelCacheTable::resetSearchIndex() */
//     RelCacheTable::resetSearchIndex(RELCAT_RELID);
//     Attribute newRelationName;    // set newRelationName with newName
//     strcpy(newRelationName.sVal,newName);
//     char name[ATTR_SIZE];
//     strcpy(name,RELCAT_ATTR_RELNAME);
//     // search the relation catalog for an entry with "RelName" = newRelationName
//     RecId recId = BlockAccess::linearSearch(RELCAT_RELID,name,newRelationName,EQ);
//     if(recId.block != -1 && recId.slot != -1)return E_RELEXIST;
//     // If relation with name newName already exists (result of linearSearch
//     //                                               is not {-1, -1})
//     //    return E_RELEXIST;


//     /* reset the searchIndex of the relation catalog using
//        RelCacheTable::resetSearchIndex() */
//     RelCacheTable::resetSearchIndex(RELCAT_RELID);
//     Attribute oldRelationName;    // set oldRelationName with oldName
//     strcpy(oldRelationName.sVal,oldName);
//     recId = BlockAccess::linearSearch(RELCAT_RELID,name,oldRelationName,EQ);
//     if(recId.block == -1 && recId.slot == -1)return E_RELNOTEXIST;

//     // search the relation catalog for an entry with "RelName" = oldRelationName

//     // If relation with name oldName does not exist (result of linearSearch is {-1, -1})
//     //    return E_RELNOTEXIST;

//     /* get the relation catalog record of the relation to rename using a RecBuffer
//        on the relation catalog [RELCAT_BLOCK] and RecBuffer.getRecord function
//     */
//     RecBuffer buf(RELCAT_BLOCK);
//     Attribute reco[RELCAT_NO_ATTRS];
//     buf.getRecord(reco,recId.slot);
    

//     strcpy(reco[RELCAT_REL_NAME_INDEX].sVal,newName);
//     /* update the relation name attribute in the record with newName.
//        (use RELCAT_REL_NAME_INDEX) */
//     // set back the record value using RecBuffer.setRecord
//     buf.setRecord(reco,recId.slot);

//     /*
//     update all the attribute catalog entries in the attribute catalog corresponding
//     to the relation with relation name oldName to the relation name newName
//     */
//     RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
//     int numAttr = (int)reco[RELCAT_NO_ATTRIBUTES_INDEX].nVal;
//     Attribute val;
//     strcpy(val.sVal,oldName);
//     /* reset the searchIndex of the attribute catalog using
//        RelCacheTable::resetSearchIndex() */
//     for(int i = 0;i<numAttr;i++)
//     {
//         recId = linearSearch(ATTRCAT_RELID,name,val,EQ);
//         RecBuffer b(recId.block);
//         b.getRecord(reco,recId.slot);
//         strcpy(reco[ATTRCAT_REL_NAME_INDEX].sVal,newName);
//         b.setRecord(reco,recId.slot);
//     }
//     //for i = 0 to numberOfAttributes :
//     //    linearSearch on the attribute catalog for relName = oldRelationName
//     //    get the record using RecBuffer.getRecord
//     //
//     //    update the relName field in the record to newName
//     //    set back the record using RecBuffer.setRecord

//     return SUCCESS;
// }

// int BlockAccess::renameAttribute(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE]) {

//     /* reset the searchIndex of the relation catalog using
//        RelCacheTable::resetSearchIndex() */
//     RelCacheTable::resetSearchIndex(RELCAT_RELID);

//     Attribute relNameAttr;    // set relNameAttr to relName
//     strcpy(relNameAttr.sVal,relName);
//     char name[ATTR_SIZE];
//     strcpy(name,ATTRCAT_ATTR_RELNAME);
//     RecId recId = BlockAccess::linearSearch(RELCAT_RELID,name,relNameAttr,EQ);
//     if(recId.block == -1 && recId.slot == -1)return E_RELNOTEXIST;
//     // Search for the relation with name relName in relation catalog using linearSearch()
//     // If relation with name relName does not exist (search returns {-1,-1})
//     //    return E_RELNOTEXIST;
//     /* reset the searchIndex of the attribute catalog using
//        RelCacheTable::resetSearchIndex() */
    
//     /* declare variable attrToRenameRecId used to store the attr-cat recId
//     of the attribute to rename */
//     Attribute attrCatEntryRecord[ATTRCAT_NO_ATTRS];
//     RecId attrToRenameRecId{-1, -1};
//     AttrCacheEntry attr;
//     RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
//     // AttrCacheTable::getAttrCatEntry(recId,)
//     /* iterate over all Attribute Catalog Entry record corresponding to the
//        relation to find the required attribute */
//     while (true) {
//         // linear search on the attribute catalog for RelName = relNameAttr
//         recId = BlockAccess::linearSearch(ATTRCAT_RELID,name,relNameAttr,EQ);
//         if(recId.block == -1 && recId.slot == -1)break;
//         // if there are no more attributes left to check (linearSearch returned {-1,-1})
//         //     break;

//         /* Get the record from the attribute catalog using RecBuffer.getRecord
//           into attrCatEntryRecord */
//         RecBuffer r(recId.block);
//         r.getRecord(attrCatEntryRecord,recId.slot);
//         if(!strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,oldName))
//         {
//             attrToRenameRecId.block = recId.block;
//             attrToRenameRecId.slot = recId.slot;
//         }
//         if(!strcmp(attrCatEntryRecord[1].sVal,newName))
//         {
//             return E_ATTREXIST;
//         }


//         // if attrCatEntryRecord.attrName = oldName
//         //     attrToRenameRecId = block and slot of this record

//         // if attrCatEntryRecord.attrName = newName
//         //     return E_ATTREXIST;
//     }

//     // if attrToRenameRecId == {-1, -1}
//     //     return E_ATTRNOTEXIST;
//     if(attrToRenameRecId.block == -1 && attrToRenameRecId.slot == -1)return E_ATTRNOTEXIST;
//     RecBuffer rec(attrToRenameRecId.block);
//     Attribute record[ATTRCAT_NO_ATTRS];
//     rec.getRecord(record,attrToRenameRecId.slot);
//     strcpy(record[ATTRCAT_ATTR_NAME_INDEX].sVal,newName);
//     rec.setRecord(record,attrToRenameRecId.slot);

//     // Update the entry corresponding to the attribute in the Attribute Catalog Relation.
//     /*   declare a RecBuffer for attrToRenameRecId.block and get the record at
//          attrToRenameRecId.slot */
//     //   update the AttrName of the record with newName
//     //   set back the record with RecBuffer.setRecord

//     return SUCCESS;
// }
// int BlockAccess::insert(int relId, Attribute *record) {
//     // get the relation catalog entry from relation cache
//     // ( use RelCacheTable::getRelCatEntry() of Cache Layer)
//     RelCatEntry relcatEntry;
//     RelCacheTable::getRelCatEntry(relId,&relcatEntry);

//     int blockNum = /* first record block of the relation (from the rel-cat entry)*/relcatEntry.firstBlk;

//     // rec_id will be used to store where the new record will be inserted
//     RecId rec_id = {-1, -1};

//     int numOfSlots = /* number of slots per record block */relcatEntry.numSlotsPerBlk;
//     int numOfAttributes = /* number of attributes of the relation */relcatEntry.numAttrs;

//     int prevBlockNum = /* block number of the last element in the linked list = -1 */-1;

//     /*
//         Traversing the linked list of existing record blocks of the relation
//         until a free slot is found OR
//         until the end of the list is reached
//     */
//     while (blockNum != -1) {
//         // create a RecBuffer object for blockNum (using appropriate constructor!)
//         RecBuffer rec(blockNum);
//         struct HeadInfo head;
//         rec.getHeader(&head);
//         unsigned char *slotmap = (unsigned char*)malloc(sizeof(unsigned char)*head.numSlots);
//         rec.getSlotMap(slotmap);

//         // get header of block(blockNum) using RecBuffer::getHeader() function

//         // get slot map of block(blockNum) using RecBuffer::getSlotMap() function

//         // search for free slot in the block 'blockNum' and store it's rec-id in rec_id
//         // (Free slot can be found by iterating over the slot map of the block)
//         /* slot map stores SLOT_UNOCCUPIED if slot is free and
//            SLOT_OCCUPIED if slot is occupied) */
//         int freeslot = -1;
//         for(int i = 0;i<head.numSlots;i++)
//         {
//             if(slotmap[i] == SLOT_UNOCCUPIED)
//             {
//                 freeslot = i;
//                 break;
//             }
//         }
//         if(freeslot != -1)
//         {
//             rec_id.block = blockNum;
//             rec_id.slot = freeslot;
//             break;
//         }
//         /* if a free slot is found, set rec_id and discontinue the traversal
//            of the linked list of record blocks (break from the loop) */
//         else
//         {
//             prevBlockNum = blockNum;
//             blockNum = head.rblock;
//         }
//         /* otherwise, continue to check the next block by updating the
//            block numbers as follows:
//               update prevBlockNum = blockNum
//               update blockNum = header.rblock (next element in the linked
//                                                list of record blocks)
//         */
//     }

//     if(rec_id.block == -1 && rec_id.slot == -1)//  if no free slot is found in existing record blocks (rec_id = {-1, -1})
//     {
//         // if relation is RELCAT, do not allocate any more blocks
//         //     return E_MAXRELATIONS;
//         char relcatName[ATTR_SIZE];
//         strcpy(relcatName,RELCAT_RELNAME);
//         if(!strcmp(relcatName,relcatEntry.relName))return E_MAXRELATIONS;

//         // Otherwise,
//         // get a new record block (using the appropriate RecBuffer constructor!)
//         RecBuffer block;
//         int ret = block.getBlockNum();
//         // get the block number of the newly allocated block
//         // (use BlockBuffer::getBlockNum() function)
//         // let ret be the return value of getBlockNum() function call
//         if (ret == E_DISKFULL) {
//             return E_DISKFULL;
//         }

//         // Assign rec_id.block = new block number(i.e. ret) and rec_id.slot = 0
//         rec_id.block = ret;
//         rec_id.slot = 0;
//         struct HeadInfo header;
//         header.blockType = REC;
//         header.pblock = -1;
//         header.lblock = -1;
//         header.rblock = -1;
//         header.numEntries = 0;
//         header.numSlots = numOfSlots;
//         header.numAttrs = numOfAttributes;
//         block.setHeader(&header);

//         /*
//             set the header of the new record block such that it links with
//             existing record blocks of the relation
//             set the block's header as follows:
//             blockType: REC, pblock: -1
//             lblock
//                   = -1 (if linked list of existing record blocks was empty
//                          i.e this is the first insertion into the relation)
//                   = prevBlockNum (otherwise),
//             rblock: -1, numEntries: 0,
//             numSlots: numOfSlots, numAttrs: numOfAttributes
//             (use BlockBuffer::setHeader() function)
//         */
//         unsigned char *slotmap = (unsigned char*)malloc(sizeof(unsigned char)*header.numSlots);
//         for(int i = 0;i<header.numSlots;i++)
//         {
//             slotmap[i] = SLOT_UNOCCUPIED;
//         }
//         block.setSlotMap(slotmap);
//         /*
//             set block's slot map with all slots marked as free
//             (i.e. store SLOT_UNOCCUPIED for all the entries)
//             (use RecBuffer::setSlotMap() function)
//         */

//         if(prevBlockNum != -1)// if prevBlockNum != -1
//         {
//             // create a RecBuffer object for prevBlockNum
//             RecBuffer buf(prevBlockNum);
//             // get the header of the block prevBlockNum and
//             struct HeadInfo prevhead;
//             buf.getHeader(&prevhead);
//             prevhead.rblock = rec_id.block;
//             // update the rblock field of the header to the new block
//             // number i.e. rec_id.block
//             buf.setHeader(&prevhead);
//             // (use BlockBuffer::setHeader() function)
//         }
//         // else
//         else
//         {
//             // update first block field in the relation catalog entry to the
//             // new block (using RelCacheTable::setRelCatEntry() function)
//             relcatEntry.firstBlk = rec_id.block;
//             RelCacheTable::setRelCatEntry(relId,&relcatEntry);
//         }

//         // update last block field in the relation catalog entry to the
//         // new block (using RelCacheTable::setRelCatEntry() function)
//         relcatEntry.lastBlk = rec_id.block;
//         RelCacheTable::setRelCatEntry(relId,&relcatEntry);
//     }

//     // create a RecBuffer object for rec_id.block
//     RecBuffer buffer(rec_id.block);
//     buffer.setRecord(record,rec_id.slot);
//     // insert the record into rec_id'th slot using RecBuffer.setRecord())
//     unsigned char *slotmap2 = (unsigned char*)malloc(sizeof(unsigned char)*numOfSlots);
//     buffer.getSlotMap(slotmap2);
//     slotmap2[rec_id.slot] = SLOT_OCCUPIED;
//     buffer.setSlotMap(slotmap2);
//     /* update the slot map of the block by marking entry of the slot to
//        which record was inserted as occupied) */
//     // (ie store SLOT_OCCUPIED in free_slot'th entry of slot map)
//     // (use RecBuffer::getSlotMap() and RecBuffer::setSlotMap() functions)
//     struct HeadInfo header1;
//     buffer.getHeader(&header1);
//     header1.numEntries = header1.numEntries + 1;
//     buffer.setHeader(&header1);
//     // increment the numEntries field in the header of the block to
//     // which record was inserted
//     // (use BlockBuffer::getHeader() and BlockBuffer::setHeader() functions)
//     relcatEntry.numRecs = relcatEntry.numRecs+1;
//     RelCacheTable::setRelCatEntry(relId,&relcatEntry);
//     // Increment the number of records field in the relation cache entry for
//     // the relation. (use RelCacheTable::setRelCatEntry function)

//     return SUCCESS;
// }
// /*
// NOTE: the caller is expected to allocate space for the argument `record` based
//       on the size of the relation. This function will only copy the result of
//       the projection onto the array pointed to by the argument.
// */
// int BlockAccess::project(int relId, Attribute *record) {
//     // get the previous search index of the relation relId from the relation
//     // cache (use RelCacheTable::getSearchIndex() function)
//     RecId prevRecId;
//     RelCacheTable::getSearchIndex(relId,&prevRecId);

//     // declare block and slot which will be used to store the record id of the
//     // slot we need to check.
//     int block, slot;

//     /* if the current search index record is invalid(i.e. = {-1, -1})
//        (this only happens when the caller reset the search index)
//     */
//     if (prevRecId.block == -1 && prevRecId.slot == -1)
//     {
//         // (new project operation. start from beginning)
//         RelCatEntry relcatEntry;
//         RelCacheTable::getRelCatEntry(relId,&relcatEntry);
//         block = relcatEntry.firstBlk;
//         slot = 0;
//         // get the first record block of the relation from the relation cache
//         // (use RelCacheTable::getRelCatEntry() function of Cache Layer)

//         // block = first record block of the relation
//         // slot = 0
//     }
//     else
//     {
//         // (a project/search operation is already in progress)
//         block = prevRecId.block;
//         slot = slot+1;
//         // block = previous search index's block
//         // slot = previous search index's slot + 1
//     }


//     // The following code finds the next record of the relation
//     /* Start from the record id (block, slot) and iterate over the remaining
//        records of the relation */
//     while (block != -1)
//     {
//         // create a RecBuffer object for block (using appropriate constructor!)
//         RecBuffer buffer(block);
//         HeadInfo head;
//         buffer.getHeader(&head);
//         unsigned char *slotmap = (unsigned char*)malloc(sizeof(unsigned char)*head.numSlots);
//         buffer.getSlotMap(slotmap);

//         // get header of the block using RecBuffer::getHeader() function
//         // get slot map of the block using RecBuffer::getSlotMap() function

//         if(/* slot >= the number of slots per block*/slot>=head.numSlots)
//         {
//             // (no more slots in this block)
//             block = head.rblock;
//             slot = 0;
//             // update block = right block of block
//             // update slot = 0
//             // (NOTE: if this is the last block, rblock would be -1. this would
//             //        set block = -1 and fail the loop condition )
//         }
//         else if (/* slot is free */slotmap[slot] == SLOT_UNOCCUPIED)
//         { // (i.e slot-th entry in slotMap contains SLOT_UNOCCUPIED)
//             slot++;
//             // increment slot
//         }
//         else {
//             // (the next occupied slot / record has been found)
//             break;
//         }
//     }

//     if (block == -1){
//         // (a record was not found. all records exhausted)
//         return E_NOTFOUND;
//     }

//     // declare nextRecId to store the RecId of the record found
//     RecId nextRecId{block, slot};
//     nextRecId.block = block;
//     nextRecId.slot = slot;
//     // set the search index to nextRecId using RelCacheTable::setSearchIndex
//     RelCacheTable::setSearchIndex(relId,&nextRecId);
//     RecBuffer buf(nextRecId.block);
//     /* Copy the record with record id (nextRecId) to the record buffer (record)
//        For this Instantiate a RecBuffer class object by passing the recId and
//        call the appropriate method to fetch the record
//     */

//     return SUCCESS;
// }
#include "BlockAccess.h"

#include <cstring>

inline bool operator == (RecId lhs, RecId rhs) {
	return (lhs.block == rhs.block && lhs.slot == rhs.slot);
}

inline bool operator != (RecId lhs, RecId rhs) {
	return (lhs.block != rhs.block || lhs.slot != rhs.slot);
}

RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op)
{
	// get the previous search index of the relation relId from the relation cache
	// (use RelCacheTable::getSearchIndex() function)
	RecId prevRecId;
	RelCacheTable::getSearchIndex(relId, &prevRecId);

	// let block and slot denote the record id of the record being currently checked
	int block = -1, slot = -1;

	// if the current search index record is invalid(i.e. both block and slot = -1)
	if (prevRecId.block == -1 && prevRecId.slot == -1)
	{
		//* no hits from previous search; 
		//* search should start from the first record itself

		// get the first record block of the relation from the relation cache
		// (use RelCacheTable::getRelCatEntry() function of Cache Layer)
		RelCatEntry relCatBuffer;
		RelCacheTable::getRelCatEntry(relId, &relCatBuffer);

		// block = first block of the relation,
		// slot = 0 (start at the first slot)
		block = relCatBuffer.firstBlk, slot = 0;
	}
	else
	{
		//* there is a hit from previous search; search should start from
		//* the record next to the search index record

		// block = search index's block
		// slot = search index's slot + 1
		block = prevRecId.block, slot = prevRecId.slot + 1;
	}

	/* The following code searches for the next record in the relation
	   that satisfies the given condition:
		* "We start from the record id (block, slot) and iterate over the remaining
		* records of the relation"
	*/

	RelCatEntry relCatBuffer;
	RelCacheTable::getRelCatEntry(relId, &relCatBuffer);
	while (block != -1)
	{
		// TODO: create a RecBuffer object for block (use RecBuffer Constructor for existing block)
		RecBuffer blockBuffer(block);

		// TODO: get header of the block using RecBuffer::getHeader() function
		HeadInfo blockHeader;
		blockBuffer.getHeader(&blockHeader);

		// TODO: get slot map of the block using RecBuffer::getSlotMap() function
		unsigned char slotMap[blockHeader.numSlots];
		blockBuffer.getSlotMap(slotMap);

		// If slot >= the number of slots per block(i.e. no more slots in this block)
		if (slot >= relCatBuffer.numSlotsPerBlk)
		{
			// TODO: update block = right block of block, update slot = 0
			block = blockHeader.rblock, slot = 0;
			continue; // continue to the beginning of this while loop
		}

		// if slot is free skip the loop
		// (i.e. check if slot'th entry in slot map of block contains SLOT_UNOCCUPIED)
		if (slotMap[slot] == SLOT_UNOCCUPIED)
		{
			slot++;
			continue;
		}

		// TODO: get the record with id (block, slot) using RecBuffer::getRecord()
		Attribute record[blockHeader.numAttrs];
		blockBuffer.getRecord(record, slot);

		// TODO: compare record's attribute value to the the given attrVal as below:
		//* firstly get the attribute offset for the attrName attribute
		//* from the attribute cache entry of the relation using
		//* AttrCacheTable::getAttrCatEntry()

		AttrCatEntry attrCatBuffer;
		AttrCacheTable::getAttrCatEntry(relId, attrName, &attrCatBuffer);

		// use the attribute offset to get the value of the attribute from current record
		int attrOffset = attrCatBuffer.offset;

		// will store the difference between the attributes 
		// set cmpVal using compareAttrs()
		int cmpVal = compareAttrs(record[attrOffset], attrVal, attrCatBuffer.attrType); 

		/* 
		TODO: check whether this record satisfies the given condition.
		* It is determined based on the output of previous comparison and the op value received.
		* The following code sets the cond variable if the condition is satisfied.
		*/
		if (
			(op == NE && cmpVal != 0) || // if op is "not equal to"
			(op == LT && cmpVal < 0) ||	 // if op is "less than"
			(op == LE && cmpVal <= 0) || // if op is "less than or equal to"
			(op == EQ && cmpVal == 0) || // if op is "equal to"
			(op == GT && cmpVal > 0) ||	 // if op is "greater than"
			(op == GE && cmpVal >= 0)	 // if op is "greater than or equal to"
		)
		{
			// TODO: set the search index in the relation cache as
			// TODO: the record id of the record that satisfies the given condition
			// (use RelCacheTable::setSearchIndex function)
			RecId newRecId = {block, slot};
			RelCacheTable::setSearchIndex(relId, &newRecId);

			return RecId{block, slot};
		}

		slot++;
	}

	//! no record in the relation with Id relid satisfies the given condition
	return RecId{-1, -1};
}

int BlockAccess::renameRelation(char oldName[ATTR_SIZE], char newName[ATTR_SIZE]){
    // TODO: reset the searchIndex of the relation catalog using RelCacheTable::resetSearchIndex() 
	RelCacheTable::resetSearchIndex(RELCAT_RELID);

	// TODO: set newRelationName with newName
    Attribute newRelationName;    
	strcpy(newRelationName.sVal, newName);

    // TODO: search the relation catalog for an entry with "RelName" = newRelationName
	RecId searchIndex = BlockAccess::linearSearch(RELCAT_RELID, RELCAT_ATTR_RELNAME, newRelationName, EQ);

    //! If relation with name newName already exists (result of linearSearch is not {-1, -1})
	if (searchIndex != RecId{-1, -1})
       return E_RELEXIST;


    // reset the searchIndex of the relation catalog using RelCacheTable::resetSearchIndex)
	RelCacheTable::resetSearchIndex(RELCAT_RELID);

	// set oldRelationName with oldName
    Attribute oldRelationName;
	strcpy(oldRelationName.sVal, oldName);

    // search the relation catalog for an entry with "RelName" = oldRelationName
	searchIndex = BlockAccess::linearSearch(RELCAT_RELID, RELCAT_ATTR_RELNAME, oldRelationName, EQ);

    //! If relation with name oldName does not exist (result of linearSearch is {-1, -1})
	if (searchIndex == RecId{-1, -1})
       return E_RELNOTEXIST;

    // TODO: get the relation catalog record of the relation to rename using a RecBuffer
    // TODO: on the relation catalog [RELCAT_BLOCK] and RecBuffer.getRecord function
	RecBuffer relCatBlock (RELCAT_BLOCK);
	
	Attribute relCatRecord [RELCAT_NO_ATTRS];
	relCatBlock.getRecord(relCatRecord, searchIndex.slot);

    // TODO: update the relation name attribute in the record with newName.
	strcpy(relCatRecord[RELCAT_REL_NAME_INDEX].sVal, newName);

    // TODO: set back the record value using RecBuffer.setRecord
	relCatBlock.setRecord(relCatRecord, searchIndex.slot);

	// TODO: update all the attribute catalog entries in the attribute catalog corresponding
	// TODO: to the relation with relation name oldName to the relation name newName

    // reset the searchIndex of the attribute catalog using RelCacheTable::resetSearchIndex()
	RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

    //for i = 0 to numberOfAttributes :
	for (int attrIndex = 0; attrIndex < relCatRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal; attrIndex++) {
		//    linearSearch on the attribute catalog for relName = oldRelationName
		//    get the record using RecBuffer.getRecord
		searchIndex = BlockAccess::linearSearch(ATTRCAT_RELID, ATTRCAT_ATTR_RELNAME, oldRelationName, EQ);
		RecBuffer attrCatBlock (searchIndex.block);

		Attribute attrCatRecord [ATTRCAT_NO_ATTRS];
		attrCatBlock.getRecord(attrCatRecord, searchIndex.slot);

		//    update the relName field in the record to newName
		//    set back the record using RecBuffer.setRecord

		strcpy(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal, newName);
		attrCatBlock.setRecord(attrCatRecord, searchIndex.slot);
	}

    return SUCCESS;
}

int BlockAccess::renameAttribute(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE]) {
    // reset the searchIndex of the relation catalog using RelCacheTable::resetSearchIndex()
	RelCacheTable::resetSearchIndex(RELCAT_RELID);

	// set relNameAttr to relName
    Attribute relNameAttr;
	strcpy(relNameAttr.sVal, relName);

	// Search for the relation with name relName in relation catalog using linearSearch()
	RecId searchIndex = BlockAccess::linearSearch(RELCAT_RELID, RELCAT_ATTR_RELNAME, relNameAttr, EQ);
    
	//! If relation with name relName does not exist (search returns {-1,-1})
	if (searchIndex == RecId{-1, -1})
       return E_RELNOTEXIST;
	
    // reset the searchIndex of the attribute catalog using RelCacheTable::resetSearchIndex()
	RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

    // declare variable attrToRenameRecId used to store the attr-cat recId of the attribute to rename
    RecId attrToRenameRecId{-1, -1};
    // Attribute attrCatEntryRecord[ATTRCAT_NO_ATTRS];

    // TODO: iterate over all Attribute Catalog Entry record corresponding to the
    // TODO: relation to find the required attribute
    while (true) {
        // linear search on the attribute catalog for RelName = relNameAttr
		searchIndex = BlockAccess::linearSearch(ATTRCAT_RELID, ATTRCAT_ATTR_RELNAME, relNameAttr, EQ);

        // if there are no more attributes left to check (linearSearch returned {-1,-1})
		if (searchIndex == RecId{-1, -1}) break;

        // TODO: Get the record from the attribute catalog using 
		// TODO: RecBuffer.getRecord into attrCatEntryRecord
		RecBuffer attrCatBlock (searchIndex.block);

		Attribute attrCatRecord [ATTRCAT_NO_ATTRS];
		attrCatBlock.getRecord(attrCatRecord, searchIndex.slot);

        // if attrCatEntryRecord.attrName = oldName
		if (strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, oldName) == 0){
			attrToRenameRecId = searchIndex;
			break;
		}

        //! if attrCatEntryRecord.attrName = newName
		if (strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, newName) == 0)
            return E_ATTREXIST;
    }

	// if attribute with the old name does not exist
    if (attrToRenameRecId == RecId{-1, -1})
        return E_ATTRNOTEXIST;

    // Update the entry corresponding to the attribute in the Attribute Catalog Relation.
    /*   declare a RecBuffer for attrToRenameRecId.block and get the record at
         attrToRenameRecId.slot */
    //   update the AttrName of the record with newName
    //   set back the record with RecBuffer.setRecord

	RecBuffer attrCatBlock (attrToRenameRecId.block);
	Attribute attrCatRecord [ATTRCAT_NO_ATTRS];
	attrCatBlock.getRecord(attrCatRecord, attrToRenameRecId.slot);
	
	strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,newName );
	attrCatBlock.setRecord(attrCatRecord, attrToRenameRecId.slot);

    return SUCCESS;
}

int BlockAccess::insert(int relId, Attribute *record) {
    // get the relation catalog entry from relation cache
    // ( use RelCacheTable::getRelCatEntry() of Cache Layer)
	RelCatEntry relCatEntry;
	RelCacheTable::getRelCatEntry(relId, &relCatEntry);

    int blockNum = relCatEntry.firstBlk;

    // rec_id will be used to store where the new record will be inserted
    RecId rec_id = {-1, -1};

    int numOfSlots = relCatEntry.numSlotsPerBlk;
    int numOfAttributes = relCatEntry.numAttrs;

	// block number of the last element in the linked list = -1 
    int prevBlockNum = -1;

	// Traversing the linked list of existing record blocks of the relation
	// until a free slot is found OR until the end of the list is reached

    while (blockNum != -1) {
        // create a RecBuffer object for blockNum (using appropriate constructor!)
		RecBuffer blockBuffer (blockNum);

        // get header of block(blockNum) using RecBuffer::getHeader() function
		HeadInfo blockHeader;
		blockBuffer.getHeader(&blockHeader);

        // get slot map of block(blockNum) using RecBuffer::getSlotMap() function
		int numSlots = blockHeader.numSlots;
		unsigned char slotMap [numSlots];
		blockBuffer.getSlotMap(slotMap);

        // search for free slot in the block 'blockNum' and store it's rec-id in rec_id
        // (Free slot can be found by iterating over the slot map of the block)
		int slotIndex = 0;
		for (; slotIndex < numSlots; slotIndex++) {
        	// if a free slot is found, set rec_id and discontinue the traversal
           	// of the linked list of record blocks (break from the loop) 
			//* slot map stores SLOT_UNOCCUPIED if slot is free and SLOT_OCCUPIED if slot is occupied
			if (slotMap[slotIndex] == SLOT_UNOCCUPIED) {
				rec_id = RecId{blockNum, slotIndex};
				break;
			}
		}

		if (rec_id != RecId{-1, -1}) break;

        /* otherwise, continue to check the next block by updating the
           block numbers as follows:
              update prevBlockNum = blockNum
              update blockNum = header.rblock (next element in the linked list of record blocks)
        */
	   prevBlockNum = blockNum;
	   blockNum = blockHeader.rblock;
    }

    //  if no free slot is found in existing record blocks (rec_id = {-1, -1})
	if (rec_id == RecId{-1, -1})
    {
        // if relation is RELCAT, do not allocate any more blocks
        //     return E_MAXRELATIONS;
		if (relId == RELCAT_RELID) return E_MAXRELATIONS;

        // Otherwise,
        // get a new record block (using the appropriate RecBuffer constructor!)
		RecBuffer blockBuffer;

        // get the block number of the newly allocated block
        // (use BlockBuffer::getBlockNum() function)
        blockNum = blockBuffer.getBlockNum();
		
		// let ret be the return value of getBlockNum() function call
        if (blockNum == E_DISKFULL) return E_DISKFULL;

        // Assign rec_id.block = new block number(i.e. ret) and rec_id.slot = 0
		rec_id = RecId {blockNum, 0};

		// TODO: set the header of the new record block such that it links with
		// TODO: existing record blocks of the relation
		// TODO: set the block's header as follows:
		// blockType: REC, pblock: -1
		// lblock = -1 (if linked list of existing record blocks was empty
		// 				i.e this is the first insertion into the relation)
		// 		= prevBlockNum (otherwise),
		// rblock: -1, numEntries: 0,
		// numSlots: numOfSlots, numAttrs: numOfAttributes
		// (use BlockBuffer::setHeader() function)
        
		HeadInfo blockHeader;
		blockHeader.blockType = REC;
		blockHeader.lblock = prevBlockNum, blockHeader.rblock = blockHeader.pblock = -1;
		blockHeader.numAttrs = numOfAttributes, blockHeader.numSlots = numOfSlots, blockHeader.numEntries = 0;

		blockBuffer.setHeader(&blockHeader);
        /*
            set block's slot map with all slots marked as free
            (i.e. store SLOT_UNOCCUPIED for all the entries)
            (use RecBuffer::setSlotMap() function)
        */
	   	unsigned char slotMap [numOfSlots];
		for (int slotIndex = 0; slotIndex < numOfSlots; slotIndex++)
			slotMap[slotIndex] = SLOT_UNOCCUPIED;

		blockBuffer.setSlotMap(slotMap);

        // if prevBlockNum != -1
		if (prevBlockNum != -1)
        {
            // TODO: create a RecBuffer object for prevBlockNum
			RecBuffer prevBlockBuffer (prevBlockNum);

            // TODO: get the header of the block prevBlockNum and
			HeadInfo prevBlockHeader;
			prevBlockBuffer.getHeader(&prevBlockHeader);

            // TODO: update the rblock field of the header to the new block
			prevBlockHeader.rblock = blockNum;
            // number i.e. rec_id.block
            // (use BlockBuffer::setHeader() function)
			prevBlockBuffer.setHeader(&prevBlockHeader);
        }
        else
        {
            // update first block field in the relation catalog entry to the
            // new block (using RelCacheTable::setRelCatEntry() function)
			relCatEntry.firstBlk = blockNum;
			RelCacheTable::setRelCatEntry(relId, &relCatEntry);
        }

        // update last block field in the relation catalog entry to the
        // new block (using RelCacheTable::setRelCatEntry() function)
		relCatEntry.lastBlk = blockNum;
		RelCacheTable::setRelCatEntry(relId, &relCatEntry);
    }

    // create a RecBuffer object for rec_id.block
    RecBuffer blockBuffer (rec_id.block);

	// insert the record into rec_id'th slot using RecBuffer.setRecord())
	blockBuffer.setRecord(record, rec_id.slot);

    /* update the slot map of the block by marking entry of the slot to
       which record was inserted as occupied) */
    // (ie store SLOT_OCCUPIED in free_slot'th entry of slot map)
    // (use RecBuffer::getSlotMap() and RecBuffer::setSlotMap() functions)
	unsigned char slotmap [numOfSlots];
	blockBuffer.getSlotMap(slotmap);

	slotmap[rec_id.slot] = SLOT_OCCUPIED;
	blockBuffer.setSlotMap(slotmap);

    // increment the numEntries field in the header of the block to
    // which record was inserted
    // (use BlockBuffer::getHeader() and BlockBuffer::setHeader() functions)
	HeadInfo blockHeader;
	blockBuffer.getHeader(&blockHeader);

	blockHeader.numEntries++;
	blockBuffer.setHeader(&blockHeader);

    // Increment the number of records field in the relation cache entry for
    // the relation. (use RelCacheTable::setRelCatEntry function)
	relCatEntry.numRecs++;
	RelCacheTable::setRelCatEntry(relId, &relCatEntry);

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
	recId = BlockAccess::linearSearch(relId, attrName, attrVal, op);

    // if there's no record satisfying the given condition (recId = {-1, -1})
	if (recId == RecId{-1, -1})
       return E_NOTFOUND;

    /* Copy the record with record id (recId) to the record buffer (record)
       For this Instantiate a RecBuffer class object using recId and
       call the appropriate method to fetch the record
    */

   	RecBuffer blockBuffer (recId.block);
   	blockBuffer.getRecord(record, recId.slot);

    return SUCCESS;
}

int BlockAccess::deleteRelation(char relName[ATTR_SIZE]) {
    // if the relation to delete is either Relation Catalog or Attribute Catalog
	// (check if the relation names are either "RELATIONCAT" and "ATTRIBUTECAT".
	// you may use the following constants: RELCAT_NAME and ATTRCAT_NAME)
	if (strcmp(relName, RELCAT_RELNAME) == 0 || strcmp(relName, ATTRCAT_RELNAME) == 0)
        return E_NOTPERMITTED;

    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
	RelCacheTable::resetSearchIndex(RELCAT_RELID);

    // assign relNameAttr.sVal = relName
    Attribute relNameAttr; // (stores relName as type union Attribute)
	strcpy((char*)relNameAttr.sVal,(const char*)relName);

    //  linearSearch on the relation catalog for RelName = relNameAttr
	RecId relCatRecId = BlockAccess::linearSearch(RELCAT_RELID, RELCAT_ATTR_RELNAME, relNameAttr ,EQ);

    // if the relation does not exist (linearSearch returned {-1, -1})
	if (relCatRecId == RecId{-1, -1}) return E_RELNOTEXIST;

    // TODO: store the relation catalog record corresponding to the relation in
    // TODO: relCatEntryRecord using RecBuffer.getRecord

	RecBuffer relCatBlockBuffer (relCatRecId.block);

	// relCatEntryRecord : relation `relName` record in relation-catalog
    Attribute relCatEntryRecord[RELCAT_NO_ATTRS];
	relCatBlockBuffer.getRecord(relCatEntryRecord, relCatRecId.slot);

	// TODO: get the first record block & number of attributes of the relation 
	// TODO: (firstBlock) & (numAttrs) using the relation catalog entry record 
	
	int firstBlock = relCatEntryRecord[RELCAT_FIRST_BLOCK_INDEX].nVal;
	int numAttributes = relCatEntryRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal;

    // TODO: Delete all the record blocks of the relation
	int currentBlockNum = firstBlock;
    
	// for each record block of the relation:
	// Hint: to know if we reached the end, check if nextBlock = -1
	while (currentBlockNum != -1) {
		RecBuffer currentBlockBuffer (currentBlockNum);

    	// get block header using BlockBuffer.getHeader
		HeadInfo currentBlockHeader;
		currentBlockBuffer.getHeader(&currentBlockHeader);

		// get the next block from the header (rblock)
		currentBlockNum = currentBlockHeader.rblock;

		// release the block using BlockBuffer.releaseBlock
		currentBlockBuffer.releaseBlock();
	}

    /***
        Deleting attribute catalog entries corresponding the relation and index
        blocks corresponding to the relation with relName on its attributes
    ***/

    // reset the searchIndex of the attribute catalog
	RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

    int numberOfAttributesDeleted = 0;

    while(true) {
        // attrCatRecId : `relname`'s entry in `ATTRCAT`
        RecId attrCatRecId = BlockAccess::linearSearch(ATTRCAT_RELID, RELCAT_ATTR_RELNAME, relNameAttr, EQ);

        // if no more attributes to iterate over (attrCatRecId == {-1, -1})
		if (attrCatRecId == RecId{-1, -1}) break;

        numberOfAttributesDeleted++;

        // create a RecBuffer for attrCatRecId.block
        // get the header of the block
        // get the record corresponding to attrCatRecId.slot
		RecBuffer attrCatBlockBuffer (attrCatRecId.block);

		HeadInfo attrCatHeader;
		attrCatBlockBuffer.getHeader(&attrCatHeader);

		Attribute attrCatRecord [ATTRCAT_NO_ATTRS];
		attrCatBlockBuffer.getRecord(attrCatRecord, attrCatRecId.slot);

        // declare variable rootBlock which will be used to store the root
        // block field from the attribute catalog record.
        int rootBlock = attrCatRecord[ATTRCAT_ROOT_BLOCK_INDEX].nVal; // get root block from the record
        // (This will be used later to delete any indexes if it exists)
		
        // Update the Slotmap for the block by setting the slot as SLOT_UNOCCUPIED
        // Hint: use RecBuffer.getSlotMap and RecBuffer.setSlotMap
		unsigned char slotmap [attrCatHeader.numSlots];
		attrCatBlockBuffer.getSlotMap(slotmap);

		slotmap[attrCatRecId.slot] = SLOT_UNOCCUPIED;
		attrCatBlockBuffer.setSlotMap(slotmap);

        /* Decrement the numEntries in the header of the block corresponding to
           the attribute catalog entry and then set back the header
           using RecBuffer.setHeader */
		attrCatHeader.numEntries--;
		attrCatBlockBuffer.setHeader(&attrCatHeader);

        /* If number of entries become 0, releaseBlock is called after fixing
           the linked list.
        */
        if (attrCatHeader.numEntries == 0) {
            /* Standard DOUBLY Linked List Delete for a Block
               Get the header of the left block and set it's rblock to this
               block's rblock
            */

            // create a RecBuffer for lblock and call appropriate methods
			RecBuffer prevBlock (attrCatHeader.lblock);
			
			HeadInfo leftHeader;
			prevBlock.getHeader(&leftHeader);

			leftHeader.rblock = attrCatHeader.rblock;
			prevBlock.setHeader(&leftHeader);


            if (attrCatHeader.rblock != INVALID_BLOCKNUM) 
			{
                /* Get the header of the right block and set it's lblock to
                   this block's lblock */
                // create a RecBuffer for rblock and call appropriate methods
				RecBuffer nextBlock (attrCatHeader.rblock);
				
				HeadInfo rightHeader;
				nextBlock.getHeader(&rightHeader);

				rightHeader.lblock = attrCatHeader.lblock;
				nextBlock.setHeader(&rightHeader);

            } 
			else 
			{
                // (the block being released is the "Last Block" of the relation.)
                /* update the Relation Catalog entry's LastBlock field for this
                   relation with the block number of the previous block. */

				RelCatEntry relCatEntryBuffer;
				RelCacheTable::getRelCatEntry(ATTRCAT_RELID, &relCatEntryBuffer);

				relCatEntryBuffer.lastBlk = attrCatHeader.lblock;
				RelCacheTable::setRelCatEntry(ATTRCAT_RELID, &relCatEntryBuffer);
            }

            // (Since the attribute catalog will never be empty(why?), we do not
            //  need to handle the case of the linked list becoming empty - i.e
            //  every block of the attribute catalog gets released.)

            // call releaseBlock()
			attrCatBlockBuffer.releaseBlock();

			// RecId nextSearchIndex;
			
        }

		/*
        // (the following part is only relevant once indexing has been implemented)
        // if index exists for the attribute (rootBlock != -1), call bplus destroy
        if (rootBlock != -1) {
            // delete the bplus tree rooted at rootBlock using BPlusTree::bPlusDestroy()
        }
		*/

		// ! This code is an extra addition, it might not be correct/needed
		// if (numberOfAttributesDeleted == numAttributes) break;
    }

    /*** Delete the entry corresponding to the relation from relation catalog ***/
    // Fetch the header of Relcat block
	// // relCatBlockBuffer = RecBuffer (RELCAT_BLOCK);

	HeadInfo relCatHeader;
	relCatBlockBuffer.getHeader(&relCatHeader);

    // TODO: Decrement the numEntries in the header of the block corresponding to the
    // TODO: relation catalog entry and set it back
	relCatHeader.numEntries--;
	relCatBlockBuffer.setHeader(&relCatHeader);

    /* Get the slotmap in relation catalog, update it by marking the slot as
       free(SLOT_UNOCCUPIED) and set it back. */
	unsigned char slotmap [relCatHeader.numSlots];
	relCatBlockBuffer.getSlotMap(slotmap);

	slotmap[relCatRecId.slot] = SLOT_UNOCCUPIED;
	relCatBlockBuffer.setSlotMap(slotmap);

    /*** Updating the Relation Cache Table ***/
    /** Update relation catalog record entry (number of records in relation
        catalog is decreased by 1) **/

	// Get the entry corresponding to relation catalog from the relation
    // cache and update the number of records and set it back
    // (using RelCacheTable::setRelCatEntry() function)

	RelCatEntry relCatEntryBuffer;
	RelCacheTable::getRelCatEntry(RELCAT_RELID, &relCatEntryBuffer);

	relCatEntryBuffer.numRecs--;
	RelCacheTable::setRelCatEntry(RELCAT_RELID, &relCatEntryBuffer);

    /** Update attribute catalog entry (number of records in attribute catalog
        is decreased by numberOfAttributesDeleted) **/
    // i.e., #Records = #Records - numberOfAttributesDeleted

    // Get the entry corresponding to attribute catalog from the relation
    // cache and update the number of records and set it back
    // (using RelCacheTable::setRelCatEntry() function)

	RelCacheTable::getRelCatEntry(ATTRCAT_RELID, &relCatEntryBuffer);
	relCatEntryBuffer.numRecs -= numberOfAttributesDeleted;
	RelCacheTable::setRelCatEntry(ATTRCAT_RELID, &relCatEntryBuffer);


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
	RecId prevSearchIndex;
	RelCacheTable::getSearchIndex(relId, &prevSearchIndex);

    // declare block and slot which will be used to store the record id of the
    // slot we need to check.
    int block, slot;

    /* if the current search index record is invalid(i.e. = {-1, -1})
       (this only happens when the caller reset the search index)
    */
    if (prevSearchIndex.block == -1 && prevSearchIndex.slot == -1)
    {
        // (new project operation. start from beginning)

        // get the first record block of the relation from the relation cache
        // (use RelCacheTable::getRelCatEntry() function of Cache Layer)

        // block = first record block of the relation, slot = 0
		RelCatEntry relCatEntryBuffer;
		RelCacheTable::getRelCatEntry(relId, &relCatEntryBuffer);

		block = relCatEntryBuffer.firstBlk, slot = 0;
    }
    else
    {
        // (a project/search operation is already in progress)

        // block = previous search index's block
        // slot = previous search index's slot + 1
		block = prevSearchIndex.block, slot = prevSearchIndex.slot+1;
    }


    // The following code finds the next record of the relation
    /* Start from the record id (block, slot) and iterate over the remaining
       records of the relation */
    while (block != -1)
    {
        // create a RecBuffer object for block (using appropriate constructor!)
		RecBuffer currentBlockBuffer (block);

        // get header of the block using RecBuffer::getHeader() function
        // get slot map of the block using RecBuffer::getSlotMap() function
		HeadInfo currentBlockHeader;
		currentBlockBuffer.getHeader(&currentBlockHeader);

		unsigned char slotmap [currentBlockHeader.numSlots];
		currentBlockBuffer.getSlotMap(slotmap);

        if(slot >= currentBlockHeader.numSlots)
        {
            // (no more slots in this block)
            // update block = right block of block
            // update slot = 0
            // (NOTE: if this is the last block, rblock would be -1. this would
            //        set block = -1 and fail the loop condition )

			block = currentBlockHeader.rblock, slot = 0;
        }
        else if (slotmap[slot] == SLOT_UNOCCUPIED) // (i.e slot-th entry in slotMap contains SLOT_UNOCCUPIED)
        { 

            // increment slot
			slot++;
        }
        else { // (the next occupied slot / record has been found)
            break;
        }
    }

    if (block == -1){
        // (a record was not found. all records exhausted)
        return E_NOTFOUND;
    }

    // declare nextRecId to store the RecId of the record found
    RecId nextSearchIndex{block, slot};

    // set the search index to nextRecId using RelCacheTable::setSearchIndex
	RelCacheTable::setSearchIndex(relId, &nextSearchIndex);

    /* Copy the record with record id (nextRecId) to the record buffer (record)
       For this Instantiate a RecBuffer class object by passing the recId and
       call the appropriate method to fetch the record
    */

   RecBuffer recordBlockBuffer (block);
   recordBlockBuffer.getRecord(record, slot);

    return SUCCESS;
}
































/*

#include "BlockAccess.h"


#include <cstring>

int BlockAccess::search(int relId, Attribute *record, char *attrName, Attribute attrVal, int op) {
   RecId recId; 
    recId = linearSearch(relId, attrName, attrVal, op);
    if (recId.block == -1) {
        return E_NOTFOUND;
    }
    RecBuffer currBuffer(recId.block);
    currBuffer.getRecord(record, recId.slot);
    return SUCCESS;
}

RecId BlockAccess::linearSearch(int relId, char *attrName, Attribute attrVal, int op) {
    RecId curr_index; 
    int ret = RelCacheTable::getSearchIndex(relId, &curr_index);
    if (ret != SUCCESS) {
        return RecId{-1, -1};
    }
    int block = -1, slot = -1;
    int slotNum = 0;
    AttrCatEntry attrCatEntry; 
    AttrCacheTable::getAttrCatEntry(relId, attrName, &attrCatEntry);
    int offset = attrCatEntry.offset;
    int type = attrCatEntry.attrType;

    RelCatEntry relCatBuffer; 
    RelCacheTable::getRelCatEntry(relId, &relCatBuffer);
    slotNum = relCatBuffer.numSlotsPerBlk;
    
    if (curr_index.block == -1 && curr_index.slot == -1) {
       block = relCatBuffer.firstBlk;
       slot = 0;
    }
    else {
        block = curr_index.block;
        slot = curr_index.slot + 1;
    }
    unsigned char slotMap[ slotNum ];
    RecId index;
    Attribute record[relCatBuffer.numAttrs];
    while (block != -1) {
       RecBuffer currBuffer(block);         
       currBuffer.getSlotMap(slotMap);
       HeadInfo head;
       currBuffer.getHeader(&head);
       for (auto i = slot; i < slotNum; ++i) {
        if (slotMap[i] == SLOT_OCCUPIED) {
            currBuffer.getRecord(record, i);        
            int res = compareAttrs(record[offset], attrVal, type);
            if (
                (op == EQ && res == 0) ||
                (op == LE && res <= 0) ||
                (op == LT && res < 0) ||
                (op == GE && res >= 0) ||
                (op == GT && res > 0) ||
                (op == NE && res != 0)
            )
                {
                    index.block = block;
                    index.slot = i;
                    RelCacheTable::setSearchIndex(relId, &index);
                    return index;
                }
        }
       }
       block = head.rblock;
       slot = 0;
    }
    return RecId{-1, -1}; 
}


int BlockAccess::renameRelation(char *oldName, char *newName) {
    char attrName[20] = RELCAT_ATTR_RELNAME;
    Attribute attrVal;
    strcpy(attrVal.sVal, oldName);
    RecId oldId =  linearSearch(RELCAT_RELID, attrName, attrVal, EQ);
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    if (oldId.block == -1 || oldId.slot == -1) {
        return E_RELNOTEXIST;
    }
    strcpy(attrVal.sVal, newName);
    RecId newId  = linearSearch(RELCAT_RELID, attrName, attrVal, EQ);
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    if (!(newId.block == -1 && newId.slot == -1)) {
        return E_RELEXIST;
    }
    RecBuffer relCatBuffer(oldId.block);
    Attribute record[RELCAT_NO_ATTRS];
    relCatBuffer.getRecord(record, oldId.slot);
    strcpy(record[RELCAT_REL_NAME_INDEX].sVal, newName);
    relCatBuffer.setRecord(record, oldId.slot);
    RecId attrId;
    
    strcpy(attrName, ATTRCAT_ATTR_RELNAME);
    strcpy(attrVal.sVal, oldName);
    do {
        attrId = linearSearch(ATTRCAT_RELID, attrName, attrVal, EQ);
        if (attrId.block == -1 && attrId.slot == -1) {
            break;
        }
        RecBuffer attrCatBuffer(attrId.block);
        attrCatBuffer.getRecord(record, attrId.slot);
        strcpy(record[ATTRCAT_REL_NAME_INDEX].sVal, newName);
        attrCatBuffer.setRecord(record, attrId.slot);
    } while (attrId.block != -1 || attrId.slot != -1);
    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    return SUCCESS;
}


int BlockAccess::renameAttribute(char *relName, char *oldName, char *newName) {

    char attrName[20] = RELCAT_ATTR_RELNAME;
    Attribute attrVal;
    strcpy(attrVal.sVal, relName);
    RecId recId =  linearSearch(RELCAT_RELID, attrName, attrVal, EQ);
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    if (recId.block == -1 || recId.slot == -1) {
        return E_RELNOTEXIST;
    }
    strcpy(attrName, ATTRCAT_ATTR_RELNAME);
    RecId oldAttrId = {-1, -1}, curr = oldAttrId;
    Attribute record[ATTRCAT_NO_ATTRS];
    do {
        curr = linearSearch(ATTRCAT_RELID, attrName, attrVal, EQ);
        if (curr.block == -1 && curr.slot == -1) {
            break;
        }
        RecBuffer currBuffer(curr.block);
        currBuffer.getRecord(record, curr.slot);
        if (!strcmp(oldName, record[ATTRCAT_ATTR_NAME_INDEX].sVal)) {
            oldAttrId = curr;
        }
        if (!strcmp(newName, record[ATTRCAT_ATTR_NAME_INDEX].sVal)) {
            return E_ATTREXIST;
        }
    } while (curr.block != -1 || curr.slot != -1);
    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    if (oldAttrId.block == -1 && oldAttrId.slot == -1) {
        return E_ATTRNOTEXIST;
    }
    RecBuffer currBuffer(oldAttrId.block);
    currBuffer.getRecord(record, oldAttrId.slot);
    strcpy(record[ATTRCAT_ATTR_NAME_INDEX].sVal, newName);
    currBuffer.setRecord(record, oldAttrId.slot);
    return SUCCESS;

}



int BlockAccess::insert(int relId, union Attribute *record) {
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(relId, &relCatEntry);
    int block = relCatEntry.firstBlk, prevBlock = -1;
    RecId currId = {-1, -1};
    int numSlots = relCatEntry.numSlotsPerBlk;
    int numAttrs = relCatEntry.numAttrs;
    unsigned char slotMap[numSlots];
    bool flag = false;
    HeadInfo head;
    while (block != -1) {
       RecBuffer recBuffer(block); 
       recBuffer.getSlotMap(slotMap);
       flag = false;
       for (auto i = 0; i < numSlots; ++i) {
        if (slotMap[i] == SLOT_UNOCCUPIED) {
            currId.block = block;
            currId.slot = i;
            flag = true;
            break;
        }
       }
       if (flag) {
            break;
       }
       recBuffer.getHeader(&head);
       prevBlock = block;
       block = head.rblock;
    }
    if (currId.block == -1) {
        if (relId == RELCAT_RELID) {
            return E_MAXRELATIONS;
        }
        RecBuffer newBlockBuffer;
        int blockNum = newBlockBuffer.getBlockNum();
        if (blockNum == E_DISKFULL) {
            return E_DISKFULL;
        }
        currId.block = blockNum;
        currId.slot = 0;
        newBlockBuffer.getHeader(&head);        
        head.lblock = prevBlock;
        head.numSlots = numSlots;
        head.numAttrs = numAttrs;
        newBlockBuffer.setHeader(&head);

        for (auto i = 0; i < numSlots; ++i) {
            slotMap[i] = SLOT_UNOCCUPIED;
        }
        newBlockBuffer.setSlotMap(slotMap);
        if (prevBlock != -1) {
            RecBuffer prevBlockBuffer(prevBlock);
            prevBlockBuffer.getHeader(&head);
            head.rblock = currId.block;
            prevBlockBuffer.setHeader(&head);
        }
        else {
            relCatEntry.firstBlk = currId.block;
        }
        relCatEntry.lastBlk = currId.block;
        RelCacheTable::setRelCatEntry(relId, &relCatEntry);
    }
    RecBuffer recBuffer(currId.block);
    recBuffer.setRecord(record, currId.slot);
    recBuffer.getSlotMap(slotMap);
    slotMap[currId.slot] = SLOT_OCCUPIED;
    recBuffer.setSlotMap(slotMap);
    recBuffer.getHeader(&head);
    head.numEntries++;
    recBuffer.setHeader(&head);
    relCatEntry.numRecs++;
    RelCacheTable::setRelCatEntry(relId, &relCatEntry);
    return SUCCESS;
    
}


int BlockAccess::deleteRelation(char *relName) {
    if (!strcmp(relName, RELCAT_RELNAME) || !strcmp(relName, ATTRCAT_RELNAME)) {
        return E_NOTPERMITTED;
    }
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    char attrName[ATTR_SIZE] = RELCAT_ATTR_RELNAME;
    Attribute attrVal;
    strcpy(attrVal.sVal, relName);
    
    RecId currId = BlockAccess::linearSearch(RELCAT_RELID, attrName, attrVal, EQ);
    if (currId.block == -1) {
        return E_RELNOTEXIST;
    }
    Attribute record[RELCAT_NO_ATTRS];
    RecBuffer relBuffer(RELCAT_BLOCK);
    relBuffer.getRecord(record, currId.slot);
    int block = record[RELCAT_FIRST_BLOCK_INDEX].nVal;
    int numAttrs = record[RELCAT_NO_ATTRIBUTES_INDEX].nVal;
    HeadInfo head;
    while (block != -1) {
        RecBuffer tempBuffer(block);
        tempBuffer.getHeader(&head);
        block = head.rblock;
        tempBuffer.releaseBlock();
    }

    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    strcpy(attrName, ATTRCAT_ATTR_RELNAME);
    int numOfDeletedAttrs = 0;
    unsigned char SlotMap[SLOTMAP_SIZE_RELCAT_ATTRCAT];
    while (true) {
        RecId attrId = BlockAccess::linearSearch(ATTRCAT_RELID, attrName, attrVal, EQ);
        if (attrId.block == -1) {
            break;
        }
        numOfDeletedAttrs++;
        RecBuffer tempBuffer(attrId.block);
        tempBuffer.getRecord(record, attrId.slot);
        int rootBlock =  record[ATTRCAT_ROOT_BLOCK_INDEX].nVal;
        tempBuffer.getSlotMap(SlotMap);
        SlotMap[attrId.slot] = SLOT_UNOCCUPIED;
        tempBuffer.setSlotMap(SlotMap);
        tempBuffer.getHeader(&head);
        head.numEntries--;
        if (head.numEntries == 0) {
            RecBuffer leftBlockBuffer(head.lblock);
            HeadInfo leftBlockHead;
            leftBlockBuffer.getHeader(&leftBlockHead);
            leftBlockHead.rblock = head.rblock;
            leftBlockBuffer.setHeader(&leftBlockHead);
            if (head.rblock != -1) {
                RecBuffer rightBlockBuffer(head.rblock);
                HeadInfo rightBlockHead;
                rightBlockBuffer.getHeader(&rightBlockHead);
                rightBlockHead.lblock = head.lblock;
                rightBlockBuffer.setHeader(&rightBlockHead);
            }
            else {
                RelCatEntry relCatEntry;
                RelCacheTable::getRelCatEntry(ATTRCAT_RELID, &relCatEntry);
                relCatEntry.lastBlk = head.lblock;
                RelCacheTable::setRelCatEntry(ATTRCAT_RELID, &relCatEntry);
            }
            tempBuffer.releaseBlock();
        }
        if (rootBlock != -1) {
            int k = 0;
            //to be implemented
        }
    }
    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    relBuffer.getSlotMap(SlotMap);
    SlotMap[currId.slot] = SLOT_UNOCCUPIED;
    relBuffer.setSlotMap(SlotMap);
    relBuffer.getHeader(&head);
    head.numEntries--;
    relBuffer.setHeader(&head);
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(RELCAT_RELID, &relCatEntry);
    relCatEntry.numRecs--;
    RelCacheTable::setRelCatEntry(RELCAT_RELID, &relCatEntry);
    RelCacheTable::getRelCatEntry(ATTRCAT_RELID, &relCatEntry);
    relCatEntry.numRecs -= numOfDeletedAttrs;
    RelCacheTable::setRelCatEntry(ATTRCAT_RELID, &relCatEntry);
    return SUCCESS;
}


int BlockAccess::project(int relId, Attribute *record) {

    RecId lastId;
    RelCacheTable::getSearchIndex(relId, &lastId); 
    int block = -1, slot = -1;
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(relId, &relCatEntry);
    if (lastId.block == -1) {
        block = relCatEntry.firstBlk;
        slot = 0;
    }
    else {
        block = lastId.block;
        slot = lastId.slot + 1;
    }
    unsigned char slotMap[relCatEntry.numSlotsPerBlk];
    while (block != -1) {
        RecBuffer currBuffer(block);
        currBuffer.getSlotMap(slotMap);
        for (auto i = slot; i < relCatEntry.numSlotsPerBlk; ++i) {
            if (slotMap[i] == SLOT_OCCUPIED) {
               lastId.block = block; 
               lastId.slot = i;
               RelCacheTable::setSearchIndex(relId, &lastId);
               currBuffer.getRecord(record, i);
               return SUCCESS;
            }
        }
        HeadInfo head;
        currBuffer.getHeader(&head);
        block = head.rblock;
        slot = 0;
    }
    return E_NOTFOUND;
}
*/
