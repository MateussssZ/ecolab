/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   IEcoMemoryAllocator1
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов IEcoMemoryAllocator1
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2016 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#include "IEcoSystem1.h"
#include "CEcoMemoryManager1Lab.h"


/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
static EcoMemoryBlock_623E1838Ptr WORST_FIT_FIND(CEcoMemoryManager1Lab_623E1838* pCMe, uint32_t size) {
    EcoMemoryBlock_623E1838Ptr current = pCMe->m_pFirstBlock;
    EcoMemoryBlock_623E1838Ptr worst_fit = NULL;
    uint32_t max_size = 0;
    
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            if (current->size > max_size) {
                max_size = current->size;
                worst_fit = current;
            }
        }
        current = current->next;
    }
    
    return worst_fit;
}

static void SPLIT_BLOCK(EcoMemoryBlock_623E1838Ptr block, uint32_t size) {
    if (block == NULL) return;
    
    /* size - это размер данных, нужно добавить размер заголовка */
    uint32_t total_needed = size + sizeof(EcoMemoryBlock_623E1838);
    uint32_t remaining_size = block->size - total_needed;
    
    /* Разделяем только если останется достаточно места для нового блока */
    if (remaining_size > sizeof(EcoMemoryBlock_623E1838)) {
        EcoMemoryBlock_623E1838Ptr new_block = (EcoMemoryBlock_623E1838Ptr)
            ((uint64_t)block + total_needed);
        
        new_block->size = remaining_size;
        new_block->is_free = 1;
        new_block->next = block->next;
        new_block->prev = block;
        
        if (block->next != NULL) {
            block->next->prev = new_block;
        }
        
        block->next = new_block;
        block->size = total_needed;  /* Только размер этого блока */
    }
}

static void UNITE_BLOCKS(CEcoMemoryManager1Lab_623E1838* pCMe, EcoMemoryBlock_623E1838Ptr block) {
    /* Объединение с предыдущим блоком */
    if (block->prev != NULL && block->prev->is_free) {
        block->prev->size += block->size;
        block->prev->next = block->next;
        
        if (block->next != NULL) {
            block->next->prev = block->prev;
        }
        
        block = block->prev;
    }
    
    /* Объединение со следующим блоком */
    if (block->next != NULL && block->next->is_free) {
        block->size += block->next->size;
        block->next = block->next->next;
        
        if (block->next != NULL) {
            block->next->prev = block;
        }
    }
}

int16_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_QueryInterface(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoMemoryManager1) ) {
        *ppv = &pCMe->m_pVTblIMgr;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoMemoryAllocator1) ) {
        *ppv = &pCMe->m_pVTblIMem;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoVirtualMemory1) ) {
        *ppv = &pCMe->m_pVTblIVirtual;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIMgr;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }

    return ERR_ECO_SUCCESES;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_AddRef(/* in */ IEcoMemoryAllocator1Ptr_t me) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 ) {
        return -1;
    }

    return atomicincrement_int32_t(&pCMe->m_cRef);
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Release(/* in */ IEcoMemoryAllocator1Ptr_t me) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 ) {
        return -1;
    }

    /* Уменьшение счетчика ссылок на компонент */
    atomicdecrement_int32_t(&pCMe->m_cRef);

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoMemoryManager1Lab_623E1838((IEcoMemoryManager1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

static uint32_t ALIGN_SIZE(uint32_t size) {
    /* Выравниваем на 8 байт (64 бита) */
    #define ALIGNMENT 8
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

/*
 *
 * <сводка>
 *   Функция Alloc
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Alloc(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ uint32_t size) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));
    
    if (size == 0) {
        return NULL;
    }

    uint32_t aligned_size = ALIGN_SIZE(size);
    
    /* Нужно учесть размер заголовка при поиске блока */
    uint32_t total_needed = aligned_size + sizeof(EcoMemoryBlock_623E1838);
    
    EcoMemoryBlock_623E1838Ptr block = WORST_FIT_FIND(pCMe, total_needed);
    if (block == NULL) {
        return NULL;
    }

    /* Убираем блок из списка свободных */
    block->is_free = 0;
    
    /* Разделяем блок если нужно */
    SPLIT_BLOCK(block, aligned_size);

    /* Обновляем статистику */
    pCMe->m_iAllocSize += aligned_size;
    pCMe->m_iBlockNum++;
    pCMe->m_iTotalAllocated += aligned_size;
    
    if (pCMe->m_iAllocSize > pCMe->m_iMaxAllocated) {
        pCMe->m_iMaxAllocated = pCMe->m_iAllocSize;
    }
    
    /* Возвращаем указатель на данные, а не на заголовок */
    return (void*)((uint64_t)block + sizeof(EcoMemoryBlock_623E1838));
}

/*
 *
 * <сводка>
 *   Функция Free
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Free(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (pv == NULL) {
        return;
    }
    
    /* Получаем заголовок из указателя на данные */
    EcoMemoryBlock_623E1838Ptr block = (EcoMemoryBlock_623E1838Ptr)
        ((uint64_t)pv - sizeof(EcoMemoryBlock_623E1838));
    
    if (block->is_free) {
        /* Двойное освобождение */
        return;
    }

    block->is_free = 1;
    
    /* Обновляем статистику */
    uint32_t data_size = block->size - sizeof(EcoMemoryBlock_623E1838);
    pCMe->m_iAllocSize -= data_size;
    pCMe->m_iBlockNum--;
    pCMe->m_iTotalFreed += data_size;
    
    /* Объединяем соседние свободные блоки */
    UNITE_BLOCKS(pCMe, block);
}

/*
 *
 * <сводка>
 *   Функция Realloc
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Realloc(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/
    if (pv == NULL) {
        return CEcoMemoryAllocator1Lab_623E1838_Alloc(me, size);
    }
    
    if (size == 0) {
        CEcoMemoryAllocator1Lab_623E1838_Free(me, pv);
        return NULL;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Copy
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Copy(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pvDst, /* in */ voidptr_t pvSrc, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Fill
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Fill(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pvDst, /* in */ char_t Fill, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Compare
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Compare(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv1, /* in */ voidptr_t pv2, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/

    return 0;
}
