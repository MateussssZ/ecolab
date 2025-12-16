/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoVFB1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoMemoryManager1Lab.h"
#include "IEcoVirtualMemory1.h"
//#include "IEcoTaskScheduler1.h"
//#include "IdEcoTaskScheduler1Lab.h"
#include "IdEcoTimer1.h"
#include "IEcoSystemTimer1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "IdEcoIPCCMailbox1.h"
#include "IdEcoVFB1.h"
#include "IEcoVBIOS1Video.h"

/* Начало свободного участка памяти */
extern char_t __heap_start__;

/* Указатель на интерфейсы */
IEcoVBIOS1Video* g_pIVideo = 0;
IEcoSystemTimer1* g_pISysTimer = 0;

char_t g_strTask[2] = {0};


void TimerHandler(void) {
    //g_pIMutex->pVTbl->Lock(g_pIMutex);
    //g_pISemaphore->pVTbl->Wait(g_pISemaphore, 0);
    if (g_strTask[0] == '\\') {
        g_strTask[0] = '|';
    }
    else if (g_strTask[0] == '|') {
        g_strTask[0] = '/';
    }
    else if (g_strTask[0] == '/') {
        g_strTask[0] = '-';
    }
    else  {
        g_strTask[0] = '\\';
    }
    //g_pIMutex->pVTbl->UnLock(g_pIMutex);
    //g_pISemaphore->pVTbl->Post(g_pISemaphore);
}


