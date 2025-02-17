#ifndef READY_LIST_PREDICATES_H
#define READY_LIST_PREDICATES_H

#include "single_core_proofs/scp_list_predicates.h"


#include "verifast_lists_extended.h"


/*@
// This predicate represents the global ready lists, i.e., the global array
// `pxReadyTasksLists` in `tasks.c`.
// Each index `p` stores a cyclic doubly linked list containing all tasks
// of priority `p` that are ready to run.
predicate readyLists_p(list<list<struct xLIST_ITEM*> > gCellLists,
                       list<list<void*> > gOwnerLists) =
    configMAX_PRIORITIES == length(gCellLists) &*&
    List_array_p(&pxReadyTasksLists, configMAX_PRIORITIES, 
                 gCellLists, gOwnerLists) &*&
    length(gCellLists) == length(gOwnerLists);


predicate List_array_p(List_t* array, int size, 
                       list<list<struct xLIST_ITEM*> > cellLists,
                       list<list<void*> > ownerLists) =
    size >= 0 &*&
    length(cellLists) == size &*&
    length(ownerLists) == length(cellLists) &*&
    size > 0
        ? (
            cellLists == cons(?gCells, ?gTailCellLists) &*&
            ownerLists == cons(?gOwners, ?gTailOwnerLists) &*&
            pointer_within_limits(array) == true &*&
            xLIST(array, ?gLen, ?gIndex, ?gListEnd, gCells, ?gVals, 
                  gOwners)
            &*&
            gLen < INT_MAX &*&
            List_array_p(array + 1, size - 1, gTailCellLists, gTailOwnerLists)
        )
        : (
            cellLists == nil &*&
            ownerLists == nil
        );

lemma void List_array_size_positive(List_t* pxArray)
requires List_array_p(pxArray, ?gSize, ?gCellLists, ?gOwnerLists);
ensures  
    List_array_p(pxArray, gSize, gCellLists, gOwnerLists) &*&
    gSize >= 0 &*& 
    gSize == length(gCellLists) &*& 
    length(gCellLists) == length(gOwnerLists);
{
    open  List_array_p(pxArray, gSize, gCellLists, gOwnerLists);
    close List_array_p(pxArray, gSize, gCellLists, gOwnerLists);
}

lemma void List_array_split(List_t* array, int index)
requires 
    List_array_p(array, ?gSize, ?gCellLists, ?gOwnerLists) &*& 
    0 <= index &*& index < gSize;
ensures 
    List_array_p(array, index, ?gPrefCellLists, ?gPrefOwnerLists) &*&
    gPrefCellLists == take(index, gCellLists) &*&
    gPrefOwnerLists == take(index, gOwnerLists) &*&
    pointer_within_limits(array) == true &*&
    xLIST(array + index, ?gLen, _, _, ?gCells, ?gVals, ?gOwners) &*&
    gLen < INT_MAX &*&
    gCells == nth(index, gCellLists) &*&
    gOwners == nth(index, gOwnerLists) &*&
    mem(gOwners, gOwnerLists) == true &*&
    List_array_p(array + index + 1, gSize-index-1, ?gSufCellLists, ?gSufOwnerLists) &*&
    gSufCellLists == drop(index+1, gCellLists) &*&
    gSufOwnerLists == drop(index+1, gOwnerLists);
{
    open List_array_p(array, gSize, gCellLists, gOwnerLists);

    if( index > 0 ) {
        List_array_split(array + 1, index - 1);
    }

    close List_array_p(array, index, take(index, gCellLists), take(index, gOwnerLists));
}

lemma void List_array_join(List_t* array)
requires
    List_array_p(array, ?gPrefSize, ?gPrefCellLists, ?gPrefOwnerLists) &*&
    xLIST(array + gPrefSize, ?gLen, _, _, ?gCells, _, ?gOwners) &*&
    gLen < INT_MAX &*&
    pointer_within_limits(array + gPrefSize) == true &*&
    List_array_p(array + gPrefSize + 1, ?gSufSize, ?gSufCellLists, ?gSufOwnerLists);
ensures 
    List_array_p(array, ?gSize, ?gCellLists, ?gOwnerLists) &*& 
    gSize == length(gCellLists) &*&
    length(gCellLists) == length(gOwnerLists) &*&
    gSize == gPrefSize + 1 + gSufSize &*&
    gCellLists == append(gPrefCellLists, cons(gCells, gSufCellLists)) &*&
    gOwnerLists == append(gPrefOwnerLists, cons(gOwners, gSufOwnerLists));
{
    open List_array_p(array, gPrefSize, gPrefCellLists, gPrefOwnerLists);
    List_array_size_positive(array + gPrefSize + 1);

    if( gPrefSize > 0 ) {
        List_array_join(array + 1);
    }

    close List_array_p(array, gPrefSize + 1 + gSufSize,
                       append(gPrefCellLists, cons(gCells, gSufCellLists)),
                       append(gPrefOwnerLists, cons(gOwners, gSufOwnerLists)));
}
@*/





