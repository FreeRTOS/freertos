/*
 * The code below has been taken from:
 * pull request:
 * https://github.com/FreeRTOS/FreeRTOS/pull/836
 * file:
 * FreeRTOS/Test/VeriFast/include/proof/list.h
 *
 * The file has been renamed from `list.h` into 
 * `scp_list_predicates.h` to avoid naming conflicts.
 * The include guards have been updated accordingly.
 * 
 * All changes to the proofs, predicates, etc.
 * are guarded by a check that `VERIFAST_SINGLE_CORE` is 
 * NOT defined.
 */




/*
 * FreeRTOS V202112.00
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

/* *INDENT-OFF* */

#ifndef SCP_LIST_PREDICATES_H
#define SCP_LIST_PREDICATES_H

#ifndef VERIFAST_SINGLE_CORE
	/* Reasons for rewrite:
	 * - "common.gh" was converted into regular header "scp_common.h"
	 * - Using existing proof setup instead of definitions below.
	 */
	#include "scp_common.h"
#else
	#define VERIFAST
	#include <stdlib.h>
	#include <stdint.h>
	//@#include "common.gh"

	typedef size_t TickType_t;
	typedef size_t UBaseType_t;
	typedef ssize_t BaseType_t;

	#define pdTRUE 1
	#define pdFALSE 0

	/* Empty/no-op macros */
	#define mtCOVERAGE_TEST_MARKER()
	#define mtCOVERAGE_TEST_DELAY()
	#define listSET_LIST_INTEGRITY_CHECK_1_VALUE( pxList )
	#define listSET_LIST_INTEGRITY_CHECK_2_VALUE( pxList )
	#define listTEST_LIST_INTEGRITY( pxList )
	#define listTEST_LIST_ITEM_INTEGRITY( pxListItem )
	#define listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxListItem )
	#define listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxListItem )
#endif /* VERIFAST_SINGLE_CORE */

/* Max value stored in sentinel xListEnd element */
#ifndef VERIFAST_SINGLE_CORE
	/* Reason for rewrite: Match RP2040 port. */
	//VF_macro #define portMAX_DELAY 0xffffffffUL

	/* Verify that the preprocessor and our VeriFast proofs evaluate
	* `portMAX_DELAY` to the same values.
	*/
	void validate_portMAX_DELAY_value()
	//@ requires true;
	//@ ensures true;
	{
		//@ TickType_t gVal = portMAX_DELAY;
		TickType_t val = portMAX_DELAY;
		//@ assert(val == gVal);
	}
#else
	#define portMAX_DELAY UINT_MAX
#endif /* VERIFAST_SINGLE_CORE */


#ifdef VERIFAST_SINGLE_CORE
	/* Reason for deletion:
	 * structs already defined in FreeRTOS header "list.h"
	 */

	struct xLIST;

	struct xLIST_ITEM {
		TickType_t xItemValue;
		struct xLIST_ITEM * pxNext;
		struct xLIST_ITEM * pxPrevious;
		void * pvOwner;
		struct xLIST *pxContainer;
	};
	typedef struct xLIST_ITEM ListItem_t;

	typedef struct xLIST {
		UBaseType_t uxNumberOfItems;
		struct xLIST_ITEM *pxIndex;
	#ifdef VERIFAST /*< ***change MiniList_t to ListItem_t*** */
		struct xLIST_ITEM xListEnd;
	#else
		MiniListItem_t xListEnd;
	#endif
	} List_t;
#endif /* VERIFAST_SINGLE_CORE */




#ifndef VERIFAST_SINGLE_CORE
	/* Reasons for rewrite:
	 * - Breaking change in VeriFast. VeriFast now ensures that no uninitialised
	 *   values are read. `x |-> _` is interpreted as "uninitialised",
	 *   `x |-> ?v` is interpreted as "initialised".
	 * - In order to verify the scheduler, we have to reason about each node's
	 *   owner. Hence, the predicate has to expose it.
	 */
	/*@
	predicate xLIST_ITEM(
		struct xLIST_ITEM *n,
		TickType_t xItemValue,
		struct xLIST_ITEM *pxNext,
		struct xLIST_ITEM *pxPrevious,
		void* pxOwner,
		struct xLIST *pxContainer;) =
		n->xItemValue |-> xItemValue &*&
		n->pxNext |-> pxNext &*&
		n->pxPrevious |-> pxPrevious &*&
		n->pvOwner |-> pxOwner &*&
		n->pxContainer |-> pxContainer;
	@*/
