 
#include "sfml-utn-inspt.h"

/*
 * Constantes generales de la ventana
 */
const unsigned int ANCHO_VENT = 800;
const unsigned int ALTO_VENT = 600;
const unsigned int FRAMERATE = 60;

// lee los eventos (clicks, posición del mouse, redimensión de la ventana, etc)
void leer_eventos(RenderWindow &window, RectangleShape &barrita);

// PARAMETROS DE LA BARRITA
const float VELOCIDAD = 3.f;
const float ANCHO_BARRITA = 75.f;
const float ALTO_BARRITA = 15.f;

// CANTIDAD DE BLOQUES
const int FILAS = 6;
const int COLUMNAS = 13;

const int CANTIDAD_DE_VIDAS = 3;

// COLORES DE LOS BLOQUES
const sf::Color colorArray[5] = {sf::Color::Cyan, sf::Color::Blue, sf::Color::Green, sf::Color::Red, sf::Color::Yellow};

// CANTIDAD DE POSICIONES QUE PUEDEN TOMAR LOS OJOS
const int TOTAL_TEXTURAS_OJOS = 5;

// Tipo de dato custom, borde de la ventana, para detectar coliciones
enum borde_t
{
  SUPERIOR,
  INFERIOR,
  IZQUIERDO,
  DERECHO
};

/**
 * @brief Detecta la colición de un rectangulo con la ventana
 *
 * @param r Rectangulo a evaluar si coliciona
 * @param borde Con qué borde (borde_t) tiene que contrastar la colisión
 * @return true si ha colisionado el rectangulo `r` con el `borde` especificado
 * @return false si no ha colicionado o se pasa un `borde` inválido.
 */

//  ---- CABECERAS DE LAS FUNCIONES ----

bool colision_con_ventana(CircleShape &r, borde_t borde);
bool colision_con_barrita(CircleShape &pelotita, RectangleShape &barrita);
RectangleShape crearBarrita();
CircleShape crearPelotita();
Font cargarFuente();
Text atributosTexto(String valor, Font fuente, Vector2f posiciones);
Texture cargarTextura(String ruta);

