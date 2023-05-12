#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>

//** Devuelve si una ubicación en el mapa es transitable para el agente */
bool CasillaTransitable(const ubicacion &x, const vector<vector<unsigned char>> &mapa){
	return (mapa[x.f][x.c] != 'P' and mapa[x.f][x.c] != 'M');
}

//** Devuelve la ubicación siguiente según el avance del agente */
ubicacion NextCasilla(const ubicacion &pos){
	ubicacion next = pos;
	switch (pos.brujula){
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
stateN0 apply(const Action &a, const stateN0 &st, const vector<vector<unsigned char>> &mapa){
	stateN0 st_result = st;
	ubicacion sig_ubicacion;
	switch (a){
	case actFORWARD: // si proxima casilla es transitable y no está ocupada por el sonámbulo
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and !(sig_ubicacion.f == st.sonambulo.f and sig_ubicacion.c == st.sonambulo.c)){
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
void AnularMatriz(vector<vector<unsigned char>> &matriz){
	for (int i = 0; i < matriz.size(); i++){
		for (int j = 0; j < matriz[i].size(); j++){
			matriz[i][j] = 0;
		}
	}
}

//** Permite pintar sobre el mapa del simulador el plan partiendo desde el estado st */
void ComportamientoJugador::VisualizaPlan(const stateN0 &st, const list<Action> &plan){
	AnularMatriz(mapaConPlan);
	stateN0 cst = st;

	auto it = plan.begin();
	while (it != plan.end()){
		switch (*it){
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
list<Action> AnchuraSoloJugador(const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa){

	nodeN0 current_node;
	list<nodeN0> frontier;
	set<nodeN0> explored;
	list<Action> plan;
	current_node.st = inicio;
	bool SolutionFound = (current_node.st.jugador.f == final.f and current_node.st.jugador.c == final.c);
	frontier.push_back(current_node);

	while (!frontier.empty() and !SolutionFound){
		frontier.pop_front();
		explored.insert(current_node);

		// Generar hijo actFORWARD
		nodeN0 child_forward = current_node;
		child_forward.st = apply(actFORWARD, current_node.st, mapa);
		if (child_forward.st.jugador.f == final.f and child_forward.st.jugador.c == final.c){
			child_forward.secuencia.push_back(actFORWARD);
			current_node = child_forward;
			SolutionFound = true;
		} else if (explored.find(child_forward) == explored.end()){
			child_forward.secuencia.push_back(actFORWARD);
			frontier.push_back(child_forward);
		}

		if (!SolutionFound){
			// Generar hijo actTURN_L
			nodeN0 child_turnl = current_node;
			child_turnl.st = apply(actTURN_L, current_node.st, mapa);
			if (explored.find(child_turnl) == explored.end()){
				child_turnl.secuencia.push_back(actTURN_L);
				frontier.push_back(child_turnl);
			}
			// Generar hijo actTURN_R
			nodeN0 child_turnr = current_node;
			child_turnr.st = apply(actTURN_R, current_node.st, mapa);
			if (explored.find(child_turnr) == explored.end()){
				child_turnr.secuencia.push_back(actTURN_R);
				frontier.push_back(child_turnr);
			}
		}

		if (!SolutionFound and !frontier.empty()){
			current_node = frontier.front();
			while (!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop_front();
				if (!frontier.empty()){
					current_node = frontier.front();
				}
			}
		}
	}

	if (SolutionFound){
		plan = current_node.secuencia;
		return plan;
	}
}

bool miraSonambulo(const stateN1 &st){
	int sonambuloF = st.sonambulo.f;
	int sonambuloC = st.sonambulo.c;
	int jugadorF = st.jugador.f;
	int jugadorC = st.jugador.c;

	int distanciaF = jugadorF - sonambuloF;
	int distanciaC = jugadorC - sonambuloC;

	switch (st.jugador.brujula){
		case norte:
			if(distanciaF >= -3 && distanciaF < 0 && abs(distanciaC) <= 3){
				return true;
			}		
			break;
		case noreste:
			if(distanciaF >= -3 && distanciaF < 0 && distanciaC >= 0 && distanciaC <= 4){
				return true;
			}
			break;
		case este:
			if(abs(distanciaF) <= 3 && distanciaC >= 0 && distanciaC < 4){
				return true;
			}
			break;
		case sureste:
			if(abs(distanciaF) <= 3 && distanciaC >= 0 && distanciaC < 4){
				return true;
			}
			break;
		case sur:
			if(distanciaF >= 0 && distanciaF <= 3 && abs(distanciaC) <= 3){
				return true;
			}
			break;
		case suroeste:
			if(distanciaF > 0 && distanciaF <= 3 && distanciaC > -4){
				return true;
			}
			break;
		case oeste:
			if(abs(distanciaF) <= 3 && distanciaC < 0 && distanciaC >= -3){
				return true;
			}
			break;
		case noroeste:
			if(distanciaF >= -3 && distanciaF < 0 && distanciaC < 0 && distanciaC >= -3){
				return true;
			}
			break;
		default:
			break;
	}
}

stateN1 applyN1(const Action &a, const stateN1 &st, const vector<vector<unsigned char>> &mapa){
	stateN1 st_result = st;
	ubicacion sig_ubicacion;
	switch (a){
	case actFORWARD: // si proxima casilla es transitable y no está ocupada por el sonámbulo
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and !(sig_ubicacion.f == st.sonambulo.f and sig_ubicacion.c == st.sonambulo.c)){
			st_result.jugador = sig_ubicacion;
		}
		st_result.sonambulo = NextCasilla(st.sonambulo);
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

void ComportamientoJugador::VisualizaPlanSonambulo(const stateN1 &st, const list<Action> &plan){
	AnularMatriz(mapaConPlan);
	stateN1 cst = st;

	auto it = plan.begin();
	while (it != plan.end()){
		switch (*it){
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

list<Action> AnchuraConSonambulo(const stateN1 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa){

	nodeN1 current_node;
	list<nodeN1> frontier;
	set<nodeN1> explored;
	list<Action> plan;
	current_node.st = inicio;
	bool SolutionFound = (current_node.st.jugador.f == final.f and current_node.st.jugador.c == final.c);
	frontier.push_back(current_node);

	while (!frontier.empty() and !SolutionFound){
		frontier.pop_front();
		explored.insert(current_node);

		// Generar hijo actFORWARD
		nodeN1 child_forward = current_node;
		child_forward.st = applyN1(actFORWARD, current_node.st, mapa);
		if (child_forward.st.jugador.f == final.f and child_forward.st.jugador.c == final.c){
			child_forward.secuencia.push_back(actFORWARD);
			current_node = child_forward;
			SolutionFound = true;
		} else if (explored.find(child_forward) == explored.end()){
			child_forward.secuencia.push_back(actFORWARD);
			frontier.push_back(child_forward);
		}

		if (!SolutionFound){
			// Generar hijo actTURN_L
			nodeN1 child_turnl = current_node;
			child_turnl.st = applyN1(actTURN_L, current_node.st, mapa);
			if (explored.find(child_turnl) == explored.end()){
				child_turnl.secuencia.push_back(actTURN_L);
				frontier.push_back(child_turnl);
			}
			// Generar hijo actTURN_R
			nodeN1 child_turnr = current_node;
			child_turnr.st = applyN1(actTURN_R, current_node.st, mapa);
			if (explored.find(child_turnr) == explored.end()){
				child_turnr.secuencia.push_back(actTURN_R);
				frontier.push_back(child_turnr);
			}

			if(miraSonambulo(current_node.st)){
				// Generar hijo actSON_FORWARD
				nodeN1 child_son_forward = current_node;
				child_son_forward.st = applyN1(actFORWARD, current_node.st, mapa);
				if(explored.find(child_son_forward) == explored.end()){
					child_son_forward.secuencia.push_back(actSON_FORWARD);
					frontier.push_back(child_son_forward);
				}
				// Generar hijo actSON_TURN_SL
				nodeN1 child_son_turnl = current_node;
				child_son_turnl.st = applyN1(actSON_TURN_SL, current_node.st, mapa);
				if(explored.find(child_son_turnl) == explored.end()){
					child_son_turnl.secuencia.push_back(actSON_TURN_SL);
					frontier.push_back(child_son_turnl);
				}
				// Generar hijo actSON_TURN_SR
				nodeN1 child_son_turnr = current_node;
				child_son_turnr.st = applyN1(actSON_TURN_SR, current_node.st, mapa);
				if(explored.find(child_son_turnr) == explored.end()){
					child_son_turnr.secuencia.push_back(actSON_TURN_SR);
					frontier.push_back(child_son_turnr);
				}
			}
		}

		if (!SolutionFound and !frontier.empty()){
			current_node = frontier.front();
			while (!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop_front();
				if (!frontier.empty()){
					current_node = frontier.front();
				}
			}
		}
	}

	if (SolutionFound){
		plan = current_node.secuencia;
		return plan;
	}
}


int ComportamientoJugador::gastosBateria(Action accion, Sensores sensores){
	int bateria = sensores.bateria;
	switch(accion){
		case actFORWARD:
			switch(sensores.terreno[2]){
				case 'A':
					if(bikini){
						bateria -= 10;
					} else {
						bateria -= 100;
					}
					break;
				case 'B':
					if(zapatillas){
						bateria -= 15;
					} else {
						bateria -= 50;
					}
					break;
				case 'T':
					bateria -= 2;
					break;
				default:
					bateria -= 1;
					break;
			}
			break;
		case actSON_FORWARD:
			switch(sensores.terreno[2]){
				case 'A':
					if(bikini){
						bateria -= 10;
					} else {
						bateria -= 100;
					}
					break;
				case 'B':
					if(zapatillas){
						bateria -= 15;
					} else {
						bateria -= 50;
					}
					break;
				case 'T':
					bateria -= 2;
					break;
				default:
					bateria -= 1;
					break;
			}
			break;
		case actTURN_L or actTURN_R:
			switch(sensores.terreno[2]){
				case 'A':
					if(bikini){
						bateria -= 5;
					} else {
						bateria -= 25;
					}
					break;
				case 'B':
					if(zapatillas){
						bateria -= 1;
					} else {
						bateria -= 5;
					}
					break;
				case 'T':
					bateria -= 2;
					break;
				default:
					bateria -= 1;
					break;
			}
			break;
		case actSON_TURN_SL: // Aquí he intentado de hacer como en la línea 230 per me estaba dando un fallo diciendo que 'actTURN_BL ya estaba representado en la línea 230 
			switch(sensores.terreno[2]){
					case 'A':
						if(bikini){
							bateria -= 2;
						} else {
							bateria -= 7;
						}
						break;
					case 'B':
						if(zapatillas){
							bateria -= 1;
						} else {
							bateria -= 3;
						}
						break;
					case 'T':
						bateria -= 1;
						break;
					default:
						bateria -= 1;
						break;
			}
			break;
			case actSON_TURN_SR:
				switch(sensores.terreno[2]){
						case 'A':
							if(bikini){
								bateria -= 2;
							} else {
								bateria -= 7;
							}
							break;
						case 'B':
							if(zapatillas){
								bateria -= 1;
							} else {
								bateria -= 3;
							}
							break;
						case 'T':
							bateria -= 1;
							break;
						default:
							bateria -= 1;
							break;
				}
				break;
	}
	return bateria;
}


// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores){
	Action accion = actIDLE;
	if(sensores.nivel != 4){

		// Incluir aquí el comportamiento del agente jugador
		if(!hayPlan){
			// Invocar al método de búsqueda
			cout << "Calculando un nuevo plan\n";
			c_state.jugador.f = sensores.posF;
			c_state.jugador.c = sensores.posC;
			c_state.jugador.brujula = sensores.sentido;
			c_state.sonambulo.f = sensores.SONposF;
			c_state.sonambulo.c = sensores.SONposC;
			c_state.sonambulo.brujula = sensores.SONsentido;
			goal.f = sensores.destinoF;
			goal.c = sensores.destinoC;
			switch (sensores.nivel){
				case 0:
					plan = AnchuraSoloJugador(c_state, goal, mapaResultado);
					break;
				case 1: // Incluir aquí la llamada al algoritmo de busqueda para el nivel 1
					plan = AnchuraSoloJugador(c_state, goal, mapaResultado);
					break;
				case 2: // Incluir aquí la llamada al algoritmo de busqueda para el nivel 2
					cout << "Pendiente de implementar el nivel 1\n";
					break;
				case 3: // Incluir aquí la llamada al algoritmo de busqueda para el nivel 3
					cout << "Pendiente de implementar el nivel 1\n";
					break;
			}
			if (plan.size() > 0){
				VisualizaPlan(c_state, plan);
				hayPlan = true;
			}

			if(sensores.terreno[0] == 'D' and !zapatillas){
				bikini = false;
				zapatillas = true;
			} else if(sensores.terreno[0] == 'K' and !bikini){
				zapatillas = false;
				bikini = true;
	}
		}
		if(hayPlan and plan.size() > 0){
			cout << "Ejecutando la siguiente acción del plan\n";
			accion = plan.front();
			plan.pop_front();
		}
		if(plan.size() == 0){
			cout << "Se completó el plan \n";
			hayPlan = false;
		}
	} else {
		// Incluir aquí la solución al nivel 4
	}
	return accion;
}


int ComportamientoJugador::interact(Action accion, int valor){
	return false;
}