#else
	/*@
	predicate xLIST_ITEM(
		struct xLIST_ITEM *n,
		TickType_t xItemValue,
		struct xLIST_ITEM *pxNext,
		struct xLIST_ITEM *pxPrevious,
		struct xLIST *pxContainer;) =
		n->xItemValue |-> xItemValue &*&
		n->pxNext |-> pxNext &*&
		n->pxPrevious |-> pxPrevious &*&
		n->pvOwner |-> _ &*&
		n->pxContainer |-> pxContainer;
	@*/
#endif /* VERIFAST_SINGLE_CORE */



#ifndef VERIFAST_SINGLE_CORE
	/* Reason for rewrite:
	 * In order to verify the scheduler, we have to reason about each node's
	 * owner. Hence, the predicate has to expose it.
	 */
	
	/* Ferreira et al. (STTT'14) doubly-linked list segment (DLS). */
	/*@
	predicate DLS(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *nprev,
		struct xLIST_ITEM *mnext,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells,
		list<TickType_t > vals,
		list<void*> owners,
		struct xLIST *pxContainer) =
		n == m
			? cells == cons(n, nil) &*&
				vals == cons(?v, nil) &*&
				owners == cons(?ow, nil) &*&
				xLIST_ITEM(n, v, mnext, nprev, ow, pxContainer)
			: cells == cons(n, ?cells0) &*&
				vals == cons(?v, ?vals0) &*&
				owners == cons(?ow, ?owners0) &*&
				xLIST_ITEM(n, v, ?o, nprev, ow, pxContainer) &*& DLS(o, n, mnext, m, cells0, vals0, owners0, pxContainer);
	@*/
#else
	/* Ferreira et al. (STTT'14) doubly-linked list segment (DLS). */
	/*@
	predicate DLS(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *nprev,
		struct xLIST_ITEM *mnext,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells,
		list<TickType_t > vals,
		struct xLIST *pxContainer) =
		n == m
			? cells == cons(n, nil) &*&
				vals == cons(?v, nil) &*&
				xLIST_ITEM(n, v, mnext, nprev, pxContainer)
			: cells == cons(n, ?cells0) &*&
				vals == cons(?v, ?vals0) &*&
				xLIST_ITEM(n, v, ?o, nprev, pxContainer) &*& DLS(o, n, mnext, m, cells0, vals0, pxContainer);
	@*/
#endif /* VERIFAST_SINGLE_CORE */


#ifndef VERIFAST_SINGLE_CORE
	/* Reason for rewrite:
	 * Predicates `xLIST_ITEM` and `DLS` have been extended to expose node
	 * owners. Proofs using these predicates must be adapted as well.
	 */

	/*@
	lemma void dls_star_item(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *m,
		struct xLIST_ITEM *o)
	requires DLS(n, ?nprev, ?mnext, m, ?cells, ?vals, ?owners, ?l) &*& xLIST_ITEM(o, ?v, ?onext, ?oprev, ?ow, ?l2);
	ensures DLS(n, nprev, mnext, m, cells, vals, owners, l) &*& xLIST_ITEM(o, v, onext, oprev, ow, l2) &*& mem(o, cells) == false;
	{
		open DLS(n, nprev, mnext, m, cells, vals, owners, l);
		if (n == m) {
			assert xLIST_ITEM(n, _, _, _, _, _);
			open xLIST_ITEM(n, _, _, _, _, _);
			open xLIST_ITEM(o, _, _, _, _, _);
			assert n != o;
			close xLIST_ITEM(o, _, _, _, _, _);
			close xLIST_ITEM(n, _, _, _, _, _);
			close DLS(n, nprev, mnext, m, cells, vals, owners, l);
		}
		else {
			assert DLS(?nnext, n, mnext, m, tail(cells), tail(vals), tail(owners), l);
			dls_star_item(nnext, m, o);
			open xLIST_ITEM(n, _, _, _, _, _);
			open xLIST_ITEM(o, _, _, _, _, _);
			assert n != o;
			close xLIST_ITEM(o, _, _, _, _, _);
			close xLIST_ITEM(n, _, _, _, _, _);
			close DLS(n, nprev, mnext, m, cells, vals, owners, l);
		}
	}


	lemma void dls_distinct(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *nprev,
		struct xLIST_ITEM *mnext,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells)
	requires DLS(n, nprev, mnext, m, cells, ?vals, ?owners, ?l);
	ensures DLS(n, nprev, mnext, m, cells, vals, owners, l) &*& distinct(cells) == true;
	{
		if (n == m) {
			open DLS(n, nprev, mnext, m, cells, vals, owners, l);
			close DLS(n, nprev, mnext, m, cells, vals, owners, l);
		} else {
			open DLS(n, nprev, mnext, m, cells, vals, owners, l);
			assert DLS(?nnext, n, mnext, m, tail(cells), tail(vals), tail(owners), l);
			dls_distinct(nnext, n, mnext, m, tail(cells));
			dls_star_item(nnext, m, n);
			close DLS(n, nprev, mnext, m, cells, vals, owners, l);
		}
	}
	@*/
