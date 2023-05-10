#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

/*struct ubicacion{
	int f;
	int c;
	Orientacion brujula;

	bool operator==(const ubicacion &ub)const {
		return (f == ub.f and c == ub.c and brujula == ub.brujula);
	}
};*/

struct stateN0{
  ubicacion jugador;
  ubicacion sonambulo;

  bool operator== (const stateN0 &x) const{
    if (jugador == x.jugador and sonambulo.f == x.sonambulo.f and sonambulo.c == x.sonambulo.c){
      return true;
    } else {
      return false;
    }
  }
};

struct nodeN0{
  stateN0 st;
  list<Action> secuencia;

  bool operator == (const nodeN0 &n) const{
    return (st == n.st);
  }

  bool operator < (const nodeN0 &n) const{
    if (st.jugador.f < n.st.jugador.f)
      return true;
    else if (st.jugador.f == n.st.jugador.f and st.jugador.c < n.st.jugador.c)
      return true;
    else if (st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula < n.st.jugador.brujula)
      return true;
    else
      return false;
  }
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      hayPlan = false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const stateN0 &st, const list<Action> &plan);

  private:
    // Declarar Variables de Estado
    list<Action> plan; //Almacena elplan en ejecución
    bool hayPlan; //Si verdad indica que se está siguiendo un plan.
    stateN0 c_state;
    ubicacion goal;
};

#endif