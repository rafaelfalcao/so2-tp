#include <iostream>
#include <string>
#include <tchar.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>

using namespace std;

HANDLE hPipe;
HANDLE hPipe2;

DWORD WINAPI Thread(LPVOID param);
void Autenticacao(LPVOID param);
void Envia(TCHAR &Comando);


#define MAXCLIENTES 5

//colocar estrutura

#define PIPENOME TEXT("\\\\.\\pipe\\teste")
#define PIPENOME2 TEXT("\\\\.\\pipe\\teste2")


struct resposta
{
	int ID_Cliente;
	int vida;
	int pontuacao;
	BOOL JogadorLogado;
	BOOL jogoCriado;
	BOOL jogoIniciado;
	BOOL comandoErrado;
	TCHAR frase[256];
	TCHAR comandoErr[256];
	char nome[50];
	int mapa[13][13];
	int numero;
};


struct resposta res;
static int ID_Cliente = 0;
TCHAR Comando[256];
TCHAR Nome[25];
DWORD n;

int _tmain(int argc, LPTSTR argv[]) {
	HANDLE hThread;
	DWORD n;


#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	if (!WaitNamedPipe(PIPENOME, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[Cliente] Conectando... '%s'\n"), PIPENOME);
	}
	
	// criação dos dois PIPES para interligar com o servidor

	hPipe = CreateFile(PIPENOME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	hPipe2 = CreateFile(PIPENOME2, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hPipe == NULL || hPipe2 == NULL) {
		_tprintf(TEXT("[Cliente] Nao e possivel conetar o servidor %s, tente mais tarde.\n"), PIPENOME);
		exit(-1);
	}

	// Realizar a autenticação

	Autenticacao(hPipe2);

	// Criação da thread para o cliente

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread, (LPVOID)hPipe2, 0, NULL);

	while (1) {
		fflush(stdin);
		_fgetts(Comando, 256, stdin);
		Comando[_tcslen(Comando) - 1] = '\0';
		res.ID_Cliente = ID_Cliente;

		Envia(*Comando);
	}
	
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hPipe);
	return 0;
}

void Autenticacao(LPVOID param) {

	HANDLE hpipelocal = (HANDLE)param;
	DWORD n;
	BOOL ret;
	int i = 0, resp;
	TCHAR buf[256];
	TCHAR frase[256];

	//autenticação do utilizador - nome

	do {
		_tprintf(TEXT("[Autenticação - Insira o nome]: "));
		fflush(stdin);
		_fgetts(Comando, 256, stdin);
		Comando[_tcslen(Comando) - 1] = '\0';
		wcscpy_s(Nome, Comando);
		res.ID_Cliente = ID_Cliente;
		WriteFile(hPipe, Comando, _tcslen(Comando) * sizeof(TCHAR), &n, NULL);

		ReadFile(hpipelocal, &resp, sizeof(resp), &n, NULL);
	} while (resp != 1);

}

DWORD WINAPI Thread(LPVOID param) {

	HANDLE hpipelocal = (HANDLE)param;
	BOOL ret;
	int i = 0, flag = 0, flag2 = 0;
	TCHAR buf[10000];
	TCHAR frase[256];

	//_tprintf(TEXT("%s"), buf);
	_tprintf(TEXT("[Cliente %s] Seja bem vindo ao jogo\n"), Nome);

	while (1) {

		ret = ReadFile(hpipelocal, &res, sizeof(struct resposta), &n, NULL);
		
		if (!ret || !n) {
			_tprintf(TEXT("[Cliente -%s] O servidor desligou-se\n\n"), Nome);
			break;
		}
		
		//if (res.JogadorLogado == true) {
			_tprintf(TEXT("[%s - Comandos]: "), Nome);
		//}
	}

	return 0;

}

void Envia(TCHAR &Comando) {

	WriteFile(hPipe, &Comando, _tcslen(&Comando) * sizeof(TCHAR), &n, NULL);
}
