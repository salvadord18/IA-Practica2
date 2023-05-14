#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>

//** Devuelve si una ubicación en el mapa es transitable para el agente */
bool CasillaTransitable(const ubicacion &x, const vector<vector<unsigned char>> &mapa)
{
	return (mapa[x.f][x.c] != 'P' and mapa[x.f][x.c] != 'M');
}

//** Devuelve la ubicación siguiente según el avance del agente */
ubicacion NextCasilla(const ubicacion &pos)
{
	ubicacion next = pos;
	switch (pos.brujula)
	{
	case norte:
		next.f = pos.f - 1;
		break;
	case noreste:
		next.f = pos.f - 1;
		next.c = pos.c + 1;
		break;
	case este:
		next.c = pos.c + 1;
		break;
	case sureste:
		next.f = pos.f + 1;
		next.c = pos.c + 1;
		break;
	case sur:
		next.f = pos.f + 1;
		break;
	case suroeste:
		next.f = pos.f + 1;
		next.c = pos.c - 1;
		break;
	case oeste:
		next.c = pos.c - 1;
		break;
	case noroeste:
		next.f = pos.f - 1;
		next.c = pos.c - 1;
		break;
	default:
		break;
	}

	return next;
}

/** Devuelve el estado que se genera si el agente puede avanzar.
 * Si no puede avanzar, se devuelve como salida el mismo estado de entrada.
 */
stateN0 apply(const Action &a, const stateN0 &st, const vector<vector<unsigned char>> &mapa)
{
	stateN0 st_result = st;
	ubicacion sig_ubicacion;
	switch (a)
	{
	case actFORWARD: // si proxima casilla es transitable y no está ocupada por el sonámbulo
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and !(sig_ubicacion.f == st.sonambulo.f and sig_ubicacion.c == st.sonambulo.c))
		{
			st_result.jugador = sig_ubicacion;
		}
		break;
	case actTURN_L:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 6) % 8);
		break;
	case actTURN_R:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 2) % 8);
		break;
	}
	return st_result;
}

/** pone a cero todos los elementos de una matriz */
void AnularMatriz(vector<vector<unsigned char>> &matriz)
{
	for (int i = 0; i < matriz.size(); i++)
	{
		for (int j = 0; j < matriz[i].size(); j++)
		{
			matriz[i][j] = 0;
		}
	}
}

//** Permite pintar sobre el mapa del simulador el plan partiendo desde el estado st */
void ComportamientoJugador::VisualizaPlan(const stateN0 &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	stateN0 cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		switch (*it)
		{
		case actFORWARD:
			cst.jugador = NextCasilla(cst.jugador);
			mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
			break;
		case actTURN_R:
			cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 2) % 8);
			break;
		case actTURN_L:
			cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 6) % 8);
			break;
		case actSON_FORWARD:
			cst.sonambulo = NextCasilla(cst.sonambulo);
			mapaConPlan[cst.sonambulo.f][cst.sonambulo.c] = 2;
			break;
		case actSON_TURN_SR:
			cst.sonambulo.brujula = (Orientacion)((cst.sonambulo.brujula + 1) % 8);
			break;
		case actSON_TURN_SL:
			cst.sonambulo.brujula = (Orientacion)((cst.sonambulo.brujula + 7) % 8);
			break;
		}
		it++;
	}
}

/** Versión segunda de la busqueda en anchura: ahora sí devuleve un plan */
list<Action> AnchuraSoloJugador(const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa)
{

	nodeN0 current_node;
	list<nodeN0> frontier;
	set<nodeN0> explored;
	list<Action> plan;
	current_node.st = inicio;
	bool SolutionFound = (current_node.st.jugador.f == final.f and current_node.st.jugador.c == final.c);
	frontier.push_back(current_node);

	while (!frontier.empty() and !SolutionFound)
	{
		frontier.pop_front();
		explored.insert(current_node);

		// Generar hijo actFORWARD
		nodeN0 child_forward = current_node;
		child_forward.st = apply(actFORWARD, current_node.st, mapa);
		if (child_forward.st.jugador.f == final.f and child_forward.st.jugador.c == final.c)
		{
			child_forward.secuencia.push_back(actFORWARD);
			current_node = child_forward;
			SolutionFound = true;
		}
		else if (explored.find(child_forward) == explored.end())
		{
			child_forward.secuencia.push_back(actFORWARD);
			frontier.push_back(child_forward);
		}

		if (!SolutionFound)
		{
			// Generar hijo actTURN_L
			nodeN0 child_turnl = current_node;
			child_turnl.st = apply(actTURN_L, current_node.st, mapa);
			if (explored.find(child_turnl) == explored.end())
			{
				child_turnl.secuencia.push_back(actTURN_L);
				frontier.push_back(child_turnl);
			}
			// Generar hijo actTURN_R
			nodeN0 child_turnr = current_node;
			child_turnr.st = apply(actTURN_R, current_node.st, mapa);
			if (explored.find(child_turnr) == explored.end())
			{
				child_turnr.secuencia.push_back(actTURN_R);
				frontier.push_back(child_turnr);
			}
		}

		if (!SolutionFound and !frontier.empty())
		{
			current_node = frontier.front();
			while (!frontier.empty() and explored.find(current_node) != explored.end())
			{
				frontier.pop_front();
				if (!frontier.empty())
				{
					current_node = frontier.front();
				}
			}
		}
	}

	if (SolutionFound)
	{
		plan = current_node.secuencia;
		return plan;
	}
}