#else 
	/*@
	lemma void dls_star_item(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *m,
		struct xLIST_ITEM *o)
	requires DLS(n, ?nprev, ?mnext, m, ?cells, ?vals, ?l) &*& xLIST_ITEM(o, ?v, ?onext, ?oprev, ?l2);
	ensures DLS(n, nprev, mnext, m, cells, vals, l) &*& xLIST_ITEM(o, v, onext, oprev, l2) &*& mem(o, cells) == false;
	{
		open DLS(n, nprev, mnext, m, cells, vals, l);
		if (n == m) {
			assert xLIST_ITEM(n, _, _, _, _);
			open xLIST_ITEM(n, _, _, _, _);
			open xLIST_ITEM(o, _, _, _, _);
			assert n != o;
			close xLIST_ITEM(o, _, _, _, _);
			close xLIST_ITEM(n, _, _, _, _);
			close DLS(n, nprev, mnext, m, cells, vals, l);
		}
		else {
			assert DLS(?nnext, n, mnext, m, tail(cells), tail(vals), l);
			dls_star_item(nnext, m, o);
			open xLIST_ITEM(n, _, _, _, _);
			open xLIST_ITEM(o, _, _, _, _);
			assert n != o;
			close xLIST_ITEM(o, _, _, _, _);
			close xLIST_ITEM(n, _, _, _, _);
			close DLS(n, nprev, mnext, m, cells, vals, l);
		}
	}


	lemma void dls_distinct(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *nprev,
		struct xLIST_ITEM *mnext,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells)
	requires DLS(n, nprev, mnext, m, cells, ?vals, ?l);
	ensures DLS(n, nprev, mnext, m, cells, vals, l) &*& distinct(cells) == true;
	{
		if (n == m) {
			open DLS(n, nprev, mnext, m, cells, vals, l);
			close DLS(n, nprev, mnext, m, cells, vals, l);
		} else {
			open DLS(n, nprev, mnext, m, cells, vals, l);
			assert DLS(?nnext, n, mnext, m, tail(cells), tail(vals), l);
			dls_distinct(nnext, n, mnext, m, tail(cells));
			dls_star_item(nnext, m, n);
			close DLS(n, nprev, mnext, m, cells, vals, l);
		}
	}
	@*/
#endif /* VERIFAST_SINGLE_CORE */

#ifndef VERIFAST_SINGLE_CORE
	/* Reason for rewrite:
	 * In order to verify the scheduler, we have to reason about each node's
	 * owner. Hence, the predicate has to expose it.
	 */

	/*@
	predicate xLIST(
		struct xLIST *l,
		int uxNumberOfItems,
		struct xLIST_ITEM *pxIndex,
		struct xLIST_ITEM *xListEnd,
		list<struct xLIST_ITEM *>cells,
		list<TickType_t >vals,
		list<void*> owners) =
		l->uxNumberOfItems |-> uxNumberOfItems &*&
		l->pxIndex |-> pxIndex &*&
		mem(pxIndex, cells) == true &*&
		xListEnd == &(l->xListEnd) &*&
		xListEnd == head(cells) &*&
		portMAX_DELAY == head(vals) &*&
		struct_xLIST_ITEM_padding(&l->xListEnd) &*&
		length(cells) == length(vals) &*&
		length(owners) == length(cells) &*&
		uxNumberOfItems + 1 == length(cells) &*&
		DLS(xListEnd, ?endprev, xListEnd, endprev, cells, vals, owners, l);
	@*/