int main()
{
  srand(time(NULL));

  sf::Font fuente = cargarFuente();

  // ----- VARIABLES -----
  int cantVidas = 3;
  int controlRebotes = 0;
  Color obtenerColor;
  //----------------------

  // ------------------------ CREACIÓN DE TEXTOS -------------------------------------------------
  Vector2f posiciones = Vector2f((ANCHO_VENT / 2.f) - 100, ALTO_VENT / 2.f);

  /*sf::Text vidasTexto =  atributosTexto("VIDAS: ", fuente, posiciones);*/
  sf::Text vidasTexto;
  vidasTexto.setFont(fuente);                           // Establece la fuente
  vidasTexto.setString("VIDAS: ");                      // Establece el texto
  vidasTexto.setCharacterSize(24);                      // Establece el tamaño del texto
  vidasTexto.setFillColor(sf::Color::White);            // Establece el color del texto
  vidasTexto.setPosition({posiciones.x, posiciones.y}); // Establece la posición

  sf::Text cantVidasTexto[4];

  posiciones.x = (ANCHO_VENT / 2.f);
  posiciones.y = (ALTO_VENT / 2.f);

  for (int i = 0; i < 4; i++)
  {
    /*cantVidasTexto[i] = atributosTexto(std::to_string(i+1), fuente, posiciones);*/
    cantVidasTexto[i].setFont(fuente);                           // Establece la fuente
    cantVidasTexto[i].setString(std::to_string(i));              // Establece el texto
    cantVidasTexto[i].setCharacterSize(24);                      // Establece el tamaño del texto
    cantVidasTexto[i].setFillColor(sf::Color::White);            // Establece el color del texto
    cantVidasTexto[i].setPosition({posiciones.x, posiciones.y}); // Establece la posición
  }
  //-------------------------------------------------------------------------------------------------

  // ---------------------- CREACIÓN DE ELEMENTOS ---------------------------

  RenderWindow ventana = RenderWindow({ANCHO_VENT, ALTO_VENT}, "Proyecto");
  ventana.setFramerateLimit(FRAMERATE);

  RectangleShape barrita = crearBarrita();

  CircleShape pelotita = crearPelotita();

  Vector2 diff = {VELOCIDAD, VELOCIDAD};

  sf::RectangleShape bloques[FILAS][COLUMNAS];

  for (int i = 0; i < FILAS; ++i)
  {
    for (int j = 0; j < COLUMNAS; ++j)
    {
      bloques[i][j].setSize(sf::Vector2f(50, 20));         // Tamaño del bloque
      bloques[i][j].setPosition(j * 60 + 15, i * 30 + 15); // Espaciado
      bloques[i][j].setFillColor(sf::Color::White); //PARA USAR EN LINUX - Color del bloque
      //bloques[i][j].setFillColor(colorArray[rand() % 5]);  // PARA USAR EN WINDOWS - Color del bloque
    }
  }

  //----------------------------------------------------------------------------

  // ------------------------------ TEXTURAS -----------------------------------------

  //Texturas de ojos que giran, para la pelotita
  sf::Texture texturasOjos[5];

  texturasOjos[0] = cargarTextura("../recursos/kenney_googly-eyes/PNG/googly-a.png");
  texturasOjos[1] = cargarTextura("../recursos/kenney_googly-eyes/PNG/googly-b.png");
  texturasOjos[2] = cargarTextura("../recursos/kenney_googly-eyes/PNG/googly-c.png");
  texturasOjos[3] = cargarTextura("../recursos/kenney_googly-eyes/PNG/googly-d.png");
  texturasOjos[4] = cargarTextura("../recursos/kenney_googly-eyes/PNG/googly-e.png");

  //Textura del fondo
  sf::Texture texturaFondo;

  texturaFondo = cargarTextura("../recursos/fondo.jpg");

  sf::Sprite fondo(texturaFondo);

  //---------------------------------------------------------------------------------------

  // ----------------------- SONIDOS --------------------------------

  sf::SoundBuffer bufferDerrota;
      if (!bufferDerrota.loadFromFile("../recursos/Sonidos/SuperMarioDerrota.wav"))
      {
        std::cerr << "Error al cargar el buffer" << std::endl;
      }

      sf::Sound sonidoDerrota;
      sonidoDerrota.setBuffer(bufferDerrota);
      sonidoDerrota.setVolume(100.0);

    sf::SoundBuffer bufferRebote;
      if (!bufferRebote.loadFromFile("../recursos/Sonidos/SonidoReboteDePelota.wav"))
      {
        std::cerr << "Error al cargar el buffer" << std::endl;
      }

      sf::Sound sonidoRebote;
      sonidoRebote.setBuffer(bufferRebote);
      sonidoRebote.setVolume(100.0);

  //-----------------------------------------------------------------

  int numeroDeTexturaAMostrar = 0;
  int contadorParaCambioDeTexturas  = 0;
  int cantVecesSonidoDerrota = 0;
  int contadorDeBloquesEliminados = 0;

  while (ventana.isOpen())
  {
    pelotita.setTexture(&texturasOjos[numeroDeTexturaAMostrar]);

    contadorParaCambioDeTexturas = contadorParaCambioDeTexturas + 1;
    if (contadorParaCambioDeTexturas == 60)
    {
      contadorParaCambioDeTexturas = 0;
      numeroDeTexturaAMostrar = numeroDeTexturaAMostrar + 1;
    }

    if (numeroDeTexturaAMostrar >= TOTAL_TEXTURAS_OJOS)
    {
      numeroDeTexturaAMostrar = 0; // Reinicia a la primera textura
    }

    if (cantVidas > 0) //&& contadorDeBloquesEliminados != (FILAS * COLUMNAS))
    {
      leer_eventos(ventana, barrita);

      if (colision_con_ventana(pelotita, IZQUIERDO))
      {
        diff.x = VELOCIDAD;
      }
      else if (colision_con_ventana(pelotita, DERECHO))
      {
        diff.x = -VELOCIDAD;
      }
      else if (colision_con_ventana(pelotita, SUPERIOR))
      {
        diff.y = VELOCIDAD;
      }
      else if (colision_con_ventana(pelotita, INFERIOR))
      {
      pelotita.setPosition({(ANCHO_VENT / 2.f), ALTO_VENT / 2.f});
      ventana.draw(pelotita);

        cantVidas = cantVidas - 1;
        diff.y = -VELOCIDAD;
        if (cantVidas != 0)
        {
          Event evento = Event();
          while ((evento.key.code == Keyboard::E))
          {
              if (evento.key.code == Keyboard::E)
              {
                diff.y = -VELOCIDAD;
              }
          }
        }
      }

      if (colision_con_barrita(pelotita, barrita) && (controlRebotes % 2 == 0))
      {
        diff.x = +VELOCIDAD;
        diff.y = -VELOCIDAD;
        controlRebotes = controlRebotes + 1;
      }
      else if (colision_con_barrita(pelotita, barrita) && (controlRebotes % 2 != 0))
      {
        diff.x = -VELOCIDAD;
        diff.y = -VELOCIDAD;
        controlRebotes = controlRebotes - 1;
      }

      for (int i = 0; i < FILAS; ++i)
      {
        for (int j = 0; j < COLUMNAS; ++j)
        {
          if (bloques[i][j].getSize().x > 0)
          { // Solo verificamos bloques que existan
            // Comprobar si la pelotita ha tocado algún bloque
            if (pelotita.getGlobalBounds().intersects(bloques[i][j].getGlobalBounds()))
            {
              sonidoRebote.play();
              // Invertir la dirección de la pelota
              diff.y = -diff.y;
              obtenerColor = bloques[i][j].getFillColor();
              pelotita.setFillColor(obtenerColor);
              //   "Eliminar" el bloque (ponemos su tamaño a cero)
              bloques[i][j].setSize(sf::Vector2f(0, 0));

              contadorDeBloquesEliminados = contadorDeBloquesEliminados + 1;
            }
          }
        }
      }
      pelotita.move(diff);

      ventana.clear();

      ventana.draw(fondo);
      ventana.draw(barrita);
      ventana.draw(pelotita);
      ventana.draw(vidasTexto);

      for (int i = 0; i < FILAS; ++i)
      {
        for (int j = 0; j < COLUMNAS; ++j)
        {
          if (bloques[i][j].getSize().x > 0)
          { // Solo dibujamos bloques que no han sido "eliminados"
            ventana.draw(bloques[i][j]);
          }
        }
      }

      if (cantVidas >= 0)
      {
        ventana.draw(cantVidasTexto[cantVidas]);
      }

    }
    else if (cantVidas < 1)
    {
      /*sf::Clock clock;

      // Tiempo en segundos para cerrar la ventana
      float closeTime = 3.0f;

      // Verificar si han pasado 5 segundos
      if (clock.getElapsedTime().asSeconds() >= closeTime)
      {
        ventana.close(); // Cerrar la ventana
      }*/

      Event event = Event();

      while (ventana.pollEvent(event))
      {
        if (event.type == Event::Closed)
        {
          ventana.close();
        }
      }
      ventana.clear(sf::Color::Black);
      sf::Text textoDerrota;
      textoDerrota.setFont(fuente);                                                  // Establece la fuente
      textoDerrota.setString("\tPERDISTE :(\nCIERRA LA VENTANA\nGRACIAS POR JUGAR"); // Establece el texto
      textoDerrota.setCharacterSize(50);                                             // Establece el tamaño del texto
      textoDerrota.setFillColor(sf::Color::Red);                                     // Establece el color del texto
      textoDerrota.setPosition({150, 180});                                          // Establece la posición

      ventana.draw(textoDerrota);

      if (cantVecesSonidoDerrota < 1)
      {
        sonidoDerrota.play();
        cantVecesSonidoDerrota = cantVecesSonidoDerrota + 1;
      }

    }
    else{
      Event event = Event();

      while (ventana.pollEvent(event))
      {
        if (event.type == Event::Closed)
        {
          ventana.close();
        }
      }
      ventana.clear(sf::Color::Black);
      sf::Text textoVictoria;
      textoVictoria.setFont(fuente);                                                  // Establece la fuente
      textoVictoria.setString("\tGANASTE! :)\nFELICITACIONES!\nCIERRA LA VENTANA\nGRACIAS POR JUGAR"); // Establece el texto
      textoVictoria.setCharacterSize(50);                                             // Establece el tamaño del texto
      textoVictoria.setFillColor(sf::Color::Green);                                     // Establece el color del texto
      textoVictoria.setPosition({150, 180});                                          // Establece la posición

      ventana.draw(textoVictoria);
      }


    ventana.display();
  }
}

