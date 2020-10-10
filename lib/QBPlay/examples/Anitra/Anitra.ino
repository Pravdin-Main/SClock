/*
 * Anitra.ino
 * Файл с примером использования библиотеки QBPlay
 * https://en.wikibooks.org/wiki/QBasic/Appendix#PLAY
 *
 * created 10.11.2017
 * modified 12.11.2017
 * with Arduino 1.8.3 (tested on Arduino Uno)
 *
 * Copyright 2017 Vitaliy Fust <aslok.zp@gmail.com>
 *
 * This work is licensed under the MIT License (MIT). To view a copy of this
 * license, visit http://opensource.org/licenses/MIT or send a letter to:
 * Open Source Initiative
 * 855 El Camino Real
 * Ste 13A, #270
 * Palo Alto, CA 94301
 * United States.
 *
 *
 */

#include "QBPlay.h"
QBPlay* play;
const byte speaker_pin = A3;

const char Anitra[] PROGMEM = "MNT150L2O5E.L4EP2O1AO3CL8DDL4O1AO3EL8FFL4O1AO3EL8DDL4O1AO3CEL64ABL16A.L8G+AB>CDEAL4EL16DL32EDEDEDL8CAL4CL16<BL32>C<B>C<B>C<BL2AP4L8A>CL4<AL16GL32AGAGAGL8F+>CL2<F+L8F>CL4<FL16EL32FEFEFEL8D+BL2D+L8BA+F+BAFBG+EBGD+BF+DBFC+BECBD+<BL64>EF+L16E.L8D+EF+GAB>E<BAGF+EF+GAL4B>EP4<EL64ABL16A.L8G+AB>CDEAL4EL16DL32EDEDEDL8CAL4CL16<BL32>C<B>C<B>C<BL2AP4L8A>CL4<AL16GL32AGAGAGL8F+>CL2<F+L8F>CL4<FL16EL32FEFEFEL8D+BL2D+L8BA+F+BAFBG+EBGD+BF+DBFC+BECBD+<BL64>EF+L16E.L8D+EF+GAB>E<BAGF+EF+GAL4B>EP4P4F.L8EEDL4D.L8EEFL2F.L4EP2P8L8<FDC<B>FDC<B>D<BAG+>D<BAG+BG+FEDC<BL4O4A+.L8AAGL4G.L8AAA+L2A+.L4AP2P8L8<A+GFEA+GFEGEDC+GEDC+EC+<A+AGFEL4D>F+G<D>AB<D>AAL64>DEL16D.L8C+DEF+GA>DL4<AL16GL32AGAGAGL8F+>DL4<F+L16EL32FEFEFEL2D.L4O2D>FG<D>AA+<D>AAL64>DEL16D.L8C+DEFGA>DL4<AL16GL32AGAGAGL8F>DL4<FL16EL32FEFEFEL2D.L8O1C+>D+A>CF>CL64FGL16F.L8EFGAA+>CFCO3GAA+>CFL2CL8O1F>F+A>CE>EL64ABL16A.L8G+AB>CDEAEO3B>CDEAL2EL8O1B>B>D+F+ABL64>F+GL16F+.L8FF+GAB>C<CF+<G+AB>C<CL2F+L8O1E>EG+B>DEL64B>CL16<B.L8A+B>CDEF<FBCDEF<FL4B>EL64ABL16A.L8G+AB>CDEAL4EL16DL32EDEDEDL8CEL4CL16<BL32>C<B>C<B>C<BL2A.L8>A>CL4<AL16GL32AGAGAGL8F+>CL2<F+L8F>CL4<FL16EL32FEFEFEL8D+BL2D+L8DFL4DL16CL32DCDCDCL8<B>FL2<BL8A+>FL4<A+L16AL32BABABAL8G+>EL2<G+L8>ED+<B>ED<A+>EC+<A>EC<G+>E<BG>E<A+F+>E<AF>E<G+EL64ABL16A.L8G+AB>CDEAEDC<BAB>CDL4EAP4P4F.L8EEDL4D.L8EEFL2F.L4EP2P8L8<FDC<B>FDC<B>D<BAG+>D<BAG+BG+FEDC<BL4O4A+.L8AAGL4G.L8AAA+L2A+.L4AP2P8L8<A+GFEA+GFEGEDC+GEDC+EC+<A+AGFEL4D>F+G<D>AB<D>AAL64>DEL16D.L8C+DEF+GA>DL4<AL16GL32AGAGAGL8F+>DL4<F+L16EL32FEFEFEL2D.L4O2D>FG<D>AA+<D>AAL64>DEL16D.L8C+DEFGA>DL4<AL16GL32AGAGAGL8F>DL4<FL16EL32FEFEFEL2D.L8O1C+>D+A>CF>CL64FGL16F.L8EFGAA+>CFCO3GAA+>CFL2CL8O1F>F+A>CE>EL64ABL16A.L8G+AB>CDEAEO3B>CDEAL2EL8O1B>B>D+F+ABL64>F+GL16F+.L8FF+GAB>C<CF+<G+AB>C<CL2F+L8O1E>EG+B>DEL64B>CL16<B.L8A+B>CDEF<FBCDEF<FL4B>EL64ABL16A.L8G+AB>CDEAL4EL16DL32EDEDEDL8CEL4CL16<BL32>C<B>C<B>C<BL2A.L8>A>CL4<AL16GL32AGAGAGL8F+>CL2<F+L8F>CL4<FL16EL32FEFEFEL8D+BL2D+L8DFL4DL16CL32DCDCDCL8<B>FL2<BL8A+>FL4<A+L16AL32BABABAL8G+>EL2<G+L8>ED+<B>ED<A+>EC+<A>EC<G+>E<BG>E<A+F+>E<AF>E<G+EL64ABL16A.L8G+AB>CDEAEDC<BAB>CDL4EAP4O1EAP2L2O4A.L4A";

void setup(){
    play = new QBPlay(speaker_pin);
    play->start((__FlashStringHelper*) Anitra);
}

void loop(){
    play->touch();
}
