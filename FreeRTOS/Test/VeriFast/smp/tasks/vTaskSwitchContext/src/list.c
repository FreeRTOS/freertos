/*
 * FreeRTOS SMP Kernel V202110.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


#include <stdlib.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
 * all the API functions to use the MPU wrappers.  That should only be done when
 * task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "list.h"

/* Lint e9021, e961 and e750 are suppressed as a MISRA exception justified
 * because the MPU ports require MPU_WRAPPERS_INCLUDED_FROM_API_FILE to be
 * defined for the header files above, but not in this file, in order to
 * generate the correct privileged Vs unprivileged linkage and placement. */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE /*lint !e961 !e750 !e9021. */

/*-----------------------------------------------------------
* PUBLIC LIST API documented in list.h
*----------------------------------------------------------*/

void vListInitialise( List_t * const pxList )
{
    /* The list structure contains a list item which is used to mark the
     * end of the list.  To initialise the list the list end is inserted
     * as the only list entry. */
    pxList->pxIndex = ( ListItem_t * ) &( pxList->xListEnd ); /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

    /* The list end value is the highest possible value in the list to
     * ensure it remains at the end of the list. */
    pxList->xListEnd.xItemValue = portMAX_DELAY;

    /* The list end next and previous pointers point to itself so we know
     * when the list is empty. */
    pxList->xListEnd.pxNext = ( ListItem_t * ) &( pxList->xListEnd );     /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */
    pxList->xListEnd.pxPrevious = ( ListItem_t * ) &( pxList->xListEnd ); /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

    pxList->uxNumberOfItems = ( UBaseType_t ) 0U;

    /* Write known values into the list if
     * configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
    listSET_LIST_INTEGRITY_CHECK_1_VALUE( pxList );
    listSET_LIST_INTEGRITY_CHECK_2_VALUE( pxList );
}
/*-----------------------------------------------------------*/

void vListInitialiseItem( ListItem_t * const pxItem )
//@ requires pxItem->pxContainer |-> _;
//@ ensures pxItem->pxContainer |-> 0;
{
    /* Make sure the list item is not recorded as being on a list. */
    pxItem->pxContainer = NULL;

    /* Write known values into the list item if
     * configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
    listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem );
    listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem );
}
/*-----------------------------------------------------------*/

void vListInsertEnd( List_t * const pxList,
                     ListItem_t * const pxNewListItem )
#ifndef VERIFAST_SINGLE_CORE
    /* Reason for rewrite:
	 * Predicates `xLIST_ITEM`, `DLS` and `xLIST` have been extended to expose
	 * node owners. Proofs using these predicates must be adapted as well.
	 */
    
    // TODO: Adapt contract and proof to new version of predicates.

    /*@requires xLIST(pxList, ?len, ?idx, ?end, ?cells, ?vals, ?owners) &*&
        xLIST_ITEM(pxNewListItem, ?val, _, _, ?ow, _) &*&
        len < INT_MAX;@*/
    /*@ensures xLIST(pxList, len+1, idx, end, ?new_cells, ?new_vals, ?new_owners) &*&
        idx == end
            ? (new_cells == append(cells, singleton(pxNewListItem)) &*&
                new_vals == append(vals, singleton(val)) &*&
                new_owners == append(owners, singleton(ow)))
            : (new_cells == append(take(index_of(idx, cells), cells), append(singleton(pxNewListItem), drop(index_of(idx, cells), cells))) &*&
                new_vals == append(take(index_of(idx, cells), vals), append(singleton(val), drop(index_of(idx, cells), vals))) &*&
                new_owners == append(take(index_of(idx, cells), owners), append(singleton(ow), drop(index_of(idx, cells), owners))));@*/
    {
        /*@xLIST_star_item(pxList, pxNewListItem);@*/
        /*@assert mem(pxNewListItem, cells) == false;@*/
        /*@open xLIST(pxList, len, idx, end, cells, vals, owners);@*/
    #ifdef VERIFAST /*< const pointer declaration */
        ListItem_t * pxIndex = pxList->pxIndex;
    #else
        ListItem_t * const pxIndex = pxList->pxIndex;

        /* Only effective when configASSERT() is also defined, these tests may catch
        * the list data structures being overwritten in memory.  They will not catch
        * data errors caused by incorrect configuration or use of FreeRTOS. */
        listTEST_LIST_INTEGRITY( pxList );
        listTEST_LIST_ITEM_INTEGRITY( pxNewListItem );
    #endif

        /*@open xLIST_ITEM(pxNewListItem, _, _, _, _, _);@*/
        /*@assert DLS(end, ?endprev, end, _, cells, vals, owners, pxList);@*/
        /*@dls_first_mem(end, endprev, end, endprev, cells);@*/
        /*@dls_last_mem(end, endprev, end, endprev, cells);@*/
        /*@
        if (end == idx)
        {
            open DLS(end, endprev, end, endprev, cells, vals, owners, pxList);
            open xLIST_ITEM(end, portMAX_DELAY, ?endnext, endprev, head(owners), pxList);
            if (end == endprev)
            {
                // Case A (singleton): idx==end==endprev
            }
            else
            {
                assert DLS(endnext, end, end, endprev, tail(cells), tail(vals), tail(owners), pxList);
                if (endnext == endprev)
                {
                    // Case B (two): idx==end and endnext==endprev
                    open DLS(endnext, end, end, endnext, _, _, _, _);
                    open xLIST_ITEM(endnext, _, _, _, _, _);
                }
                else
                {
                    // Case C: idx==end and DLS:endnext...endprev
                    split(endnext, end, end, endprev, tail(cells), tail(vals), endprev, index_of(endprev, tail(cells)));
                    open DLS(endprev, _, _, _, _, _, _, _);
                    open xLIST_ITEM(endprev, _, _, _, _, _);
                }
            }
        }
        else
        {
            int i = index_of(idx, cells);
            split(end, endprev, end, endprev, cells, vals, idx, i);
            assert DLS(end, endprev, idx, ?idxprev, take(i, cells), take(i, vals), take(i, owners), pxList);
            assert DLS(idx, idxprev, end, endprev, drop(i, cells), drop(i, vals), drop(i, owners), pxList);
            open DLS(idx, idxprev, end, endprev, _, _, _, _);
            open xLIST_ITEM(idx, _, _, _, _, _);
            if (end == idxprev)
            {
                // Case D: end==idxprev and DLS:idx...endprev
                take_take(1, i, vals);
                take_head(vals);
                open DLS(end, endprev, idx, idxprev, take(i, cells), take(i, vals), take(i, owners), pxList);
                open xLIST_ITEM(end, portMAX_DELAY, _, _, _, _);
                assert length(take(i, cells)) == 1;
            }
            else
            {
                // Case E: DLS:end...idxprev and DLS:idx...endprev
                dls_last_mem(end, endprev, idx, idxprev, take(i, cells));
                split(end, endprev, idx, idxprev, take(i, cells), take(i, vals), idxprev, index_of(idxprev, take(i, cells)));
                open DLS(idxprev, _, _, idxprev, _, _, _, _);
                length_take(i, cells);
                drop_take_singleton(i, vals);
                drop_take_singleton(i, owners);
                open xLIST_ITEM(idxprev, nth(i-1, vals), _, _, _, _);
            }
        }
        @*/

        /* Insert a new list item into pxList, but rather than sort the list,
        * makes the new list item the last item to be removed by a call to
        * listGET_OWNER_OF_NEXT_ENTRY(). */
        pxNewListItem->pxNext = pxIndex;
        pxNewListItem->pxPrevious = pxIndex->pxPrevious;

        /* Only used during decision coverage testing. */
        mtCOVERAGE_TEST_DELAY();

        pxIndex->pxPrevious->pxNext = pxNewListItem;
        pxIndex->pxPrevious = pxNewListItem;

        /* Remember which list the item is in. */
        pxNewListItem->pxContainer = pxList;

        ( pxList->uxNumberOfItems )++;

        /*@
        if (end == idx)
        {
            close xLIST_ITEM(pxNewListItem, val, end, endprev, ow, pxList);
            close DLS(pxNewListItem, endprev, end, pxNewListItem, singleton(pxNewListItem), singleton(val), singleton(ow), pxList);
            close xLIST_ITEM(end, portMAX_DELAY, ?endnext, pxNewListItem, head(owners), pxList);
            if (end == endprev)
            {
                // Case A (singleton): idx==end==endprev
                close DLS(end, pxNewListItem, endnext, end, cells, vals, owners, pxList);
                join(end, pxNewListItem, endnext, end, cells, vals,
                    pxNewListItem, endprev, end, pxNewListItem, singleton(pxNewListItem), singleton(val));
                close xLIST(pxList, len+1, idx, end, append(cells, singleton(pxNewListItem)), append(vals, singleton(val)), append(owners, singleton(ow)));
            }
            else
            {
                close xLIST_ITEM(endprev, ?endprevval, pxNewListItem, ?endprevprev, ?endprevowner, _);
                if (endnext == endprev)
                {
                    // Case B (two): idx==end and endnext==endprev
                    close DLS(endprev, end, pxNewListItem, endprev, singleton(endprev), singleton(endprevval), singleton(endprevowner), pxList);
                    close DLS(end, pxNewListItem, pxNewListItem, endprev, cells, vals, owners, pxList);
                    join(end, pxNewListItem, pxNewListItem, endprev, cells, vals,
                        pxNewListItem, endprev, end, pxNewListItem, singleton(pxNewListItem), singleton(val));
                    close xLIST(pxList, len+1, idx, end, append(cells, singleton(pxNewListItem)), append(vals, singleton(val)), append(owners, singleton(ow)));
                }
                else
                {
                    // Case C: idx==end and DLS:endnext...endprev
                    close DLS(endprev, endprevprev, pxNewListItem, endprev, singleton(endprev), singleton(endprevval), singleton(endprevowner), pxList);
                    assert DLS(endnext, end, endprev, endprevprev, ?cells_endnext_to_endprevprev, ?vals_endnext_to_endprevprev, _, pxList);
                    join(endnext, end, endprev, endprevprev, cells_endnext_to_endprevprev, vals_endnext_to_endprevprev,
                        endprev, endprevprev, pxNewListItem, endprev, singleton(endprev), singleton(endprevval));
                    close DLS(end, pxNewListItem, pxNewListItem, endprev, cells, vals, owners, pxList);
                    join(end, pxNewListItem, pxNewListItem, endprev, cells, vals,
                        pxNewListItem, endprev, end, pxNewListItem, singleton(pxNewListItem), singleton(val));
                    close xLIST(pxList, len+1, idx, end, append(cells, singleton(pxNewListItem)), append(vals, singleton(val)), append(owners, singleton(ow)));
                }
            }
        }
        else
        {
            // Case D: end==idxprev and DLS:idx...endprev
            // Case E: DLS:end...idxprev and DLS:idx...endprev
            int i = index_of(idx, cells);
            close xLIST_ITEM(pxNewListItem, val, idx, ?idxprev, ow, pxList);
            close xLIST_ITEM(idx, ?idxval, ?idxnext, pxNewListItem, ?idxowner, pxList);
            nth_drop2(vals, i);
            assert idxval == nth(i, vals);
            nth_drop2(owners, i);
            assert idxowner == nth(i, owners);
            close xLIST_ITEM(idxprev, ?idxprevval, pxNewListItem, ?idxprevprev, ?idxprevowner, pxList);

            if (end == idxprev)
            {
                close DLS(end, endprev, pxNewListItem, end, singleton(end), singleton(portMAX_DELAY), singleton(head(owners)), pxList);
            }
            else
            {
                length_take(i, cells);
                take_take(i-1, i, vals);
                take_singleton(i-1, vals);
                take_singleton(i, vals);
                take_take(i-1, i, owners);
                take_singleton(i-1, owners);
                take_singleton(i, owners);
                assert DLS(end, endprev, idxprev, idxprevprev, ?cells_end_to_idxprevprev, take(i-1, vals), take(i-1, owners), pxList);
                close DLS(idxprev, idxprevprev, pxNewListItem, idxprev, singleton(idxprev), singleton(idxprevval), singleton(idxprevowner), pxList);
                join(end, endprev, idxprev, idxprevprev, cells_end_to_idxprevprev, take(i-1, vals),
                    idxprev, idxprevprev, pxNewListItem, idxprev, singleton(idxprev), singleton(idxprevval));
            }

            if (idx == endprev)
            {
                close DLS(idx, pxNewListItem, end, idx, singleton(idx), singleton(idxval), singleton(idxowner), pxList);
            }
            else
            {
                assert DLS(end, endprev, pxNewListItem, idxprev, ?cells_end_to_idxprev, ?vals_end_to_idxprev, _, pxList);
                close DLS(idx, pxNewListItem, end, endprev, drop(i, cells), drop(i, vals), drop(i, owners), pxList);
            }

            assert DLS(end, endprev, pxNewListItem, idxprev, take(i, cells), take(i, vals), take(i, owners), pxList);
            assert DLS(idx, pxNewListItem, end, endprev, drop(i, cells), drop(i, vals), drop(i, owners), pxList);
            assert xLIST_ITEM(pxNewListItem, val, idx, idxprev, ow, pxList);
            dls_star_item(idx, endprev, pxNewListItem);
            close DLS(pxNewListItem, idxprev, end, endprev, cons(pxNewListItem, drop(i, cells)), cons(val, drop(i, vals)), cons(ow, drop(i, owners)), pxList);
            join(end, endprev, pxNewListItem, idxprev, take(i, cells), take(i, vals),
                pxNewListItem, idxprev, end, endprev, cons(pxNewListItem, drop(i, cells)), cons(val, drop(i, vals)));
            assert DLS(end, endprev, end, endprev, ?cells_new, ?vals_new, ?owners_new, pxList);
            assert cells_new == append(take(i, cells), append(singleton(pxNewListItem), drop(i, cells)));
            assert vals_new == append(take(i, vals) , append(singleton(val), drop(i, vals)));
            assert owners_new == append(take(i, owners) , append(singleton(ow), drop(i, owners)));
            head_append(take(i, cells), append(singleton(pxNewListItem), drop(i, cells)));
            take_take(1, i, cells);
            head_append(take(i, vals), append(singleton(val), drop(i, vals)));
            take_take(1, i, vals);
            close xLIST(pxList, len+1, idx, end, cells_new, vals_new, owners_new);
        }
        @*/
    }
#else    
    /* The contract and proof below have been wirtten by Aalok Thakkar and Nathan
    * Chong in 2020 for the single-core setup.
    */
    /*@requires xLIST(pxList, ?len, ?idx, ?end, ?cells, ?vals) &*&
        xLIST_ITEM(pxNewListItem, ?val, _, _, _);@*/
    /*@ensures xLIST(pxList, len+1, idx, end, ?new_cells, ?new_vals) &*&
        idx == end
            ? (new_cells == append(cells, singleton(pxNewListItem)) &*&
                new_vals == append(vals, singleton(val)))
            : (new_cells == append(take(index_of(idx, cells), cells), append(singleton(pxNewListItem), drop(index_of(idx, cells), cells))) &*&
                new_vals == append(take(index_of(idx, cells), vals), append(singleton(val), drop(index_of(idx, cells), vals))));@*/
    {
        /*@xLIST_star_item(pxList, pxNewListItem);@*/
        /*@assert mem(pxNewListItem, cells) == false;@*/
        /*@open xLIST(pxList, len, idx, end, cells, vals);@*/
    #ifdef VERIFAST /*< const pointer declaration */
        ListItem_t * pxIndex = pxList->pxIndex;
    #else
        ListItem_t * const pxIndex = pxList->pxIndex;

        /* Only effective when configASSERT() is also defined, these tests may catch
        * the list data structures being overwritten in memory.  They will not catch
        * data errors caused by incorrect configuration or use of FreeRTOS. */
        listTEST_LIST_INTEGRITY( pxList );
        listTEST_LIST_ITEM_INTEGRITY( pxNewListItem );
    #endif

        /*@open xLIST_ITEM(pxNewListItem, _, _, _, _);@*/
        /*@assert DLS(end, ?endprev, end, _, cells, vals, pxList);@*/
        /*@dls_first_mem(end, endprev, end, endprev, cells);@*/
        /*@dls_last_mem(end, endprev, end, endprev, cells);@*/
        /*@
        if (end == idx)
        {
            open DLS(end, endprev, end, endprev, cells, vals, pxList);
            open xLIST_ITEM(end, portMAX_DELAY, ?endnext, endprev, pxList);
            if (end == endprev)
            {
                // Case A (singleton): idx==end==endprev
            }
            else
            {
                assert DLS(endnext, end, end, endprev, tail(cells), tail(vals), pxList);
                if (endnext == endprev)
                {
                    // Case B (two): idx==end and endnext==endprev
                    open DLS(endnext, end, end, endnext, _, _, _);
                    open xLIST_ITEM(endnext, _, _, _, _);
                }
                else
                {
                    // Case C: idx==end and DLS:endnext...endprev
                    split(endnext, end, end, endprev, tail(cells), tail(vals), endprev, index_of(endprev, tail(cells)));
                    open DLS(endprev, _, _, _, _, _, _);
                    open xLIST_ITEM(endprev, _, _, _, _);
                }
            }
        }
        else
        {
            int i = index_of(idx, cells);
            split(end, endprev, end, endprev, cells, vals, idx, i);
            assert DLS(end, endprev, idx, ?idxprev, take(i, cells), take(i, vals), pxList);
            assert DLS(idx, idxprev, end, endprev, drop(i, cells), drop(i, vals), pxList);
            open DLS(idx, idxprev, end, endprev, _, _, _);
            open xLIST_ITEM(idx, _, _, _, _);
            if (end == idxprev)
            {
                // Case D: end==idxprev and DLS:idx...endprev
                take_take(1, i, vals);
                take_head(vals);
                open DLS(end, endprev, idx, idxprev, take(i, cells), take(i, vals), pxList);
                open xLIST_ITEM(end, portMAX_DELAY, _, _, _);
                assert length(take(i, cells)) == 1;
            }
            else
            {
                // Case E: DLS:end...idxprev and DLS:idx...endprev
                dls_last_mem(end, endprev, idx, idxprev, take(i, cells));
                split(end, endprev, idx, idxprev, take(i, cells), take(i, vals), idxprev, index_of(idxprev, take(i, cells)));
                open DLS(idxprev, _, _, idxprev, _, _, _);
                length_take(i, cells);
                drop_take_singleton(i, vals);
                open xLIST_ITEM(idxprev, nth(i-1, vals), _, _, _);
            }
        }
        @*/

        /* Insert a new list item into pxList, but rather than sort the list,
        * makes the new list item the last item to be removed by a call to
        * listGET_OWNER_OF_NEXT_ENTRY(). */
        pxNewListItem->pxNext = pxIndex;
        pxNewListItem->pxPrevious = pxIndex->pxPrevious;

        /* Only used during decision coverage testing. */
        mtCOVERAGE_TEST_DELAY();

        pxIndex->pxPrevious->pxNext = pxNewListItem;
        pxIndex->pxPrevious = pxNewListItem;

        /* Remember which list the item is in. */
        pxNewListItem->pxContainer = pxList;

        ( pxList->uxNumberOfItems )++;

        /*@
        if (end == idx)
        {
            close xLIST_ITEM(pxNewListItem, val, end, endprev, pxList);
            close DLS(pxNewListItem, endprev, end, pxNewListItem, singleton(pxNewListItem), singleton(val), pxList);
            close xLIST_ITEM(end, portMAX_DELAY, ?endnext, pxNewListItem, pxList);
            if (end == endprev)
            {
                // Case A (singleton): idx==end==endprev
                close DLS(end, pxNewListItem, endnext, end, cells, vals, pxList);
                join(end, pxNewListItem, endnext, end, cells, vals,
                    pxNewListItem, endprev, end, pxNewListItem, singleton(pxNewListItem), singleton(val));
                close xLIST(pxList, len+1, idx, end, append(cells, singleton(pxNewListItem)), append(vals, singleton(val)));
            }
            else
            {
                close xLIST_ITEM(endprev, ?endprevval, pxNewListItem, ?endprevprev, _);
                if (endnext == endprev)
                {
                    // Case B (two): idx==end and endnext==endprev
                    close DLS(endprev, end, pxNewListItem, endprev, singleton(endprev), singleton(endprevval), pxList);
                    close DLS(end, pxNewListItem, pxNewListItem, endprev, cells, vals, pxList);
                    join(end, pxNewListItem, pxNewListItem, endprev, cells, vals,
                        pxNewListItem, endprev, end, pxNewListItem, singleton(pxNewListItem), singleton(val));
                    close xLIST(pxList, len+1, idx, end, append(cells, singleton(pxNewListItem)), append(vals, singleton(val)));
                }
                else
                {
                    // Case C: idx==end and DLS:endnext...endprev
                    close DLS(endprev, endprevprev, pxNewListItem, endprev, singleton(endprev), singleton(endprevval), pxList);
                    assert DLS(endnext, end, endprev, endprevprev, ?cells_endnext_to_endprevprev, ?vals_endnext_to_endprevprev, pxList);
                    join(endnext, end, endprev, endprevprev, cells_endnext_to_endprevprev, vals_endnext_to_endprevprev,
                        endprev, endprevprev, pxNewListItem, endprev, singleton(endprev), singleton(endprevval));
                    close DLS(end, pxNewListItem, pxNewListItem, endprev, cells, vals, pxList);
                    join(end, pxNewListItem, pxNewListItem, endprev, cells, vals,
                        pxNewListItem, endprev, end, pxNewListItem, singleton(pxNewListItem), singleton(val));
                    close xLIST(pxList, len+1, idx, end, append(cells, singleton(pxNewListItem)), append(vals, singleton(val)));
                }
            }
        }
        else
        {
            // Case D: end==idxprev and DLS:idx...endprev
            // Case E: DLS:end...idxprev and DLS:idx...endprev
            int i = index_of(idx, cells);
            close xLIST_ITEM(pxNewListItem, val, idx, ?idxprev, pxList);
            close xLIST_ITEM(idx, ?idxval, ?idxnext, pxNewListItem, pxList);
            nth_drop2(vals, i);
            assert idxval == nth(i, vals);
            close xLIST_ITEM(idxprev, ?idxprevval, pxNewListItem, ?idxprevprev, pxList);

            if (end == idxprev)
            {
                close DLS(end, endprev, pxNewListItem, end, singleton(end), singleton(portMAX_DELAY), pxList);
            }
            else
            {
                length_take(i, cells);
                take_take(i-1, i, vals);
                take_singleton(i-1, vals);
                take_singleton(i, vals);
                assert DLS(end, endprev, idxprev, idxprevprev, ?cells_end_to_idxprevprev, take(i-1, vals), pxList);
                close DLS(idxprev, idxprevprev, pxNewListItem, idxprev, singleton(idxprev), singleton(idxprevval), pxList);
                join(end, endprev, idxprev, idxprevprev, cells_end_to_idxprevprev, take(i-1, vals),
                    idxprev, idxprevprev, pxNewListItem, idxprev, singleton(idxprev), singleton(idxprevval));
            }

            if (idx == endprev)
            {
                close DLS(idx, pxNewListItem, end, idx, singleton(idx), singleton(idxval), pxList);
            }
            else
            {
                assert DLS(end, endprev, pxNewListItem, idxprev, ?cells_end_to_idxprev, ?vals_end_to_idxprev, pxList);
                close DLS(idx, pxNewListItem, end, endprev, drop(i, cells), drop(i, vals), pxList);
            }

            assert DLS(end, endprev, pxNewListItem, idxprev, take(i, cells), take(i, vals), pxList);
            assert DLS(idx, pxNewListItem, end, endprev, drop(i, cells), drop(i, vals), pxList);
            assert xLIST_ITEM(pxNewListItem, val, idx, idxprev, pxList);
            dls_star_item(idx, endprev, pxNewListItem);
            close DLS(pxNewListItem, idxprev, end, endprev, cons(pxNewListItem, drop(i, cells)), cons(val, drop(i, vals)), pxList);
            join(end, endprev, pxNewListItem, idxprev, take(i, cells), take(i, vals),
                pxNewListItem, idxprev, end, endprev, cons(pxNewListItem, drop(i, cells)), cons(val, drop(i, vals)));
            assert DLS(end, endprev, end, endprev, ?cells_new, ?vals_new, pxList);
            assert cells_new == append(take(i, cells), append(singleton(pxNewListItem), drop(i, cells)));
            assert vals_new == append(take(i, vals) , append(singleton(val), drop(i, vals)));
            head_append(take(i, cells), append(singleton(pxNewListItem), drop(i, cells)));
            take_take(1, i, cells);
            head_append(take(i, vals), append(singleton(val), drop(i, vals)));
            take_take(1, i, vals);
            close xLIST(pxList, len+1, idx, end, cells_new, vals_new);
        }
        @*/
    }
#endif /* VERIFAST_SINGLE_CORE */
/*-----------------------------------------------------------*/

void vListInsert( List_t * const pxList,
                  ListItem_t * const pxNewListItem )
{
    ListItem_t * pxIterator;
    const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;

    /* Only effective when configASSERT() is also defined, these tests may catch
     * the list data structures being overwritten in memory.  They will not catch
     * data errors caused by incorrect configuration or use of FreeRTOS. */
    listTEST_LIST_INTEGRITY( pxList );
    listTEST_LIST_ITEM_INTEGRITY( pxNewListItem );

    /* Insert the new list item into the list, sorted in xItemValue order.
     *
     * If the list already contains a list item with the same item value then the
     * new list item should be placed after it.  This ensures that TCBs which are
     * stored in ready lists (all of which have the same xItemValue value) get a
     * share of the CPU.  However, if the xItemValue is the same as the back marker
     * the iteration loop below will not end.  Therefore the value is checked
     * first, and the algorithm slightly modified if necessary. */
    if( xValueOfInsertion == portMAX_DELAY )
    {
        pxIterator = pxList->xListEnd.pxPrevious;
    }
    else
    {
        /* *** NOTE ***********************************************************
        *  If you find your application is crashing here then likely causes are
        *  listed below.  In addition see https://www.FreeRTOS.org/FAQHelp.html for
        *  more tips, and ensure configASSERT() is defined!
        *  https://www.FreeRTOS.org/a00110.html#configASSERT
        *
        *   1) Stack overflow -
        *      see https://www.FreeRTOS.org/Stacks-and-stack-overflow-checking.html
        *   2) Incorrect interrupt priority assignment, especially on Cortex-M
        *      parts where numerically high priority values denote low actual
        *      interrupt priorities, which can seem counter intuitive.  See
        *      https://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html and the definition
        *      of configMAX_SYSCALL_INTERRUPT_PRIORITY on
        *      https://www.FreeRTOS.org/a00110.html
        *   3) Calling an API function from within a critical section or when
        *      the scheduler is suspended, or calling an API function that does
        *      not end in "FromISR" from an interrupt.
        *   4) Using a queue or semaphore before it has been initialised or
        *      before the scheduler has been started (are interrupts firing
        *      before vTaskStartScheduler() has been called?).
        *   5) If the FreeRTOS port supports interrupt nesting then ensure that
        *      the priority of the tick interrupt is at or below
        *      configMAX_SYSCALL_INTERRUPT_PRIORITY.
        **********************************************************************/

        for( pxIterator = ( ListItem_t * ) &( pxList->xListEnd ); pxIterator->pxNext->xItemValue <= xValueOfInsertion; pxIterator = pxIterator->pxNext ) /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. *//*lint !e440 The iterator moves to a different value, not xValueOfInsertion. */
        {
            /* There is nothing to do here, just iterating to the wanted
             * insertion position. */
        }
    }

    pxNewListItem->pxNext = pxIterator->pxNext;
    pxNewListItem->pxNext->pxPrevious = pxNewListItem;
    pxNewListItem->pxPrevious = pxIterator;
    pxIterator->pxNext = pxNewListItem;

    /* Remember which list the item is in.  This allows fast removal of the
     * item later. */
    pxNewListItem->pxContainer = pxList;

    ( pxList->uxNumberOfItems )++;
}
/*-----------------------------------------------------------*/

UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
#ifndef VERIFAST_SINGLE_CORE
    /* Reason for rewrite:
	 * Predicates `xLIST_ITEM`, `DLS` and `xLIST` have been extended to expose
	 * node owners. Proofs using these predicates must be adapted as well.
	 */

    /*@requires
        exists<struct xLIST * >(?l) &*&
        xLIST(l, ?len, ?idx, ?end, ?cells, ?vals, ?owners) &*&
        end != pxItemToRemove &*&
        mem(pxItemToRemove, cells) == true;@*/
    /*@ensures
        result == len-1 &*&
        xLIST_ITEM(pxItemToRemove, nth(index_of(pxItemToRemove, cells), vals), _, ?pxItemToRemovePrevious, nth(index_of(pxItemToRemove, cells), owners), NULL) &*&
        pxItemToRemovePrevious == nth(index_of(pxItemToRemove, cells)-1, cells) &*&
        xLIST(l, result, idx == pxItemToRemove ? pxItemToRemovePrevious : idx,  end, remove(pxItemToRemove, cells), remove_nth(index_of(pxItemToRemove, cells), vals), remove_nth(index_of(pxItemToRemove, cells), owners));
    @*/
    {
    /* For brevity we alias x to pxItemToRemove */
    /*@struct xLIST_ITEM *x = pxItemToRemove;@*/

    /* Start by establishing that the list must be non-empty since x != end */
    /*@open xLIST(l, len, idx, end, cells, vals, owners);@*/
    /*@assert DLS(end, ?endprev, end, _, cells, vals, owners, l);@*/
    /*@assert vals == cons(portMAX_DELAY, _);@*/
    /*@dls_not_empty(end, endprev, cells, x);@*/

    /* We know the xLIST is a DLS: end...endprev
    Split this into DLS1:end...xprev and DLS2:x...endprev */
    /*@int i = index_of(x, cells);@*/
    /*@split(end, endprev, end, endprev, cells, vals, x, i);@*/
    /*@list<struct xLIST_ITEM *> ys = take(i, cells);@*/
    /*@list<struct xLIST_ITEM *> zs = drop(i, cells);@*/
    /*@list<TickType_t> vs = take(i, vals);@*/
    /*@list<TickType_t> ws = drop(i, vals);@*/
    /*@list<void*> ts = take(i, owners);@*/
    /*@list<void*> us = drop(i, owners);@*/
    /*@assert length(ys) == length(vs);@*/
    /*@assert length(zs) == length(ws);@*/
    /*@assert length(ts) == length(vs);@*/
    /*@assert length(us) == length(ws);@*/
    /*@assert DLS(end, endprev, x, ?xprev, ys, vs, ts, l);@*/ /*< DLS1 (ys, vs) */
    /*@assert DLS(x, xprev, end, endprev, zs, ws, us, l);@*/  /*< DLS2 (zs, ws) */

    /* Now case split to open DLS1 and DLS2 appropriately */
    /*@
    if (end == xprev)
    {
        if (x == endprev)
        {
            //Case A
            //DLS1: extract end=prev=next
            open DLS(end, endprev, x, xprev, ys, vs, ts, l);
            assert owners == cons(_, _);
            open xLIST_ITEM(end, portMAX_DELAY, x, endprev, head(owners), l);
            //DLS2: extract x
            open DLS(x, xprev, end, endprev, zs, ws, us, l);
            //Lengths
            assert length(ys) == 1;
            assert length(zs) == 1;
            assert length(us) == 1;
        }
        else
        {
            //Case B
            //DLS1: extract end=prev
            open DLS(end, endprev, x, xprev, ys, vs, ts, l);
            open xLIST_ITEM(end, portMAX_DELAY, x, endprev, head(owners), l);
            //DLS2: extract next and x
            open DLS(x, end, end, endprev, zs, ws, us, l);
            assert DLS(?xnext, x, end, endprev, tail(zs), tail(ws), tail(us), l);
            open DLS(xnext, x, end, endprev, tail(zs), tail(ws), tail(us), l);
            open xLIST_ITEM(xnext, _, _, x, _, l);
            //Lengths
            assert length(ys) == 1;
        }
    }
    else
    {
        if (x == endprev)
        {
            //Case C
            //DLS1: extract end=next and prev
            dls_last_mem(end, endprev, x, xprev, ys);
            assert mem(xprev, ys) == true;
            open DLS(end, endprev, x, xprev, ys, vs, ts, l);
            open xLIST_ITEM(end, portMAX_DELAY, ?endnext, endprev, head(ts), l);
            if (endnext == xprev)
            {
                open DLS(endnext, end, x, xprev, tail(ys), tail(vs), tail(ts), l);
                open xLIST_ITEM(xprev, _, x, _, _, l);
            }
            else
            {
                assert DLS(endnext, end, x, xprev, tail(ys), tail(vs), tail(ts), l);
                int k = index_of(xprev, tail(ys));
                dls_last_mem(endnext, end, x, xprev, tail(ys));
                split(endnext, end, x, xprev, tail(ys), tail(vs), xprev, k);
                open DLS(xprev, _, x, xprev, _, _, _, l);
                open xLIST_ITEM(xprev, _, x, _, _, l);
            }
            //DLS2: extract x
            open DLS(x, xprev, end, endprev, zs, ws, us, l);
            //Lengths
            assert length(zs) == 1;
        }
        else
        {
            //Case D
            //DLS1: extract prev
            dls_last_mem(end, endprev, x, xprev, ys);
            int j = index_of(xprev, ys);
            open DLS(end, endprev, x, xprev, ys, vs, ts, l);
            open xLIST_ITEM(end, portMAX_DELAY, ?endnext, endprev, head(ts), l);
            if (endnext == xprev)
            {
                open DLS(endnext, end, x, xprev, tail(ys), tail(vs), tail(ts), l);
                assert tail(ys) == singleton(xprev);
                open xLIST_ITEM(xprev, _, x, _, _, l);
            }
            else
            {
                assert DLS(endnext, end, x, xprev, tail(ys), tail(vs), tail(ts), l);
                int k = index_of(xprev, tail(ys));
                dls_last_mem(endnext, end, x, xprev, tail(ys));
                split(endnext, end, x, xprev, tail(ys), tail(vs), xprev, k);
                open DLS(xprev, _, x, xprev, _, _, _, l);
                open xLIST_ITEM(xprev, _, x, _, _, l);
            }
            //DLS2: extract next and x
            open DLS(x, xprev, end, endprev, zs, ws, us, l);
            assert xLIST_ITEM(x, _, ?xnext, _, _, l);
            open DLS(xnext, x, end, endprev, tail(zs), tail(ws), tail(us), l);
            open xLIST_ITEM(xnext, _, _, x, _, l);
        }
    }
    @*/
    /*@drop_nth_index_of(vals, i);@*/
    /*@drop_nth_index_of(owners, i);@*/
    /*@open xLIST_ITEM(x, nth(i, vals), ?xnext, xprev, nth(i, owners), l);@*/

/* The list item knows which list it is in.  Obtain the list from the list
 * item. */
#ifdef VERIFAST /*< const pointer declaration */
    List_t * pxList = pxItemToRemove->pxContainer;
#else
    List_t * const pxList = pxItemToRemove->pxContainer;
#endif

    pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
    pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

    /* Only used during decision coverage testing. */
    mtCOVERAGE_TEST_DELAY();

    /* Make sure the index is left pointing to a valid item. */
    if( pxList->pxIndex == pxItemToRemove )
    {
        pxList->pxIndex = pxItemToRemove->pxPrevious;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    pxItemToRemove->pxContainer = NULL;
    ( pxList->uxNumberOfItems )--;

    return pxList->uxNumberOfItems;

    /*@
    // Reassemble DLS1 and a modified DLS2, which no longer includes x
    if (end == xprev)
    {
        if (x == endprev)
        {
            //Case A
            close xLIST_ITEM(end, portMAX_DELAY, _, _, _, _);
            close DLS(end, end, end, end, singleton(end), singleton(portMAX_DELAY), singleton(head(owners)), l);
        }
        else
        {
            //Case B
            close xLIST_ITEM(xprev, _, xnext, endprev, head(owners), l);
            close DLS(end, endprev, xnext, xprev, singleton(end), singleton(portMAX_DELAY), singleton(head(owners)), l);
            close xLIST_ITEM(xnext, _, _, xprev, _, l);
            close DLS(xnext, xprev, end, endprev, tail(zs), tail(ws), tail(us), l);
            join(end, endprev, xnext, xprev, singleton(end), singleton(portMAX_DELAY),
                 xnext, xprev, end, endprev, tail(zs), tail(ws));
        }
    }
    else
    {
        if (x == endprev)
        {
            //Case C
            close xLIST_ITEM(end, _, ?endnext, xprev, head(ts), l);
            close xLIST_ITEM(xprev, ?xprev_val, end, _, ?xprev_owner, l);
            if (endnext == xprev)
            {
                close DLS(xprev, end, end, xprev, singleton(xprev), singleton(xprev_val), singleton(xprev_owner), l);
                close DLS(end, xprev, end, xprev, cons(end, singleton(xprev)), cons(portMAX_DELAY, singleton(xprev_val)), cons(head(ts), singleton(xprev_owner)), l);
            }
            else
            {
                close DLS(xprev, ?xprevprev, xnext, xprev, singleton(xprev), singleton(xprev_val), singleton(xprev_owner), l);
                assert DLS(endnext, end, xprev, xprevprev, ?cells_endnext_to_xprevprev, ?vals_endnext_to_xprevprev, _, l);
                join(endnext, end, xprev, xprevprev, cells_endnext_to_xprevprev, vals_endnext_to_xprevprev,
                     xprev, xprevprev, xnext, xprev, singleton(xprev), singleton(xprev_val));
                close DLS(end, xprev, end, xprev, ys, vs, ts, l);
            }
        }
        else
        {
            //Case D
            close xLIST_ITEM(xnext, _, ?xnextnext, xprev, ?xnext_owner, l);
            close DLS(xnext, xprev, end, endprev, tail(zs), tail(ws), tail(us), l);
            close xLIST_ITEM(end, _, ?endnext, endprev, head(ts), l);
            close xLIST_ITEM(xprev, ?xprev_val, xnext, _, ?xprev_owner, l);
            if (endnext == xprev)
            {
                close DLS(xprev, _, xnext, xprev, singleton(xprev), singleton(xprev_val), singleton(xprev_owner), l);
                close DLS(end, endprev, xnext, xprev, ys, vs, ts, l);
                join(end, endprev, xnext, xprev, ys, vs,
                     xnext, xprev, end, endprev, tail(zs), tail(ws));
            }
            else
            {
                close DLS(xprev, ?xprevprev, xnext, xprev, singleton(xprev), singleton(xprev_val), singleton(xprev_owner), l);
                assert DLS(endnext, end, xprev, xprevprev, ?cells_endnext_to_xprevprev, ?vals_endnext_to_xprevprev, _, l);
                join(endnext, end, xprev, xprevprev, cells_endnext_to_xprevprev, vals_endnext_to_xprevprev,
                     xprev, xprevprev, xnext, xprev, singleton(xprev), singleton(xprev_val));
                close DLS(end, endprev, xnext, xprev, ys, vs, ts, l);
                join(end, endprev, xnext, xprev, ys, vs,
                     xnext, xprev, end, endprev, tail(zs), tail(ws));
            }
        }
    }
    @*/
    /*@remove_remove_nth(cells, x);@*/
    /*@
    if (idx == x)
    {
        close xLIST(l, len-1, xprev, end, append(ys, tail(zs)), append(vs, tail(ws)), append(ts, tail(us)));
    }
    else
    {
        idx_remains_in_list(cells, idx, x, i);
        close xLIST(l, len-1, idx, end, append(ys, tail(zs)), append(vs, tail(ws)), append(ts, tail(us)));
    }
    @*/
    /*@close xLIST_ITEM(x, nth(i, vals), xnext, xprev, nth(i, owners), NULL);@*/
}
#else
    // Contract and proof written by Aalok Thakkar and Nathan Chong for the
    // single-core setup in 2020.

    /*@requires
        exists<struct xLIST * >(?l) &*&
        xLIST(l, ?len, ?idx, ?end, ?cells, ?vals) &*&
        end != pxItemToRemove &*&
        mem(pxItemToRemove, cells) == true;@*/
    /*@ensures
        result == len-1 &*&
        xLIST_ITEM(pxItemToRemove, nth(index_of(pxItemToRemove, cells), vals), _, ?pxItemToRemovePrevious, NULL) &*&
        pxItemToRemovePrevious == nth(index_of(pxItemToRemove, cells)-1, cells) &*&
        xLIST(l, result, idx == pxItemToRemove ? pxItemToRemovePrevious : idx,  end, remove(pxItemToRemove, cells), remove_nth(index_of(pxItemToRemove, cells), vals));@*/
    {
        /* For brevity we alias x to pxItemToRemove */
        /*@struct xLIST_ITEM *x = pxItemToRemove;@*/

        /* Start by establishing that the list must be non-empty since x != end */
        /*@open xLIST(l, len, idx, end, cells, vals);@*/
        /*@assert DLS(end, ?endprev, end, _, cells, vals, l);@*/
        /*@assert vals == cons(portMAX_DELAY, _);@*/
        /*@dls_not_empty(end, endprev, cells, x);@*/

        /* We know the xLIST is a DLS: end...endprev
        Split this into DLS1:end...xprev and DLS2:x...endprev */
        /*@int i = index_of(x, cells);@*/
        /*@split(end, endprev, end, endprev, cells, vals, x, i);@*/
        /*@list<struct xLIST_ITEM *> ys = take(i, cells);@*/
        /*@list<struct xLIST_ITEM *> zs = drop(i, cells);@*/
        /*@list<TickType_t> vs = take(i, vals);@*/
        /*@list<TickType_t> ws = drop(i, vals);@*/
        /*@assert length(ys) == length(vs);@*/
        /*@assert length(zs) == length(ws);@*/
        /*@assert DLS(end, endprev, x, ?xprev, ys, vs, l);@*/ /*< DLS1 (ys, vs) */
        /*@assert DLS(x, xprev, end, endprev, zs, ws, l);@*/  /*< DLS2 (zs, ws) */

        /* Now case split to open DLS1 and DLS2 appropriately */
        /*@
        if (end == xprev)
        {
            if (x == endprev)
            {
                //Case A
                //DLS1: extract end=prev=next
                open DLS(end, endprev, x, xprev, ys, vs, l);
                open xLIST_ITEM(end, portMAX_DELAY, x, endprev, l);
                //DLS2: extract x
                open DLS(x, xprev, end, endprev, zs, ws, l);
                //Lengths
                assert length(ys) == 1;
                assert length(zs) == 1;
            }
            else
            {
                //Case B
                //DLS1: extract end=prev
                open DLS(end, endprev, x, xprev, ys, vs, l);
                open xLIST_ITEM(end, portMAX_DELAY, x, endprev, l);
                //DLS2: extract next and x
                open DLS(x, end, end, endprev, zs, ws, l);
                assert DLS(?xnext, x, end, endprev, tail(zs), tail(ws), l);
                open DLS(xnext, x, end, endprev, tail(zs), tail(ws), l);
                open xLIST_ITEM(xnext, _, _, x, l);
                //Lengths
                assert length(ys) == 1;
            }
        }
        else
        {
            if (x == endprev)
            {
                //Case C
                //DLS1: extract end=next and prev
                dls_last_mem(end, endprev, x, xprev, ys);
                assert mem(xprev, ys) == true;
                open DLS(end, endprev, x, xprev, ys, vs, l);
                open xLIST_ITEM(end, portMAX_DELAY, ?endnext, endprev, l);
                if (endnext == xprev)
                {
                    open DLS(endnext, end, x, xprev, tail(ys), tail(vs), l);
                    open xLIST_ITEM(xprev, _, x, _, l);
                }
                else
                {
                    assert DLS(endnext, end, x, xprev, tail(ys), tail(vs), l);
                    int k = index_of(xprev, tail(ys));
                    dls_last_mem(endnext, end, x, xprev, tail(ys));
                    split(endnext, end, x, xprev, tail(ys), tail(vs), xprev, k);
                    open DLS(xprev, _, x, xprev, _, _, l);
                    open xLIST_ITEM(xprev, _, x, _, l);
                }
                //DLS2: extract x
                open DLS(x, xprev, end, endprev, zs, ws, l);
                //Lengths
                assert length(zs) == 1;
            }
            else
            {
                //Case D
                //DLS1: extract prev
                dls_last_mem(end, endprev, x, xprev, ys);
                int j = index_of(xprev, ys);
                open DLS(end, endprev, x, xprev, ys, vs, l);
                open xLIST_ITEM(end, portMAX_DELAY, ?endnext, endprev, l);
                if (endnext == xprev)
                {
                    open DLS(endnext, end, x, xprev, tail(ys), tail(vs), l);
                    assert tail(ys) == singleton(xprev);
                    open xLIST_ITEM(xprev, _, x, _, l);
                }
                else
                {
                    assert DLS(endnext, end, x, xprev, tail(ys), tail(vs), l);
                    int k = index_of(xprev, tail(ys));
                    dls_last_mem(endnext, end, x, xprev, tail(ys));
                    split(endnext, end, x, xprev, tail(ys), tail(vs), xprev, k);
                    open DLS(xprev, _, x, xprev, _, _, l);
                    open xLIST_ITEM(xprev, _, x, _, l);
                }
                //DLS2: extract next and x
                open DLS(x, xprev, end, endprev, zs, ws, l);
                assert xLIST_ITEM(x, _, ?xnext, _, l);
                open DLS(xnext, x, end, endprev, tail(zs), tail(ws), l);
                open xLIST_ITEM(xnext, _, _, x, l);
            }
        }
        @*/
        /*@drop_nth_index_of(vals, i);@*/
        /*@open xLIST_ITEM(x, nth(i, vals), ?xnext, xprev, l);@*/

    /* The list item knows which list it is in.  Obtain the list from the list
    * item. */
    #ifdef VERIFAST /*< const pointer declaration */
        List_t * pxList = pxItemToRemove->pxContainer;
    #else
        List_t * const pxList = pxItemToRemove->pxContainer;
    #endif

        pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
        pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

        /* Only used during decision coverage testing. */
        mtCOVERAGE_TEST_DELAY();

        /* Make sure the index is left pointing to a valid item. */
        if( pxList->pxIndex == pxItemToRemove )
        {
            pxList->pxIndex = pxItemToRemove->pxPrevious;
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        pxItemToRemove->pxContainer = NULL;
        ( pxList->uxNumberOfItems )--;

        return pxList->uxNumberOfItems;

        /*@
        // Reassemble DLS1 and a modified DLS2, which no longer includes x
        if (end == xprev)
        {
            if (x == endprev)
            {
                //Case A
                close xLIST_ITEM(end, portMAX_DELAY, _, _, _);
                close DLS(end, end, end, end, singleton(end), singleton(portMAX_DELAY), l);
            }
            else
            {
                //Case B
                close xLIST_ITEM(xprev, _, xnext, endprev, l);
                close DLS(end, endprev, xnext, xprev, singleton(end), singleton(portMAX_DELAY), l);
                close xLIST_ITEM(xnext, _, _, xprev, l);
                close DLS(xnext, xprev, end, endprev, tail(zs), tail(ws), l);
                join(end, endprev, xnext, xprev, singleton(end), singleton(portMAX_DELAY),
                    xnext, xprev, end, endprev, tail(zs), tail(ws));
            }
        }
        else
        {
            if (x == endprev)
            {
                //Case C
                close xLIST_ITEM(end, _, ?endnext, xprev, l);
                close xLIST_ITEM(xprev, ?xprev_val, end, _, l);
                if (endnext == xprev)
                {
                    close DLS(xprev, end, end, xprev, singleton(xprev), singleton(xprev_val), l);
                    close DLS(end, xprev, end, xprev, cons(end, singleton(xprev)), cons(portMAX_DELAY, singleton(xprev_val)), l);
                }
                else
                {
                    close DLS(xprev, ?xprevprev, xnext, xprev, singleton(xprev), singleton(xprev_val), l);
                    assert DLS(endnext, end, xprev, xprevprev, ?cells_endnext_to_xprevprev, ?vals_endnext_to_xprevprev, l);
                    join(endnext, end, xprev, xprevprev, cells_endnext_to_xprevprev, vals_endnext_to_xprevprev,
                        xprev, xprevprev, xnext, xprev, singleton(xprev), singleton(xprev_val));
                    close DLS(end, xprev, end, xprev, ys, vs, l);
                }
            }
            else
            {
                //Case D
                close xLIST_ITEM(xnext, _, ?xnextnext, xprev, l);
                close DLS(xnext, xprev, end, endprev, tail(zs), tail(ws), l);
                close xLIST_ITEM(end, _, ?endnext, endprev, l);
                close xLIST_ITEM(xprev, ?xprev_val, xnext, _, l);
                if (endnext == xprev)
                {
                    close DLS(xprev, _, xnext, xprev, singleton(xprev), singleton(xprev_val), l);
                    close DLS(end, endprev, xnext, xprev, ys, vs, l);
                    join(end, endprev, xnext, xprev, ys, vs,
                        xnext, xprev, end, endprev, tail(zs), tail(ws));
                }
                else
                {
                    close DLS(xprev, ?xprevprev, xnext, xprev, singleton(xprev), singleton(xprev_val), l);
                    assert DLS(endnext, end, xprev, xprevprev, ?cells_endnext_to_xprevprev, ?vals_endnext_to_xprevprev, l);
                    join(endnext, end, xprev, xprevprev, cells_endnext_to_xprevprev, vals_endnext_to_xprevprev,
                        xprev, xprevprev, xnext, xprev, singleton(xprev), singleton(xprev_val));
                    close DLS(end, endprev, xnext, xprev, ys, vs, l);
                    join(end, endprev, xnext, xprev, ys, vs,
                        xnext, xprev, end, endprev, tail(zs), tail(ws));
                }
            }
        }
        @*/
        /*@remove_remove_nth(cells, x);@*/
        /*@
        if (idx == x)
        {
            close xLIST(l, len-1, xprev, end, append(ys, tail(zs)), append(vs, tail(ws)));
        }
        else
        {
            idx_remains_in_list(cells, idx, x, i);
            close xLIST(l, len-1, idx, end, append(ys, tail(zs)), append(vs, tail(ws)));
        }
        @*/
        /*@close xLIST_ITEM(x, nth(i, vals), xnext, xprev, NULL);@*/
    }



#endif /* VERIFAST_SINGLE_CORE */

/*-----------------------------------------------------------*/