void leer_eventos(RenderWindow &window, RectangleShape &barrita)
{
  Vector2f posicion;
  Event event = Event();
  while (window.pollEvent(event))
  {
    if (event.type == Event::Closed)
    {
      window.close();
    }
    if (event.type == Event::KeyPressed)
    {
      if (event.key.code == Keyboard::Right)
      {
        if (barrita.getPosition().x <= ANCHO_VENT)
        {
          posicion = barrita.getPosition();
          barrita.setPosition({posicion.x + 15.f, posicion.y});
        }
      }

      if (event.key.code == Keyboard::Left)
      {
        if (barrita.getPosition().x > ANCHO_BARRITA)
        {
          posicion = barrita.getPosition();
          barrita.setPosition({posicion.x - 15.f, posicion.y});
        }
      }
    }
  }
}

bool colision_con_ventana(CircleShape &r, borde_t borde)
{
  switch (borde)
  {
  case SUPERIOR:
    return r.getGlobalBounds().top <= 0;
  case INFERIOR:
    return (r.getGlobalBounds().top + r.getGlobalBounds().height) >= ALTO_VENT;
  case IZQUIERDO:
    return r.getGlobalBounds().left <= 0;
  case DERECHO:
    return (r.getGlobalBounds().left + r.getGlobalBounds().width) >= ANCHO_VENT;
  }
  return false;
}

