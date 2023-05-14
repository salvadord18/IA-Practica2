#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct stateN0{
  ubicacion jugador;
  ubicacion sonambulo;

  bool operator== (const stateN0 &st) const{
    if (jugador == st.jugador and sonambulo.f == st.sonambulo.f and sonambulo.c == st.sonambulo.c){
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

struct stateN1{
  ubicacion jugador;
  ubicacion sonambulo;

  bool operator== (const stateN1 &st) const{
    return (jugador.f == st.jugador.f and 
    jugador.c == st.jugador.c and sonambulo.f == st.sonambulo.f and 
    sonambulo.c == st.sonambulo.c and jugador.brujula == st.jugador.brujula and 
    sonambulo.brujula == st.sonambulo.brujula);
  }

  bool operator<(const stateN1 &st) const{
    return (jugador.f < st.jugador.f or 
    (jugador.f == st.jugador.f and jugador.c < st.jugador.c) or 
    (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula < st.jugador.brujula) or 
    (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and sonambulo.f < st.sonambulo.f) or 
    (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and sonambulo.f == st.sonambulo.f and sonambulo.c < st.sonambulo.c) or 
    (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and sonambulo.f == st.sonambulo.f and sonambulo.c == st.sonambulo.c and sonambulo.brujula < st.sonambulo.brujula));
  }
};

struct nodeN1{
  stateN1 st;
  list<Action> secuencia;

  bool operator == (const nodeN1 &n) const{
    return (st == n.st);
  }

  bool operator < (const nodeN1 &n) const{
    if (st.jugador.f < n.st.jugador.f)
      return true;
    else if (st.jugador.f == n.st.jugador.f and st.jugador.c < n.st.jugador.c)
      return true;
    else if (st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula < n.st.jugador.brujula)
      return true;
    else if (st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and 
            st.sonambulo.f < n.st.sonambulo.f)
      return true;
    else if (st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and 
            st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c < n.st.sonambulo.c)
      return true;
    else if (st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and 
            st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c and st.sonambulo.brujula < n.st.sonambulo.brujula)
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
      zapatillas = false;
      bikini = false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const stateN0 &st, const list<Action> &plan);
    int gastosBateria(Action accion, Sensores sensores);
    void VisualizaPlanN1(const stateN1 &st, const list<Action> &plan);

  private:
    // Declarar Variables de Estado
    list<Action> plan; //Almacena el plan en ejecución
    bool hayPlan; //Si verdad indica que se está siguiendo un plan.
    stateN0 c_state;
    ubicacion goal;
    bool zapatillas;
    bool bikini;
    stateN1 c_stateN1;
};

#endif