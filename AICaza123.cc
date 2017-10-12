#include "Player.hh"
using namespace std;

#define PLAYER_NAME Caza123
typedef vector< vector<bool> > matriu_booleana;
typedef vector< vector<Pos> > matriu_posicions;

struct PLAYER_NAME : public Player {

    static Player* factory () {
        return new PLAYER_NAME;
    }

  vector<Dir> all_dirs;
  
  bool esquivar(Pos p) {
    if (within_window(Pos(p - FAST), round()+1) and (cell(p-FAST).type == STARSHIP or cell(p-FAST).type == MISSILE)) {
      return false;
    }
    else if (within_window(Pos(p - DEFAULT), round()+1) and (cell(p-DEFAULT).type == STARSHIP or cell(p-DEFAULT).type == MISSILE)) {
     return false;
    }
    else if (within_window(Pos(p - complex<int>(0,3)), round()+1) and (cell(p- complex<int>(0,3)).type == STARSHIP or cell(p- complex<int>(0,3)).type == MISSILE)) {
      return false;
    }
    else if (within_window(Pos(p - complex<int>(0,4)), round()+1) and (cell(p- complex<int>(0,4)).type == STARSHIP or cell(p- complex<int>(0,4)).type == MISSILE)) {
      return false;
    }
    return true;
  }
  
  bool es_pot(Pos p, Dir d) { // mira les posicions intermitges als FASTs, UP, DOWN per tal de no trobar asteroides.
    if (d == UP) {
      if(within_window(Pos(p + DEFAULT), round()+1) and within_window(Pos(p + SLOW_UP), round()+1)) {
	CType t1 = cell(Pos(p+DEFAULT)).type;
	CType t2 = cell(Pos(p+SLOW_UP)).type;
	if ((t1 == EMPTY or t1 == POINT_BONUS or t1 == MISSILE_BONUS) and (t2 == EMPTY or t2 == POINT_BONUS or t2 == MISSILE_BONUS)) return true;
      }
      return false;
    }
    else if (d == DOWN) {
      if(within_window(Pos(p + DEFAULT), round()+1) and within_window(Pos(p + SLOW_DOWN), round()+1)) {
	CType t1 = cell(Pos(p+DEFAULT)).type;
	CType t2 = cell(Pos(p+SLOW_DOWN)).type;
	if ((t1 == EMPTY or t1 == POINT_BONUS or t1 == MISSILE_BONUS) and (t2 == EMPTY or t2 == POINT_BONUS or t2 == MISSILE_BONUS)) return true;
      }
      return false;
    }
    else if (d == FAST) {
      if(within_window(Pos(p + DEFAULT), round()+1)) {
	CType t1 = cell(Pos(p+DEFAULT)).type;
	if ((t1 == EMPTY or t1 == POINT_BONUS or t1 == MISSILE_BONUS)) return true;
      }
      return false;
    }
    else if (d == FAST_UP) {
      if(within_window(Pos(p + DEFAULT), round()+1) and within_window(Pos(p + SLOW_UP), round()+1)) {
	CType t1 = cell(Pos(p+DEFAULT)).type;
	CType t2 = cell(Pos(p+SLOW_UP)).type;
	if ((t1 == EMPTY or t1 == POINT_BONUS or t1 == MISSILE_BONUS) and (t2 == EMPTY or t2 == POINT_BONUS or t2 == MISSILE_BONUS)) {
	  if(within_window(Pos(p + UP), round()+1) and within_window(Pos(p + FAST), round()+1)) {
	    CType t3 = cell(Pos(p+UP)).type;
	    CType t4 = cell(Pos(p+FAST)).type;
	    if ((t3 == EMPTY or t3 == POINT_BONUS or t3 == MISSILE_BONUS) and (t4 == EMPTY or t4 == POINT_BONUS or t4 == MISSILE_BONUS)) return true;
	  }
	}
      }
      return false;
    }
    else if (d == FAST_DOWN) {
      if(within_window(Pos(p + DEFAULT), round()+1) and within_window(Pos(p + SLOW_DOWN), round()+1)) {
	CType t1 = cell(Pos(p+DEFAULT)).type;
	CType t2 = cell(Pos(p+SLOW_DOWN)).type;
	if ((t1 == EMPTY or t1 == POINT_BONUS or t1 == MISSILE_BONUS) and (t2 == EMPTY or t2 == POINT_BONUS or t2 == MISSILE_BONUS)) {
	  if(within_window(Pos(p + FAST), round()+1) and within_window(Pos(p + DOWN), round()+1)) {
	    CType t3 = cell(Pos(p+DOWN)).type;
	    CType t4 = cell(Pos(p+FAST)).type;
	    if ((t3 == EMPTY or t3 == POINT_BONUS or t3 == MISSILE_BONUS) and (t4 == EMPTY or t4 == POINT_BONUS or t4 == MISSILE_BONUS)) return true;
	  }
	}
      }
      return false;
    }
    return true;
  }
  