bool miraSonambulo(const stateN1 &st)
{
	ubicacion jugador = st.jugador;
	ubicacion sonambulo = st.sonambulo;
	stateN1 cst = st;

	Orientacion orientacionJugador = jugador.brujula;
	int fil = jugador.f;
	int col = jugador.c;

	bool miraSonambulo = false;

	switch (orientacionJugador)
	{
	case norte:
		if ((sonambulo.c == col) and ((fil - sonambulo.f) <= 3) and ((fil - sonambulo.f) > 0))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.c == col - 1) or (sonambulo.c == col + 1)) and ((fil - sonambulo.f) <= 3) and ((fil - sonambulo.f) >= 1))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.c == col - 2) or (sonambulo.c == col + 2)) and ((fil - sonambulo.f) <= 3) and ((fil - sonambulo.f) >= 2))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.c == col - 3) or (sonambulo.c == col + 3)) and ((fil - sonambulo.f) == 3))
		{
			miraSonambulo = true;
		}
		else
		{
			miraSonambulo = false;
		}
		break;
	case este:
		if ((sonambulo.f == fil) and ((sonambulo.c - col) <= 3) and ((sonambulo.c - col) > 0))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.f == fil - 1) or (sonambulo.f == fil + 1)) and ((sonambulo.c - col) <= 3) and ((sonambulo.c - col) >= 1))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.f == fil - 2) or (sonambulo.f == fil + 2)) and ((sonambulo.c - col) <= 3) and ((sonambulo.c - col) >= 2))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.f == fil - 3) or (sonambulo.f == fil + 3)) and ((sonambulo.c - col) == 3))
		{
			miraSonambulo = true;
		}
		else
		{
			miraSonambulo = false;
		}
		break;
	case sur:
		if ((sonambulo.c == col) and ((sonambulo.f - fil) <= 3) and ((sonambulo.f - fil) > 0))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.c == col - 1) or (sonambulo.c == col + 1)) and ((sonambulo.f - fil) <= 3) and ((sonambulo.f - fil) >= 1))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.c == col - 2) or (sonambulo.c == col + 2)) and ((sonambulo.f - fil) <= 3) and ((sonambulo.f - fil) >= 2))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.c == col - 3) or (sonambulo.c == col + 3)) and ((sonambulo.f - fil) == 3))
		{
			miraSonambulo = true;
		}
		else
		{
			miraSonambulo = false;
		}
		break;
	case oeste:
		if ((sonambulo.f == fil) and ((col - sonambulo.c) <= 3) and ((col - sonambulo.c) > 0))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.f == fil - 1) or (sonambulo.f == fil + 1)) and ((col - sonambulo.c) <= 3) and ((col - sonambulo.c) >= 1))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.f == fil - 2) or (sonambulo.f == fil + 2)) and ((col - sonambulo.c) <= 3) and ((col - sonambulo.c) >= 2))
		{
			miraSonambulo = true;
		}
		else if (((sonambulo.f == fil - 3) or (sonambulo.f == fil + 3)) and ((col - sonambulo.c) == 3))
		{
			miraSonambulo = true;
		}
		else
		{
			miraSonambulo = false;
		}
		break;
	default:
		break;
	}
	return miraSonambulo;
}

