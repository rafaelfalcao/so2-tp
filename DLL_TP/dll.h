#pragma once
#include<windows.h>
#include<tchar.h>

#ifdef AULA2_DLL_EXPORTS // nome do projeto _EXPORTS
//estou dentro da DLL

#define DLL_IMP_API __declspec(dllexport)

#else
//estou no executavel

#define DLL_IMP_API __declspec(dllimport)

#endif


extern "C" {
	// colocar prototipos das funções aqui
	// ao coloca las aqui estas irão poder acedidas 
}