/*@
lemma void List_array_p_index_within_limits(List_t* array, int index)
requires List_array_p(array, ?gSize, ?gCellLists, ?gOwnerLists) &*&
         0 <= index &*& index < gSize;
ensures List_array_p(array, gSize, gCellLists, gOwnerLists) &*&
        pointer_within_limits(&array[index]) == true;
{
    open List_array_p(array, gSize, gCellLists, gOwnerLists);
    if( index > 0) {
        List_array_p_index_within_limits(&array[1], index-1);
    }
    close List_array_p(array, gSize, gCellLists, gOwnerLists);
}
@*/



// -------------------------------------------------------------------------
// Lemmas to close the ready list predicate in different scenarios.
/*@
lemma void closeUnchanged_readyLists(list<list<struct xLIST_ITEM*> > cellLists,
                                     list<list<void*> > ownerLists)
requires 
    configMAX_PRIORITIES == length(cellLists) &*&
    configMAX_PRIORITIES == length(ownerLists) &*&
    List_array_p(&pxReadyTasksLists, ?gIndex, ?gPrefCellLists, ?gPrefOwnerLists) &*&
    gIndex < length(cellLists) &*&
    xLIST(&pxReadyTasksLists + gIndex, ?gLen, _, _, ?gCells, ?gVals, ?gOwners) &*&
    gLen < INT_MAX &*&
    gCells == nth(gIndex, cellLists) &*&
    gOwners == nth(gIndex, ownerLists) &*&
    pointer_within_limits(&pxReadyTasksLists + gIndex) == true &*&
    List_array_p(&pxReadyTasksLists + gIndex + 1, configMAX_PRIORITIES - gIndex - 1,
                 ?gSufCellLists, ?gSufOwnerLists) &*&
    gPrefCellLists == take(gIndex, cellLists) &*&
    gSufCellLists == drop(gIndex+1, cellLists) &*&
    gPrefOwnerLists == take(gIndex, ownerLists) &*&
    gSufOwnerLists == drop(gIndex+1, ownerLists);
ensures
    readyLists_p(cellLists, ownerLists);
{
    // Prove `0 <= gIndex`:
        open List_array_p(&pxReadyTasksLists, gIndex, gPrefCellLists, gPrefOwnerLists);
        close List_array_p(&pxReadyTasksLists, gIndex, gPrefCellLists, gPrefOwnerLists);
    assert( 0 <= gIndex );
    
    List_array_join(&pxReadyTasksLists);
    assert( List_array_p(&pxReadyTasksLists, ?gSize, ?gCellLists2, ?gOwnerLists2) );
    
    append_take_nth_drop(gIndex, cellLists);
    append_take_nth_drop(gIndex, ownerLists);
    assert( gSize == configMAX_PRIORITIES );
    assert( gCellLists2 == cellLists );
    assert( gOwnerLists2 == ownerLists );

    close readyLists_p(cellLists, ownerLists);
}

lemma void closeReordered_readyLists(list<list<struct xLIST_ITEM*> > cellLists,
                                     list<list<void*> > ownerLists,
                                     list<struct xLIST_ITEM*> reorderedCells,
                                     list<void*> reorderedOwners,
                                     list<void*> tasks)
requires
    configMAX_PRIORITIES == length(cellLists) &*&
    configMAX_PRIORITIES == length(ownerLists) &*&
    List_array_p(&pxReadyTasksLists, ?gIndex, ?gPrefCellLists, ?gPrefOwnerLists) &*&
    gIndex < length(cellLists) &*&
    xLIST(&pxReadyTasksLists + gIndex, ?gLen, _, _, reorderedCells, _, reorderedOwners) &*&
    gLen < INT_MAX &*&
    length(reorderedCells) == length(nth(gIndex, cellLists)) &*&
    length(reorderedOwners) == length(nth(gIndex, ownerLists)) &*&
    pointer_within_limits(&pxReadyTasksLists + gIndex) == true &*&
    List_array_p(&pxReadyTasksLists + gIndex + 1, configMAX_PRIORITIES - gIndex - 1,
                 ?gSufCellLists, ?gSufOwnerLists) &*&
    gPrefCellLists == take(gIndex, cellLists) &*&
    gSufCellLists == drop(gIndex+1, cellLists) &*&
    gPrefOwnerLists == take(gIndex, ownerLists) &*&
    gSufOwnerLists == drop(gIndex+1, ownerLists) &*&
    forall(ownerLists, (superset)(tasks)) == true &*&
    forall(reorderedOwners, (mem_list_elem)(tasks)) == true;
ensures
    readyLists_p(?gReorderedCellLists, ?gReorderedOwnerLists) &*&
    forall(gReorderedOwnerLists, (superset)(tasks)) == true;
{
    // Prove that `gIndex != 0 -> gIndex > 0`
    if(gIndex != 0) {
        open List_array_p(&pxReadyTasksLists, gIndex, gPrefCellLists, gPrefOwnerLists);
        close List_array_p(&pxReadyTasksLists, gIndex, gPrefCellLists, gPrefOwnerLists);
        assert( gIndex > 0 );
    }

    List_array_join(&pxReadyTasksLists);
    assert( List_array_p(&pxReadyTasksLists, configMAX_PRIORITIES, 
                         ?gReorderedCellLists, ?gReorderedOwnerLists) );

    if(gIndex == 0) {
        assert( nth(0, gReorderedCellLists) == reorderedCells );
    } else {
        nth_take(0, gIndex, cellLists);
        assert( nth(0, gReorderedCellLists) == nth(0, gPrefCellLists) );
        assert( nth(0, gPrefCellLists) == nth(0, cellLists) );
    }
    assert( length(nth(0, gReorderedCellLists)) == length(nth(0, cellLists)) );

    close readyLists_p(gReorderedCellLists, gReorderedOwnerLists);


    // Below we prove `forall(gReorderedOwnerLists, (superset)(tasks)) == true`
    forall_take(ownerLists, (superset)(tasks), gIndex);
    forall_drop(ownerLists, (superset)(tasks), gIndex+1);
    assert( forall(gPrefOwnerLists, (superset)(tasks)) == true );
    assert( forall(gSufOwnerLists, (superset)(tasks)) == true );
    forall_mem_implies_superset(tasks, reorderedOwners);
    assert( superset(tasks, reorderedOwners) == true );
    assert( forall(singleton(reorderedOwners), (superset)(tasks)) == true );
    assert( forall(cons(reorderedOwners, gSufOwnerLists), (superset)(tasks)) == true );

    forall_append(gPrefOwnerLists, cons(reorderedOwners, gSufOwnerLists),
                  (superset)(tasks));
}
@*/