#else 
	/*@
	predicate xLIST(
		struct xLIST *l,
		int uxNumberOfItems,
		struct xLIST_ITEM *pxIndex,
		struct xLIST_ITEM *xListEnd,
		list<struct xLIST_ITEM *>cells,
		list<TickType_t >vals) =
		l->uxNumberOfItems |-> uxNumberOfItems &*&
		l->pxIndex |-> pxIndex &*&
		mem(pxIndex, cells) == true &*&
		xListEnd == &(l->xListEnd) &*&
		xListEnd == head(cells) &*&
		portMAX_DELAY == head(vals) &*&
		struct_xLIST_ITEM_padding(&l->xListEnd) &*&
		length(cells) == length(vals) &*&
		uxNumberOfItems + 1 == length(cells) &*&
		DLS(xListEnd, ?endprev, xListEnd, endprev, cells, vals, l);
	@*/
#endif /* VERIFAST_SINGLE_CORE */



#ifndef VERIFAST_SINGLE_CORE
	/* Reason for rewrite:
	 * Predicates `xLIST_ITEM`, `DLS` and `xLIST` have been extended to expose
	 * node owners. Proofs using these predicates must be adapted as well.
	 */


	/*@
	lemma void xLIST_distinct_cells(struct xLIST *l)
	requires xLIST(l, ?n, ?idx, ?end, ?cells, ?vals, ?owners);
	ensures xLIST(l, n, idx, end, cells, vals, owners) &*& distinct(cells) == true;
	{
		open xLIST(l, n, idx, end, cells, vals, owners);
		assert DLS(end, ?endprev, end, _, cells, vals, owners, l);
		dls_distinct(end, endprev, end, endprev, cells);
		close xLIST(l, n, idx, end, cells, vals, owners);
	}

	lemma void xLIST_star_item(struct xLIST *l, struct xLIST_ITEM *x)
	requires xLIST(l, ?n, ?idx, ?end, ?cells, ?vals, ?owners) &*& xLIST_ITEM(x, ?v, ?xnext, ?xprev, ?ow, ?l2);
	ensures xLIST(l, n, idx, end, cells, vals, owners) &*& xLIST_ITEM(x, v, xnext, xprev, ow, l2) &*& mem(x, cells) == false;
	{
		open xLIST(l, n, idx, end, cells, vals, owners);
		assert DLS(end, ?endprev, end, _, cells, vals, owners, l);
		dls_distinct(end, endprev, end, endprev, cells);
		dls_star_item(end, endprev, x);
		close xLIST(l, n, idx, end, cells, vals, owners);
	}

	lemma void dls_first_mem(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *nprev,
		struct xLIST_ITEM *mnext,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells)
	requires DLS(n, nprev, mnext, m, cells, ?vals, ?owners, ?l);
	ensures DLS(n, nprev, mnext, m, cells, vals, owners, l) &*& mem(n, cells) == true &*& index_of(n, cells) == 0;
	{
		open DLS(n, nprev, mnext, m, cells, vals, owners, l);
		if (n == m) {
			assert cells == cons(n, nil);
			close DLS(n, nprev, mnext, m, cells, vals, owners, l);
		} else {
			assert cells == cons(n, ?tail);
			close DLS(n, nprev, mnext, m, cells, vals, owners, l);
		}
	}

	lemma void dls_not_empty(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells,
		struct xLIST_ITEM *x)
	requires DLS(n, m, n, m, cells, ?vals, ?owners, ?l) &*& mem(x, cells) == true &*& x != n;
	ensures DLS(n, m, n, m, cells, vals, owners, l) &*& n != m;
	{
		open DLS(n, m, n, m, cells, vals, owners, l);
		close DLS(n, m, n, m, cells, vals, owners, l);
	}

	lemma void dls_last_mem(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *nprev,
		struct xLIST_ITEM *mnext,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells)
	requires DLS(n, nprev, mnext, m, cells, ?vals, ?owners, ?l);
	ensures DLS(n, nprev, mnext, m, cells, vals, owners, l) &*& mem(m, cells) == true &*& index_of(m, cells) == length(cells) - 1;
	{
		open DLS(n, nprev, mnext, m, cells, vals, owners, l);
		if (n == m) {
			// trivial
		} else {
			open xLIST_ITEM(n, _, ?nnext, _, _, l);
			assert DLS(?o, n, mnext, m, tail(cells), tail(vals), tail(owners), l);
			dls_last_mem(o, n, mnext, m, tail(cells));
			close xLIST_ITEM(n, _, nnext, _, _, l);
		}
		close DLS(n, nprev, mnext, m, cells, vals, owners, l);
	}


	lemma void split(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *nprev,
		struct xLIST_ITEM *mnext,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells,
		list<TickType_t > vals,
		struct xLIST_ITEM *x,
		int i)
	requires DLS(n, nprev, mnext, m, cells, vals, ?owners, ?l) &*& x != n &*& mem(x, cells) == true &*& index_of(x,cells) == i;
	ensures DLS(n, nprev, x, ?xprev, take(i, cells), take(i, vals), take(i, owners), l) &*& DLS(x, xprev, mnext, m, drop(i, cells), drop(i, vals), drop(i, owners), l) &*& xprev == nth(i-1, cells);
	{
		open DLS(n, nprev, mnext, m, cells, vals, owners, l);
		assert n != m;
		assert xLIST_ITEM(n, ?v, ?nnext, _, ?ow, _);
		assert DLS(nnext, n, mnext, m, tail(cells), tail(vals), tail(owners), l);
		if (nnext == x) {
			close DLS(n, nprev, x, n, singleton(n), singleton(v), singleton(ow), l);
			open DLS(x, n, mnext, m, tail(cells), tail(vals), tail(owners), l);
			open xLIST_ITEM(x, _, ?xnext, ?xprev, ?xow, l);
			close xLIST_ITEM(x, _, xnext, xprev, xow, l);
			close DLS(x, n, mnext, m, tail(cells), tail(vals), tail(owners), l);
		} else {
			assert nnext != x;
			split(nnext, n, mnext, m, tail(cells), tail(vals), x, i - 1);
			assert DLS(nnext, n, x, ?xprev, take(i-1, tail(cells)), take(i-1, tail(vals)), take(i-1, tail(owners)), l);
			dls_distinct(nnext, n, x, xprev, take(i-1, tail(cells)));
			dls_star_item(nnext, xprev, n);
			dls_last_mem(nnext, n, x, xprev, take(i-1, tail(cells)));
			assert n != xprev;
			close DLS(n, nprev, x, xprev, take(i, cells), take(i, vals), take(i, owners), l);
		}
	}

	lemma void join(
		struct xLIST_ITEM *n1,
		struct xLIST_ITEM *nprev1,
		struct xLIST_ITEM *mnext1,
		struct xLIST_ITEM *m1,
		list<struct xLIST_ITEM * > cells1,
		list<TickType_t > vals1,
		struct xLIST_ITEM *n2,
		struct xLIST_ITEM *nprev2,
		struct xLIST_ITEM *mnext2,
		struct xLIST_ITEM *m2,
		list<struct xLIST_ITEM * > cells2,
		list<TickType_t > vals2)
	requires
		DLS(n1, nprev1, mnext1, m1, cells1, vals1, ?owners1, ?l) &*&
		DLS(n2, nprev2, mnext2, m2, cells2, vals2, ?owners2, l) &*&
		mnext1 == n2 &*& m1 == nprev2;
	ensures DLS(n1, nprev1, mnext2, m2, append(cells1, cells2), append(vals1, vals2), append(owners1, owners2), l);
	{
		if (n1 == m1) {
			dls_first_mem(n1, nprev1, mnext1, m1, cells1);
			dls_last_mem(n2, nprev2, mnext2, m2, cells2);
			open DLS(n1, nprev1, mnext1, m1, cells1, vals1, owners1, l);
			dls_star_item(n2, m2, n1);
			close DLS(n1, nprev1, mnext2, m2, append(singleton(n1), cells2), append(vals1, vals2), append(owners1, owners2) ,l);
		} else {
			open DLS(n1, nprev1, mnext1, m1, cells1, vals1, owners1, l);
			assert DLS(?o, n1, mnext1, m1, ?cells1_tail, ?vals1_tail, ?owners1_tail, l);
			join(o, n1, mnext1, m1, cells1_tail, vals1_tail,
				n2, nprev2, mnext2, m2, cells2, vals2);
			assert DLS(o, n1, mnext2, m2, append(cells1_tail, cells2), append(vals1_tail, vals2), append(owners1_tail, owners2), l);
			dls_last_mem(o, n1, mnext2, m2, append(cells1_tail, cells2));
			dls_star_item(o, m2, n1);
			close DLS(n1, nprev1, mnext2, m2, append(cells1, cells2), append(vals1, vals2), append(owners1, owners2), l);
		}
	}
	@*/