  bool mou_nau(Starship& s, Pos seguent) { // Fa el moviment en direcciÃƒÆ’Ã‚Â³ BONUS.
    CType t = cell(seguent).type;
    if (t == STARSHIP or t == MISSILE) {
      cerr << "ES QUEDA QUIET PERQUE TROBA NAU O MISSIL" << endl;
      if((Pos(s.pos + DEFAULT) == seguent) or (Pos(s.pos + FAST) == seguent)) {
	if((within_window(Pos(s.pos + SLOW_UP), round())) and ((cell(Pos(s.pos + SLOW_UP)).type == EMPTY) or (cell(Pos(s.pos + SLOW_UP)).type == POINT_BONUS) or (cell(Pos(s.pos + SLOW_UP)).type == MISSILE_BONUS))) {
	  seguent = seguent + SLOW_UP;
	  if (within_window(Pos(seguent - FAST), round()+1) and (cell(seguent-FAST).type == STARSHIP or cell(seguent-DEFAULT).type == STARSHIP or cell(seguent-FAST).type == MISSILE or cell(seguent-DEFAULT).type == MISSILE)) {
	  move(s.sid,SLOW);
	  return true;
	  }
	  else {
	    cerr << "MOU SLOW_UP" << endl; 
	    move(s.sid,SLOW_UP);
	  }
	}
	else if((within_window(Pos(s.pos + SLOW_DOWN), round())) and ((cell(Pos(s.pos + SLOW_DOWN)).type == EMPTY) or (cell(Pos(s.pos + SLOW_DOWN)).type == POINT_BONUS) or (cell(Pos(s.pos + SLOW_DOWN)).type == MISSILE_BONUS))) {
	    seguent = seguent + SLOW_UP;
	    if (within_window(Pos(seguent - FAST), round()+1) and (cell(seguent-FAST).type == STARSHIP or cell(seguent-DEFAULT).type == STARSHIP or cell(seguent-FAST).type == MISSILE or cell(seguent-DEFAULT).type == MISSILE)) {
	    move(s.sid,SLOW);
	    return true;
	    }
	    else {
	    cerr << "MOU SLOW_DOWN" << endl;
	    move(s.sid,SLOW_DOWN);
	    }
	  }
	}
      }
    else {
      if(Pos(s.pos + DEFAULT) == seguent) {
	if (within_window(Pos(s.pos - FAST), round()+1) and (cell(s.pos-FAST).type == STARSHIP or cell(s.pos-FAST).type == MISSILE)) {
	  if((within_window(Pos(s.pos + SLOW_UP), round())) and ((cell(Pos(s.pos + SLOW_UP)).type == EMPTY) or (cell(Pos(s.pos + SLOW_UP)).type == POINT_BONUS) or (cell(Pos(s.pos + SLOW_UP)).type == MISSILE_BONUS))) {
	    seguent = seguent + SLOW_UP;
	    if (within_window(Pos(seguent - FAST), round()+1) and (cell(seguent-FAST).type == STARSHIP or cell(seguent-DEFAULT).type == STARSHIP or cell(seguent-FAST).type == MISSILE or cell(seguent-DEFAULT).type == MISSILE)) {
	    move(s.sid,SLOW);
	    return true;
	    }
	    else {
	      cerr << "MOU SLOW_UP" << endl; 
	      move(s.sid,SLOW_UP);
	    }
	  }
	  else if((within_window(Pos(s.pos + SLOW_DOWN), round())) and ((cell(Pos(s.pos + SLOW_DOWN)).type == EMPTY) or (cell(Pos(s.pos + SLOW_DOWN)).type == POINT_BONUS) or (cell(Pos(s.pos + SLOW_DOWN)).type == MISSILE_BONUS))) {
	    seguent = seguent + SLOW_UP;
	    if (within_window(Pos(seguent - FAST), round()+1) and (cell(seguent-FAST).type == STARSHIP or cell(seguent-DEFAULT).type == STARSHIP or cell(seguent-FAST).type == MISSILE or cell(seguent-DEFAULT).type == MISSILE)) {
	    move(s.sid,SLOW);
	    return true;
	    }
	    else {
	    cerr << "MOU SLOW_DOWN" << endl;
	    move(s.sid,SLOW_DOWN);
	    }
	  }
	}
	else {
	  move(s.sid,DEFAULT);
	  cerr << "MOU DEFAULT" << endl;
	}
	return true;
      }
      else if(Pos(s.pos + SLOW_UP) == seguent) {
	if (within_window(Pos(seguent - FAST), round()+1) and (cell(seguent-FAST).type == STARSHIP or cell(seguent-DEFAULT).type == STARSHIP or cell(seguent-FAST).type == MISSILE or cell(seguent-DEFAULT).type == MISSILE)) {
	  move(s.sid,SLOW);
	  return true;
	}
	else {
	  move(s.sid,SLOW_UP);
	  cerr << "MOU SLOW_UP" << endl;
	}
	return true;
      }
      else if(Pos(s.pos + SLOW_DOWN) == seguent) {
      if (within_window(Pos(seguent - FAST), round()+1) and (cell(seguent-FAST).type == STARSHIP or cell(seguent-DEFAULT).type == STARSHIP or cell(seguent-FAST).type == MISSILE or cell(seguent-DEFAULT).type == MISSILE)) {
	  move(s.sid,SLOW);
	  return true;
	}
	else {
	  move(s.sid,SLOW_DOWN);
	  cerr << "MOU SLOW_DOWN" << endl;
	  return true;
	} 
      }
      else if(Pos(s.pos + UP) == seguent) {
      if (within_window(Pos(seguent - FAST), round()+1) and (cell(seguent-FAST).type == STARSHIP or cell(seguent-DEFAULT).type == STARSHIP or cell(seguent-FAST).type == MISSILE or cell(seguent-DEFAULT).type == MISSILE)) {
	  move(s.sid,SLOW);
	  return true;
	}
	else {
	  if(es_pot(s.pos,UP)) {
	    move(s.sid,UP);
	    cerr << "MOU UP" << endl;
	    return true;
	  }
	}
      }
      else if(Pos(s.pos + DOWN) == seguent) {
	if (within_window(Pos(seguent - FAST), round()+1) and (cell(seguent-FAST).type == STARSHIP or cell(seguent-DEFAULT).type == STARSHIP or cell(seguent-FAST).type == MISSILE or cell(seguent-DEFAULT).type == MISSILE)) {
	  move(s.sid,SLOW);
	  return true;
	}
	else {
	  if(es_pot(s.pos,DOWN)) {
	    move(s.sid,DOWN);
	    cerr << "MOU DOWN" << endl;
	    return true;
	  }
	  else return false;
	}
      }
      else if(Pos(s.pos + FAST_UP) == seguent) {
	if (within_window(Pos(seguent - FAST), round()+1) and (cell(seguent-FAST).type == STARSHIP or cell(seguent-DEFAULT).type == STARSHIP or cell(seguent-FAST).type == MISSILE or cell(seguent-DEFAULT).type == MISSILE)) {
	  move(s.sid,SLOW);
	  return true;
	}
	else {
	  if(es_pot(s.pos,FAST_UP)) {
	    move(s.sid,FAST_UP);
	    cerr << "MOU FAST_UP" << endl;
	    return true;
	  }
	  else return false;
	}
      }
      else if(Pos(s.pos + FAST) == seguent) {
	if (within_window(Pos(s.pos - FAST), round()) and (cell(s.pos-FAST).type == STARSHIP or cell(s.pos-FAST).type == MISSILE)) {
	  if((within_window(Pos(s.pos + SLOW_UP), round())) and ((cell(Pos(s.pos + SLOW_UP)).type == EMPTY) or (cell(Pos(s.pos + SLOW_UP)).type == POINT_BONUS) or (cell(Pos(s.pos + SLOW_UP)).type == MISSILE_BONUS))) {
	    seguent = seguent + SLOW_UP;
	    if (within_window(Pos(seguent - FAST), round()+1) and (cell(seguent-FAST).type == STARSHIP or cell(seguent-DEFAULT).type == STARSHIP or cell(seguent-FAST).type == MISSILE or cell(seguent-DEFAULT).type == MISSILE)) {
	    move(s.sid,SLOW);
	    return true;
	    }
	    else {
	      cerr << "MOU SLOW_UP" << endl; 
	      move(s.sid,SLOW_UP);
	    }
	  }
	  else if((within_window(Pos(s.pos + SLOW_DOWN), round())) and ((cell(Pos(s.pos + SLOW_DOWN)).type == EMPTY) or (cell(Pos(s.pos + SLOW_DOWN)).type == POINT_BONUS) or (cell(Pos(s.pos + SLOW_DOWN)).type == MISSILE_BONUS))) {
	    seguent = seguent + SLOW_UP;
	    if (within_window(Pos(seguent - FAST), round()+1) and (cell(seguent-FAST).type == STARSHIP or cell(seguent-DEFAULT).type == STARSHIP or cell(seguent-FAST).type == MISSILE or cell(seguent-DEFAULT).type == MISSILE)) {
	    move(s.sid,SLOW);
	    return true;
	    }
	    else {
	    cerr << "MOU SLOW_DOWN" << endl;
	    move(s.sid,SLOW_DOWN);
	    }
	  }
	}
	else {
	  if(es_pot(s.pos,FAST)) {
	    move(s.sid,FAST);
	    cerr << "MOU FAST" << endl;
	    return true;
	  }
	}
      }
      else if(Pos(s.pos + FAST_DOWN) == seguent) {
	if (within_window(Pos(s.pos - FAST_DOWN), round()) and (cell(s.pos-FAST_DOWN).type == STARSHIP or cell(s.pos-DOWN).type == STARSHIP or cell(s.pos-SLOW_DOWN).type == STARSHIP or cell(s.pos-FAST_DOWN).type == MISSILE or cell(s.pos-DOWN).type == MISSILE or cell(s.pos-SLOW_DOWN).type == MISSILE)) {
	  move(s.sid,SLOW);
	  return true;
	}
	else {
	  if(es_pot(s.pos,FAST_DOWN)) {
	    move(s.sid,FAST_DOWN);
	    cerr << "MOU FAST_DOWN" << endl;
	    return true;
	  }
	}
      }
    }
    return false;
  }

  
  void mou_nau_sense_bonus(Starship& s) { // Fa el moviment en el cas de que no es trobi BONUS a la pantalla.
    
    for(Dir d : all_dirs) {
      if (within_window(s.pos+d, round()+1)) {
	CType t = cell(Pos(s.pos+d)).type;
	if (t != ASTEROID and es_pot(s.pos,d) and esquivar(s.pos+d)) {
	  move(s.sid,d);
	}
      }
    }
  }
    
//     if((within_window(Pos(s.pos + FAST), round())) and ((cell(Pos(s.pos + FAST)).type == EMPTY) or (cell(Pos(s.pos + FAST)).type == POINT_BONUS) or (cell(Pos(s.pos + FAST)).type == MISSILE_BONUS)) and (es_pot(s.pos,FAST))) {
//       cerr << "MOU FAST" << endl;
//       move(s.sid,FAST);
//     }
//     else if((within_window(Pos(s.pos + FAST_UP), round())) and ((cell(Pos(s.pos + FAST_UP)).type == EMPTY) or (cell(Pos(s.pos + FAST_UP)).type == POINT_BONUS) or (cell(Pos(s.pos + FAST_UP)).type == MISSILE_BONUS)) and (es_pot(s.pos,FAST_UP))) {
//       cerr << "MOU FAST_UP" << endl; 
//       move(s.sid,FAST_UP);
//     }
//     else if((within_window(Pos(s.pos + FAST_DOWN), round())) and ((cell(Pos(s.pos + FAST_DOWN)).type == EMPTY) or (cell(Pos(s.pos + FAST_DOWN)).type == POINT_BONUS) or (cell(Pos(s.pos + FAST_DOWN)).type == MISSILE_BONUS)) and (es_pot(s.pos,FAST_DOWN))) {
//       cerr << "MOU FAST_DOWN" << endl;
//       move(s.sid,FAST_DOWN);
//     }
//     else if((within_window(Pos(s.pos + DEFAULT), round())) and ((cell(Pos(s.pos + DEFAULT)).type == EMPTY) or (cell(Pos(s.pos + DEFAULT)).type == POINT_BONUS) or (cell(Pos(s.pos + DEFAULT)).type == MISSILE_BONUS))) {
//       cerr << "MOU DEFAULT" << endl;
//       move(s.sid,DEFAULT);
//     }
//     else if((within_window(Pos(s.pos + UP), round())) and ((cell(Pos(s.pos + UP)).type == EMPTY) or (cell(Pos(s.pos + UP)).type == POINT_BONUS) or (cell(Pos(s.pos + UP)).type == MISSILE_BONUS)) and (es_pot(s.pos,UP))) {
//       cerr << "MOU UP" << endl;
//       move(s.sid,UP);
//     }
//     else if((within_window(Pos(s.pos + DOWN), round())) and ((cell(Pos(s.pos + DOWN)).type == EMPTY) or (cell(Pos(s.pos + DOWN)).type == POINT_BONUS) or (cell(Pos(s.pos + DOWN)).type == MISSILE_BONUS)) and (es_pot(s.pos,DOWN))) {
//       cerr << "MOU DOWN" << endl;
//       move(s.sid,DOWN);
//     }
//     else if((within_window(Pos(s.pos + SLOW_UP), round())) and ((cell(Pos(s.pos + SLOW_UP)).type == EMPTY) or (cell(Pos(s.pos + SLOW_UP)).type == POINT_BONUS) or (cell(Pos(s.pos + SLOW_UP)).type == MISSILE_BONUS))) {
//       cerr << "MOU SLOW_UP" << endl;
//       move(s.sid,SLOW_UP);
//     }
//     else if((within_window(Pos(s.pos + SLOW_DOWN), round())) and ((cell(Pos(s.pos + SLOW_DOWN)).type == EMPTY) or (cell(Pos(s.pos + SLOW_DOWN)).type == POINT_BONUS) or (cell(Pos(s.pos + SLOW_DOWN)).type == MISSILE_BONUS))) {
//       cerr << "MOU SLOW_DOWN" << endl;
//       move(s.sid,SLOW_DOWN);
//     }
//   }
  
