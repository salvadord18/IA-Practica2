#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct stateN0
{
  ubicacion jugador;
  ubicacion sonambulo;

  bool operator==(const stateN0 &st) const
  {
    if (jugador == st.jugador and sonambulo.f == st.sonambulo.f and sonambulo.c == st.sonambulo.c)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
};

struct nodeN0
{
  stateN0 st;
  list<Action> secuencia;

  bool operator==(const nodeN0 &n) const
  {
    return (st == n.st);
  }

  bool operator<(const nodeN0 &n) const
  {
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

struct stateN1
{
  ubicacion jugador;
  ubicacion sonambulo;

  bool operator==(const stateN1 &st) const
  {
    return (jugador.f == st.jugador.f and
            jugador.c == st.jugador.c and sonambulo.f == st.sonambulo.f and
            sonambulo.c == st.sonambulo.c and jugador.brujula == st.jugador.brujula and
            sonambulo.brujula == st.sonambulo.brujula);
  }

  bool operator<(const stateN1 &st) const
  {
    return (jugador.f < st.jugador.f or
            (jugador.f == st.jugador.f and jugador.c < st.jugador.c) or
            (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula < st.jugador.brujula) or
            (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and sonambulo.f < st.sonambulo.f) or
            (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and sonambulo.f == st.sonambulo.f and sonambulo.c < st.sonambulo.c) or
            (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and sonambulo.f == st.sonambulo.f and sonambulo.c == st.sonambulo.c and sonambulo.brujula < st.sonambulo.brujula));
  }
};

struct nodeN1
{
  stateN1 st;
  list<Action> secuencia;

  bool operator==(const nodeN1 &n) const
  {
    return (st == n.st);
  }

  bool operator<(const nodeN1 &n) const
  {
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

struct stateN2
{
  ubicacion jugador;
  ubicacion sonambulo;

  bool bikini = false;
  bool zapatillas = false;

  bool operator==(const stateN2 &st) const
  {
    if (jugador.f == st.jugador.f and
        jugador.c == st.jugador.c and sonambulo.f == st.sonambulo.f and
        sonambulo.c == st.sonambulo.c and jugador.brujula == st.jugador.brujula and
        sonambulo.brujula == st.sonambulo.brujula)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  bool operator<(const stateN2 &st) const
  {
    if ((jugador.f < st.jugador.f) or
        (jugador.f == st.jugador.f and jugador.c < st.jugador.c) or
        (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula < st.jugador.brujula) or
        (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and bikini < st.bikini) or
        (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and bikini == st.bikini and zapatillas < st.zapatillas))
      return true;
    else
      return false;
  }
};

struct nodeN2
{
  stateN2 st;
  list<Action> secuencia;
  int coste;

  bool operator==(const nodeN2 &n) const
  {
    return (st == n.st);
  }

  bool operator<(const nodeN2 &n) const
  {
    return coste > n.coste;
  }
};

struct stateN3
{
  ubicacion jugador;
  ubicacion sonambulo;

  bool bikini = false;
  bool zapatillas = false;

  bool bikini_son = false;
  bool zapatillas_son = false;

  bool operator==(const stateN3 &st) const
  {
    return (jugador.f == st.jugador.f and
            jugador.c == st.jugador.c and sonambulo.f == st.sonambulo.f and
            sonambulo.c == st.sonambulo.c and jugador.brujula == st.jugador.brujula and
            sonambulo.brujula == st.sonambulo.brujula);
  }

  bool operator<(const stateN3 &st) const
  {
    return (jugador.f < st.jugador.f or
            (jugador.f == st.jugador.f and jugador.c < st.jugador.c) or
            (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula < st.jugador.brujula) or
            (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and sonambulo.f < st.sonambulo.f) or
            (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and sonambulo.f == st.sonambulo.f and sonambulo.c < st.sonambulo.c) or
            (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and sonambulo.f == st.sonambulo.f and sonambulo.c == st.sonambulo.c and sonambulo.brujula < st.sonambulo.brujula) or
            (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and sonambulo.f == st.sonambulo.f and sonambulo.c == st.sonambulo.c and sonambulo.brujula == st.sonambulo.brujula and bikini_son < st.bikini_son) or
            (jugador.f == st.jugador.f and jugador.c == st.jugador.c and jugador.brujula == st.jugador.brujula and sonambulo.f == st.sonambulo.f and sonambulo.c == st.sonambulo.c and sonambulo.brujula == st.sonambulo.brujula and bikini_son == st.bikini_son and zapatillas_son < st.zapatillas_son));
  }
};

struct nodeN3
{
  stateN3 st;
  list<Action> secuencia;
  int coste;
  int heuristica;

  bool operator==(const nodeN3 &n) const
  {
    return (st == n.st);
  }

  bool operator<(const nodeN3 &n) const
  {
    return ((coste + heuristica) > (n.coste + n.heuristica));
  }
};

class ComportamientoJugador : public Comportamiento
{
public:
  ComportamientoJugador(unsigned int size) : Comportamiento(size)
  {
    // Inicializar Variables de Estado
  }
  ComportamientoJugador(std::vector<std::vector<unsigned char>> mapaR) : Comportamiento(mapaR)
  {
    // Inicializar Variables de Estado
    hayPlan = false;
  }
  ComportamientoJugador(const ComportamientoJugador &comport) : Comportamiento(comport) {}
  ~ComportamientoJugador() {}

  Action think(Sensores sensores);
  int interact(Action accion, int valor);
  void VisualizaPlan(const stateN0 &st, const list<Action> &plan);
  void VisualizaPlan(const stateN1 &st, const list<Action> &plan);
  void VisualizaPlan(const stateN2 &st, const list<Action> &plan);
  void cogeObjeto(stateN2 &st);
  int gastosBateria(stateN2 &st, Action accion);
  list<Action> CostoUniforme(const stateN2 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa);
  void VisualizaPlan(const stateN3 &st, const list<Action> &plan);
  void cogeObjeto(stateN3 &st);
  int gastosBateria(stateN3 &st, Action accion);
  list<Action> AEstrella(const stateN3 &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa);

private:
  // Declarar Variables de Estado
  list<Action> plan; // Almacena el plan en ejecución
  bool hayPlan;      // Si verdad indica que se está siguiendo un plan.
  stateN0 c_state;
  ubicacion goal;
  stateN1 c_stateN1;
  stateN2 c_stateN2;
  stateN3 c_stateN3;
};

#endif