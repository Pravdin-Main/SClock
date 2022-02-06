#ifndef sound_h
#define sound_h

#include <Arduino.h>
#include <QBPlay.h>
#include <GyverTimer.h>

const byte speaker_pin = A2;

// Тема из Star Wars
const char sound1[] PROGMEM = "MNT150L2O3CGP16L16FEDL2>C<GP16L16FEDL2>C<G\
P16L16FEFL2DP16L16<GGGL2>CGP32L16FEDL2>C<GP16L16FEDL2>C<GP16L16A+AA+L1GL2G\
.L8<G.L16GL4A.L8A>FEDCL16CDEDP16L8<AL4BL8G.L16GL4A.L8A>FEDCGP8L4D.P8L8<G.L1\
6GL4A.L8A>FEDCL16CDEDP16L8<AL4BP16L8>G.L16GL8>C.L16<A+L8G+.L16GL8F.L16D+L8D.\
L16CL1GL2G.P16L16GGGL8>CP8L16<CCCL2C.";
// const char sound1[] PROGMEM = "M";
// 
// const char sound2[] PROGMEM = "T75L64O2C+L16AEC+EAEL64G+L16BEL64AL16>C+<E\
// C+EL64AL16>C+<AL64BL16>D<AL64>C+L16E<AL64>C+L16E<AL64>DL16F+<AL64>EL16G+<A\
// L64>G+L16B<AL64>F+L16A<AL64>EL16G+<AL64>DL16F+<AL64>C+L16E<F+L64BL16>D<F+L6\
// 4EL16>C+<F+L64DL16BF+L64C+L16AE>C+<EL64DL16BEG+EL64C+L16AEC+EA>C+EC+L64<C+L8\
// A.P16L64C+L8A.P16L64C+L4AP8L8EL64C+L4AL64EL4BL64AL4>C+.L8C+L64<F+AL8>C+C+E.L1\
// 6DL64<DL4>C+L8<BBL64F+L4>DL64<EL4>C+L64<F+L4A.L8AL64DAL4BL8G+>C+L4<AP8L8EL64C+\
// EL4AL64EG+L4BL64AL4>C+.L8C+L64<F+AL8>C+C+L16C+L8E.L16DL64<DL4>C+L8<BBL64EL4>DL6\
// 4<DL4>C+<A.L8AL64DAL4BL8E>C+L4<AP4L64EAL4>EL64<BL8>DL64<AL8>C+L64<AL4>C+L64<G+L4\
// BL64F+L8AL64F+L8AL64EL8G+L64D+L8F+L64D+L4F+L64C+L4EL64C+L8EL64D+L8F+L64EL8G+L64F+\
// L8AL64EL8B>E<B.L16G+L4BL64D+L8AF+L64<AA+>D+L4F+L64<G+L4>EL64C+EL4>C+L64<EL8>C+<AL6\
// 4DL4G+F+L64C+EL4>C+L64<EL8>C+<AL64DL4G+F+L64F+L4>F+C+.L64<G+L8BL64AL8>C+L64<BL8>DL6\
// 4<AL4>C+L8<B>EL64<BL4>DL64<AL8>C+<EAL64C+L8EBL64EL8G+>C+L64<EL8AL64EL8A>C+L64<EAL8>C\
// +C+L64C+<EAL8>E.L16DL64<AL4>C+L64<AL8BL64AL8BL4>AAL8AEC+F+EC+<AF+L4EC+L64<G+L8B>D+D<B\
// L16A>EC+EAEBEL64AL16>C+<EC+E>C+<AL64BL16>D<AL64>C+L16E<AL64>C+L16E<AL64>DL16F+<AL64>EL\
// 16G+<AL64>G+L16B<AL64>F+L16A<AL64>EL16G+<AL64>DL16F+<AL64>C+L16E<F+L64BL16>D<F+L64EL16>\
// C+<F+L64DL16BF+L64C+L16AE>C+<EL64DL16BEG+EL64C+L16AEC+EA>C+EC+L64<A>EL8AP8L64<A>EL8AP8L6\
// 4<A>EL8A";
const char sound2[] PROGMEM = "T";
//
// const char sound3[] PROGMEM = "MNT150L4O1BL8EBL4>CL8<E>CL4C+L8<E>C+L4CL8\
// <E>CL4<BL8EBL4>CL8<E>CL4C+L8<E>C+L4CL8<E>C>EL16F+F+L8F+L4F+.L8<EEE>EL16GG\
// L8GL4G.L8<F+F+F+>EL16F+F+L8F+L4F+.L8<EEE>EL16GGL8GL4G.L8<F+F+F+>EL16F+F+L8\
// F+L4F+.L8<EEE>EL16GGL8GL4G.L8<F+F+EO4D+L64<BL8>DL2DL64O2GL8BL64F+L8AL32G.L1\
// B.L8EL16F+F+L8F+L4F+.L8>EEE<EL16GGL8GL4G.L8>F+F+F+<EL16F+F+L8F+L4F+.L8>EEE<E\
// L16GGL8GL4G.L8>F+F+F+<EL16F+F+L8F+L4F+.L8>EEE<EL16GGL8GL4G.L8>F+F+E>D+L64<BL8\
// >DL2DL64<GL8BL64F+L8AL32G.L1B.L4O1BL8EBL4>CL8<E>CL4C+L8<E>C+L4CL8<E>CL4<BL8EBL\
// 4>CL8<E>CL4C+L8<E>C+L4CL8<E>C>EL64EL4GL8>D+L64<GBL4>D.L8<GL64FL8A+L64GL2B.L8GL3\
// 2AGL4F+.L8<B>EC+L2C+L64<GBL8>F+.P16L64<GBL4>F+L8EL64EL4GL8>D+L64<GBL4>D.L8<GL64F\
// L8A+L64GL2B.L8GL32AGL4F+.L8<B>D+EL2EL64<GBL8>C+.P16L64<GBL4>C+L8EL64EL4GL8>D+L64<\
// GBL4>D.L8<GL64FL8A+L64GL2B.L8GL32AGL4F+.L8<B>EC+L2C+L64<GBL8>F+.P16L64<GBL4>F+L8EL\
// 64EL4GL8>D+L64<GBL4>D.L8<GL64FL8A+L64GL2B.L8GL32AGL4F+.L8<B>D+EL2EL64<GBL8>C+.P16L6\
// 4<GBL4>C+L64<GBL8>EL64<GBL4>EL64<GBL8>EL64<A>D+L8F+.L16EL64<A>D+L4F+L64<B>EL8GL64<B>\
// EL4GL64<B>EL8GL64<A>D+L8F+.L16EL64<A>D+L4F+L64<GBL8>EL64<GBL4>EL64<GBL8>EL64<A>D+L8F+\
// .L16EL64<A>D+L4F+L64<B>EL8GL64<B>EL4GL64<B>EL8GL64<A>D+L8F+.L16EL64<A>D+L4F+L64C+EGL8B\
// L64C+EGL8BP8L2O1BL8BL64O3C+EGL8BL64C+EGL8BP8L4O1B.BL64O3C+EGL16BL8BL16BL8BL64C+EGL8BL4O\
// 1BL8EBL4>CL8<E>CL4C+L8<E>C+L4CL8<E>C>EL16F+F+L8F+L4F+.L8EEEEL16GGL8GL4G.L8F+F+F+EL16F+F+\
// L8F+L4F+.L8EEEEL16GGL8GL4G.L8F+F+F+EL16F+F+L8F+L4F+.L8EEEEL16GGL8GL4G.L8F+F+E>D+L64<BL8>D\
// L2DL64GL8BL64F+L8AL32G.L1B.P16L8O2EL4GL8>D+L4DL8<GBB>F+L4FL8<BL4>DL8A+L4AL8FAL4AL8D+DDP8P4\
// L64O1EB>GB>C+L2F+.";
const char sound3[] PROGMEM = "M";

void play_sound(uint8_t);

#endif