stateN1 apply(const Action &a, const stateN1 &st, const vector<vector<unsigned char>> &mapa)
{
	stateN1 st_result = st;
	ubicacion sig_ubicacion;
	switch (a)
	{
	case actFORWARD: // si proxima casilla es transitable y no está ocupada por el sonámbulo
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and !(sig_ubicacion.f == st.sonambulo.f and sig_ubicacion.c == st.sonambulo.c))
		{
			st_result.jugador = sig_ubicacion;
		}
		break;
	case actTURN_L:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 6) % 8);
		break;
	case actTURN_R:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 2) % 8);
		break;
	case actSON_FORWARD:
		sig_ubicacion = NextCasilla(st.sonambulo);
		if (CasillaTransitable(sig_ubicacion, mapa) and !(sig_ubicacion.f == st.jugador.f and sig_ubicacion.c == st.jugador.c))
		{
			st_result.sonambulo = sig_ubicacion;
		}
		break;
	case actSON_TURN_SL:
		st_result.sonambulo.brujula = static_cast<Orientacion>((st_result.sonambulo.brujula + 7) % 8);
		break;
	case actSON_TURN_SR:
		st_result.sonambulo.brujula = static_cast<Orientacion>((st_result.sonambulo.brujula + 1) % 8);
		break;
	}

	return st_result;
}

void ComportamientoJugador::VisualizaPlan(const stateN1 &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	stateN1 cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		switch (*it)
		{
		case actFORWARD:
			cst.jugador = NextCasilla(cst.jugador);
			mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
			break;
		case actTURN_R:
			cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 2) % 8);
			break;
		case actTURN_L:
			cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 6) % 8);
			break;
		case actSON_FORWARD:
			cst.sonambulo = NextCasilla(cst.sonambulo);
			mapaConPlan[cst.sonambulo.f][cst.sonambulo.c] = 2;
			break;
		case actSON_TURN_SR:
			cst.sonambulo.brujula = (Orientacion)((cst.sonambulo.brujula + 1) % 8);
			break;
		case actSON_TURN_SL:
			cst.sonambulo.brujula = (Orientacion)((cst.sonambulo.brujula + 7) % 8);
			break;
		}
		it++;
	}
}

list<Action> AnchuraConSonambulo(const stateN1 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa)
{

	nodeN1 current_node;
	list<nodeN1> frontier;
	set<nodeN1> explored;
	list<Action> plan;
	current_node.st = inicio;
	bool SolutionFound = (current_node.st.sonambulo.f == final.f and current_node.st.sonambulo.c == final.c);
	frontier.push_back(current_node);

	while (!frontier.empty() and !SolutionFound)
	{
		frontier.pop_front();
		explored.insert(current_node);

		if (!SolutionFound)
		{

			if (miraSonambulo(current_node.st))
			{
				// Generar hijo actSON_FORWARD
				nodeN1 child_son_forward = current_node;
				child_son_forward.st = apply(actSON_FORWARD, current_node.st, mapa);
				if (child_son_forward.st.sonambulo.f == final.f and child_son_forward.st.sonambulo.c == final.c)
				{
					child_son_forward.secuencia.push_back(actSON_FORWARD);
					current_node = child_son_forward;
					SolutionFound = true;
				}
				else if (explored.find(child_son_forward) == explored.end())
				{
					child_son_forward.secuencia.push_back(actSON_FORWARD);
					frontier.push_back(child_son_forward);
				}

				// Generar hijo actSON_TURN_SL
				nodeN1 child_son_turnl = current_node;
				child_son_turnl.st = apply(actSON_TURN_SL, current_node.st, mapa);
				if (explored.find(child_son_turnl) == explored.end())
				{
					child_son_turnl.secuencia.push_back(actSON_TURN_SL);
					frontier.push_back(child_son_turnl);
				}
				// Generar hijo actSON_TURN_SR
				nodeN1 child_son_turnr = current_node;
				child_son_turnr.st = apply(actSON_TURN_SR, current_node.st, mapa);
				if (explored.find(child_son_turnr) == explored.end())
				{
					child_son_turnr.secuencia.push_back(actSON_TURN_SR);
					frontier.push_back(child_son_turnr);
				}
			}

			if (!SolutionFound)
			{

				// Generar hijo actFORWARD
				nodeN1 child_forward = current_node;
				child_forward.st = apply(actFORWARD, current_node.st, mapa);
				if (explored.find(child_forward) == explored.end())
				{
					child_forward.secuencia.push_back(actFORWARD);
					frontier.push_back(child_forward);
				}

				// Generar hijo actTURN_L
				nodeN1 child_turnl = current_node;
				child_turnl.st = apply(actTURN_L, current_node.st, mapa);
				if (explored.find(child_turnl) == explored.end())
				{
					child_turnl.secuencia.push_back(actTURN_L);
					frontier.push_back(child_turnl);
				}
				// Generar hijo actTURN_R
				nodeN1 child_turnr = current_node;
				child_turnr.st = apply(actTURN_R, current_node.st, mapa);
				if (explored.find(child_turnr) == explored.end())
				{
					child_turnr.secuencia.push_back(actTURN_R);
					frontier.push_back(child_turnr);
				}
			}
		}

		if (!SolutionFound and !frontier.empty())
		{
			current_node = frontier.front();
			while (!frontier.empty() and explored.find(current_node) != explored.end())
			{
				frontier.pop_front();
				if (!frontier.empty())
				{
					current_node = frontier.front();
				}
			}
		}
	}

	if (SolutionFound)
	{
		plan = current_node.secuencia;
		return plan;
	}
}

