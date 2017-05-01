#include "Util.h"
#include "Engenho.h"

#define MAXCLIENTES 5
#define PIPENOME TEXT("\\\\.\\pipe\\teste")
#define PIPE2NOME TEXT("\\\\.\\pipe\\teste2")
#define MUTEX_NAME TEXT("O servidor está a correr?")
#define REGISTRY_KEY TEXT("Software\\TPSO2\\");

DWORD WINAPI ThreadLeituraEscrita(LPVOID param);
bool NovoRegisto(LPCWSTR name);
HANDLE clientes[MAXCLIENTES], hEvent, hThread[MAXCLIENTES];

BOOL fim = FALSE;
Engenho *e;

struct resposta
{
	int ID_Cliente;
	BOOL JogadorLogado;
	BOOL jogoCriado;
	BOOL jogoIniciado;
	BOOL comandoErrado;
	TCHAR frase[256];
	int mapa[9][9];
};

typedef struct {
	TCHAR nome[30];
	TCHAR pass[30];
	HANDLE pipe;
	HANDLE envia;
	HANDLE recebe;

}utilizador;

int numero = 0;
struct resposta res;
utilizador utili[MAXCLIENTES];


int _tmain(int argc, LPTSTR argv[]) {

	HANDLE hpipe;
	OVERLAPPED ovl;

#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	// ciclo for para percorrer os clientes 

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	ovl.hEvent = hEvent;

	_tprintf(TEXT("\t------Espera pelos clientes-------\n\n"));

	//Servidor esta a correr?
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);

	if (hMutex == NULL)
	{
		// no duplicate instances found
		hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
	}
	else
	{
		// a duplicate was found
		_tprintf(TEXT("Já existe uma instância do servidor a correr!\n"));
		return 0;
	}


	// Abrir a DLL

	//HMODULE bib = LoadLibrary(TEXT("DLL_TP"));

	// Realizar verificacões da DLL

	// Criação dos pipes e thread para os clientes

	for (int i = 0; i < MAXCLIENTES && !fim; i++) {

		// duplex


		hpipe = CreateNamedPipe(PIPENOME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, MAXCLIENTES, 256, 256, 1000, NULL);

		// generic 

		clientes[i] = CreateNamedPipe(PIPE2NOME, PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, MAXCLIENTES, 256, 256, 1000, NULL);

		if (clientes[i] == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[Servidor] Erro ao conectar o cliente\n"));
			exit(-1);
		}

		ConnectNamedPipe(clientes[i], &ovl);

		WaitForSingleObject(ovl.hEvent, INFINITE);

		hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadLeituraEscrita, (LPVOID)hpipe, 0, NULL);


	}

	// Esperar por multiplos objetos
	WaitForMultipleObjects(MAXCLIENTES, hThread, FALSE, INFINITE);
	// Libertar a DLL
	//FreeLibrary(bib);


	_tprintf(TEXT("[Servidor] O sistema vai ser desligado\n"));

	return 0;

}

void Autenticacao(LPVOID param) {

	DWORD n;
	HANDLE client = (HANDLE)param;
	TCHAR nome[256];
	TCHAR buf[10000];
	BOOL ret, value;
	LPCTSTR pStr, pStr2;
	int p, existe;

	do {
		existe = 0;

		ret = ReadFile(client, nome, sizeof(nome), &n, NULL);
		nome[n / sizeof(TCHAR)] = '\0';
		pStr2 = nome;

		for (p = 0; p < MAXCLIENTES; p++) {
			if (_tcscmp(nome, utili[p].nome) == 0) {
				existe = 1;
			}
		}
		if (existe == 0) {
			wcscpy_s(utili[numero].nome, nome);
			utili[numero].pipe = client;
			
			if (e->VerificaRegisto(nome) == true) {
				_tprintf(TEXT("[Servidor] O cliente %s ja está registado\n\n"), utili[numero].nome);
			}
			else {
				e->NovoRegisto(nome);
				_tprintf(TEXT("[Servidor] O cliente tem o nome como: %s\n\n"), utili[numero].nome);
			}

			_tprintf(TEXT("[Servidor] Login com sucesso\n\n"));
			_tprintf(TEXT("[Servidor] O nome do cliente é: %s\n\n"), utili[numero].nome);
			p = 1;
			WriteFile(clientes[numero], &p, sizeof(p), 0, NULL);
			numero++;

			_tcscpy_s(buf, 256, (TEXT("[Servidor] Voce esta ligado ao servidor\n\n")));
			WriteFile(clientes[numero], buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
						
		}

	} while(existe != 0);

}

DWORD WINAPI ThreadLeituraEscrita(LPVOID param) {

	DWORD n;
	BOOL value;
	TCHAR buf[256];
	HANDLE client = (HANDLE)param;

	Autenticacao(param);

	do {

		_tcscpy_s(buf, 256, (TEXT("[Servidor] Voce esta ligado")));
		
		for (int i = 0; i < MAXCLIENTES; i++) {
			res.JogadorLogado = true;
			WriteFile(clientes[i], &res, sizeof(struct resposta), &n, NULL);
		}

		ReadFile(client, buf, sizeof(buf), &n, NULL);
		buf[n / sizeof(TCHAR)] = '\0';

		for (int y = 0; y < MAXCLIENTES; y++) {
			if (client == utili[y].pipe) {
				_tprintf(TEXT("[Servidor] - Clinte %s (ID:%d) disse: %s\n\n"), utili[y].nome, y, buf);
			}
		}

	} while (_tcsncmp(buf, TEXT("FIM"), 3));

	return 0;
}