/*@
predicate VF_reordeReadyList__ghost_args(list<void*> tasks,
                                         list<list<struct xLIST_ITEM*> > cellLists,
                                         list<list<void*> > ownerLists,
                                         int offset) 
    = true;
@*/

void VF_reordeReadyList(List_t* pxReadyList, ListItem_t * pxTaskItem)
/*@ requires
        // ghost arguments
            VF_reordeReadyList__ghost_args(?gTasks, ?gCellLists, ?gOwnerLists, ?gOffset)
            &*&
            length(gCellLists) == configMAX_PRIORITIES &*&
            length(gOwnerLists) == configMAX_PRIORITIES &*&
            0 <= gOffset &*& gOffset < length(gCellLists) 
        &*&
        // current ready list
            xLIST(pxReadyList, ?gSize, ?gIndex, ?gEnd, ?gCells, ?gVals, ?gOwners) &*&
            pxReadyList == &pxReadyTasksLists + gOffset &*&
            pointer_within_limits(pxReadyList) == true &*&
            gSize < INT_MAX &*&
            gEnd != pxTaskItem &*&
            mem(pxTaskItem, gCells) == true &*&
            gCells == nth(gOffset, gCellLists) &*&
            gOwners == nth(gOffset, gOwnerLists)
        &*&
        // prefix and suffix of ready lists array
            List_array_p(&pxReadyTasksLists, gOffset, ?gPrefCellLists, ?gPrefOwnerLists) &*&
            List_array_p(&pxReadyTasksLists + gOffset + 1, configMAX_PRIORITIES - gOffset - 1,
                        ?gSufCellLists, ?gSufOwnerLists)
            &*&
            gPrefCellLists == take(gOffset, gCellLists) &*&
            gSufCellLists == drop(gOffset+1, gCellLists) &*&
            gPrefOwnerLists == take(gOffset, gOwnerLists) &*&
            gSufOwnerLists == drop(gOffset+1, gOwnerLists) &*&
            forall(gOwnerLists, (superset)(gTasks)) == true &*&
            subset(gOwners, gTasks) == true;
@*/
/*@ ensures 
        readyLists_p(?gReorderedCellLists, ?gReorderedOwnerLists) &*&
        length(gReorderedCellLists) == length(gCellLists) &*&
        length(gReorderedOwnerLists) == length(gOwnerLists) &*&
        length(gReorderedCellLists) == length(gReorderedOwnerLists) &*&
        forall(gReorderedOwnerLists, (superset)(gTasks)) == true;
 @*/
{
    //@ open VF_reordeReadyList__ghost_args(_, _, _, _);

    // Proving `∀o ∈ gOwners. o ∈ gTasks`
        //@ forall_mem(gOwners, gOwnerLists, (superset)(gTasks));
        //@ assert( superset(gTasks, gOwners) == true );
        //@ subset_implies_forall_mem(gOwners, gTasks);
    //@ assert( forall(gOwners, (mem_list_elem)(gTasks)) == true );

    // Proving `length(gCells) == length(gOwners) == gSize + 1`:
        //@ open xLIST(pxReadyList, gSize, gIndex, gEnd, gCells, gVals, gOwners);
        //@ close xLIST(pxReadyList, gSize, gIndex, gEnd, gCells, gVals, gOwners);
    //@ assert( length(gCells) == length(gOwners) );
    //@ assert( length(gCells) == gSize +1 );

    //@ close exists(pxReadyList);
    uxListRemove( pxTaskItem );
    //@ assert( xLIST(pxReadyList, gSize-1, ?gIndex2, gEnd, ?gCells2, ?gVals2, ?gOwners2) );
    //@ assert( xLIST_ITEM(pxTaskItem, _, _, _, ?gTaskItem_owner, _) );

    // Proving `length(gCell2) == length(gOwners2) == gSize` and `gIndex2 ∈ gCells2`:
        //@ open xLIST(pxReadyList, gSize-1, gIndex2, gEnd, gCells2, gVals2, gOwners2);
        //@ close xLIST(pxReadyList, gSize-1, gIndex2, gEnd, gCells2, gVals2, gOwners2);
    //@ assert( length(gCells2) == gSize );
    //@ assert( length(gOwners2) == gSize );
    //@ assert( mem(gIndex2, gCells2) == true );

    // Proving `gTaskItem_owner ∈ gOwners`:
        //@ assert( gTaskItem_owner == nth(index_of(pxTaskItem, gCells), gOwners) );
        //@ mem_index_of(pxTaskItem, gCells);
        //@ nth_implies_mem(index_of(pxTaskItem, gCells), gOwners);
    //@ assert( mem(gTaskItem_owner, gOwners) == true );

    // Proving `gTaskItem_owner ∈ gTasks`:
        //@ forall_mem(gTaskItem_owner, gOwners, (mem_list_elem)(gTasks));
    //@ assert( mem(gTaskItem_owner, gTasks) == true );

    // Proving `gOwners2 ⊆ gTasks` 
        //@ assert( forall(gOwners, (mem_list_elem)(gTasks)) == true );
        //@ forall_remove_nth(index_of(pxTaskItem, gCells), gOwners, (mem_list_elem)(gTasks));
        //@ assert( forall(gOwners2, (mem_list_elem)(gTasks)) == true );
        //@ forall_mem_implies_superset(gTasks, gOwners2);
    //@ assert( subset(gOwners2, gTasks) == true );

    vListInsertEnd( pxReadyList, pxTaskItem );
    //@ assert( xLIST(pxReadyList, gSize, ?gIndex3, gEnd, ?gCells3, ?gVals3, ?gOwners3) );

    // Proving `gOwners3 ⊆ gTasks` and `length(gOwners3) == length(gOwners)`:
    // We must handle the case split introduced by postcondition of `vListInsertEnd`.
        /*@
        if( gIndex2 == gEnd ) {
            assert( gCells3 == append(gCells2, singleton(pxTaskItem)) );
            assert( gOwners3 == append(gOwners2, singleton(gTaskItem_owner)) );
            
            assert( subset(singleton(gTaskItem_owner), gTasks) == true );
            subset_append(gOwners2, singleton(gTaskItem_owner), gTasks);
        } else {
            int i = index_of(gIndex2, gCells2);
            assert( gCells3 == append(take(i, gCells2),
                                      append(singleton(pxTaskItem), 
                                             drop(i, gCells2))) );
            list<void*> ot = append(singleton(gTaskItem_owner), drop(i, gOwners2));
            assert( gOwners3 == append(take(i, gOwners2), ot) );
            
            
            // Proving `take(i, gOwners2) ⊆ gTasks`:
                subset_take(i, gOwners2);
                assert( subset(take(i, gOwners2), gOwners2) == true );
                assert( subset(gOwners2, gTasks) == true );
                subset_trans(take(i, gOwners2), gOwners2, gTasks);
            assert( subset(take(i, gOwners2), gTasks) == true );

            // Proving `drop(i, gOwners2) ⊆ gTasks`:
                subset_drop(i, gOwners2);
                subset_trans(drop(i, gOwners2), gOwners2, gTasks);
            assert( subset(drop(i, gOwners2), gTasks) == true );
            
            // Proving `gOwners3 ⊆ gTasks`:
                subset_append(singleton(gTaskItem_owner), drop(i, gOwners2), gTasks);
                subset_append(take(i, gOwners2), ot, gTasks);
            assert( subset(gOwners3, gTasks) == true );        

            // Proving `length(gOwners3) == length(gOwners)`:
                mem_index_of(gIndex2, gCells2);
                append_take_nth_drop(i, gOwners2);
            assert( length(gOwners3) == gSize+1 );
        }
        @*/
    //@ assert( subset(gOwners3, gTasks) == true );
    //@ assert( length(gOwners3) == length(gOwners) );

    //@ subset_implies_forall_mem(gOwners3, gTasks);
    //@ assert( forall(gOwners3, (mem_list_elem)(gTasks)) == true );

    //@ closeReordered_readyLists(gCellLists, gOwnerLists, gCells3, gOwners3, gTasks);

    // Proving that reordering preserves the length of cell lists and owner lists:
        //@ open readyLists_p(?gReorderedCellLists, ?gReorderedOwnerLists);
        //@ close readyLists_p(gReorderedCellLists, gReorderedOwnerLists);
    //@ assert( length(gReorderedCellLists) == length(gCellLists) );
    //@ assert( length(gReorderedOwnerLists) == length(gOwnerLists) );
    //@ assert( length(gReorderedCellLists) == length(gReorderedOwnerLists) );
}
#endif /* READY_LIST_PREDICATES_H */