stateN2 apply(const Action &a, const stateN2 &st, const vector<vector<unsigned char>> &mapa)
{
	stateN2 st_result = st;
	ubicacion sig_ubicacion;
	switch (a)
	{
	case actFORWARD: // si proxima casilla es transitable y no está ocupada por el sonámbulo
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and !(sig_ubicacion.f == st.sonambulo.f and sig_ubicacion.c == st.sonambulo.c))
		{
			st_result.jugador = sig_ubicacion;
		}
		break;
	case actTURN_L:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 6) % 8);
		break;
	case actTURN_R:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 2) % 8);
		break;
	}
	return st_result;
}

void ComportamientoJugador::VisualizaPlan(const stateN2 &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	stateN2 cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		switch (*it)
		{
		case actFORWARD:
			cst.jugador = NextCasilla(cst.jugador);
			mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
			break;
		case actTURN_R:
			cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 2) % 8);
			break;
		case actTURN_L:
			cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 6) % 8);
			break;
		case actSON_FORWARD:
			cst.sonambulo = NextCasilla(cst.sonambulo);
			mapaConPlan[cst.sonambulo.f][cst.sonambulo.c] = 2;
			break;
		case actSON_TURN_SR:
			cst.sonambulo.brujula = (Orientacion)((cst.sonambulo.brujula + 1) % 8);
			break;
		case actSON_TURN_SL:
			cst.sonambulo.brujula = (Orientacion)((cst.sonambulo.brujula + 7) % 8);
			break;
		}
		it++;
	}
}

void ComportamientoJugador::cogeObjeto(const stateN2 &st){
	int f = st.jugador.f;
	int c = st.jugador.c;
	char casilla = mapaResultado[f][c];
	switch(casilla){
		case 'K':
			zapatillas = false;
			bikini = true;
			break;
		case 'D':
			bikini = false;
			zapatillas = true;
			break;
	}
}

int ComportamientoJugador::gastosBateria(const stateN2 &st, Action accion)
{
	int sig_f = st.jugador.f;
	int sig_c = st.jugador.c;

	int coste = 0;
	char casilla = mapaResultado[sig_f][sig_c];

	switch (accion)
	{
	case actFORWARD:
		switch (casilla)
		{
		case 'A':
			if (st.bikini)
			{
				coste = 10;
			}
			else
			{
				coste = 100;
			}
			break;
		case 'B':
			if (st.zapatillas)
			{
				coste = 15;
			}
			else
			{
				coste = 50;
			}
			break;
		case 'T':
			coste = 2;
			break;
		default:
			coste = 1;
			break;
		}
		break;

	case actTURN_L or actTURN_R:
		switch (casilla)
		{
		case 'A':
			if (st.bikini)
			{
				coste = 5;
			}
			else
			{
				coste = 25;
			}
			break;
		case 'B':
			if (st.zapatillas)
			{
				coste = 1;
			}
			else
			{
				coste = 5;
			}
			break;
		case 'T':
			coste = 2;
			break;
		default:
			coste = 1;
			break;
		}
		break;
	}
	return coste;
}

