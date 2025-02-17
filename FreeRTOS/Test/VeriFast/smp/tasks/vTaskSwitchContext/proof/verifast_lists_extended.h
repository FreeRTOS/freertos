#ifndef VERIFAST_LISTS_EXTENDED_H
#define VERIFAST_LISTS_EXTENDED_H

/* This file contains lemmas that would fit `list.gh` which is part
 * of VeriFast's standard library.
 */

// Most of the following lemmas are axioms.


/*@
lemma void head_drop_n_equals_nths<t>(list<t> xs, int n);
requires n >= 0;
ensures head(drop(n, xs)) == nth(n, xs);

lemma void drop_index_equals_singleton_implies_last_element<t>(list<t> xs, t x);
requires drop(index_of(x, xs), xs) == cons(x, nil);
ensures index_of(x, xs) == length(xs) - 1;

lemma void nth_index<t>(list<t> xs, t x);
requires mem(x, xs) == true;
ensures nth(index_of(x, xs), xs) == x;

lemma void mem_prefix_implies_mem<t>(t x, list<t> xs, int n);
requires mem(x, take(n, xs)) == true;
ensures mem(x, xs) == true;

lemma void mem_suffix_implies_mem<t>(t x, list<t> xs, int n);
requires mem(x, drop(n, xs)) == true;
ensures mem(x, xs) == true;

lemma void drop_n_plus_m<t>(list<t> xs, int n, int m);
requires true;
ensures drop(n, drop(m, xs)) == drop(n + m, xs);


fixpoint bool superset<t>(list<t> super, list<t> sub) {
    return subset(sub, super);
}


lemma void update_out_of_bounds<t>(int index, t x, list<t> xs)
requires (index < 0 || index >= length(xs));
ensures update(index, x, xs) == xs;
{
    switch(xs) {
        case nil:   // nothing to do
        case cons(h, rest): {
            update_out_of_bounds(index-1, x, rest);
        }
    }
}

lemma void index_of_different<t>(t x1, t x2, list<t> xs)
requires x1 != x2 &*& mem(x1, xs) == true &*& mem(x2, xs) == true;
ensures index_of(x1, xs) != index_of(x2, xs);
{
    switch(xs) {
        case nil: 
        case cons(h, rest):
            if(h != x1 && h != x2) {
                index_of_different(x1, x2, rest);
            }
    }
}

lemma void remove_result_subset<t>(t x, list<t> xs);
requires true;
ensures subset(remove(x, xs), xs) == true;

lemma void append_take_nth_drop<t>(int n, list<t> xs);
requires 0 <= n &*& n < length(xs);
ensures xs == append( take(n, xs), cons(nth(n, xs), drop(n+1, xs)) );

// Note: `listex.gh` contains lemma `forall_drop` but no corresponding 
//       `forall_take`.
lemma void forall_take<t>(list<t> xs, fixpoint(t, bool) p, int i);
    requires forall(xs, p) == true;
    ensures forall(take(i, xs), p) == true;

lemma void forall_mem_implies_superset<t>(list<t> super, list<t> sub);
requires forall(sub, (mem_list_elem)(super)) == true;
ensures superset(super, sub) == true;

lemma void subset_implies_forall_mem<t>(list<t> sub, list<t> super);
requires subset(sub, super) == true;
ensures forall(sub, (mem_list_elem)(super)) == true;

lemma void forall_remove<t>(t x, list<t> xs, fixpoint(t, bool) p);
requires forall(xs, p) == true;
ensures forall(remove(x, xs), p) == true;

lemma void forall_remove_nth<t>(int n, list<t> xs, fixpoint(t, bool) p);
requires forall(xs, p) == true;
ensures forall(remove_nth(n, xs), p) == true;

lemma void nth_implies_mem<t>(int n, list<t> xs);
requires 0 <= n &*& n < length(xs);
ensures mem(nth(n, xs), xs) == true;

lemma void subset_append<t>(list<t> sub1, list<t> sub2, list<t> super);
requires subset(sub1, super) == true &*& subset(sub2, super) == true;
ensures subset(append(sub1, sub2), super) == true;

lemma void subset_take<t>(int i, list<t> xs);
requires true;
ensures subset(take(i, xs), xs) == true;

lemma void subset_drop<t>(int i, list<t> xs);
requires true;
ensures subset(drop(i, xs), xs) == true;
@*/



#endif /* VERIFAST_LISTS_EXTENDED_H */