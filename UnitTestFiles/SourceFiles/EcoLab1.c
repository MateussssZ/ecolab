/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoLab1
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
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "C:\Users\01kri\OneDrive\Документы\Visual Studio 2010\Projects\Eco.Lab1\SharedFiles\IEcoLab1Events.h"
#include "IdEcoLab1.h"
#include <time.h>
#include <stdio.h>

#include "C:\Users\01kri\OneDrive\Документы\Visual Studio 2010\Projects\Eco.Lab1\UnitTestFiles\HeaderFiles\CEcoLab1Sink.h"
#include "IEcoConnectionPointContainer.h"

#include "C:\Users\01kri\Downloads\Eco.CalculatorC\SharedFiles\IEcoCalculatorX.h"
#include "C:\Users\01kri\Downloads\Eco.CalculatorD\SharedFiles\IEcoCalculatorY.h"

#include "C:\Users\01kri\Downloads\Eco.CalculatorA\SharedFiles\IdEcoCalculatorA.h"
#include "C:\Users\01kri\Downloads\Eco.CalculatorD\SharedFiles\IdEcoCalculatorD.h"
#include "C:\Users\01kri\Downloads\Eco.CalculatorB\SharedFiles\IdEcoCalculatorB.h"
#include "C:\Users\01kri\Downloads\Eco.CalculatorE\SharedFiles\IdEcoCalculatorE.h"
#include "C:\Users\01kri\Downloads\Eco.CalculatorC\SharedFiles\IdEcoCalculatorC.h"

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

int __cdecl compare_int(const void* a, const void* b) {
    return (*((int32_t*)a) - *((int32_t*)b));
}

void __cdecl print_array_basic(int32_t* arr, uint32_t length) {
	uint32_t i = 0;
    if (arr == NULL || length == 0) {
        printf("Array is empty or NULL\n");
        return;
    }
    for (i = 0; i < length; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    /* Указатель на тестируемый интерфейс */
    IEcoLab1* pIEcoLab1 = 0;
	IEcoCalculatorX *pIX = 0;
    IEcoCalculatorY *pIY = 0;

	 /* Указатель на интерфейс контейнера точек подключения */
    IEcoConnectionPointContainer* pICPC = 0;
    /* Указатель на интерфейс точки подключения */
    IEcoConnectionPoint* pICP = 0;
    /* Указатель на обратный интерфейс */
    IEcoLab1Events* pIEcoLab1Sink = 0;
    IEcoUnknown* pISinkUnk = 0;
    uint32_t cAdvise = 0;

	int32_t *sorted_arr;
    int32_t *generated_arr;
    uint32_t arrayLength;
    int32_t i;
    clock_t start, end;
    double cpu_time_used;

    /* Проверка и создание системного интрефейса */
    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
        if (result != 0 && pISys == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
            goto Release;
        }
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
#ifdef ECO_LIB
    /* Регистрация статического компонента для работы со списком */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0 ) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

	 result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoList1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_53884AFC93C448ECAA929C8D3A562281);
    if (result != 0 ) {
        /* Освобождение в случае ошибки */
        goto Release;
    }


	result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorB, (IEcoUnknown*)GetIEcoComponentFactoryPtr_AE202E543CE54550899603BD70C62565);
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorA, (IEcoUnknown*)GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E);
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
	result = g_pIBus->pVTbl->RegisterComponent(g_pIBus, &CID_EcoCalculatorC, (IEcoUnknown*)GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E);
    if (result != 0 ) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorD, (IEcoUnknown*)GetIEcoComponentFactoryPtr_3A8E44677E82475CB4A3719ED8397E61);
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorE, (IEcoUnknown*)GetIEcoComponentFactoryPtr_872FEF1DE3314B87AD44D1E7C232C2F0);
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
#endif
    /* Получение интерфейса управления памятью */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    /* Проверка */
    if (result != 0 || pIMem == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
        goto Release;
    }

    /* Получение тестируемого интерфейса */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**) &pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
        /* Освобождение интерфейсов в случае ошибки */
        goto Release;
    }
	/* Проверка поддержки подключений обратного интерфейса */
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoConnectionPointContainer, (void **)&pICPC);
    if (result != 0 || pICPC == 0) {
        /* Освобождение интерфейсов в случае ошибки */
        goto Release;
    }

    /* Запрос на получения интерфейса точки подключения */
    result = pICPC->pVTbl->FindConnectionPoint(pICPC, &IID_IEcoLab1Events, &pICP);
    if (result != 0 || pICP == 0) {
        /* Освобождение интерфейсов в случае ошибки */
        goto Release;
    }

	pICPC->pVTbl->Release(pICPC);

    /* Создание экземпляра обратного интерфейса */
    result = createCEcoLab1Sink(pIMem, (IEcoLab1Events**)&pIEcoLab1Sink);

    if (pIEcoLab1Sink != 0) {
        result = pIEcoLab1Sink->pVTbl->QueryInterface(pIEcoLab1Sink, &IID_IEcoUnknown,(void **)&pISinkUnk);
        if (result != 0 || pISinkUnk == 0) {
            /* Освобождение интерфейсов в случае ошибки */
            goto Release;
        }
        /* Подключение */
        result = pICP->pVTbl->Advise(pICP, pISinkUnk, &cAdvise);
        /* Проверка */
        if (result == 0 && cAdvise == 1) {
            printf("Succesfully sinked");
        }
        /* Освобождение интерфейса */
        pISinkUnk->pVTbl->Release(pISinkUnk);
    }

	printf("\nTEST 1: Use ecolab1 method(PseudoGenerator) which contains fire methods with calling single connectable object method\n");
    arrayLength = 10;
	/* Генерация массива */
	printf("Before sort:");
    result = pIEcoLab1->pVTbl->PseudoGenerator(pIEcoLab1, arrayLength, i, &generated_arr);
    if (result != ERR_ECO_SUCCESES)
    {
		pIMem->pVTbl->Free(pIMem, generated_arr);
        goto Release;
    }

	printf("\nTEST 2: Use ecolab1 method(InsertionSort) which contains fire methods with calling multiple connectable object methods\n");
	/* Старт алгоритма */
	result = pIEcoLab1->pVTbl->InsertionSort(pIEcoLab1, generated_arr, arrayLength, &sorted_arr);
	/* Конец алгоритма */
    if (result != ERR_ECO_SUCCESES)
    {
		pIMem->pVTbl->Free(pIMem, sorted_arr);
		goto Release;
    }
	printf("\n\n");


    /* Освобождение блока памяти */
    pIMem->pVTbl->Free(pIMem, generated_arr);
	generated_arr = 0;
	pIMem->pVTbl->Free(pIMem, sorted_arr);
	sorted_arr = 0;

    
    if (pIEcoLab1Sink != 0) {
        /* Отключение */
        result = pICP->pVTbl->Unadvise(pICP, cAdvise);
        pIEcoLab1Sink->pVTbl->Release(pIEcoLab1Sink);
        pICP->pVTbl->Release(pICP);
    }


Release:
	/* Освобождение выделенной памяти массивов */
	if (generated_arr != 0) {
		pIMem->pVTbl->Free(pIMem, generated_arr);
		generated_arr = 0;
	}
	if (sorted_arr != 0) {
		pIMem->pVTbl->Free(pIMem, sorted_arr);
		sorted_arr = 0;
	}

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

    /* Освобождение тестируемого интерфейса */
    if (pIEcoLab1 != 0) {
        pIEcoLab1->pVTbl->Release(pIEcoLab1);
    }


    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}