#else 
	/*@
	lemma void xLIST_distinct_cells(struct xLIST *l)
	requires xLIST(l, ?n, ?idx, ?end, ?cells, ?vals);
	ensures xLIST(l, n, idx, end, cells, vals) &*& distinct(cells) == true;
	{
		open xLIST(l, n, idx, end, cells, vals);
		assert DLS(end, ?endprev, end, _, cells, vals, l);
		dls_distinct(end, endprev, end, endprev, cells);
		close xLIST(l, n, idx, end, cells, vals);
	}

	lemma void xLIST_star_item(struct xLIST *l, struct xLIST_ITEM *x)
	requires xLIST(l, ?n, ?idx, ?end, ?cells, ?vals) &*& xLIST_ITEM(x, ?v, ?xnext, ?xprev, ?l2);
	ensures xLIST(l, n, idx, end, cells, vals) &*& xLIST_ITEM(x, v, xnext, xprev, l2) &*& mem(x, cells) == false;
	{
		open xLIST(l, n, idx, end, cells, vals);
		assert DLS(end, ?endprev, end, _, cells, vals, l);
		dls_distinct(end, endprev, end, endprev, cells);
		dls_star_item(end, endprev, x);
		close xLIST(l, n, idx, end, cells, vals);
	}

	lemma void dls_first_mem(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *nprev,
		struct xLIST_ITEM *mnext,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells)
	requires DLS(n, nprev, mnext, m, cells, ?vals, ?l);
	ensures DLS(n, nprev, mnext, m, cells, vals, l) &*& mem(n, cells) == true &*& index_of(n, cells) == 0;
	{
		open DLS(n, nprev, mnext, m, cells, vals, l);
		if (n == m) {
			assert cells == cons(n, nil);
			close DLS(n, nprev, mnext, m, cells, vals, l);
		} else {
			assert cells == cons(n, ?tail);
			close DLS(n, nprev, mnext, m, cells, vals, l);
		}
	}

	lemma void dls_not_empty(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells,
		struct xLIST_ITEM *x)
	requires DLS(n, m, n, m, cells, ?vals, ?l) &*& mem(x, cells) == true &*& x != n;
	ensures DLS(n, m, n, m, cells, vals, l) &*& n != m;
	{
		open DLS(n, m, n, m, cells, vals, l);
		close DLS(n, m, n, m, cells, vals, l);
	}

	lemma void dls_last_mem(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *nprev,
		struct xLIST_ITEM *mnext,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells)
	requires DLS(n, nprev, mnext, m, cells, ?vals, ?l);
	ensures DLS(n, nprev, mnext, m, cells, vals, l) &*& mem(m, cells) == true &*& index_of(m, cells) == length(cells) - 1;
	{
		open DLS(n, nprev, mnext, m, cells, vals, l);
		if (n == m) {
			// trivial
		} else {
			open xLIST_ITEM(n, _, ?nnext, _, l);
			assert DLS(?o, n, mnext, m, tail(cells), tail(vals), l);
			dls_last_mem(o, n, mnext, m, tail(cells));
			close xLIST_ITEM(n, _, nnext, _, l);
		}
		close DLS(n, nprev, mnext, m, cells, vals, l);
	}


	lemma void split(
		struct xLIST_ITEM *n,
		struct xLIST_ITEM *nprev,
		struct xLIST_ITEM *mnext,
		struct xLIST_ITEM *m,
		list<struct xLIST_ITEM * > cells,
		list<TickType_t > vals,
		struct xLIST_ITEM *x,
		int i)
	requires DLS(n, nprev, mnext, m, cells, vals, ?l) &*& x != n &*& mem(x, cells) == true &*& index_of(x,cells) == i;
	ensures DLS(n, nprev, x, ?xprev, take(i, cells), take(i, vals), l) &*& DLS(x, xprev, mnext, m, drop(i, cells), drop(i, vals), l) &*& xprev == nth(i-1, cells);
	{
		open DLS(n, nprev, mnext, m, cells, vals, l);
		assert n != m;
		assert xLIST_ITEM(n, ?v, ?nnext, _, _);
		assert DLS(nnext, n, mnext, m, tail(cells), tail(vals), l);
		if (nnext == x) {
			close DLS(n, nprev, x, n, singleton(n), singleton(v), l);
			open DLS(x, n, mnext, m, tail(cells), tail(vals), l);
			open xLIST_ITEM(x, _, ?xnext, ?xprev, l);
			close xLIST_ITEM(x, _, xnext, xprev, l);
			close DLS(x, n, mnext, m, tail(cells), tail(vals), l);
		} else {
			assert nnext != x;
			split(nnext, n, mnext, m, tail(cells), tail(vals), x, i - 1);
			assert DLS(nnext, n, x, ?xprev, take(i-1, tail(cells)), take(i-1, tail(vals)), l);
			dls_distinct(nnext, n, x, xprev, take(i-1, tail(cells)));
			dls_star_item(nnext, xprev, n);
			dls_last_mem(nnext, n, x, xprev, take(i-1, tail(cells)));
			assert n != xprev;
			close DLS(n, nprev, x, xprev, take(i, cells), take(i, vals), l);
		}
	}

	lemma void join(
		struct xLIST_ITEM *n1,
		struct xLIST_ITEM *nprev1,
		struct xLIST_ITEM *mnext1,
		struct xLIST_ITEM *m1,
		list<struct xLIST_ITEM * > cells1,
		list<TickType_t > vals1,
		struct xLIST_ITEM *n2,
		struct xLIST_ITEM *nprev2,
		struct xLIST_ITEM *mnext2,
		struct xLIST_ITEM *m2,
		list<struct xLIST_ITEM * > cells2,
		list<TickType_t > vals2)
	requires
		DLS(n1, nprev1, mnext1, m1, cells1, vals1, ?l) &*&
		DLS(n2, nprev2, mnext2, m2, cells2, vals2, l) &*&
		mnext1 == n2 &*& m1 == nprev2;
	ensures DLS(n1, nprev1, mnext2, m2, append(cells1, cells2), append(vals1, vals2), l);
	{
		if (n1 == m1) {
			dls_first_mem(n1, nprev1, mnext1, m1, cells1);
			dls_last_mem(n2, nprev2, mnext2, m2, cells2);
			open DLS(n1, nprev1, mnext1, m1, cells1, vals1, l);
			dls_star_item(n2, m2, n1);
			close DLS(n1, nprev1, mnext2, m2, append(singleton(n1), cells2), append(vals1, vals2), l);
		} else {
			open DLS(n1, nprev1, mnext1, m1, cells1, vals1, l);
			assert DLS(?o, n1, mnext1, m1, ?cells1_tail, ?vals1_tail, l);
			join(o, n1, mnext1, m1, cells1_tail, vals1_tail,
				n2, nprev2, mnext2, m2, cells2, vals2);
			assert DLS(o, n1, mnext2, m2, append(cells1_tail, cells2), append(vals1_tail, vals2), l);
			dls_last_mem(o, n1, mnext2, m2, append(cells1_tail, cells2));
			dls_star_item(o, m2, n1);
			close DLS(n1, nprev1, mnext2, m2, append(cells1, cells2), append(vals1, vals2), l);
		}
	}
	@*/
