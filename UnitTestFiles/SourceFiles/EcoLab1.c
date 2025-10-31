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
#include "IdEcoLab1.h"
#include <time.h>
#include <stdio.h>

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

	/* Получение IEcoCalculatorX и демонстрация его работы */
	result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void **)&pIX);
	if (result != 0 || pIX == 0) {
        /* Освобождение интерфейсов в случае ошибки */
       goto Release;
    }
 
    printf("Addition: %d\n", pIX->pVTbl->Addition(pIX, 3, 5));
    printf("Subtraction: %d\n", pIX->pVTbl->Subtraction(pIX, 10, 5));

	/* Получение IEcoCalculatorY и демонстрация его работы */
	result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, (void **)&pIY);
    printf("Division: %d\n", pIY->pVTbl->Division(pIY, 10, 2));
    printf("Multiplication: %d\n", pIY->pVTbl->Multiplication(pIY, 10, 5));


	/* Тестируем компонент EcoCalculatorE через интерфейсы */
	result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorE, 0, &IID_IEcoCalculatorY, (void **)&pIY);
    if (result == 0 && pIY != 0)
    {
        printf("EcoCalculatorE from IEcoCalculatorY\n");
        /* Получение интерфейса по работе со сложением и вычитанием у компонента "B" */
        result = pIY->pVTbl->QueryInterface(pIY, &IID_IEcoCalculatorX, (void **)&pIX);
        printf("Addition: %d\n", pIX->pVTbl->Addition(pIX, 2, 2));
        printf("Subtraction: %d\n", pIX->pVTbl->Subtraction(pIX, 2, 2));
    }
    /* Тестируем компонент EcoCalculatorD */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorD, 0, &IID_IEcoCalculatorY, (void **)&pIY);
    if (result == 0 && pIY != 0)
    {
        printf("EcoCalculatorD from IEcoCalculatorY\n");
        /* Получение интерфейса по работе со сложением и вычитанием у компонента "A" */
        result = pIY->pVTbl->QueryInterface(pIY, &IID_IEcoCalculatorX, (void **)&pIX);
        printf("Addition: %d\n", pIX->pVTbl->Addition(pIX, 2, 2));
        printf("Subtraction: %d\n", pIX->pVTbl->Subtraction(pIX, 2, 2));
    }

    /* Тестируем компонент EcoCalculatorС */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorC, 0, &IID_IEcoCalculatorY, (void **)&pIY);
    if (result == 0 && pIY != 0)
    {
        printf("EcoCalculatorC from IEcoCalculatorY\n");
        /* Получение интерфейса по работе со сложением и вычитанием у компонента "C" */
        result = pIY->pVTbl->QueryInterface(pIY, &IID_IEcoCalculatorX, (void **)&pIX);
        printf("Addition: %d\n", pIX->pVTbl->Addition(pIX, 2, 2));
        printf("Subtraction: %d\n", pIX->pVTbl->Subtraction(pIX, 2, 2));
    }
    /* Тестируем компонент EcoCalculatorB c поддержкой агрегирования */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorB, 0, &IID_IEcoCalculatorX, (void **)&pIX);
    if (result == 0 || pIX != 0)
    {
        printf("EcoCalculatorB from IEcoCalculatorX\n");
        /* Тестируем компонент EcoCalculatorA*/
        result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorA, 0, &IID_IEcoCalculatorX, (void **)&pIX);
        printf("Addition: %d\n", pIX->pVTbl->Addition(pIX, 2, 2));
        printf("Subtraction: %d\n", pIX->pVTbl->Subtraction(pIX, 2, 2));

	}

	result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorA, 0, &IID_IEcoCalculatorX, (void **)&pIX);
    if (result != 0 || pIX == 0)
    {
        printf("EcoCalculatorA from IEcoCalculatorX\n");
        printf("Addition: %d\n", pIX->pVTbl->Addition(pIX, 2, 2));
        printf("Subtraction: %d\n", pIX->pVTbl->Subtraction(pIX, 2, 2));
	}
    for (i = 1; i <= 10; i++)
    {
        arrayLength = 10;
		/* Генерация массива */
        result = pIEcoLab1->pVTbl->PseudoGenerator(pIEcoLab1, arrayLength, i, &generated_arr);
        if (result != ERR_ECO_SUCCESES)
        {
            pIMem->pVTbl->Free(pIMem, generated_arr);
            goto Release;
        }

		/* Старт алгоритма */
        start = clock();
		printf("Before sort:");
		print_array_basic(generated_arr, arrayLength);
		result = pIEcoLab1->pVTbl->InsertionSort(pIEcoLab1, generated_arr, arrayLength, &sorted_arr);
		printf("After sort:");
		print_array_basic(sorted_arr, arrayLength);
		/* Конец алгоритма */
        end = clock();
        if (result != ERR_ECO_SUCCESES)
        {
            pIMem->pVTbl->Free(pIMem, sorted_arr);
            goto Release;
        }
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("\n\n");
        //printf("InsertionSort (length = %u) time: %f seconds\n", arrayLength, cpu_time_used);

		/* Освобождение выделенной памяти массивов */
		pIMem->pVTbl->Free(pIMem, generated_arr);
		generated_arr = 0;
		pIMem->pVTbl->Free(pIMem, sorted_arr);
		sorted_arr = 0;
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