list<Action> ComportamientoJugador::CostoUniforme(const stateN2 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa)
{

	nodeN2 current_node;
	priority_queue<nodeN2> frontier;
	set<nodeN2> explored;
	list<Action> plan;
	current_node.st = inicio;
	current_node.secuencia.empty();

	current_node.st.bikini = false;
	current_node.st.zapatillas = false;
	current_node.coste = gastosBateria(current_node.st, actIDLE);
	bool SolutionFound = (current_node.st.jugador.f == final.f and current_node.st.jugador.c == final.c);
	frontier.push(current_node);

	while (!frontier.empty() and !SolutionFound)
	{
		current_node = frontier.top();
		frontier.pop();
		explored.insert(current_node);

		if (explored.find(current_node) == explored.end())
		{
			// Generar hijo actFORWARD
			nodeN2 child_forward = current_node;
			child_forward.st = apply(actFORWARD, current_node.st, mapa);
			if (child_forward.st.jugador.f == final.f and child_forward.st.jugador.c == final.c)
			{
				child_forward.secuencia.push_back(actFORWARD);
				current_node = child_forward;
				SolutionFound = true;
			}
			else if (explored.find(child_forward) == explored.end())
			{
				child_forward.secuencia.push_back(actFORWARD);
				child_forward.coste = current_node.coste + gastosBateria(current_node.st, actFORWARD);
				cogeObjeto(child_forward.st);
				frontier.push(child_forward);
			}

			if (!SolutionFound)
			{
				// Generar hijo actTURN_L
				nodeN2 child_turnl = current_node;
				child_turnl.st = apply(actTURN_L, current_node.st, mapa);
				if (explored.find(child_turnl) == explored.end())
				{
					child_turnl.secuencia.push_back(actTURN_L);
					child_turnl.coste = current_node.coste + gastosBateria(current_node.st, actTURN_L);
					cogeObjeto(child_forward.st);
					frontier.push(child_turnl);
				}
				// Generar hijo actTURN_R
				nodeN2 child_turnr = current_node;
				child_turnr.st = apply(actTURN_R, current_node.st, mapa);
				if (explored.find(child_turnr) == explored.end())
				{
					child_turnr.secuencia.push_back(actTURN_R);
					child_turnr.coste = current_node.coste + gastosBateria(current_node.st, actTURN_R);
					cogeObjeto(child_forward.st);
					frontier.push(child_turnr);
				}
			}
		}
	}
	plan = current_node.secuencia;
	return plan;
}

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;
	if (sensores.nivel != 4)
	{

		// Incluir aquí el comportamiento del agente jugador
		if (!hayPlan)
		{
			// Invocar al método de búsqueda
			cout << "Calculando un nuevo plan\n";
			c_state.jugador.f = sensores.posF;
			c_state.jugador.c = sensores.posC;
			c_state.jugador.brujula = sensores.sentido;
			c_state.sonambulo.f = sensores.SONposF;
			c_state.sonambulo.c = sensores.SONposC;
			c_state.sonambulo.brujula = sensores.SONsentido;

			c_stateN1.jugador.f = sensores.posF;
			c_stateN1.jugador.c = sensores.posC;
			c_stateN1.jugador.brujula = sensores.sentido;
			c_stateN1.sonambulo.f = sensores.SONposF;
			c_stateN1.sonambulo.c = sensores.SONposC;
			c_stateN1.sonambulo.brujula = sensores.SONsentido;

			c_stateN2.jugador.f = sensores.posF;
			c_stateN2.jugador.c = sensores.posC;
			c_stateN2.jugador.brujula = sensores.sentido;
			c_stateN2.sonambulo.f = sensores.SONposF;
			c_stateN2.sonambulo.c = sensores.SONposC;
			c_stateN2.sonambulo.brujula = sensores.SONsentido;

			goal.f = sensores.destinoF;
			goal.c = sensores.destinoC;
			switch (sensores.nivel)
			{
			case 0:
				plan = AnchuraSoloJugador(c_state, goal, mapaResultado);
				break;
			case 1: // Incluir aquí la llamada al algoritmo de busqueda para el nivel 1
				plan = AnchuraConSonambulo(c_stateN1, goal, mapaResultado);
				break;
			case 2: // Incluir aquí la llamada al algoritmo de busqueda para el nivel 2
				plan = CostoUniforme(c_stateN2, goal, mapaResultado);
				break;
			case 3: // Incluir aquí la llamada al algoritmo de busqueda para el nivel 3
				cout << "Pendiente de implementar el nivel 1\n";
				break;
			}
			if (plan.size() > 0)
			{
				VisualizaPlan(c_state, plan);
				hayPlan = true;
			}

		}
		if (hayPlan and plan.size() > 0)
		{
			cout << "Ejecutando la siguiente acción del plan\n";
			accion = plan.front();
			plan.pop_front();
		}
		if (plan.size() == 0)
		{
			cout << "Se completó el plan \n";
			hayPlan = false;
		}
	}
	else
	{
		// Incluir aquí la solución al nivel 4
	}
	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}