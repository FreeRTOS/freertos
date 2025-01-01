/*
 * This file contains defines to configure the VeriFast proof setup.
 *
 */


#ifndef PROOF_DEFS_H
    // Delete keywords VeriFast canot parse (in some contexts)
    #define inline
    #define __always_inline

    /* `projdefs.h` defines `pdFALSE` and `pdTRUE` as 0 and 1 of type
     * `BaseType_t`. Both are assigned to variables smaller or
     * unsigned types. While that's safe in practice, it is not
     * type safe. Hence we define 
     */
    #undef pdFALSE
    #undef pdTRUE
    #define pdFALSE             ( ( char ) 0 )
    #define pdTRUE              ( ( char ) 1 )
    #define pd_U_FALSE          ( ( unsigned char ) pdFALSE )
    #define pd_U_TRUE           ( ( unsigned char ) pdTRUE )

    #undef assert
    #undef configASSERT
    #define configASSERT(x)     assert(x)
#endif /* PROOF_DEFS_H */
