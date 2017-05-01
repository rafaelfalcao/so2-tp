#pragma once
#ifndef ENGENHO_H
#define ENGENHO_H
#include "Util.h"
#include "Mapa.h"
#define REGISTRY_KEY TEXT("Software\\TPSO2\\");


class Engenho {
	bool jogoCriado;
	bool jogoIniciado;
	Mapa *m;

public:

	Engenho();
	
	void Engenho::NovoRegisto(LPCWSTR nome);
	bool Engenho::VerificaRegisto(LPCWSTR nome);

	int ExecutaComando(string tcomando, string comando);

	bool getJogoCriado();
	bool getJogoIniciado();

};

#endif