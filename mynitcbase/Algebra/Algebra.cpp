#include "Algebra.h"

#include <cstring>
#include <iostream>


// will return if a string can be parsed as a floating point number
bool isNumber(char* str) {
  int len;
  float ignore;
  /*
    sscanf returns the number of elements read, so if there is no float matching
    the first %f, ret will be 0, else it'll be 1

    %n gets the number of characters read. this scanf sequence will read the
    first float ignoring all the whitespace before and after. and the number of
    characters read that far will be stored in len. if len == strlen(str), then
    the string only contains a float with/without whitespace. else, there's other
    characters.
  */
  int ret = sscanf(str, "%f %n", &ignore, &len);
  return ret == 1 && len == strlen(str);
}

/* used to select all the records that satisfy a condition.
the arguments of the function are
- srcRel - the source relation we want to select from
- targetRel - the relation we want to select into. (ignore for now)
- attr - the attribute that the condition is checking
- op - the operator of the condition
- strVal - the value that we want to compare against (represented as a string)
*/
// int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]) {
//   int srcRelId = OpenRelTable::getRelId(srcRel);      // we'll implement this later
//   if (srcRelId == E_RELNOTOPEN) {
//     return E_RELNOTOPEN;
//   }

//   AttrCatEntry attrCatEntry;
//   // get the attribute catalog entry for attr, using AttrCacheTable::getAttrcatEntry()
//   //    return E_ATTRNOTEXIST if it returns the error
//   int ret = AttrCacheTable::getAttrCatEntry(srcRelId, attr, &attrCatEntry);
//   if (ret == E_ATTRNOTEXIST) {
//     return ret;
//   }

//   /*** Convert strVal (string) to an attribute of data type NUMBER or STRING ***/
//   int type = attrCatEntry.attrType;
//   Attribute attrVal;
//   if (type == NUMBER) {
//     if (isNumber(strVal)) {       // the isNumber() function is implemented below
//       attrVal.nVal = atof(strVal);
//     }
//     else {
//       return E_ATTRTYPEMISMATCH;
//     }
//   }
//   else if (type == STRING) {
//     strcpy(attrVal.sVal, strVal);
//   }

//   /*** Selecting records from the source relation ***/

//   // Before calling the search function, reset the search to start from the first hit
//   // using RelCacheTable::resetSearchIndex()
//   RelCacheTable::resetSearchIndex(srcRelId);

//   RelCatEntry relCatEntry;
//   // get relCatEntry using RelCacheTable::getRelCatEntry()
//   RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);

//   /************************
//   The following code prints the contents of a relation directly to the output
//   console. Direct console output is not permitted by the actual the NITCbase
//   specification and the output can only be inserted into a new relation. We will
//   be modifying it in the later stages to match the specification.
//   ************************/

//   printf("|");
//   for (int i = 0; i < relCatEntry.numAttrs; ++i) {
//     AttrCatEntry attrCatEntry;
//     // get attrCatEntry at offset i using AttrCacheTable::getAttrCatEntry()
//     AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatEntry);

//     printf(" %s |", attrCatEntry.attrName);
//   }
//   printf("\n");

//   while (true) {
//     RecId searchRes = BlockAccess::linearSearch(srcRelId, attr, attrVal, op);

//     if (searchRes.block != -1 && searchRes.slot != -1) {

//       // get the record at searchRes using BlockBuffer.getRecord
//       Attribute record[relCatEntry.numAttrs];
//       RecBuffer recblock(searchRes.block);
//       recblock.getRecord(record, searchRes.slot);

//       // print the attribute values in the same format as above
//       for (int i = 0; i < relCatEntry.numAttrs; i++) {
//         // get the attrCatEntry for the srcRelId with corresponding offsets
//         AttrCatEntry attrCatEntry;
//         AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatEntry);

//         // with the attrCatEntry we can get the attrType
//         // print correspondingly
//         if (attrCatEntry.attrType == STRING)
//           printf(" %s |", record[i].sVal);
//         else
//           printf(" %d |", (int)record[i].nVal);
//       }
//       printf("\n");

