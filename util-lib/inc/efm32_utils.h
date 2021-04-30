#ifndef UTIL_LIB_INC_EFM32_UTILS_H_
#define UTIL_LIB_INC_EFM32_UTILS_H_

// Enables the usage of low energy peripherals, e.g. LEUART
#define CMU_ENABLE_LE_PERIPHERALS()  do {	CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_LE; } while (0);


#endif /* UTIL_LIB_INC_EFM32_UTILS_H_ */
