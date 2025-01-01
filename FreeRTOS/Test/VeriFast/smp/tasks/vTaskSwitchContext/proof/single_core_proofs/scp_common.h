/*
 * The code below has been taken from:
 * pull request:
 * https://github.com/FreeRTOS/FreeRTOS/pull/836
 * file:
 * FreeRTOS/Test/VeriFast/include/proof/list.h
 *
 * The file has been converted from a ghost header
 * into a regular header.
 * It has also been renamed from `common.h` into 
 * `scp_common.h`.
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

#ifndef SCP_COMMON_H
#define SCP_COMMON_H

/*@
#include <listex.gh>

fixpoint list<t> rotate_left<t>(int n, list<t> xs) {
  return append(drop(n, xs), take(n, xs));
}

fixpoint list<t> singleton<t>(t x) {
  return cons(x, nil);
}

lemma void note(bool b)
  requires b;
  ensures b;
{}

lemma_auto void rotate_length<t>(int n, list<t> xs)
  requires 0 <= n && n <= length(xs);
  ensures length(rotate_left(n, xs)) == length(xs);
{}

lemma void take_length_eq<t>(int k, list<t> xs, list<t> ys)
  requires 0 <= k && k <= length(xs) && take(k, xs) == ys;
  ensures length(ys) == k;
{}

lemma void leq_bound(int x, int b)
  requires b <= x && x <= b;
  ensures x == b;
{}

lemma void mul_mono_l_strict(int x, int y, int n)
    requires 0 < n &*& x < y;
    ensures x * n < y * n;
{
    for (int i = 1; i < n; i++)
        invariant i <= n &*& x * i < y * i;
        decreases n - i;
    {}
}

lemma void div_leq(int x, int y, int n)
  requires 0 < n && x * n <= y * n;
  ensures x <= y;
{
  assert x * n <= y * n;
  if (x <= y) {
    mul_mono_l(x,y,n);
  } else {
    mul_mono_l_strict(y,x,n); //< contradiction
  }
}

lemma void div_lt(int x, int y, int n)
  requires 0 < n && x * n < y * n;
  ensures x < y;
{
  assert x * n <= y * n;
  if (x == y) {
  } else if (x <= y) {
    mul_mono_l(x,y,n);
  } else {
    assert y < x;
    mul_mono_l(y,x,n); //< contradiction
  }
}

lemma_auto void mod_same(int n)
  requires 0 < n;
  ensures n % n == 0;
{
  div_rem_nonneg(n, n);
  if (n / n < 1) {} else if (n / n > 1) {
    mul_mono_l(2, n/n, n);
  } else {}
}

lemma void mod_lt(int x, int n)
  requires 0 <= x && x < n;
  ensures x % n == x;
{
  div_rem_nonneg(x, n);
  if (x / n > 0) {
    mul_mono_l(1, x / n, n);
  } else {
  }
}

lemma void mod_plus_one(int x, int y, int n)
  requires 0 <= y && 0 < n && x == (y % n);
  ensures ((x+1) % n) == ((y+1) % n);
{
  div_rem_nonneg(y, n);
  div_rem_nonneg(y+1, n);
  div_rem_nonneg(y%n+1, n);
  if (y%n+1 < n) {
    mod_lt(y%n+1, n);
    assert y%n == y - y/n*n;
    assert (y+1)%n == y + 1 - (y + 1)/n*n;
    if ((y+1)/n > y/n) {
      mul_mono_l(y/n + 1, (y+1)/n, n);
    } else if ((y+1)/n < y/n) {
      mul_mono_l((y+1)/n + 1, y/n, n);
    }
    assert y - (y+1)/n*n == y - y/n*n;
    assert y+1 - (y+1)/n*n == y - y/n*n + 1;
    assert (y+1)%n == y%n + 1;
  } else {
    assert y%n+1 == n;
    assert (y%n+1)%n == 0;
    if (y/n + 1 < (y+1)/n) {
      mul_mono_l(y/n + 2, (y+1)/n, n);
    } else if (y/n + 1 > (y+1)/n) {
      mul_mono_l((y+1)/n, y/n, n);
    }
    assert (y+1)/n == y/n + 1;
    note((y+1)/n*n == (y/n + 1)*n);
    assert (y+1)%n == 0;
  }
  assert (y%n+1)%n == (y+1)%n;
}

lemma void mod_mul(int x, int n, int y)
  requires 0 < n && 0 <= x && 0 <= y;
  ensures (x*n + y)%n == y%n;
{
  mul_mono_l(0, x, n);
  div_rem_nonneg(x*n+y, n);
  div_rem_nonneg(y, n);

  if ((x*n+y)/n > x + y/n) {
    mul_mono_l(x + y/n + 1, (x*n+y)/n, n);
  } else if ((x*n+y)/n < x + y/n) {
    mul_mono_l((x*n+y)/n + 1, x + y/n, n);
  }
  note((x*n + y)/n == x + y/n);
  note((x*n + y)/n*n == (x + y/n)*n);
}

lemma void mod_plus_distr(int x, int y, int n)
  requires 0 < n && 0 <= x && 0 <= y;
  ensures ((x % n) + y) % n == (x + y) % n;
{
  div_rem_nonneg(x, n);
  div_rem_nonneg(x%n+y, n);
  div_rem_nonneg(x+y, n);

  assert x == x/n*n + x%n;
  mod_mul(x/n, n, x%n + y);
}

lemma_auto void mod_mod(int x, int n)
  requires 0 < n && 0 <= x;
  ensures (x % n) % n == (x % n);
{
  mod_plus_distr(x, 0, n);
}

lemma void mod_plus(int x, int y, int n);
  requires 0 < n && 0 <= x && 0 <= y;
  ensures (x + y) % n == ((x % n) + (y % n)) % n;

lemma_auto void mod_range(int x, int n)
  requires 0 <= x && 0 < n;
  ensures 0 <= (x % n) && (x % n) < n;
{
  div_rem_nonneg(x, n);
}

lemma void head_append<t>(list<t> xs, list<t> ys)
  requires 0 < length(xs);
  ensures head(append(xs, ys)) == head(xs);
{
  switch(xs)
  {
    case cons(c, cs):
    case nil:
  }
}

lemma void drop_take_singleton<t>(int i, list<t> xs)
  requires 0 < i && i < length(xs);
  ensures drop(i-1, take(i, xs)) == singleton(nth(i-1, xs));
{
  switch (xs) {
  case nil:
  case cons(x0, xs0):
    if (i == 1) {
    } else {
      drop_take_singleton(i-1, xs0);
    }
  }
}

lemma void take_singleton<t>(int i, list<t> xs)
  requires 0 <= i && i < length(xs);
  ensures append(take(i, xs), singleton(nth(i, xs))) == take(i+1, xs);
{
  switch (xs) {
  case nil:
  case cons(x0, xs0):
    if (i == 0) {
    } else {
      take_singleton(i-1, xs0);
    }
  }
}

lemma void drop_update_le<t>(int i, int j, t x, list<t> xs)
  requires 0 <= i && i <= j && j < length(xs);
  ensures drop(i, update(j, x, xs)) == update(j - i, x, drop(i, xs));
{
  switch (xs) {
  case nil:
  case cons(x0, xs0):
    if (i == 0) {
    } else {
      drop_update_le(i - 1, j - 1, x, xs0);
    }
  }
}

lemma void drop_update_ge<t>(int i, int j, t x, list<t> xs)
  requires 0 <= j && j < i && i < length(xs);
  ensures drop(i, update(j, x, xs)) == drop(i, xs);
{
  switch (xs) {
  case nil:
  case cons(x0, xs0):
    if (j == 0) {
    } else {
      drop_update_ge(i - 1, j - 1, x, xs0);
    }
  }
}

lemma void take_update_le<t>(int i, int j, t x, list<t> xs)
  requires 0 <= i && i <= j;
  ensures take(i, update(j, x, xs)) == take(i, xs);
{
  switch (xs) {
  case nil:
  case cons(x0, xs0):
    if (i == 0) {
    } else {
      take_update_le(i - 1, j - 1, x, xs0);
    }
  }
}

lemma void take_update_ge<t>(int i, int j, t x, list<t> xs)
  requires 0 <= j && j < i && i <= length(xs);
  ensures take(i, update(j, x, xs)) == update(j, x, take(i, xs));
{
  switch (xs) {
  case nil:
  case cons(x0, xs0):
    if (j == 0) {
    } else {
      take_update_ge(i - 1, j - 1, x, xs0);
    }
  }
}

lemma void update_eq_append<t>(int i, t x, list<t> xs)
  requires 0 <= i && i < length(xs);
  ensures update(i, x, xs) == append(take(i, xs), cons(x, drop(i + 1, xs)));
{
  switch (xs) {
  case nil:
  case cons(x0, xs0):
    if (i == 0) {
    } else {
      update_eq_append(i - 1, x, xs0);
    }
  }
}

lemma void take_append_ge<t>(int n, list<t> xs, list<t> ys)
  requires length(xs) <= n;
  ensures take(n, append(xs, ys)) == append(xs, take(n - length(xs), ys));
{
  switch (xs) {
  case nil:
  case cons(x0, xs0):
    take_append_ge(n - 1, xs0, ys);
  }
}

lemma void drop_drop<t>(int m, int n, list<t> xs)
  requires 0 <= m && 0 <= n;
  ensures drop(m, drop(n, xs)) == drop(m+n, xs);
{
  switch (xs) {
  case nil:
  case cons(x0, xs0):
    if (n == 0) {} else {
      drop_drop(m, n-1, xs0);
    }
  }
}

lemma void take_take<t>(int m, int n, list<t> xs)
  requires 0 <= m && m <= n && n <= length(xs);
  ensures take(m, take(n, xs)) == take(m, xs);
{
  switch (xs) {
  case nil:
  case cons(x0, xs0):
    if (m == 0) {} else {
      take_take(m - 1, n - 1, xs0);
    }
  }
}

lemma_auto void take_head<t>(list<t> xs)
  requires 0 < length(xs);
  ensures take(1, xs) == singleton(head(xs));
{
  switch(xs) {
  case nil:
  case cons(x0, xs0):
  }
}

// Following lemma from `verifast/bin/rt/_list.java`
lemma void remove_remove_nth<t>(list<t> xs, t x)
  requires mem(x, xs) == true;
  ensures remove(x, xs) == remove_nth(index_of(x, xs), xs);
{
  switch (xs) {
  case nil:
  case cons(h, tl):
    if (x == h) {
      assert index_of(x, xs) == 0;
    } else {
      remove_remove_nth(tl, x);
    }
  }
}

lemma void mem_take_false<t>(t x, int n, list<t> xs)
    requires mem(x, xs) == false;
    ensures mem(x, take(n, xs)) == false;
{
  switch (xs) {
  case nil:
  case cons(x0, xs0):
      if (x0 != x && n != 0)
          mem_take_false(x, n - 1, xs0);
  }
}

// Following lemma from `verifast/bin/rt/_list.java`. Renamed to
// avoid clash with listex.c's nth_drop lemma. 
lemma void nth_drop2<t>(list<t> vs, int i)
requires 0 <= i && i < length(vs);
ensures nth(i, vs) == head(drop(i, vs));
{
  switch (vs) {
  case nil:
  case cons(v, vs0):
    if (i == 0) {
    } else {
      nth_drop2(vs0, i - 1);
    }
  }
}

lemma void enq_lemma<t>(int k, int i, list<t> xs, list<t> ys, t z)
  requires 0 <= k && 0 <= i && 0 < length(xs) && k < length(xs) && i < length(xs) && take(k, rotate_left(i, xs)) == ys;
  ensures take(k+1, rotate_left(i, update((i+k)%length(xs), z, xs))) == append(ys, cons(z, nil));
{
  int j = (i+k)%length(xs);
  assert take(k, append(drop(i, xs), take(i, xs))) == ys;
  if (i + k < length(xs)) {
    mod_lt(i + k, length(xs));
    assert j == i + k;
    drop_update_le(i, i + k, z, xs);
    assert drop(i, update(i + k, z, xs)) == update(k, z, drop(i, xs));
    take_update_le(i, i + k, z, xs);
    assert take(i, update(i + k, z, xs)) == take(i, xs);
    take_append(k+1, update(k, z, drop(i, xs)), take(i, xs));
    assert take(k+1, append(update(k, z, drop(i, xs)), take(i, xs))) == take(k+1, update(k, z, drop(i, xs)));
    update_eq_append(k, z, drop(i, xs));
    assert update(k, z, drop(i, xs)) == append(take(k, drop(i, xs)), cons(z, drop(k + 1, drop(i, xs))));
    take_append_ge(k+1, take(k, drop(i, xs)), cons(z, drop(k + 1, drop(i, xs))));
    assert take(k+1, append(take(k, drop(i, xs)), cons(z, drop(k + 1, drop(i, xs))))) ==
           append(take(k, drop(i, xs)), {z});
    take_append(k, drop(i, xs), take(i, xs));
    assert take(k+1, append(take(k, drop(i, xs)), cons(z, drop(k + 1, drop(i, xs))))) ==
           append(take(k, append(drop(i, xs), take(i, xs))), {z});
    assert take(k+1, update(k, z, drop(i, xs))) ==
           append(take(k, append(drop(i, xs), take(i, xs))), {z});
    assert take(k+1, append(update(k, z, drop(i, xs)), take(i, xs))) ==
           append(take(k, append(drop(i, xs), take(i, xs))), {z});
    assert take(k+1, append(drop(i, update(i + k, z, xs)), take(i, update(i + k, z, xs)))) ==
           append(take(k, append(drop(i, xs), take(i, xs))), {z});

  } else {
    assert i + k < 2 * length(xs);
    div_rem_nonneg(i + k, length(xs));
    if ((i + k) / length(xs) > 1) {
      mul_mono_l(2, (i + k) / length(xs), length(xs));
    } else if ((i + k) / length(xs) < 1) {
      mul_mono_l((i + k) / length(xs), 0, length(xs));
    }
    assert j == i + k - length(xs);
    assert j < i;
    drop_update_ge(i, j, z, xs);
    assert drop(i, update(j, z, xs)) == drop(i, xs);
    take_update_ge(i, j, z, xs);
    assert update(j, z, take(i, xs)) == take(i, update(j, z, xs));
    take_append_ge(k+1, drop(i, xs), take(i, update(j, z, xs)));
    assert take(k+1, append(drop(i, update(j, z, xs)), take(i, update(j, z, xs)))) ==
           append(drop(i, xs), take(j+1, update(j, z, take(i, xs))));
    update_eq_append(j, z, take(i, xs));
    assert update(j, z, take(i, xs)) == append(take(j, take(i, xs)), cons(z, drop(j + 1, take(i, xs))));
    take_take(j, i, xs);
    assert update(j, z, take(i, xs)) == append(take(j, xs), cons(z, drop(j+1, take(i, xs))));
    take_append_ge(j+1, take(j, xs), cons(z, drop(j+1, take(i, xs))));
    assert append(drop(i, xs), take(j+1, update(j, z, take(i, xs)))) ==
           append(drop(i, xs), append(take(j, xs), {z}));
    take_append_ge(k, drop(i, xs), take(i, xs));
    append_assoc(drop(i, xs), take(j, xs), {z});
    assert append(drop(i, xs), append(take(j, xs), {z})) ==
           append(take(k, append(drop(i, xs), take(i, xs))), {z});
    assert append(drop(i, xs), take(j+1, update(j, z, take(i, xs)))) ==
           append(take(k, append(drop(i, xs), take(i, xs))), {z});
  }
  assert take(k+1, append(drop(i, update(j, z, xs)), take(i, update(j, z, xs)))) ==
         append(take(k, append(drop(i, xs), take(i, xs))), {z});
  assert take(k+1, append(drop(i, update(j, z, xs)), take(i, update(j, z, xs)))) == append(ys, {z});
}

lemma void front_enq_lemma<t>(int k, int i, list<t> xs, list<t> ys, t z)
  requires 0 < length(xs) && 0 <= k && k < length(xs) && 0 <= i && i < length(xs) && take(k, rotate_left((i+1)%length(xs), xs)) == ys;
  ensures take(k+1, rotate_left(i, update(i, z, xs))) == cons(z, ys);
{
  int n = length(xs);
  if (i+1 < n) {
    mod_lt(i+1, n);
    assert i < n;
    assert take(k+1, rotate_left(i, update(i, z, xs))) ==
           take(k+1, append(drop(i, update(i, z, xs)), take(i, update(i, z, xs))));
    drop_update_le(i, i, z, xs);
    take_update_le(i, i, z, xs);
    assert take(k+1, append(drop(i, update(i, z, xs)), take(i, update(i, z, xs)))) ==
           take(k+1, append(update(0, z, drop(i, xs)), take(i, xs)));
    update_eq_append(0, z, drop(i, xs));
    assert update(0, z, drop(i, xs)) == cons(z, drop(1, drop(i, xs)));
    drop_drop(1, i, xs);
    assert take(k+1, append(update(0, z, drop(i, xs)), take(i, xs))) ==
           take(k+1, append(cons(z, drop(i+1, xs)), take(i, xs)));
    assert take(k+1, append(cons(z, drop(i+1, xs)), take(i, xs))) ==
           cons(z, take(k, append(drop(i+1, xs), take(i, xs))));

    assert ys == take(k, rotate_left(i+1, xs));
    assert ys == take(k, append(drop(i+1, xs), take(i+1, xs)));
    if (k <= length(drop(i+1, xs))) {
      take_append(k, drop(i+1, xs), take(i+1, xs));
      take_append(k, drop(i+1, xs), take(i, xs));
    } else {
      take_append_ge(k, drop(i+1, xs), take(i+1, xs));
      take_append_ge(k, drop(i+1, xs), take(i, xs));

      assert (i+1) + k < 2 * n;
      div_rem_nonneg((i+1) + k, n);
      if (((i+1) + k) / n > 1) {
        mul_mono_l(2, ((i+1) + k) / n, n);
      } else if (((i+1) + k) / n < 1) {
        mul_mono_l(((i+1) + k) / n, 0, n);
      }
      int j = ((i+1)+k)%n;
      assert j <= i;
      int l = length(drop(i+1, xs));
      assert l == n - i - 1;
      take_take(k - l, i + 1, xs);
      take_take(k - l, i, xs);
    }
  } else {
    assert i == (n-1);
    assert (i + 1) % n == 0;
    drop_update_le(i, i, z, xs);
    update_eq_append(0, z, xs);
    assert take(k+1, rotate_left(i, update(i, z, xs))) ==
           take(k+1, append(drop(i, update(i, z, xs)), take(i, update(i, z, xs))));
    drop_update_le(i, i, z, xs);
    assert take(k+1, rotate_left(i, update(i, z, xs))) ==
           take(k+1, append(update(0, z, drop(i, xs)), take(i, update(i, z, xs))));
    update_eq_append(0, z, drop(i, xs));
    assert take(k+1, rotate_left(i, update(i, z, xs))) ==
           take(k+1, append(cons(z, drop(1, drop(i, xs))), take(i, update(i, z, xs))));
    drop_drop(1, i, xs);
    assert take(k+1, rotate_left(i, update(i, z, xs))) ==
           take(k+1, append(cons(z, nil), take(i, update(i, z, xs))));
    take_update_le(i, i, z, xs);
    assert take(k+1, rotate_left(i, update(i, z, xs))) ==
           cons(z, take(k, take(i, xs)));
    take_take(k, i, xs);
    assert take(k+1, rotate_left(i, update(i, z, xs))) == cons(z, ys);
  }
}

lemma void deq_lemma<t>(int k, int i, list<t> xs, list<t> ys, t z)
  requires 0 < k && k <= length(xs) && 0 <= i && i < length(xs) && take(k, rotate_left(i, xs)) == ys && z == head(ys);
  ensures take(k-1, rotate_left((i+1)%length(xs), xs)) == tail(ys);
{
  int j = (i+1)%length(xs);
  drop_n_plus_one(i, xs);
  assert tail(take(k, append(drop(i, xs), take(i, xs)))) == take(k-1, append(drop(i+1, xs), take(i, xs)));
  if (i+1 < length(xs)) {
    mod_lt(i+1, length(xs));
    assert j == i+1;
    if (k-1 <= length(xs)-j) {
      take_append(k-1, drop(j, xs), take(j, xs));
      take_append(k-1, drop(j, xs), take(i, xs));
    } else {
      assert k+i > length(xs);
      take_append_ge(k-1, drop(j, xs), take(j, xs));
      take_append_ge(k-1, drop(j, xs), take(i, xs));
      assert k-1-(length(xs)-j) == k+i-length(xs);
      assert k+i-length(xs) <= i;
      take_take(k+i-length(xs), j, xs);
      take_take(k+i-length(xs), i, xs);
      assert take(k+i-length(xs), take(j, xs)) == take(k+i-length(xs), take(i, xs));
    }
  } else {
    assert i+1 == length(xs);
    assert (i+1)%length(xs) == 0;
    assert j == 0;
    assert append(drop(j, xs), take(j, xs)) == xs;
    assert append(drop(i+1, xs), take(i, xs)) == take(i, xs);
    take_append_ge(k-1, drop(i+1, xs), take(i, xs));
    take_take(k-1, i, xs);
  }
  assert take(k-1, append(drop(j, xs), take(j, xs))) == take(k-1, append(drop(i+1, xs), take(i, xs)));
  assert take(k-1, append(drop(j, xs), take(j, xs))) == tail(take(k, append(drop(i, xs), take(i, xs))));
}

lemma void deq_value_lemma<t>(int k, int i, list<t> xs, list<t> ys)
  requires 0 < k && k <= length(ys) && 0 <= i && i < length(xs) && take(k, rotate_left(i, xs)) == ys;
  ensures nth(i, xs) == head(ys);
{
  drop_n_plus_one(i, xs);
  assert nth(i, xs) == head(take(k, append(drop(i, xs), take(i, xs))));
}

lemma void combine_list_no_change<t>(list<t>prefix, t x, list<t>suffix, int i, list<t> xs)
  requires 0 <= i && i < length(xs) && prefix == take(i, xs) && x == nth(i, xs) && suffix == drop(i+1, xs);
  ensures xs == append(prefix, cons(x, suffix));
{
  drop_n_plus_one(i, xs);
}

// Following lemma from `verifast/examples/vstte2010/problem4.java`. 
lemma void update_rewrite<t>(list<t> vs, t v, int pos)
  requires 0 <= pos && pos < length(vs);
  ensures update(pos, v, vs) == append(take(pos, vs), cons(v, (drop(pos+1, vs))));
{
  switch(vs) {
  case nil:
  case cons(h, t):
    if (pos == 0) {
    } else {
      update_rewrite(t, v, pos - 1);
    }
  }
}

lemma void combine_list_update<t>(list<t>prefix, t x, list<t>suffix, int i, list<t> xs)
  requires 0 <= i && i < length(xs) && prefix == take(i, xs) && suffix == drop(i+1, xs);
  ensures update(i, x, xs) == append(prefix, cons(x, suffix));
{
  update_rewrite(xs, x, i);
}

@*/


#endif /* SCP_COMMON_H */