  bool buscar_bonus(Pos& p, Starship& s) { // BFS que busca un BONUS (moneda o bonus de missil) i retorna el cami mÃƒÆ’Ã‚Â©s curt per arribar-hi.
    matriu_booleana visitats(number_rows(), vector<bool>(number_window_columns(), false));
    matriu_posicions pares(number_rows(), vector<Pos>(number_window_columns()));
    queue<Pos> q;
    q.push(s.pos); //posicio inicial de la nau, pasada com a parÃƒÆ’ metre.
    int f = first(s.pos);
    int c = second(s.pos);
    visitats[f][c%number_window_columns()] = true; //marques a true la poscio a on comenÃƒÆ’Ã‚Â§es.   
     
    while (not q.empty()) {
      Pos pact = q.front();
      q.pop();
      
      for(Dir d : all_dirs) {
	if (within_window(pact+d, round()+1)) {
	  CType t = cell(Pos(pact+d)).type;
	  Pos nova = Pos(pact + d);
	  int fila = first(nova);
	  int columna = second(nova);
	  if (t != ASTEROID and not visitats[fila][columna%number_window_columns()] and es_pot(pact,d)) {
	    q.push(nova);
	    pares[fila][columna%number_window_columns()] = pact;
	    visitats[fila][columna%number_window_columns()] = true;
	    if (t == POINT_BONUS) {
	      bool acabat = false;
	      while (not acabat) {
		int f = first(nova);
		int c = second(nova);
		if (pares[f][c%number_window_columns()] == s.pos) acabat = true;
		else {
		  nova = pares[f][c%number_window_columns()];
		}
	      }
	      p = nova;
	      return true;
	    }
	  }  
	}
      }
    }
    return false;
  }
     