//     }
//     else {

//       // (all records over)
//       break;
//     }
//   }

//   return SUCCESS;
// }
int Algebra::insert(char relName[ATTR_SIZE], int nAttrs, char record[][ATTR_SIZE]){
    // if relName is equal to "RELATIONCAT" or "ATTRIBUTECAT"
    // return E_NOTPERMITTED;
    char relcatName[ATTR_SIZE];
    char attrcatName[ATTR_SIZE];
    strcpy(relcatName,RELCAT_RELNAME);
    strcpy(attrcatName,ATTRCAT_RELNAME);
    if((!strcmp(relName,relcatName))||(!strcmp(attrcatName,relName)))return E_NOTPERMITTED;

    // get the relation's rel-id using OpenRelTable::getRelId() method
    int relId = OpenRelTable::getRelId(relName);
    if(relId == E_RELNOTOPEN)return E_RELNOTOPEN;
    RelCatEntry relcatEntry;
    RelCacheTable::getRelCatEntry(relId,&relcatEntry);


    // if relation is not open in open relation table, return E_RELNOTOPEN
    // (check if the value returned from getRelId function call = E_RELNOTOPEN)
    // get the relation catalog entry from relation cache
    // (use RelCacheTable::getRelCatEntry() of Cache Layer)
    if(relcatEntry.numAttrs != nAttrs)return E_NATTRMISMATCH;
    /* if relCatEntry.numAttrs != numberOfAttributes in relation,
       return E_NATTRMISMATCH */
    Attribute recordValues[nAttrs];
    // let recordValues[numberOfAttributes] be an array of type union Attribute

    /*
        Converting 2D char array of record values to Attribute array recordValues
     */
    for(int i = 0;i<nAttrs;i++)// iterate through 0 to nAttrs-1: (let i be the iterator)
    {
        // get the attr-cat entry for the i'th attribute from the attr-cache
        AttrCatEntry attrcatEntry;
        AttrCacheTable::getAttrCatEntry(relId,i,&attrcatEntry);
        // (use AttrCacheTable::getAttrCatEntry())
        int type = attrcatEntry.attrType;
        // let type = attrCatEntry.attrType;
        if (type == NUMBER) {
          if (isNumber(record[i])) {       // the isNumber() function is implemented below
            recordValues[i].nVal = atof(record[i]);
          }
          else {
            return E_ATTRTYPEMISMATCH;
          }
        }
        // if (type == NUMBER)
        // {
        //     // if the char array record[i] can be converted to a number
        //     if(// (check this using isNumber() function)
        //     {
        //         /* convert the char array to numeral and store it
        //            at recordValues[i].nVal using atof() */
        //     }
        //     // else
        //     {
        //         return E_ATTRTYPEMISMATCH;
        //     }
        // }
        else if (type == STRING)
        {
          // copy record[i] to recordValues[i].sVal
          strcpy(recordValues[i].sVal,record[i]);
        }
    }

    // insert the record by calling BlockAccess::insert() function
    // let retVal denote the return value of insert call
    int retVal = BlockAccess::insert(relId,recordValues);

    return retVal;
}
int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]) {
    // get the srcRel's rel-id (let it be srcRelid), using OpenRelTable::getRelId()
    // if srcRel is not open in open relation table, return E_RELNOTOPEN
    int srcRelId = OpenRelTable::getRelId(srcRel);      // we'll implement this later
    if (srcRelId == E_RELNOTOPEN) {
      return E_RELNOTOPEN;
    }
    // get the attr-cat entry for attr, using AttrCacheTable::getAttrCatEntry()
    // if getAttrcatEntry() call fails return E_ATTRNOTEXIST
    AttrCatEntry attrCatEntry;
    int ret = AttrCacheTable::getAttrCatEntry(srcRelId, attr, &attrCatEntry);
    if (ret == E_ATTRNOTEXIST) {
      return ret;
    }

    /*** Convert strVal to an attribute of data type NUMBER or STRING ***/

    Attribute attrVal;
    int type = attrCatEntry.attrType;

    if (type == NUMBER)
    {
        // if the input argument strVal can be converted to a number
        // (check this using isNumber() function)
        if(isNumber(strVal))
        {
          attrVal.nVal = atof(strVal);
            // convert strVal to double and store it at attrVal.nVal using atof()
        }
        // else
        else
        {
            return E_ATTRTYPEMISMATCH;
        }
    }
    else if (type == STRING)
    {
        // copy strVal to attrVal.sVal
        strcpy(attrVal.sVal,strVal);
    }

    /*** Creating and opening the target relation ***/
    // Prepare arguments for createRel() in the following way:
    // get RelcatEntry of srcRel using RelCacheTable::getRelCatEntry()
    RelCatEntry relcatbuf;
    RelCacheTable::getRelCatEntry(srcRelId,&relcatbuf);
    int src_nAttrs = relcatbuf.numAttrs/* the no. of attributes present in src relation */ ;


    /* let attr_names[src_nAttrs][ATTR_SIZE] be a 2D array of type char
        (will store the attribute names of rel). */
    // let attr_types[src_nAttrs] be an array of type int
    char attr_names[src_nAttrs][ATTR_SIZE];
    int attr_types[src_nAttrs];
    for(int i = 0;i<src_nAttrs;i++)
    {
      AttrCatEntry attrcatbuf;
      AttrCacheTable::getAttrCatEntry(srcRelId,i,&attrcatbuf);
      strcpy(attr_names[i],attrcatbuf.attrName);
      attr_types[i] = attrcatbuf.attrType;
    }
    /*iterate through 0 to src_nAttrs-1 :
        get the i'th attribute's AttrCatEntry using AttrCacheTable::getAttrCatEntry()
        fill the attr_names, attr_types arrays that we declared with the entries
        of corresponding attributes
    */
    ret = Schema::createRel(targetRel,src_nAttrs,attr_names,attr_types);
    if(ret != SUCCESS)return ret;
    /* Create the relation for target relation by calling Schema::createRel()
       by providing appropriate arguments */
    // if the createRel returns an error code, then return that value.
    int targetrelId = OpenRelTable::openRel(targetRel);
    if(targetrelId<0 || targetrelId>=MAX_OPEN)
    {
      Schema::deleteRel(targetRel);
      return targetrelId;
    }
    /* Open the newly created target relation by calling OpenRelTable::openRel()
       method and store the target relid */
    /* If opening fails, delete the target relation by calling Schema::deleteRel()
       and return the error value returned from openRel() */

    /*** Selecting and inserting records into the target relation ***/
    /* Before calling the search function, reset the search to start from the
       first using RelCacheTable::resetSearchIndex() */
    Attribute record[src_nAttrs];

    /*
        The BlockAccess::search() function can either do a linearSearch or
        a B+ tree search. Hence, reset the search index of the relation in the
        relation cache using RelCacheTable::resetSearchIndex().
        Also, reset the search index in the attribute cache for the select
        condition attribute with name given by the argument `attr`. Use
        AttrCacheTable::resetSearchIndex().
        Both these calls are necessary to ensure that search begins from the
        first record.
    */
    RelCacheTable::resetSearchIndex(srcRelId);
    AttrCacheTable::resetSearchIndex(srcRelId,attr);

    // read every record that satisfies the condition by repeatedly calling
    // BlockAccess::search() until there are no more records to be read

    while (/* BlockAccess::search() returns success */BlockAccess::search(srcRelId,record,attr,attrVal,op)==SUCCESS) {

        // ret = BlockAccess::insert(targetRelId, record);
        ret = BlockAccess::insert(targetrelId,record);
        if(ret != SUCCESS)
        {
          Schema::closeRel(targetRel);
          Schema::deleteRel(targetRel);
          return ret;
        }
        // if (insert fails) {
        //     close the targetrel(by calling Schema::closeRel(targetrel))
        //     delete targetrel (by calling Schema::deleteRel(targetrel))
        //     return ret;
        // }
    }
    Schema::closeRel(targetRel);
    return SUCCESS;
    // Close the targetRel by calling closeRel() method of schema layer

    // return SUCCESS.
}
int Algebra::project(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE]) {
//clones relation
    int srcRelId = OpenRelTable::getRelId(srcRel);/*srcRel's rel-id (use OpenRelTable::getRelId() function)*/
    if(srcRelId<0 || srcRelId>=MAX_OPEN)return E_RELNOTOPEN;
    // if srcRel is not open in open relation table, return E_RELNOTOPEN

    // get RelCatEntry of srcRel using RelCacheTable::getRelCatEntry()
    RelCatEntry relcatbuf;
    RelCacheTable::getRelCatEntry(srcRelId,&relcatbuf);
    // get the no. of attributes present in relation from the fetched RelCatEntry.
    int numAttrs = relcatbuf.numAttrs;
    // attrNames and attrTypes will be used to store the attribute names
    // and types of the source relation respectively
    char attrNames[numAttrs][ATTR_SIZE];
    int attrTypes[numAttrs];

    /*iterate through every attribute of the source relation :
        - get the AttributeCat entry of the attribute with offset.
          (using AttrCacheTable::getAttrCatEntry())
        - fill the arrays `attrNames` and `attrTypes` that we declared earlier
          with the data about each attribute
    */
    for(int i = 0;i<numAttrs;i++)
    {
      AttrCatEntry attrcatbuf;
      AttrCacheTable::getAttrCatEntry(srcRelId,i,&attrcatbuf);
      strcpy(attrNames[i],attrcatbuf.attrName);
      attrTypes[i] = attrcatbuf.attrType;
    }

    /*** Creating and opening the target relation ***/

    // Create a relation for target relation by calling Schema::createRel()
    int ret = Schema::createRel(targetRel,numAttrs,attrNames,attrTypes);
    if(ret != SUCCESS)return ret;
    // if the createRel returns an error code, then return that value.

    // Open the newly created target relation by calling OpenRelTable::openRel()
    // and get the target relid
    int targetrelId = OpenRelTable::openRel(targetRel);
    if(targetrelId<0 || targetrelId>=MAX_OPEN)
    {
      Schema::deleteRel(targetRel);
      return targetrelId;
    }
    // If opening fails, delete the target relation by calling Schema::deleteRel() of
    // return the error value returned from openRel().


    /*** Inserting projected records into the target relation ***/

    // Take care to reset the searchIndex before calling the project function
    // using RelCacheTable::resetSearchIndex()
    RelCacheTable::resetSearchIndex(srcRelId);

    Attribute record[numAttrs];
    while (BlockAccess::project(srcRelId,record)==SUCCESS/* BlockAccess::project(srcRelId, record) returns SUCCESS */)
    {
        // record will contain the next record
        // ret = BlockAccess::insert(targetRelId, proj_record);
        ret = BlockAccess::insert(targetrelId,record);
        if (/* insert fails */ret != SUCCESS) {
            // close the targetrel by calling Schema::closeRel()
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return ret;
            // delete targetrel by calling Schema::deleteRel()
            // return ret;
        }
    }

    // Close the targetRel by calling Schema::closeRel()
    Schema::closeRel(targetRel);
    return SUCCESS;

    // return SUCCESS.
}