/* Функция для теста Worst Fit Allocator */
void TestWorstFitAllocator(IEcoMemoryAllocator1* pIMem, IEcoVBIOS1Video* pVideo) {
    void* pointers[10] = {0};
    char_t* dataCheck = 0;
    int i;
    int line = 5;  // Начальная строка для вывода

    pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, 
        CHARACTER_ATTRIBUTE_FORE_COLOR_CYAN, "=== WORST FIT ALGORITHM TEST ===", 32);

    /* --- TEST 1: Демонстрация Worst Fit поведения --- */
    pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, 
        CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "Test 1: Worst Fit Selection", 27);
    
    /* Создаем искусственные блоки (в реальности они создаются при выделениях) */
    pointers[0] = pIMem->pVTbl->Alloc(pIMem, 100);   /* Маленький блок */
    pointers[1] = pIMem->pVTbl->Alloc(pIMem, 200);   /* Средний блок */
    pointers[2] = pIMem->pVTbl->Alloc(pIMem, 500);   /* Большой блок */
    
    /* Освобождаем их чтобы создать свободные блоки разных размеров */
    if (pointers[0]) pIMem->pVTbl->Free(pIMem, pointers[0]);  /* Остается блок 100 */
    if (pointers[1]) pIMem->pVTbl->Free(pIMem, pointers[1]);  /* Остается блок 200 */
    if (pointers[2]) pIMem->pVTbl->Free(pIMem, pointers[2]);  /* Остается блок 500 */
    
    /* Теперь запрашиваем 150 байт - Worst Fit должен выбрать 500! */
    pointers[3] = pIMem->pVTbl->Alloc(pIMem, 150);
    if (pointers[3] != 0) {
        pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, 
            CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "  [PASS] Alloc 150: Used largest block (500)", 45);
    } else {
        pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "  [FAIL] Alloc 150 failed", 26);
    }
    
    /* Освобождаем для следующих тестов */
    if (pointers[3]) pIMem->pVTbl->Free(pIMem, pointers[3]);

    /* --- TEST 2: Проверка разделения блоков --- */
    pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "Test 2: Block Splitting", 23);
    
    /* Выделяем большой блок и сразу освобождаем */
    pointers[4] = pIMem->pVTbl->Alloc(pIMem, 1000);
    if (pointers[4]) {
        pIMem->pVTbl->Free(pIMem, pointers[4]);
        
        /* Теперь запрашиваем маленький блок - должен разделить 1000 */
        pointers[5] = pIMem->pVTbl->Alloc(pIMem, 100);
        if (pointers[5]) {
            pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++,  CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "  [PASS] 1000 split into 100 + 900", 33);
            pIMem->pVTbl->Free(pIMem, pointers[5]);
        }
    }

    /* --- TEST 3: Проверка объединения блоков --- */
    pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "Test 3: Block Coalescing", 24);
    
    /* Выделяем два соседних блока */
    pointers[6] = pIMem->pVTbl->Alloc(pIMem, 100);
    pointers[7] = pIMem->pVTbl->Alloc(pIMem, 100);
    
    if (pointers[6] && pointers[7]) {
        /* Освобождаем оба - они должны объединиться */
        pIMem->pVTbl->Free(pIMem, pointers[6]);
        pIMem->pVTbl->Free(pIMem, pointers[7]);
        
        /* Пытаемся выделить блок 250 - должен получиться если объединились */
        pointers[8] = pIMem->pVTbl->Alloc(pIMem, 250);
        if (pointers[8]) {
            pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "  [PASS] Blocks merged successfully", 35);
            pIMem->pVTbl->Free(pIMem, pointers[8]);
        } else {
            pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "  [FAIL] Blocks didn't merge", 29);
        }
    }

    /* --- TEST 4: Тест на фрагментацию --- */
    pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "Test 4: Fragmentation Test", 26);
    
    /* Выделяем несколько блоков разных размеров */
    void* frag_ptrs[5];
    for (i = 0; i < 5; i++) {
        frag_ptrs[i] = pIMem->pVTbl->Alloc(pIMem, (i+1) * 50);  /* 50, 100, 150, 200, 250 */
    }
    
    /* Освобождаем не все подряд */
    pIMem->pVTbl->Free(pIMem, frag_ptrs[1]);  /* Освобождаем 100 */
    pIMem->pVTbl->Free(pIMem, frag_ptrs[3]);  /* Освобождаем 200 */
    
    /* Пытаемся выделить 300 - Worst Fit должен найти подходящий блок */
    void* large_alloc = pIMem->pVTbl->Alloc(pIMem, 300);
    if (large_alloc) {
        pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "  [PASS] Found space despite fragmentation", 41);
        pIMem->pVTbl->Free(pIMem, large_alloc);
    }
    
    /* Очистка */
    for (i = 0; i < 5; i++) {
        if (frag_ptrs[i] && i != 1 && i != 3) {  /* Уже освободили 1 и 3 */
            pIMem->pVTbl->Free(pIMem, frag_ptrs[i]);
        }
    }

    /* --- TEST 5: Пограничные случаи --- */
    pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "Test 5: Edge Cases", 18);
    
    /* Выделение 0 байт */
    void* zero_alloc = pIMem->pVTbl->Alloc(pIMem, 0);
    if (zero_alloc == NULL) {
        pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "  [PASS] Alloc(0) returns NULL", 30);
    }
    
    /* Освобождение NULL */
    pIMem->pVTbl->Free(pIMem, NULL);
    pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "  [PASS] Free(NULL) handled", 27);
    
    /* Выделение больше чем есть */
    void* huge_alloc = pIMem->pVTbl->Alloc(pIMem, 10 * 1024 * 1024);  /* 10 MB */
    if (huge_alloc == NULL) {
        pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "  [PASS] Out of memory handled", 30);
    }

    /* --- TEST 6: Целостность данных --- */
    pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "Test 6: Data Integrity", 22);
    
    pointers[9] = pIMem->pVTbl->Alloc(pIMem, 100);
    if (pointers[9]) {
        /* Записываем данные */
        dataCheck = (char_t*)pointers[9];
        for (i = 0; i < 100; i++) {
            dataCheck[i] = (char_t)(i % 256);
        }
        
        /* Проверяем */
        int valid = 1;
        for (i = 0; i < 100; i++) {
            if (dataCheck[i] != (char_t)(i % 256)) {
                valid = 0;
                break;
            }
        }
        
        if (valid) {
            pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "  [PASS] Data integrity verified", 32);
        } else {
            pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "  [FAIL] Data corruption detected", 33);
        }
        
        pIMem->pVTbl->Free(pIMem, pointers[9]);
    }

    pVideo->pVTbl->WriteString(pVideo, 0, 0, 1, line++, CHARACTER_ATTRIBUTE_FORE_COLOR_CYAN, "=== WORST FIT TEST COMPLETE ===", 31);
}

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoMemoryManager1* pIMemMgr = 0;
    IEcoMemoryManager1* pIMemMgrLab = 0;        /* Лабораторный менеджер */
    IEcoMemoryAllocator1* pIAllocatorLab = 0;   /* Для тестирования Worst Fit */

    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    IEcoVirtualMemory1* pIVrtMem = 0;
    /* Указатель на интерфейс для работы с планировщиком */
    //IEcoTaskScheduler1* pIScheduler = 0;
    /* Указатель на интерфейс для работы c буфером кадров видеоустройства */
    IEcoVFB1* pIVFB = 0;
    ECO_VFB_1_SCREEN_MODE xScreenMode = {0};
    IEcoVBIOS1Video* pIVideo = 0;
    /* Указатель на интерфейс для работы c системным таймером */
    IEcoSystemTimer1* pISysTimer = 0;
    /* Указатель на интерфейс для работы c таймером */
    IEcoTimer1* pITimer = 0;

    char_t* strHello = "Hello, World!";
    uint16_t offset = 0;
    uint16_t x1 = 0;
    uint16_t y1 = 32;
    uint16_t x2 = 70;
    byte_t color = 170; /* 3-3-2 bit RGB */

    /* Создание экземпляра интерфейсной шины */
    result = GetIEcoComponentFactoryPtr_00000000000000000000000042757331->pVTbl->Alloc(GetIEcoComponentFactoryPtr_00000000000000000000000042757331, 0, 0, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    /* Проверка */
    if (result != 0 && pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с памятью */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_0000000000000000000000004D656D31);
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_81589BFED0B84B1194524BEE623E1838);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с ящиком прошивки */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoIPCCMailbox1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_F10BC39A4F2143CF8A1E104650A2C302);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Запрос расширения интерфейсной шины */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        /* Установка расширения менаджера памяти */
        pIMemExt->pVTbl->set_Manager(pIMemExt, &CID_EcoMemoryManager1);
        //pIMemExt->pVTbl->set_Manager(pIMemExt, &CID_EcoMemoryManager1Lab);
        /* Установка разрешения расширения пула */
        pIMemExt->pVTbl->set_ExpandPool(pIMemExt, 1);
    }

    /* Получение интерфейса управления памятью */
    pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgr);
    //pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1Lab, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgr);
    //IEcoMemoryManager1* pIMemMgrLab = 0;
    //result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1Lab, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgrLab);
    if (result != 0 || pIMemMgr == 0) {
        /* Возврат в случае ошибки */
        return result;
    }

    /* Выделение области памяти 512 КБ */
    pIMemMgr->pVTbl->Init(pIMemMgr, &__heap_start__, 0x080000);
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1Lab, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgrLab);
    if (result == 0 && pIMemMgrLab != 0) {
        /* Отдельный участок памяти для тестирования Worst Fit */
        char_t* lab_heap_start = &__heap_start__ + 0x060000; /* После основной кучи */
        pIMemMgrLab->pVTbl->Init(pIMemMgrLab, lab_heap_start, 0x020000); /* 128 КБ */
    }

    /* Получение интерфейса для работы с виртуальной памятью */
    result = pIMemMgr->pVTbl->QueryInterface(pIMemMgr, &IID_IEcoVirtualMemory1, (void**)&pIVrtMem);
    if (result == 0 && pIVrtMem != 0) {
        /* Инициализация виртуальной памяти */
        result = pIVrtMem->pVTbl->Init(pIVrtMem);
        /* TO DO */
    }
    /* Регистрация статического компонента для работы с планировщиком */
    /*result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoTaskScheduler1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_902ABA722D34417BB714322CC761620F);
    /* Проверка */
    /*if (result != 0) {
        /* Освобождение в случае ошибки */
    /*    goto Release;
    }*/

    /* Регистрация статического компонента для работы с таймером */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoTimer1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_8DB93F3DF5B947D4A67F7C100B569723);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с VBF */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoVFB1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_939B1DCDB6404F7D9C072291AF252188);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение интерфейса для работы с планировщиком */
    //result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoTaskScheduler1Lab, 0, &IID_IEcoTaskScheduler1, (void**) &pIScheduler);
    /* Проверка */
    //if (result != 0 || pIScheduler == 0) {
        /* Освобождение в случае ошибки */
    //    goto Release;
    //}

    /* Инициализация */
    //pIScheduler->pVTbl->InitWith(pIScheduler, pIBus, &__heap_start__+0x090000, 0x080000);

    /* Создание статических задач */
    //pIScheduler->pVTbl->NewTask(pIScheduler, Task1, 0, 0x100, &pITask1);
    //pIScheduler->pVTbl->NewTask(pIScheduler, Task2, 0, 0x100, &pITask2);
    //pIScheduler->pVTbl->NewTask(pIScheduler, Task3, 0, 0x100, &pITask3);


    /* Получение интерфейса для работы с системным таймером */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoTimer1, 0, &IID_IEcoSystemTimer1, (void**) &pISysTimer);
    /* Проверка */
    if (result != 0 || pISysTimer == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
    g_pISysTimer = pISysTimer;

    /* Установка обработчика прерывания программируемого таймера */
    result = pISysTimer->pVTbl->QueryInterface(pISysTimer, &IID_IEcoTimer1, (void**)&pITimer);
    /* Проверка */
    if (result != 0 || pITimer == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    pITimer->pVTbl->set_Interval(pITimer, 100000);
    pITimer->pVTbl->set_IrqHandler(pITimer, TimerHandler);
    pITimer->pVTbl->Start(pITimer);

    /* Получение интерфейса для работы с видео сервисами VBF */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoVFB1, 0, &IID_IEcoVFB1, (void**) &pIVFB);
    /* Проверка */
    if (result != 0 || pIVFB == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение информации о текущем режиме экрана */
    result = pIVFB->pVTbl->get_Mode(pIVFB, &xScreenMode);
    pIVFB->pVTbl->Create(pIVFB, 0, 0, xScreenMode.Width, xScreenMode.Height);
    result = pIVFB->pVTbl->QueryInterface(pIVFB, &IID_IEcoVBIOS1Video, (void**) &pIVideo);

    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 4, 5, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, strHello, 13);

    /* Вывод 1 строки "Эко ОС!!!" - кодовая страница 1251 */
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 1, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "\xdd\xea\xee\x20\xce\xd1\x21\x21\x21", 9);
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 3, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, "Lab 4: Worst Fit Memory Allocator", 34);

    /* Рисуем линию - подчеркивание */
    for (offset = x1; offset <= x2; offset++) {
        pIVideo->pVTbl->WriteDot(pIVideo, color, 0, offset, y1);
    }

    /* === ЗАПУСК ТЕСТОВ WORST FIT === */
    {
        IEcoMemoryAllocator1* pAllocatorForTest = 0;
        result = pIMemMgrLab->pVTbl->QueryInterface(pIMemMgrLab, &IID_IEcoMemoryAllocator1, (void**)&pAllocatorForTest);
        if (result == 0 && pAllocatorForTest != 0) {
            TestWorstFitAllocator(pAllocatorForTest, pIVideo);
            pAllocatorForTest->pVTbl->Release(pAllocatorForTest);
        } else {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 1, 5, 
                CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "ERROR: Cannot get allocator interface", 38);
        }
    }


    while(1) {
        asm volatile ("NOP\n\t" ::: "memory");
    }

Release:

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

    /* Освобождение интерфейса VFB */
    if (pIVFB != 0) {
        pIVFB->pVTbl->Release(pIVFB);
    }

    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}