  bool up(Dir d)   { return d == SLOW_UP   or d == UP   or d == FAST_UP;   }
  bool down(Dir d) { return d == SLOW_DOWN or d == DOWN or d == FAST_DOWN; }

  virtual void play () {

    if (round() == 0) {
      // Inicialitza el vector amb totes les posicions que pot visitar.
      // DEFAULT: endevant; UP: diagonal cap adalt; DOWN: diagonal cap abaix
      // SLOW: quiet; SLOW_UP: adalt; SLOW_DOWN: abaix
      // FAST: 2 endevant; FAST_UP: 2 en diagonal adalt; FAST_DOWN: 2 en diagonal abaix
      all_dirs = {FAST_UP,    FAST, FAST_DOWN,
		     DOWN,      UP,   DEFAULT,           
                  SLOW_UP,    SLOW, SLOW_DOWN}; // cambiar prioritat dels moviments, cambiant l'ordre del vector.  
    }

    // Per cada una de les meves naus
    for (Starship_Id sid = begin(me()); sid != end(me()); ++sid) {

      Starship s = starship(sid);
      if (not s.alive) cerr << "NAU NO ESTA VIVA! HA MORT!" << endl;
      if (s.alive and round() > 0) { // Si la meva nau estÃƒÆ’  viva
        cerr << "NAU VIVA!" << endl;
        Pos p = s.pos;
	
	if(within_window(Pos(p + DEFAULT), round()) and within_window(Pos(p + FAST), round())) { //DISPARA NAU
	  cerr << "mira de disparar" << endl;
	  Cell ahead = cell(Pos(p + DEFAULT)); 
	  Cell ahead2 = cell(Pos(p + FAST));
	  if (s.nb_miss > 0 and ahead.type == STARSHIP) {
	    Player_Id x = player_of(ahead.sid);
	    if (x != me()) shoot(sid);
	    cerr << "DISPARAA" << endl;
	  }
	  else if (s.nb_miss > 0 and ahead2.type == STARSHIP) {
	    Player_Id x = player_of(ahead2.sid);
	    if (x != me()) shoot(sid);
	    cerr << "DISPARAA" << endl;
	  }
	}
	
	if (buscar_bonus(p,s)) {
	  cerr << "Mou cap al bonus" << endl;
	  if (esquivar(p-s.pos)) move(s.sid,p-s.pos);
	  //bool mogut = mou_nau(s,p);
// 	  if (not mogut) {
// 	    cerr << "no s'ha pogut moure cap al bonus, mou cap on pot" << endl;
// 	    mou_nau_sense_bonus(s);
// 	  }
	}
	else {
	    cerr << "No ha trobat bonus, mou cap on pot" << endl;
	    mou_nau_sense_bonus(s);
	}
      }
    }
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);