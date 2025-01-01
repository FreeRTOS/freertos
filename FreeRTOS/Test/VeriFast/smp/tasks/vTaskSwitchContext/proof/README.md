This directory contains the bulk of VeriFast formalizations and proofs.


# Directory Structure
```
├── lock_predicates.h
│   Contains the formalization of the lock invariants, i.e., the invariants
│   associated with: Masking interrupts, the task lock and the ISR lock.
│   This file also contains the lemmas to prove that the task state updates
│   in `prvSelectHighestPriorityTask` preserve the lock invariants.
│
├── port_locking_contracts.h
│   Contains VeriFast function contracts for macros with port-specific
│   definitions used to invoke synchronization mechanisms, e.g., masking
│   interrupts and acquiring locks. These port-specific definitions often
│   contain inline assembly VeriFast cannot reason about. The contracts allow us
│   to abstract the semantics of the assembly.
│
├── ready_list_predicates.h
│   Contains the predicates describing the ready lists as well as lemmas to
│   reason about ready lists.
│
├── stack_predicates.h
│   Contains the formalization of the stack layout used in the RP2040 port.
│
├── task_predicates.h
│   Contains predicates describing task control blocks.
│
├── task_running_states.h
│   `tasks.c` defines macros that are used to denote task run states.
│   The proof headers in this directory cannot refer to these macros.
│   This header contains auxiliary definitions used to expose the run state
│   macros to the proof headers.
│
├── verifast_lists_extended.h
│   Contains list axioms and lemmas that would naturally fit into VeriFast's
│   standard list library `listex.gh`.
│   
├── README.md
│
├── single_core_proofs
│   Contains the old list formalization and proofs written by
│   Aalok Thakkar and Nathan Chong in 2020 for the single-core
│   setup.
│   │
│   ├── scp_common.h
│   │   Contains auxiliary definitions and lemmas.
│   │
│   └── scp_list_predicates.h
│       Contains the formalizaton of doubly linked lists and list items.
│
└── single_core_proofs_extended
    Contains new proofs extending the single-core list
    formalization.
```