#endif /* VERIFAST_SINGLE_CORE */

/*@
lemma void idx_remains_in_list<t>(
	list<t> cells,
	t idx,
	t x,
	int ix)
requires
	idx != x &*&
	mem(idx, cells) == true &*&
	mem(x, cells) == true &*&
	index_of(x, cells) == ix;
ensures mem(idx, remove_nth(ix, cells)) == true;
{
	neq_mem_remove(idx, x, cells);
	remove_remove_nth(cells, x);
}
@*/

// Following lemma from `verifast/examples/shared_boxes/concurrentqueue.c`.
// Used in the uxListRemove proof to show that the item to remove `x` must
// have value `nth(i, vals)` where `i == index_of(x, cells)`. 
/*@
lemma void drop_nth_index_of<t>(list<t> vs, int i)
requires
	0 <= i && i < length(vs);
ensures
	head(drop(i , vs)) == nth(i, vs);
{
	switch(vs) {
		case nil:
		case cons(h, t):
		if (i == 0) {
			// trivial
		} else {
			drop_nth_index_of(t, i - 1);
		}
	}
}
@*/

/*@
lemma void remove_append<t>(t x, list<t> l1, list<t> l2)
	requires mem(x, l1) == false;
	ensures remove(x, append(l1, l2)) == append(l1, remove(x, l2));
{
	switch(l1) {
		case nil:
		case cons(h1, t1):
			remove_append(x, t1, l2);
	}
}
@*/


#endif /* SCP_LIST_PREDICATES_H */

/* *INDENT-ON* */
