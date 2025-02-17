# FreeRTOS VeriFast Proofs
This directory contains an unbounded memory safety and thread safety proof
for the SMP implementation of context switches: `vTaskSwitchContext`

This proof is a work-in-progress since the SMP distribution of the kernel has not yet been merged into the main FreeRTOS distribution.
Therefore, this proof and single-core implementation in this repository are currently out-of-sync.
Once that happens, the proof might need to be updated.



## VeriFast
[VeriFast](https://github.com/verifast/verifast)
is a deductive program verifier for C based on separation logic.
It supports verifying concurrent code and reasoning about complex data structures.

VeriFast proofs are *unbounded*.
That is, until explicitly specified, it does not assume any bound on the size of the involved data structures.
Hence, proofs give us unbounded guarantees.
In our case, this means that our proof holds for any number of tasks and any size of the involved data structures.

Reasoning about concurrent code can be tricky because of all the interleavings that can occur.
VeriFast does not assume anything about the occuring interleavings.
Therefore, the proven guarantees hold for every possible interleaving that might occur during runtime.

Being a deductive verifier, VeriFast requires us to manually write a proof.
In particular, we have to specify what well-formed data structures look like and to annotate the code with proof steps.
It then symbolically executes the annotated code and queries an SMT solver to check the validity of proof steps.

This directory contains all the specifications and proof steps necessary to check that the scheduler is memory and thread safe.



## Key Result
Informally, the proof guarantees the following:
```
Proof Assumptions:
 - Data structure specification
 - Locking discipline
 - Contracts abstracting assembly
 - FreeRTOS config
 - Function contract of `vTaskSwitchContext`

==>

Unbounded memory & thread safety guarantees for `vTaskSwitchContext`:
  ∀ #tasks. ∀ task interleavings. ∀ interrupt schedules. ∀ data sizes. ∀ cores C1, …, Cn.
    vTaskSwitchContext(C1)  ||  …  ||  vTaskSwitchContext(Cn)
  =>  (no memory error ∧ no race condition)
```

We have to model certain aspects of the system and our proof assumes that these models are correct (cf. `Proof Assumptions` below for a detailed explanation).
In particular, this modeling step includes writing a precondition for `vTaskSwitchContext` that specifies the context in which the function may be called.

Our proof considers any number of running tasks, any possible task interleavings and any interrupts that might occur during execution.
In particular, it considers any possible size for the involved data structures, since it is an unbounded proof.

The proof ensures that every concurrent execution of `vTaskSwitchContext` on any cores is memory safe and mutually thread safe.
That is, when we execute multiple instances of the function on different cores, we won't get any memory errors or data races, no matter how these instances interleave or when interrupts occur.


# Found Buffer Underflow
During the verification of `vTaskSwitchContext` we found a buffer underflow, fixed it and verified that our fix works.
The guarantees stated in the section above concern the fixed-up code.
We submitted the fix as a pull request: [Fixed buffer underflow in prvSelectHighestPriorityTask. #607](https://github.com/FreeRTOS/FreeRTOS-Kernel/pull/607)

Our verification target `vTaskSwitchContext` calls the auxiliary function `prvSelectHighestPriorityTask` to choose the task that will be scheduled next.
This works as long as the idle tasks have already been created.
The idle tasks are tasks whose only purpose is to run and do nothing in case there is no other task that can be scheduled.

However, `prvSelectHighestPriorityTask` can also be called before the idle tasks have been created.
When that happens, the function decrements the global variable `uxTopReadyPriority` to -1.
This variable is supposed to store the highest priority for which we know that there is a ready task.
Priorities start at 0, so -1 is an invalid value.

During the next regular context switch, `vTaskSwitchContext` calls `prvSelectHighestPriorityTask`.
The latter looks at `uxTopReadyPriority` to detect at which priority level it should start its search.
Hence, it accesses the global ready list array at index -1, i.e., `pxReadyTasksLists[ uxCurrentPriority ]`.
This causes a memory error.

# Proof Directory Structure
```
FreeRTOS/FreeRTOS
│
│
├── Source
│   │   This directory contains the FreeRTOS kernel. Currently, it still
│   │   contains the single-core implementation.
│   │
│   │
│   ├── *.c files
│   │   The base directory contains the source files. Note that our proof uses
│   │   annotated copies of the SMP versions of these files, which are located
│   │   in the proof directory.
│   │
│   │
│   └── include
│       Contains the header files. Note that our proof uses annotated copies of
│       the SMP versions of these files, which are located in the proof
│       directory.
│
│
├── .github/workflows
│   └── verifast-proof-diff.yml
│       This workflow is triggered on every pull request and checks for
│       potential divergences between the production code and the proof.
│
│
└── Test/VeriFast/tasks/vTaskSwitchContext
    │
    ├── run-verifast.sh
    │   Shell script to check the proof with VeriFast.
    │
    ├── run-vfide.sh
    │   Shell script to load the proof into the VeriFast IDE.
    │
    ├── diff.sh
    │   Shell script to flag changes in the production code that potentially
    │   break the validity of the VeriFast proof. An empty diff means that the
    │   proof and the production code remain in sync.
    │
    ├── preprocessing_scripts
    │   Contains scripts to preprocess and rewrite the source code.
    │
    ├── demos/FreeRTOS-SMP-Demos
    │   Contains the FreeRTOS SMP demo. Our proof uses some of its
    │   configuration files. Since these files contain syntax VeriFast cannot
    │   parse, we currently use a fork of the SMP demo where the problematic
    │   code has been adapted for VeriFast, until the corresponding VerFast
    │   issues have been fixed.
    │    
    ├── github-workflows
    │   └── verifast-proof-diff.yml
    │       The GitHub worklow that checks for divergences between the proof
    │       and the production code.
    │
    ├── include
    │   Contains the header files used by our proof.
    │   These files are taken from the SMP implementation of the kernel.
    │   Some have been annotated with VeriFast contracts, predicates, lemmas
    │   and proof steps.
    │
    ├── portable/Thirdparty/GCC/RP2040
    │    Contains the SMP version of the Raspberry Pi Pico port
    │    layer.
    │
    │
    ├── proof
    │   Contains the VeriFast proof files.
    │   │
    │   ├── *.h files
    │   │   Headers from the SMP implementation of the kernel.
    │   │   Some contain VeriFast formalizations and proofs.
    │   │
    │   ├── README.md
    │   │   Contains overview about proof files.
    │   │
    │   ├── single_core_proofs
    │   │   Contains the old list formalization and proofs written by
    │   │   Aalok Thakkar and Nathan Chong in 2020 for the single-core
    │   │   setup. We adapted the formalization for this proof, but the files
    │   │   still contain the original version for reference.
    │   │
    │   └── single_core_proofs_extended
    │       Contains new proofs about the single-core list formalization.
    │
    │
    ├── proof_setup
    │   Contains config files for the proof. The proof assumes a setup for
    │   RP2040.
    │
    ├── pico-sdk
    │   Contains headers from the Pico sdk referenced by the proof setup.
    │   Some files contain parts we had to rewrite so that VeriFast would parse
    │   them.
    │
    ├── src
    │   Contains the source  files used by our proof.
    │   These files are taken from the SMP implementation of the kernel.
    │   Some have been annotated with VeriFast contracts, predicates, lemmas
    │   and proof steps.
    │
    └── stats
        Contains some statistics about the VeriFast proof.
```



# Checking the Proof
The proof can be checked by running one of the scripts `run-verifast.sh` and
`run-vfide.sh` residing in this directory (see repo structure above).
Both scripts preprocess the annotated code with Clang and rewrite syntax
VeriFast does not understand into something equivalent.
The result is written to a temporary file (`preprocessed_files/tasks_vf_pp.c`)
before it is processed by VeriFast.
This file contains a copy of all the code and annotations required to check the
proof.
Both scripts expect the command line arguments explained below.

- #### run-verifast.sh:
  Preprocesses the code and proof files and uses the
  command-line version of VeriFast to check the resulting proof file.
  A call must have the form:
  #### run-verifast.sh \<REPO_BASE_DIR\> \<VERIFAST_DIR\>
  where
  - \<REPO_BASE_DIR\> is the absolute path to this repository's base directory,
    i.e., `FreeRTOS` in the repo structure depicted above.
  - \<VERIFAST_DIR\> is the absolute path to the VeriFast installation
    directory.

- #### run-vfide.sh:
  Preprocesses the code and proof files and loads the resulting proof file into
  the VeriFast IDE.
  A call must have the form:
  #### run-vfide.sh \<REPO_BASE_DIR\> \<VERIFAST_DIR\> \[\<FONT_SIZE\>\]
  where
  - \<REPO_BASE_DIR\> \<VERIFAST_DIR\> are as explained above
  - \<FONT_SIZE\> is an optional argument specifying the IDE's font size.


# Reading the Proof
The most important aspects any reader has to know about before they can understand the proof are the locking discipline and the lock invariants.
We suggest to read the proof in a top-down approach.
That is, the reader should start by reading the documentation and definitions of the most important concepts.
Afterwards, we suggest to continue by reading the most important parts of the verified functions:
The contracts and the loop invariants.
Only once these are understood, we suggest to read the low-level proof annotations in the verified functions (e.g. open/close statements, lemma calls).

We propose the following order:
  1. The locking discipline, formalized and documented in `proof/port_locking_contracts.h`.

    FreeRTOS uses macros to invoke synchronization mechanisms (activating/deactivating interrupts and acquiring/releasing locks).
    The definitions of these macros are port-specific.
    The file `proof/port_locking_contracts.h` contains contracts abstracting the port-specific definitions and formalizing the synchronization mechanisms and the locking discipline, e.g., the order in which locks have to be acquired.

  2. The lock invariants, formalized and documented in `proof/lock_predicates.h`.

     The invariants express which resources the locks and the masking of interrupts protect.
     When we acquire a lock or deactivate interrupts, the invariants determine which level of access permissions (i.e. read or write access) we get for the protected resources.
     Since the locks protect the ready lists and task control blocks, the invariants reference the ready list and task predicates defined in `proof/ready_list_predicates.h` and `task_predicates.h`.

  3. The contracts for the functions we verified, i.e., `vTaskSwitchContext` and `prvSelectHighestPriorityTask`, cf. `src/tasks.c`.

  4. The loop invariants in `prvSelectHighestPriorityTask`.

  5. The low-level proof annotations in `vTaskSwitchContext` and `prvSelectHighestPriorityTask`, e.g., open/close statements and lemma calls.






# Maintaining the Proof
This directory contains annotated copies of the SMP versions FreeRTOS source and header files.
The annotations in these files tell VeriFast which functions it should verify and what the proof looks like.

Since the SMP version of the kernel has not yet been merged into this repository, the proof is currently out-of-sync with the production code.

However, even when the proof and production code are in sync, including these annotations in the production code would lead to a huge visual burden for developers.
Therefore, the proof should continue to use annotated copies.
The downside of this approach is that even after the SMP distribution has been merged into this repository, the code and the proof can once again run out of sync.

Therefore, we provide a currently deactivated GitHub workflow to check for potential divergences, cf.
`github-workflows/github/verifast-proof-diff.yml`.
Once activated, the workflow is triggered on every pull request.
It aggregates and preprocesses the parts of the production code relevant to our proof as well as the annotated copies in this directory.
Afterwards, it computes a diff between both versions and fails if the result is not empty, in which case the diff result will be logged in the GitHub actions log.
An empty diff means that the pull request did not change anything that can affect our proof and the proof remains valid.
A non-empty diff shows which changes in the pull request potentially impact our proof.
In this case, the changes should also be applied to the annotated copies and the proof should be checked again.
If the detected divergence was not a false positive and indeed impacted the proof, the proof will likely require manual repair.

The diff can also be manually checked by running the command
`diff.sh <REPO_BASE_DIR>`, where the argument is the absolute path to the repository's base directory.



# Disclaimer
All scripts and proofs have been tested under OS X 12.6.1 and with the VeriFast nightly build from Dec 31, 2022 (corresponds to commit [9e32b122b54152a2ac75a811aa422d638b56c6ab](https://github.com/verifast/verifast/commit/9e32b122b54152a2ac75a811aa422d638b56c6ab)).



# Proof Assumptions
We have to model certain aspects of the system in order to reason about the task scheduler.
The proof treats these models as assumptions.
Therefore, the proof's correctness relies on the correctness of our models.



- ### FreeRTOS Configuration
  The VeriFast proofs assume a setup for the Raspberry Pi Pico, i.e., RP2040, cf. directory `proof_setup` and `portable/Thirdparty/GCC/RP2040`.
  We use the config files from the official FreeRTOS SMP demo for the RP2040 and from official RP2040 port.
  The most important properties of this configuration are:
  - It supports running multiple priorities in parallel on different cores.
  - Core affinity is deactivated, i.e., all tasks may be scheduled on any core.

  The Raspberry Pi Pico only has two cores and we want to ensure that our proof does not accidentally rely on the properties that come with this binary setup.
  Hence, we changed the number of cores to an arbitrary large number.



- ### Contracts Abstracting Assembly
  The port layer of FreeRTOS contains assembly code that is essential for our proof.
  In particular, code to mask interrupts and code to acquire and release locks.
  VeriFast is a program verifier for C and not designed to handle any kind of assembly.
  The port-specific assembly is called via macros with a port-specific definition.
  We redefined these macros to call dummy function prototypes instead.
  We equipped these prototypes with VeriFast contracts that capture the semantics of the original assembly code, cf. `proof/port_locking_contracts.h`.
  This way, VeriFast refers to the contracts to reason about the macro calls and does not have to deal with the assembly code.



- ### Data structure specification
  VeriFast expects us to specify the memory layout of the data structures accessed by the task scheduler.
  In a proof, these specifications tell us what a well-formed instance of a data structure looks like and how me may manipulate it to preserve well-formedness.

  Most notably, the scheduler searches the so called "ready lists", a global array of cyclic doubly linked lists storing tasks of specific priorities that are ready to be scheduled.
  Reasoning about this data structure is challenging because it requires heavy reasoning about its complex internals.

  Previously, Aalok Thakkar and Nathan Chong used VeriFast to prove functional correctness of the stand-alone list data structure for a single-core setup, c.f. [FreeRTOS Pull Request 836: Update VeriFast proofs](https://github.com/FreeRTOS/FreeRTOS/pull/836).
  We reused their formalization and proofs as much as possible.
  However, we had to heavily adapt both to tailor them to the needs of the scheduler proof, cf. `Proof Details` below.

  The reused specification resides in `proofs/single_core_proofs/`.
  The full ready list array is specified in `proofs/ready_list_predicates.h`.


- ### Function Contract of `vTaskSwitchContext`
  VeriFast expects every function that it verifies to have a so called "function contract".
  These contracts consist of a precondition, also called the "requires clause" and a postcondition, also called the "ensures clause".
  The precondition characterizes the context in which the function may be called.
  This determines the state in which our proof starts.
  The postcondition characterizes the state we want to be in when the function terminates.

  Starting from the precondition, VeriFast symbolically executes the function's code and our annotated proof steps.
  The proof succeeds if every step succeeds and if the proof ends in a state that complies with the specified postcondition.

  Hence, the function contract determines *WHAT* we prove.
  `vTaskSwitchContext` is called by an interrupt defined in the port layer on some core `C`.
  This interrupt masks interrupts on this core and acquires the locks protecting the ready lists.
  Therefore, the precondition of `vTaskSwitchContext` states that:
  - the function is executed on an arbitrary core `C`
  - interrupts on core `C` are deactivated
  - the locks protecting the ready lists have been acquired
  - that all the relevant global data structures are well-formed

  The postcondition states that all these properties are preserved, which is what the interrupt calling into the scheduler expects.



- ### Locking discipline and lock invariants
  FreeRTOS' SMP implementation uses the following synchronization mechanisms:
  - Deactivating interrupts:
    Some data is only meant to be accessed on a specific core C.
    Such data may only be accessed after interrupts on core C have been deactivated.
    For instance the global array `pxCurrentTCBs` in `tasks.c` has an entry for
    every core.
    `pxCurrentTCBs[C]` stores a pointer to the task control block (TCB) of the task running on core C.
    Core C is always allowed to read `pxCurrentTCBs[C]`.
    However, writing requires the interrupts on core C to be deactivated.

  - task lock:
    The task lock is used to protect ciritical sections and resources from being accessed by multiple tasks simultaneously.

  - ISR lock:
    The ISR/ interrupt lock is used to protect critical sections and resources from being accessed by multiple interrupts simultaneously.

  - task lock + ISR lock:
    Access to certain resources and ciritical sections are protected by both the task lock and the ISR lock.
    For these, it is crucial that we first acquire the task lock and then the ISR lock.
    Likewise, we must release them in opposite order.
    Failure to comply with this order may lead to deadlocks.
    The resources protected by both locks are the main resources this proof deals with.
    These include the ready lists and the certain access rights to the tasks' run states.

 #### Lock Invariants
 Every synchronization mechanism protects specific data structures and sections of code.
 For our proof, we associate every synchronization mechanism `L` with permissions to access the resources it protects.
 We do this by defining a so called "lock invariant" `I`.
 Besides pure access permissions the invariant can also specify more specifc properties, such as that a data structure must be well-formed.
 (We call it "lock invariant" even though we also use the same technique to model the masking of interrupts.)
 When we acquire lock `L` (or deactivate the interrupts) we produce the lock invariant `I`.
 That means, we get the access permissions `I` expresses.
 When we release the lock `L` (or reactivate the interrupts), we consume the invariant `I`.
 That means that we lose the access permissions granted by `I`.
 While we hold the lock, we are free to manipulate the resources it protects (according to the permissions granted by `I`).
 However, we have to prove that whatever we do with these resources preserves any guarantees given by the invariant.
 For instance, if `I` says a data structure is well-formed then we must prove that our actions preserve well-formedness.
 Otherwise, consuming `I` during the release step will fail and consequently the entire proof will fail.

 FreeRTOS uses macros with port-specifc definitions to acquire and release locks and to mask and unmask interrupts.
 We abstracted these with VeriFast contracts defined in `proof/port_locking_contracts.h`.
 The contracts ensure that invoking any synchronization mechanism produces or consumes the corresponding invariant.
 The invariants are defined in `proof/lock_predicates.h`




# Proof Details

## Context Switches and Ready Lists
Our proof ensures that the context switches performed by `vTaskSwitchContext` are memory and thread safe.
The most difficult part of a context switch is to find a new task that we can schedule.
For that, `vTaskSwitchContext` calls `prvSelectHighestPriorityTask` which searches for the task with the highest priority that can be scheduled.
FreeRTOS maintains a global data structure called the "ready lists".
It is an array `pxReadyTasksLists` with an entry for every priority level that a task might have.
For every such priority level `p`, `pxReadyTasksLists[p]` stores a cyclic doubly linked list containing all tasks of priority level `p` that are ready to be scheduled, including currently running ones.
`prvSelectHighestPriorityTask` searches through these lists in descending order.
That is, in order to verify `vTaskSwitchContext`, we have to reason about the ready lists.



## Reusing the Single-Core List Formalization and Proofs
In 2020 Aalok Thakkar and Nathan Chong verified the functional correctness of the FreeRTOS list API for a single-core setup, cf. [FreeRTOS Pull Request 836: Update VeriFast proofs](https://github.com/FreeRTOS/FreeRTOS/pull/836).
The list API has not been changed during the port of FreeRTOS to SMP.
Ready lists are fully protected by the task and ISR locks, which allows FreeRTOS to continue using the single-core implementation of the list API.

We reuse the single-core list formalization to model the ready list for each priority level.
However, due to challenges that arise in the scheduler, we had to extend and adapt the existing formalization.

The single-core list formalization and lemmas that we reuse are located in `proofs/single_core_proofs/scp_list_predicates.h`.
The list API is defined in `include/list.h` and `src/list.c`.
The latter also contains the API proofs.



## Comparing the Original List Proofs and Our Adaptation
As mentioned, we had to heavily adapt the list formalization and proofs to reuse them for the scheduler verification.
Therefore, both `scp_list_predicates.h` and `list.c` contain an updated version of the formalization and proofs used by our context-switch proof and the original version by Aalok Thakkar and Nathan Chong.
The latter is guarded by a preprocessor define `VERIFAST_SINGLE_CORE`.
We can compare both versions by preprocessing both files twice: Once with the define `VERIFAST_SINGLE_CORE`, which yields the original version, and once without which gives us the version used by our proofs.
Afterwards, a diff will show all the adaptations we had to apply.



## List Predicates

The single-core list formalization defines two main predicates:
- ```
  predicate xLIST_ITEM(struct xLIST_ITEM *n,
                       TickType_t xItemValue,
                       struct xLIST_ITEM *pxNext,
                       struct xLIST_ITEM *pxPrevious,
                       struct xLIST *pxContainer;)
  ```
  Represents a list item of type `xLIST_ITEM`.
  The arguments have the following semantics:
  - `n`: A pointer to the node whose memory the predicate represents.
  - `xItemValue`: The value stored in node `n`.
  - `pxNext`: The node's "next" pointer, i.e., `n->pxNext`.
  - `pxPrevious`: The node's "previous" pointer, i.e., `n->pxPrevious`.
  - `pxContainer`: The doubly linked list containing this node.
- ```
  predicate DLS(struct xLIST_ITEM *n,
                struct xLIST_ITEM *nprev,
                struct xLIST_ITEM *mnext,
                struct xLIST_ITEM *m,
                list<struct xLIST_ITEM * > cells,
                list<TickType_t > vals,
                struct xLIST *pxContainer)
  ```
  Represents a non-empty doubly linked list segment.
  The semantics of the arguments are as follows:
  - `n`: The left-most node in the segment.
  - `nPrev`: The left-most node's "previous" pointer, i.e., `n->pxPrevious`.
  - `mNext`: The right-most node's "next" pointer, i.e., `m->pxNext`.
  - `m`: The right-most node.
  - `cells`: A VeriFast list storing pointers to all nodes the list contains.
  - `vals`: A VeriFast list storing the list nodes' values.
  - `pxContainer`: A pointer to list struct.

  The single-core formalization also uses `DLS` not just to represent list segments but also to express unsegmented cyclic linked lists.
  In FreeRTOS lists start with a sentinel, called "end".
  Using the `DLS` predicate, a cyclic list has the form:
  `DLS(end, endPrev, end, endPrev, cells, vals, list)`




## Issue 1: List Predicates Do Not Expose Tasks
Each node in a ready list points to task control block (TCB) representing a task that is ready to run.
The TCB a node points to is called its "owner".
`prvSelectHighestPriorityTask` iterates through the ready lists and looks at each TCB it finds to determine which task to schedule next.
Hence, it is crucial that we reason about these TCBs.
However, the list predicates depicted above do not expose this information.
Hence, we have to extend the predicate signatures to:
```
  predicate xLIST_ITEM(struct xLIST_ITEM *n,
                       TickType_t xItemValue,
                       struct xLIST_ITEM *pxNext,
                       struct xLIST_ITEM *pxPrevious,
                       void* pxOwner,
                       struct xLIST *pxContainer;)
```
where `pxOwner` is the TCB pointer stored in the represented node
and
```
predicate DLS(struct xLIST_ITEM *n,
              struct xLIST_ITEM *nprev,
              struct xLIST_ITEM *mnext,
              struct xLIST_ITEM *m,
              list<struct xLIST_ITEM * > cells,
              list<TickType_t > vals,
              list<void*> owners,
              struct xLIST *pxContainer)
```
where `owners` is a list of all the TCBs pointed to by the list nodes.

While this change seems simple on a first glance, it forced us to adapt all the list proofs we reuse.



## Issue 2: Model-induced Complexity

The formalization of doubly-linked list segments induces heavy complexity.
The problem lies in the fact that `DLS` cannot express empty list segments.
This leads to complex case distinctions whenever we access list nodes.
Consequently, our proof becomes very complex and every list access leads to an exponential blow-up of the proof tree.
This in turn leads to very bad performance when checking the proof.
We solved this problem by introducing a new representation of a cyclic doubly-linked list as a potentially empty prefix, the node we want to access and a potentially empty suffix: `DLS_prefix(....) &*& xLIST_ITEM(node, ...) &*& DLS_suffix(...)`
We added lemmas that allow us to freely convert between a `DLS` predicate and our new representation.
Thereby, the proof became a lot simpler and it reduced the time needed to check the proof from ~20 minutes to about 12.5 seconds.
The following sections explain the details of the problem and our solution.

### Iterating through a DLS

The function `prvSelectHighestPriorityTask` iterates through the ready lists.
Hence, reasoning about it requires us to reason about iteration through memory described by a `DLS` predicate instance. Consider the following scenario:
We have a `DLS` predicate representing our cyclic ready list and a task item pointer `pxTaskItem` which points to an element of this list.

- `DLS(end, endPrev, end, endPrev, cells, vals, owners, readyList)`
- `mem(pxTaskItem, cells) == true`

Suppose we want to move the task pointer forward

- `pxTaskItem2 = pxTaskItem->pxNext`

In order to verify this line we have to do two things:

1. Justify the heap access to `pxTaskItem->pxNext`
2. Prove that `pxTaskItem2` points to an element of the list. This is
   necessary to reason about any code that uses `pxTaskItem2`.

We can do this by opening the recursive predicate at the nodes for `pxTaskItem` and `pxTaskItem->next`, for which we can reuse the existing list proof lemmas.
When the right parts of the predicate are exposed, we can prove (1) and (2).
Afterwards, we have to close the predicate again.





### Proofs Are Hard

Proving (1) and (2) forces us to consider many different cases, which leads to complicated proofs.
The position of `pxTaskItem` in the list determines how we should open the `DLS` (either by using the existing `split` lemma or with VeriFast’s `open` command) and also how we have to close it at the end of the proof.
Accessing `pxTaskItem->pxNext` introduces more case splits that complicate the proof.
Again, closing the predicate has to account for all the introduced cases.

Introducing lemmas to open and close the predicate helps us to hide this complexity inside the lemmas.
Thereby, the main proof using these lemmas gets shorter.
However, the next section explains why this approach does not eliminate the complexity.

Note that proofs for forward iteration cannot be reused for backwards iteration.
Instead the latter requires separate proofs.



### Bad Performance

As explained above, reasoning about a single statement that moves the item pointer forward or backward introduces many case splits. `prvSelectHighestPriorityTask` contains multiple statements that manipulate the item pointer.
From VeriFast’s perspective, each consecutive proof of such an iteration statement splits up the proof tree further.
In other words: Every iteration statement leads to an exponential blow-up of the sub-proof-tree rooted at this statement.
This is the case even though this part of the code we reason about is linear.

Introducing lemmas for opening and closing shortens the consecutive iteration proofs significantly, but does not eliminate the case splits.
The reason for this is that the `DLS` predicate cannot express empty segments and depending on the current proof path, the shape of the heap changes.
Our proof has to account for the following possibilities:
- non-empty prefix and no suffix:
  ```
  DLS(...) &*& xLIST_ITEM(node, ...)
  ```
- non-empty prefix and non-empty suffix:
  ```
  DLS(...) &*& xLIST_ITEM(node, ...) &*& DLS(...)
  ```
- no prefix and non-empty suffix:
  ```
  xLIST_ITEM(node, ...) &*& DLS(...)
  ```

In our proof we know that the ready list we travers always contains the sentinel and an additional node.
So, we can eliminate the case where both the prefix and the suffix are empty.

We cannot unify the representation of the proof state as long as we stick to the `DLS` predicate.
Instead the opening lemma’s postcondition and the closing lemma’s precondition must reflect the case split.
Consequently, applying the lemmas in a proof introduces the case splits anyway and consecutive iteration statements/ lemma applications increase the number of proof paths exponentially.
VeriFast requires ~20 min to reason about 4 iteration statements.



### Solution: Introduce new representation for opened DLS


The only way to eliminate the case splits in `prvSelectHighestPriorityTask` is to unify the proof state of an opened `DLS` across all proof paths.
We introduce two new predicates that express potentially empty prefixes and suffixes of opened cyclic `DLS`.
With that, we can formalize an opened list in a unified way as

- `DLS_prefix(....) &*& xLIST_ITEM(pxTaskItem, ...) &*& DLS_suffix(...)`

Additionally, we write opening and closing lemmas that transform the a `DLS` predicate instance into our new representation and back.
The proof state we get after opening the list does not force VeriFast to consider any case splits.
This finally eliminates the complexity induced by the non-empty list model.

Eliminating these case splits reduces verification time from ~20min to ~12.5s

Before we introduced this new list representation, we wrote opening and closing lemmas that used the `DLS` formulation.
It turns out that switching to the new representation does not only simplify the proof state we get after opening, but it also simplifies the opening and closing lemmas, though they remain very complicated.

The old opening and closing lemmas required switching the SMT solver to Z3, which is much slower than VeriFast's standard SMT solver.
The lemmas required heavy reasoning about applications of `list<t>` fixpoint functions and the shape of the inductive `list<t>` datatype.
VeriFast offers limited capabilities to reason about fixpoint functions (apart from axiomatizing) and the standard SMT solver often has problems reasoning about the shape of results, e.g., assertions of the form `drop(i, vals) == cons(_, _)`.
The new lemmas' proofs don’t require Z3.
This allowed us to switch back to VeriFast’s standard SMT solver.

Note that the lemmas still have to consider every possible case internally. That is, the opening and closing lemmas remain complicated.