int Algebra::project(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], int tar_nAttrs, char tar_Attrs[][ATTR_SIZE]) {

    int srcRelId = OpenRelTable::getRelId(srcRel);/*srcRel's rel-id (use OpenRelTable::getRelId() function)*/
    if(srcRelId<0 || srcRelId>=MAX_OPEN)return E_RELNOTOPEN;
    // if srcRel is not open in open relation table, return E_RELNOTOPEN

    // get RelCatEntry of srcRel using RelCacheTable::getRelCatEntry()
    RelCatEntry relcatbuf;
    RelCacheTable::getRelCatEntry(srcRelId,&relcatbuf);

    // get the no. of attributes present in relation from the fetched RelCatEntry.
    int src_nAttrs = relcatbuf.numAttrs;

    // declare attr_offset[tar_nAttrs] an array of type int.
    int attr_offset[tar_nAttrs];
    // where i-th entry will store the offset in a record of srcRel for the
    // i-th attribute in the target relation.

    // let attr_types[tar_nAttrs] be an array of type int.
    int attr_types[tar_nAttrs];
    // where i-th entry will store the type of the i-th attribute in the
    // target relation.


    /*** Checking if attributes of target are present in the source relation
         and storing its offsets and types ***/
    for(int i = 0;i<tar_nAttrs;i++)
    {
      AttrCatEntry attrcatbuf;
      int ret = AttrCacheTable::getAttrCatEntry(srcRelId,tar_Attrs[i],&attrcatbuf);
      if(ret != SUCCESS)return ret;
      attr_offset[i] = attrcatbuf.offset;
      attr_types[i] = attrcatbuf.attrType;
    }
    /*iterate through 0 to tar_nAttrs-1 :
        - get the attribute catalog entry of the attribute with name tar_attrs[i].
        - if the attribute is not found return E_ATTRNOTEXIST
        - fill the attr_offset, attr_types arrays of target relation from the
          corresponding attribute catalog entries of source relation
    */


    /*** Creating and opening the target relation ***/
    int ret = Schema::createRel(targetRel,tar_nAttrs,tar_Attrs,attr_types);
    if(ret != SUCCESS)return ret;
    // Create a relation for target relation by calling Schema::createRel()

    // if the createRel returns an error code, then return that value.

    // Open the newly created target relation by calling OpenRelTable::openRel()
    // and get the target relid
    int targetrelId = OpenRelTable::openRel(targetRel);
    if(targetrelId<0 || targetrelId>=MAX_OPEN)return targetrelId;
    // If opening fails, delete the target relation by calling Schema::deleteRel()
    // and return the error value from openRel()


    /*** Inserting projected records into the target relation ***/

    // Take care to reset the searchIndex before calling the project function
    // using RelCacheTable::resetSearchIndex()
    RelCacheTable::resetSearchIndex(srcRelId);
    Attribute record[src_nAttrs];

    while (/* BlockAccess::project(srcRelId, record) returns SUCCESS */BlockAccess::project(srcRelId,record) == SUCCESS) {
        // the variable `record` will contain the next record

        Attribute proj_record[tar_nAttrs];

        //iterate through 0 to tar_attrs-1:
        //    proj_record[attr_iter] = record[attr_offset[attr_iter]]
        for(int i = 0;i<tar_nAttrs;i++)
        {
          proj_record[i] = record[attr_offset[i]];
        }
        ret = BlockAccess::insert(targetrelId,proj_record);
        // ret = BlockAccess::insert(targetRelId, proj_record);

        if (/* insert fails */ret != SUCCESS) {
            // close the targetrel by calling Schema::closeRel()
            // delete targetrel by calling Schema::deleteRel()
            // return ret;
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return ret;
        }
    }

    // Close the targetRel by calling Schema::closeRel()
    Schema::closeRel(targetRel);
    return SUCCESS;

    // return SUCCESS.
}
int Algebra::join(char srcRelation1[ATTR_SIZE], char srcRelation2[ATTR_SIZE], char targetRelation[ATTR_SIZE], char attribute1[ATTR_SIZE], char attribute2[ATTR_SIZE]) {

    // get the srcRelation1's rel-id using OpenRelTable::getRelId() method
    int src1RelId = OpenRelTable::getRelId(srcRelation1);
    int src2RelId = OpenRelTable::getRelId(srcRelation2);

    // get the srcRelation2's rel-id using OpenRelTable::getRelId() method
    if(src1RelId<0||src1RelId>=MAX_OPEN)return E_RELNOTOPEN;
    if(src2RelId<0||src2RelId>=MAX_OPEN)return E_RELNOTOPEN;
    // if either of the two source relations is not open
    //     return E_RELNOTOPEN

    AttrCatEntry attrCatEntry1, attrCatEntry2;
    int ret1 = AttrCacheTable::getAttrCatEntry(src1RelId,attribute1,&attrCatEntry1);
    int ret2 = AttrCacheTable::getAttrCatEntry(src2RelId,attribute2,&attrCatEntry2);
    if(ret1 == E_ATTRNOTEXIST || ret2 == E_ATTRNOTEXIST)return E_ATTRNOTEXIST;
    // get the attribute catalog entries for the following from the attribute cache
    // (using AttrCacheTable::getAttrCatEntry())
    // - attrCatEntry1 = attribute1 of srcRelation1
    // - attrCatEntry2 = attribute2 of srcRelation2

    // if attribute1 is not present in srcRelation1 or attribute2 is not
    // present in srcRelation2 (getAttrCatEntry() returned E_ATTRNOTEXIST)
    //     return E_ATTRNOTEXIST.

    // if attribute1 and attribute2 are of different types return E_ATTRTYPEMISMATCH
    if(attrCatEntry1.attrType!=attrCatEntry2.attrType)return E_ATTRTYPEMISMATCH;
    // iterate through all the attributes in both the source relations and check if
    // there are any other pair of attributes other than join attributes
    // (i.e. attribute1 and attribute2) with duplicate names in srcRelation1 and
    // srcRelation2 (use AttrCacheTable::getAttrCatEntry())
    // If yes, return E_DUPLICATEATTR

    // get the relation catalog entries for the relations from the relation cache
    // (use RelCacheTable::getRelCatEntry() function)
    RelCatEntry relcatEntry1;
    RelCatEntry relcatEntry2;
    RelCacheTable::getRelCatEntry(src1RelId,&relcatEntry1);
    RelCacheTable::getRelCatEntry(src2RelId,&relcatEntry2);
    int numOfAttributes1 = /* number of attributes in srcRelation1 */relcatEntry1.numAttrs;
    int numOfAttributes2 = /* number of attributes in srcRelation2 */relcatEntry2.numAttrs;

    // if rel2 does not have an index on attr2
    //     create it using BPlusTree:bPlusCreate()
    //     if call fails, return the appropriate error code
    //     (if your implementation is correct, the only error code that will
    //      be returned here is E_DISKFULL)
    for (int attrindex1 = 0; attrindex1 < numOfAttributes1; attrindex1++)
    {
        AttrCatEntry attrCatEntryTemp1;
        AttrCacheTable::getAttrCatEntry(src1RelId, attrindex1, &attrCatEntryTemp1);

        if (strcmp(attrCatEntryTemp1.attrName, attribute1) == 0) continue;
        
        for (int attrindex2 = 0; attrindex2 < numOfAttributes2; attrindex2++)
        {
            AttrCatEntry attrCatEntryTemp2;
            AttrCacheTable::getAttrCatEntry(src2RelId, attrindex2, &attrCatEntryTemp2);

            if (strcmp (attrCatEntryTemp2.attrName, attribute2) == 0) continue;

            if (strcmp (attrCatEntryTemp1.attrName, attrCatEntryTemp2.attrName) == 0)
                return E_DUPLICATEATTR;
        }
    }
    if(attrCatEntry2.rootBlock == -1)
    {
      int ret = BPlusTree::bPlusCreate(src2RelId,attribute2);
      if(ret == E_DISKFULL)return ret;
    }
    int numOfAttributesInTarget = numOfAttributes1 + numOfAttributes2 - 1;
    // Note: The target relation has number of attributes one less than
    // nAttrs1+nAttrs2 (Why?)

    // declare the following arrays to store the details of the target relation
    char targetRelAttrNames[numOfAttributesInTarget][ATTR_SIZE];
    int targetRelAttrTypes[numOfAttributesInTarget];
    int ind = 0;
    for(int i = 0;i<numOfAttributes1;i++)
    {
      AttrCatEntry temp;
      AttrCacheTable::getAttrCatEntry(src1RelId,i,&temp);
      strcpy(targetRelAttrNames[i],temp.attrName);
      targetRelAttrTypes[i] = temp.attrType;
      ind = i;
    }
    for(int i = 0;i<numOfAttributes2;i++)
    {
      AttrCatEntry temp;
      AttrCacheTable::getAttrCatEntry(src2RelId,i,&temp);
      if(!strcmp(temp.attrName,attribute2))continue;
      strcpy(targetRelAttrNames[ind],temp.attrName);
      targetRelAttrTypes[ind++] = temp.attrType;
    }



    // iterate through all the attributes in both the source relations and
    // update targetRelAttrNames[],targetRelAttrTypes[] arrays excluding attribute2
    // in srcRelation2 (use AttrCacheTable::getAttrCatEntry())

    // create the target relation using the Schema::createRel() function
    int ret = Schema::createRel(targetRelation,numOfAttributesInTarget,targetRelAttrNames,targetRelAttrTypes);
    if(ret != SUCCESS)return ret;
    // if createRel() returns an error, return that error
    int tarRelId = OpenRelTable::openRel(targetRelation);
    // Open the targetRelation using OpenRelTable::openRel()

    if(tarRelId<0||tarRelId>=MAX_OPEN)// if openRel() fails (No free entries left in the Open Relation Table)
    {
      Schema::deleteRel(targetRelation);
      return tarRelId;
        // delete target relation by calling Schema::deleteRel()
        // return the error code
    }

    Attribute record1[numOfAttributes1];
    Attribute record2[numOfAttributes2];
    Attribute targetRecord[numOfAttributesInTarget];

    // this loop is to get every record of the srcRelation1 one by one
    while (BlockAccess::project(src1RelId, record1) == SUCCESS) {

        // reset the search index of `srcRelation2` in the relation cache
        // using RelCacheTable::resetSearchIndex()
        RelCacheTable::resetSearchIndex(src2RelId);
        AttrCacheTable::resetSearchIndex(src2RelId,attribute2);

        // reset the search index of `attribute2` in the attribute cache
        // using AttrCacheTable::resetSearchIndex()

        // this loop is to get every record of the srcRelation2 which satisfies
        //the following condition:
        // record1.attribute1 = record2.attribute2 (i.e. Equi-Join condition)
        while (BlockAccess::search(
            src2RelId, record2, attribute2, record1[attrCatEntry1.offset], EQ
        ) == SUCCESS ) {

            // copy srcRelation1's and srcRelation2's attribute values(except
            // for attribute2 in rel2) from record1 and record2 to targetRecord
            int ti = 0;
            for(int i = 0;i<numOfAttributes1;i++)
            {
              targetRecord[i] = record1[i];
              ti = i;
            }
            for(int i = 0;i<numOfAttributes2;i++)
            {
              if(i == attrCatEntry2.offset)continue;
              targetRecord[ti++] = record2[i];
            }
            // insert the current record into the target relation by calling
            // BlockAccess::insert()
            int ret3 = BlockAccess::insert(tarRelId,targetRecord);
            if(/* insert fails (insert should fail only due to DISK being FULL) */ret3 == E_DISKFULL) {

                // close the target relation by calling OpenRelTable::closeRel()
                OpenRelTable::closeRel(tarRelId);
                Schema::deleteRel(targetRelation);
                // delete targetRelation (by calling Schema::deleteRel())
                return E_DISKFULL;
            }
        }
    }
    OpenRelTable::closeRel(tarRelId);
    // close the target relation by calling OpenRelTable::closeRel()
    return SUCCESS;
}