bool colision_con_barrita(CircleShape &pelotita, RectangleShape &barrita)
{
  return pelotita.getGlobalBounds().intersects(barrita.getGlobalBounds());
}

RectangleShape crearBarrita()
{
  RectangleShape barrita = RectangleShape({ANCHO_BARRITA, ALTO_BARRITA});
  barrita.setFillColor(Color::Red);
  barrita.setOrigin({75.f, 75.f});
  barrita.setPosition({(ANCHO_VENT / 2.f) + 10, ALTO_VENT - 20});

  return barrita;
}

CircleShape crearPelotita()
{
  CircleShape pelotita = CircleShape(15);
  pelotita.setFillColor(Color::Green);
  pelotita.setOrigin({75.f, 75.f});
  pelotita.setPosition({(ANCHO_VENT / 2.f), ALTO_VENT / 2.f});

  return pelotita;
}

/*Text atributosTexto(String valor, Font fuente, Vector2f posiciones)
{
  Text texto;
  texto.setFont(fuente);                                    // Establece la fuente
  texto.setString(valor);                                   // Establece el texto
  texto.setCharacterSize(24);                               // Establece el tamaño del texto
  texto.setFillColor(sf::Color::White);                     // Establece el color del texto
  texto.setPosition({posiciones.x, posiciones.y});          // Establece la posición

  return texto;
}*/

Font cargarFuente()
{
  Font fuente;

  if (!fuente.loadFromFile("../recursos/Fuentes/DRAGON HUNTER.otf"))
  {
    std::cerr << "Error al cargar la fuente" << std::endl;
  }

  return fuente;
}

Texture cargarTextura(String ruta)
{
  Texture textura;

  if (!textura.loadFromFile(ruta))
  {
    std::cerr << "Error al cargar la fuente" << std::endl;
  }

  return textura;
}
